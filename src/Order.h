//
// Created by Riccardo on 17/02/2024.
//

#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Limit.h"

class Limit;
class Order {
    /* Here is an overview of the variables in the Order class:
     * - idNumber, buyOrSell, shares, limit, entryTime, eventTime: These variables store the essential details of an individual order such as: its unique identifier (idNumber), the type of the order (buy or sell) (orderType),
     *  the quantity (shares), the price (limit) and the timestamps (entryTime for when the order is submitted and eventTime for the last time any event (like modifications, partial fills or cancellations affected the order).
     * -nextOrder, prevOrder: These are pointers that link the Order instances together. This is critical for order execution logic, ensuring that orders are processed in the correct sequence.
     */
    const std::string idNumber;
    bool orderType;
    int shares;
    int limit;
    int entryTime;
    int eventTime;
    Order *nextOrder;
    Order *prevOrder;
    Limit *parentLimit;

public:

    Order(bool orderType, int shares, int limit, int entryTime, int eventTime, Limit *parentLimit);

    Order();

    static const std::string generateUuid();

    void setNextOrder(Order *nextOrder);

    void setPrevOrder(Order *prevOrder);

    int getLimit() const;

    int getOrderType() const;

    Order* getNextOrder() const;

    Order* getPrevOrder() const;

    Limit* getParentLimit() const;

    int getEntryTime() const;

    int getEventTime() const;
    
    int getShares() const;
};
