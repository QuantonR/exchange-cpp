#include "../src/Book.h"
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <chrono>

// Using namespace std::chrono for cleaner code
using namespace std::chrono;

class OrderBookTest : public ::testing::Test {
protected:
    Book orderBook; // Assuming your order book class is named Book

    void SetUp() override {
        orderBook = Book(); // Ensure a fresh state for each test
    }

    void TearDown() override {
        // Cleanup not necessary unless using dynamic allocation not managed by smart pointers
    }

    // Helper function to get current time as integer seconds
    int getCurrentTimeSeconds() const {
        return static_cast<int>(system_clock::to_time_t(system_clock::now()));
    }
};

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};

TEST_F(OrderBookTest, AddingFirstBuyOrder) {
    int nowSeconds = getCurrentTimeSeconds();
    int orderPrice = 25;
    int insertedSize = 3;

    orderBook.addLimitOrder(true, insertedSize, nowSeconds, nowSeconds, orderPrice);

    Limit* highestBuy = orderBook.getHighestBuy();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), orderPrice);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), insertedSize);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
    EXPECT_EQ(headOrder->getEntryTime(), nowSeconds);
}

TEST_F(OrderBookTest, AddingFirstSellOrder) {
    int nowSeconds = getCurrentTimeSeconds();
    int orderPrice = 30;
    int insertedSize = 5;

    orderBook.addLimitOrder(false, insertedSize, nowSeconds, nowSeconds, orderPrice);

    Limit* lowestSell = orderBook.getLowestSell();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell->getLimitPrice(), orderPrice);
    EXPECT_EQ(lowestSell->getSize(), 1);
    EXPECT_EQ(lowestSell->getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell->getHeadOrder();
    ASSERT_EQ(headOrder, lowestSell->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
    EXPECT_EQ(headOrder->getEntryTime(), nowSeconds);
}

TEST_F(OrderBookTest, AddingWorseBuyAndSellOrders) {
    orderBook.addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 20); // Lower buy price
    orderBook.addLimitOrder(false, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 35); // Higher sell price

    // Checking that the best buy and sell limits are still correct
    EXPECT_EQ(orderBook.getHighestBuy()->getLimitPrice(), 20);
    EXPECT_EQ(orderBook.getLowestSell()->getLimitPrice(), 35);
}

TEST_F(OrderBookTest, AddingOrderToExistingLimit) {
    int price = 25;
    orderBook.addLimitOrder(true, 3, getCurrentTimeSeconds(), getCurrentTimeSeconds(), price);
    orderBook.addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), price); // Same price

    Limit* limit = orderBook.getHighestBuy();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

TEST_F(OrderBookTest, AddingBestBuyAndSellOrders) {
    orderBook.addLimitOrder(true, 3, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 25); // Initial best buy
    orderBook.addLimitOrder(false, 3, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 30); // Initial best sell
    orderBook.addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 26); // Better buy
    orderBook.addLimitOrder(false, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 29); // Better sell

    EXPECT_EQ(orderBook.getHighestBuy()->getLimitPrice(), 26);
    EXPECT_EQ(orderBook.getLowestSell()->getLimitPrice(), 29);
}
