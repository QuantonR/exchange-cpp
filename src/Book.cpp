//
// Created by Riccardo on 17/02/2024.
//

#include "Book.h"

Book::Book(Limit *buyTree, Limit *sellTree, Limit *lowestSell, Limit *highestBuy) : buyTree(buyTree),
                                                                                    sellTree(sellTree),
                                                                                    lowestSell(lowestSell),
                                                                                    highestBuy(highestBuy) {};

Book::Book() : buyTree(nullptr),
               sellTree(nullptr),
               lowestSell(nullptr),
               highestBuy(nullptr){};

const void Book::addLimitOrder(bool orderType, int size, int entryTime, int eventType, int limitPrice){

    Limit* fatherTree = orderType ? this->getBuyTree() : this->getSellTree();
    Limit* searchedLimit = searchForLimit(fatherTree, nullptr, size, limitPrice);
};

Limit* Book::searchForLimit(Limit* tree, Limit* parent, int size, int limitPrice) {

    if (tree == nullptr) {
        // Reached a leaf, insert the new Limit here
        tree = new Limit(limitPrice, size , parent);
        return tree;
        // You might need to adjust the constructor call based on the actual parameters of the Limit constructor
    } else if (limitPrice < tree->getLimitPrice()) {
        // Go left if the new limit's price is less than the current node's price
        searchForLimit(tree->getLeftChild(), tree, size, limitPrice);
    } else if (limitPrice > tree->getLimitPrice()) {
        // Go right otherwise
        searchForLimit(tree->getRightChild(), tree, size, limitPrice);
    } else if (limitPrice == tree->getLimitPrice()){
        return tree;
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
