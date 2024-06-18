#include "benchmark.hpp"

void Benchmark::addLimits(int numberOfLimitsToPlace) const{
    
    numberOfLimitsToPlace = numberOfLimitsToPlace / 10;
    for(int i=0; i<numberOfLimitsToPlace; ++i){
        
        if (i < numberOfLimitsToPlace / 2){
            // first place buy orders
            for(int j = 0; j < 10; ++j){
                orderBook->addOrderToBook(true, 50, i + 1);
            }
            if ((i + 1) % 100 == 0){
                // execute sell mkt orders
                orderBook->placeMarketOrder(50, false);  // Sell market order
            }
        } else {
            for(int j = 0; j < 10; ++j){
                orderBook->addOrderToBook(false, 50, i + 1);
            }
            if ((i + 1) % 100 == 0){
                orderBook->placeMarketOrder(50, true);  // Buy market order
            }
        }
    }
}


int main(){
    
    Benchmark("Adding 10 limits", 10);
    Benchmark("Adding 100 limits", 100);
    Benchmark("Adding 1000 limits", 1000);
    Benchmark("Adding 10000 limits", 10000);
    Benchmark("Adding 100000 limits", 100000);
    Benchmark("Adding 41460256 limits", 41460256);
    
    return;
}
