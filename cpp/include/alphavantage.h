#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <nlohmann/json.hpp>

namespace alphavantage {

struct PriceBar {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double adj_close;
    long volume;
};

class Client {
public:
    explicit Client(const std::string& api_key);

    // Fetch daily adjusted prices for a symbol
    // Returns prices sorted by date ascending (oldest first)
    std::vector<PriceBar> fetch_daily_adjusted(const std::string& symbol);

    // Check if client is configured with valid API key
    bool is_configured() const;

    // Get last error message
    const std::string& last_error() const;

private:
    std::string api_key_;
    std::string last_error_;

    // HTTP GET request helper
    std::optional<std::string> http_get(const std::string& url);

    // Parse AlphaVantage JSON response
    std::vector<PriceBar> parse_daily_response(const std::string& json_str);
};

} // namespace alphavantage
