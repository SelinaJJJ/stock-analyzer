#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "alphavantage.h"

namespace signals {

struct Signal {
    std::string symbol;
    std::string trend;          // strong_up, up, neutral, down, strong_down
    double prob_up;
    double current_price;
    double monthly_return;      // as percentage
    double volatility;          // annualized, as percentage
    double ma5;
    double ma20;
    int data_points;
    std::string error;          // empty if no error
};

class SignalService {
public:
    explicit SignalService(alphavantage::Client& client);

    // Compute signals for a list of symbols
    std::vector<Signal> compute_signals(const std::vector<std::string>& symbols);

    // Compute signal for a single symbol
    Signal compute_signal(const std::string& symbol);

    // Convert signals to JSON
    static nlohmann::json to_json(const std::vector<Signal>& signals, const std::string& as_of);
    static nlohmann::json signal_to_json(const Signal& signal);

private:
    alphavantage::Client& client_;

    // Determine trend based on price action and moving averages
    std::string determine_trend(double price, double ma5, double ma20, double monthly_return);

    // Calculate probability of upward movement
    double calculate_prob_up(const std::string& trend, double monthly_return);
};

} // namespace signals
