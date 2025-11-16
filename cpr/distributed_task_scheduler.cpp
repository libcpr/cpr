#include <cpr/distributed/task_scheduler.h>
#include <algorithm>
#include <stdexcept>

namespace cpr {
namespace distributed {

void TaskGraph::AddTask(const TaskMetadata& metadata) {
    if (tasks_.find(metadata.task_id) != tasks_.end()) {
        throw std::invalid_argument("Task with the same ID already exists");
    }
    tasks_[metadata.task_id] = metadata;
}

void TaskGraph::AddDependency(const std::string& task_id, const TaskDependency& dependency) {
    if (tasks_.find(task_id) == tasks_.end()) {
        throw std::invalid_argument("Task not found");
    }
    if (tasks_.find(dependency.dependency_id) == tasks_.end()) {
        throw std::invalid_argument("Dependency task not found");
    }
    dependencies_[task_id].push_back(dependency);
}

const std::unordered_map<std::string, TaskMetadata>& TaskGraph::GetTasks() const {
    return tasks_;
}

const std::unordered_map<std::string, std::vector<TaskDependency>>& TaskGraph::GetDependencies() const {
    return dependencies_;
}

std::vector<std::string> TaskGraph::TopologicalSort() const {
    // Kahn's algorithm for topological sorting
    std::unordered_map<std::string, int> in_degree;
    std::unordered_map<std::string, std::vector<std::string>> adjacency_list;

    // Initialize in_degree and adjacency_list
    for (const auto& task_pair : tasks_) {
        in_degree[task_pair.first] = 0;
        adjacency_list[task_pair.first] = {};
    }

    for (const auto& dep_pair : dependencies_) {
        const std::string& task_id = dep_pair.first;
        for (const auto& dependency : dep_pair.second) {
            const std::string& dep_id = dependency.dependency_id;
            adjacency_list[dep_id].push_back(task_id);
            in_degree[task_id]++;
        }
    }

    // Queue to store nodes with in_degree 0
    std::queue<std::string> q;
    for (const auto& in_degree_pair : in_degree) {
        if (in_degree_pair.second == 0) {
            q.push(in_degree_pair.first);
        }
    }

    std::vector<std::string> sorted_order;
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        sorted_order.push_back(current);

        for (const auto& neighbor : adjacency_list[current]) {
            if (--in_degree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    // Check if there's a cycle
    if (sorted_order.size() != tasks_.size()) {
        throw std::runtime_error("Task graph contains cycles");
    }

    return sorted_order;
}

} // namespace distributed
} // namespace cpr