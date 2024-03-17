//
// Created by Riccardo on 17/02/2024.
//

#include "Limit.h"


Limit::Limit(int limitPrice, int size, Limit *parent) : limitPrice(limitPrice), size(1), totalVolume(size), parent(parent), leftChild(nullptr), rightChild(nullptr),
    headOrder(nullptr), tailOrder(nullptr){};

void Limit::addOrder(bool orderType, int size, int entryTime, int eventType){

    this -> totalVolume += size;
    this -> size += 1;
    Order* newOrder = new Order(orderType, size , this -> getLimitPrice(), entryTime, eventType, this);
    auto nextOrder = this->getTailOrder();
    if (nextOrder == nullptr) {
        this->setTailOrder(newOrder);
        this->setHeadOrder(newOrder);
    } else {
        newOrder->setPrevOrder(nextOrder);
        newOrder->setNextOrder(nullptr);
        this->setTailOrder(newOrder);
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
