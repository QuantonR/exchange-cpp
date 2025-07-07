#pragma once

#include <unordered_map>
#include <memory>
#include "LOBSide.hpp"
#include "Order.h"
#include "OrderIdSequence.h"

/**
 * @class Book
 * @brief Represents an order book that manages buy and sell orders, allowing for placing, canceling, and modifying orders.
 */
class Book {
public:
    Book();

    // adding limit orders
    void addOrderToBook(Side side, int shares, float limit, OrderType type, OrderIdSequence& idSeq);

    // placing market orders
    void placeMarketOrder(int volume, Side side);

    // canceling orders
    void cancelOrder(int64_t orderId);

    // modifying orders
    void modifyOrderLimitPrice(int64_t orderId, float newLimitPrice, OrderIdSequence& idSeq);
    void modifyOrderSize(int64_t orderId, int newSize);

    // internal order map management
    void addOrderToAllOrders(std::unique_ptr<Order> order);
    void removeOrderFromAllOrders(int64_t orderId);

    // getters
    LOBSide<Side::Sell>& getSellSide();
    LOBSide<Side::Buy>& getBuySide();
    const std::unordered_map<int64_t, std::unique_ptr<Order>>& getAllOrders() const;

private:
    LOBSide<Side::Sell> sellSide;
    LOBSide<Side::Buy> buySide;

    std::unordered_map<int64_t, std::unique_ptr<Order>> allOrders;

    // disable copying
    Book(const Book&) = delete;
    Book& operator=(const Book&) = delete;
};
