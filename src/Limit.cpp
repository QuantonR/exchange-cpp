#include "Limit.h"
#include "Book.h"

/**
 * @brief Constructs a new Limit object representing a price level in the order book.
 * @param limitPrice The price associated with this limit.
 */
Limit::Limit(int limitPrice)
    : limitPrice(limitPrice), size(0), totalVolume(0),
      headOrder(nullptr), tailOrder(nullptr) {}

/**
 * @brief Adds an order to this limit and updates the order book.
 * @param orderData The data associated with the order.
 * @param book Reference to the order book, used for updating the global order list.
 * @param idSequence Reference to the OrderIdSequence for generating a unique order ID.
 */
void Limit::addOrderToLimit(const OrderData& orderData, Book& book, OrderIdSequence& idSequence) {
    // This will create a new Order and add it to the Limit
    auto newOrder = std::make_unique<Order>(orderData, this, idSequence);
    Order* newOrderPtr = newOrder.get();

    // Increment totalVolume and size for the Limit
    totalVolume += orderData.shares;
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

    // Use book.addOrderToAllOrders to update the allOrders map
    book.addOrderToAllOrders(std::move(newOrder));
}

/**
 * @brief Partially fills orders at this limit until the remaining volume is zero or no more orders are left.
 * @param remainingVolume The volume that still needs to be filled.
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
 * @brief Fully fills all orders at this limit and removes them from the order book.
 * @param book Reference to the order book, used for removing orders from the order map.
 */
void Limit::fullFill(Book& book) {
    while (headOrder && tailOrder) {
        Order* nxtOrder = headOrder->getNextOrder();

        if (nxtOrder) {
            nxtOrder->setPrevOrder(nullptr);
        }
        
        // Use book.removeOrderFromAllOrders to update the allOrders map
        book.removeOrderFromAllOrders(headOrder->getOrderId());
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

/**
 * @brief Returns the price level of this limit.
 * @return The limit price.
 */
int Limit::getLimitPrice() const {
    return limitPrice;
}

/**
 * @brief Returns the number of orders at this limit.
 * @return The number of orders (size).
 */
int Limit::getSize() const {
    return size;
}

/**
 * @brief Returns the total volume of shares at this limit.
 * @return The total volume of shares.
 */
int Limit::getTotalVolume() const {
    return totalVolume;
}

/**
 * @brief Returns the first order in the linked list at this limit.
 * @return Pointer to the head order.
 */
Order* Limit::getHeadOrder() const {
    return headOrder;
}

/**
 * @brief Returns the last order in the linked list at this limit.
 * @return Pointer to the tail order.
 */
Order* Limit::getTailOrder() const {
    return tailOrder;
}

/**
 * @brief Sets the total volume of shares at this limit.
 * @param newVolume The new total volume to set.
 */
void Limit::setTotalVolume(const int& newVolume) {
    totalVolume = newVolume;
}

/**
 * @brief Sets the last order in the linked list at this limit.
 * @param newTailOrder Pointer to the new tail order.
 */
void Limit::setTailOrder(Order* newTailOrder) {
    tailOrder = newTailOrder;
}

/**
 * @brief Sets the first order in the linked list at this limit.
 * @param newHeadOrder Pointer to the new head order.
 */
void Limit::setHeadOrder(Order* newHeadOrder) {
    headOrder = newHeadOrder;
}
