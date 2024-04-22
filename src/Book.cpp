//
// Created by Riccardo on 17/02/2024.
//

#include "Book.h"


Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr) {}

void Book::addLimitOrder(bool orderType, int size, int entryTime, int eventType, int limitPrice) {
    std::unique_ptr<Limit>& fatherTree = orderType ? buyTree : sellTree;
    Limit* searchedLimit = addLimitToTree(fatherTree, nullptr, size, limitPrice, orderType);
    searchedLimit->addOrder(orderType, size, entryTime, eventType);
    updateAfterAddingLimit(searchedLimit, orderType);
}

Limit* Book::addLimitToTree(std::unique_ptr<Limit>& tree, Limit* parent, int size, int limitPrice, bool orderType) {
    if (!tree) {
        tree = std::make_unique<Limit>(limitPrice, size, parent);
        // Update highestBuy or lowestSell immediately after adding the new limit
        if (orderType && (!highestBuy || limitPrice > highestBuy->getLimitPrice())) {
            highestBuy = tree.get();
        } else if (!orderType && (!lowestSell || limitPrice < lowestSell->getLimitPrice())) {
            lowestSell = tree.get();
        }
        return tree.get();
    }

    // Recursively add the limit to the tree
    if (limitPrice < tree->getLimitPrice()) {
        if (!tree->getLeftChild()) {
            tree->setLeftChild(std::make_unique<Limit>(limitPrice, size, tree.get()));
            return tree->getLeftChild();
        } else {
            return addLimitToTree(tree->getLeftUniquePtr(), tree.get(), size, limitPrice, orderType);
        }
    } else if (limitPrice > tree->getLimitPrice()) {
        if (!tree->getRightChild()) {
            tree->setRightChild(std::make_unique<Limit>(limitPrice, size, tree.get()));
            return tree->getRightChild();
        } else {
            return addLimitToTree(tree->getRightUniquePtr(), tree.get(), size, limitPrice, orderType);
        }
    }
    // If the limit already exists, we simply return it.
    return tree.get();
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

// No setters for buyTree and sellTree since they are managed by unique_ptr
