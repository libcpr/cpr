#ifndef CPR_BLOCKCHAIN_REQUEST_AUDIT_H
#define CPR_BLOCKCHAIN_REQUEST_AUDIT_H

#include <cpr/cpr.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>
#include <functional>

namespace cpr {
namespace blockchain {

// Blockchain transaction status
enum class TransactionStatus {
    PENDING,
    CONFIRMED,
    FAILED,
    REVERTED
};

// Blockchain transaction
struct BlockchainTransaction {
    std::string transaction_id;
    std::string block_id;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    std::string from_node;
    std::string to_node;
    std::string data_hash;
    std::string previous_hash;
    TransactionStatus status;
    int confirmations;
    std::unordered_map<std::string, std::string> metadata;
};

// Request audit record
struct AuditRecord {
    std::string record_id;
    std::string request_id;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    std::string client_ip;
    std::string user_id;
    std::string request_method;
    std::string request_url;
    std::string request_params;
    std::string request_body_hash;
    std::string response_status;
    std::string response_body_hash;
    std::string node_id;
    std::string transaction_id;
    std::unordered_map<std::string, std::string> headers;
    std::string signature;
};

// Role-based access control (RBAC)
enum class UserRole {
    ADMIN,
    AUDITOR,
    USER,
    GUEST
};

// Permission type
enum class Permission {
    READ,
    WRITE,
    EXECUTE,
    ADMINISTER
};

// Smart contract rule
struct SmartContractRule {
    std::string rule_id;
    std::string condition;
    std::string action;
    bool enabled;
    std::chrono::time_point<std::chrono::steady_clock> created_at;
    std::chrono::time_point<std::chrono::steady_clock> updated_at;
};

// Audit trail
struct AuditTrail {
    std::string trail_id;
    std::string request_id;
    std::vector<AuditRecord> records;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
    bool verified;
    std::string verification_hash;
};

// Request audit interface
class RequestAudit {
public:
    virtual ~RequestAudit() = default;
    virtual std::string CreateAuditRecord(const cpr::Session& session, const cpr::Response& response) = 0;
    virtual std::string CreateAuditRecord(const cpr::AsyncResponse& async_response) = 0;
    virtual bool VerifyAuditRecord(const std::string& record_id) = 0;
    virtual bool VerifyAuditTrail(const std::string& trail_id) = 0;
    virtual AuditRecord GetAuditRecord(const std::string& record_id) = 0;
    virtual AuditTrail GetAuditTrail(const std::string& request_id) = 0;
    virtual std::vector<AuditRecord> GetAuditRecordsByTimeRange(const std::chrono::time_point<std::chrono::steady_clock>& start,
                                                                 const std::chrono::time_point<std::chrono::steady_clock>& end) = 0;
    virtual std::vector<AuditRecord> GetAuditRecordsByNodeId(const std::string& node_id) = 0;
    virtual std::vector<AuditRecord> GetAuditRecordsByUserId(const std::string& user_id) = 0;
    virtual bool AddSmartContractRule(const SmartContractRule& rule) = 0;
    virtual bool RemoveSmartContractRule(const std::string& rule_id) = 0;
    virtual bool EnableSmartContractRule(const std::string& rule_id) = 0;
    virtual bool DisableSmartContractRule(const std::string& rule_id) = 0;
    virtual bool CheckRequestCompliance(const cpr::Session& session) = 0;
    virtual void SetUserRole(const std::string& user_id, UserRole role) = 0;
    virtual bool CheckPermission(const std::string& user_id, Permission permission) = 0;
};

// Blockchain client interface
class BlockchainClient {
public:
    virtual ~BlockchainClient() = default;
    virtual std::string SendTransaction(const std::string& data) = 0;
    virtual BlockchainTransaction GetTransaction(const std::string& transaction_id) = 0;
    virtual bool VerifyTransaction(const std::string& transaction_id) = 0;
    virtual bool Connect() = 0;
    virtual bool Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual std::string GetLatestBlockId() = 0;
    virtual std::string GetBlock(const std::string& block_id) = 0;
    virtual void SetEndpoint(const std::string& endpoint) = 0;
    virtual void SetCredentials(const std::string& username, const std::string& password) = 0;
};

} // namespace blockchain
} // namespace cpr

#endif // CPR_BLOCKCHAIN_REQUEST_AUDIT_H