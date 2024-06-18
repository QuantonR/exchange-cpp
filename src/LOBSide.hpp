#ifndef LOBSIDE_HPP
#define LOBSIDE_HPP

#include <map>
#include <memory>
#include "Limit.h"
#include "Side.hpp"


template<Side S>
class LOBSide {
private:
    std::map<int, std::unique_ptr<Limit>> sideTree;
    int sideVolume;
    Limit* bestLimit;
    
public:
    LOBSide();
    
    Limit* findLimit(int limitPrice) const;
    std::unique_ptr<Order> addOrderToSide(int limitPrice, int volume);
    void updateBestLimit();
    void insertLimitToVect(Limit* limit);
    void placeMarketOrder(int volume, std::vector<int>& executeOrderIds);
    void executeOrder(int& volume, Limit*& LimitToExecute, std::vector<int>& executeOrderIds);
    
    static int getCurrentTimeSeconds();
    
    LOBSide(const LOBSide&) = delete;
    LOBSide& operator=(const LOBSide&) = delete;
    
    Limit* getBestLimit() const;
    int getSideVolume() const;
};

template<Side S>
LOBSide<S>::LOBSide() : sideVolume(0), bestLimit(nullptr) {}

template<Side S>
std::unique_ptr<Order> LOBSide<S>::addOrderToSide(int limitPrice, int volume) {
    sideVolume += volume;
    Limit* limitToAdd = findLimit(limitPrice);
    if (!limitToAdd) {
        auto newLimit = std::make_unique<Limit>(limitPrice);
        limitToAdd = newLimit.get();
        sideTree.insert({limitPrice, std::move(newLimit)});
        updateBestLimit();
    }
    
    // Maybe error here: No order are added to the Limit.
    return limitToAdd -> addOrderToLimit(S, volume, getCurrentTimeSeconds());
}

template<Side S>
Limit* LOBSide<S>::findLimit(int limitPrice) const {
    auto result = sideTree.find(limitPrice);
    if (result == sideTree.end()) {
        return nullptr;
    } else {
        return result->second.get();
    }
}

template<Side S>
void LOBSide<S>::updateBestLimit() {
    if (sideTree.empty()) {
        bestLimit = nullptr;
    } else {
        if constexpr (S == Side::Buy) {
            bestLimit = sideTree.rbegin()->second.get();  // Highest price for buy side
        } else if constexpr (S == Side::Sell) {
            bestLimit = sideTree.begin()->second.get();   // Lowest price for sell side
        }
    }
}

template<Side S>
void LOBSide<S>::placeMarketOrder(int volume, std::vector<int>& executeOrderIds){
    
    if (volume > sideVolume){
        throw std::runtime_error("The market order size is too big and it can't be executed right now.");
    }
    
    Limit* limitToExecute = bestLimit;
    
    if (limitToExecute == nullptr){
        throw std::runtime_error("No corrisponding orders available to match the market order.");
    }
    
    while (volume > 0 && limitToExecute){
        executeOrder(volume, limitToExecute, executeOrderIds);
    }
}

template<Side S>
void LOBSide<S>::executeOrder(int& volume, Limit*& limitToExecute, std::vector<int>& executeOrderIds){
    
    const int limitVolume = limitToExecute -> getTotalVolume();
    if (limitVolume > volume) {
        limitToExecute -> partialFill(volume);
        volume = 0;
    } else {
        limitToExecute->fullFill(executeOrderIds);
        volume -= limitVolume;

        // Erase the current limit
        sideTree.erase(limitToExecute->getLimitPrice());
        
        // Update best limit before erasing
        updateBestLimit();
        
        // Update limitToExecute to the new best limit
        limitToExecute = bestLimit;
    }
}

template<Side S>
int LOBSide<S>::getCurrentTimeSeconds() {
    return static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

template<Side S>
Limit* LOBSide<S>::getBestLimit() const{
    
    return bestLimit;
}

template <Side S>
int LOBSide<S>::getSideVolume() const{
    
    return sideVolume;
}

#endif // LOBSIDE_HPP
