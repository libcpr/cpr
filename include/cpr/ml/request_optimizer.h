#ifndef CPR_ML_REQUEST_OPTIMIZER_H
#define CPR_ML_REQUEST_OPTIMIZER_H

#include <cpr/cpr.h>
#include <cpr/session.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>

namespace cpr {
namespace ml {

// Request feature types
enum class FeatureType {
    CATEGORICAL,
    NUMERICAL,
    BOOLEAN,
    TIMESTAMP
};

// Request feature
template <typename T>
struct Feature {
    std::string name;
    FeatureType type;
    T value;
    double weight; // Feature importance weight
};

// Request feature vector
using FeatureVector = std::unordered_map<std::string, double>;

// Request prediction results
struct RequestPrediction {
    double success_probability; // Probability of request success (0-1)
    double predicted_response_time; // Predicted response time in milliseconds
    double failure_risk_score; // Failure risk score (0-1)
    double latency_risk_score; // Latency risk score (0-1)
    std::unordered_map<std::string, double> feature_importance; // Importance of each feature
};

// Request optimization suggestions
struct OptimizationSuggestions {
    bool use_proxy; // Whether to use a proxy
    std::string proxy_address; // Suggested proxy address
    std::chrono::milliseconds timeout; // Suggested timeout
    int retry_count; // Suggested number of retries
    bool enable_compression; // Whether to enable compression
    bool prioritize_execution; // Whether to prioritize this request
    int thread_priority; // Suggested thread priority
    std::unordered_map<std::string, std::string> custom_headers; // Suggested custom headers
};

// A/B testing group
enum class ABTestGroup {
    CONTROL, // Default strategy
    EXPERIMENT // Optimized strategy
};

// A/B testing results
struct ABTestResults {
    std::string test_id;
    int control_count;
    int experiment_count;
    double control_success_rate;
    double experiment_success_rate;
    double control_avg_response_time;
    double experiment_avg_response_time;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> end_time;
};

// Request optimizer interface
class RequestOptimizer {
public:
    virtual ~RequestOptimizer() = default;
    virtual FeatureVector ExtractFeatures(const cpr::Session& session, const cpr::Response* response = nullptr) = 0;
    virtual RequestPrediction Predict(const FeatureVector& features) = 0;
    virtual OptimizationSuggestions GenerateOptimizations(const RequestPrediction& prediction) = 0;
    virtual void UpdateModel(const FeatureVector& features, bool success, double response_time) = 0;
    virtual ABTestGroup AssignABGroup(const cpr::Session& session) = 0;
    virtual void RecordABTestResult(ABTestGroup group, bool success, double response_time) = 0;
    virtual ABTestResults GetABTestResults() = 0;
    virtual void StartOnlineLearning() = 0;
    virtual void StopOnlineLearning() = 0;
    virtual void SetModelPath(const std::string& path) = 0;
    virtual bool LoadModel() = 0;
    virtual bool SaveModel() = 0;
};

// Request feature extractor
class FeatureExtractor {
public:
    virtual ~FeatureExtractor() = default;
    virtual FeatureVector Extract(const cpr::Session& session, const cpr::Response* response = nullptr) = 0;
};

// Prediction model interface
class PredictionModel {
public:
    virtual ~PredictionModel() = default;
    virtual RequestPrediction Predict(const FeatureVector& features) = 0;
    virtual void Train(const std::vector<std::pair<FeatureVector, bool>>& success_data,
                       const std::vector<std::pair<FeatureVector, double>>& latency_data) = 0;
    virtual void Update(const FeatureVector& features, bool success, double response_time) = 0;
    virtual bool Load(const std::string& path) = 0;
    virtual bool Save(const std::string& path) = 0;
};

} // namespace ml
} // namespace cpr

#endif // CPR_ML_REQUEST_OPTIMIZER_H