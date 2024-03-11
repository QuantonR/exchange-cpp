//
// Created by Riccardo on 17/02/2024.
//

#ifndef EXCHANGE_CPP_BOOK_H
#define EXCHANGE_CPP_BOOK_H

#include <ostream>
#include "Limit.h"

class Book {
    /* Here is an overview of the variables in the Order class:
     * - buyTree: This is the root of a binary search tree. This structure allows for efficient insertion, deletion, and searching of limit orders. This is used to navigate throughout the buy side of the order book.
     * - sellTree: Similar to the buyTree but for the sell limit orders.
     * - lowestSell: This is a pointer that references the Limit instance representing the lowest sell limit order in the order book. This way we don't have to traverse all the sellTree to find the lowest sell price.
     * - highestBuy: similar to lowestSell but for buy limit orders.
     */

    Limit *buyTree; // used for finding the best buy price
    Limit *sellTree; // used for finding the lowest sell price
    Limit *lowestSell; // pointer to the lowest sell price. Used for quickly matching with best buy
    Limit *highestBuy; // pointer to the best sell

public:
    Book();

    Limit *getBuyTree() const;

    Limit *getSellTree() const;

    Limit *getLowestSell() const;

    Limit *getHighestBuy() const;

    void setBuyTree(Limit *buyTree);

    void setSellTree(Limit *sellTree);

    void setLowestSell(Limit *lowestSell);

    void setHighestBuy(Limit *highestBuy);

    const void addLimitOrder(bool orderType, int size, int entryTime, int eventType, int limitPrice);

    Limit* addLimitToTree(Limit* tree, Limit* parent, int size, int limitPrice, bool orderType);

    void updateAfterAddingLimit(Limit* newLimit, bool isBuyOrder);

    friend std::string toString(const Book &book);

    friend std::string printTree(Limit* tree);
};

#endif //EXCHANGE_CPP_BOOK_H
