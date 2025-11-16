#ifndef CPR_DISTRIBUTED_H
#define CPR_DISTRIBUTED_H

// Include all distributed modules
#include <cpr/distributed/task_scheduler.h>
#include <cpr/distributed/node_manager.h>
#include <cpr/distributed/result_aggregator.h>
#include <cpr/ml/request_optimizer.h>
#include <cpr/sync/file_sync.h>
#include <cpr/blockchain/request_audit.h>
#include <cpr/gateway/proxy_gateway.h>

namespace cpr {

/**
 * @brief Distributed CPR library namespace
 * 
 * This namespace contains all distributed features of the CPR library:
 * - Task scheduling and load balancing
 * - Machine learning based request optimization
 * - Distributed file synchronization
 * - Blockchain based request auditing
 * - Multi-protocol proxy gateway
 */
namespace distributed {
    // Import all distributed components
    using namespace cpr::distributed;
}

namespace ml {
    // Import all machine learning components
    using namespace cpr::ml;
}

namespace sync {
    // Import all file synchronization components
    using namespace cpr::sync;
}

namespace blockchain {
    // Import all blockchain components
    using namespace cpr::blockchain;
}

namespace gateway {
    // Import all gateway components
    using namespace cpr::gateway;
}

} // namespace cpr

#endif // CPR_DISTRIBUTED_H