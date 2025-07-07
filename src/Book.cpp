#include "Book.h"

/**
 * @brief Default constructor for Book.
 *        Initializes an empty order book.
 */
Book::Book() = default;

/**
 * @brief Adds an order to the book, potentially executing against opposite orders.
 *
 * If the order is a market order, it is executed immediately against the opposite side.
 * If it's a limit order, any crossing volume is matched first, then any unfilled remainder is added as a limit order.
 *
 * @param side Side of the order (Buy or Sell).
 * @param shares Number of shares.
 * @param limit Limit price (ignored for market orders).
 * @param type Order type (Limit or Market).
 * @param idSeq Reference to order ID sequence generator.
 *
 * @throws std::invalid_argument if shares <= 0.
 */
void Book::addOrderToBook(Side side, int shares, float limit, OrderType type, OrderIdSequence& idSeq) {
    if (shares <= 0) {
        throw std::invalid_argument("Order size must be positive.");
    }
    const int price = (type == OrderType::Limit) ? static_cast<int>(std::round(limit * 100)) : -1;

    if (type == OrderType::Market) {
        // Market order: execute immediately
        if (side == Side::Buy) {
            sellSide.executeMarket(shares);
        } else {
            buySide.executeMarket(shares);
        }
        return;
    }

    // Limit order: try to cross first
    while (shares > 0) {
        Limit* bestOpposite = (side == Side::Buy) ? sellSide.getBestLimit() : buySide.getBestLimit();

        if (!bestOpposite)
            break;

        int bestOppositePrice = bestOpposite->getLimitPrice();

        bool crosses =
            (side == Side::Buy && price >= bestOppositePrice) ||
            (side == Side::Sell && price <= bestOppositePrice);

        if (!crosses)
            break;

        int volToExec = std::min(shares, bestOpposite->getTotalVolume());
        shares -= volToExec;

        if (side == Side::Buy) {
            sellSide.executeMatching(volToExec);
        } else {
            buySide.executeMatching(volToExec);
        }
    }

    // Place remainder as limit order
    if (shares > 0) {
        auto order = std::make_unique<Order>(side, shares, limit, type, nullptr, idSeq);
        Order* orderPtr = order.get();

        if (side == Side::Buy) {
            buySide.addOrder(price, orderPtr);
        } else {
            sellSide.addOrder(price, orderPtr);
        }

        allOrders.emplace(orderPtr->getOrderId(), std::move(order));
    }
}

/**
 * @brief Places a market order consuming available liquidity on the opposite side.
 *
 * @param volume Number of shares to execute.
 * @param side Side of the market order.
 */
void Book::placeMarketOrder(int volume, Side side) {
    if (side == Side::Buy) {
        sellSide.executeMarket(volume);
    } else {
        buySide.executeMarket(volume);
    }
}

/**
 * @brief Cancels an order from the book by its unique order ID.
 *
 * Updates both the Limit volume and the total volume of the side.
 *
 * @param orderId The unique order ID.
 *
 * @throws std::invalid_argument if orderId is not found.
 */
void Book::cancelOrder(int64_t orderId) {
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Order ID not found");
    }

    Order* order = it->second.get();
    int shares = order->getShares();
    int price = order->getLimit();
    Limit* parentLimit = order->getParentLimit();

    if (parentLimit) {
        parentLimit->adjustVolume(-shares);
    }

    if (order->getOrderSide() == Side::Buy) {
        buySide.removeOrder(price, order);
    } else {
        sellSide.removeOrder(price, order);
    }

    allOrders.erase(it);
}

/**
 * @brief Modifies the limit price of an existing order by canceling and re-adding.
 *
 * @param orderId The order ID to modify.
 * @param newLimitPrice New limit price.
 * @param idSeq Order ID generator for the new order.
 *
 * @throws std::invalid_argument if orderId is not found.
 */
void Book::modifyOrderLimitPrice(int64_t orderId, float newLimitPrice, OrderIdSequence& idSeq) {
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Order ID not found");
    }
    auto* order = it->second.get();
    Side side = order->getOrderSide();
    int shares = order->getShares();
    OrderType type = order->getOrderType();

    cancelOrder(orderId);
    addOrderToBook(side, shares, newLimitPrice, type, idSeq);
}

/**
 * @brief Modifies the size (volume) of an existing order.
 *
 * Updates the Limit volume and the side's total volume accordingly.
 *
 * @param orderId The order ID to modify.
 * @param newSize New number of shares.
 *
 * @throws std::invalid_argument if orderId is not found.
 */
void Book::modifyOrderSize(int64_t orderId, int newSize) {
    auto it = allOrders.find(orderId);
    if (it == allOrders.end()) {
        throw std::invalid_argument("Order ID not found");
    }
    auto* order = it->second.get();
    int oldSize = order->getShares();
    int delta = newSize - oldSize;

    order->setShares(newSize);

    Limit* parentLimit = order->getParentLimit();
    if (parentLimit) {
        parentLimit->adjustVolume(delta);
    }

    if (order->getOrderSide() == Side::Buy) {
        buySide.adjustTotalVolume(delta);
    } else {
        sellSide.adjustTotalVolume(delta);
    }
}

/**
 * @brief Adds an order to the internal order map.
 *
 * @param order Unique pointer to the Order to insert.
 */
void Book::addOrderToAllOrders(std::unique_ptr<Order> order) {
    allOrders.emplace(order->getOrderId(), std::move(order));
}

/**
 * @brief Removes an order from the internal order map.
 *
 * @param orderId The order ID to remove.
 */
void Book::removeOrderFromAllOrders(int64_t orderId) {
    allOrders.erase(orderId);
}

/**
 * @brief Returns a reference to the sell side (ask side) of the book.
 *
 * @return LOBSide<Side::Sell>&
 */
LOBSide<Side::Sell>& Book::getSellSide() {
    return sellSide;
}

/**
 * @brief Returns a reference to the buy side (bid side) of the book.
 *
 * @return LOBSide<Side::Buy>&
 */
LOBSide<Side::Buy>& Book::getBuySide() {
    return buySide;
}

/**
 * @brief Returns all active orders in the book.
 *
 * @return const reference to unordered_map of order IDs to Orders.
 */
const std::unordered_map<int64_t, std::unique_ptr<Order>>& Book::getAllOrders() const {
    return allOrders;
}
