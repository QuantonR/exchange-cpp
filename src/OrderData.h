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

#include <chrono>
#include <optional>
#include "OrderType.h"
#include "Side.hpp"

/**
 * @brief Gets the current time in seconds.
 * @return Current time in seconds.
 */
inline int getCurrentTimeSeconds() {
    return static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

/**
 * @struct OrderData
 * @brief Represents the data associated with an order.
 */
struct OrderData {
    Side orderSide;
    OrderType orderType;
    int shares;
    std::optional<int> limit; // limit is an optional field (market orders)
    int entryTime;
    int eventTime;

    // Constructor where limit is provided
    OrderData(Side orderSide, int shares, float limit, OrderType orderType)
        : orderSide(orderSide), shares(shares), limit(static_cast<int>(std::round(limit * 100))), orderType(orderType),
          entryTime(getCurrentTimeSeconds()), eventTime(getCurrentTimeSeconds()) {}

    // Constructor where limit is omitted
    OrderData(Side orderSide, int shares, OrderType orderType)
        : orderSide(orderSide), shares(shares), limit(std::nullopt), orderType(orderType),
          entryTime(getCurrentTimeSeconds()), eventTime(getCurrentTimeSeconds()) {}
};
