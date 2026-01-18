#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include "../include/indicators.h"

void assert_close(double a, double b, double tol=1e-6) {
    if (std::isnan(a) && std::isnan(b)) return;
    if (std::abs(a-b) > tol) {
        std::cerr << "Assertion failed: " << a << " vs " << b << "\n";
        std::exit(2);
    }
}

int main() {
    // sma test
    std::vector<double> xs = {1,2,3,4,5,6,7,8,9,10};
    double s = quant::sma(xs, 5);
    assert_close(s, 8.0);

    // ema basic run
    double e = quant::ema(xs, 5);
    if (std::isnan(e)) { std::cerr << "EMA returned NaN"; return 2; }

    // rsi on up series -> 100
    std::vector<double> up = {1,2,3,4,5,6,7,8,9,10};
    double r = quant::rsi(up, 5);
    if (r < 99.9) { std::cerr << "RSI expected ~100, got "<<r<<"\n"; return 2; }

    // max drawdown
    std::vector<double> p = {100, 120, 80, 90, 70};
    double dd = quant::max_drawdown(p);
    assert_close(dd, (120.0-70.0)/120.0);

    std::cout << "All tests passed" << std::endl;
    return 0;
}
