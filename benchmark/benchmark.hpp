#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <memory>
#include "../src/Book.h"

class Benchmark{
    
    std::chrono::high_resolution_clock::time_point start;
    std::string name;
    std::unique_ptr<Book> orderBook;
    
public:
    Benchmark(std::string name, const int numberOfLimitsToPlace): orderBook(std::make_unique<Book>()),name(name), start(std::chrono::high_resolution_clock::now()){
        
        addLimits(numberOfLimitsToPlace);
    }
    ~Benchmark(){
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        std::cout << "Benchmark \"" << name << "\" completed in " << duration << " nanoseconds\n";
    }
    
    void addLimits(const int numberOfLimitsToPlace) const;
};
