#include "Book.h"
#include "Exchange.hpp"
#include "LOBSide.hpp"

/**
 * @brief Constructor that initializes the buy and sell sides of the order book.
 */
Book::Book(Exchange& exchange, std::string symbol) : exchange(exchange), symbol(symbol), sellSide(std::make_unique<LOBSide<Side::Sell>>(*this)), buySide(std::make_unique<LOBSide<Side::Buy>>(*this)) {}

/**
 * @brief Template function to add an order to the correct side of the order book.
 * @tparam S The side of the order (buy or sell).
 * @param side Reference to the side of the order book.
 * @param orderData Reference to the order data containing the order details.
 */
template<Side S>
void addOrderToSide(LOBSide<S>& side, OrderData& orderData, uint64_t newOrderId) {
    side.addOrderToSide(orderData, newOrderId);
}

/**
 * @brief Template function to place a market order on the correct side of the order book.
 * @tparam S The side of the order execution.
 * @param side Reference to the side of the order book.
 * @param volume Volume of the market order.
 */
template<Side S>
void placeMktOrder(LOBSide<S>& side, OrderData& orderData, uint64_t marketOrderId) {
    side.placeMarketOrder(orderData, marketOrderId);
}

/**
 * @brief Adds an order to the order book, placing it on the correct side and executing against opposing orders if necessary.
 * @param orderData Reference to the order data containing the order details.
 */
void Book::addOrderToBook(OrderData orderData) {
    
    Limit* bestLimitOppositeSide = (orderData.orderSide == Side::Buy) ? sellSide->getBestLimit() : buySide->getBestLimit();
    
    uint64_t newOrderId = getNextOrderId();

    // Check if the new limit order crosses the spread. If so, start executing the order until it stops crossing the spread
    while (bestLimitOppositeSide &&
           ((orderData.orderSide == Side::Buy && sellSide && sellSide->getBestLimit() && orderData.limit > sellSide->getBestLimit()->getLimitPrice()) ||
            (orderData.orderSide == Side::Sell && buySide && buySide->getBestLimit() && orderData.limit < buySide->getBestLimit()->getLimitPrice()))) {
        // TODO: I am having issues with implementign this for orders with the same clientID. I want to loop throught all the limits in the order book in that case and check at the end if it is the same limit price as the min (buy) or max (sell) and if so close the loop
        if (orderData.orderSide == Side::Buy) {
            sellSide->executeOrder(orderData, newOrderId, bestLimitOppositeSide);
        } else {
            buySide->executeOrder(orderData, newOrderId, bestLimitOppositeSide);
        }
        if (!orderData.shares) return; // Exit the loop if the order volume is completely executed
    }

    if (orderData.orderSide == Side::Buy) {
        addOrderToSide(*buySide, orderData, newOrderId);
    } else {
        addOrderToSide(*sellSide, orderData, newOrderId);
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
 * @brief Adds an execution to the execution queue.
 * @param execution A unique_ptr to the Execution object to be added to the queue.
 */
void Book::addExecutionToQueue(std::unique_ptr<Execution> execution){
    exchange.addExecutionToQueue(std::move(execution));
}

/**
 * @brief Places a market order, executing it against the existing limit orders on the opposing side.
 * @param volume Volume of the market order.
 * @param orderSide The side of the market order (buy or sell).
 */
void Book::placeMarketOrder(OrderData& orderData) {
    
    uint64_t newOrderId = getNextOrderId();
    if (orderData.orderSide == Side::Buy) {
        placeMktOrder(*sellSide, orderData, newOrderId);
    } else {
        placeMktOrder(*buySide, orderData, newOrderId);
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
 * @throws std::invalid_argument if the order ID is not found in the book.
 */
void Book::modifyOrderLimitPrice(int64_t orderId, float newLimitPrice) {
    
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Invalid order to modify: the order is not in the Book");
    }

    // Access order safely
    auto orderToModify = it->second.get();

    OrderData modifiedOrderData = OrderData(orderToModify->getOrderSide(), orderToModify->getShares(), orderToModify->getClientId(), newLimitPrice, orderToModify->getOrderType());
    
    // Cancel the order and ensure it does not leave a dangling pointer
    removeOrderFromLimit(orderToModify);
    
    // Add the modified order back to the book
    addOrderToBook(modifiedOrderData);
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
const std::unordered_map<uint64_t, std::unique_ptr<Order>>* Book::getAllOrders() const {
    return &allOrders;
}

uint64_t Book::getNextOrderId() {
    return exchange.getNextOrderId();
}

uint64_t Book::getNextExecutionId() {
    return exchange.getNextExecutionId();
}

std::string Book::getSymbol() const {
    return symbol;
}
