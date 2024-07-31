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

#ifndef LOBSIDE_HPP
#define LOBSIDE_HPP

#include <map>
#include <memory>
#include "Limit.h"
#include "Side.hpp"

class Book;
class Limit;

/**
 * @class LOBSide
 * @brief Manages the buy or sell side of the order book.
 * @tparam S Side of the order book (buy or sell).
 */
template<Side S>
class LOBSide {
public:
    LOBSide(Book& book);

    Limit* findLimit(int limitPrice) const;
    void addOrderToSide(OrderData& orderData, OrderIdSequence& orderIdSequence);
    void placeMarketOrder(int volume);
    void executeOrder(int& volume, Limit*& LimitToExecute);
    void cancelLimit(Limit* limitToCancel);

    LOBSide(const LOBSide&) = delete;
    LOBSide& operator=(const LOBSide&) = delete;

    // getters
    Limit* getBestLimit() const;
    int getSideVolume() const;
    const std::map<int, std::unique_ptr<Limit>>& getSideTree() const;
    
private:
    /// Stores all limits for this side, mapped by limit price
    std::map<int, std::unique_ptr<Limit>> sideTree;
    /// Total volume of orders for this side
    int sideVolume;
    /// Pointer to the best limit for this side
    Limit* bestLimit;
    
    Book& book;
    
    void updateBestLimit();
    
    static int getCurrentTimeSeconds();
};

/**
 * @brief Constructor that initializes the side of the order book.
 * @param book Reference to the order book to which this side belongs.
 */
template<Side S>
LOBSide<S>::LOBSide(Book& book) : sideVolume(0), bestLimit(nullptr), book(book) {}

/**
 * @brief Adds an order to the side of the order book.
 * @param orderData Reference to the order data containing the order details.
 * @param orderIdSequence Reference to the OrderIdSequence for generating a unique order ID.
 */
template<Side S>
void LOBSide<S>::addOrderToSide(OrderData& orderData, OrderIdSequence& orderIdSequence) {
    
    sideVolume += orderData.shares;
    Limit* limitToAdd = findLimit(orderData.limit.value());
    if (!limitToAdd) {
        auto newLimit = std::make_unique<Limit>(orderData.limit.value());
        limitToAdd = newLimit.get();
        sideTree.emplace(orderData.limit.value(), std::move(newLimit));
        updateBestLimit();
    }

    limitToAdd->addOrderToLimit(orderData, book, orderIdSequence);
}

/**
 * @brief Finds a limit by its price.
 * @param limitPrice Price of the limit.
 * @return Pointer to the limit if found, nullptr otherwise.
 */
template<Side S>
Limit* LOBSide<S>::findLimit(int limitPrice) const {
    
    auto result = sideTree.find(limitPrice);
    if (result == sideTree.end()) {
        return nullptr;
    } else {
        return result->second.get();
    }
}

/**
 * @brief Updates the best limit for the side, which is the highest price for buy side
 *        and the lowest price for sell side.
 */
template<Side S>
void LOBSide<S>::updateBestLimit() {
    
    if (sideTree.empty()) {
        bestLimit = nullptr;
    } else {
        if constexpr (S == Side::Buy) {
            bestLimit = sideTree.rbegin()->second.get();  // Highest price for buy side
        } else if constexpr (S == Side::Sell) {
            bestLimit = sideTree.begin()->second.get();   // Lowest price for sell side
        }
    }
}

/**
 * @brief Places a market order, executing it against existing limit orders on the side.
 * @param volume Volume of the market order.
 * @throws std::runtime_error if the market order size is too large to be executed
 *         or if no corresponding orders are available.
 */
template<Side S>
void LOBSide<S>::placeMarketOrder(int volume) {
    if (volume > sideVolume) {
        throw std::runtime_error("The market order size is too big and it can't be executed right now.");
    }

    Limit* limitToExecute = bestLimit;

    if (limitToExecute == nullptr) {
        throw std::runtime_error("No corresponding orders available to match the market order.");
    }

    while (volume > 0 && limitToExecute) {
        executeOrder(volume, limitToExecute);
    }
}

/**
 * @brief Executes an order against a specified limit.
 * @param volume Volume of the order to execute.
 * @param limitToExecute Pointer to the limit to execute against.
 */
template<Side S>
void LOBSide<S>::executeOrder(int& volume, Limit*& limitToExecute) {
    const int limitVolume = limitToExecute->getTotalVolume();
    if (limitVolume > volume) {
        limitToExecute->partialFill(volume);
        sideVolume -= volume;
        volume = 0;
    } else {
        int orderVolume = limitVolume;
        limitToExecute->fullFill(book);
        volume -= orderVolume;
        sideVolume -= orderVolume;

        cancelLimit(limitToExecute);
        limitToExecute = bestLimit;
    }
}

/**
 * @brief Cancels a limit from the side.
 * @param limitToCancel Pointer to the limit to be canceled.
 */
template<Side S>
void LOBSide<S>::cancelLimit(Limit* limitToCancel) {
    // Ensure limitToCancel is valid
    if (!limitToCancel) return;

    // Update the volume of the side tree
    sideVolume -= limitToCancel->getTotalVolume();

    // Erase the limit from the side tree
    sideTree.erase(limitToCancel->getLimitPrice());

    limitToCancel = nullptr;

    // Update best limit after erasing
    updateBestLimit();
}

/**
 * @brief Returns the best limit for the side.
 * @return Pointer to the best limit.
 */
template<Side S>
Limit* LOBSide<S>::getBestLimit() const {
    return bestLimit;
}

/**
 * @brief Returns the total volume for the side.
 * @return Total volume.
 */
template <Side S>
int LOBSide<S>::getSideVolume() const {
    return sideVolume;
}

/**
 * @brief Returns the side tree.
 * @return Reference to the side tree.
 */
template<Side S>
const std::map<int, std::unique_ptr<Limit>>& LOBSide<S>::getSideTree() const {
    return sideTree;
}

#endif // LOBSIDE_HPP
