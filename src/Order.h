#pragma once

#include <stdexcept>
#include "Limit.h"
#include "Side.hpp"


class Limit;


class Order {
    /* Here is an overview of the variables in the Order class:
       - orderId, buyOrSell, shares, limit, entryTime, eventTime: These variables store the essential details of an individual order such as: its unique identifier (orderId), the type of the order (buy or sell) (orderType), the quantity (shares), the price (limit) and the timestamps (entryTime for when the order is submitted and eventTime for the last time any event (like modifications, partial fills or cancellations affected the order).
       -nextOrder, prevOrder: These are pointers that link the Order instances together. This is critical for order execution logic, ensuring that orders are processed in the correct sequence.
     */
    static int orderId;
    
    const Side orderType;
    int shares;
    const int limit;
    const int entryTime;
    int eventTime;
    Order *nextOrder;
    Order *prevOrder;
    Limit *parentLimit;

public:

    Order(Side orderType, int shares, int limit, int entryTime, Limit *parentLimit);

    static int updateId();

    void setNextOrder(Order *nextOrder);
    void setPrevOrder(Order *prevOrder);
    void setShares(const int shares);

    int getLimit() const;
    Side getOrderType() const;
    Order* getNextOrder() const;
    Order* getPrevOrder() const;
    Limit* getParentLimit() const;
    int getEntryTime() const;
    int getEventTime() const;
    int getShares() const;
    int getOrderId() const;
    
    Order& operator=(const Order&) = delete;
    Order(const Order&) = delete;
};
