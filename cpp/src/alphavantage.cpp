#include "alphavantage.h"
#include <curl/curl.h>
#include <sstream>
#include <algorithm>

namespace alphavantage {

namespace {
    size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        size_t total_size = size * nmemb;
        userp->append(static_cast<char*>(contents), total_size);
        return total_size;
    }
}

Client::Client(const std::string& api_key) : api_key_(api_key) {}

bool Client::is_configured() const {
    return !api_key_.empty();
}

const std::string& Client::last_error() const {
    return last_error_;
}

std::optional<std::string> Client::http_get(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        last_error_ = "Failed to initialize CURL";
        return std::nullopt;
    }

    std::string response_body;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        last_error_ = std::string("CURL error: ") + curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        return std::nullopt;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (http_code != 200) {
        last_error_ = "HTTP error: " + std::to_string(http_code);
        return std::nullopt;
    }

    return response_body;
}

std::vector<PriceBar> Client::parse_daily_response(const std::string& json_str) {
    std::vector<PriceBar> bars;

    try {
        auto json = nlohmann::json::parse(json_str);

        // Check for API errors
        if (json.contains("Error Message")) {
            last_error_ = json["Error Message"].get<std::string>();
            return bars;
        }

        if (json.contains("Note")) {
            last_error_ = "Rate limit: " + json["Note"].get<std::string>();
            return bars;
        }

        if (!json.contains("Time Series (Daily)")) {
            last_error_ = "No time series data in response";
            return bars;
        }

        auto& time_series = json["Time Series (Daily)"];

        for (auto& [date, data] : time_series.items()) {
            PriceBar bar;
            bar.date = date;
            bar.open = std::stod(data["1. open"].get<std::string>());
            bar.high = std::stod(data["2. high"].get<std::string>());
            bar.low = std::stod(data["3. low"].get<std::string>());
            bar.close = std::stod(data["4. close"].get<std::string>());

            if (data.contains("5. adjusted close")) {
                bar.adj_close = std::stod(data["5. adjusted close"].get<std::string>());
            } else {
                bar.adj_close = bar.close;
            }

            if (data.contains("6. volume")) {
                bar.volume = std::stol(data["6. volume"].get<std::string>());
            } else if (data.contains("5. volume")) {
                bar.volume = std::stol(data["5. volume"].get<std::string>());
            } else {
                bar.volume = 0;
            }

            bars.push_back(bar);
        }

        // Sort by date ascending (oldest first)
        std::sort(bars.begin(), bars.end(),
            [](const PriceBar& a, const PriceBar& b) {
                return a.date < b.date;
            });

    } catch (const std::exception& e) {
        last_error_ = std::string("JSON parse error: ") + e.what();
        return {};
    }

    return bars;
}

std::vector<PriceBar> Client::fetch_daily_adjusted(const std::string& symbol) {
    if (!is_configured()) {
        last_error_ = "API key not configured";
        return {};
    }

    std::ostringstream url;
    url << "https://www.alphavantage.co/query"
        << "?function=TIME_SERIES_DAILY_ADJUSTED"
        << "&symbol=" << symbol
        << "&outputsize=compact"
        << "&apikey=" << api_key_;

    auto response = http_get(url.str());
    if (!response) {
        return {};
    }

    return parse_daily_response(*response);
}

} // namespace alphavantage
