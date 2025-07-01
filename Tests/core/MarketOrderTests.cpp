#include <gtest/gtest.h>

#include "../../src/core/Exchange.hpp"
#include "../../src/core/OrderData.h"
#include "../../src/core/Side.hpp"
#include "../../src/core/OrderType.h"
#include "../../src/core/Order.h"
#include "../../src/core/Book.h"

class MarketOrderTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange>exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("myExchange");
        orderBook = std::make_unique<Book>(*exchange, "AAPL");
    }
};

// test for single market sell order
TEST_F(MarketOrderTest, SingleMarketOrderSell) {
    
    OrderData orderData1 = OrderData(Side::Sell, 3, 1, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Sell, 2, 2, 29.14, OrderType::Limit);

    orderBook->addOrderToBook(orderData1); // Initial best sell
    orderBook->addOrderToBook(orderData2); // Better sell

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 2);
    
    OrderData orderData3 = OrderData(Side::Buy, 1, 3, OrderType::Market);
    orderBook->placeMarketOrder(orderData3);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 1);
}

// test for single market buy order
TEST_F(MarketOrderTest, SingleMarketOrderBuy) {
    
    OrderData orderData1 = OrderData(Side::Buy, 3, 1, 50.14, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 10, 2, 55, OrderType::Limit);

    orderBook->addOrderToBook(orderData1); // Initial best buy
    orderBook->addOrderToBook(orderData2); // Better buy

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    
    OrderData orderData3 = OrderData(Side::Sell, 5, 3, OrderType::Market);
    orderBook->placeMarketOrder(orderData3);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 5);
}

// test for market orders that cancel first level
TEST_F(MarketOrderTest, MarketOrdersThatCancelFirstLevel) {
    
    OrderData orderData1 = OrderData(Side::Buy, 3, 123, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 10, 456, 35, OrderType::Limit);
    OrderData orderData3 = OrderData(Side::Sell, 7, 64, 55, OrderType::Limit);
    OrderData orderData4 = OrderData(Side::Sell, 14, 98, 50, OrderType::Limit);

    orderBook->addOrderToBook(orderData1); // Initial best buy
    orderBook->addOrderToBook(orderData2); // Better buy
    orderBook->addOrderToBook(orderData3); // Initial best sell
    orderBook->addOrderToBook(orderData4); // Better sell

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 14);

    OrderData orderData5 = OrderData(Side::Sell, 11, 45, OrderType::Market);
    orderBook->placeMarketOrder(orderData5);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 2);

    OrderData orderData6 = OrderData(Side::Buy, 15, 34, OrderType::Market);
    orderBook->placeMarketOrder(orderData6);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 6);
}

// test for market orders that cancel multiple orders at first level
TEST_F(MarketOrderTest, MarketOrdersThatCancelMultipleOrderFirstLevel) {
    
    OrderData orderData1 = OrderData(Side::Buy, 3, 1, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 10, 92, 35, OrderType::Limit);
    OrderData orderData3 = OrderData(Side::Buy, 15, 103, 35, OrderType::Limit);

    orderBook->addOrderToBook(orderData1); // initial best buy
    orderBook->addOrderToBook(orderData2); // better buy
    orderBook->addOrderToBook(orderData3); // same level as best buy

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 25);

    OrderData orderData4 = OrderData(Side::Sell, 27, 45, OrderType::Market);
    orderBook->placeMarketOrder(orderData4);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 1);
}

// test for market order that empties the order book
TEST_F(MarketOrderTest, MarketOrderThatEmptiesOrderBook) {
    
    OrderData orderData = OrderData(Side::Buy, 3, 45, 30, OrderType::Limit);

    orderBook->addOrderToBook(orderData);

    OrderData orderData2 = OrderData(Side::Sell, 3, 43, OrderType::Market);
    orderBook->placeMarketOrder(orderData2);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);

    OrderData newOrderData = OrderData(Side::Buy, 10, 12, 10, OrderType::Limit);
    orderBook->addOrderToBook(newOrderData);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1000);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
}

// test for market order with no book
TEST_F(MarketOrderTest, MarketOrderWithNoBook) {
    
    EXPECT_THROW({
        OrderData orderData3 = OrderData(Side::Sell, 11, 13, OrderType::Market);
        orderBook->placeMarketOrder(orderData3);  // Market Order with no order book
    }, std::runtime_error);
}

// test for market order size greater than book
TEST_F(MarketOrderTest, MarketOrderSizeGreaterThenBook) {
    OrderData orderData = OrderData(Side::Sell, 3.15, 1, 90, OrderType::Limit);
    orderBook->addOrderToBook(orderData);
    
    EXPECT_THROW({
        OrderData orderData3 = OrderData(Side::Buy, 15, 71, OrderType::Market);
        orderBook->placeMarketOrder(orderData3); // Market Order with size greater than order book
    }, std::runtime_error);
}
