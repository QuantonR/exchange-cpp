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
#include "OrderData.h"
#include "OrderIdSequence.h"
#include "Side.hpp"

// Forward declaration of Limit
class Limit;
class OrderIdSqeuence;
struct OrderData;


/**
 * @class Order
 * @brief Represents an individual order in the order book, containing details like order type, limit price, shares, and its position in the order list.
 */
class Order {
public:
    Order(const OrderData& orderData, Limit* parentLimit, OrderIdSequence& idSequence);

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

private:
    int64_t orderId;
    OrderData orderData;
    Order* nextOrder;
    Order* prevOrder;
    Limit* parentLimit;
};
