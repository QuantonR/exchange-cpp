#include <gtest/gtest.h>

#include "../../src/core/Exchange.hpp"
#include "../../src/core/OrderData.h"
#include "../../src/core/Side.hpp"
#include "../../src/core/OrderType.h"
#include "../../src/core/Order.h"
#include "../../src/core/Book.h"

class ExchangeTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange>exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("ENDEX");
        orderBook = std::make_unique<Book>(*exchange);
    }
};


// test for adding an instrument to an exchange
TEST_F(ExchangeTest, TestAddingTickerToExchange) {
    
    exchange->addInstrument("TTF 24Q-ICN");
    
    EXPECT_TRUE(exchange -> getOrderBook("TTF 24Q-ICN") != nullptr);
}

// searching for an ob that is not in the exchange
TEST_F(ExchangeTest, TestSearchingForTickerNotInExchange) {
    
    exchange->addInstrument("TTF 24Q-ICN");
    
    EXPECT_TRUE(exchange -> getOrderBook("TTF 24Z-ICN") == nullptr);
}

// adding a limit order to the exchange
TEST_F(ExchangeTest, TestAddingLimitOrderToExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    OrderData order(Side::Buy, 5, 1, 47, OrderType::Limit);
    
    exchange->addOrder(ttfTicker, order);
    
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    const std::unordered_map<uint64_t, std::unique_ptr<Order>>* allOrders = ob->getAllOrders();
    EXPECT_EQ(allOrders->size(), 1);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
}

// test adding a limit order to the exchange without providing limit price
TEST_F(ExchangeTest, AddingGTCOrderWithoutPrice) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    EXPECT_THROW({
        OrderData order(Side::Buy, 5, 2, OrderType::Limit);
        
        exchange->addOrder(ttfTicker, order);
    }, std::invalid_argument);
}

// placing market order in exchage
TEST_F(ExchangeTest, TestAddingMarketOrderToExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    OrderData order1(Side::Buy, 5, 83, 47, OrderType::Limit);
    OrderData order2(Side::Sell, 2, 11, OrderType::Market);

    exchange->addOrder(ttfTicker, order1);
    exchange->addOrder(ttfTicker, order2);
    
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    const std::unordered_map<uint64_t, std::unique_ptr<Order>>* allOrders = ob->getAllOrders();
    EXPECT_EQ(allOrders->size(), 1);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getTotalVolume(), 3);
}

// placing modify order size
TEST_F(ExchangeTest, TestModifyOrderSizeFromExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    OrderData order(Side::Buy, 5, 98, 47, OrderType::Limit);
    exchange->addOrder(ttfTicker, order);
    
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    exchange -> modifyOrderSize(ttfTicker, 0, 10);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getTotalVolume(), 10);
}

// placing modify order limit price
TEST_F(ExchangeTest, TestModifyOrderLimitFromExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    OrderData order(Side::Buy, 5, 13, 47, OrderType::Limit);
    exchange->addOrder(ttfTicker, order);
    
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    exchange -> modifyLimitPrice(ttfTicker, 0, 50);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 5000);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getTotalVolume(), 5);
}

// retrieving all instruments tickers
TEST_F(ExchangeTest, TestGetTickerList) {
    
    std::string ttfTickerAug = "TTF 24Q-ICN";
    std::string ttfTickerDec = "TTF 24Z-ICN";
    exchange->addInstrument(ttfTickerAug);
    exchange->addInstrument(ttfTickerDec);
    
    std::vector<std::string> allTickers = exchange->getTickerList();
    
    // Check if the tickers are in the vector
    EXPECT_NE(std::find(allTickers.begin(), allTickers.end(), ttfTickerAug), allTickers.end());
    EXPECT_NE(std::find(allTickers.begin(), allTickers.end(), ttfTickerDec), allTickers.end());
}

// removing an instruments from the exchange
TEST_F(ExchangeTest, TestRemoveTickerFromExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    exchange->removeInstrument(ttfTicker);
    
    EXPECT_TRUE(exchange -> getOrderBook(ttfTicker) == nullptr);
}

// retrieving best bid and ask from the exchange
TEST_F(ExchangeTest, TestGetNBBO) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    OrderData order1(Side::Buy, 5, 34, 100, OrderType::Limit);
    exchange->addOrder(ttfTicker, order1);
    
    OrderData order2(Side::Sell, 5, 12, 200, OrderType::Limit);
    exchange->addOrder(ttfTicker, order2);
    
    std::pair <std::optional<int>, std::optional<int>> nbbo = exchange -> getNBBO(ttfTicker);
    
    EXPECT_EQ(nbbo.first, 10000);
    EXPECT_EQ(nbbo.second, 20000);
}

// retrieving best bid and ask from the exchange when there is no bid
TEST_F(ExchangeTest, TestGetNBBOWhenNoBid) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    OrderData order(Side::Sell, 5, 1, 200, OrderType::Limit);
    exchange->addOrder(ttfTicker, order);
        
    std::pair <std::optional<int>, std::optional<int>> nbbo = exchange -> getNBBO(ttfTicker);
    
    EXPECT_TRUE(!nbbo.first.has_value());
    EXPECT_EQ(nbbo.second, 20000);
}

// retrieving best bid and ask from the exchange when there is no ask
TEST_F(ExchangeTest, TestGetNBBOWhenNoAsk) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    OrderData order(Side::Buy, 5, 1, 200, OrderType::Limit);
    exchange->addOrder(ttfTicker, order);
        
    std::pair <std::optional<int>, std::optional<int>> nbbo = exchange -> getNBBO(ttfTicker);
    
    EXPECT_EQ(nbbo.first, 20000);
    EXPECT_TRUE(!nbbo.second.has_value());
}
