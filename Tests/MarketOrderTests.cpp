#include "../src/Book.h"
#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono;

class MarketOrderTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    OrderIdSequence orderIdSequence;

    void SetUp() override {
        orderBook = std::make_unique<Book>();
    }
};

// test for single market sell order
TEST_F(MarketOrderTest, SingleMarketOrderSell) {
    
    OrderData orderData1 = OrderData(Side::Sell, 3, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Sell, 2, 29.14, OrderType::Limit);

    orderBook->addOrderToBook(orderData1, orderIdSequence); // Initial best sell
    orderBook->addOrderToBook(orderData2, orderIdSequence); // Better sell

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 2);
    orderBook->placeMarketOrder(1, Side::Buy);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 1);
}

// test for single market buy order
TEST_F(MarketOrderTest, SingleMarketOrderBuy) {
    
    OrderData orderData1 = OrderData(Side::Buy, 3, 50.14, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 10, 55, OrderType::Limit);

    orderBook->addOrderToBook(orderData1, orderIdSequence); // Initial best buy
    orderBook->addOrderToBook(orderData2, orderIdSequence); // Better buy

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    orderBook->placeMarketOrder(5, Side::Sell);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 5);
}

// test for market orders that cancel first level
TEST_F(MarketOrderTest, MarketOrdersThatCancelFirstLevel) {
    
    OrderData orderData1 = OrderData(Side::Buy, 3, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 10, 35, OrderType::Limit);
    OrderData orderData3 = OrderData(Side::Sell, 7, 55, OrderType::Limit);
    OrderData orderData4 = OrderData(Side::Sell, 14, 50, OrderType::Limit);

    orderBook->addOrderToBook(orderData1, orderIdSequence); // Initial best buy
    orderBook->addOrderToBook(orderData2, orderIdSequence); // Better buy
    orderBook->addOrderToBook(orderData3, orderIdSequence); // Initial best sell
    orderBook->addOrderToBook(orderData4, orderIdSequence); // Better sell

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 14);

    orderBook->placeMarketOrder(11, Side::Sell);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 2);

    orderBook->placeMarketOrder(15, Side::Buy);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 6);
}

// test for market orders that cancel multiple orders at first level
TEST_F(MarketOrderTest, MarketOrdersThatCancelMultipleOrderFirstLevel) {
    
    OrderData orderData1 = OrderData(Side::Buy, 3, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 10, 35, OrderType::Limit);
    OrderData orderData3 = OrderData(Side::Buy, 15, 35, OrderType::Limit);

    orderBook->addOrderToBook(orderData1, orderIdSequence); // initial best buy
    orderBook->addOrderToBook(orderData2, orderIdSequence); // better buy
    orderBook->addOrderToBook(orderData3, orderIdSequence); // same level as best buy

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 25);

    orderBook->placeMarketOrder(27, Side::Sell);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 1);
}

// test for market order that empties the order book
TEST_F(MarketOrderTest, MarketOrderThatEmptiesOrderBook) {
    
    OrderData orderData = OrderData(Side::Buy, 3, 30, OrderType::Limit);

    orderBook->addOrderToBook(orderData, orderIdSequence);

    orderBook->placeMarketOrder(3, Side::Sell);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);

    OrderData newOrderData = OrderData(Side::Buy, 10, 10, OrderType::Limit);
    orderBook->addOrderToBook(newOrderData, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1000);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
}

// test for market order with no book
TEST_F(MarketOrderTest, MarketOrderWithNoBook) {
    
    EXPECT_THROW({
        orderBook->placeMarketOrder(11, Side::Sell); // Market Order with no order book
    }, std::runtime_error);
}

// test for market order size greater than book
TEST_F(MarketOrderTest, MarketOrderSizeGreaterThenBook) {
    OrderData orderData = OrderData(Side::Sell, 3.15, 1, OrderType::Limit);
    orderBook->addOrderToBook(orderData, orderIdSequence);
    
    EXPECT_THROW({
        orderBook->placeMarketOrder(15, Side::Buy); // Market Order with size greater than order book
    }, std::runtime_error);
}
