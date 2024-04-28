//
// Created by Riccardo on 17/02/2024.
//

#include "Book.h"


Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr) {}

void Book::addLimitOrder(bool orderType, int orderShares, int entryTime, int eventType, int limitPrice) {
    // Get the correct tree (buy or sell) based on orderType.
    std::unique_ptr<Limit>& tree = orderType ? buyTree : sellTree;
    
    // Check if the limit already exists, and get or create a new limit as necessary.
    Limit* limit = findLimit(tree.get(), limitPrice);
    if (limit == nullptr) {
        limit = addLimitToTree(tree, nullptr, orderShares, limitPrice, orderType);
    }
    // Add the order to the found or new limit.
    limit->addOrder(orderType, orderShares, entryTime, eventType);
    updateAfterAddingLimit(limit, orderType);
}

Limit* Book::findLimit(Limit* root, int limitPrice) const {
    Limit* current = root;
    while (current != nullptr) {
        if (current->getLimitPrice() == limitPrice) {
            // Found the limit with the specified price
            return current;
        } else if (limitPrice < current->getLimitPrice()) {
            // If the search price is less, go left
            current = current->getLeftChild();
        } else {
            // If the search price is greater, go right
            current = current->getRightChild();
        }
    }
    // If we reach here, no limit with the specified price exists in the tree
    return nullptr;
}

Limit* Book::addLimitToTree(std::unique_ptr<Limit>& tree, Limit* parent, int volume, int limitPrice, bool orderType) {
    if (!tree) {
        tree = std::make_unique<Limit>(limitPrice, parent);
        // Update highestBuy or lowestSell immediately after adding the new limit
        if (orderType) {
            highestBuy = (highestBuy && highestBuy->getLimitPrice() > limitPrice) ? highestBuy : tree.get();
        } else {
            lowestSell = (lowestSell && lowestSell->getLimitPrice() < limitPrice) ? lowestSell : tree.get();
        }
        return tree.get();
    } else {
        // This should call a public method on Limit that handles recursion.
        return tree->addLimit(volume, limitPrice, orderType);
    }
}

void Book::updateAfterAddingLimit(Limit* newLimit, bool isBuyOrder) {
    if (isBuyOrder && (!highestBuy || newLimit->getLimitPrice() > highestBuy->getLimitPrice())) {
        highestBuy = newLimit;
    } else if (!isBuyOrder && (!lowestSell || newLimit->getLimitPrice() < lowestSell->getLimitPrice())) {
        lowestSell = newLimit;
    }
}

Limit* Book::getBuyTree() const {
    return buyTree.get();
}

Limit* Book::getSellTree() const {
    return sellTree.get();
}

Limit* Book::getLowestSell() const {
    return lowestSell;
}

Limit* Book::getHighestBuy() const {
    return highestBuy;
}
