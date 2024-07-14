cmake_minimum_required(VERSION 3.20)

# Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

project(exchange_cpp)

# Set C++ standard for the entire project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Include Google Test
add_subdirectory(external/googletest)

# Include QuickFIX headers
add_subdirectory(external/quickfix)

# Define the list of source and header files
set(SOURCES
    src/Book.cpp
    src/Order.cpp
    src/Limit.cpp
    src/Exchange.cpp
)

set(HEADERS
    src/Book.h
    src/Order.h
    src/Limit.h
    src/LOBSide.hpp
    src/Exchange.hpp
    src/Side.hpp
    src/OrderData.h
    src/OrderType.h
    src/OrderIdSequence.h
)

# Check that all source files exist
foreach(file ${SOURCES} ${HEADERS})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
        message(FATAL_ERROR "Cannot find source file: ${file}")
    endif()
endforeach()

# Define a shared library with your project files
add_library(exchange_lib SHARED ${SOURCES} ${HEADERS})

# Group source and header files in IDEs like Xcode
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

# Define an executable for the test suite
add_executable(exchange_test
    tests/LimitOrderTests.cpp
    tests/MarketOrderTests.cpp
    tests/ExchangeTest.cpp
    tests/main.cpp
)

# Link libraries to your executable
target_link_libraries(exchange_test PRIVATE
    exchange_lib
    gtest
    gtest_main
    quickfix
)

# Define an executable for the benchmark suite
add_executable(exchange_benchmark
    benchmark/benchmark.cpp
)

# Link libraries to your benchmark executable
target_link_libraries(exchange_benchmark PRIVATE
    exchange_lib
)

# Set properties for the C++ standard
set_target_properties(exchange_lib exchange_test exchange_benchmark PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

# Enable testing and define a test case
enable_testing()
add_test(NAME Google_Tests COMMAND exchange_test)
