#include "Order.h"

/**
 * @brief Utility to get current timestamp in seconds.
 * @return Current time as an integer timestamp.
 */
inline int getCurrentTimeSeconds() {
    return static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

/**
 * @brief Constructs a new Order.
 * @param orderSide The side of the order (buy or sell).
 * @param shares The number of shares.
 * @param limit The limit price (in float), or -1 for market.
 * @param orderType The order type (Limit or Market).
 * @param parentLimit Pointer to the parent Limit object.
 * @param idSequence Sequence generator to create a unique order ID.
 * @throws std::invalid_argument if the order parameters are invalid.
 */
Order::Order(Side orderSide, int shares, float limit, OrderType orderType, Limit* parentLimit, OrderIdSequence& idSequence)
    : orderSide(orderSide),
      shares(shares),
      limit(static_cast<int>(std::round(limit * 100))),
      orderType(orderType),
      entryTime(getCurrentTimeSeconds()),
      eventTime(getCurrentTimeSeconds()),
      parentLimit(parentLimit),
      nextOrder(nullptr),
      prevOrder(nullptr)
{
    if ((limit <= 0 && orderType == OrderType::Limit) || (limit != -1 && orderType == OrderType::Market)) {
        throw std::invalid_argument("Incorrect limit price");
    }
    if (shares <= 0) {
        throw std::invalid_argument("The order size must be positive");
    }
    orderId = idSequence.getNextId();
}

/**
 * @brief Sets the pointer to the next order in the linked list.
 * @param nextOrder Pointer to the next order.
 */
void Order::setNextOrder(Order* nextOrder) {
    this->nextOrder = nextOrder;
}

/**
 * @brief Sets the pointer to the previous order in the linked list.
 * @param prevOrder Pointer to the previous order.
 */
void Order::setPrevOrder(Order* prevOrder) {
    this->prevOrder = prevOrder;
}

/**
 * @brief Sets the number of shares for the order.
 * @param shares New number of shares.
 */
void Order::setShares(const int shares) {
    this->shares = shares;
}

/**
 * @brief Returns the limit price of the order.
 * @return Price limit as integer (in cents).
 */
int Order::getLimit() const {
    return limit;
}

/**
 * @brief Returns the side of the order.
 * @return Side (Buy or Sell).
 */
Side Order::getOrderSide() const {
    return orderSide;
}

/**
 * @brief Returns a pointer to the next order.
 * @return Pointer to the next order.
 */
Order* Order::getNextOrder() const {
    return nextOrder;
}

/**
 * @brief Returns a pointer to the previous order.
 * @return Pointer to the previous order.
 */
Order* Order::getPrevOrder() const {
    return prevOrder;
}

/**
 * @brief Returns the parent Limit object.
 * @return Pointer to the parent Limit.
 */
Limit* Order::getParentLimit() const {
    return parentLimit;
}

/**
 * @brief Returns the entry timestamp of the order.
 * @return Entry time in seconds since epoch.
 */
int Order::getEntryTime() const {
    return entryTime;
}

/**
 * @brief Returns the timestamp of the last event for this order.
 * @return Event time in seconds since epoch.
 */
int Order::getEventTime() const {
    return eventTime;
}

/**
 * @brief Returns the current number of shares.
 * @return Number of shares.
 */
int Order::getShares() const {
    return shares;
}

/**
 * @brief Returns the type of the order.
 * @return Order type (Limit or Market).
 */
OrderType Order::getOrderType() const {
    return orderType;
}

/**
 * @brief Returns the unique order ID.
 * @return Order ID.
 */
int64_t Order::getOrderId() const {
    return orderId;
}

/**
 * @brief Sets the parent Limit object for this order.
 * @param parentLimit Pointer to the parent Limit.
 */
void Order::setParentLimit(Limit* parentLimit) {
    this->parentLimit = parentLimit;
}
