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

#include <stdexcept>
#include "Limit.h"
#include "Side.hpp"

class Limit;

/**
 * @class Order
 * @brief Represents an individual order in the order book.
 */
class Order {
public:
    Order(Side orderType, int shares, int limit, int entryTime, Limit* parentLimit);
    
    static void resetGlobalOrderId();

    Order& operator=(const Order&) = delete;
    Order(const Order&) = delete;

    // getters and setters
    int getLimit() const;
    Side getOrderType() const;
    Order* getNextOrder() const;
    Order* getPrevOrder() const;
    Limit* getParentLimit() const;
    int getEntryTime() const;
    int getEventTime() const;
    int getShares() const;
    int64_t getOrderId() const;
    
    void setNextOrder(Order* nextOrder);
    void setPrevOrder(Order* prevOrder);
    void setShares(const int shares);
    
private:
    /// Unique ID for the order, incremented globally
    static int64_t globalOrderId;
    
    /// Unique ID for this order instance
    int64_t orderId;

    /// Type of the order (buy or sell)
    const Side orderType;
    
    /// Number of shares for the order
    int shares;
    
    /// Price limit for the order
    const int limit;
    
    /// Time when the order was entered
    const int entryTime;
    
    /// Time when the order was last modified or filled
    int eventTime;
    
    /// Pointer to the next order in the linked list
    Order* nextOrder;
    
    /// Pointer to the previous order in the linked list
    Order* prevOrder;
    
    /// Pointer to the parent limit
    Limit* parentLimit;
    
    static int64_t updateId();
};
