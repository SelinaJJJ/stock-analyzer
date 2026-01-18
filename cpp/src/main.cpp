#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "../include/indicators.h"

// Simple CSV reader for price data (expects header with symbol,date,open,high,low,close,adj_close,volume)
struct Row { std::string symbol; std::string date; double close; double volume; };

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: quant_core <prices.csv>\n";
        return 1;
    }
    std::string path = argv[1];
    std::ifstream in(path);
    if (!in) { std::cerr << "Cannot open " << path << "\n"; return 2; }
    std::string line;
    // read header
    std::getline(in, line);
    std::map<std::string, std::vector<double>> close_map;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string symbol, date, open, high, low, close, adj, vol;
        std::getline(ss, symbol, ',');
        std::getline(ss, date, ',');
        std::getline(ss, open, ',');
        std::getline(ss, high, ',');
        std::getline(ss, low, ',');
        std::getline(ss, close, ',');
        std::getline(ss, adj, ',');
        std::getline(ss, vol, ',');
        try {
            double c = std::stod(close);
            close_map[symbol].push_back(c);
        } catch(...) { continue; }
    }

    // print CSV header for features
    std::cout << "symbol,as_of,close,ma50,ma200,ema20,slope20,rv20\n";
    for (auto &kv : close_map) {
        const std::string &sym = kv.first;
        const std::vector<double> &prices = kv.second;
        int n = prices.size();
        if (n < 60) continue; // need minimum history
        double ma50 = quant::sma(prices, 50);
        double ma200 = quant::sma(prices, 200);
        double ema20 = quant::ema(prices, 20);
        std::vector<double> returns20;
        for (int i = n-20; i < n; ++i) {
            if (i==0) continue;
            returns20.push_back((prices[i] - prices[i-1]) / prices[i-1]);
        }
        double rv20 = quant::realized_vol(returns20);
        auto slope20opt = quant::slope_logprice(std::vector<double>(prices.end()-20, prices.end()));
        double slope20 = slope20opt ? *slope20opt : 0.0;
        std::cout << sym << "," << "TODAY" << "," << prices.back() << ","
                  << ma50 << "," << ma200 << "," << ema20 << "," << slope20 << "," << rv20 << "\n";
    }

    return 0;
}
