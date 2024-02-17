//
// Created by Riccardo on 17/02/2024.
//

#include "Order.h"

Order::Order(int idNumber, bool orderType, int shares, int limit, int entryTime, int eventTime, Order *nextOrder, Order *prevOrder,
             Limit *parentLimit) : idNumber(idNumber), orderType(orderType), shares(shares), limit(limit), entryTime(entryTime),
                                   eventTime(eventTime), nextOrder(nextOrder), prevOrder(prevOrder),
                                   parentLimit(parentLimit) {}

Order::Order() : idNumber(0), orderType(true), shares(0), limit(0), entryTime(0),
                 eventTime(0), nextOrder(nullptr), prevOrder(nullptr),
                 parentLimit(nullptr) {};


