#include "Book.h"


Book::Book() : buyTree(nullptr), sellTree(nullptr), lowestSell(nullptr), highestBuy(nullptr), buyTreeSize(0), sellTreeSize(0) {}

void Book::addOrderToBook(bool orderType, int orderVolume, float floatLimitPrice) {
    
    int limitPrice = static_cast<int>(std::round(floatLimitPrice * 100)); // I decided to use int prices so, given 1.24 it will get converted to 124

    // first thing we check if the new limit order crosses the spread.
    while ((lowestSell && limitPrice > lowestSell ->getLimitPrice() && orderType && orderVolume) || (highestBuy && limitPrice < highestBuy -> getLimitPrice() && !orderType && orderVolume)) {
        Limit* limitToExecute = orderType ? lowestSell : highestBuy;
        executeOrder(orderVolume, limitToExecute, orderType);
    }
    
    if (!orderVolume){
        return;
    }
    
    std::unique_ptr<Limit>& tree = orderType ? buyTree : sellTree;

    if (orderType){
        buyTreeSize += orderVolume;
    } else{
        sellTreeSize += orderVolume;
    }

    // Check if the limit already exists, and get or create a new limit as necessary.
    Limit* limit = findLimit(tree.get(), limitPrice);
    if (limit == nullptr) {
        limit = addLimitToTree(tree, nullptr, orderVolume, limitPrice, orderType);
    }
    // Add the order to the found or new limit.
    int entryTime = getCurrentTimeSeconds();
    limit->addOrderToLimit(orderType, orderVolume, entryTime);
    updateAfterAddingLimit(limit, orderType);
}

Limit* Book::findLimit(Limit* root, int limitPrice) const {
    Limit* current = root;
    while (current != nullptr) {
        if (current->getLimitPrice() == limitPrice) {
            // Found the limit with the specified price
            return current;
        } else if (limitPrice < current->getLimitPrice()) {
            // If the search price is less, go left
            current = current->getLeftChild();
        } else {
            // If the search price is greater, go right
            current = current->getRightChild();
        }
    }
    // If we reach here, no limit with the specified price exists in the tree
    return nullptr;
}

Limit* Book::addLimitToTree(std::unique_ptr<Limit>& tree, Limit* parent, int volume, int limitPrice, bool orderType) {
    if (!tree) {
        tree = std::make_unique<Limit>(limitPrice, parent);
        // Update highestBuy or lowestSell immediately after adding the new limit
        if (orderType) {
            highestBuy = (highestBuy && highestBuy->getLimitPrice() > limitPrice) ? highestBuy : tree.get();
        } else {
            lowestSell = (lowestSell && lowestSell->getLimitPrice() < limitPrice) ? lowestSell : tree.get();
        }
        return tree.get();
    } else {
        return tree->addLimit(volume, limitPrice, orderType);
    }
}

void Book::updateAfterAddingLimit(Limit* newLimit, bool isBuyOrder) {
    if (isBuyOrder && (!highestBuy || newLimit->getLimitPrice() > highestBuy->getLimitPrice())) {
        highestBuy = newLimit;
    } else if (!isBuyOrder && (!lowestSell || newLimit->getLimitPrice() < lowestSell->getLimitPrice())) {
        lowestSell = newLimit;
    }
}

void Book::placeMarketOrder(int volume, bool orderType){
    
    Limit* limitToExecute = orderType ? lowestSell : highestBuy;
    if (limitToExecute == nullptr){
        throw std::runtime_error("No corrisponding orders available to match the market order.");
    }
    int treeSize = orderType ? sellTreeSize : buyTreeSize;
    if (volume > treeSize){
        throw std::runtime_error("The market order size is too big and it can't be executed right now.");
    }
    if (orderType){
        sellTreeSize -= volume;
    } else {
        buyTreeSize -= volume;
    }
    while (volume > 0 && limitToExecute){
        // Todo: what happens if it the book is just one limit order and the placed market order cancel the full limit and there still are volumes in the market order? One solution could be to store the total volume of the buy and sell side and check if the market order volumes are more than those values
        executeOrder(volume, limitToExecute, orderType);
    }
}

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
Limit* Book::findNextLimit(Limit* currentLimit, bool orderType) {
    if (orderType) {
        // For buy orders, find the next highest buy limit
        if (currentLimit->getRightChild()) {
            currentLimit = currentLimit->getRightChild();
            while (currentLimit->getLeftChild()) {
                currentLimit = currentLimit->getLeftChild();
            }
            return currentLimit;
        } else {
            while (currentLimit->getParent() && currentLimit == currentLimit->getParent()->getRightChild()) {
                currentLimit = currentLimit->getParent();
            }
            return currentLimit->getParent();
        }
    } else {
        // For sell orders, find the next lowest sell limit
        if (currentLimit->getLeftChild()) {
            currentLimit = currentLimit->getLeftChild();
            while (currentLimit->getRightChild()) {
                currentLimit = currentLimit->getRightChild();
            }
            return currentLimit;
        } else {
            while (currentLimit->getParent() && currentLimit == currentLimit->getParent()->getLeftChild()) {
                currentLimit = currentLimit->getParent();
            }
            return currentLimit->getParent();
        }
    }
    return nullptr; // If there is no next limit
}

void Book::removeLimit(Limit* limit) {
    if (limit == highestBuy) {
        highestBuy = findNextLimit(limit, false);
        if (highestBuy == nullptr) {
            buyTree.reset();
        }
    }
    if (limit == lowestSell) {
        lowestSell = findNextLimit(limit, true);
        if (lowestSell == nullptr) {
            sellTree.reset();
        }
    }

    if (limit->getParent()) {
        Limit* parent = limit->getParent();
        if (parent->getLeftChild() == limit) {
            parent->setLeftChild(nullptr);
        } else if (parent->getRightChild() == limit) {
            parent->setRightChild(nullptr);
        }
    }
    limit = nullptr;
}

int Book::getCurrentTimeSeconds() const {
    return static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

Limit* Book::getBuyTree() const {
    return buyTree.get();
}

Limit* Book::getSellTree() const {
    return sellTree.get();
}

Limit* Book::getLowestSell() const {
    return lowestSell;
}

Limit* Book::getHighestBuy() const {
    return highestBuy;
}
