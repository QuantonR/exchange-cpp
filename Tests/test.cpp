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

    void TearDown() override {}
};

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};

TEST_F(OrderBookTest, AddingFirstBuyOrder) {
    float orderPrice = 25.09;
    int intOrderPrice = 2509;
    int volume = 3;
    
    orderBook -> addOrderToBook(true, volume, orderPrice);

    Limit* highestBuy = orderBook -> getBuySide() -> getBestLimit();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy -> getLimitPrice(), intOrderPrice);
    EXPECT_EQ(highestBuy -> getSize(), 1);
    EXPECT_EQ(highestBuy -> getTotalVolume(), volume);

    Order* headOrder = highestBuy -> getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy -> getTailOrder());
    EXPECT_EQ(headOrder -> getShares(), volume);
 }


TEST_F(OrderBookTest, AddingFirstSellOrder) {
    float floatPrice = 30.052;
    int orderPrice = 3005;
    int insertedSize = 5;

    orderBook -> addOrderToBook(false, insertedSize, floatPrice);

    Limit* lowestSell = orderBook -> getSellSide() -> getBestLimit();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell -> getLimitPrice(), orderPrice);
    EXPECT_EQ(lowestSell -> getSize(), 1);
    EXPECT_EQ(lowestSell -> getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell -> getHeadOrder();
    ASSERT_EQ(headOrder, lowestSell -> getTailOrder());
    EXPECT_EQ(headOrder -> getShares(), insertedSize);
}

TEST_F(OrderBookTest, AddingWorseBuyAndSellOrders) {
    orderBook -> addOrderToBook(true, 2, 20.05); // Lower buy price
    orderBook -> addOrderToBook(false, 2, 35.000); // Higher sell price

    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getLimitPrice(), 2005);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit() -> getLimitPrice(), 3500);
}

TEST_F(OrderBookTest, AddingOrderToExistingLimit) {
    int price = 25.03;
    orderBook -> addOrderToBook(true, 3, price);
    orderBook -> addOrderToBook(true, 2, price); // Same price

    Limit* limit = orderBook -> getBuySide() -> getBestLimit();
    EXPECT_EQ(limit -> getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit -> getTotalVolume(), 5); // Combined volume of orders
}

TEST_F(OrderBookTest, AddingBestBuyAndSellOrders) {
    orderBook -> addOrderToBook(true, 3, 25.04); // Initial best buy
    orderBook -> addOrderToBook(false, 3, 30.0002); // Initial best sell
    orderBook -> addOrderToBook(true, 2, 26.02); // Better buy
    orderBook -> addOrderToBook(false, 2, 29.14); // Better sell

    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getLimitPrice(), 2602);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit() -> getLimitPrice(), 2914);
}

TEST_F(OrderBookTest, Adding3WorstBuyLimits){
    orderBook -> addOrderToBook(true, 10, 10.04); // Best buy
    orderBook -> addOrderToBook(true, 2, 9.00); // Worst buy
    orderBook -> addOrderToBook(true, 5, 9.00); // Another worst buy

    auto buyTree = orderBook -> getBuySide();
    
    orderBook -> addOrderToBook(true, 10, 9.00); // Another worst buy

    EXPECT_EQ(buyTree -> getBestLimit() -> getLimitPrice(), 1004);
    EXPECT_EQ(buyTree -> getSideVolume(), 27);
}

TEST_F(OrderBookTest, Adding3WorstSellLimits){
    orderBook -> addOrderToBook(false, 30, 30.15); // Best sell
    orderBook -> addOrderToBook(false, 40, 31.12); // Worst sell
    orderBook -> addOrderToBook(false, 45, 31.12); // Another worst sell

    auto sellTree = orderBook -> getSellSide();
    
    orderBook -> addOrderToBook(false, 15, 31.12); // Another worst sell

    EXPECT_EQ(sellTree -> getBestLimit() -> getLimitPrice(), 3015);
    EXPECT_EQ(sellTree -> getSideVolume(), 130);
}

