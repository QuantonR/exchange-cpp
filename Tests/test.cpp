#include <gtest/gtest.h>
#include "../src/Book.h"
#include <chrono>

class OrderBookTest : public ::testing::Test {
protected:
    Book orderBook; // Assuming your order book class is named Book

    void SetUp() override {
        // Initialize your orderBook here, if needed
        orderBook = Book(); // Assuming Book has a default constructor
    }

    void TearDown() override {
        // Clean up any necessary resources. This is run after each test case.
    }
};

TEST_F(OrderBookTest, AddingBuyOrderIncreasesSize) {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    int now_seconds = static_cast<int>(now_c);

    orderBook.addLimitOrder(true, 3, now_seconds, now_seconds, 25); // Assuming you have a method to get the size
    // orderBook.addOrder(/* parameters for a buy order */);
    int newSize = orderBook.getBuySize();
    ASSERT_EQ(newSize, initialSize + 1);
}

TEST_F(OrderBookTest, AddingLowerSellOrderUpdatesLowestSell) {
    // Assuming you add a method to add an order and return a bool if it updates lowestSell
    // bool updated = orderBook.addOrder(/* parameters for a lower sell order */);
    ASSERT_TRUE(updated);
}

TEST_F(OrderBookTest, AddingOrderToEmptyBook) {
    // orderBook.addOrder(/* parameters for a buy order */);
    ASSERT_NE(orderBook.getHighestBuy(), 0); // Assuming 0 indicates no orders

    // orderBook.addOrder(/* parameters for a sell order */);
    ASSERT_NE(orderBook.getLowestSell(), 0); // Now there's a sell order
}

TEST_F(OrderBookTest, RemovingHighestBuyOrder) {
    // Add two buy orders, one higher than the other
    // orderBook.addOrder(/* parameters for a buy order with higher price */);
    // orderBook.addOrder(/* parameters for a buy order with lower price */);

    // Remove the higher buy order
    // orderBook.removeOrder(/* parameters to identify the higher buy order */);

    // Verify the highest buy is now set to the lower price
    ASSERT_EQ(orderBook.getHighestBuy(), /* expected lower price */);
}

TEST_F(OrderBookTest, OrderBookIntegrityAfterMultipleOperations) {
    // Add a series of buy and sell orders
    // orderBook.addOrder(/* parameters for buy orders */);
    // orderBook.addOrder(/* parameters for sell orders */);

    // Perform some removals
    // orderBook.removeOrder(/* parameters to identify an order to remove */);

    // Verify the integrity of the book, such as the correct highest buy and lowest sell
    // ASSERT_TRUE(orderBook.integrityCheck()); // Assuming you implement a method to check the book's integrity
}
