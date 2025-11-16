#include <cpr/ml/request_optimizer.h>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>

namespace cpr {
namespace ml {

class DefaultRequestOptimizer : public RequestOptimizer {
public:
    DefaultRequestOptimizer() : online_learning_running_(false), model_loaded_(false) {
        // Initialize with default model parameters
        // In a real implementation, these would be loaded from a trained model
        default_timeout_ = std::chrono::seconds(10);
        
        // Initialize AB testing
        ab_test_results_.test_id = "default_ab_test";
        ab_test_results_.start_time = std::chrono::steady_clock::now();
        ab_test_results_.control_count = 0;
        ab_test_results_.experiment_count = 0;
        ab_test_results_.control_success_rate = 0.0;
        ab_test_results_.experiment_success_rate = 0.0;
        ab_test_results_.control_avg_response_time = 0.0;
        ab_test_results_.experiment_avg_response_time = 0.0;
    }

    FeatureVector ExtractFeatures(const cpr::Session& session, const cpr::Response* response = nullptr) override {
        FeatureVector features;

        // Extract basic features from the session
        features["request_size"] = EstimateRequestSize(session);
        features["has_payload"] = session.GetPayload().size() > 0 ? 1.0 : 0.0;
        features["has_proxy"] = !session.GetProxies().empty() ? 1.0 : 0.0;
        features["has_timeout"] = session.GetTimeout().count() > 0 ? 1.0 : 0.0;
        features["timeout_value"] = static_cast<double>(session.GetTimeout().count());
        features["has_headers"] = !session.GetHeaders().empty() ? 1.0 : 0.0;
        features["header_count"] = static_cast<double>(session.GetHeaders().size());

        // Extract protocol feature
        const auto& url = session.GetUrl();
        features["is_https"] = url.GetScheme() == "https" ? 1.0 : 0.0;

        // Extract timestamp feature (hour of day)
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto tm_now = std::localtime(&time_t_now);
        features["hour_of_day"] = static_cast<double>(tm_now->tm_hour);

        // Extract response features if available
        if (response != nullptr) {
            features["response_status"] = static_cast<double>(response->status_code);
            features["response_size"] = static_cast<double>(response->text.size());
            features["response_time"] = static_cast<double>(response->elapsed.count());
            features["is_success"] = (response->status_code >= 200 && response->status_code < 300) ? 1.0 : 0.0;
        }

        return features;
    }

    RequestPrediction Predict(const FeatureVector& features) override {
        RequestPrediction prediction;

        // Base success probability on HTTPS usage and response status if available
        double base_success = 0.7;
        if (features.find("is_https") != features.end() && features["is_https"] == 1.0) {
            base_success += 0.15;
        }
        if (features.find("response_status") != features.end()) {
            int status = static_cast<int>(features["response_status"]);
            if (status >= 200 && status < 300) {
                base_success += 0.2;
            } else if (status >= 400) {
                base_success -= 0.3;
            }
        }

        prediction.success_probability = std::clamp(base_success, 0.0, 1.0);

        // Predict response time based on request size and timeout
        double base_time = 500.0; // Base 500ms
        if (features.find("request_size") != features.end()) {
            base_time += features["request_size"] * 0.1; // 0.1ms per byte
        }
        if (features.find("timeout_value") != features.end()) {
            base_time = std::min(base_time, features["timeout_value"] * 0.8);
        }

        prediction.predicted_response_time = base_time;

        // Calculate risk scores
        prediction.failure_risk_score = 1.0 - prediction.success_probability;
        prediction.latency_risk_score = (prediction.predicted_response_time > 1000.0) ? 0.7 : 0.3;

        // Set feature importance
        prediction.feature_importance["is_https"] = 0.25;
        prediction.feature_importance["request_size"] = 0.2;
        prediction.feature_importance["response_status"] = 0.15;
        prediction.feature_importance["timeout_value"] = 0.1;
        prediction.feature_importance["hour_of_day"] = 0.05;

        return prediction;
    }

    OptimizationSuggestions GenerateOptimizations(const RequestPrediction& prediction) override {
        OptimizationSuggestions suggestions;

        // Base suggestions on prediction results
        if (prediction.failure_risk_score > 0.5) {
            suggestions.retry_count = 2;
        } else {
            suggestions.retry_count = 0;
        }

        // Adjust timeout based on predicted response time
        suggestions.timeout = std::chrono::milliseconds(
            static_cast<long long>(prediction.predicted_response_time * 1.5) // 50% buffer
        );

        // Enable compression for large predicted response times
        if (prediction.predicted_response_time > 800.0) {
            suggestions.enable_compression = true;
        }

        // Prioritize execution for high importance requests
        if (prediction.success_probability < 0.3 && prediction.predicted_response_time < 500.0) {
            suggestions.prioritize_execution = true;
            suggestions.thread_priority = 1; // Higher priority
        }

        // Use default proxy if failure risk is high
        if (prediction.failure_risk_score > 0.7) {
            suggestions.use_proxy = true;
            suggestions.proxy_address = "http://proxy.example.com:8080";
        }

        return suggestions;
    }

