#pragma once

#include <cstdint>

class Order;

/**
 * @class Limit
 * @brief Represents a price level in the order book, managing orders at that specific price level.
 */
class Limit {
public:
    explicit Limit(int32_t limitPrice);

    void addOrder(Order* order);
    void removeOrder(Order* order);
    bool empty() const;
    int32_t getTotalVolume() const;
    int32_t getLimitPrice() const;
    int32_t getSize() const;
    Order* getHead() const;
    Order* getTail() const;
    void adjustVolume(int delta);

private:
    const int32_t limitPrice;
    int32_t totalVolume;
    int32_t size;
    Order* head;
    Order* tail;
};
