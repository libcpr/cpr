#include <cpr/distributed/node_manager.h>
#include <algorithm>
#include <thread>
#include <chrono>

namespace cpr {
namespace distributed {

class DefaultNodeManager : public NodeManager {
public:
    DefaultNodeManager() : heartbeat_monitor_running_(false), 
                          node_timeout_(std::chrono::seconds(30)),
                          cpu_overload_threshold_(80.0),
                          memory_overload_threshold_(80.0),
                          active_tasks_overload_threshold_(100) {
    }

    ~DefaultNodeManager() {
        StopHeartbeatMonitor();
    }

    bool RegisterNode(const std::string& node_id, const std::string& address, int port) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (nodes_.find(node_id) != nodes_.end()) {
            return false; // Node already registered
        }

        auto node = std::make_shared<WorkerNode>();
        node->node_id = node_id;
        node->address = address;
        node->port = port;
        node->status = NodeStatus::IDLE;
        node->last_heartbeat = std::chrono::steady_clock::now();
        node->resources = NodeResources{0.0, 0.0, 0.0, 0.0, 0, 100, std::chrono::steady_clock::now()};

        nodes_[node_id] = node;
        return true;
    }

    bool UnregisterNode(const std::string& node_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return nodes_.erase(node_id) > 0;
    }

    bool UpdateNodeHeartbeat(const std::string& node_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = nodes_.find(node_id);
        if (it == nodes_.end()) {
            return false;
        }
        it->second->last_heartbeat = std::chrono::steady_clock::now();
        if (it->second->status == NodeStatus::DOWN || it->second->status == NodeStatus::DISCONNECTED) {
            it->second->status = NodeStatus::IDLE;
        }
        return true;
    }

    bool UpdateNodeResources(const std::string& node_id, const NodeResources& resources) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = nodes_.find(node_id);
        if (it == nodes_.end()) {
            return false;
        }
        it->second->resources = resources;
        it->second->resources.last_update = std::chrono::steady_clock::now();

        // Update node status based on resources
        bool overloaded = (resources.cpu_usage > cpu_overload_threshold_) ||
                          (resources.memory_usage > memory_overload_threshold_) ||
                          (resources.active_tasks > active_tasks_overload_threshold_);

        if (overloaded) {
            it->second->status = NodeStatus::OVERLOADED;
        } else if (it->second->status == NodeStatus::OVERLOADED) {
            it->second->status = NodeStatus::IDLE;
        }
        return true;
    }

    NodeStatus GetNodeStatus(const std::string& node_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = nodes_.find(node_id);
        if (it == nodes_.end()) {
            return NodeStatus::DOWN;
        }
        return it->second->status;
    }

    std::shared_ptr<WorkerNode> GetNode(const std::string& node_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = nodes_.find(node_id);
        if (it == nodes_.end()) {
            return nullptr;
        }
        return it->second;
    }

    std::vector<std::shared_ptr<WorkerNode>> GetAvailableNodes() override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<WorkerNode>> available_nodes;

        for (const auto& node_pair : nodes_) {
            if (node_pair.second->status == NodeStatus::IDLE || node_pair.second->status == NodeStatus::RUNNING) {
                available_nodes.push_back(node_pair.second);
            }
        }
        return available_nodes;
    }

    std::vector<std::shared_ptr<WorkerNode>> GetNodesByStatus(NodeStatus status) override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<WorkerNode>> result;

        for (const auto& node_pair : nodes_) {
            if (node_pair.second->status == status) {
                result.push_back(node_pair.second);
            }
        }
        return result;
    }

    void StartHeartbeatMonitor() override {
        {   
            std::lock_guard<std::mutex> lock(mutex_);
            if (heartbeat_monitor_running_) {
                return;
            }
            heartbeat_monitor_running_ = true;
        }

        heartbeat_thread_ = std::thread([this]() {
            while (true) {
                {   
                    std::lock_guard<std::mutex> lock(mutex_);
                    if (!heartbeat_monitor_running_) {
                        break;
                    }
                }

                CheckNodeHeartbeats();
                std::this_thread::sleep_for(std::chrono::seconds(5)); // Check every 5 seconds
            }
        });
        heartbeat_thread_.detach();
    }

    void StopHeartbeatMonitor() override {
        std::lock_guard<std::mutex> lock(mutex_);
        heartbeat_monitor_running_ = false;
        // Thread will exit on its own
    }

    void SetNodeTimeout(std::chrono::milliseconds timeout) override {
        std::lock_guard<std::mutex> lock(mutex_);
        node_timeout_ = timeout;
    }

    void SetOverloadThreshold(double cpu_threshold, double memory_threshold, int active_tasks_threshold) override {
        std::lock_guard<std::mutex> lock(mutex_);
        cpu_overload_threshold_ = cpu_threshold;
        memory_overload_threshold_ = memory_threshold;
        active_tasks_overload_threshold_ = active_tasks_threshold;
    }

