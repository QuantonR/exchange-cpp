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
    float orderPrice = 25.09;
    int intOrderPrice = 2509;
    int volume = 3;

    orderBook->addLimitOrder(true, volume, nowSeconds, nowSeconds, orderPrice);

    Limit* highestBuy = orderBook->getHighestBuy();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), intOrderPrice);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), volume);
    EXPECT_EQ(headOrder->getEntryTime(), nowSeconds);
}

TEST_F(OrderBookTest, AddingFirstSellOrder) {
    int nowSeconds = getCurrentTimeSeconds();
    float floatPrice = 30.052;
    int orderPrice = 3005;
    int insertedSize = 5;

    orderBook->addLimitOrder(false, insertedSize, nowSeconds, nowSeconds, floatPrice);

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
    orderBook->addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 20.05); // Lower buy price
    orderBook->addLimitOrder(false, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), 35.000); // Higher sell price

    EXPECT_EQ(orderBook->getHighestBuy()->getLimitPrice(), 2005);
    EXPECT_EQ(orderBook->getLowestSell()->getLimitPrice(), 3500);
}

TEST_F(OrderBookTest, AddingOrderToExistingLimit) {
    int price = 25.03;
    orderBook->addLimitOrder(true, 3, getCurrentTimeSeconds(), getCurrentTimeSeconds(), price);
    orderBook->addLimitOrder(true, 2, getCurrentTimeSeconds(), getCurrentTimeSeconds(), price); // Same price

    Limit* limit = orderBook->getHighestBuy();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

TEST_F(OrderBookTest, AddingBestBuyAndSellOrders) {
    int now_time = getCurrentTimeSeconds();
    orderBook->addLimitOrder(true, 3, now_time, now_time, 25.04); // Initial best buy
    orderBook->addLimitOrder(false, 3, now_time, now_time, 30.0002); // Initial best sell
    orderBook->addLimitOrder(true, 2, now_time, now_time, 26.02); // Better buy
    orderBook->addLimitOrder(false, 2, now_time, now_time, 29.14); // Better sell

    EXPECT_EQ(orderBook->getHighestBuy()->getLimitPrice(), 2602);
    EXPECT_EQ(orderBook->getLowestSell()->getLimitPrice(), 2914);
}

TEST_F(OrderBookTest, Adding3WorstBuyLimits){
    int now_time = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(true, 10, now_time, now_time, 10.04); // Best buy
    orderBook -> addLimitOrder(true, 2, now_time, now_time, 9.00); // Worst buy
    orderBook -> addLimitOrder(true, 5, now_time, now_time, 9.00); // Another worst buy

    auto buyTree = orderBook -> getBuyTree();
    EXPECT_EQ(buyTree->getLeftChild()->getLimitPrice(), 900);
    EXPECT_EQ(buyTree->getLeftChild()->getSize(), 2);
    EXPECT_EQ(buyTree->getLeftChild()->getTotalVolume(), 7);
    
    orderBook -> addLimitOrder(true, 10, now_time, now_time, 9.00); // Another worst buy

    EXPECT_EQ(buyTree->getLeftChild()->getSize(), 3);
    EXPECT_EQ(buyTree->getLeftChild()->getTotalVolume(), 17);
}

TEST_F(OrderBookTest, Adding3WorstSellLimits){
    int now_time = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(false, 30, now_time, now_time, 30.15); // Best sell
    orderBook -> addLimitOrder(false, 40, now_time, now_time, 31.12); // Worst sell
    orderBook -> addLimitOrder(false, 45, now_time, now_time, 31.12); // Another worst sell

    auto sellTree = orderBook -> getSellTree();
    EXPECT_EQ(sellTree->getRightChild()->getLimitPrice(), 3112);
    EXPECT_EQ(sellTree->getRightChild()->getSize(), 2);
    EXPECT_EQ(sellTree->getRightChild()->getTotalVolume(), 85);
    
    orderBook -> addLimitOrder(false, 15, now_time, now_time, 31.12); // Another worst sell

    EXPECT_EQ(sellTree->getRightChild()->getSize(), 3);
    EXPECT_EQ(sellTree->getRightChild()->getTotalVolume(), 100);
}

TEST_F(OrderBookTest, NegativeSizeTest) {
    int nowTime = getCurrentTimeSeconds();
    EXPECT_THROW({
        orderBook->addLimitOrder(true, -30.00, nowTime, nowTime, 30); // Negative volume
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, NegativeLimitPrice) {
    int nowTime = getCurrentTimeSeconds();
    EXPECT_THROW({
        orderBook->addLimitOrder(true, 3.15, nowTime, nowTime, -1); // Negative price
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, correctValueInOrder) {
    int nowTime = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(false, 30, nowTime, nowTime, 30.15);
    
    auto sellTree = orderBook -> getSellTree();
    auto order = sellTree->getHeadOrder();
    
    EXPECT_EQ(3015, order->getLimit());
    EXPECT_EQ(nowTime, order->getEntryTime());
    EXPECT_EQ(nowTime, order->getEventTime());
    EXPECT_EQ(30, order->getShares());
    EXPECT_EQ(false, order->getOrderType());
    
    int secondNowTime = getCurrentTimeSeconds();
    orderBook->addLimitOrder(false, 5, secondNowTime, secondNowTime, 30.15);
    sellTree = orderBook->getSellTree();
    order = sellTree->getHeadOrder();
    auto nextOrder = order->getNextOrder();
    
    EXPECT_EQ(3015, nextOrder->getLimit());
    EXPECT_EQ(false, nextOrder->getOrderType());
    EXPECT_EQ(5, nextOrder->getShares());
    EXPECT_EQ(secondNowTime, nextOrder->getEntryTime());
    EXPECT_EQ(secondNowTime, nextOrder->getEventTime());
}

TEST_F(OrderBookTest, LimitAddOrderTest){
    int nowTime = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(true, 30, nowTime, nowTime, 17.98);
    Limit* buyTree = orderBook->getBuyTree();
    
    int secondEntryTime = getCurrentTimeSeconds();
    buyTree->addOrder(true, 7, secondEntryTime, secondEntryTime);
    
    Order* addedOrder = buyTree->getHeadOrder()->getNextOrder();
    
    EXPECT_EQ(1798, addedOrder->getLimit());
    EXPECT_EQ(true, addedOrder->getOrderType());
    EXPECT_EQ(7, addedOrder->getShares());
    EXPECT_EQ(secondEntryTime, addedOrder->getEntryTime());
    EXPECT_EQ(secondEntryTime, addedOrder->getEventTime());
}

TEST_F(OrderBookTest, LimitAddLimitTest){
    int nowTime = getCurrentTimeSeconds();
    orderBook -> addLimitOrder(true, 5, nowTime, nowTime, 34.12);
    Limit* buyTree = orderBook->getBuyTree();
    
    buyTree->addLimit(15, 3410, true);
    Limit* newLimit = buyTree->getLeftChild();
}
