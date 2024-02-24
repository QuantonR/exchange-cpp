//
// Created by Riccardo on 17/02/2024.
//

#include "Limit.h"

Limit::Limit() : limitPrice(0), size(0), totalVolume(0),
                 parent(nullptr), leftChild(nullptr), rightChild(nullptr),
                 headOrder(nullptr), tailOrder(nullptr) {}

Limit::Limit(int limitPrice, int size, int totalVolume, int entryTime, bool orderType, Limit *parent) : limitPrice(limitPrice), size(size), totalVolume(totalVolume),
                                                   parent(parent) {

    Order newOrder = Order(orderType, size, limitPrice, entryTime, entryTime,this);

};

void addLimit(Limit *parent, Limit *limitToAdd){


};

int Limit::getLimitPrice() const {
    return limitPrice;
}

int Limit::getSize() const {
    return size;
}

int Limit::getTotalVolume() const {
    return totalVolume;
}

Order *Limit::getHeadOrder() const {
    return headOrder;
}

Order *Limit::getTailOrder() const {
    return tailOrder;
}

void Limit::setHeadOrder(Order *headOrder) {
    Limit::headOrder = headOrder;
}

void Limit::setTailOrder(Order *tailOrder) {
    Limit::tailOrder = tailOrder;
};