private:
    void CheckNodeHeartbeats() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();

        for (auto& node_pair : nodes_) {
            auto& node = node_pair.second;
            auto time_since_last_heartbeat = now - node->last_heartbeat;

            if (time_since_last_heartbeat > node_timeout_) {
                node->status = NodeStatus::DOWN;
            }
        }
    }

    std::unordered_map<std::string, std::shared_ptr<WorkerNode>> nodes_;
    bool heartbeat_monitor_running_;
    std::thread heartbeat_thread_;
    std::chrono::milliseconds node_timeout_;
    double cpu_overload_threshold_;
    double memory_overload_threshold_;
    int active_tasks_overload_threshold_;
    mutable std::mutex mutex_;
};

// Load Balancer implementations
class DefaultLoadBalancer : public LoadBalancer {
public:
    DefaultLoadBalancer() : strategy_(LoadBalancingStrategy::ROUND_ROBIN), round_robin_index_(0) {
    }

    std::shared_ptr<WorkerNode> SelectNode(const TaskMetadata& task) override {
        auto available_nodes = GetAvailableNodes();
        if (available_nodes.empty()) {
            return nullptr;
        }

        switch (strategy_) {
            case LoadBalancingStrategy::ROUND_ROBIN:
                return RoundRobinSelect(available_nodes);
            case LoadBalancingStrategy::LEAST_CONNECTIONS:
                return LeastConnectionsSelect(available_nodes);
            case LoadBalancingStrategy::LEAST_LOAD:
                return LeastLoadSelect(available_nodes);
            case LoadBalancingStrategy::CUSTOM:
                if (custom_selector_) {
                    return custom_selector_(task);
                }
                // Fall back to round-robin if custom selector is not set
                return RoundRobinSelect(available_nodes);
            default:
                return RoundRobinSelect(available_nodes);
        }
    }

    void SetStrategy(LoadBalancingStrategy strategy) override {
        strategy_ = strategy;
    }

    void SetCustomSelector(std::function<std::shared_ptr<WorkerNode>(const TaskMetadata&)> selector) override {
        custom_selector_ = selector;
    }

    void UpdateNodeStats(const std::string& node_id, double response_time, bool success) override {
        // Implementation for response time based selection
        std::lock_guard<std::mutex> lock(stats_mutex_);
        node_stats_[node_id].response_time = response_time;
        node_stats_[node_id].last_update = std::chrono::steady_clock::now();
    }

    double GetNodeWeight(const std::string& node_id) override {
        // Implementation for weighted strategies
        std::lock_guard<std::mutex> lock(stats_mutex_);
        auto it = node_stats_.find(node_id);
        if (it == node_stats_.end()) {
            return 1.0;
        }
        // Simple weight calculation: inverse of response time
        return 1.0 / (it->second.response_time + 0.0001); // Avoid division by zero
    }

private:
    struct NodeStats {
        double response_time{0.0};
        std::chrono::time_point<std::chrono::steady_clock> last_update;
    };

    std::vector<std::shared_ptr<WorkerNode>> GetAvailableNodes() {
        // In a real implementation, this would get nodes from NodeManager
        // For simplicity, we'll just return an empty vector
        // In practice, NodeManager should be injected into LoadBalancer
        return {};
    }

    std::shared_ptr<WorkerNode> RoundRobinSelect(const std::vector<std::shared_ptr<WorkerNode>>& nodes) {
        std::lock_guard<std::mutex> lock(round_robin_mutex_);
        if (round_robin_index_ >= nodes.size()) {
            round_robin_index_ = 0;
        }
        return nodes[round_robin_index_++];
    }

    std::shared_ptr<WorkerNode> LeastConnectionsSelect(const std::vector<std::shared_ptr<WorkerNode>>& nodes) {
        auto best_node = nodes[0];
        int min_connections = best_node->resources.active_tasks;

        for (size_t i = 1; i < nodes.size(); ++i) {
            int current_connections = nodes[i]->resources.active_tasks;
            if (current_connections < min_connections) {
                best_node = nodes[i];
                min_connections = current_connections;
            }
        }
        return best_node;
    }

    std::shared_ptr<WorkerNode> LeastLoadSelect(const std::vector<std::shared_ptr<WorkerNode>>& nodes) {
        auto best_node = nodes[0];
        double min_load = best_node->resources.cpu_usage + best_node->resources.memory_usage;

        for (size_t i = 1; i < nodes.size(); ++i) {
            double current_load = nodes[i]->resources.cpu_usage + nodes[i]->resources.memory_usage;
            if (current_load < min_load) {
                best_node = nodes[i];
                min_load = current_load;
            }
        }
        return best_node;
    }

    LoadBalancingStrategy strategy_;
    int round_robin_index_;
    std::function<std::shared_ptr<WorkerNode>(const TaskMetadata&)> custom_selector_;
    std::unordered_map<std::string, NodeStats> node_stats_;
    mutable std::mutex round_robin_mutex_;
    mutable std::mutex stats_mutex_;
};

} // namespace distributed
} // namespace cpr