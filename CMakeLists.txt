cmake_minimum_required(VERSION 3.20)

# Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

project(exchange_cpp)

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost 1.84.0 REQUIRED)

# Include Boost headers
include_directories(${Boost_INCLUDE_DIRS})

# Define a shared library with your project files
add_library(exchange_lib SHARED
    src/Book.cpp
    src/Order.cpp
    src/Limit.cpp
    src/Book.h
    src/Order.h
    src/Limit.h
)

# Group source and header files in IDEs like Xcode
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/Book.cpp
    src/Order.cpp
    src/Limit.cpp
    src/Book.h
    src/Order.h
    src/Limit.h
)

# Define an executable for the test suite
add_executable(exchange_test
    Tests/test.cpp
)

# Link libraries to your test executable
target_link_libraries(exchange_test PRIVATE
    exchange_lib
    CURL::libcurl
    nlohmann_json::nlohmann_json
    GTest::GTest
    ${Boost_LIBRARIES}
)

# Define an executable for the test suite
add_executable(exchange_benchmark
    benchmark/benchmark.cpp
)

# Link libraries to your test executable
target_link_libraries(exchange_test PRIVATE
    exchange_lib
    CURL::libcurl
    nlohmann_json::nlohmann_json
    ${Boost_LIBRARIES}
)

# Set properties for the C++ standard
set_target_properties(exchange_lib exchange_test PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

# Enable testing and define a test case
enable_testing()
add_test(NAME Google_Tests COMMAND exchange_test)
