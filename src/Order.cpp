#include "Order.h"

Order::Order() : idNumber("0"), orderType(true), shares(0), limit(0), entryTime(0),
                 eventTime(0), nextOrder(nullptr), prevOrder(nullptr),
                 parentLimit(nullptr) {}

Order::Order(bool orderType, int shares, int limit, int entryTime, Limit *parentLimit) :
                idNumber(Order::generateUuid()), orderType(orderType), shares(shares), limit(limit),
                entryTime(entryTime), eventTime(entryTime), nextOrder(nullptr), prevOrder(nullptr), parentLimit(parentLimit){
                    
                    if (limit <= 0){
                        throw std::invalid_argument("The price must be positive");
                    }
                    if (shares <= 0){
                        throw std::invalid_argument("The order size must be positive");
                    }
                }

const std::string Order::generateUuid() {
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    return boost::uuids::to_string(uuid);
}

void Order::setNextOrder(Order *nextOrder) {
    this->nextOrder = nextOrder;
}

void Order::setPrevOrder(Order *prevOrder) {
    this->prevOrder = prevOrder;
};

void Order::setShares(const int shares){
    this->shares = shares;
}

int Order::getLimit() const {
    return limit;
};

bool Order::getOrderType() const {

    return orderType;
};

Order* Order::getNextOrder() const {

    return nextOrder;
};

Order* Order::getPrevOrder() const {

    return prevOrder;
};

Limit* Order::getParentLimit() const {

    return parentLimit;
};

int Order::getEntryTime() const {

    return entryTime;
};

int Order::getEventTime() const {

    return eventTime;
};

int Order::getShares() const {

    return shares;
};
