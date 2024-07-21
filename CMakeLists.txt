cmake_minimum_required(VERSION 3.20)

# Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

project(exchange_cpp VERSION 1.0 LANGUAGES CXX)

# Set C++ standard for the entire project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Include Google Test
add_subdirectory(external/googletest)

# Include QuickFIX
add_subdirectory(external/quickfix)

# Include QuickFIX headers
include_directories(${CMAKE_SOURCE_DIR}/external/quickfix/src/C++)
include_directories(${CMAKE_SOURCE_DIR}/external/quickfix/include)

# Define the list of source and header files
set(SOURCES
    src/Book.cpp
    src/Order.cpp
    src/Limit.cpp
    src/Exchange.cpp
    src/FixExchange.cpp
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
    src/FixExchange.hpp
)

# Check that all source files exist
foreach(file ${SOURCES} ${HEADERS})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
        message(FATAL_ERROR "Cannot find source file: ${file}")
    endif()
endforeach()

# Group source and header files in IDEs like Xcode
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

# Define the library target
add_library(exchange_lib SHARED ${SOURCES} ${HEADERS})

# Set properties for the C++ standard for the library
set_target_properties(exchange_lib PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

# Define the test executable
add_executable(exchange_test
    tests/LimitOrderTests.cpp
    tests/MarketOrderTests.cpp
    tests/ExchangeTest.cpp
    tests/main.cpp
)

# Link libraries to your test executable
target_link_libraries(exchange_test PRIVATE
    exchange_lib
    gtest
    gtest_main
    quickfix
)

# Define the benchmark executable
add_executable(fix_main
    src/MainExchangeFix.cpp
)

# Link libraries to your benchmark executable
target_link_libraries(fix_main PRIVATE
    exchange_lib
    quickfix
)

# Set properties for the C++ standard for the executables
set_target_properties(exchange_test fix_main PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

# Enable testing and define a test case
enable_testing()
add_test(NAME Google_Tests COMMAND exchange_test)

# Print summary
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
