#ifndef CPR_DISTRIBUTED_TASK_SCHEDULER_H
#define CPR_DISTRIBUTED_TASK_SCHEDULER_H

#include <cpr/cpr.h>
#include <cpr/session.h>
#include <cpr/interceptor.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace cpr {
namespace distributed {

// Task status enum
enum class TaskStatus {
    PENDING,
    RUNNING,
    SUCCESS,
    FAILURE,
    RETRYING
};

// Task type enum
enum class TaskType {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS
};

// Task dependency information
struct TaskDependency {
    std::string dependency_id;
    bool required; // Whether the dependency is mandatory
};

// Task metadata
struct TaskMetadata {
    std::string task_id;
    TaskType type;
    cpr::Url url;
    cpr::Parameters parameters;
    cpr::Header header;
    cpr::Payload payload;
    cpr::Body body;
    cpr::Timeout timeout;
    int max_retries; // Maximum number of retries
    bool idempotent; // Whether the request is idempotent
    std::vector<TaskDependency> dependencies;
    std::unordered_map<std::string, std::string> tags; // Custom tags for classification
};

// Task execution context
struct TaskContext {
    TaskMetadata metadata;
    TaskStatus status;
    int retry_count;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
    std::shared_ptr<cpr::Session> session;
    cpr::Response response;
    std::string error_message;
};

// DAG task graph
class TaskGraph {
public:
    void AddTask(const TaskMetadata& metadata);
    void AddDependency(const std::string& task_id, const TaskDependency& dependency);
    [[nodiscard]] const std::unordered_map<std::string, TaskMetadata>& GetTasks() const;
    [[nodiscard]] const std::unordered_map<std::string, std::vector<TaskDependency>>& GetDependencies() const;
    [[nodiscard]] std::vector<std::string> TopologicalSort() const;

private:
    std::unordered_map<std::string, TaskMetadata> tasks_;
    std::unordered_map<std::string, std::vector<TaskDependency>> dependencies_;
};

// Task scheduler interface
class TaskScheduler {
public:
    virtual ~TaskScheduler() = default;
    virtual std::string SubmitTask(const TaskMetadata& task) = 0;
    virtual std::string SubmitTaskGraph(const TaskGraph& graph) = 0;
    virtual bool CancelTask(const std::string& task_id) = 0;
    virtual TaskStatus GetTaskStatus(const std::string& task_id) = 0;
    virtual cpr::Response GetTaskResult(const std::string& task_id) = 0;
    virtual std::unordered_map<std::string, TaskContext> GetTaskGraphResults(const std::string& graph_id) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

// Global task scheduler singleton
template <typename SchedulerImpl>
class TaskSchedulerSingleton {
public:
    static SchedulerImpl& Instance() {
        static SchedulerImpl instance;
        return instance;
    }

    TaskSchedulerSingleton(const TaskSchedulerSingleton&) = delete;
    TaskSchedulerSingleton& operator=(const TaskSchedulerSingleton&) = delete;
    TaskSchedulerSingleton(TaskSchedulerSingleton&&) = delete;
    TaskSchedulerSingleton& operator=(TaskSchedulerSingleton&&) = delete;

private:
    TaskSchedulerSingleton() = default;
    ~TaskSchedulerSingleton() = default;
};

} // namespace distributed
} // namespace cpr

#endif // CPR_DISTRIBUTED_TASK_SCHEDULER_H