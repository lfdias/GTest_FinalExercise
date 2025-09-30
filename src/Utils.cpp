#include "BankAccount.hpp"
#include <cctype>   // std::isalpha
#include <cstdio>   // fprintf, fflush
#include <cstdlib>  // std::exit, std::abort
#include <cstdio>
#include <cstdlib>
#include <cerrno>

namespace bank {

bool LooksLikeIban(const std::string& s) {
    // Minimal rule: length and first 2 letters
    if (s.size() < 6) return false;
    return std::isalpha(static_cast<unsigned char>(s[0])) &&
           std::isalpha(static_cast<unsigned char>(s[1]));
}

bool InClosedRange(int v, int lo, int hi) {
    // True if v is within [lo, hi]
    return (lo <= v) && (v <= hi);
}

int MaybeExit(const std::string& arg) {
    // Simulate a tiny CLI: "--help" prints usage and terminates with code 2
    if (arg == "--help") {
        std::fprintf(stderr, "Usage: bank_tool [options]\n");
        std::fflush(stderr);
        std::exit(2); // used by EXPECT_EXIT(..., ExitedWithCode(2), "Usage:")
    }
    return 0; // normal path
}

namespace detail {
void FatalIf(bool cond, const char* msg) {
    // Guard that kills the process (for DEATH TESTS).
    // Important: write to stderr and fflush before abort so gtest captures the message.
    if (cond) {
        std::fprintf(stderr, "%s\n", msg);
        std::fflush(stderr);
        std::abort(); // captured by EXPECT_DEATH
    }
}
} // namespace detail

} // namespace bank
