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
#include <iostream>

#include "Side.hpp"

enum class ExecutionType {
    PartialFill,
    FullFill,
};

struct Execution {
    std::string symbol;
    uint64_t executionId;           // Unique ID for the execution
    uint64_t orderMakerId;          // ID of the maker's order
    uint64_t orderTakerId;          // ID of the taker's order
    double executionPrice;          // Price at which the trade was executed
    unsigned int executionSize;     // Number of shares/contracts traded
    std::chrono::system_clock::time_point executionTime; // Timestamp of the execution
    Side sideMaker;                 // Side of the maker's trade (e.g., "Buy", "Sell")
    Side sideTaker;                 // Side of the taker's trade (e.g., "Buy", "Sell")
    ExecutionType makerExecType;    // Type of execution (e.g., "PartialFill", "Fill")
    ExecutionType takerExecType;
    uint32_t takerClientId;
    uint32_t makerClientId;
    int makerExecutedQuantity;
    int takerExecutedQuantity;
    int makerOpenQuantity;
    int takerOpenQuantity;
    float makerAvgPrice;
    float takerAvgPrice;

    Execution(std::string symbol, uint64_t executionId, uint64_t makerId, uint64_t takerId, int execPrice, unsigned int execSize, Side makerSide, Side takerSide, ExecutionType makerExecType, ExecutionType takerExecType, uint32_t takerClientId, uint32_t makerClientId, int makerExecutedQuantity, int takerExecutedQuantity, int makerOpenQuantity, int takerOpenQuantity, float makerAvgPrice, float takerAvgPrice)
        : symbol(symbol), executionId(executionId), orderMakerId(makerId), orderTakerId(takerId), executionPrice(static_cast<double>(execPrice) / 100), executionSize(execSize), executionTime(std::chrono::system_clock::now()), sideMaker(makerSide), sideTaker(takerSide), makerExecType(makerExecType), takerExecType(takerExecType), takerClientId(takerClientId), makerClientId(makerClientId), makerExecutedQuantity(makerExecutedQuantity), takerExecutedQuantity(takerExecutedQuantity), makerOpenQuantity(makerOpenQuantity), takerOpenQuantity(takerOpenQuantity), makerAvgPrice(static_cast<float>(makerAvgPrice) / 100), takerAvgPrice(static_cast<float>(takerAvgPrice) / 100) {}
};
