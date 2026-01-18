#include "crow.h"
#include "alphavantage.h"
#include "signals.h"
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string get_current_date() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::vector<std::string> split_symbols(const std::string& symbols_csv) {
    std::vector<std::string> result;
    std::istringstream iss(symbols_csv);
    std::string symbol;
    while (std::getline(iss, symbol, ',')) {
        // Trim whitespace
        size_t start = symbol.find_first_not_of(" \t");
        size_t end = symbol.find_last_not_of(" \t");
        if (start != std::string::npos) {
            result.push_back(symbol.substr(start, end - start + 1));
        }
    }
    return result;
}

// Helper to add CORS headers to response
void add_cors_headers(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    // Get configuration from environment
    const char* api_key_env = std::getenv("ALPHAVANTAGE_API_KEY");
    std::string api_key = api_key_env ? api_key_env : "";

    const char* symbols_env = std::getenv("STOCK_SYMBOLS");
    std::string symbols_csv = symbols_env ? symbols_env : "AAPL,MSFT,GOOG,AMZN,NVDA,META,TSLA";

    const char* port_env = std::getenv("PORT");
    int port = port_env ? std::stoi(port_env) : 8080;

    // Initialize services
    alphavantage::Client av_client(api_key);
    signals::SignalService signal_service(av_client);

    auto symbols = split_symbols(symbols_csv);

    // Create Crow app
    crow::SimpleApp app;

    // Health check endpoint
    CROW_ROUTE(app, "/actuator/health")
    ([]() {
        nlohmann::json response;
        response["status"] = "UP";
        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        add_cors_headers(res);
        return res;
    });

    // Get available universes
    CROW_ROUTE(app, "/api/universe")
    ([]() {
        nlohmann::json response = nlohmann::json::array();
        response.push_back("SP500");
        response.push_back("NASDAQ100");
        response.push_back("WATCHLIST");

        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        add_cors_headers(res);
        return res;
    });

    // Get signals
    CROW_ROUTE(app, "/api/signals")
    ([&signal_service, &symbols](const crow::request& req) {
        // Get optional date parameter
        std::string as_of = get_current_date();
        if (req.url_params.get("date")) {
            as_of = req.url_params.get("date");
        }

        // Compute signals for all configured symbols
        auto computed_signals = signal_service.compute_signals(symbols);

        // Convert to JSON
        auto response = signals::SignalService::to_json(computed_signals, as_of);

        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        add_cors_headers(res);
        return res;
    });

    // Get signal for specific symbol
    CROW_ROUTE(app, "/api/signals/<string>")
    ([&signal_service](const std::string& symbol) {
        auto signal = signal_service.compute_signal(symbol);
        auto response = signals::SignalService::signal_to_json(signal);

        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        add_cors_headers(res);
        return res;
    });

    // Trigger manual data ingestion
    CROW_ROUTE(app, "/api/ingest").methods("POST"_method)
    ([&av_client, &symbols]() {
        nlohmann::json response;
        response["symbols_attempted"] = symbols.size();

        int records_ingested = 0;
        nlohmann::json failed = nlohmann::json::array();

        for (const auto& symbol : symbols) {
            auto bars = av_client.fetch_daily_adjusted(symbol);
            if (bars.empty()) {
                failed.push_back(symbol + ": " + av_client.last_error());
            } else {
                records_ingested += static_cast<int>(bars.size());
            }
        }

        response["records_ingested"] = records_ingested;
        response["failed_symbols"] = failed;

        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        add_cors_headers(res);
        return res;
    });

    // Handle OPTIONS preflight requests for CORS
    CROW_ROUTE(app, "/api/<path>").methods("OPTIONS"_method)
    ([](const std::string&) {
        crow::response res(204);
        add_cors_headers(res);
        return res;
    });

    // Start server
    std::cout << "StockPulse C++ Backend Server" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "API Key configured: " << (api_key.empty() ? "NO" : "YES") << std::endl;
    std::cout << "Symbols: " << symbols_csv << std::endl;
    std::cout << std::endl;

    if (api_key.empty()) {
        std::cerr << "WARNING: ALPHAVANTAGE_API_KEY not set. API calls will fail." << std::endl;
    }

    app.port(port).multithreaded().run();

    return 0;
}
