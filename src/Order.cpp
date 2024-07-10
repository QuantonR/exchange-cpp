#include "Order.h"

int64_t Order::globalOrderId = 0;

/**
 * @brief Constructs a new Order.
 * @param orderType Type of the order.
 * @param shares Number of shares.
 * @param limit Price limit.
 * @param entryTime Time of entry.
 * @param parentLimit Pointer to the parent limit.
 */
Order::Order(Side orderType, int shares, int limit, int entryTime, Limit* parentLimit)
    : orderType(orderType), shares(shares), limit(limit),
      entryTime(entryTime), parentLimit(parentLimit), eventTime(entryTime), nextOrder(nullptr), prevOrder(nullptr) {
          
    if (limit <= 0) {
        throw std::invalid_argument("The price must be positive");
    }
    if (shares <= 0) {
        throw std::invalid_argument("The order size must be positive");
    }

    orderId = updateId();
}

/**
 * @brief Updates the global order ID.
 * @return New order ID.
 */
int64_t Order::updateId() {
    return globalOrderId++;
}

/**
 * @brief Sets the next order in the list.
 * @param nextOrder Pointer to the next order.
 */
void Order::setNextOrder(Order* nextOrder) {
    this->nextOrder = nextOrder;
}

/**
 * @brief Sets the previous order in the list.
 * @param prevOrder Pointer to the previous order.
 */
void Order::setPrevOrder(Order* prevOrder) {
    this->prevOrder = prevOrder;
}

/**
 * @brief Sets the number of shares for the order.
 * @param shares Number of shares.
 */
void Order::setShares(const int shares) {
    this->shares = shares;
}

/**
 * @brief Returns the price limit of the order.
 * @return Price limit.
 */
int Order::getLimit() const {
    return limit;
}

/**
 * @brief Returns the type of the order.
 * @return Order type (buy or sell).
 */
Side Order::getOrderType() const {
    return orderType;
}

/**
 * @brief Returns the next order in the list.
 * @return Pointer to the next order.
 */
Order* Order::getNextOrder() const {
    return nextOrder;
}

/**
 * @brief Returns the previous order in the list.
 * @return Pointer to the previous order.
 */
Order* Order::getPrevOrder() const {
    return prevOrder;
}

/**
 * @brief Returns the parent limit of the order.
 * @return Pointer to the parent limit.
 */
Limit* Order::getParentLimit() const {
    return parentLimit;
}

/**
 * @brief Returns the entry time of the order.
 * @return Entry time.
 */
int Order::getEntryTime() const {
    return entryTime;
}

/**
 * @brief Returns the event time of the order.
 * @return Event time.
 */
int Order::getEventTime() const {
    return eventTime;
}

/**
 * @brief Returns the number of shares for the order.
 * @return Number of shares.
 */
int Order::getShares() const {
    return shares;
}

/**
 * @brief Returns the order ID.
 * @return Order ID.
 */
int64_t Order::getOrderId() const {
    return orderId;
}

/**
 * @brief Resets the global order ID.
 */
void Order::resetGlobalOrderId() {
    globalOrderId = 0;
}
