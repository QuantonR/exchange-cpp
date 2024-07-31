// An order book implementation
//
// MIT License
//
// Copyright (c) 2024 Riccardo Canton
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <ostream>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include <memory>
#include "LOBSide.hpp"

/**
 * @class Book
 * @brief Represents an order book that manages buy and sell orders, allowing for placing, canceling, and modifying orders.
 */
class Book {
    
public:
    Book();

    // functions for adding limit orders to the book
    void addOrderToBook(OrderData orderData, OrderIdSequence& orderIdSequence);

    // placing market orders
    void placeMarketOrder(int volume, Side orderSide);

    // canceling orders
    void removeOrderFromLimit(Order* orderToCancel);
    void cancelOrder(int64_t orderId);

    // modify order parameters
    void modifyOrderLimitPrice(int64_t orderId, float newLimitPrice, OrderIdSequence& orderIdSequence);
    void modifyOrderSize(int64_t orderId, int newSize);
    
    // modify allOrders map
    void addOrderToAllOrders(std::unique_ptr<Order> order);
    void removeOrderFromAllOrders(int64_t orderId);
    
    // getters
    LOBSide<Side::Sell>* getSellSide() const;
    LOBSide<Side::Buy>* getBuySide() const;
    const std::unordered_map<int64_t, std::unique_ptr<Order>>* getAllOrders() const;
    
private:
    /// the sell side of the order book
    std::unique_ptr<LOBSide<Side::Sell>> sellSide;
    /// the buy side of the order book
    std::unique_ptr<LOBSide<Side::Buy>> buySide;
    /// a map of all orders in the order book
    std::unordered_map<int64_t, std::unique_ptr<Order>> allOrders;

    Book& operator=(const Book&) = delete;
    Book(const Book&) = delete;

};
