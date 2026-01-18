cmake_minimum_required(VERSION 3.16)
project(quant_core VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Download Crow (header-only HTTP framework)
include(FetchContent)
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(crow)

# Download nlohmann/json for JSON parsing
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Indicators library (existing quant core)
add_library(indicators STATIC src/indicators.cpp)
target_include_directories(indicators PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(indicators PRIVATE -Wall -Wextra -Wpedantic)

# AlphaVantage client library
add_library(alphavantage STATIC src/alphavantage.cpp)
target_include_directories(alphavantage PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(alphavantage PUBLIC CURL::libcurl nlohmann_json::nlohmann_json)
target_compile_options(alphavantage PRIVATE -Wall -Wextra -Wpedantic)

# Signal service library
add_library(signals STATIC src/signals.cpp)
target_include_directories(signals PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(signals PUBLIC indicators alphavantage nlohmann_json::nlohmann_json)
target_compile_options(signals PRIVATE -Wall -Wextra -Wpedantic)

# Original CLI tool (for CSV processing)
add_executable(quant_core src/main.cpp)
target_link_libraries(quant_core PRIVATE indicators)

# REST API Server
add_executable(stock_server src/server.cpp)
target_link_libraries(stock_server PRIVATE
    indicators
    signals
    alphavantage
    Crow::Crow
    nlohmann_json::nlohmann_json
    Threads::Threads
)
target_compile_options(stock_server PRIVATE -Wall -Wextra -Wpedantic)

# Unit tests
add_executable(unit_tests tests/tests.cpp)
target_link_libraries(unit_tests PRIVATE indicators)
target_compile_options(unit_tests PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
