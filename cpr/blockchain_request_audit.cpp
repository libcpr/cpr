#include <cpr/blockchain/request_audit.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <iostream>

namespace cpr {
namespace blockchain {

// Helper function to calculate SHA-256 hash
std::string CalculateSHA256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

// Helper function to calculate HMAC-SHA256 signature
std::string CalculateHMACSHA256(const std::string& data, const std::string& key) {
    unsigned char* hash = HMAC(EVP_sha256(), key.c_str(), key.size(), 
                              reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), NULL, NULL);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

// Helper function to generate a random UUID-like string
std::string GenerateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

class DefaultRequestAudit : public RequestAudit {
public:
    DefaultRequestAudit() {
        // Initialize with default blockchain client
        blockchain_client_ = std::make_shared<DefaultBlockchainClient>();
        blockchain_client_->Connect();
    }

    std::string CreateAuditRecord(const cpr::Session& session, const cpr::Response& response) override {
        AuditRecord record;
        record.record_id = GenerateUUID();
        record.request_id = session.GetCurlHolder()->GetRequestId();
        record.timestamp = std::chrono::steady_clock::now();
        record.client_ip = "127.0.0.1";
        record.user_id = "unknown";
        record.request_method = session.GetCurlHolder()->GetHttpMethod();
        record.request_url = session.GetCurlHolder()->GetUrl();
        record.request_params = "";
        record.request_body_hash = "";
        record.response_status = std::to_string(response.status_code);
        record.response_body_hash = CalculateSHA256(response.text);
        record.node_id = "node_001";
        
        // Calculate signature
        std::string data_to_sign = record.record_id + record.request_id + record.client_ip;
        record.signature = CalculateHMACSHA256(data_to_sign, "secret_key");
        
        // Send to blockchain
        std::string transaction_id = blockchain_client_->SendTransaction(record.record_id);
        record.transaction_id = transaction_id;
        
        // Store locally
        {   
            std::lock_guard<std::mutex> lock(records_mutex_);
            audit_records_[record.record_id] = record;
        }
        
        return record.record_id;
    }

    std::string CreateAuditRecord(const cpr::AsyncResponse& async_response) override {
        // Extract session and response from async response
        // For simplicity, generate a basic record
        AuditRecord record;
        record.record_id = GenerateUUID();
        record.request_id = "async_" + GenerateUUID();
        record.timestamp = std::chrono::steady_clock::now();
        record.client_ip = "127.0.0.1";
        record.user_id = "unknown";
        record.request_method = "GET";
        record.request_url = "async_url";
        record.request_params = "";
        record.request_body_hash = "";
        record.response_status = "200";
        record.response_body_hash = CalculateSHA256("async_response");
        record.node_id = "node_001";
        
        std::string data_to_sign = record.record_id + record.request_id + record.client_ip;
        record.signature = CalculateHMACSHA256(data_to_sign, "secret_key");
        
        std::string transaction_id = blockchain_client_->SendTransaction(record.record_id);
        record.transaction_id = transaction_id;
        
        {   
            std::lock_guard<std::mutex> lock(records_mutex_);
            audit_records_[record.record_id] = record;
        }
        
        return record.record_id;
    }

    bool VerifyAuditRecord(const std::string& record_id) override {
        std::lock_guard<std::mutex> lock(records_mutex_);
        auto it = audit_records_.find(record_id);
        if (it == audit_records_.end()) {
            return false;
        }
        
        // Verify signature
        const AuditRecord& record = it->second;
        std::string data_to_sign = record.record_id + record.request_id + record.client_ip;
        std::string expected_signature = CalculateHMACSHA256(data_to_sign, "secret_key");
        
        if (record.signature != expected_signature) {
            return false;
        }
        
        // Verify on blockchain
        return blockchain_client_->VerifyTransaction(record.transaction_id);
    }

    bool VerifyAuditTrail(const std::string& trail_id) override {
        std::lock_guard<std::mutex> lock(trails_mutex_);
        auto it = audit_trails_.find(trail_id);
        if (it == audit_trails_.end()) {
            return false;
        }
        
        const AuditTrail& trail = it->second;
        for (const AuditRecord& record : trail.records) {
            if (!VerifyAuditRecord(record.record_id)) {
                return false;
            }
        }
        
        return true;
    }