TEST_F(OrderBookTest, NegativeSizeTest) {
    EXPECT_THROW({
        orderBook -> addOrderToBook(true, -30.00, 30); // Negative volume
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, NegativeLimitPrice) {
    EXPECT_THROW({
        orderBook -> addOrderToBook(true, 3.15, -1); // Negative price
    }, std::invalid_argument);
}

TEST_F(OrderBookTest, correctValueInOrder) {
    orderBook -> addOrderToBook(false, 30, 30.15);
    
    auto sellTree = orderBook -> getSellSide() -> getBestLimit();
    auto order = sellTree -> getHeadOrder();
    
    EXPECT_EQ(3015, order -> getLimit());
    EXPECT_EQ(30, order -> getShares());
    EXPECT_EQ(Side::Sell, order -> getOrderType());
    
    orderBook -> addOrderToBook(false, 5, 30.15);
    sellTree = orderBook -> getSellSide() -> getBestLimit();
    order = sellTree -> getHeadOrder();
    auto nextOrder = order -> getNextOrder();
    
    EXPECT_EQ(3015, nextOrder -> getLimit());
    EXPECT_EQ(Side::Sell, nextOrder -> getOrderType());
    EXPECT_EQ(5, nextOrder -> getShares());
}


TEST_F(OrderBookTest, singleMarketOrderSell){
    
    orderBook -> addOrderToBook(false, 3, 30.0002); // Initial best sell
    orderBook -> addOrderToBook(false, 2, 29.14); // Better sell

    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit() -> getTotalVolume(), 2);
    orderBook -> placeMarketOrder(1, true);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit() -> getTotalVolume(), 1);
}

TEST_F(OrderBookTest, singleMarketOrderBuy){
    
    orderBook -> addOrderToBook(true, 3, 50.14); // Initial best buy
    orderBook -> addOrderToBook(true, 10, 55); // Better sell

    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getTotalVolume(), 10);
    orderBook -> placeMarketOrder(5, false);
    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getTotalVolume(), 5);
}

TEST_F(OrderBookTest, MarketOrdersThatCancelFirstLevel){
    
    orderBook -> addOrderToBook(true, 3, 30); // Initial best buy
    orderBook -> addOrderToBook(true, 10, 35); // Better buy
    orderBook -> addOrderToBook(false, 7, 55); // Initial best sell
    orderBook -> addOrderToBook(false, 14, 50); // Better sell

    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getTotalVolume(), 10);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit() -> getTotalVolume(), 14);

    orderBook -> placeMarketOrder(11, false);
    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getTotalVolume(), 2);
    
    orderBook -> placeMarketOrder(15, true);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit() -> getTotalVolume(), 6);
}

TEST_F(OrderBookTest, MarketOrderThatEmptiesOrderBook){
    
    orderBook -> addOrderToBook(true, 3, 30);

    orderBook -> placeMarketOrder(3, false);
    
    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit(), nullptr);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit(), nullptr);
    
    orderBook -> addOrderToBook(true, 10, 10);
    
    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getTotalVolume(), 10);
    EXPECT_EQ(orderBook -> getBuySide() -> getBestLimit() -> getLimitPrice(), 1000);
    EXPECT_EQ(orderBook -> getSellSide() -> getBestLimit(), nullptr);
}

TEST_F(OrderBookTest, MarketOrderWithNoBook) {
    EXPECT_THROW({
        orderBook -> placeMarketOrder(11, false); // Market Order with no order book
    }, std::runtime_error);
}

TEST_F(OrderBookTest, MarketOrderSizeGreaterThenBook) {
    EXPECT_THROW({
        orderBook -> addOrderToBook(false, 3.15, 1);
        orderBook -> placeMarketOrder(15, true); // Market Order with size greater than order book
    }, std::runtime_error);
}

