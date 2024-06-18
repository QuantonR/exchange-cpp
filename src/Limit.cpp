#include "Limit.h"

// Constructor
Limit::Limit(int limitPrice)
    : limitPrice(limitPrice), size(0), totalVolume(0),
      headOrder(nullptr), tailOrder(nullptr) {}

std::unique_ptr<Order> Limit::addOrderToLimit(Side orderType, int orderShares, int entryTime) {
    
    // This will create a new Order and add it to the Limit
    auto newOrder = std::make_unique<Order>(orderType, orderShares, limitPrice, entryTime, this);
    Order* newOrderPtr = newOrder.get();
    
    // Increment totalVolume and size for the Limit
    totalVolume += orderShares;
    size += 1;

    if (!headOrder) {
        // If this is the first order, set head and tail to this order
        headOrder = newOrderPtr;
        tailOrder = headOrder;
    } else {
        newOrderPtr->setPrevOrder(tailOrder); // Link the new order with the current tail
        tailOrder->setNextOrder(newOrderPtr); // Link the current tail with the new order
        tailOrder = newOrderPtr; // tailOrder now points to the new order
    }
    
    return newOrder;
}

void Limit::partialFill(int remainingVolume){
    
    totalVolume -= remainingVolume;
    while (remainingVolume > 0 && headOrder){
        Order* order = getHeadOrder();
        int orderShares = order->getShares();

        if (remainingVolume >= orderShares){
            
            remainingVolume -= orderShares;
            size -= 1;
            Order* nxtOrder = order -> getNextOrder();
            headOrder = nxtOrder;
            if (headOrder){
                headOrder -> setPrevOrder(nullptr);
            } else {
                tailOrder = nullptr;
            }
        } else {
            order -> setShares(orderShares - remainingVolume);
            remainingVolume = 0;
        }
    }
}

void Limit::fullFill(std::vector<int>& executeOrderIds){
    
    while (headOrder) {
        executeOrderIds.push_back(headOrder->getOrderId());
        Order* nxtOrder = headOrder->getNextOrder();
        headOrder = nxtOrder;
        if (headOrder) {
            headOrder->setPrevOrder(nullptr);
        } else {
            tailOrder = nullptr;
        }
    }
    size = 0;
    totalVolume = 0;
}

// Getters & Setters
int Limit::getLimitPrice() const {
    return limitPrice;
}

int Limit::getSize() const {
    return size;
}

int Limit::getTotalVolume() const {
    return totalVolume;
}

Order* Limit::getHeadOrder() const {
    return headOrder;
}

Order* Limit::getTailOrder() const {
    return tailOrder;
}

void Limit::setTotalVolume(const int& newVolume){
    totalVolume=newVolume;
}
