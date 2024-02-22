cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "C:/cpp_packages/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(exchange_cpp)

find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS uuid)

set(CMAKE_CXX_STANDARD 20)

add_executable(exchange_cpp
        main.cpp
        src/Book.cpp
        src/Order.cpp
        src/Limit.cpp
)

target_link_libraries(exchange_cpp PRIVATE CURL::libcurl nlohmann_json::nlohmann_json Boost::uuid)
