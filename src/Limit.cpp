#include "Limit.h"

// Constructor
Limit::Limit(int limitPrice, int size, Limit* parent)
    : limitPrice(limitPrice), size(size), totalVolume(0), parent(parent),
      leftChild(nullptr), rightChild(nullptr), headOrder(nullptr), tailOrder(nullptr) {}

// Adds an order
void Limit::addOrder(bool orderType, int size, int entryTime, int eventType) {
    totalVolume += size;
    this->size++;

    // Make a new order and set it at the end of the list
    auto newOrder = std::make_unique<Order>(orderType, size, limitPrice, entryTime, eventType, this);
    if (!tailOrder) {
        headOrder = std::move(newOrder); // New order becomes the head and the tail since it's the first order
        tailOrder = headOrder.get();
    } else {
        newOrder->setPrevOrder(tailOrder); // Set the new order's previous order to the current tail
        tailOrder->setNextOrder(newOrder.get()); // Set current tail's next order to the new order
        tailOrder = newOrder.release(); // The new order is now the last order, release it from unique_ptr
    }
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
