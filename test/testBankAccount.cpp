#include <gtest/gtest.h>
#include <cerrno>
#include <gmock/gmock.h>
#include "BankAccount.hpp"
#include "IRateProvider.hpp"
#include <tuple>

using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Values;

// ---------- Mock for the rate provider ----------
struct MockRateProvider : bank::IRateProvider {
    MOCK_METHOD(double, GetMonthlyRate, (const std::string& accountId), (override));
};

// ---------- Fixture (fresh state per test) ----------
class AccountFixture : public ::testing::Test {
protected:
    bank::BankAccount acc{"A1", "Alice", 1000.0}; // each test starts from this state
};

// Happy path + validation/guard branches (exceptions)
TEST_F(AccountFixture, DepositWithdrawAndGuards) {
    ASSERT_EQ(acc.Balance(), 1000.0);

    // happy path
    acc.Deposit(200.0);
    EXPECT_EQ(acc.Balance(), 1200.0);
    acc.Withdraw(50.0);
    EXPECT_EQ(acc.Balance(), 1150.0);

    // deposit guards
    EXPECT_THROW(acc.Deposit(0.0),  std::invalid_argument);
    EXPECT_THROW(acc.Deposit(-1.0), std::invalid_argument);

    // withdraw guards
    EXPECT_THROW(acc.Withdraw(0.0),   std::invalid_argument);
    EXPECT_THROW(acc.Withdraw(-1.0),  std::invalid_argument);
    EXPECT_THROW(acc.Withdraw(99999), std::invalid_argument); // insufficient funds
}

// Constructor error branches + trivial accessors
TEST(Constructors, ErrorsAndAccessors) {
    EXPECT_THROW(bank::BankAccount("X","",0.0),     std::invalid_argument); // empty owner
    EXPECT_THROW(bank::BankAccount("X","Bob",-1.0), std::invalid_argument); // negative initial
    bank::BankAccount ok{"ZZ","Zoe",0.0};
    EXPECT_EQ(ok.Id(), "ZZ");
    EXPECT_EQ(ok.Owner(), "Zoe");
}

// ---------- PARAMETERIZED tests: balance -> fee ----------
class FeeParamTest : public ::testing::TestWithParam<std::tuple<double,double>> {};

TEST_P(FeeParamTest, MonthlyFeeMatchesTable) {
    const auto [balance, expected] = GetParam();
    EXPECT_DOUBLE_EQ(bank::BankAccount::MonthlyFeeFor(balance), expected);
}

INSTANTIATE_TEST_SUITE_P(
    FeeTable, FeeParamTest,
    Values(
        std::make_tuple(0.0,     5.0),
        std::make_tuple(999.99,  5.0),
        std::make_tuple(1000.0,  2.5),
        std::make_tuple(4999.99, 2.5),
        std::make_tuple(5000.0,  0.0)
    )
);

// Negative branch for fee
TEST(FeeNegative, ThrowsIfNegativeBalance) {
    EXPECT_THROW(bank::BankAccount::MonthlyFeeFor(-0.01), std::invalid_argument);
}

// ---------- MOCK + ApplyMonthlyInterest ----------
TEST_F(AccountFixture, InterestApplied_UsesProvider) {
    MockRateProvider mock;
    // Expect exactly one call with id "A1" and return 1.25%
    EXPECT_CALL(mock, GetMonthlyRate(StrEq("A1")))
        .Times(1)
        .WillOnce(Return(0.0125));
    acc.ApplyMonthlyInterest(mock); // 1000 + 1.25% = 1012.5
    EXPECT_NEAR(acc.Balance(), 1012.5, 1e-9);
}

TEST_F(AccountFixture, NegativeRateThrows_NoStateChange) {
    MockRateProvider mock;
    EXPECT_CALL(mock, GetMonthlyRate(StrEq("A1")))
        .Times(1)
        .WillOnce(Return(-0.10)); // invalid rate
    const double before = acc.Balance();
    EXPECT_THROW(acc.ApplyMonthlyInterest(mock), std::invalid_argument);
    EXPECT_DOUBLE_EQ(acc.Balance(), before); // balance unchanged on failure
}
