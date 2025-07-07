#include "../src/Exchange.hpp"
#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono;

class ExchangeTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange> exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("ENDEX");
        orderBook = std::make_unique<Book>();
    }
};


// test for adding an instrument to an exchange
TEST_F(ExchangeTest, TestAddingTickerToExchange) {
    exchange->addInstrument("TTF 24Q-ICN");
    EXPECT_TRUE(exchange->getOrderBook("TTF 24Q-ICN") != nullptr);
}

// searching for an ob that is not in the exchange
TEST_F(ExchangeTest, TestSearchingForTickerNotInExchange) {
    exchange->addInstrument("TTF 24Q-ICN");
    EXPECT_TRUE(exchange->getOrderBook("TTF 24Z-ICN") == nullptr);
}

// adding a limit order to the exchange
TEST_F(ExchangeTest, TestAddingLimitOrderToExchange) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    exchange->addOrder(ttfTicker, Side::Buy, 5, 47, OrderType::Limit);

    Book* ob = exchange->getOrderBook(ttfTicker);

    const auto* allOrders = &ob->getAllOrders();
    EXPECT_EQ(allOrders->size(), 1);

    EXPECT_EQ(ob->getBuySide().getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getSize(), 1);
}

// test adding a limit order to the exchange without providing limit price
TEST_F(ExchangeTest, AddingGTCOrderWithoutPrice) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);

    EXPECT_THROW({
        exchange->addOrder(ttfTicker, Side::Buy, 5, -1, OrderType::Limit);
    }, std::invalid_argument);
}

// placing market order in exchange
TEST_F(ExchangeTest, TestAddingMarketOrderToExchange) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    exchange->addOrder(ttfTicker, Side::Buy, 5, 47, OrderType::Limit);
    exchange->addOrder(ttfTicker, Side::Sell, 2, -1, OrderType::Market);

    Book* ob = exchange->getOrderBook(ttfTicker);

    const auto* allOrders = &ob->getAllOrders();
    EXPECT_EQ(allOrders->size(), 1);

    EXPECT_EQ(ob->getBuySide().getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getTotalVolume(), 3);
}

// placing modify order size
TEST_F(ExchangeTest, TestModifyOrderSizeFromExchange) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);

    exchange->addOrder(ttfTicker, Side::Buy, 5, 47, OrderType::Limit);

    Book* ob = exchange->getOrderBook(ttfTicker);

    exchange->modifyOrderSize(ttfTicker, 0, 10);

    EXPECT_EQ(ob->getBuySide().getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getTotalVolume(), 10);
}

// placing modify order limit price
TEST_F(ExchangeTest, TestModifyOrderLimitFromExchange) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);

    exchange->addOrder(ttfTicker, Side::Buy, 5, 47, OrderType::Limit);

    Book* ob = exchange->getOrderBook(ttfTicker);

    exchange->modifyLimitPrice(ttfTicker, 0, 50);

    EXPECT_EQ(ob->getBuySide().getBestLimit()->getLimitPrice(), 5000);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide().getBestLimit()->getTotalVolume(), 5);
}

// retrieving all instruments tickers
TEST_F(ExchangeTest, TestGetTickerList) {
    std::string ttfTickerAug = "TTF 24Q-ICN";
    std::string ttfTickerDec = "TTF 24Z-ICN";
    exchange->addInstrument(ttfTickerAug);
    exchange->addInstrument(ttfTickerDec);

    std::vector<std::string> allTickers = exchange->getTickerList();

    EXPECT_NE(std::find(allTickers.begin(), allTickers.end(), ttfTickerAug), allTickers.end());
    EXPECT_NE(std::find(allTickers.begin(), allTickers.end(), ttfTickerDec), allTickers.end());
}

// removing an instrument from the exchange
TEST_F(ExchangeTest, TestRemoveTickerFromExchange) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    exchange->removeInstrument(ttfTicker);

    EXPECT_TRUE(exchange->getOrderBook(ttfTicker) == nullptr);
}

// retrieving best bid and ask from the exchange
TEST_F(ExchangeTest, TestGetNBBO) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);

    exchange->addOrder(ttfTicker, Side::Buy, 5, 100, OrderType::Limit);
    exchange->addOrder(ttfTicker, Side::Sell, 5, 200, OrderType::Limit);

    auto nbbo = exchange->getNBBO(ttfTicker);

    EXPECT_EQ(nbbo.first, 10000);
    EXPECT_EQ(nbbo.second, 20000);
}

// retrieving best bid and ask when there is no bid
TEST_F(ExchangeTest, TestGetNBBOWhenNoBid) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);

    exchange->addOrder(ttfTicker, Side::Sell, 5, 200, OrderType::Limit);

    auto nbbo = exchange->getNBBO(ttfTicker);

    EXPECT_TRUE(!nbbo.first.has_value());
    EXPECT_EQ(nbbo.second, 20000);
}

// retrieving best bid and ask when there is no ask
TEST_F(ExchangeTest, TestGetNBBOWhenNoAsk) {
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);

    exchange->addOrder(ttfTicker, Side::Buy, 5, 200, OrderType::Limit);

    auto nbbo = exchange->getNBBO(ttfTicker);

    EXPECT_EQ(nbbo.first, 20000);
    EXPECT_TRUE(!nbbo.second.has_value());
}
