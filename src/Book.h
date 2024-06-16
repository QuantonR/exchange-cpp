#pragma once

#include <ostream>
#include <math.h>
#include <chrono>
#include "LOBSide.hpp"


class Book {
    
    /* 
     Here is an overview of the variables in the Order class:
     - buyTree: This is the root of a binary search tree. This structure allows for efficient insertion, deletion, and searching of limit orders. This is used to navigate throughout the buy side of the order book.
     - sellTree: Similar to the buyTree but for the sell limit orders.
     - allOrders: is an hash table containing all orders with as key the orderID
     - lowestSell: This is a pointer that references the Limit instance representing the lowest sell limit order in the order book. This way we don't have to traverse all the sellTree to find the lowest sell price.
     - highestBuy: similar to lowestSell but for buy limit orders.
     */

private:
    std::unique_ptr<LOBSide<Side::Sell>> sellSide;
    std::unique_ptr<LOBSide<Side::Buy>> buySide;
    
    std::unordered_map<int, std::unique_ptr<Order>> allOrders;
    
    // functions for adding orders
    Limit* addLimitToTree(std::unique_ptr<Limit>& tree, Limit* parent,  int volume, int limitPrice, bool orderType);
    void updateAfterAddingLimit(Limit* newLimit, bool isBuyOrder);
    Limit* findLimit(Limit* root, int limitPrice, bool orderType) const;
    
    // functions for executing orders
    void executeOrder(int& remainingVolume, Limit*& limitToExecute, const bool& orderType);
    Limit* findNextLimit(Limit* currentLimit, bool orderType);
    void removeLimit(Limit* limitToRemove);
    
    int getCurrentTimeSeconds() const;
    
    Book& operator=(const Book&) = delete;
    Book(const Book&) = delete;
    
public:
    Book();

    void addOrderToBook(bool orderType, int size, float floatLimitPrice);
    
    void placeMarketOrder(int volume, bool orderType);

    LOBSide<Side::Sell>* getSellSide() const;
    LOBSide<Side::Buy>* getBuySide() const;
};
