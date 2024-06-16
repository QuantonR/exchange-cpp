#include "Order.h"

int Order::orderId = 0;


Order::Order(Side orderType, int shares, int limit, int entryTime, Limit *parentLimit) :
                orderType(orderType), shares(shares), limit(limit),
                entryTime(entryTime), parentLimit(parentLimit), eventTime(entryTime), nextOrder(nullptr), prevOrder(nullptr) {
                    
                    if (limit <= 0){
                        throw std::invalid_argument("The price must be positive");
                    }
                    if (shares <= 0){
                        throw std::invalid_argument("The order size must be positive");
                    }
                    
                    updateId();
                }

int Order::updateId(){
    
    orderId += 1;
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

Side Order::getOrderType() const {

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

int Order::getOrderId() const{
 
    return orderId;
}
