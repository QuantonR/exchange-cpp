//
// Created by Riccardo on 17/02/2024.
//

#ifndef EXCHANGE_CPP_LIMIT_H
#define EXCHANGE_CPP_LIMIT_H

#include "Order.h"
class Order;
class Limit {

    /* Here is an overview of the variables in the limit class:
     *  -limitPrice: The price of this limit in the order book. It is used to match limit orders that meets or exceed this price and to reorganizing the buy and sell tree in the book class.
     *  -size: it is the number of individual Order instances linked to this limit. Useful for understanding the depth of the market at this price and for iterating over orders.
     *  -totalVolume: represent the total quantity of shares or units available at this price. Used for calculating the potential impact of large trades and for providing market depth information.
     *  -parent, leftChild, rightChild: These pointers are used to embed the Limit instance with a binary tree structure. This is essential for having good insertion, deletion and traversal time complexity.
     *  -headOrder, tailOrder: these two pointers serves as entry points to the doubly linked list of order. This structure allows for FIFO order execution. It is used for adding new orders to the end of the list and for
     *  executing or removing orders from the front.
     */

    int limitPrice;
    int size;
    int totalVolume;
    Limit *parent;
    Limit *leftChild;
    Limit *rightChild;
    Order *headOrder;
    Order *tailOrder;

public:
    Limit();

    int getLimitPrice() const;

    void setHeadOrder(Order *headOrder);

    void setTailOrder(Order *tailOrder);

    int getSize() const;

    int getTotalVolume() const;

    Order *getHeadOrder() const;

    Order *getTailOrder() const;

    Limit(int limitPrice, int size, int totalVolume, Limit *parent);
};


#endif //EXCHANGE_CPP_LIMIT_H