    void UpdateModel(const FeatureVector& features, bool success, double response_time) override {
        // In a real implementation, this would update the machine learning model
        // For now, we'll just log the update
        std::lock_guard<std::mutex> lock(model_mutex_);
        // TODO: Implement online learning
    }

    ABTestGroup AssignABGroup(const cpr::Session& session) override {
        // Simple random assignment (50/50 split)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        return dis(gen) < 0.5 ? ABTestGroup::CONTROL : ABTestGroup::EXPERIMENT;
    }

    void RecordABTestResult(ABTestGroup group, bool success, double response_time) override {
        std::lock_guard<std::mutex> lock(ab_test_mutex_);

        if (group == ABTestGroup::CONTROL) {
            ab_test_results_.control_count++;
            double success_delta = success ? 1.0 : 0.0;
            ab_test_results_.control_success_rate = 
                (ab_test_results_.control_success_rate * (ab_test_results_.control_count - 1) + success_delta) / 
                ab_test_results_.control_count;
            ab_test_results_.control_avg_response_time = 
                (ab_test_results_.control_avg_response_time * (ab_test_results_.control_count - 1) + response_time) / 
                ab_test_results_.control_count;
        } else {
            ab_test_results_.experiment_count++;
            double success_delta = success ? 1.0 : 0.0;
            ab_test_results_.experiment_success_rate = 
                (ab_test_results_.experiment_success_rate * (ab_test_results_.experiment_count - 1) + success_delta) / 
                ab_test_results_.experiment_count;
            ab_test_results_.experiment_avg_response_time = 
                (ab_test_results_.experiment_avg_response_time * (ab_test_results_.experiment_count - 1) + response_time) / 
                ab_test_results_.experiment_count;
        }
    }

    ABTestResults GetABTestResults() override {
        std::lock_guard<std::mutex> lock(ab_test_mutex_);
        return ab_test_results_;
    }

    void StartOnlineLearning() override {
        {   
            std::lock_guard<std::mutex> lock(learning_mutex_);
            if (online_learning_running_) {
                return;
            }
            online_learning_running_ = true;
        }

        learning_thread_ = std::thread([this]() {
            while (true) {
                {   
                    std::lock_guard<std::mutex> lock(learning_mutex_);
                    if (!online_learning_running_) {
                        break;
                    }
                }

                // TODO: Implement online learning iteration
                std::this_thread::sleep_for(std::chrono::seconds(60)); // Learn every 60 seconds
            }
        });
        learning_thread_.detach();
    }

    void StopOnlineLearning() override {
        std::lock_guard<std::mutex> lock(learning_mutex_);
        online_learning_running_ = false;
    }

    void SetModelPath(const std::string& path) override {
        std::lock_guard<std::mutex> lock(model_mutex_);
        model_path_ = path;
    }

    bool LoadModel() override {
        std::lock_guard<std::mutex> lock(model_mutex_);
        // In a real implementation, this would load a trained model from file
        model_loaded_ = true;
        return true;
    }

    bool SaveModel() override {
        std::lock_guard<std::mutex> lock(model_mutex_);
        // In a real implementation, this would save the current model to file
        return true;
    }

private:
    double EstimateRequestSize(const cpr::Session& session) {
        // Estimate the request size based on session properties
        double size = 0.0;

        // URL size
        size += session.GetUrl().GetUrl().size();

        // Headers size
        for (const auto& header : session.GetHeaders()) {
            size += header.first.size() + header.second.size() + 4; // Key + value + ": " + "\r\n"
        }

        // Payload size
        size += session.GetPayload().size();

        // Parameters size
        for (const auto& param : session.GetParameters()) {
            size += param.first.size() + param.second.size() + 1; // Key + value + "="
        }

        return size;
    }

    std::chrono::milliseconds default_timeout_;
    std::string model_path_;
    bool model_loaded_;
    bool online_learning_running_;
    std::thread learning_thread_;
    ABTestResults ab_test_results_;
    mutable std::mutex model_mutex_;
    mutable std::mutex ab_test_mutex_;
    mutable std::mutex learning_mutex_;
};

class DefaultFeatureExtractor : public FeatureExtractor {
public:
    FeatureVector Extract(const cpr::Session& session, const cpr::Response* response = nullptr) override {
        DefaultRequestOptimizer optimizer;
        return optimizer.ExtractFeatures(session, response);
    }
};

} // namespace ml
} // namespace cpr