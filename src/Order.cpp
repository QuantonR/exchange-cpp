//
// Created by Riccardo on 17/02/2024.
//

#include "Order.h"

Order::Order() : idNumber("0"), orderType(true), shares(0), limit(0), entryTime(0),
                 eventTime(0), nextOrder(nullptr), prevOrder(nullptr),
                 parentLimit(nullptr) {}

Order::Order(bool orderType, int shares, int limit, int entryTime, int eventTime, Limit *parentLimit) :
                idNumber(Order::generateUuid()), orderType(orderType), shares(shares), limit(limit),
                entryTime(entryTime), eventTime(eventTime), nextOrder(nullptr), prevOrder(nullptr), parentLimit(parentLimit){}

const std::string Order::generateUuid() {
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    return boost::uuids::to_string(uuid);
}

void Order::setNextOrder(Order *nextOrder) {
    Order::nextOrder = nextOrder;
}

void Order::setPrevOrder(Order *prevOrder) {
    Order::prevOrder = prevOrder;
};


int Order::getLimit() const {
    return limit;
}

int getOrderType() const;

Order* getNextOrder() const;

Order* getPrevOrder() const;

Limit* getParentLimit() const;
