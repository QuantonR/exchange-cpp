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
};

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};

TEST_F(OrderBookTest, AddingFirstBuyOrder) {
    float orderPrice = 25.09;
    int intOrderPrice = 2509;
    int volume = 3;

    orderBook->addOrderToBook(true, volume, orderPrice);

    Limit* highestBuy = orderBook->getHighestBuy();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), intOrderPrice);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), volume);
}

TEST_F(OrderBookTest, AddingFirstSellOrder) {
    float floatPrice = 30.052;
    int orderPrice = 3005;
    int insertedSize = 5;

    orderBook->addOrderToBook(false, insertedSize, floatPrice);

    Limit* lowestSell = orderBook->getLowestSell();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell->getLimitPrice(), orderPrice);
    EXPECT_EQ(lowestSell->getSize(), 1);
    EXPECT_EQ(lowestSell->getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell->getHeadOrder();
    ASSERT_EQ(headOrder, lowestSell->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
}

TEST_F(OrderBookTest, AddingWorseBuyAndSellOrders) {
    orderBook->addOrderToBook(true, 2, 20.05); // Lower buy price
    orderBook->addOrderToBook(false, 2, 35.000); // Higher sell price

    EXPECT_EQ(orderBook->getHighestBuy()->getLimitPrice(), 2005);
    EXPECT_EQ(orderBook->getLowestSell()->getLimitPrice(), 3500);
}

TEST_F(OrderBookTest, AddingOrderToExistingLimit) {
    int price = 25.03;
    orderBook->addOrderToBook(true, 3, price);
    orderBook->addOrderToBook(true, 2, price); // Same price

    Limit* limit = orderBook->getHighestBuy();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

TEST_F(OrderBookTest, AddingBestBuyAndSellOrders) {
    orderBook->addOrderToBook(true, 3, 25.04); // Initial best buy
    orderBook->addOrderToBook(false, 3, 30.0002); // Initial best sell
    orderBook->addOrderToBook(true, 2, 26.02); // Better buy
    orderBook->addOrderToBook(false, 2, 29.14); // Better sell

    EXPECT_EQ(orderBook->getHighestBuy()->getLimitPrice(), 2602);
    EXPECT_EQ(orderBook->getLowestSell()->getLimitPrice(), 2914);
}

TEST_F(OrderBookTest, Adding3WorstBuyLimits){
    orderBook -> addOrderToBook(true, 10, 10.04); // Best buy
    orderBook -> addOrderToBook(true, 2, 9.00); // Worst buy
    orderBook -> addOrderToBook(true, 5, 9.00); // Another worst buy

    auto buyTree = orderBook -> getBuyTree();
    EXPECT_EQ(buyTree->getLeftChild()->getLimitPrice(), 900);
    EXPECT_EQ(buyTree->getLeftChild()->getSize(), 2);
    EXPECT_EQ(buyTree->getLeftChild()->getTotalVolume(), 7);
    
    orderBook -> addOrderToBook(true, 10, 9.00); // Another worst buy

    EXPECT_EQ(buyTree->getLeftChild()->getSize(), 3);
    EXPECT_EQ(buyTree->getLeftChild()->getTotalVolume(), 17);
}

TEST_F(OrderBookTest, Adding3WorstSellLimits){
    orderBook -> addOrderToBook(false, 30, 30.15); // Best sell
    orderBook -> addOrderToBook(false, 40, 31.12); // Worst sell
    orderBook -> addOrderToBook(false, 45, 31.12); // Another worst sell

    auto sellTree = orderBook -> getSellTree();
    EXPECT_EQ(sellTree->getRightChild()->getLimitPrice(), 3112);
    EXPECT_EQ(sellTree->getRightChild()->getSize(), 2);
    EXPECT_EQ(sellTree->getRightChild()->getTotalVolume(), 85);
    
    orderBook -> addOrderToBook(false, 15, 31.12); // Another worst sell

    EXPECT_EQ(sellTree->getRightChild()->getSize(), 3);
    EXPECT_EQ(sellTree->getRightChild()->getTotalVolume(), 100);
}

TEST_F(OrderBookTest, NegativeSizeTest) {
    EXPECT_THROW({
        orderBook->addOrderToBook(true, -30.00, 30); // Negative volume
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, NegativeLimitPrice) {
    EXPECT_THROW({
        orderBook->addOrderToBook(true, 3.15, -1); // Negative price
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, correctValueInOrder) {
    orderBook -> addOrderToBook(false, 30, 30.15);
    
    auto sellTree = orderBook -> getSellTree();
    auto order = sellTree->getHeadOrder();
    
    EXPECT_EQ(3015, order->getLimit());
    EXPECT_EQ(30, order->getShares());
    EXPECT_EQ(false, order->getOrderType());
    
    orderBook->addOrderToBook(false, 5, 30.15);
    sellTree = orderBook->getSellTree();
    order = sellTree->getHeadOrder();
    auto nextOrder = order->getNextOrder();
    
    EXPECT_EQ(3015, nextOrder->getLimit());
    EXPECT_EQ(false, nextOrder->getOrderType());
    EXPECT_EQ(5, nextOrder->getShares());
}

TEST_F(OrderBookTest, LimitAddOrderTest){
    orderBook -> addOrderToBook(true, 30, 17.98);
    Limit* buyTree = orderBook->getBuyTree();
    
    int secondEntryTime = static_cast<int>(system_clock::to_time_t(system_clock::now()));
    buyTree->addOrderToLimit(true, 7, secondEntryTime);
    
    Order* addedOrder = buyTree->getHeadOrder()->getNextOrder();
    
    EXPECT_EQ(1798, addedOrder->getLimit());
    EXPECT_EQ(true, addedOrder->getOrderType());
    EXPECT_EQ(7, addedOrder->getShares());
}

TEST_F(OrderBookTest, LimitAddLimitTest){
    orderBook -> addOrderToBook(true, 5, 34.12);
    Limit* buyTree = orderBook->getBuyTree();
    
    buyTree->addLimit(15, 3410, true);
    Limit* newLimit = buyTree->getLeftChild();
}
