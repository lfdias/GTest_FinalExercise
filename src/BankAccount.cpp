#include "BankAccount.hpp"

namespace bank {

BankAccount::BankAccount(std::string id, std::string owner, double initial)
    : id_(std::move(id)), owner_(std::move(owner)), balance_(initial) {
    // Simple construction rules:
    if (owner_.empty()) throw std::invalid_argument("owner empty");
    if (initial < 0)    throw std::invalid_argument("negative initial balance");
}

void BankAccount::Deposit(double amount) {
    // Only positive amounts are allowed
    if (amount <= 0) throw std::invalid_argument("deposit <= 0");
    balance_ += amount;
}

void BankAccount::Withdraw(double amount) {
    // Only positive amounts and not exceeding the balance
    if (amount <= 0)       throw std::invalid_argument("withdraw <= 0");
    if (amount > balance_) throw std::invalid_argument("insufficient funds");
    balance_ -= amount;
}

void BankAccount::ApplyMonthlyInterest(IRateProvider& provider) {
    // Ask the provider (mocked in tests) for the monthly rate
    const double r = provider.GetMonthlyRate(this->id_);
    if (r < 0) throw std::invalid_argument("rate < 0"); // simple business rule
    balance_ += balance_ * r;
}

double BankAccount::MonthlyFeeFor(double balance) {
    if (balance < 0) throw std::invalid_argument("negative");  
    if (balance < 1000) return 5.0;
    if (balance < 5000) return 2.5;
    return 0.0;
}

} // namespace bank
