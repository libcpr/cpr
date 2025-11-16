#ifndef CPR_GATEWAY_PROXY_GATEWAY_H
#define CPR_GATEWAY_PROXY_GATEWAY_H

#include <cpr/cpr.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>

typedef void* ssl_ctx_t;

amespace cpr {
namespace gateway {

// Protocol type
enum class Protocol {
    HTTP,
    HTTPS,
    WEBSOCKET,
    GRPC
};

// Gateway request
struct GatewayRequest {
    std::string request_id;
    Protocol protocol;
    std::string method;
    std::string url;
    std::string path;
    cpr::Header header;
    cpr::Body body;
    std::string client_ip;
    int client_port;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    std::unordered_map<std::string, std::string> metadata;
};

// Gateway response
struct GatewayResponse {
    std::string response_id;
    std::string request_id;
    int status_code;
    cpr::Header header;
    cpr::Body body;
    std::chrono::milliseconds latency;
    std::string error_message;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};

// Routing rule
struct RoutingRule {
    std::string rule_id;
    std::string pattern;
    std::string target;
    Protocol target_protocol;
    std::vector<std::string> conditions;
    int priority;
    bool enabled;
    std::string description;
};

// Rate limiting configuration
struct RateLimitConfig {
    std::string key;
    int limit;
    std::chrono::milliseconds window;
    std::string action;
    std::unordered_map<std::string, std::string> metadata;
};

// WAF rule
struct WAFRule {
    std::string rule_id;
    std::string type;
    std::string pattern;
    std::string action;
    int priority;
    bool enabled;
    std::string description;
};

// Gateway metrics
struct GatewayMetrics {
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    int total_requests;
    int successful_requests;
    int failed_requests;
    double avg_latency;
    double p50_latency;
    double p95_latency;
    double p99_latency;
    int active_connections;
    int total_connections;
    std::unordered_map<Protocol, int> protocol_distribution;
    std::unordered_map<int, int> status_code_distribution;
};

// Proxy gateway interface
class ProxyGateway {
public:
    virtual ~ProxyGateway() = default;
    virtual bool Start(int port, const std::string& bind_address = "0.0.0.0") = 0;
    virtual bool Stop() = 0;
    virtual bool AddRoutingRule(const RoutingRule& rule) = 0;
    virtual bool RemoveRoutingRule(const std::string& rule_id) = 0;
    virtual bool UpdateRoutingRule(const RoutingRule& rule) = 0;
    virtual bool AddRateLimit(const RateLimitConfig& config) = 0;
    virtual bool RemoveRateLimit(const std::string& key) = 0;
    virtual bool AddWAFRule(const WAFRule& rule) = 0;
    virtual bool RemoveWAFRule(const std::string& rule_id) = 0;
    virtual bool EnableWAF() = 0;
    virtual bool DisableWAF() = 0;
    virtual GatewayMetrics GetMetrics() = 0;
    virtual void SetRequestHandler(std::function<GatewayResponse(const GatewayRequest&)> handler) = 0;
    virtual void SetErrorHandler(std::function<void(const std::string&, const std::string&)> handler) = 0;
    virtual void SetSSLContext(ssl_ctx_t ssl_ctx) = 0;
    virtual bool LoadSSLCertificates(const std::string& cert_path, const std::string& key_path) = 0;
    virtual bool EnableCompression(bool enable) = 0;
    virtual bool SetBufferSize(size_t size) = 0;
};

// Protocol converter interface
class ProtocolConverter {
public:
    virtual ~ProtocolConverter() = default;
    virtual GatewayRequest ConvertToGatewayRequest(const void* protocol_specific_request) = 0;
    virtual void* ConvertFromGatewayRequest(const GatewayRequest& request) = 0;
    virtual GatewayResponse ConvertToGatewayResponse(const void* protocol_specific_response) = 0;
    virtual void* ConvertFromGatewayResponse(const GatewayResponse& response) = 0;
    virtual Protocol GetSourceProtocol() const = 0;
    virtual Protocol GetTargetProtocol() const = 0;
};

// Load balancer for gateway
class GatewayLoadBalancer {
public:
    virtual ~GatewayLoadBalancer() = default;
    virtual std::string SelectTarget(const GatewayRequest& request) = 0;
    virtual void AddTarget(const std::string& target, double weight = 1.0) = 0;
    virtual void RemoveTarget(const std::string& target) = 0;
    virtual void UpdateTargetWeight(const std::string& target, double weight) = 0;
    virtual void SetHealthCheckUrl(const std::string& url) = 0;
    virtual void StartHealthCheck(std::chrono::milliseconds interval) = 0;
    virtual void StopHealthCheck() = 0;
};

} // namespace gateway
} // namespace cpr

#endif // CPR_GATEWAY_PROXY_GATEWAY_H