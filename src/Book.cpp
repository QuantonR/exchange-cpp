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

const void Book::addLimitOrder(bool orderType){

    Limit limitToAdd = Limit();
    if (orderType == true){
        // this means that it is a buy order

        if (getBuyTree() == nullptr){

            setBuyTree(limitToAdd);
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
