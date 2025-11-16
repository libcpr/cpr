#ifndef CPR_DISTRIBUTED_RESULT_AGGREGATOR_H
#define CPR_DISTRIBUTED_RESULT_AGGREGATOR_H

#include <cpr/cpr.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>

namespace cpr {
namespace distributed {

// Task execution result
struct TaskResult {
    std::string task_id;
    std::string graph_id;
    TaskStatus status;
    cpr::Response response;
    std::string error_message;
    std::chrono::milliseconds duration;
    std::string node_id;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
};

// Global execution report
struct ExecutionReport {
    std::string report_id;
    std::string graph_id;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
    std::chrono::milliseconds total_duration;
    int total_tasks;
    int successful_tasks;
    int failed_tasks;
    int retried_tasks;
    std::vector<TaskResult> task_results;
    std::unordered_map<std::string, std::string> metadata;
};

// Result aggregator interface
class ResultAggregator {
public:
    virtual ~ResultAggregator() = default;
    virtual bool AddResult(const TaskResult& result) = 0;
    virtual bool AddResults(const std::vector<TaskResult>& results) = 0;
    virtual std::vector<TaskResult> GetResultsByGraphId(const std::string& graph_id) = 0;
    virtual std::vector<TaskResult> GetResultsByNodeId(const std::string& node_id) = 0;
    virtual TaskResult GetResultByTaskId(const std::string& task_id) = 0;
    virtual ExecutionReport GenerateReport(const std::string& graph_id) = 0;
    virtual void ClearResults(const std::string& graph_id) = 0;
    virtual void SetResultTimeout(std::chrono::milliseconds timeout) = 0;
    virtual void SetResultCallback(std::function<void(const TaskResult&)> callback) = 0;
};

// Fault tolerance configuration
struct FaultToleranceConfig {
    int max_retries; // Default maximum retries
    std::chrono::milliseconds retry_delay; // Delay between retries
    bool retry_idempotent_only; // Only retry idempotent requests
    std::chrono::milliseconds task_timeout; // Default task timeout
    bool enable_auto_recovery; // Enable automatic task recovery on node failure
    bool enable_duplicate_detection; // Enable duplicate task detection
};

// Fault tolerance manager interface
class FaultToleranceManager {
public:
    virtual ~FaultToleranceManager() = default;
    virtual bool ShouldRetry(const TaskContext& task_ctx) = 0;
    virtual bool RetryTask(const TaskContext& task_ctx) = 0;
    virtual bool RecoverTasks(const std::string& node_id) = 0;
    virtual bool MarkTaskAsDuplicate(const std::string& task_id) = 0;
    virtual void SetConfig(const FaultToleranceConfig& config) = 0;
    virtual FaultToleranceConfig GetConfig() const = 0;
};

} // namespace distributed
} // namespace cpr

#endif // CPR_DISTRIBUTED_RESULT_AGGREGATOR_H