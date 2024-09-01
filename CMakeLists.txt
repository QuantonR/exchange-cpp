cmake_minimum_required(VERSION 3.20)

# Set the default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

project(exchange_cpp VERSION 1.0 LANGUAGES CXX)

# Set C++ standard for the entire project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Include Google Test
add_subdirectory(external/googletest)

# Include QuickFIX
add_subdirectory(external/quickfix)

# Define the list of core source and header files
set(CORE_SOURCES
    src/core/Book.cpp
    src/core/Order.cpp
    src/core/Limit.cpp
    src/core/Exchange.cpp
)

set(CORE_HEADERS
    src/core/Book.h
    src/core/Order.h
    src/core/Limit.h
    src/core/LOBSide.hpp
    src/core/Exchange.hpp
    src/core/Side.hpp
    src/core/OrderData.h
    src/core/OrderType.h
    src/core/IDGenerator.h
    src/core/Execution.h
)

# Define the list of FIX-related source and header files
set(FIX_SOURCES
    src/fix/FixMessageHandler.cpp
    src/fix/ExecutionManager.cpp
    src/fix/FixClient.cpp
)

set(FIX_HEADERS
    src/fix/FixMessageHandler.h
    src/fix/ExecutionManager.h
    src/fix/FixClient.h
    src/fix/ExchangeRunner.h
)

# Create the core library target
add_library(exchange_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# Create the FIX library target
add_library(exchange_fix STATIC ${FIX_SOURCES} ${FIX_HEADERS})

# Link the QuickFIX library to the FIX components
target_link_libraries(exchange_fix PRIVATE quickfix)

# Define the main executable (exchange main application)
add_executable(fix_main
    src/fix/ExchangeMain.cpp
)

# Link libraries to the main executable
target_link_libraries(fix_main PRIVATE
    exchange_core
    exchange_fix
    quickfix
)

# Define the test executable
add_executable(exchange_test
    tests/core/LimitOrderTests.cpp
    tests/core/OrderModificationTest.cpp
    tests/core/ExecutionTests.cpp
    tests/core/MarketOrderTests.cpp
    tests/core/ExchangeTest.cpp
    tests/core/main.cpp
    tests/fix/FixTests.cpp
)

# Link libraries to your test executable
target_link_libraries(exchange_test PRIVATE
    exchange_core
    exchange_fix
    gtest
    gtest_main
    quickfix
)

# Copy exchangeConfig.cfg to the output directory
configure_file(${CMAKE_SOURCE_DIR}/src/fix/exchangeConfig.cfg ${CMAKE_BINARY_DIR}/exchangeConfig.cfg COPYONLY)

# Modify fix_main to use the exchangeConfig.cfg
add_custom_command(TARGET fix_main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/src/fix/exchangeConfig.cfg $<TARGET_FILE_DIR:fix_main>/exchangeConfig.cfg)

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

# Hide unnecessary intermediate folders and targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Group core and fix targets in Xcode
set_target_properties(exchange_core PROPERTIES FOLDER "Core")
set_target_properties(exchange_fix PROPERTIES FOLDER "Fix")
set_target_properties(exchange_test PROPERTIES FOLDER "Tests")
set_target_properties(fix_main PROPERTIES FOLDER "Executables")
