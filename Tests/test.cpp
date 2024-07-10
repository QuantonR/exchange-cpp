#include "../src/Exchange.hpp"
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <chrono>

using namespace std::chrono;

class OrderBookTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange>exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("ENDEX");
        orderBook = std::make_unique<Book>();
        Order::resetGlobalOrderId();
    }

    void TearDown() override {}
};

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// tests for order book
// test for adding the first buy order
TEST_F(OrderBookTest, AddingFirstBuyOrder) {
    
    float orderPrice = 25.09;
    int limitPrice = 2509;
    int volume = 3;

    orderBook->addOrderToBook(true, volume, orderPrice);

    Limit* highestBuy = orderBook->getBuySide()->getBestLimit();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), limitPrice);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), volume);
}

// test for adding the first sell order
TEST_F(OrderBookTest, AddingFirstSellOrder) {
    
    float floatPrice = 30.052;
    int orderPrice = 3005;
    int insertedSize = 5;

    orderBook->addOrderToBook(false, insertedSize, floatPrice);

    Limit* lowestSell = orderBook->getSellSide()->getBestLimit();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell->getLimitPrice(), orderPrice);
    EXPECT_EQ(lowestSell->getSize(), 1);
    EXPECT_EQ(lowestSell->getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell->getHeadOrder();
    ASSERT_EQ(headOrder, lowestSell->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
}

// test for adding worse buy and sell orders
TEST_F(OrderBookTest, AddingWorseBuyAndSellOrders) {
    
    orderBook->addOrderToBook(true, 2, 20.05); // Lower buy price
    orderBook->addOrderToBook(false, 2, 35.000); // Higher sell price

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2005);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 3500);
}

// test for adding order to existing limit
TEST_F(OrderBookTest, AddingOrderToExistingLimit) {
    
    int price = 25.03;
    orderBook->addOrderToBook(true, 3, price);
    orderBook->addOrderToBook(true, 2, price); // Same price

    Limit* limit = orderBook->getBuySide()->getBestLimit();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

// test for checking all orders in unordered map
TEST_F(OrderBookTest, CheckAllOrdersUnorderedMap) {
    
    orderBook->addOrderToBook(true, 3, 25.04);
    orderBook->addOrderToBook(false, 3, 30.0002);
    orderBook->addOrderToBook(true, 2, 26.02);
    orderBook->addOrderToBook(false, 2, 29.14);
    orderBook->addOrderToBook(true, 2, 26.02);
    orderBook->addOrderToBook(false, 2, 29.14);

    const std::unordered_map<int64_t, std::unique_ptr<Order>>* allOrders = orderBook->getAllOrders();
    EXPECT_EQ(allOrders->size(), 6);

    // Verify each order
    for (const auto& [orderId, orderPtr] : *allOrders) {
        EXPECT_TRUE(orderPtr != nullptr);  // Ensure the order pointer is not null

        // Access order properties and verify them
        int orderVolume = orderPtr->getShares();
        int limitPrice = orderPtr->getLimit();  // Assuming limitPrice is stored as anint (cents)
        Side orderType = orderPtr->getOrderType();

        // Check for specific orders
        if (orderVolume == 3 && limitPrice == 2504) {
            EXPECT_EQ(orderType, Side::Buy);
        } else if (orderVolume == 3 && limitPrice == 3000) {
            EXPECT_EQ(orderType, Side::Sell);
        } else if (orderVolume == 2 && limitPrice == 2602) {
            EXPECT_EQ(orderType, Side::Buy);
        } else if (orderVolume == 2 && limitPrice == 2914) {
            EXPECT_EQ(orderType, Side::Sell);
        } else {
            FAIL() << "Unexpected order found";
        }
    }
}

// test for adding best buy and sell orders
TEST_F(OrderBookTest, AddingBestBuyAndSellOrders) {
    
    orderBook->addOrderToBook(true, 3, 25.04); // Initial best buy
    orderBook->addOrderToBook(false, 3, 30.0002); // Initial best sell
    orderBook->addOrderToBook(true, 2, 26.02); // Better buy
    orderBook->addOrderToBook(false, 2, 29.14); // Better sell

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2602);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 2914);
}

// test for adding worst buy limits
TEST_F(OrderBookTest, Adding3WorstBuyLimits) {
    
    orderBook->addOrderToBook(true, 10, 10.04); // Best buy
    orderBook->addOrderToBook(true, 2, 9.00); // Worst buy
    orderBook->addOrderToBook(true, 5, 9.00); // Another worst buy

    auto buyTree = orderBook->getBuySide();

    orderBook->addOrderToBook(true, 10, 9.00); // Another worst buy

    EXPECT_EQ(buyTree->getBestLimit()->getLimitPrice(), 1004);
    EXPECT_EQ(buyTree->getSideVolume(), 27);
}

