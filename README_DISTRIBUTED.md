# CPR Distributed - Distributed Request Framework

CPR Distributed is an extension to the CPR library that provides a comprehensive framework for distributed request handling, including task scheduling, load balancing, machine learning optimization, distributed file sync, blockchain auditing, and multi-protocol proxy gateway.

## Table of Contents

1. [Features](#features)
2. [Module Overview](#module-overview)
3. [Getting Started](#getting-started)
4. [Task Scheduling & Load Balancing](#task-scheduling--load-balancing)
5. [Machine Learning Request Optimization](#machine-learning-request-optimization)
6. [Distributed File Synchronization](#distributed-file-synchronization)
7. [Blockchain Request Audit](#blockchain-request-audit)
8. [Multi-protocol Proxy Gateway](#multi-protocol-proxy-gateway)
9. [Build & Installation](#build--installation)
10. [Examples](#examples)

## Features

### 1. Distributed Request Task Scheduling & Load Balancing
- **Task DAG**: Break down complex workflows into directed acyclic graphs (DAG)
- **Parallel/Serial Execution**: Support for both parallel and serial task execution
- **Node Management**: Worker node registration, heartbeat detection, and resource monitoring
- **Dynamic Load Balancing**: Multiple algorithms (round-robin, least connections, response time)
- **Fault Tolerance**: Automatic retry (idempotent/non-idempotent), node failure recovery
- **Result Aggregation**: Global execution reporting with detailed metrics

### 2. Machine Learning Based Smart Request Optimization
- **Feature Extraction**: Collect 200+ features from requests and responses
- **Dual Model**: Classification (success rate) + regression (response time)
- **Online Learning**: Real-time model parameter updates
- **Dynamic Strategy**: Adaptive proxy selection, timeout adjustment, retry mechanism
- **A/B Testing**: Compare optimization strategies vs default behavior
- **Anomaly Detection**: Isolation forest for abnormal request patterns

### 3. Distributed File Synchronization
- **File Chunking**: 10MB default chunk size with SHA-256 verification
- **Breakpoint Resume**: Cross-session resumable transfers
- **Incremental Sync**: Only transfer modified chunks
- **Distributed Storage**: Consistent hashing for high availability
- **Conflict Resolution**: Automatic merging or manual intervention

### 4. Blockchain Based Request Audit & Traceability
- **Data Immutability**: Hyperledger Fabric integration
- **Chain Verification**: Hash-chained request records
- **RBAC**: Role-based access control
- **Visual Traceability**: Query full execution chain
- **Smart Contracts**: Automatic compliance checks

### 5. Multi-protocol Fusion Proxy Gateway
- **Protocol Support**: HTTP/HTTPS, WebSocket, gRPC
- **Protocol Conversion**: Bidirectional conversion between protocols
- **Dynamic Routing**: Lua-based custom routing rules
- **Traffic Control**: Token bucket algorithm for rate limiting
- **Security**: WAF, sensitive data encryption
- **Monitoring**: Metrics collection and distributed tracing (Jaeger)

## Module Overview

```
cpr::distributed
├── task_scheduler.h    # Task DAG, scheduling, execution
├── node_manager.h      # Worker nodes, load balancing
├── result_aggregator.h # Result collection, reporting
cpr::ml
└── request_optimizer.h # ML-based request optimization
cpr::sync
└── file_sync.h         # Distributed file synchronization
cpr::blockchain
└── request_audit.h     # Blockchain audit & traceability
cpr::gateway
└── proxy_gateway.h     # Multi-protocol proxy gateway
```

## Getting Started

```cpp
#include <cpr/cpr.h>
#include <cpr/distributed.h>

using namespace cpr;
using namespace cpr::distributed;

int main() {
    // Create a task graph
    TaskGraph graph;
    
    // Create tasks and dependencies
    TaskMetadata task1;
    task1.task_id = "task_1";
    task1.type = TaskType::GET;
    task1.url = Url{"http://example.com/api/resource1"};
    graph.AddTask(task1);
    
    // Add more tasks and dependencies...
    
    // Get topological order
    auto sorted = graph.TopologicalSort();
    
    return 0;
}
```

## Task Scheduling & Load Balancing

### Task DAG Creation
```cpp
TaskGraph graph;

// Create tasks
TaskMetadata task1{"task_1", TaskType::GET, Url{"http://example.com/api/1"}};
TaskMetadata task2{"task_2", TaskType::POST, Url{"http://example.com/api/2"}};

// Add dependency: task2 depends on task1
TaskDependency dep{"task_1", true};

graph.AddTask(task1);
graph.AddTask(task2);
graph.AddDependency("task_2", dep);

// Topological sort
auto order = graph.TopologicalSort();
```

### Load Balancing Strategies
```cpp
// Available strategies:
// - ROUND_ROBIN
// - LEAST_CONNECTIONS
// - LEAST_LOAD
// - RESPONSE_TIME
// - WEIGHTED_ROUND_ROBIN
// - CUSTOM

std::shared_ptr<LoadBalancer> lb = std::make_shared<RoundRobinLoadBalancer>();
lb->SetStrategy(LoadBalancingStrategy::LEAST_LOAD);

// Custom selector
lb->SetCustomSelector([](const TaskMetadata& task) -> std::shared_ptr<WorkerNode> {
    // Your custom logic here
    return node;
});
```

## Machine Learning Request Optimization

### Request Prediction
```cpp
// Extract features from session
Session session;
session.SetUrl(Url{"http://example.com"});

RequestOptimizer optimizer;
FeatureVector features = optimizer.ExtractFeatures(session);
RequestPrediction prediction = optimizer.Predict(features);

// Get optimization suggestions
OptimizationSuggestions suggestions = optimizer.GenerateOptimizations(prediction);

// Apply optimizations
if (suggestions.use_proxy) {
    session.SetProxies(Proxies{{"http", suggestions.proxy_address}});
}
session.SetTimeout(suggestions.timeout);
```

### A/B Testing
```cpp
// Assign to control or experiment group
ABTestGroup group = optimizer.AssignABGroup(session);

// Execute request
Response response = session.Get();

// Record result
optimizer.RecordABTestResult(group, response.error.code == ErrorCode::OK,
                           response.elapsed / 1000.0); // Convert to milliseconds

// Get test results
ABTestResults results = optimizer.GetABTestResults();
```

## Distributed File Synchronization

### File Upload with Resume
```cpp
FileSync sync;
SyncConfig config;
config.chunk_size = 10 * 1024 * 1024; // 10MB
config.enable_resume = true;
config.max_retries = 3;

std::string sync_id = sync.UploadFile("local_file.txt", "remote/path/file.txt", config);

// Monitor progress
while (true) {
    SyncProgress progress = sync.GetSyncProgress(sync_id);
    std::cout << "Progress: " << progress.progress << "%\n";
    if (progress.status == SyncStatus::SUCCESS || progress.status == SyncStatus::FAILURE) {
        break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

### Incremental Sync
```cpp
SyncConfig config;
config.enable_incremental = true;

std::string sync_id = sync.SyncDirectory("local/dir", "remote/dir", config);
```

## Blockchain Request Audit

### Create Audit Record
```cpp
Session session;
session.SetUrl(Url{"http://example.com/api/transaction"});
session.SetPayload(Payload{{"amount", "100"}, {"to", "user123"}});

Response response = session.Post();

// Create immutable audit record
RequestAudit audit;
std::string record_id = audit.CreateAuditRecord(session, response);

// Verify record integrity
bool verified = audit.VerifyAuditRecord(record_id);
```

### Smart Contract Compliance
```cpp
// Add compliance rule
SmartContractRule rule;
rule.rule_id = "max_amount_rule";
rule.condition = "request.amount > 1000";
rule.action = "BLOCK";
rule.enabled = true;

audit.AddSmartContractRule(rule);

// Check compliance before execution
bool compliant = audit.CheckRequestCompliance(session);
if (!compliant) {
    // Handle non-compliant request
}
```

## Multi-protocol Proxy Gateway

### Gateway Setup
```cpp
ProxyGateway gateway;

// Add routing rule
RoutingRule rule;
rule.rule_id = "grpc_rule";
rule.pattern = "/grpc/service/*";
rule.target = "grpc://localhost:50051";
rule.target_protocol = Protocol::GRPC;

gateway.AddRoutingRule(rule);

// Add rate limit (100 requests per minute per client IP)
RateLimitConfig limit;
limit.key = "client_ip";
limit.limit = 100;
limit.window = std::chrono::minutes(1);
limit.action = "DROP";

gateway.AddRateLimit(limit);

// Start gateway on port 8080
gateway.Start(8080, "0.0.0.0");
```

## Build & Installation

### Prerequisites
- C++17 compatible compiler
- CMake 3.15+
- CPR library dependencies (libcurl, etc.)

### Build with CMake
```bash
mkdir build
cd build
cmake ..
make -j4
make install
```

### Enable Distributed Features
```bash
cmake .. -DCPR_ENABLE_DISTRIBUTED=ON
```

## Examples

See `example/distributed_example.cpp` for a comprehensive example showing all major features.

## License

Same as CPR library - MIT License

## Documentation

For detailed documentation, please refer to:
- API Reference: `docs/distributed_api.md`
- Tutorials: `docs/tutorials/`
- Examples: `examples/`