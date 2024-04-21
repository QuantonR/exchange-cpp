//
// Created by Riccardo on 17/02/2024.
//

#include "Limit.h"


Limit::Limit(int limitPrice, int size, Limit *parent) : limitPrice(limitPrice), size(0), totalVolume(0), parent(parent), leftChild(nullptr), rightChild(nullptr),
    headOrder(nullptr), tailOrder(nullptr){};

void Limit::addOrder(bool orderType, int size, int entryTime, int eventType) {
    totalVolume += size;
    this->size += 1;
    std::unique_ptr<Order> newOrder = std::make_unique<Order>(orderType, size, this->limitPrice, entryTime, eventType, this);
    if (!tailOrder) {
        headOrder = std::move(newOrder);
        tailOrder = headOrder.get();
    } else {
        newOrder->setPrevOrder(tailOrder);
        tailOrder->setNextOrder(newOrder.get());
        tailOrder = newOrder.release();
    }
};

void Limit::addLimit(Limit *parent, bool orderType, int size, int entryTime, int eventType, int limitPrice){

    setLimitPrice(limitPrice);
    setParent(parent);
    if (getLimitPrice() > parent->getLimitPrice()) {
        parent->setRightChild(this);
    } else {
        parent->setLeftChild(this);
    }
    addOrder(orderType, size, entryTime, eventType);
}

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
}

void Limit::setLimitPrice(int limitPrice) {
    Limit::limitPrice = limitPrice;
}

void Limit::setParent(Limit *parent) {
    Limit::parent = parent;
}

void Limit::setLeftChild(Limit *leftChild) {
    Limit::leftChild = leftChild;
}

void Limit::setRightChild(Limit *rightChild) {
    Limit::rightChild = rightChild;
}

Limit *Limit::getLeftChild() const {
    return leftChild;
}

Limit *Limit::getRightChild() const {
    return rightChild;
}
