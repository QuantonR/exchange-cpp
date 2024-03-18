# exchange-cpp

## Overview
This repository contains the implementation of an efficient order book component designed for use in financial trading platforms. The order book manages market and limit orders, supports order matching, and allows for the modification and cancellation of orders. It's built with a focus on performance, reliability, and accuracy, catering to high-frequency trading environments.

## Features
- Market and Limit Order Support: Handles both market orders, which are executed immediately at the best available price, and limit orders, which are executed at a specified price or better.
- Efficient Matching Engine: Utilizes a sophisticated algorithm to match buy and sell orders based on price and time priority, ensuring a fair and efficient trading process.
- Dynamic Order Management: Offers the ability to cancel or modify existing orders, providing traders with the flexibility to adapt their strategies in real time.
- Data Structure: Implements a highly optimized data structure for storing orders, including:
  - Order: Represents an individual order with details such as order ID, type (buy/sell), quantity, price, and timestamps.
  - Limit: Represents a price level in the book, holding orders of the same price.
  - Book: The top-level structure that organizes and manages buy and sell limits in a balanced manner.

## Getting Started

1. Clone this repository to your local machine:

'git clone https://github.com/QuantonR/exchange-cpp.git'

2. Navigate to the project directory:

'cd order-book-component'

3. Compile the source code (ensure you have a C++ compiler installed):

'g++ -o orderBook main.cpp (Add additional source files as needed)'

4. Run the compiled program:

'./orderBook'
