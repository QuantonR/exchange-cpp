#include "Limit.h"

// Constructor
Limit::Limit(int limitPrice, Limit* parent)
    : limitPrice(limitPrice), size(0), totalVolume(0), parent(parent),
      leftChild(nullptr), rightChild(nullptr), headOrder(nullptr), tailOrder(nullptr) {}

void Limit::addOrder(bool orderType, int orderShares, int entryTime, int eventTime) {
    
    // This will create a new Order and add it to the Limit
    auto newOrder = std::make_unique<Order>(orderType, orderShares, limitPrice, entryTime, eventTime, this);

    // Increment totalVolume and size for the Limit
    totalVolume += orderShares;
    size += 1;

    if (!headOrder) {
        // If this is the first order, set head and tail to this order
        headOrder = std::move(newOrder);
        tailOrder = headOrder.get();
    } else {
        newOrder->setPrevOrder(tailOrder); // Link the new order with the current tail
        tailOrder->setNextOrder(newOrder.get()); // Link the current tail with the new order
        tailOrder = newOrder.release(); // tailOrder now points to the new order, release ownership from newOrder
    }
}

Limit* Limit::addLimit(int size, int limitPrice, bool orderType) {
    if (limitPrice < this->limitPrice) {
        if (!leftChild) {
            leftChild = std::make_unique<Limit>(limitPrice, this);
            return leftChild.get();
        } else {
            return leftChild->addLimit(size, limitPrice, orderType);
        }
    } else if (limitPrice > this->limitPrice) {
        if (!rightChild) {
            rightChild = std::make_unique<Limit>(limitPrice, this);
            return rightChild.get();
        } else {
            return rightChild->addLimit(size, limitPrice, orderType);
        }
    }
    // If the limit price is equal, we do not add a new limit; we just return this limit.
    return this;
}

// Getter implementations
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
    return headOrder.get();
}

Order* Limit::getTailOrder() const {
    return tailOrder;
}

void Limit::setLeftChild(std::unique_ptr<Limit> left) {
    leftChild = std::move(left);
}

void Limit::setRightChild(std::unique_ptr<Limit> right) {
    rightChild = std::move(right);
}

Limit* Limit::getLeftChild() const {
    return leftChild.get();
}

Limit* Limit::getRightChild() const {
    return rightChild.get();
}

std::unique_ptr<Limit>& Limit::getLeftUniquePtr() {
    return leftChild;
}

std::unique_ptr<Limit>& Limit::getRightUniquePtr() {
    return rightChild;
}
