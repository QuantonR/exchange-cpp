#include "Book.h"


Book::Book() : sellSide(std::make_unique<LOBSide<Side::Sell>>()), buySide(std::make_unique<LOBSide<Side::Buy>>()) {}


template<Side S>
std::unique_ptr<Order> addOrderToSide(LOBSide<S>& side, int limitPrice, int volume) {
    return side.addOrderToSide(limitPrice, volume);
}

template<Side S>
void placeMktOrder(LOBSide<S>& side, int volume, std::vector<int>& executeOrderIds) {
    side.placeMarketOrder(volume, executeOrderIds);
}

void Book::addOrderToBook(bool orderType, int orderVolume, float floatLimitPrice) {
    
    // first thing we check if the new limit order crosses the spread.
//    while ((lowestSell && limitPrice > lowestSell ->getLimitPrice() && orderType && orderVolume) || (highestBuy && limitPrice < highestBuy -> getLimitPrice() && !orderType && orderVolume)) {
//        Limit* limitToExecute = orderType ? lowestSell : highestBuy;
//        executeOrder(orderVolume, limitToExecute, orderType);
//    }
    if (!orderVolume){
        return;
    }
    
    int limitPrice = static_cast<int>(std::round(floatLimitPrice * 100));
    std::unique_ptr<Order> newOrder;
    
    if (orderType) {
        newOrder = addOrderToSide(*buySide, limitPrice, orderVolume);
    } else {
        newOrder = addOrderToSide(*sellSide, limitPrice, orderVolume);
    }
    
    allOrders.insert({newOrder->getOrderId(), std::move(newOrder)});
}

void Book::placeMarketOrder(int volume, bool orderType){
  
    std::vector<int> executedOrderIds;
    
    if (orderType) {
        placeMktOrder(*sellSide, volume, executedOrderIds);
    } else {
        placeMktOrder(*buySide, volume, executedOrderIds);
    }
    
    // Remove executed orders from allOrders
    for (int orderId : executedOrderIds) {
        allOrders.erase(orderId);
    }
}

/*
void Book::executeOrder(int& remainingVolume, Limit*& limitToExecute, const bool& orderType){
    
    int limitVolume = limitToExecute -> getTotalVolume();
    if (remainingVolume >= limitVolume){
        remainingVolume -= limitVolume;
        Limit* nextLimit = findNextLimit(limitToExecute, orderType);
        removeLimit(limitToExecute);
        limitToExecute = nextLimit;
    } else {
        
        limitToExecute->partialFill(remainingVolume);
        remainingVolume = 0;
    }
}

//Limit* Book::findNextLimit(Limit* currentLimit, bool orderType) {
//    if (orderType) {
//        // For buy orders, find the next highest buy limit
//        if (currentLimit->getRightChild()) {
//            currentLimit = currentLimit->getRightChild();
//            while (currentLimit->getLeftChild()) {
//                currentLimit = currentLimit->getLeftChild();
//            }
//            return currentLimit;
//        } else {
//            while (currentLimit->getParent() && currentLimit == currentLimit->getParent()->getRightChild()) {
//                currentLimit = currentLimit->getParent();
//            }
//            return currentLimit->getParent();
//        }
//    } else {
//        // For sell orders, find the next lowest sell limit
//        if (currentLimit->getLeftChild()) {
//            currentLimit = currentLimit->getLeftChild();
//            while (currentLimit->getRightChild()) {
//                currentLimit = currentLimit->getRightChild();
//            }
//            return currentLimit;
//        } else {
//            while (currentLimit->getParent() && currentLimit == currentLimit->getParent()->getLeftChild()) {
//                currentLimit = currentLimit->getParent();
//            }
//            return currentLimit->getParent();
//        }
//    }
//    return nullptr; // If there is no next limit
//}
//
//void Book::removeLimit(Limit* limit) {
//    if (limit == highestBuy) {
//        highestBuy = findNextLimit(limit, false);
//        if (highestBuy == nullptr) {
//            buyTree.reset();
//        }
//    }
//    if (limit == lowestSell) {
//        lowestSell = findNextLimit(limit, true);
//        if (lowestSell == nullptr) {
//            sellTree.reset();
//        }
//    }
//
//    if (limit->getParent()) {
//        Limit* parent = limit->getParent();
//        if (parent->getLeftChild() == limit) {
//            parent->setLeftChild(nullptr);
//        } else if (parent->getRightChild() == limit) {
//            parent->setRightChild(nullptr);
//        }
//    }
//    limit = nullptr;
//}
//
//int Book::getCurrentTimeSeconds() const {
//    return static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
//}
*/

LOBSide<Side::Sell>* Book::getSellSide() const{
    
    return sellSide.get();
}

LOBSide<Side::Buy>* Book::getBuySide() const{
    
    return buySide.get();
}