/*
TEST_F(OrderBookTest, TestSellLimitOrdersCrossingSpread){
    
    orderBook -> addOrderToBook(true, 50, 15);
    
    orderBook -> addOrderToBook(false, 40, 7);
    
    EXPECT_EQ(orderBook -> getHighestBuy() -> getTotalVolume(), 10);
    EXPECT_EQ(orderBook -> getHighestBuy() -> getLimitPrice(), 1500);
}

TEST_F(OrderBookTest, TestBuyLimitOrdersCrossingSpread){
    
    orderBook -> addOrderToBook(false, 60, 24);
    
    orderBook -> addOrderToBook(true, 70, 30);
    
    EXPECT_EQ(orderBook -> getHighestBuy() -> getTotalVolume(), 10);
    EXPECT_EQ(orderBook -> getHighestBuy() -> getLimitPrice(), 3000);
}

TEST_F(OrderBookTest, TestLimitOrderFillsBestSellLimit){
    
    orderBook -> addOrderToBook(false, 35, 24.9); // best sell
    orderBook -> addOrderToBook(false, 100, 30); // second level sell
    orderBook -> addOrderToBook(true, 75, 35); // buy order that crosses the spread
    
    EXPECT_EQ(orderBook -> getLowestSell() -> getTotalVolume(), 60);
    EXPECT_EQ(orderBook -> getLowestSell() -> getLimitPrice(), 3000);
}

TEST_F(OrderBookTest, TestLimitOrderFillsBestBuyLimit){
    
    orderBook -> addOrderToBook(true, 35, 24.9); // first best buy
    orderBook -> addOrderToBook(true, 100, 30); // new best buy
    orderBook -> addOrderToBook(false, 115, 20); // sell that crosses the spread
    
    EXPECT_EQ(orderBook -> getHighestBuy() -> getTotalVolume(), 20);
    EXPECT_EQ(orderBook -> getHighestBuy() -> getLimitPrice(), 2490);
}

TEST_F(OrderBookTest, TestLimitCrossFirstLevel){
    
    orderBook -> addOrderToBook(false, 10, 45);
    orderBook -> addOrderToBook(false, 5, 40);
    orderBook -> addOrderToBook(true, 7, 42.5);
    
    EXPECT_EQ(orderBook -> getLowestSell() -> getTotalVolume(), 10);
    EXPECT_EQ(orderBook -> getLowestSell() -> getLimitPrice(), 4500);
    EXPECT_EQ(orderBook -> getHighestBuy() -> getTotalVolume(), 2);
    EXPECT_EQ(orderBook -> getHighestBuy() -> getLimitPrice(), 4250);
}

TEST_F(OrderBookTest, LimitCancelFullBook){
    
    orderBook -> addOrderToBook(false, 10, 45);
    orderBook -> addOrderToBook(false, 5, 40);
    orderBook -> addOrderToBook(true, 15, 50);
    
    EXPECT_EQ(orderBook -> getHighestBuy(), nullptr);
    EXPECT_EQ(orderBook -> getBuyTree(), nullptr);
    EXPECT_EQ(orderBook -> getLowestSell(), nullptr);
    EXPECT_EQ(orderBook -> getSellTree(), nullptr);
}

TEST_F(OrderBookTest, LimitCancelBookAndPlaceOrder){
    
    orderBook -> addOrderToBook(false, 10, 45);
    orderBook -> addOrderToBook(false, 5, 40);
    orderBook -> addOrderToBook(true, 20, 50);
    
    EXPECT_EQ(orderBook -> getHighestBuy() -> getTotalVolume(), 5);
    EXPECT_EQ(orderBook -> getHighestBuy() -> getLimitPrice(), 5000);
    EXPECT_EQ(orderBook -> getLowestSell(), nullptr);
    EXPECT_EQ(orderBook -> getSellTree(), nullptr);
}
*/
