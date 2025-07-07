#include "Limit.h"
#include "Order.h"

/**
 * @brief Constructs a Limit representing a specific price level.
 * @param price The price level for this limit (in cents).
 */
Limit::Limit(int32_t price)
    : limitPrice(price), totalVolume(0), size(0), head(nullptr), tail(nullptr)
{}

/**
 * @brief Adds an order to this limit level.
 *        The order is appended to the end of the linked list.
 * @param order Pointer to the order to add.
 */
void Limit::addOrder(Order* order) {
    totalVolume += order->getShares();
    order->setPrevOrder(tail);
    order->setNextOrder(nullptr);
    if (tail)
        tail->setNextOrder(order);
    else
        head = order;
    tail = order;
    size += 1;
}

/**
 * @brief Removes an order from this limit level.
 *        Updates linked list pointers accordingly.
 * @param order Pointer to the order to remove.
 */
void Limit::removeOrder(Order* order) {
    size -= 1;
    if (order->getPrevOrder())
        order->getPrevOrder()->setNextOrder(order->getNextOrder());
    else
        head = order->getNextOrder();

    if (order->getNextOrder())
        order->getNextOrder()->setPrevOrder(order->getPrevOrder());
    else
        tail = order->getPrevOrder();
}

/**
 * @brief Adjusts the total volume at this price level.
 * @param delta The change in volume (positive or negative).
 */
void Limit::adjustVolume(int delta) {
    totalVolume += delta;
}

/**
 * @brief Checks if this limit has any orders.
 * @return True if no orders are present, false otherwise.
 */
bool Limit::empty() const {
    return head == nullptr;
}

/**
 * @brief Returns the total volume at this limit level.
 * @return Sum of all order shares at this limit.
 */
int32_t Limit::getTotalVolume() const {
    return totalVolume;
}

/**
 * @brief Returns the price level represented by this limit.
 * @return The price (in cents).
 */
int32_t Limit::getLimitPrice() const {
    return limitPrice;
}

/**
 * @brief Returns the number of orders in this limit.
 * @return Number of orders.
 */
int32_t Limit::getSize() const {
    return size;
}

/**
 * @brief Returns the first order in the linked list.
 * @return Pointer to the head order.
 */
Order* Limit::getHead() const {
    return head;
}

/**
 * @brief Returns the last order in the linked list.
 * @return Pointer to the tail order.
 */
Order* Limit::getTail() const {
    return tail;
}
