#include "Book.h"


Book::Book() : sellSide(std::make_unique<LOBSide<Side::Sell>>()), buySide(std::make_unique<LOBSide<Side::Buy>>()) {}

/**
 * @brief Adds an order to the correct side.
 * @param side The side of the order (buy or sell)
 * @param volume Volume of the order.
 * @param allOrders Reference to the map of all orders.
 */
template<Side S>
void addOrderToSide(LOBSide<S>& side, int limitPrice, int volume, std::unordered_map<int64_t, std::unique_ptr<Order>>& allOrders) {
    side.addOrderToSide(limitPrice, volume, allOrders);
}

/**
 * @brief Places a market order on the correct side.
 * @param side The side of the order execution.
 * @param volume Volume of the order.
 * @param allOrders Reference to the map of all orders.
 */
template<Side S>
void placeMktOrder(LOBSide<S>& side, int volume, std::unordered_map<int64_t, std::unique_ptr<Order>>& allOrders) {
    side.placeMarketOrder(volume, allOrders);
}

/**
 * @brief Adds an order to the order book.
 * @param orderSide Type of the order (true for buy, false for sell).
 * @param orderVolume Volume of the order.
 * @param floatLimitPrice Price of the order.
 */
void Book::addOrderToBook(bool orderSide, int orderVolume, float floatLimitPrice) {
    
    int limitPrice = static_cast<int>(std::round(floatLimitPrice * 100));

    Limit* bestLimitOppositeSide = (orderSide) ? sellSide->getBestLimit() : buySide->getBestLimit();

    // Check if the new limit order crosses the spread. If so, start executing the order until it stops crossing the spread
    while (bestLimitOppositeSide &&
           ((orderSide && sellSide && sellSide->getBestLimit() && limitPrice > sellSide->getBestLimit()->getLimitPrice()) ||
            (!orderSide && buySide && buySide->getBestLimit() && limitPrice < buySide->getBestLimit()->getLimitPrice()))) {
        if (orderSide) {
            sellSide->executeOrder(orderVolume, bestLimitOppositeSide, allOrders);
        } else {
            buySide->executeOrder(orderVolume, bestLimitOppositeSide, allOrders);
        }
        if (!orderVolume) return; // Exit the loop if the order volume is completely executed
    }

    if (orderSide) {
        addOrderToSide(*buySide, limitPrice, orderVolume, allOrders);
    } else {
        addOrderToSide(*sellSide, limitPrice, orderVolume, allOrders);
    }
}

/**
 * @brief Places a market order.
 * @param volume Volume of the market order.
 * @param orderSide Type of the order (true for buy, false for sell).
 */
void Book::placeMarketOrder(int volume, bool orderSide) {
    
    if (orderSide) {
        placeMktOrder(*sellSide, volume, allOrders);
    } else {
        placeMktOrder(*buySide, volume, allOrders);
    }
}

/**
 * @brief Removes an order from its limit.
 * @param orderToCancel Pointer to the order to be removed.
 */
void Book::removeOrderFromLimit(Order* orderToCancel) {
    
    Order* nxtOrder = orderToCancel->getNextOrder();
    Order* prevOrder = orderToCancel->getPrevOrder();
    Limit* parent = orderToCancel->getParentLimit();

    bool isOnlyOrder = (!nxtOrder && !prevOrder);
    bool isLastOrder = (!nxtOrder && prevOrder);
    bool isFirstOrder = (nxtOrder && !prevOrder);
    bool isMiddleOrder = (nxtOrder && prevOrder);

    if (isOnlyOrder) {
        // Order to cancel is the only order in the limit
        if (orderToCancel->getOrderType() == Side::Buy) {
            buySide->cancelLimit(parent);
        } else {
            sellSide->cancelLimit(parent);
        }
    } else if (isLastOrder) {
        // Order to cancel is the last one in the limit
        prevOrder->setNextOrder(nullptr);
        parent->setTailOrder(prevOrder);
    } else if (isFirstOrder) {
        // Order to cancel is the first order in the limit
        nxtOrder->setPrevOrder(nullptr);
        parent->setHeadOrder(nxtOrder);
    } else if (isMiddleOrder) {
        // Order to cancel is in the middle of the limit
        nxtOrder->setPrevOrder(prevOrder);
        prevOrder->setNextOrder(nxtOrder);
    }

    // Update the limit's size and total volume
    if (!isOnlyOrder) {
        parent->decreaseSize();
        parent->setTotalVolume(parent->getTotalVolume() - orderToCancel->getShares());
    }
}

/**
 * @brief Cancels an order by its ID.
 * @param orderId ID of the order to be canceled.
 */
void Book::cancelOrder(int64_t orderId) {
    
    auto pairToCancel = allOrders.find(orderId);
    if (pairToCancel == allOrders.end()) {
        throw std::invalid_argument("Invalid order to cancel: the order is not in the Book");
    }

    auto orderToCancel = pairToCancel->second.get();
    removeOrderFromLimit(orderToCancel);
    allOrders.erase(orderId);  // Ensure this happens after the order is fully unlinked
}

/**
 * @brief Modifies the limit price of an order.
 * @param orderId ID of the order to be modified.
 * @param newLimit New limit price.
 */
void Book::modifyOrderLimitPrice(int64_t orderId, float newLimitPrice) {
    
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Invalid order to modify: the order is not in the Book");
    }

    // Access order safely
    auto orderToModify = it->second.get();

    int orderSize = orderToModify->getShares();
    Side orderSide = orderToModify->getOrderType();
    bool Side = (orderSide == Side::Sell) ? false : true;

    // Cancel the order and ensure it does not leave a dangling pointer
    removeOrderFromLimit(orderToModify);

    // Add the modified order back to the book
    addOrderToBook(Side, orderSize, newLimitPrice);
}

/**
 * @brief Modifies the size of an order.
 * @param orderId ID of the order to be modified.
 * @param newSize New size of the order.
 */
void Book::modifyOrderSize(int64_t orderId, int newSize) {
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Invalid order to modify: the order is not in the Book");
    }
    auto orderToModify = it->second.get();
    int oldSize = orderToModify->getShares();
    orderToModify->setShares(newSize);

    Limit* parent = orderToModify->getParentLimit();
    parent->setTotalVolume(parent->getTotalVolume() - oldSize + newSize);
}

/**
 * @brief Returns the sell side of the order book.
 * @return Pointer to the sell side.
 */
LOBSide<Side::Sell>* Book::getSellSide() const {
    return sellSide.get();
}

/**
 * @brief Returns the buy side of the order book.
 * @return Pointer to the buy side.
 */
LOBSide<Side::Buy>* Book::getBuySide() const {
    return buySide.get();
}

/**
 * @brief Returns all orders in the order book.
 * @return Pointer to the unordered map containing all orders.
 */
const std::unordered_map<int64_t, std::unique_ptr<Order>>* Book::getAllOrders() const {
    return &allOrders;
}
