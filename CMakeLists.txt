cmake_minimum_required(VERSION 3.20)

project(MatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Main Application Target
add_executable(matching_engine
    src/main.cpp
    src/OrderBook.cpp
)
target_include_directories(matching_engine PRIVATE include)

# 2. Automated Tests Target (Separate executable)
add_executable(run_tests
    tests/tests.cpp
    src/OrderBook.cpp
)
target_include_directories(run_tests PRIVATE include)