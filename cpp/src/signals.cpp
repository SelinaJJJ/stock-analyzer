#include "signals.h"
#include "indicators.h"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace signals {

SignalService::SignalService(alphavantage::Client& client) : client_(client) {}

std::vector<Signal> SignalService::compute_signals(const std::vector<std::string>& symbols) {
    std::vector<Signal> results;
    results.reserve(symbols.size());

    for (const auto& symbol : symbols) {
        results.push_back(compute_signal(symbol));
    }

    return results;
}

Signal SignalService::compute_signal(const std::string& symbol) {
    Signal signal;
    signal.symbol = symbol;
    signal.trend = "unknown";
    signal.prob_up = 0.5;
    signal.current_price = 0;
    signal.monthly_return = 0;
    signal.volatility = 0;
    signal.ma5 = 0;
    signal.ma20 = 0;
    signal.data_points = 0;

    // Fetch price data
    auto bars = client_.fetch_daily_adjusted(symbol);

    if (bars.empty()) {
        signal.error = client_.last_error();
        return signal;
    }

    signal.data_points = static_cast<int>(bars.size());

    if (bars.size() < 5) {
        signal.trend = "insufficient_data";
        signal.error = "Need at least 5 data points";
        return signal;
    }

    // Extract close prices (bars are sorted oldest to newest)
    std::vector<double> closes;
    closes.reserve(bars.size());
    for (const auto& bar : bars) {
        closes.push_back(bar.close);
    }

    // Current price is the last close
    signal.current_price = closes.back();

    // Calculate monthly return (last 20-30 trading days)
    size_t month_days = std::min(static_cast<size_t>(30), closes.size());
    double month_ago_price = closes[closes.size() - month_days];
    signal.monthly_return = ((signal.current_price - month_ago_price) / month_ago_price) * 100.0;

    // Calculate daily returns for volatility
    std::vector<double> returns;
    returns.reserve(closes.size() - 1);
    for (size_t i = 1; i < closes.size(); ++i) {
        returns.push_back((closes[i] - closes[i-1]) / closes[i-1]);
    }

    // Calculate annualized volatility
    double vol = quant::realized_vol(returns);
    signal.volatility = vol * std::sqrt(252.0) * 100.0;  // Annualized as percentage

    // Calculate moving averages (on most recent data)
    // We need to reverse for SMA calculation since quant::sma uses last N values
    std::vector<double> closes_reversed(closes.rbegin(), closes.rend());

    if (closes.size() >= 5) {
        signal.ma5 = quant::sma(closes_reversed, 5);
    }

    if (closes.size() >= 20) {
        signal.ma20 = quant::sma(closes_reversed, 20);
    } else {
        signal.ma20 = quant::sma(closes_reversed, static_cast<int>(closes.size()));
    }

    // Determine trend
    signal.trend = determine_trend(signal.current_price, signal.ma5, signal.ma20, signal.monthly_return);

    // Calculate probability
    signal.prob_up = calculate_prob_up(signal.trend, signal.monthly_return);

    return signal;
}

std::string SignalService::determine_trend(double price, double ma5, double ma20, double monthly_return) {
    // Strong uptrend: price > MA5 > MA20 and positive monthly return > 2%
    if (price > ma5 && ma5 > ma20 && monthly_return > 2.0) {
        return "strong_up";
    }

    // Uptrend: price > MA20 and positive return
    if (price > ma20 && monthly_return > 0) {
        return "up";
    }

    // Strong downtrend: price < MA5 < MA20 and negative monthly return < -2%
    if (price < ma5 && ma5 < ma20 && monthly_return < -2.0) {
        return "strong_down";
    }

    // Downtrend: price < MA20 and negative return
    if (price < ma20 && monthly_return < 0) {
        return "down";
    }

    return "neutral";
}

double SignalService::calculate_prob_up(const std::string& trend, double monthly_return) {
    double base_prob;

    if (trend == "strong_up") {
        base_prob = 0.75;
    } else if (trend == "up") {
        base_prob = 0.60;
    } else if (trend == "neutral") {
        base_prob = 0.50;
    } else if (trend == "down") {
        base_prob = 0.40;
    } else if (trend == "strong_down") {
        base_prob = 0.25;
    } else {
        base_prob = 0.50;
    }

    // Adjust based on monthly return magnitude
    double adjustment = std::min(0.15, std::abs(monthly_return) / 100.0);
    if (monthly_return > 0) {
        base_prob += adjustment;
    } else {
        base_prob -= adjustment;
    }

    // Clamp to valid range
    return std::max(0.05, std::min(0.95, base_prob));
}

nlohmann::json SignalService::signal_to_json(const Signal& signal) {
    nlohmann::json j;
    j["symbol"] = signal.symbol;
    j["trend"] = signal.trend;
    j["prob_up"] = std::round(signal.prob_up * 100) / 100.0;

    if (signal.current_price > 0) {
        j["current_price"] = std::round(signal.current_price * 100) / 100.0;
        j["monthly_return"] = std::round(signal.monthly_return * 100) / 100.0;
        j["volatility"] = std::round(signal.volatility * 100) / 100.0;
        j["ma5"] = std::round(signal.ma5 * 100) / 100.0;
        j["ma20"] = std::round(signal.ma20 * 100) / 100.0;
        j["data_points"] = signal.data_points;
    }

    if (!signal.error.empty()) {
        j["error"] = signal.error;
    }

    return j;
}

nlohmann::json SignalService::to_json(const std::vector<Signal>& signals, const std::string& as_of) {
    nlohmann::json result;
    result["as_of"] = as_of;
    result["horizon"] = "30d";

    nlohmann::json signal_array = nlohmann::json::array();
    for (const auto& signal : signals) {
        signal_array.push_back(signal_to_json(signal));
    }
    result["signals"] = signal_array;

    return result;
}

} // namespace signals
