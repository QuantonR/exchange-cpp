//
// Created by Riccardo on 17/02/2024.
//

#include "Book.h"

Book::Book() : buyTree(nullptr),
               sellTree(nullptr),
               lowestSell(nullptr),
               highestBuy(nullptr){};

const void Book::addLimitOrder(bool orderType, int size, int entryTime, int eventType, int limitPrice){

    std::unique_ptr<Limit> &fatherTree = orderType ? this->getBuyTree() : this->getSellTree();
    Limit* searchedLimit = addLimitToTree(fatherTree, nullptr, size, limitPrice, orderType);
    searchedLimit -> addOrder(orderType, size, entryTime, eventType);
    updateAfterAddingLimit(searchedLimit, orderType);
};

Limit* Book::addLimitToTree(Limit* tree, Limit* parent, int size, int limitPrice, bool orderType) {
    if (!tree) {
        // Reached a leaf, insert the new Limit here
        tree = std::make_unique<Limit>(limitPrice, size, parent);
        return tree.get()
    }
    // Determine the direction to traverse based on limitPrice and recursively add
    if (limitPrice < tree->getLimitPrice()) {
        return addLimitToTree(tree->leftChild, tree.get(), size, limitPrice, orderType);
    } else if (limitPrice > tree->getLimitPrice()) {
        return addLimitToTree(tree->rightChild, tree.get(), size, limitPrice, orderType);
    }
    return tree.get();
}


void Book::updateAfterAddingLimit(Limit* newLimit, bool isBuyOrder) {
    if (isBuyOrder) {
        if (!highestBuy || newLimit->getLimitPrice() > highestBuy->getLimitPrice()) {
            setHighestBuy(newLimit);
        }
    } else {
        if (!lowestSell || newLimit->getLimitPrice() < lowestSell->getLimitPrice()) {
            setLowestSell(newLimit);
        }
    }
}

Limit *Book::getBuyTree() const {
    return buyTree;
}

Limit *Book::getSellTree() const {
    return sellTree;
}

Limit *Book::getLowestSell() const {
    return lowestSell;
}

Limit *Book::getHighestBuy() const {
    return highestBuy;
}

void Book::setBuyTree(Limit *buyTree) {
    Book::buyTree = buyTree;
}

void Book::setSellTree(Limit *sellTree) {
    Book::sellTree = sellTree;
}

void Book::setLowestSell(Limit *lowestSell) {
    Book::lowestSell = lowestSell;
}

void Book::setHighestBuy(Limit *highestBuy) {
    Book::highestBuy = highestBuy;
}

