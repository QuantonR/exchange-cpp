#pragma once

#include "Order.h"
#include <memory>

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
private:
    const int limitPrice;
    int size;
    int totalVolume;
    Limit* parent; // Raw pointer as ownership is managed by the book
    std::unique_ptr<Limit> leftChild;
    std::unique_ptr<Limit> rightChild;
    std::unique_ptr<Order> headOrder; // Unique_ptr for automatic memory management
    Order* tailOrder; // Raw pointer for traversal without ownership

public:
    Limit(int limitPrice, Limit* parent); // Constructor

    void addOrderToLimit(bool orderType, int size, int entryTime); // Adds an order to this limit
    void partialFill(int remainingVolume); // Executes a partial fill of an order
    int getLimitPrice() const; // Getter for limit price
    int getSize() const; // Getter for size
    int getTotalVolume() const; // Getter for total volume
    
    Limit* getParent() const; // Getter for parent

    Order* getHeadOrder() const; // Getter for headOrder
    Order* getTailOrder() const; // Getter for tailOrder

    // Setters for smart pointer managed members
    void setLeftChild(std::unique_ptr<Limit> left);
    void setRightChild(std::unique_ptr<Limit> right);
    void setTotalVolume(const int& newVolume);
    
    void decreaseSize(const int& newSize);

    // Getters for smart pointer managed members (returns raw pointers for internal manipulation)
    Limit* getLeftChild() const;
    Limit* getRightChild() const;
    Limit* addLimit(int size, int limitPrice, bool orderType);
    
    std::unique_ptr<Limit>& getRightUniquePtr();
    std::unique_ptr<Limit>& getLeftUniquePtr();
};
