#include "Limit.h"

// Constructor
Limit::Limit(int limitPrice)
    : limitPrice(limitPrice), size(0), totalVolume(0),
      headOrder(nullptr), tailOrder(nullptr) {}

/**
 * @brief Adds an order to this limit.
 * @param orderType Type of the order.
 * @param orderShares Number of shares for the order.
 * @param entryTime Entry time of the order.
 * @param allOrders Reference to the map of all orders.
 */
void Limit::addOrderToLimit(Side orderType, int orderShares, int entryTime, std::unordered_map<int64_t, std::unique_ptr<Order>>& allOrders) {
    
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

    allOrders.insert({newOrderPtr->getOrderId(), std::move(newOrder)});
}

/**
 * @brief Partially fills an order at this limit.
 * @param remainingVolume Volume to be filled.
 */
void Limit::partialFill(int remainingVolume) {
    
    totalVolume -= remainingVolume;
    while (remainingVolume > 0 && headOrder) {
        Order* order = getHeadOrder();
        int orderShares = order->getShares();

        if (remainingVolume >= orderShares) {
            remainingVolume -= orderShares;
            decreaseSize();
            Order* nxtOrder = order->getNextOrder();
            headOrder = nxtOrder;
            if (headOrder) {
                headOrder->setPrevOrder(nullptr);
            } else {
                tailOrder = nullptr;
            }
        } else {
            order->setShares(orderShares - remainingVolume);
            remainingVolume = 0;
        }
    }
}

/**
 * @brief Fully fills the limit and removes all orders.
 * @param allOrders Reference to the map of all orders.
 */
void Limit::fullFill(std::unordered_map<int64_t, std::unique_ptr<Order>>& allOrders) {
    
    while (headOrder && tailOrder) {
        Order* nxtOrder = headOrder->getNextOrder();

        if (nxtOrder) {
            nxtOrder->setPrevOrder(nullptr);
        }

        allOrders.erase(headOrder->getOrderId());
        headOrder = nullptr;
        headOrder = nxtOrder;
    }

    size = 0;
    totalVolume = 0;
}

/**
 * @brief Decreases the size of the limit.
 */
void Limit::decreaseSize() {
    
    if (size > 0) {
        size -= 1;
    }
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

void Limit::setTotalVolume(const int& newVolume) {
    totalVolume = newVolume;
}

void Limit::setTailOrder(Order* newTailOrder) {
    tailOrder = newTailOrder;
}

void Limit::setHeadOrder(Order* newHeadOrder) {
    headOrder = newHeadOrder;
}
