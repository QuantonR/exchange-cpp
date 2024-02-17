cmake_minimum_required(VERSION 3.20)
project(exchange_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(exchange_cpp main.cpp
        src/Book.cpp
        src/Order.cpp
        src/Limit.cpp
        )