# exchange-cpp

## Overview

This project is an implementation of a Limit Order Book (LOB) system. The system manages buy and sell orders, matches market orders, and handles order cancellations and modifications. The project is structured into multiple classes representing different components of the order book.

## Classes

### Exchange
- Manages order books for multiple tickers.
- Functions to add tickers, add orders, cancel orders, modify orders, and get the NBBO.
- Uses an unordered_map to store ticker symbols and their associated order books.

### Book
- Manages the buy and sell sides of the order book.
- Functions to add, modify, and cancel orders.
- Places market orders and maintains all orders in an unordered map for fast retrival.

### LOBSide
- Manages one side of the order book (buy or sell).
- Handles adding orders, placing market orders, and executing orders.
- Maintains the best limit and total volume for the side.
- Stores all limits in a map.

### Limit
- Represents a price level in the order book.
- Maintains a dubly linked list of orders at this price level.
- Functions to add orders, partially fill, and fully fill orders.

### Order
- Represents an individual order in the order book.
- Stores order details such as type, shares, price, and timestamps.
- Linked to other orders through next and previous pointers.

## Testing

The project includes a comprehensive set of tests using Google Test. The tests cover various scenarios including adding orders, placing market orders, canceling orders, and modifying orders.

## Build and Run

1. Ensure you have CMake installed.
2. Create a build directory: `mkdir build && cd build`
3. Run CMake: `cmake ..`
4. Build the project: `make`
5. Run the tests: `./test.cpp`

## Next steps:

1. Use FIX protocol for communication
2. Multithread per ticker
3. Implement feed subscription
4. Implement webUI using Qt

## Author

Riccardo Canton