// test for adding worst sell limits
TEST_F(OrderBookTest, Adding3WorstSellLimits) {
    
    orderBook->addOrderToBook(false, 30, 30.15); // Best sell
    orderBook->addOrderToBook(false, 40, 31.12); // Worst sell
    orderBook->addOrderToBook(false, 45, 31.12); // Another worst sell

    auto sellTree = orderBook->getSellSide();

    orderBook->addOrderToBook(false, 15, 31.12); // Another worst sell

    EXPECT_EQ(sellTree->getBestLimit()->getLimitPrice(), 3015);
    EXPECT_EQ(sellTree->getSideVolume(), 130);
}

// test for negative order size
TEST_F(OrderBookTest, NegativeSizeTest) {
    
    EXPECT_THROW({
        orderBook->addOrderToBook(true, -30, 30); // Negative volume
    }, std::invalid_argument);
}

// test for negative limit price
TEST_F(OrderBookTest, NegativeLimitPrice) {
    
    EXPECT_THROW({
        orderBook->addOrderToBook(true, 3.15, -1); // Negative price
    }, std::invalid_argument);
}

// test for correct value in order
TEST_F(OrderBookTest, CorrectValueInOrder) {
    
    orderBook->addOrderToBook(false, 30, 30.15);

    auto sellTree = orderBook->getSellSide()->getBestLimit();
    auto order = sellTree->getHeadOrder();

    EXPECT_EQ(3015, order->getLimit());
    EXPECT_EQ(30, order->getShares());
    EXPECT_EQ(Side::Sell, order->getOrderType());

    orderBook->addOrderToBook(false, 5, 30.15);
    sellTree = orderBook->getSellSide()->getBestLimit();
    order = sellTree->getHeadOrder();
    auto nextOrder = order->getNextOrder();

    EXPECT_EQ(3015, nextOrder->getLimit());
    EXPECT_EQ(Side::Sell, nextOrder->getOrderType());
    EXPECT_EQ(5, nextOrder->getShares());
}

// test for single market sell order
TEST_F(OrderBookTest, SingleMarketOrderSell) {
    
    orderBook->addOrderToBook(false, 3, 30.0002); // Initial best sell
    orderBook->addOrderToBook(false, 2, 29.14); // Better sell

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 2);
    orderBook->placeMarketOrder(1, true);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 1);
}

// test for single market buy order
TEST_F(OrderBookTest, SingleMarketOrderBuy) {
    
    orderBook->addOrderToBook(true, 3, 50.14); // Initial best buy
    orderBook->addOrderToBook(true, 10, 55); // Better buy

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    orderBook->placeMarketOrder(5, false);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 5);
}

// test for market orders that cancel first level
TEST_F(OrderBookTest, MarketOrdersThatCancelFirstLevel) {
    
    orderBook->addOrderToBook(true, 3, 30); // Initial best buy
    orderBook->addOrderToBook(true, 10, 35); // Better buy
    orderBook->addOrderToBook(false, 7, 55); // Initial best sell
    orderBook->addOrderToBook(false, 14, 50); // Better sell

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 14);

    orderBook->placeMarketOrder(11, false);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 2);

    orderBook->placeMarketOrder(15, true);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 6);
}

// test for market orders that cancel multiple orders at first level
TEST_F(OrderBookTest, MarketOrdersThatCancelMultipleOrderFirstLevel) {
    
    orderBook->addOrderToBook(true, 3, 30); // initial best buy
    orderBook->addOrderToBook(true, 10, 35); // better buy
    orderBook->addOrderToBook(true, 15, 35); // same level as best buy

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 25);

    orderBook->placeMarketOrder(27, false);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 1);
}

// test for market order that empties the order book
TEST_F(OrderBookTest, MarketOrderThatEmptiesOrderBook) {
    
    orderBook->addOrderToBook(true, 3, 30);

    orderBook->placeMarketOrder(3, false);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);

    orderBook->addOrderToBook(true, 10, 10);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1000);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
}

// test for market order with no book
TEST_F(OrderBookTest, MarketOrderWithNoBook) {
    
    EXPECT_THROW({
        orderBook->placeMarketOrder(11, false); // Market Order with no order book
    }, std::runtime_error);
}

// test for market order size greater than book
TEST_F(OrderBookTest, MarketOrderSizeGreaterThenBook) {
    
    EXPECT_THROW({
        orderBook->addOrderToBook(false, 3.15, 1);
        orderBook->placeMarketOrder(15, true); // Market Order with size greater than order book
    }, std::runtime_error);
}

