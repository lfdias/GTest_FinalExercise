cmake_minimum_required(VERSION 3.16)

# Project name + C++ standard
project(GTest_FinalExercise LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: enable coverage on GCC/Clang
option(ENABLE_COVERAGE "Enable code coverage (GCC/Clang)" OFF)
if(ENABLE_COVERAGE AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)
endif()

# Bring in vendored GoogleTest (expects external/googletest in your repo)
add_subdirectory(external/googletest)

# Your headers live here
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Production library (your 2 sources) ----
add_library(bank_objs
  src/BankAccount.cpp
  src/Utils.cpp
)
target_include_directories(bank_objs PUBLIC ${CMAKE_SOURCE_DIR}/include)

# ---- Single test executable (main + both test files) ----
add_executable(all_tests
  test/test_main.cpp
  test/testBankAccount.cpp
  test/testUtils.cpp
)

# Link: your lib + gtest/gmock (since we provide our own main, DO NOT link gtest_main)
target_link_libraries(all_tests PRIVATE bank_objs gtest gmock)

# Test discovery
enable_testing()
add_test(NAME all COMMAND all_tests)
