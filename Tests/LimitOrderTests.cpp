#include "../src/Book.h"
#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono;

class LimitOrderTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    OrderIdSequence orderIdSequence;

    void SetUp() override {
        orderBook = std::make_unique<Book>();
    }
};

// Test for adding the first buy order
TEST_F(LimitOrderTest, AddingFirstBuyOrder) {
    float orderPrice = 25.09;
    int limitPrice = 2509;
    int volume = 3;
    OrderType orderType = OrderType::Limit;
    
    orderBook->addOrderToBook(Side::Buy, volume, orderPrice, orderType, orderIdSequence);

    int bestPrice = orderBook->getBuySide().getBestPrice();
    Limit* highestBuy = orderBook->getBuySide().getLimit(bestPrice);
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), limitPrice);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHead();
    ASSERT_EQ(headOrder, highestBuy->getTail());
    EXPECT_EQ(headOrder->getShares(), volume);
}

// Test for adding the first sell order
TEST_F(LimitOrderTest, AddingFirstSellOrder) {
    float floatPrice = 30.052;
    int orderPrice = 3005;
    int insertedSize = 5;
    OrderType orderType = OrderType::Limit;
    
    orderBook->addOrderToBook(Side::Sell, insertedSize, floatPrice, orderType, orderIdSequence);

    Limit* lowestSell = orderBook->getSellSide().getBestLimit();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell->getLimitPrice(), orderPrice);
    EXPECT_EQ(lowestSell->getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell->getHead();
    ASSERT_EQ(headOrder, lowestSell->getTail());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
}

// Test for adding worse buy and sell orders
TEST_F(LimitOrderTest, AddingWorseBuyAndSellOrders) {
    
    orderBook->addOrderToBook(Side::Buy, 2, 20.05, OrderType::Limit, orderIdSequence); // Lower buy price
    orderBook->addOrderToBook(Side::Sell, 2, 35.00, OrderType::Limit, orderIdSequence); // Higher sell price

    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getLimitPrice(), 2005);
    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getLimitPrice(), 3500);
}

// Test for adding order to existing limit
TEST_F(LimitOrderTest, AddingOrderToExistingLimit) {
    float price = 25.03;

    orderBook->addOrderToBook(Side::Buy, 3, price, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 2, price, OrderType::Limit, orderIdSequence); // Same price

    Limit* limit = orderBook->getBuySide().getBestLimit();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

// Test for checking all orders in unordered map
TEST_F(LimitOrderTest, CheckAllOrdersUnorderedMap) {
    orderBook->addOrderToBook(Side::Buy, 3, 25.04, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 3, 30.00, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 2, 26.02, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 2, 29.14, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 2, 26.02, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 2, 29.14, OrderType::Limit, orderIdSequence);

    // BIND REFERENCE, DO NOT COPY
    const auto& allOrders = orderBook->getAllOrders();
    EXPECT_EQ(allOrders.size(), 6);

    for (const auto& [orderId, orderPtr] : allOrders) {
        EXPECT_TRUE(orderPtr != nullptr);

        int orderVolume = orderPtr->getShares();
        int limitPrice = orderPtr->getLimit();
        Side orderSide = orderPtr->getOrderSide();

        if (orderVolume == 3 && limitPrice == 2504) {
            EXPECT_EQ(orderSide, Side::Buy);
        } else if (orderVolume == 3 && limitPrice == 3000) {
            EXPECT_EQ(orderSide, Side::Sell);
        } else if (orderVolume == 2 && limitPrice == 2602) {
            EXPECT_EQ(orderSide, Side::Buy);
        } else if (orderVolume == 2 && limitPrice == 2914) {
            EXPECT_EQ(orderSide, Side::Sell);
        } else {
            FAIL() << "Unexpected order found";
        }
    }
}

// Test for adding best buy and sell orders
TEST_F(LimitOrderTest, AddingBestBuyAndSellOrders) {

    orderBook->addOrderToBook(Side::Buy, 3, 25.04, OrderType::Limit, orderIdSequence); // Initial best buy
    orderBook->addOrderToBook(Side::Sell, 3, 30.00, OrderType::Limit, orderIdSequence); // Initial best sell
    orderBook->addOrderToBook(Side::Buy, 2, 26.02, OrderType::Limit, orderIdSequence); // Better buy
    orderBook->addOrderToBook(Side::Sell, 2, 29.14, OrderType::Limit, orderIdSequence); // Better sell

    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getLimitPrice(), 2602);
    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getLimitPrice(), 2914);
}

// Test for adding worst buy limits
TEST_F(LimitOrderTest, Adding3WorstBuyLimits) {

    orderBook->addOrderToBook(Side::Buy, 10, 10.04, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 2, 9, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 5, 9, OrderType::Limit, orderIdSequence);

    int bestPrice = orderBook->getBuySide().getBestLimit()->getLimitPrice();
    
    orderBook->addOrderToBook(Side::Buy, 10, 9, OrderType::Limit, orderIdSequence);
    int vol =orderBook->getBuySide().getTotalVolume();
    
    EXPECT_EQ(bestPrice, 1004);
    EXPECT_EQ(vol, 27);
}

