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

    Limit* father = nullptr;
    if (orderType == true){
        // this means that it is a buy order

        if (getBuyTree() == nullptr){

            Limit* limitToAdd = new Limit();
            setBuyTree(limitToAdd);
        }
    }
};

void Book::searchForLimit(Limit* tree, int size, int entryTime, int eventType, int limitPrice, bool orderType) {

    if (tree == nullptr) {
        // Reached a leaf, insert the new Limit here
        tree = new Limit(limitPrice, size , nullptr);
        // You might need to adjust the constructor call based on the actual parameters of the Limit constructor
    } else if (limitPrice < tree->getLimitPrice()) {
        // Go left if the new limit's price is less than the current node's price
        searchForLimit(tree->getLeftChild(), size, entryTime, eventType, limitPrice, orderType);
    } else if (limitPrice > tree->getLimitPrice()) {
        // Go right otherwise
        searchForLimit(tree->getRightChild(), size, entryTime, eventType, limitPrice, orderType);
    } else if (limitPrice == tree->getLimitPrice()){
        tree -> addOrder(tree, orderType, size, entryTime, eventType);
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
