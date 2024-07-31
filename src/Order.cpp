#include "Order.h"

/**
 * @brief Constructs a new Order.
 * @param orderData The data associated with the order, including type, side, size, limit price, and timestamps.
 * @param parentLimit Pointer to the parent limit where this order resides.
 * @param idSequence Reference to the OrderIdSequence for generating a unique order ID.
 */
Order::Order(const OrderData& orderData, Limit* parentLimit, OrderIdSequence& idSequence)
    : orderData(orderData), parentLimit(parentLimit), nextOrder(nullptr), prevOrder(nullptr) {
          
    if (orderData.limit <= 0) {
        throw std::invalid_argument("The price must be positive");
    }
    if (orderData.shares <= 0) {
        throw std::invalid_argument("The order size must be positive");
    }

    orderId = idSequence.getNextId();
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
    orderData.shares = shares;
}

/**
 * @brief Returns the price limit of the order.
 * @return Price limit.
 */
int Order::getLimit() const {
    return orderData.limit.value();
}

/**
 * @brief Returns the side of the order (buy or sell).
 * @return Order side.
 */
Side Order::getOrderSide() const {
    return orderData.orderSide;
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
    return orderData.entryTime;
}

/**
 * @brief Returns the event time of the order.
 * @return Event time.
 */
int Order::getEventTime() const {
    return orderData.eventTime;
}

/**
 * @brief Returns the number of shares for the order.
 * @return Number of shares.
 */
int Order::getShares() const {
    return orderData.shares;
}

/**
 * @brief Returns the type of the order.
 * @return The type of the order (Limit or Market).
 */
OrderType Order::getOrderType() const {
    return orderData.orderType;
}

/**
 * @brief Returns the order ID.
 * @return Order ID.
 */
int64_t Order::getOrderId() const {
    return orderId;
}
