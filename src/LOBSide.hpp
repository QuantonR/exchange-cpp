#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "Limit.h"
#include "Order.h"
#include "Side.hpp"

/**
 * @class LOBSide
 * @brief Manages one side of the order book (buy or sell) using a bitmap for fast price lookup.
 * @tparam S The side of the book (Side::Buy or Side::Sell).
 */
template<Side S>
class LOBSide {
public:
    static constexpr int32_t MIN_PRICE = 0;
    static constexpr int32_t MAX_PRICE = 200000; // $2000.00 x100
    static constexpr int32_t RANGE = MAX_PRICE - MIN_PRICE + 1;
    static constexpr int32_t BITMAP_SIZE = (RANGE + 63) / 64;

    LOBSide();

    void addOrder(int32_t price, Order* order);
    void removeOrder(int32_t price, Order* order);
    void executeMarket(int32_t volume);
    void executeMatching(int32_t& volume);
    void adjustTotalVolume(int delta);

    Limit* getLimit(int32_t price);
    Limit* getBestLimit();
    int32_t getBestPrice() const;
    int32_t getTotalVolume() const;

private:
    std::vector<Limit> limits;
    uint64_t bitmap[BITMAP_SIZE]{};
    int32_t totalVolume;
    int32_t bestPrice;

    void updateBestPrice();
};

/**
 * @brief Constructs the LOBSide object.
 *        Initializes all price levels (Limit objects) across the configured price range,
 *        reserves storage for performance, and sets the initial best price based on the side.
 */
template<Side S>
LOBSide<S>::LOBSide()
    : totalVolume(0),
      bestPrice(S == Side::Buy ? -1 : MAX_PRICE + 1)
{
    limits.reserve(RANGE);
    for (int32_t i = 0; i < RANGE; ++i) {
        limits.emplace_back(i);
    }
}

/**
 * @brief Adds a new order at the specified price.
 *        Updates the bitmap and best price if necessary.
 * @param price The price level (in cents).
 * @param order Pointer to the order to add.
 */
template<Side S>
void LOBSide<S>::addOrder(int32_t price, Order* order) {
    auto& limit = limits[price];
    if (limit.empty()) {
        int chunk = price / 64;
        int bit = price % 64;
        bitmap[chunk] |= (1ULL << bit);
    }

    limit.addOrder(order);
    order->setParentLimit(&limit);
    totalVolume += order->getShares();

    if constexpr (S == Side::Buy) {
        if (price > bestPrice)
            bestPrice = price;
    } else {
        if (price < bestPrice)
            bestPrice = price;
    }
}

/**
 * @brief Removes an order from the specified price level.
 *        Updates the bitmap and best price if necessary.
 * @param price The price level (in cents).
 * @param order Pointer to the order to remove.
 */
template<Side S>
void LOBSide<S>::removeOrder(int32_t price, Order* order) {
    auto& limit = limits[price];
    limit.removeOrder(order);
    totalVolume -= order->getShares();

    if (limit.empty()) {
        int chunk = price / 64;
        int bit = price % 64;
        bitmap[chunk] &= ~(1ULL << bit);
        updateBestPrice();
    }
}

/**
 * @brief Returns a pointer to the limit object at the given price.
 * @param price The price level.
 * @return Pointer to Limit.
 */
template<Side S>
Limit* LOBSide<S>::getLimit(int32_t price) {
    return &limits[price];
}

/**
 * @brief Returns a pointer to the current best limit (top of book).
 * @return Pointer to best Limit or nullptr if the book is empty.
 */
template<Side S>
Limit* LOBSide<S>::getBestLimit() {
    if (bestPrice < MIN_PRICE || bestPrice > MAX_PRICE)
        return nullptr;
    return &limits[bestPrice];
}

/**
 * @brief Returns the current best price on this side of the book.
 * @return Best price (in cents).
 */
template<Side S>
int32_t LOBSide<S>::getBestPrice() const {
    return bestPrice;
}

/**
 * @brief Returns the total volume of all orders on this side.
 * @return Total volume.
 */
template<Side S>
int32_t LOBSide<S>::getTotalVolume() const {
    return totalVolume;
}

/**
 * @brief Updates the best price after orders are removed.
 */
template<Side S>
void LOBSide<S>::updateBestPrice() {
    bestPrice = S == Side::Buy ? -1 : MAX_PRICE + 1;

    if constexpr (S == Side::Buy) {
        for (int i = BITMAP_SIZE - 1; i >= 0; --i) {
            if (bitmap[i]) {
                int pos = 63 - __builtin_clzll(bitmap[i]);
                bestPrice = i * 64 + pos;
                return;
            }
        }
    } else {
        for (int i = 0; i < BITMAP_SIZE; ++i) {
            if (bitmap[i]) {
                int pos = __builtin_ctzll(bitmap[i]);
                bestPrice = i * 64 + pos;
                return;
            }
        }
    }
}

/**
 * @brief Executes a market order, consuming volume until fully filled or the book is empty.
 *        Throws if insufficient liquidity.
 * @param volume The desired volume to execute.
 */
template<Side S>
void LOBSide<S>::executeMarket(int32_t volume) {
    if (volume > totalVolume)
        throw std::runtime_error("Market order volume exceeds available liquidity.");

    int32_t originalVolume = volume;

    while (volume > 0 && bestPrice >= MIN_PRICE && bestPrice <= MAX_PRICE) {
        Limit& limit = limits[bestPrice];
        Order* order = limit.getHead();

        while (volume > 0 && order) {
            int orderShares = order->getShares();
            if (volume >= orderShares) {
                volume -= orderShares;
                totalVolume -= orderShares;
                limit.adjustVolume(-orderShares);

                Order* next = order->getNextOrder();
                limit.removeOrder(order);
                order = next;
            } else {
                order->setShares(orderShares - volume);
                totalVolume -= volume;
                limit.adjustVolume(-volume);
                volume = 0;
            }
        }

        if (limit.empty()) {
            int chunk = bestPrice / 64;
            int bit = bestPrice % 64;
            bitmap[chunk] &= ~(1ULL << bit);
            updateBestPrice();
        } else {
            break;
        }
    }
}

/**
 * @brief Executes matching volume against the best limit, decrementing the volume reference.
 * @param volume Reference to the remaining volume to fill (decrements as filled).
 */
template<Side S>
void LOBSide<S>::executeMatching(int32_t& volume) {
    if (volume > totalVolume) {
        throw std::runtime_error("Market order volume exceeds available liquidity.");
    }

    while (volume > 0 && bestPrice >= MIN_PRICE && bestPrice <= MAX_PRICE) {
        Limit& limit = limits[bestPrice];
        Order* order = limit.getHead();

        while (volume > 0 && order) {
            int orderShares = order->getShares();
            if (volume >= orderShares) {
                volume -= orderShares;
                totalVolume -= orderShares;
                limit.adjustVolume(-orderShares);

                Order* next = order->getNextOrder();
                limit.removeOrder(order);
                order = next;
            } else {
                order->setShares(orderShares - volume);
                totalVolume -= volume;
                limit.adjustVolume(-volume);
                volume = 0;
            }
        }

        if (limit.empty()) {
            int chunk = bestPrice / 64;
            int bit = bestPrice % 64;
            bitmap[chunk] &= ~(1ULL << bit);
            updateBestPrice();
        } else {
            break;
        }
    }
}

/**
 * @brief Adjusts the total volume by a given delta.
 * @param delta Positive or negative change in volume.
 */
template<Side S>
void LOBSide<S>::adjustTotalVolume(int delta) {
    totalVolume += delta;
}
