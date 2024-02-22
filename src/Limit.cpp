//
// Created by Riccardo on 17/02/2024.
//

#include "Limit.h"

Limit::Limit(int limitPrice, int size, int totalVolume, Limit *parent, Limit *leftChild, Limit *rightChild,
             Order *headOrder, Order *tailOrder) : limitPrice(limitPrice), size(size), totalVolume(totalVolume),
                                                   parent(parent), leftChild(leftChild), rightChild(rightChild),
                                                   headOrder(headOrder), tailOrder(tailOrder) {};

Limit::Limit() : limitPrice(0), size(0), totalVolume(0),
                 parent(nullptr), leftChild(nullptr), rightChild(nullptr),
                 headOrder(nullptr), tailOrder(nullptr) {};