// Test for adding worst sell limits
TEST_F(LimitOrderTest, Adding3WorstSellLimits) {

    orderBook->addOrderToBook(Side::Sell, 30, 30.15, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 40, 31.12, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 45, 31.12, OrderType::Limit, orderIdSequence);

    int bestPrice = orderBook->getSellSide().getBestLimit()->getLimitPrice();
    
    orderBook->addOrderToBook(Side::Sell, 15, 31.12, OrderType::Limit, orderIdSequence);
    int vol =orderBook->getSellSide().getTotalVolume();

    EXPECT_EQ(bestPrice, 3015);
    EXPECT_EQ(vol, 130);
}

// Test for negative order size
TEST_F(LimitOrderTest, NegativeSizeTest) {

    EXPECT_THROW({
        orderBook->addOrderToBook(Side::Buy, -30, 30.00, OrderType::Limit, orderIdSequence);
    }, std::invalid_argument);
}

// Test for negative limit price
TEST_F(LimitOrderTest, NegativeLimitPrice) {

    EXPECT_THROW({
        orderBook->addOrderToBook(Side::Buy, 315, -100, OrderType::Limit, orderIdSequence);
    }, std::invalid_argument);
}

// Test for correct value in order
TEST_F(LimitOrderTest, CorrectValueInOrder) {

    orderBook->addOrderToBook(Side::Sell, 30, 30.15, OrderType::Limit, orderIdSequence);

    auto sellTree = orderBook->getSellSide().getBestLimit();
    auto order = sellTree->getHead();

    EXPECT_EQ(3015, order->getLimit());
    EXPECT_EQ(30, order->getShares());
    EXPECT_EQ(Side::Sell, order->getOrderSide());

    orderBook->addOrderToBook(Side::Sell, 5, 30.15, OrderType::Limit, orderIdSequence);
    sellTree = orderBook->getSellSide().getBestLimit();
    order = sellTree->getHead();
    auto nextOrder = order->getNextOrder();

    EXPECT_EQ(3015, nextOrder->getLimit());
    EXPECT_EQ(Side::Sell, nextOrder->getOrderSide());
    EXPECT_EQ(5, nextOrder->getShares());
}

// Test for sell limit orders crossing the spread
TEST_F(LimitOrderTest, TestSellLimitOrdersCrossingSpread) {

    orderBook->addOrderToBook(Side::Buy, 50, 15, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 40, 7, OrderType::Limit, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getLimitPrice(), 1500);
}

// Test for buy limit orders crossing the spread
TEST_F(LimitOrderTest, TestBuyLimitOrdersCrossingSpread) {

    orderBook->addOrderToBook(Side::Sell, 60, 24.00, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 70, 30.00, OrderType::Limit, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getLimitPrice(), 3000);
}

// Test for limit order filling best sell limit
TEST_F(LimitOrderTest, TestLimitOrderFillsBestSellLimit) {

    orderBook->addOrderToBook(Side::Sell, 35, 24.90, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 100, 30, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 75, 35.00, OrderType::Limit, orderIdSequence);

    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getTotalVolume(), 60);
    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getLimitPrice(), 3000);
}

// Test for limit order filling best buy limit
TEST_F(LimitOrderTest, TestLimitOrderFillsBestBuyLimit) {

    orderBook->addOrderToBook(Side::Buy, 35, 24.90, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 100, 30, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 115, 20, OrderType::Limit, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getLimitPrice(), 2490);
}

// Test for limit order crossing the first level
TEST_F(LimitOrderTest, TestLimitCrossFirstLevel) {

    orderBook->addOrderToBook(Side::Sell, 10, 45, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 5, 40, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 7, 42.50, OrderType::Limit, orderIdSequence);

    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getLimitPrice(), 4500);
    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getTotalVolume(), 2);
    EXPECT_EQ(orderBook->getBuySide().getBestLimit()->getLimitPrice(), 4250);
}

// Test for canceling the full book
TEST_F(LimitOrderTest, LimitCancelFullBook) {
    orderBook->addOrderToBook(Side::Sell, 10, 45, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 5, 40, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 15, 50, OrderType::Limit, orderIdSequence);

    // For Buy side: expect bestPrice to be -1 (no bids)
    EXPECT_EQ(orderBook->getBuySide().getBestPrice(), -1);
    EXPECT_EQ(orderBook->getBuySide().getTotalVolume(), 0);

    // For Sell side: expect bestPrice to be MAX_PRICE + 1 (no asks)
    EXPECT_EQ(orderBook->getSellSide().getBestPrice(), LOBSide<Side::Sell>::MAX_PRICE + 1);
    EXPECT_EQ(orderBook->getSellSide().getTotalVolume(), 0);
}

