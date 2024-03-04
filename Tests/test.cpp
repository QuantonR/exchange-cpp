//
// Created by Riccardo on 03/03/2024.
//
#include <gtest/gtest.h>
#include "Book.h" // Make sure to include the path to your Book class

class OrderBookTest : public ::testing::Test {
protected:
    Book orderBook; // Assuming your order book class is named Book

    void SetUp() override {
        orderBook = Book();
    }

    void TearDown() override {
        // Clean up any necessary resources. This is run after each test case.
    }
};
