//
// Created by Riccardo on 17/02/2024.
//

#include "Limit.h"

Limit::Limit(int limitPrice, int size, int totalVolume, Limit *parent, Limit *leftChild, Limit *rightChild,
             Order *headOrder, Order *tailOrder) : limitPrice(limitPrice), size(size), totalVolume(totalVolume),
                                                   parent(parent), leftChild(leftChild), rightChild(rightChild),
                                                   headOrder(headOrder), tailOrder(tailOrder) {};

Limit::Limit() : limitPrice(0), size(0), totalVolume(0),
                 parent(nullptr), leftChild(nullptr), rightChild(nullptr),
                 headOrder(nullptr), tailOrder(nullptr) {}

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