    AuditRecord GetAuditRecord(const std::string& record_id) override {
        std::lock_guard<std::mutex> lock(records_mutex_);
        auto it = audit_records_.find(record_id);
        if (it != audit_records_.end()) {
            return it->second;
        }
        return AuditRecord{};
    }

    AuditTrail GetAuditTrail(const std::string& request_id) override {
        AuditTrail trail;
        trail.request_id = request_id;
        trail.trail_id = GenerateUUID();
        trail.verified = true;
        
        std::lock_guard<std::mutex> lock(records_mutex_);
        for (const auto& pair : audit_records_) {
            if (pair.second.request_id == request_id) {
                trail.records.push_back(pair.second);
            }
        }
        
        if (!trail.records.empty()) {
            trail.start_time = trail.records[0].timestamp;
            trail.end_time = trail.records.back().timestamp;
        }
        
        // Calculate verification hash
        std::string hash_data;
        for (const AuditRecord& record : trail.records) {
            hash_data += record.record_id + record.transaction_id;
        }
        trail.verification_hash = CalculateSHA256(hash_data);
        
        {   
            std::lock_guard<std::mutex> lock(trails_mutex_);
            audit_trails_[trail.trail_id] = trail;
        }
        
        return trail;
    }

    std::vector<AuditRecord> GetAuditRecordsByTimeRange(const std::chrono::time_point<std::chrono::steady_clock>& start, 
                                                        const std::chrono::time_point<std::chrono::steady_clock>& end) override {
        std::vector<AuditRecord> results;
        std::lock_guard<std::mutex> lock(records_mutex_);
        
        for (const auto& pair : audit_records_) {
            if (pair.second.timestamp >= start && pair.second.timestamp <= end) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }

    std::vector<AuditRecord> GetAuditRecordsByNodeId(const std::string& node_id) override {
        std::vector<AuditRecord> results;
        std::lock_guard<std::mutex> lock(records_mutex_);
        
        for (const auto& pair : audit_records_) {
            if (pair.second.node_id == node_id) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }

    std::vector<AuditRecord> GetAuditRecordsByUserId(const std::string& user_id) override {
        std::vector<AuditRecord> results;
        std::lock_guard<std::mutex> lock(records_mutex_);
        
        for (const auto& pair : audit_records_) {
            if (pair.second.user_id == user_id) {
                results.push_back(pair.second);
            }
        }
        
        return results;
    }

    bool AddSmartContractRule(const SmartContractRule& rule) override {
        std::lock_guard<std::mutex> lock(rules_mutex_);
        smart_contract_rules_[rule.rule_id] = rule;
        return true;
    }

    bool RemoveSmartContractRule(const std::string& rule_id) override {
        std::lock_guard<std::mutex> lock(rules_mutex_);
        return smart_contract_rules_.erase(rule_id) > 0;
    }

    bool EnableSmartContractRule(const std::string& rule_id) override {
        std::lock_guard<std::mutex> lock(rules_mutex_);
        auto it = smart_contract_rules_.find(rule_id);
        if (it != smart_contract_rules_.end()) {
            it->second.enabled = true;
            return true;
        }
        return false;
    }

    bool DisableSmartContractRule(const std::string& rule_id) override {
        std::lock_guard<std::mutex> lock(rules_mutex_);
        auto it = smart_contract_rules_.find(rule_id);
        if (it != smart_contract_rules_.end()) {
            it->second.enabled = false;
            return true;
        }
        return false;
    }

    bool CheckRequestCompliance(const cpr::Session& session) override {
        std::lock_guard<std::mutex> lock(rules_mutex_);
        
        // Check all enabled smart contract rules
        for (const auto& pair : smart_contract_rules_) {
            const SmartContractRule& rule = pair.second;
            if (!rule.enabled) {
                continue;
            }
            
            // In a real implementation, would check if the request matches the rule conditions
            // For now, we'll just check if the rule's target method and path match
            // Note: This is a simplified implementation for demonstration purposes
            auto url = session.GetCurlHolder()->GetUrl();
            auto method = session.GetCurlHolder()->GetHttpMethod();
            
            // Check if method matches
            if (rule.target_method != "*" && rule.target_method != method) {
                continue;
            }
            
            // Check if path matches (simple prefix match)
            if (url.rfind(rule.target_path, 0) == 0) {
                // Request matches a rule
                if (!rule.action) {
                    // Action is not set, deny by default
                    return false;
                }
                
                // Execute the rule action
                return rule.action();
            }
        }
        
        // If no rules match, allow by default
        return true;
    }

    void SetUserRole(const std::string& user_id, UserRole role) override {
        std::lock_guard<std::mutex> lock(roles_mutex_);
        user_roles_[user_id] = role;
    }

    bool CheckPermission(const std::string& user_id, Permission permission) override {
        std::lock_guard<std::mutex> lock(roles_mutex_);
        auto it = user_roles_.find(user_id);
        if (it == user_roles_.end()) {
            return false;
        }
        
        UserRole role = it->second;
        if (role == UserRole::ADMIN) {
            return true;
        } else if (role == UserRole::AUDITOR && permission == Permission::READ) {
            return true;
        } else if (role == UserRole::USER && (permission == Permission::READ || permission == Permission::WRITE)) {
            return true;
        }
        
        return false;
    }

private:
    std::shared_ptr<BlockchainClient> blockchain_client_;
    std::unordered_map<std::string, AuditRecord> audit_records_;
    std::unordered_map<std::string, AuditTrail> audit_trails_;
    std::unordered_map<std::string, SmartContractRule> smart_contract_rules_;
    std::unordered_map<std::string, UserRole> user_roles_;
    
    mutable std::mutex records_mutex_;
    mutable std::mutex trails_mutex_;
    mutable std::mutex rules_mutex_;
    mutable std::mutex roles_mutex_;
};

class DefaultBlockchainClient : public BlockchainClient {
public:
    DefaultBlockchainClient() : connected_(false) {
        // Initialize with default endpoint
        endpoint_ = "http://localhost:8545";
    }

    std::string SendTransaction(const std::string& data) override {
        // Simulate sending transaction to blockchain
        std::string transaction_id = "tx_" + CalculateSHA256(data + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        
        BlockchainTransaction tx;
        tx.transaction_id = transaction_id;
        tx.block_id = "block_001";
        tx.timestamp = std::chrono::steady_clock::now();
        tx.from_node = "node_001";
        tx.to_node = "node_002";
        tx.data_hash = CalculateSHA256(data);
        tx.previous_hash = "prev_hash";
        tx.status = TransactionStatus::CONFIRMED;
        tx.confirmations = 1;
        
        {   
            std::lock_guard<std::mutex> lock(transactions_mutex_);
            transactions_[transaction_id] = tx;
        }
        
        return transaction_id;
    }

    BlockchainTransaction GetTransaction(const std::string& transaction_id) override {
        std::lock_guard<std::mutex> lock(transactions_mutex_);
        auto it = transactions_.find(transaction_id);
        if (it != transactions_.end()) {
            return it->second;
        }
        return BlockchainTransaction{};
    }

    bool VerifyTransaction(const std::string& transaction_id) override {
        std::lock_guard<std::mutex> lock(transactions_mutex_);
        auto it = transactions_.find(transaction_id);
        if (it == transactions_.end()) {
            return false;
        }
        
        // In a real implementation, would verify the transaction against the blockchain
        // For now, we'll just check if it's marked as confirmed
        const BlockchainTransaction& tx = it->second;
        return tx.status == TransactionStatus::CONFIRMED;
    }

    bool Connect() override {
        connected_ = true;
        return true;
    }

    bool Disconnect() override {
        connected_ = false;
        return true;
    }

    bool IsConnected() const override {
        return connected_;
    }

    std::string GetLatestBlockId() override {
        return "block_001";
    }

    std::string GetBlock(const std::string& block_id) override {
        // Simulate block data
        return "{\"block_id\":\"block_001\",\"timestamp\":123456789,\"transactions\":[]}";
    }

    void SetEndpoint(const std::string& endpoint) override {
        endpoint_ = endpoint;
    }

    void SetCredentials(const std::string& username, const std::string& password) override {
        username_ = username;
        password_ = password;
    }

private:
    std::string endpoint_;
    std::string username_;
    std::string password_;
    bool connected_;
    std::unordered_map<std::string, BlockchainTransaction> transactions_;
    mutable std::mutex transactions_mutex_;
};

} // namespace blockchain
} // namespace cpr