#include "../src/Book.h"
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <chrono>

using namespace std::chrono;

class OrderBookTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook; // Use a smart pointer for Book

    void SetUp() override {
        orderBook = std::make_unique<Book>(); // Reinitialize the Book object for each test
    }

    void TearDown() override {
        // Cleanup is managed by smart pointers
    }

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
    int volume = 3;

    orderBook->addLimitOrder(true, volume, nowSeconds, nowSeconds, orderPrice);

    Limit* highestBuy = orderBook->getHighestBuy();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), orderPrice);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), volume);
    EXPECT_EQ(headOrder->getEntryTime(), nowSeconds);
}

TEST_F(OrderBookTest, AddingFirstSellOrder) {
    int nowSeconds = getCurrentTimeSeconds();
    int orderPrice = 30;
    int insertedSize = 5;

    orderBook->addLimitOrder(false, insertedSize, nowSeconds, nowSeconds, orderPrice);

    Limit* lowestSell = orderBook->getLowestSell();
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
    orderBook->addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 20); // Lower buy price
    orderBook->addLimitOrder(false, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 35); // Higher sell price

    EXPECT_EQ(orderBook->getHighestBuy()->getLimitPrice(), 20);
    EXPECT_EQ(orderBook->getLowestSell()->getLimitPrice(), 35);
}

TEST_F(OrderBookTest, AddingOrderToExistingLimit) {
    int price = 25;
    orderBook->addLimitOrder(true, 3, getCurrentTimeSeconds(), getCurrentTimeSeconds(), price);
    orderBook->addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), price); // Same price

    Limit* limit = orderBook->getHighestBuy();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

TEST_F(OrderBookTest, AddingBestBuyAndSellOrders) {
    int now_time = getCurrentTimeSeconds();
    orderBook->addLimitOrder(true, 3, now_time, now_time, 25); // Initial best buy
    orderBook->addLimitOrder(false, 3, now_time, now_time, 30); // Initial best sell
    orderBook->addLimitOrder(true, 2, now_time, now_time, 26); // Better buy
    orderBook->addLimitOrder(false, 2, now_time, now_time, 29); // Better sell

    EXPECT_EQ(orderBook->getHighestBuy()->getLimitPrice(), 26);
    EXPECT_EQ(orderBook->getLowestSell()->getLimitPrice(), 29);
}

TEST_F(OrderBookTest, Adding3WorstBuyLimits){
    int now_time = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(true, 10, now_time, now_time, 10); // Best buy
    orderBook -> addLimitOrder(true, 2, now_time, now_time, 9); // Worst buy
    orderBook -> addLimitOrder(true, 5, now_time, now_time, 9); // Another worst buy

    auto buyTree = orderBook -> getBuyTree();
    EXPECT_EQ(buyTree->getLeftChild()->getLimitPrice(), 9);
    EXPECT_EQ(buyTree->getLeftChild()->getSize(), 2);
    EXPECT_EQ(buyTree->getLeftChild()->getTotalVolume(), 7);
    
    orderBook -> addLimitOrder(true, 10, now_time, now_time, 9); // Another worst buy

    EXPECT_EQ(buyTree->getLeftChild()->getSize(), 3);
    EXPECT_EQ(buyTree->getLeftChild()->getTotalVolume(), 17);
}

TEST_F(OrderBookTest, Adding3WorstSellLimits){
    int now_time = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(false, 30, now_time, now_time, 30); // Best sell
    orderBook -> addLimitOrder(false, 40, now_time, now_time, 31); // Worst sell
    orderBook -> addLimitOrder(false, 45, now_time, now_time, 31); // Another worst sell

    auto sellTree = orderBook -> getSellTree();
    EXPECT_EQ(sellTree->getRightChild()->getLimitPrice(), 31);
    EXPECT_EQ(sellTree->getRightChild()->getSize(), 2);
    EXPECT_EQ(sellTree->getRightChild()->getTotalVolume(), 85);
    
    orderBook -> addLimitOrder(false, 15, now_time, now_time, 31); // Another worst sell

    EXPECT_EQ(sellTree->getRightChild()->getSize(), 3);
    EXPECT_EQ(sellTree->getRightChild()->getTotalVolume(), 100);
}

TEST_F(OrderBookTest, NegativeSizeTest) {
    int nowTime = getCurrentTimeSeconds();
    EXPECT_THROW({
        orderBook->addLimitOrder(true, -30, nowTime, nowTime, 30); // Negative volume
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, NegativeLimitPrice) {
    int nowTime = getCurrentTimeSeconds();
    EXPECT_THROW({
        orderBook->addLimitOrder(true, 3, nowTime, nowTime, -1); // Negative price
    }, std::invalid_argument);
}