// Test for canceling a book and placing an order
TEST_F(LimitOrderTest, LimitCancelBookAndPlaceOrder) {
    orderBook->addOrderToBook(Side::Sell, 10, 45, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 5, 40, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 20, 50, OrderType::Limit, orderIdSequence);

    // Buy side: expect best price to be 5000
    EXPECT_EQ(orderBook->getBuySide().getBestPrice(), 5000);
    EXPECT_EQ(orderBook->getBuySide().getTotalVolume(), 5);

    // Confirm the limit exists and has the correct volume
    auto* buyLimit = orderBook->getBuySide().getLimit(5000);
    ASSERT_NE(buyLimit, nullptr);
    EXPECT_EQ(buyLimit->getTotalVolume(), 5);
    EXPECT_EQ(buyLimit->getSize(), 1); // One order remaining at this price

    // Sell side: empty
    EXPECT_EQ(orderBook->getSellSide().getBestPrice(), LOBSide<Side::Sell>::MAX_PRICE + 1);
    EXPECT_EQ(orderBook->getSellSide().getTotalVolume(), 0);
}

// Test for canceling a middle order in a limit
TEST_F(LimitOrderTest, TestCancelMiddleOrderInLimit) {
    orderBook->addOrderToBook(Side::Buy, 10, 47, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 20, 47, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 30, 47, OrderType::Limit, orderIdSequence);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getBuySide().getLimit(4700)->getSize(), 2);
    EXPECT_EQ(orderBook->getBuySide().getLimit(4700)->getHead()->getOrderId(), 0);
    EXPECT_EQ(orderBook->getBuySide().getLimit(4700)->getHead()->getNextOrder()->getOrderId(), 2);
    EXPECT_EQ(orderBook->getBuySide().getLimit(4700)->getTotalVolume(), 40);
    EXPECT_EQ(orderBook->getAllOrders().find(1), orderBook->getAllOrders().end());
}

// Test for canceling the head order
TEST_F(LimitOrderTest, TestCancelHeadOrder) {
    orderBook->addOrderToBook(Side::Buy, 10, 47, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 20, 47, OrderType::Limit, orderIdSequence);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getBuySide().getLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getBuySide().getLimit(4700)->getHead()->getOrderId(), 1);
    EXPECT_EQ(orderBook->getAllOrders().find(0), orderBook->getAllOrders().end());
}

// Test for canceling the tail order
TEST_F(LimitOrderTest, TestCancelTailOrder) {
    orderBook->addOrderToBook(Side::Sell, 10, 47, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 20, 47, OrderType::Limit, orderIdSequence);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide().getLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getSellSide().getLimit(4700)->getHead()->getOrderId(), 0);
    EXPECT_EQ(orderBook->getAllOrders().find(1), orderBook->getAllOrders().end());
}

// Test for canceling order that deletes a limit level
TEST_F(LimitOrderTest, TestCancelOrderThatDeletsLimitLevel) {
    orderBook->addOrderToBook(Side::Sell, 10, 47, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Sell, 20, 45, OrderType::Limit, orderIdSequence);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide().getLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getAllOrders().find(1), orderBook->getAllOrders().end());
    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getLimitPrice(), 4700);
}

// Test for canceling order that deletes the book
TEST_F(LimitOrderTest, TestCancelOrderThatDeletsBook) {
    orderBook->addOrderToBook(Side::Sell, 10, 47, OrderType::Limit, orderIdSequence);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getSellSide().getTotalVolume(), 0);
    EXPECT_EQ(orderBook->getSellSide().getBestLimit(), nullptr);
}

// Test for canceling an order not in the book
TEST_F(LimitOrderTest, CancelOrderNotInBook) {
    EXPECT_THROW({
        orderBook->cancelOrder(10); // order that is not in the book
    }, std::invalid_argument);
}

// Test for modifying order limit
TEST_F(LimitOrderTest, TestModifyingOrderLimit) {
    orderBook->addOrderToBook(Side::Sell, 20, 50, OrderType::Limit, orderIdSequence);

    orderBook->modifyOrderLimitPrice(0, 40, orderIdSequence);

    EXPECT_EQ(orderBook->getSellSide().getLimit(4000)->getSize(), 1);
    EXPECT_EQ(orderBook->getSellSide().getBestLimit()->getLimitPrice(), 4000);
}

// Test for modifying order limit into an existing limit
TEST_F(LimitOrderTest, TestModifyingOrderLimitIntoExistingLimit) {
    orderBook->addOrderToBook(Side::Buy, 10, 47, OrderType::Limit, orderIdSequence);
    orderBook->addOrderToBook(Side::Buy, 10, 45, OrderType::Limit, orderIdSequence);

    orderBook->modifyOrderLimitPrice(0, 45, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide().getLimit(4500)->getSize(), 2);
    EXPECT_EQ(orderBook->getBuySide().getLimit(4500)->getTotalVolume(), 20);
}

// Test for changing order size
TEST_F(LimitOrderTest, TestChangingOrderSize) {
    orderBook->addOrderToBook(Side::Buy, 10, 45, OrderType::Limit, orderIdSequence);
    orderBook->modifyOrderSize(0, 20);

    EXPECT_EQ(orderBook->getBuySide().getLimit(4500)->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide().getLimit(4500)->getSize(), 1);
}
