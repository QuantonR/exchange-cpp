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

#include <vector>
#include <unordered_map>
#include <memory>
#include "Order.h"
#include "Side.hpp"

struct OrderData;
class Book;
class OrderIdSequence;
class Order;

/**
 * @class Limit
 * @brief Represents a price level in the order book, managing orders at that specific price level.
 */
class Limit {
public:
    Limit(int limitPrice);

    void addOrderToLimit(const OrderData& orderData, Book& book, OrderIdSequence& idSequence);
    void partialFill(int remainingVolume);
    void fullFill(Book& book);
    void decreaseSize();

    // getters and setters
    int getLimitPrice() const;
    int getSize() const;
    int getTotalVolume() const;

    Order* getHeadOrder() const;
    Order* getTailOrder() const;

    void setTotalVolume(const int& newVolume);
    void setTailOrder(Order* newTailOrder);
    void setHeadOrder(Order* newHeadOrder);
    
private:
    /// Price level of this limit
    const int limitPrice;
    /// Number of orders at this price level
    int size;
    /// Total volume of shares at this price level
    int totalVolume;
    /// Pointer to the first order in the doubly  linked list at this price level
    Order* headOrder;
    /// Pointer to the last order in the doubly linked list at this price level
    Order* tailOrder;
};
