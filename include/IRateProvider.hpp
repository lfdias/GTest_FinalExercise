#pragma once
#include <string>

namespace bank {

// Minimal interface to provide a monthly interest rate.
// We'll MOCK this in tests with Google Mock.
struct IRateProvider {
    virtual ~IRateProvider() = default;

    // Example: return 0.0125 for 1.25% per month.
    virtual double GetMonthlyRate(const std::string& accountId) = 0;
};

} // namespace bank
