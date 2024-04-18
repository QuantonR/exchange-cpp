cmake_minimum_required(VERSION 3.20)

project(exchange_cpp)

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost 1.84.0 REQUIRED)

# Include Boost headers
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

# Define the executable that uses your project files
add_executable(exchange_cpp
    src/Book.cpp
    src/Order.cpp
    src/Limit.cpp
    src/Book.h
    src/Order.h
    src/Limit.h
    Tests/test.cpp
)

# Link libraries to your main executable
target_link_libraries(exchange_cpp PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    GTest::GTest
    ${Boost_LIBRARIES}
)
