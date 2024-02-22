//
// Created by Riccardo on 17/02/2024.
//

#include "Book.h"

Book::Book(Limit *buyTree, Limit *sellTree, Limit *lowestSell, Limit *highestBuy) : buyTree(buyTree),
                                                                                    sellTree(sellTree),
                                                                                    lowestSell(lowestSell),
                                                                                    highestBuy(highestBuy) {};

Book::Book() : buyTree(nullptr),
               sellTree(nullptr),
               lowestSell(nullptr),
               highestBuy(nullptr){};

