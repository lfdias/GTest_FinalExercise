// Single entry-point for all your GoogleTest/GoogleMock tests.
// This executable will include *all* your test .cpp files.
#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char** argv) {
  ::testing::InitGoogleMock(&argc, argv);     // also inits GoogleTest
  // Useful default for reliable death tests on Linux/WSL:
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  return RUN_ALL_TESTS();
}
