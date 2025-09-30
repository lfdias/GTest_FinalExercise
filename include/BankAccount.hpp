#pragma once
#include <stdexcept>
#include <string>
#include "IRateProvider.hpp"

namespace bank {

// Fatal guard for DEATH TESTS (implementation in Utils.cpp).
namespace detail {
void FatalIf(bool cond, const char* msg);
}

// Simple, didactic bank account: balance, deposit, withdraw, apply interest.
class BankAccount {
public:
    // Create an account with id, owner, and initial balance (must be non-negative).
    BankAccount(std::string id, std::string owner, double initial = 0.0);

    // Trivial accessors
    const std::string& Id()    const { return id_;    }
    const std::string& Owner() const { return owner_; }
    double             Balance() const { return balance_; }

    // Deposit: amount must be > 0
    void Deposit(double amount);

    // Withdraw: amount must be > 0 and <= current balance
    void Withdraw(double amount);

    // Apply monthly interest via an external provider (mocked in tests)
    void ApplyMonthlyInterest(IRateProvider& provider);

    // Monthly fee schedule used in PARAMETERIZED tests
    static double MonthlyFeeFor(double balance);

private:
    std::string id_;
    std::string owner_;
    double      balance_;
};

// ---------- Utilities (implemented in Utils.cpp) ----------

// “Looks like IBAN” (ultra-simple rule for demo):
//   - at least 6 characters
//   - first 2 characters are letters (e.g., "PT", "DE", ...)
bool LooksLikeIban(const std::string& s);

// Predicate: v ∈ [lo, hi]
bool InClosedRange(int v, int lo, int hi);

// For EXPECT_EXIT: if arg == "--help", print usage and exit(2); otherwise return 0.
int MaybeExit(const std::string& arg);

} // namespace bank
