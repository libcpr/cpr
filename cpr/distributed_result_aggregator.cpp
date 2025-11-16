#include <cpr/distributed/result_aggregator.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <uuid/uuid.h> // Note: Requires libuuid

namespace cpr {
namespace distributed {

class DefaultResultAggregator : public ResultAggregator {
public:
    DefaultResultAggregator() : result_timeout_(std::chrono::hours(24)) {
    }

    bool AddResult(const TaskResult& result) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        
        // Check for duplicate task result
        auto it = task_results_.find(result.task_id);
        if (it != task_results_.end()) {
            return false; // Duplicate result
        }

        // Add to task results map
        task_results_[result.task_id] = result;

        // Add to graph results map
        graph_results_[result.graph_id].push_back(result);

        // Add to node results map  
        node_results_[result.node_id].push_back(result);

        // Notify callback if set
        if (result_callback_) {
            result_callback_(result);
        }

        return true;
    }

    bool AddResults(const std::vector<TaskResult>& results) override {
        bool all_added = true;
        for (const auto& result : results) {
            if (!AddResult(result)) {
                all_added = false;
            }
        }
        return all_added;
    }

    std::vector<TaskResult> GetResultsByGraphId(const std::string& graph_id) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        auto it = graph_results_.find(graph_id);
        if (it == graph_results_.end()) {
            return {};
        }
        return it->second;
    }

    std::vector<TaskResult> GetResultsByNodeId(const std::string& node_id) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        auto it = node_results_.find(node_id);
        if (it == node_results_.end()) {
            return {};
        }
        return it->second;
    }

    TaskResult GetResultByTaskId(const std::string& task_id) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        auto it = task_results_.find(task_id);
        if (it == task_results_.end()) {
            // Return an empty TaskResult if not found
            return TaskResult{};
        }
        return it->second;
    }

    ExecutionReport GenerateReport(const std::string& graph_id) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        auto it = graph_results_.find(graph_id);
        if (it == graph_results_.end()) {
            // Return an empty report if no results found
            return ExecutionReport{};
        }

        const std::vector<TaskResult>& results = it->second;
        
        ExecutionReport report;
        report.report_id = GenerateUUID();
        report.graph_id = graph_id;
        report.total_tasks = static_cast<int>(results.size());
        report.successful_tasks = 0;
        report.failed_tasks = 0;
        report.retried_tasks = 0;
        report.task_results = results;

        // Calculate durations and status counts
        if (!results.empty()) {
            report.start_time = results[0].start_time;
            report.end_time = results[0].end_time;
            
            for (const auto& result : results) {
                // Update total duration
                if (result.start_time < report.start_time) {
                    report.start_time = result.start_time;
                }
                if (result.end_time > report.end_time) {
                    report.end_time = result.end_time;
                }

                // Count status
                if (result.status == TaskStatus::SUCCESS) {
                    report.successful_tasks++;
                } else if (result.status == TaskStatus::FAILURE) {
                    report.failed_tasks++;
                } else if (result.status == TaskStatus::RETRY) {
                    report.retried_tasks++;
                }
            }

            report.total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                report.end_time - report.start_time
            );
        }

        return report;
    }

    void ClearResults(const std::string& graph_id) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        auto it = graph_results_.find(graph_id);
        if (it == graph_results_.end()) {
            return;
        }

        // Remove from task_results_ and node_results_
        for (const auto& result : it->second) {
            task_results_.erase(result.task_id);
            
            auto node_it = node_results_.find(result.node_id);
            if (node_it != node_results_.end()) {
                auto& node_result_list = node_it->second;
                node_result_list.erase(
                    std::remove_if(node_result_list.begin(), node_result_list.end(),
                        [&result](const TaskResult& r) { return r.task_id == result.task_id; }),
                    node_result_list.end()
                );
                
                // Remove node entry if no results left
                if (node_result_list.empty()) {
                    node_results_.erase(node_it);
                }
            }
        }

        // Remove from graph_results_
        graph_results_.erase(it);
    }

    void SetResultTimeout(std::chrono::milliseconds timeout) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        result_timeout_ = timeout;
        // TODO: Implement result cleanup based on timeout
    }

    void SetResultCallback(std::function<void(const TaskResult&)> callback) override {
        std::lock_guard<std::mutex> lock(results_mutex_);
        result_callback_ = callback;
    }

private:
    std::string GenerateUUID() {
        uuid_t uuid;
        uuid_generate(uuid);
        char uuid_str[37];
        uuid_unparse(uuid, uuid_str);
        return std::string(uuid_str);
    }

    std::unordered_map<std::string, TaskResult> task_results_;
    std::unordered_map<std::string, std::vector<TaskResult>> graph_results_;
    std::unordered_map<std::string, std::vector<TaskResult>> node_results_;
    std::chrono::milliseconds result_timeout_;
    std::function<void(const TaskResult&)> result_callback_;
    mutable std::mutex results_mutex_;
};

class DefaultFaultToleranceManager : public FaultToleranceManager {
public:
    DefaultFaultToleranceManager() : config_({3, std::chrono::seconds(1), true, std::chrono::seconds(30), true, true}) {
    }

    bool ShouldRetry(const TaskContext& task_ctx) override {
        // Check if retry is enabled
        if (task_ctx.retries >= config_.max_retries) {
            return false;
        }

        // Check if task is idempotent if retry_idempotent_only is set
        if (config_.retry_idempotent_only && !task_ctx.idempotent) {
            return false;
        }

        // Check if task has timed out
        auto now = std::chrono::steady_clock::now();
        auto task_duration = now - task_ctx.start_time;
        if (task_duration > config_.task_timeout) {
            return true;
        }

        // Check if the task result indicates a retryable error
        if (task_ctx.status != TaskStatus::FAILURE) {
            return false;
        }

        // TODO: Add more sophisticated retry logic based on error type
        return true;
    }

    bool RetryTask(const TaskContext& task_ctx) override {
        // In a real implementation, this would re-execute the task
        // For now, we'll just simulate a retry by returning true
        return true;
    }

    bool RecoverTasks(const std::string& node_id) override {
        // In a real implementation, this would redistribute tasks from a failed node
        // For now, we'll just return true to indicate recovery is possible
        return true;
    }

    bool MarkTaskAsDuplicate(const std::string& task_id) override {
        std::lock_guard<std::mutex> lock(duplicate_tasks_mutex_);
        auto result = duplicate_tasks_.insert(task_id);
        return result.second; // Returns true if inserted (new duplicate)
    }

    void SetConfig(const FaultToleranceConfig& config) override {
        config_ = config;
    }

    FaultToleranceConfig GetConfig() const override {
        return config_;
    }

private:
    FaultToleranceConfig config_;
    std::unordered_set<std::string> duplicate_tasks_;
    mutable std::mutex duplicate_tasks_mutex_;
};

} // namespace distributed
} // namespace cpr