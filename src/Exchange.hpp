// An exchange implementation
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

#ifndef Exchange_hpp
#define Exchange_hpp

#include "Book.h"
#include <cassert>
#include <utility>
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>

/**
 * @class Exchange
 * @brief Represents an exchange that manages order books for multiple instruments (tickers), allowing for adding, modifying, and removing orders.
 */

class Exchange {
public:
    Exchange(const std::string& exchangeName);
    
    void addOrder(const std::string& ticker, OrderData& orderData);
    
    void modifyLimitPrice(const std::string& ticker, int64_t orderId, int newLimitPrice);
    void modifyOrderSize(const std::string& ticker, int64_t orderId, int newSize);
    
    void addInstrument(const std::string& newTicker);
    void removeInstrument(const std::string& ticker);
    
    Book* getOrderBook(const std::string& ticker) const;
    std::vector<std::string> getTickerList() const;
    std::pair<std::optional<int>, std::optional<int>> getNBBO(const std::string& ticker) const;
    
    // Deleted copy constructor and assignment operator to prevent copying
    Exchange(const Exchange&) = delete;
    Exchange& operator=(const Exchange&) = delete;
    
private:
    /// a map of ticker symbols to their respective order books
    std::unordered_map<std::string, std::unique_ptr<Book>> tickerLob;
    /// the name of the exchange
    std::string exchangeName;
    /// Order ID sequence generator
    OrderIdSequence globalOrderId;
};

#endif /* Exchange_hpp */