// test for sell limit orders crossing the spread
TEST_F(OrderBookTest, TestSellLimitOrdersCrossingSpread) {
    
    orderBook->addOrderToBook(true, 50, 15);
    orderBook->addOrderToBook(false, 40, 7);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1500);
}

// test for buy limit orders crossing the spread
TEST_F(OrderBookTest, TestBuyLimitOrdersCrossingSpread) {
    
    orderBook->addOrderToBook(false, 60, 24);
    orderBook->addOrderToBook(true, 70, 30);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 3000);
}

// test for limit order filling best sell limit
TEST_F(OrderBookTest, TestLimitOrderFillsBestSellLimit) {
    
    orderBook->addOrderToBook(false, 35, 24.9); // best sell
    orderBook->addOrderToBook(false, 100, 30); // second level sell
    orderBook->addOrderToBook(true, 75, 35); // buy order that crosses the spread

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 60);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 3000);
}

// test for limit order filling best buy limit
TEST_F(OrderBookTest, TestLimitOrderFillsBestBuyLimit) {
    
    orderBook->addOrderToBook(true, 35, 24.9); // first best buy
    orderBook->addOrderToBook(true, 100, 30); // new best buy
    orderBook->addOrderToBook(false, 115, 20); // sell that crosses the spread

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2490);
}

// test for limit order crossing the first level
TEST_F(OrderBookTest, TestLimitCrossFirstLevel) {
    
    orderBook->addOrderToBook(false, 10, 45);
    orderBook->addOrderToBook(false, 5, 40);
    orderBook->addOrderToBook(true, 7, 42.5);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4500);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 2);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 4250);
}

// test for canceling the full book
TEST_F(OrderBookTest, LimitCancelFullBook) {
    
    orderBook->addOrderToBook(false, 10, 45);
    orderBook->addOrderToBook(false, 5, 40);
    orderBook->addOrderToBook(true, 15, 50);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
    EXPECT_TRUE(orderBook->getSellSide()->getSideTree().empty());
}

// test for canceling a book and placing an order
TEST_F(OrderBookTest, LimitCancelBookAndPlaceOrder) {
    orderBook->addOrderToBook(false, 10, 45);
    orderBook->addOrderToBook(false, 5, 40);
    orderBook->addOrderToBook(true, 20, 50);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 5);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 5000);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
    EXPECT_TRUE(orderBook->getSellSide()->getSideTree().empty());
}

// test for canceling a middle order in a limit
TEST_F(OrderBookTest, TestCancelMiddleOrderInLimit) {
    
    orderBook->addOrderToBook(true, 10, 47);
    orderBook->addOrderToBook(true, 20, 47);
    orderBook->addOrderToBook(true, 30, 47);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getSize(), 2);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), 0);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getNextOrder()->getOrderId(), 2);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getTotalVolume(), 40);

    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
}

// test for canceling the head order
TEST_F(OrderBookTest, TestCancelHeadOrder) {
    
    orderBook->addOrderToBook(true, 10, 47);
    orderBook->addOrderToBook(true, 20, 47);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getSize(), 1);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), 1);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId());

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getTotalVolume(), 20);

    EXPECT_EQ(orderBook->getAllOrders()->find(0), orderBook->getAllOrders()->end());
}

// test for canceling the tail order
TEST_F(OrderBookTest, TestCancelTailOrder) {
    
    orderBook->addOrderToBook(false, 10, 47);
    orderBook->addOrderToBook(false, 20, 47);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getSize(), 1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getHeadOrder()->getOrderId(), 0);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getHeadOrder()->getOrderId(), orderBook->getSellSide()->findLimit(4700)->getHeadOrder()->getOrderId());

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getTotalVolume(), 10);

    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
}

// test for canceling order that deletes a limit level
TEST_F(OrderBookTest, TestCancelOrderThatDeletsLimitLevel) {
    
    orderBook->addOrderToBook(false, 10, 47);
    orderBook->addOrderToBook(false, 20, 45);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4500), nullptr);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getSize(), 1);

    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4700);
}

// test for canceling order that deletes the book
TEST_F(OrderBookTest, TestCancelOrderThatDeletsBook) {
    
    orderBook->addOrderToBook(false, 10, 47);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700), nullptr);

    EXPECT_EQ(orderBook->getSellSide()->getSideVolume(), 0);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
}

// test for canceling an order not in the book
TEST_F(OrderBookTest, CancelOrderNotInBook) {
    
    EXPECT_THROW({
        orderBook->cancelOrder(10); // order that is not in the book
    }, std::invalid_argument);
}

// test for modifying order limit
TEST_F(OrderBookTest, TestModifyingOrderLimit) {
    
    orderBook->addOrderToBook(false, 20, 50);

    orderBook->modifyOrderLimitPrice(0, 40);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4750), nullptr);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4000)->getSize(), 1);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4000);
}

