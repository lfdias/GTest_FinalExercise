#include <cerrno>
#include <gtest/gtest.h>
#include "BankAccount.hpp"   // declares LooksLikeIban, InClosedRange, MaybeExit, detail::FatalIf
#include <string>
#include <cctype>
#include <cstdlib>           // getenv
#include <vector>
#include <deque>
#include <list>
#include <numeric>



// ---------- Custom assertion (AssertionResult) for clear failure messages ----------
static ::testing::AssertionResult LooksLikeIbanWithMsg(const std::string& s) {
    if (s.size() < 6) {
        return ::testing::AssertionFailure()
               << "IBAN too short: len=" << s.size() << " (need >= 6).";
    }
    if (!std::isalpha(static_cast<unsigned char>(s[0])) ||
        !std::isalpha(static_cast<unsigned char>(s[1]))) {
        return ::testing::AssertionFailure()
               << "First two characters must be letters. Got '"
               << (s.size() >= 2 ? s.substr(0,2) : s) << "'.";
    }
    return ::testing::AssertionSuccess();
}

// ---------- LooksLikeIban: true/false + custom assertion ----------
TEST(Utils_IBAN, LooksLikeIban_PositiveAndNegatives) {
    EXPECT_TRUE(bank::LooksLikeIban("PT1234"));   // true
    EXPECT_FALSE(bank::LooksLikeIban("P1"));      // too short
    EXPECT_FALSE(bank::LooksLikeIban("1T9999"));  // first char not a letter
    EXPECT_TRUE(LooksLikeIbanWithMsg("DE0099"));  // custom assertion (better diagnostics)
}

// ---------- Predicates: EXPECT_PRED3 + false path ----------
TEST(Utils_Range, InClosedRange_Predicate) {
    EXPECT_PRED3(bank::InClosedRange, 5, 0, 10);  // within
    EXPECT_FALSE(bank::InClosedRange(11, 0, 10)); // outside
}

// ---------- DEATH / EXIT: FatalIf and MaybeExit ----------
TEST(Utils_Death, FatalIfAborts) {
    EXPECT_DEATH(bank::detail::FatalIf(true, "fatal guard fired"), "fatal guard fired");
}
TEST(Utils_Death, FatalIfSafePath) {
    bank::detail::FatalIf(false, "no-op");
    SUCCEED();
}
TEST(Utils_Exit, HelpExitsWithCode2) {
    EXPECT_EXIT(bank::MaybeExit("--help"), ::testing::ExitedWithCode(2), "Usage:");
}
TEST(Utils_Exit, OtherArgDoesNotExit) {
    EXPECT_EQ(bank::MaybeExit("run"), 0);
}

// ---------- TYPED TESTS (bonus demo): same logic across multiple container types ----------
template <class C>
class Ledger {
public:
    void Add(double v){ items_.push_back(v); }
    double Total() const { return std::accumulate(items_.begin(), items_.end(), 0.0); }
    size_t Size() const { return items_.size(); }
    void Clear(){ items_.clear(); }
private:
    C items_;
};

template <typename TCont>
class LedgerTyped : public ::testing::Test { protected: Ledger<TCont> led; };

using ContainerTypes = ::testing::Types<std::vector<double>, std::deque<double>, std::list<double>>;
TYPED_TEST_SUITE(LedgerTyped, ContainerTypes);

TYPED_TEST(LedgerTyped, AddAndClear) {
    this->led.Add(1.0);
    this->led.Add(2.5);
    EXPECT_EQ(this->led.Size(), 2u);
    EXPECT_DOUBLE_EQ(this->led.Total(), 3.5);
    this->led.Clear();
    EXPECT_EQ(this->led.Size(), 0u);
}

// ---------- Skip / Disabled ----------
TEST(Utils_Skip, MaybeSkip) {
    if (!std::getenv("BANK_RUN_SKIP_DEMO"))
        GTEST_SKIP() << "Set BANK_RUN_SKIP_DEMO=1 to run.";
    SUCCEED();
}

TEST(Utils_Disabled, DISABLED_KnownIssue) {
    // Kept for documentation; does not run by default
    EXPECT_EQ(2+2, 5);
}
