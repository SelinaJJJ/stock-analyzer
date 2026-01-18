#include "../include/indicators.h"
#include <cmath>
#include <numeric>
#include <optional>
#include <algorithm>

namespace quant {

double sma(const std::vector<double>& data, int period) {
    if ((int)data.size() < period || period <= 0) return NAN;
    double sum = 0.0;
    for (int i = data.size() - period; i < (int)data.size(); ++i) sum += data[i];
    return sum / period;
}

double ema(const std::vector<double>& data, int period) {
    if ((int)data.size() < period || period <= 0) return NAN;
    double k = 2.0 / (period + 1.0);
    // initialize with sma of first period
    double ema_val = 0.0;
    for (int i = 0; i < period; ++i) ema_val += data[i];
    ema_val /= period;
    for (int i = period; i < (int)data.size(); ++i) {
        ema_val = data[i] * k + ema_val * (1.0 - k);
    }
    return ema_val;
}

double realized_vol(const std::vector<double>& returns) {
    if (returns.empty()) return NAN;
    double mean = 0.0;
    for (double r : returns) mean += r;
    mean /= returns.size();
    double sumsq = 0.0;
    for (double r : returns) sumsq += (r - mean) * (r - mean);
    double variance = sumsq / (returns.size() - 1 > 0 ? returns.size() - 1 : 1);
    return std::sqrt(variance);
}

std::optional<double> slope_logprice(const std::vector<double>& prices) {
    int n = prices.size();
    if (n < 2) return std::nullopt;
    std::vector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i;
        y[i] = std::log(prices[i]);
    }
    double x_mean = 0.0;
    for (double xi : x) x_mean += xi;
    x_mean /= n;
    double y_mean = 0.0;
    for (double yi : y) y_mean += yi;
    y_mean /= n;
    double num = 0.0, den = 0.0;
    for (int i = 0; i < n; ++i) {
        num += (x[i] - x_mean) * (y[i] - y_mean);
        den += (x[i] - x_mean) * (x[i] - x_mean);
    }
    if (den == 0.0) return std::nullopt;
    return num / den;
}

double atr(const std::vector<double>& high, const std::vector<double>& low, const std::vector<double>& close, int period) {
    int n = close.size();
    if (n < period + 1) return NAN;
    std::vector<double> trs;
    trs.reserve(n-1);
    for (int i = 1; i < n; ++i) {
        double high_i = high[i];
        double low_i = low[i];
        double prev_close = close[i-1];
        double tr = std::max({high_i - low_i, std::abs(high_i - prev_close), std::abs(low_i - prev_close)});
        trs.push_back(tr);
    }
    // simple moving average of TRs over last 'period' days
    if ((int)trs.size() < period) return NAN;
    double sum = 0.0;
    for (int i = trs.size() - period; i < (int)trs.size(); ++i) sum += trs[i];
    return sum / period;
}

double rsi(const std::vector<double>& closes, int period) {
    int n = closes.size();
    if (n <= period) return NAN;
    double gain = 0.0, loss = 0.0;
    for (int i = 1; i <= period; ++i) {
        double diff = closes[i] - closes[i-1];
        if (diff > 0) gain += diff; else loss += -diff;
    }
    gain /= period;
    loss /= period;
    if (loss == 0) return 100.0;
    double rs = gain / loss;
    return 100.0 - (100.0 / (1.0 + rs));
}

double max_drawdown(const std::vector<double>& prices) {
    if (prices.empty()) return 0.0;
    double peak = prices[0];
    double maxdd = 0.0;
    for (double p : prices) {
        if (p > peak) peak = p;
        double dd = (peak - p) / peak;
        if (dd > maxdd) maxdd = dd;
    }
    return maxdd;
}

} // namespace quant
