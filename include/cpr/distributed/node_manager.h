#ifndef CPR_DISTRIBUTED_NODE_MANAGER_H
#define CPR_DISTRIBUTED_NODE_MANAGER_H

#include <cpr/cpr.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

namespace cpr {
namespace distributed {

// Node status enum
enum class NodeStatus {
    IDLE,
    RUNNING,
    OVERLOADED,
    DOWN,
    DISCONNECTED
};

// Node resource information
struct NodeResources {
    double cpu_usage; // CPU usage percentage (0-100)
    double memory_usage; // Memory usage percentage (0-100)
    double network_in; // Network incoming bandwidth (MB/s)
    double network_out; // Network outgoing bandwidth (MB/s)
    int active_tasks; // Number of active tasks
    int max_tasks; // Maximum number of tasks the node can handle
    std::chrono::time_point<std::chrono::steady_clock> last_update;
};

// Worker node information
struct WorkerNode {
    std::string node_id;
    std::string address;
    int port;
    NodeStatus status;
    NodeResources resources;
    std::chrono::time_point<std::chrono::steady_clock> last_heartbeat;
    std::unordered_map<std::string, std::string> capabilities; // Supported features
};

// Node manager interface
class NodeManager {
public:
    virtual ~NodeManager() = default;
    virtual bool RegisterNode(const std::string& node_id, const std::string& address, int port) = 0;
    virtual bool UnregisterNode(const std::string& node_id) = 0;
    virtual bool UpdateNodeHeartbeat(const std::string& node_id) = 0;
    virtual bool UpdateNodeResources(const std::string& node_id, const NodeResources& resources) = 0;
    virtual NodeStatus GetNodeStatus(const std::string& node_id) = 0;
    virtual std::shared_ptr<WorkerNode> GetNode(const std::string& node_id) = 0;
    virtual std::vector<std::shared_ptr<WorkerNode>> GetAvailableNodes() = 0;
    virtual std::vector<std::shared_ptr<WorkerNode>> GetNodesByStatus(NodeStatus status) = 0;
    virtual void StartHeartbeatMonitor() = 0;
    virtual void StopHeartbeatMonitor() = 0;
    virtual void SetNodeTimeout(std::chrono::milliseconds timeout) = 0;
    virtual void SetOverloadThreshold(double cpu_threshold, double memory_threshold, int active_tasks_threshold) = 0;
};

// Load balancing strategy enum
enum class LoadBalancingStrategy {
    ROUND_ROBIN,
    LEAST_CONNECTIONS,
    LEAST_LOAD,
    RESPONSE_TIME,
    WEIGHTED_ROUND_ROBIN,
    CUSTOM
};

// Load balancer interface
class LoadBalancer {
public:
    virtual ~LoadBalancer() = default;
    virtual std::shared_ptr<WorkerNode> SelectNode(const TaskMetadata& task) = 0;
    virtual void SetStrategy(LoadBalancingStrategy strategy) = 0;
    virtual void SetCustomSelector(std::function<std::shared_ptr<WorkerNode>(const TaskMetadata&)> selector) = 0;
    virtual void UpdateNodeStats(const std::string& node_id, double response_time, bool success) = 0;
    virtual double GetNodeWeight(const std::string& node_id) = 0;
};

} // namespace distributed
} // namespace cpr

#endif // CPR_DISTRIBUTED_NODE_MANAGER_H