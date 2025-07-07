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
#include "OrderIdSequence.h"
#include "Side.hpp"
#include "OrderType.h"

// Forward declaration of Limit
class Limit;
class OrderIdSqeuence;
struct OrderData;


/**
 * @class Order
 * @brief Represents an individual order in the order book, containing order details such as side, size, limit price, timestamps, and links to neighboring orders.
 */
class Order {
public:
    Order(Side orderSide, int shares, float limit, OrderType orderType, Limit* parentLimit, OrderIdSequence& idSequence);
    
    Order& operator=(const Order&) = delete;
    Order(const Order&) = delete;

    // getters
    int getLimit() const;
    Side getOrderSide() const;
    Order* getNextOrder() const;
    Order* getPrevOrder() const;
    Limit* getParentLimit() const;
    int getEntryTime() const;
    int getEventTime() const;
    int getShares() const;
    int64_t getOrderId() const;
    OrderType getOrderType() const;
    
    // setters
    void setNextOrder(Order* nextOrder);
    void setPrevOrder(Order* prevOrder);
    void setShares(const int shares);
    void setParentLimit(Limit* parentLimit);
private:
    int64_t orderId;
    int32_t id;
    int32_t shares;
    int32_t limit; // -1 for market
    int32_t entryTime;
    int32_t eventTime;
    Side orderSide;
    OrderType orderType;
    char padding[2]; // to align pointers
    Order* prevOrder;
    Order* nextOrder;
    Limit* parentLimit;
    char padding2[16]; // pad to 64 bytes exactly
};
