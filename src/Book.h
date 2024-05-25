#pragma once

#include <ostream>
#include <memory>
#include <math.h>
#include <chrono>
#include "Limit.h"


class Book {
    
    /* Here is an overview of the variables in the Order class:
     * - buyTree: This is the root of a binary search tree. This structure allows for efficient insertion, deletion, and searching of limit orders. This is used to navigate throughout the buy side of the order book.
     * - sellTree: Similar to the buyTree but for the sell limit orders.
     * - lowestSell: This is a pointer that references the Limit instance representing the lowest sell limit order in the order book. This way we don't have to traverse all the sellTree to find the lowest sell price.
     * - highestBuy: similar to lowestSell but for buy limit orders.
     */

private:
    std::unique_ptr<Limit> buyTree;
    std::unique_ptr<Limit> sellTree;
    Limit* lowestSell;
    Limit* highestBuy;
    
    // these two integer are used for market order. If the volume requested for a market order is greater than the corrisponding tree sie then we throw an error
    int buyTreeSize;
    int sellTreeSize;
    
    // functions for adding orders
    Limit* addLimitToTree(std::unique_ptr<Limit>& tree, Limit* parent,  int volume, int limitPrice, bool orderType);
    void updateAfterAddingLimit(Limit* newLimit, bool isBuyOrder);
    Limit* findLimit(Limit* root, int limitPrice) const;
    
    
    // functions for executing orders
    void executeOrder(int& remainingVolume, Limit*& limitToExecute, const bool& orderType);
    Limit* findNextLimit(Limit* currentLimit, bool orderType);
    void removeLimit(Limit* limitToRemove);
    
    int getCurrentTimeSeconds() const;
    
public:
    Book();

    void addOrderToBook(bool orderType, int size, float floatLimitPrice);
    
    void placeMarketOrder(int volume, bool orderType);

    Limit* getBuyTree() const;
    Limit* getSellTree() const;
    Limit* getLowestSell() const;
    Limit* getHighestBuy() const;
};
