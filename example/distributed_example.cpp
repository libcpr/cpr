#include <cpr/cpr.h>
#include <cpr/distributed.h>
#include <cpr/ml/request_optimizer.h>
#include <cpr/sync/file_sync.h>
#include <cpr/blockchain/request_audit.h>
#include <cpr/gateway/proxy_gateway.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>

using namespace cpr;
using namespace cpr::distributed;
using namespace cpr::ml;
using namespace cpr::sync;
using namespace cpr::blockchain;
using namespace cpr::gateway;

int main() {
    std::cout << "=== CPR Distributed Example ===\n";

    // Example 1: Creating a task graph
    std::cout << "\n1. Creating task graph...\n";
    TaskGraph graph;

    // Create task 1: GET request
    TaskMetadata task1;
    task1.task_id = "task_1";
    task1.type = TaskType::GET;
    task1.url = Url{"http://example.com/api/resource1"};
    task1.timeout = Timeout{10000};
    task1.max_retries = 3;
    task1.idempotent = true;
    graph.AddTask(task1);

    // Create task 2: POST request with dependency on task 1
    TaskMetadata task2;
    task2.task_id = "task_2";
    task2.type = TaskType::POST;
    task2.url = Url{"http://example.com/api/resource2"};
    task2.payload = Payload{{"param1", "value1"}, {"param2", "value2"}};
    task2.timeout = Timeout{15000};
    task2.max_retries = 2;
    task2.idempotent = false;
    graph.AddTask(task2);

    // Add dependency: task2 depends on task1
    TaskDependency dep;
    dep.dependency_id = "task_1";
    dep.required = true;
    graph.AddDependency("task_2", dep);

    // Print topological order
    try {
        auto sorted = graph.TopologicalSort();
        std::cout << "   Topological order: ";
        for (const auto& task_id : sorted) {
            std::cout << task_id << " -> ";
        }
        std::cout << "END\n";
    } catch (const std::exception& e) {
        std::cerr << "   Error: " << e.what() << std::endl;
    }

    // Example 2: Node Manager and Load Balancer
    std::cout << "\n2. Testing Node Manager and Load Balancer...\n";
    auto node_manager = std::make_shared<DefaultNodeManager>();
    auto load_balancer = std::make_shared<DefaultLoadBalancer>(LoadBalancingStrategy::ROUND_ROBIN);
    
    // Add nodes
    node_manager->AddNode("node1", "192.168.1.100", "http://node1:8080");
    node_manager->AddNode("node2", "192.168.1.101", "http://node2:8080");
    node_manager->AddNode("node3", "192.168.1.102", "http://node3:8080");
    
    // Update node resources
    NodeResources resources1 = {8, 16384, 500, "x86_64"};
    node_manager->UpdateNodeResources("node1", resources1);
    
    NodeResources resources2 = {16, 32768, 1000, "x86_64"};
    node_manager->UpdateNodeResources("node2", resources2);
    
    // Select nodes using round-robin strategy
    load_balancer->SetStrategy(LoadBalancingStrategy::ROUND_ROBIN);
    std::string node1 = load_balancer->SelectNode(TaskMetadata{});
    std::string node2 = load_balancer->SelectNode(TaskMetadata{});
    std::string node3 = load_balancer->SelectNode(TaskMetadata{});
    std::cout << "Round-robin node selection: " << node1 << ", " << node2 << ", " << node3 << std::endl;

    // Example 3: Machine Learning Request Optimizer
    std::cout << "\n3. Testing Machine Learning Request Optimizer...\n";
    auto optimizer = std::make_shared<DefaultRequestOptimizer>();
    
    // Create a session
    Session session;
    session.SetUrl(Url{"http://example.com/api/data"});
    session.SetHeader(Header{{"Content-Type", "application/json"}});
    session.SetBody(Body{"{\"param1\": 10, \"param2\": 20}"});
    
    // Get optimization suggestions
    auto suggestions = optimizer->OptimizeRequest(session);
    std::cout << "Optimization suggestions: retry_count=" << suggestions.retry_count << ", timeout=" << suggestions.timeout.count() << "ms" << std::endl;

    // Example 4: File Sync
    std::cout << "\n4. Testing File Sync...\n";
    auto file_sync = std::make_shared<DefaultFileSync>();
    auto distributed_storage = std::make_shared<DefaultDistributedStorage>();
    
    file_sync->SetDistributedStorage(distributed_storage);
    
    // Create a test file
    std::ofstream test_file("test.txt");
    test_file << "Hello, CPR Distributed!" << std::endl;
    test_file.close();
    
    // Simulate file upload
    SyncConfig config;
    config.chunk_size = 1024;
    config.enable_checksum = true;
    
    std::string sync_id = file_sync->UploadFile("test.txt", "remote/test.txt", config);
    
    // Get sync progress
    auto progress = file_sync->GetSyncProgress(sync_id);
    std::cout << "File sync status: " << static_cast<int>(progress.status) << std::endl;
    
    // Clean up test file
    std::remove("test.txt");

    std::cout << "\n=== Example Complete ===\n";
    return 0;
}