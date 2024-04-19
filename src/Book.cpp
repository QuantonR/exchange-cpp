//
// Created by Riccardo on 17/02/2024.
//

#include "Book.h"

Book::Book() : buyTree(nullptr),
               sellTree(nullptr),
               lowestSell(nullptr),
               highestBuy(nullptr){};

const void Book::addLimitOrder(bool orderType, int size, int entryTime, int eventType, int limitPrice){

    Limit* fatherTree = orderType ? this->getBuyTree() : this->getSellTree();
    Limit* searchedLimit = addLimitToTree(fatherTree, nullptr, size, limitPrice, orderType);
    searchedLimit -> addOrder(orderType, size, entryTime, eventType);
    updateAfterAddingLimit(searchedLimit, orderType);
};

Limit* Book::addLimitToTree(Limit* tree, Limit* parent, int size, int limitPrice, bool orderType) {
    if (tree == nullptr) {
        // Reached a leaf, insert the new Limit here
        tree = new Limit(limitPrice, size, parent); // Ensure the Limit constructor is correctly defined

        // If this is the root of the tree, update the buy or sell tree root
        if (parent == nullptr) {
            if (orderType) {
                setBuyTree(tree);
                setHighestBuy(tree);
            } else {
                setSellTree(tree);
                setLowestSell(tree);
            }
        }
        return tree;
    }

    // Determine the direction to traverse based on limitPrice and recursively add the limit
    if (limitPrice < tree->getLimitPrice()) {
        // Recursively add to the left, updating tree's left child if it's nullptr
        tree->setLeftChild(addLimitToTree(tree->getLeftChild(), tree, size, limitPrice, orderType));
    } else if (limitPrice > tree->getLimitPrice()) {
        // Recursively add to the right, updating tree's right child if it's nullptr
        tree->setRightChild(addLimitToTree(tree->getRightChild(), tree, size, limitPrice, orderType));
    }

    // If a limit with the exact price exists, just return the existing node
    return tree;
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

std::string printTree(Limit* tree) {
    if (!tree) {
        return "";
    }

    std::string left = printTree(tree->getLeftChild());
    std::string right = printTree(tree->getRightChild());
    std::string node = std::to_string(tree->getLimitPrice()) + " ";

    return left + node + right; // In-order traversal concatenation
}

std::string toString(const Book& book) {
    
    std::string str = "Buy Tree: ";
    str += printTree(book.getBuyTree());
    str += "\nSell Tree: ";
    str += printTree(book.getSellTree());

    Limit* lowestSell = book.getLowestSell();
    str += "\nLowest Sell: ";
    str += lowestSell ? std::to_string(lowestSell->getLimitPrice()) : "None";

    Limit* highestBuy = book.getHighestBuy();
    str += "\nHighest Buy: ";
    str += highestBuy ? std::to_string(highestBuy->getLimitPrice()) : "None";

    return str;
}
