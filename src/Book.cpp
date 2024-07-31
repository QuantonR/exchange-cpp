#include "Book.h"

/**
 * @brief Constructor that initializes the buy and sell sides of the order book.
 */
Book::Book() : sellSide(std::make_unique<LOBSide<Side::Sell>>(*this)), buySide(std::make_unique<LOBSide<Side::Buy>>(*this)) {}

/**
 * @brief Template function to add an order to the correct side of the order book.
 * @tparam S The side of the order (buy or sell).
 * @param side Reference to the side of the order book.
 * @param orderData Reference to the order data containing the order details.
 * @param orderIdSequence Reference to the OrderIdSequence for generating a unique order ID.
 */
template<Side S>
void addOrderToSide(LOBSide<S>& side, OrderData orderData, OrderIdSequence& orderIdSequence) {
    side.addOrderToSide(orderData, orderIdSequence);
}

/**
 * @brief Template function to place a market order on the correct side of the order book.
 * @tparam S The side of the order execution.
 * @param side Reference to the side of the order book.
 * @param volume Volume of the market order.
 */
template<Side S>
void placeMktOrder(LOBSide<S>& side, int volume) {
    side.placeMarketOrder(volume);
}

/**
 * @brief Adds an order to the order book, placing it on the correct side and executing against opposing orders if necessary.
 * @param orderData Reference to the order data containing the order details.
 * @param orderIdSequence Reference to the OrderIdSequence for generating a unique order ID.
 */
void Book::addOrderToBook(OrderData orderData, OrderIdSequence& orderIdSequence) {
    
    Limit* bestLimitOppositeSide = (orderData.orderSide == Side::Buy) ? sellSide->getBestLimit() : buySide->getBestLimit();

    // Check if the new limit order crosses the spread. If so, start executing the order until it stops crossing the spread
    while (bestLimitOppositeSide &&
           ((orderData.orderSide == Side::Buy && sellSide && sellSide->getBestLimit() && orderData.limit > sellSide->getBestLimit()->getLimitPrice()) ||
            (orderData.orderSide == Side::Sell && buySide && buySide->getBestLimit() && orderData.limit < buySide->getBestLimit()->getLimitPrice()))) {
        if (orderData.orderSide == Side::Buy) {
            sellSide->executeOrder(orderData.shares, bestLimitOppositeSide);
        } else {
            buySide->executeOrder(orderData.shares, bestLimitOppositeSide);
        }
        if (!orderData.shares) return; // Exit the loop if the order volume is completely executed
    }

    if (orderData.orderSide == Side::Buy) {
        addOrderToSide(*buySide, orderData, orderIdSequence);
    } else {
        addOrderToSide(*sellSide, orderData, orderIdSequence);
    }
}

/**
 * @brief Adds an order to the allOrders map.
 * @param order A unique_ptr to the Order object to be added to the map.
 */
void Book::addOrderToAllOrders(std::unique_ptr<Order> order) {
    allOrders.insert({order->getOrderId(), std::move(order)});
}

/**
 * @brief Places a market order, executing it against the existing limit orders on the opposing side.
 * @param volume Volume of the market order.
 * @param orderSide The side of the market order (buy or sell).
 */
void Book::placeMarketOrder(const int volume, Side orderSide) {
    
    if (orderSide == Side::Buy) {
        placeMktOrder(*sellSide, volume);
    } else {
        placeMktOrder(*buySide, volume);
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
        if (orderToCancel->getOrderSide() == Side::Buy) {
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
 * @brief Removes an order from the internal map of all orders in the book by its ID.
 * @param orderId The ID of the order that needs to be removed from the map.
 */
void Book::removeOrderFromAllOrders(int64_t orderId) {
    allOrders.erase(orderId);
}

/**
 * @brief Cancels an order in the order book by its ID, removing it from the corresponding limit and the internal order map.
 * @param orderId ID of the order to be canceled.
 * @throws std::invalid_argument if the order ID is not found in the book.
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
 * @brief Modifies the limit price of an order by canceling it and re-adding it with the new price.
 * @param orderId ID of the order to be modified.
 * @param newLimitPrice The new limit price for the order.
 * @param orderIdSequence Reference to the OrderIdSequence for generating a new unique order ID.
 * @throws std::invalid_argument if the order ID is not found in the book.
 */
void Book::modifyOrderLimitPrice(int64_t orderId, float newLimitPrice, OrderIdSequence& orderIdSequence) {
    
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Invalid order to modify: the order is not in the Book");
    }

    // Access order safely
    auto orderToModify = it->second.get();

    OrderData modifiedOrderData = OrderData(orderToModify->getOrderSide(), orderToModify->getShares(), newLimitPrice, orderToModify->getOrderType());
    
    // Cancel the order and ensure it does not leave a dangling pointer
    removeOrderFromLimit(orderToModify);
    
    // Add the modified order back to the book
    addOrderToBook(modifiedOrderData, orderIdSequence);
}

/**
 * @brief Modifies the size (volume) of an order in the order book.
 * @param orderId ID of the order to be modified.
 * @param newSize The new size (volume) for the order.
 * @throws std::invalid_argument if the order ID is not found in the book.
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
