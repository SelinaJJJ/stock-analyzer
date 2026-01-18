#pragma once
#include <vector>
#include <optional>

namespace quant {

// Simple indicator utilities. Input is a vector of doubles (close prices or volumes).
double sma(const std::vector<double>& data, int period);
double ema(const std::vector<double>& data, int period);
double realized_vol(const std::vector<double>& returns); // sample stddev
std::optional<double> slope_logprice(const std::vector<double>& prices);
double atr(const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close, int period);
double rsi(const std::vector<double>& closes, int period);
double max_drawdown(const std::vector<double>& prices);

} // namespace quant