// test for modifying order limit into an existing limit
TEST_F(OrderBookTest, TestModifyingOrderLimitIntoExistingLimit) {
    
    orderBook->addOrderToBook(true, 10, 47);
    orderBook->addOrderToBook(true, 10, 45);

    orderBook->modifyOrderLimitPrice(0, 45);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700), nullptr);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getSize(), 2);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getTotalVolume(), 20);
}

// test for changing order size
TEST_F(OrderBookTest, TestChangingOrderSize) {
    
    orderBook->addOrderToBook(true, 10, 45);

    orderBook->modifyOrderSize(0, 20);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getTotalVolume(), 20);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getSize(), 1);
}

// test for adding an instrument to an exchange
TEST_F(OrderBookTest, TestAddingTickerToExchange) {
    
    exchange->addInstrument("TTF 24Q-ICN");
    
    EXPECT_TRUE(exchange -> getOrderBook("TTF 24Q-ICN") != nullptr);
}

// searching for an ob that is not in the exchange
TEST_F(OrderBookTest, TestSearchingForTickerNotInExchange) {
    
    exchange->addInstrument("TTF 24Q-ICN");
    
    EXPECT_TRUE(exchange -> getOrderBook("TTF 24Z-ICN") == nullptr);
}

// adding a limit order to the exchange
TEST_F(OrderBookTest, TestAddingLimitOrderToExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel, 47);
    
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    const std::unordered_map<int64_t, std::unique_ptr<Order>>* allOrders = ob->getAllOrders();
    EXPECT_EQ(allOrders->size(), 1);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
}

// test adding a limit order to the exchange without providing limit price
TEST_F(OrderBookTest, AddingGTCOrderWithoutPrice) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    EXPECT_THROW({
        exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel);
    }, std::invalid_argument);
}

// placing market order in exchage
TEST_F(OrderBookTest, TestAddingMarketOrderToExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel, 47);
    exchange->addOrder(ttfTicker, false, 2, OrderType::Market);
    
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    const std::unordered_map<int64_t, std::unique_ptr<Order>>* allOrders = ob->getAllOrders();
    EXPECT_EQ(allOrders->size(), 1);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getTotalVolume(), 3);
}

// placing modify order size
TEST_F(OrderBookTest, TestModifyOrderSizeFromExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel, 47);
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    exchange -> modifyOrderSize(ttfTicker, 0, 10);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 4700);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getTotalVolume(), 10);
}

// placing modify order limit price
TEST_F(OrderBookTest, TestModifyOrderLimitFromExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel, 47);
    Book* ob = exchange->getOrderBook(ttfTicker);
    
    exchange -> modifyLimitPrice(ttfTicker, 0, 50);
    
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getLimitPrice(), 5000);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getSize(), 1);
    EXPECT_EQ(ob->getBuySide()->getBestLimit()->getTotalVolume(), 5);
}

// retrieving all instruments tickers
TEST_F(OrderBookTest, TestGetTickerList) {
    
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
TEST_F(OrderBookTest, TestRemoveTickerFromExchange) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    exchange->removeInstrument(ttfTicker);
    
    EXPECT_TRUE(exchange -> getOrderBook(ttfTicker) == nullptr);
}

// retrieving best bid and ask from the exchange
TEST_F(OrderBookTest, TestGetNBBO) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel, 100);
    exchange->addOrder(ttfTicker, false, 5, OrderType::GoodTillCancel, 200);
    
    std::pair <std::optional<int>, std::optional<int>> nbbo = exchange -> getNBBO(ttfTicker);
    
    EXPECT_EQ(nbbo.first, 10000);
    EXPECT_EQ(nbbo.second, 20000);
}

// retrieving best bid and ask from the exchange when there is no bid
TEST_F(OrderBookTest, TestGetNBBOWhenNoBid) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, false, 5, OrderType::GoodTillCancel, 200);
    
    std::pair <std::optional<int>, std::optional<int>> nbbo = exchange -> getNBBO(ttfTicker);
    
    EXPECT_TRUE(!nbbo.first.has_value());
    EXPECT_EQ(nbbo.second, 20000);
}

// retrieving best bid and ask from the exchange when there is no ask
TEST_F(OrderBookTest, TestGetNBBOWhenNoAsk) {
    
    std::string ttfTicker = "TTF 24Q-ICN";
    exchange->addInstrument(ttfTicker);
    
    exchange->addOrder(ttfTicker, true, 5, OrderType::GoodTillCancel, 200);
    
    std::pair <std::optional<int>, std::optional<int>> nbbo = exchange -> getNBBO(ttfTicker);
    
    EXPECT_EQ(nbbo.first, 20000);
    EXPECT_TRUE(!nbbo.second.has_value());
}
