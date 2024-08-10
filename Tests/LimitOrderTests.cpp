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
    
    OrderData orderData(Side::Buy, volume, orderPrice, orderType);

    orderBook->addOrderToBook(orderData, orderIdSequence);

    Limit* highestBuy = orderBook->getBuySide()->getBestLimit();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), limitPrice);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), volume);
}

// Test for adding the first sell order
TEST_F(LimitOrderTest, AddingFirstSellOrder) {
    float floatPrice = 30.052;
    int orderPrice = 3005;
    int insertedSize = 5;
    OrderType orderType = OrderType::Limit;
    
    OrderData orderData(Side::Sell, insertedSize, floatPrice, orderType);

    orderBook->addOrderToBook(orderData, orderIdSequence);

    Limit* lowestSell = orderBook->getSellSide()->getBestLimit();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell->getLimitPrice(), orderPrice);
    EXPECT_EQ(lowestSell->getSize(), 1);
    EXPECT_EQ(lowestSell->getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell->getHeadOrder();
    ASSERT_EQ(headOrder, lowestSell->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
}

// Test for adding worse buy and sell orders
TEST_F(LimitOrderTest, AddingWorseBuyAndSellOrders) {
    OrderData buyOrderData(Side::Buy, 2, 20.05, OrderType::Limit);
    OrderData sellOrderData(Side::Sell, 2, 35.00, OrderType::Limit);
    
    orderBook->addOrderToBook(buyOrderData, orderIdSequence); // Lower buy price
    orderBook->addOrderToBook(sellOrderData, orderIdSequence); // Higher sell price

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2005);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 3500);
}

// Test for adding order to existing limit
TEST_F(LimitOrderTest, AddingOrderToExistingLimit) {
    float price = 25.03;
    OrderData firstOrderData(Side::Buy, 3, price, OrderType::Limit);
    OrderData secondOrderData(Side::Buy, 2, price, OrderType::Limit);

    orderBook->addOrderToBook(firstOrderData, orderIdSequence);
    orderBook->addOrderToBook(secondOrderData, orderIdSequence); // Same price

    Limit* limit = orderBook->getBuySide()->getBestLimit();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

// Test for checking all orders in unordered map
TEST_F(LimitOrderTest, CheckAllOrdersUnorderedMap) {
    OrderData order1(Side::Buy, 3, 25.04, OrderType::Limit);
    OrderData order2(Side::Sell, 3, 30.00, OrderType::Limit);
    OrderData order3(Side::Buy, 2, 26.02, OrderType::Limit);
    OrderData order4(Side::Sell, 2, 29.14, OrderType::Limit);
    OrderData order5(Side::Buy, 2, 26.02, OrderType::Limit);
    OrderData order6(Side::Sell, 2, 29.14, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);
    orderBook->addOrderToBook(order3, orderIdSequence);
    orderBook->addOrderToBook(order4, orderIdSequence);
    orderBook->addOrderToBook(order5, orderIdSequence);
    orderBook->addOrderToBook(order6, orderIdSequence);

    const std::unordered_map<int64_t, std::unique_ptr<Order>>* allOrders = orderBook->getAllOrders();
    EXPECT_EQ(allOrders->size(), 6);

    // Verify each order
    for (const auto& [orderId, orderPtr] : *allOrders) {
        EXPECT_TRUE(orderPtr != nullptr);  // Ensure the order pointer is not null

        // Access order properties and verify them
        int orderVolume = orderPtr->getShares();
        int limitPrice = orderPtr->getLimit();  // Assuming limitPrice is stored as an int (cents)
        Side orderSide = orderPtr->getOrderSide();

        // Check for specific orders
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
    OrderData buyOrder1(Side::Buy, 3, 25.04, OrderType::Limit);
    OrderData sellOrder1(Side::Sell, 3, 30.00, OrderType::Limit);
    OrderData buyOrder2(Side::Buy, 2, 26.02, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 2, 29.14, OrderType::Limit);

    orderBook->addOrderToBook(buyOrder1, orderIdSequence); // Initial best buy
    orderBook->addOrderToBook(sellOrder1, orderIdSequence); // Initial best sell
    orderBook->addOrderToBook(buyOrder2, orderIdSequence); // Better buy
    orderBook->addOrderToBook(sellOrder2, orderIdSequence); // Better sell

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2602);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 2914);
}

// Test for adding worst buy limits
TEST_F(LimitOrderTest, Adding3WorstBuyLimits) {
    OrderData order1(Side::Buy, 10, 10.04, OrderType::Limit); // Best buy
    OrderData order2(Side::Buy, 2, 9, OrderType::Limit); // Worst buy
    OrderData order3(Side::Buy, 5, 9, OrderType::Limit); // Another worst buy

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);
    orderBook->addOrderToBook(order3, orderIdSequence);

    auto buyTree = orderBook->getBuySide();

    OrderData order4(Side::Buy, 10, 9, OrderType::Limit); // Another worst buy
    orderBook->addOrderToBook(order4, orderIdSequence);

    EXPECT_EQ(buyTree->getBestLimit()->getLimitPrice(), 1004);
    EXPECT_EQ(buyTree->getSideVolume(), 27);
}

// Test for adding worst sell limits
TEST_F(LimitOrderTest, Adding3WorstSellLimits) {
    OrderData order1(Side::Sell, 30, 30.15, OrderType::Limit); // Best sell
    OrderData order2(Side::Sell, 40, 31.12, OrderType::Limit); // Worst sell
    OrderData order3(Side::Sell, 45, 31.12, OrderType::Limit); // Another worst sell

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);
    orderBook->addOrderToBook(order3, orderIdSequence);

    auto sellTree = orderBook->getSellSide();

    OrderData order4(Side::Sell, 15, 31.12, OrderType::Limit); // Another worst sell
    orderBook->addOrderToBook(order4, orderIdSequence);

    EXPECT_EQ(sellTree->getBestLimit()->getLimitPrice(), 3015);
    EXPECT_EQ(sellTree->getSideVolume(), 130);
}

// Test for negative order size
TEST_F(LimitOrderTest, NegativeSizeTest) {
    OrderData orderData(Side::Buy, -30, 30.00, OrderType::Limit); // Negative volume

    EXPECT_THROW({
        orderBook->addOrderToBook(orderData, orderIdSequence);
    }, std::invalid_argument);
}

// Test for negative limit price
TEST_F(LimitOrderTest, NegativeLimitPrice) {
    OrderData orderData(Side::Buy, 315, -100, OrderType::Limit); // Negative price

    EXPECT_THROW({
        orderBook->addOrderToBook(orderData, orderIdSequence);
    }, std::invalid_argument);
}

// Test for correct value in order
TEST_F(LimitOrderTest, CorrectValueInOrder) {
    OrderData sellOrder(Side::Sell, 30, 30.15, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder, orderIdSequence);

    auto sellTree = orderBook->getSellSide()->getBestLimit();
    auto order = sellTree->getHeadOrder();

    EXPECT_EQ(3015, order->getLimit());
    EXPECT_EQ(30, order->getShares());
    EXPECT_EQ(Side::Sell, order->getOrderSide());

    OrderData secondSellOrder(Side::Sell, 5, 30.15, OrderType::Limit);
    orderBook->addOrderToBook(secondSellOrder, orderIdSequence);
    sellTree = orderBook->getSellSide()->getBestLimit();
    order = sellTree->getHeadOrder();
    auto nextOrder = order->getNextOrder();

    EXPECT_EQ(3015, nextOrder->getLimit());
    EXPECT_EQ(Side::Sell, nextOrder->getOrderSide());
    EXPECT_EQ(5, nextOrder->getShares());
}

// Test for sell limit orders crossing the spread
TEST_F(LimitOrderTest, TestSellLimitOrdersCrossingSpread) {
    OrderData buyOrder(Side::Buy, 50, 15, OrderType::Limit);
    OrderData sellOrder(Side::Sell, 40, 7, OrderType::Limit);

    orderBook->addOrderToBook(buyOrder, orderIdSequence);
    orderBook->addOrderToBook(sellOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1500);
}

// Test for buy limit orders crossing the spread
TEST_F(LimitOrderTest, TestBuyLimitOrdersCrossingSpread) {
    OrderData sellOrder(Side::Sell, 60, 24.00, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 70, 30.00, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder, orderIdSequence);
    orderBook->addOrderToBook(buyOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 3000);
}

// Test for limit order filling best sell limit
TEST_F(LimitOrderTest, TestLimitOrderFillsBestSellLimit) {
    OrderData sellOrder1(Side::Sell, 35, 24.90, OrderType::Limit); // best sell
    OrderData sellOrder2(Side::Sell, 100, 30, OrderType::Limit); // second level sell
    OrderData buyOrder(Side::Buy, 75, 35.00, OrderType::Limit); // buy order that crosses the spread

    orderBook->addOrderToBook(sellOrder1, orderIdSequence);
    orderBook->addOrderToBook(sellOrder2, orderIdSequence);
    orderBook->addOrderToBook(buyOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 60);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 3000);
}

// Test for limit order filling best buy limit
TEST_F(LimitOrderTest, TestLimitOrderFillsBestBuyLimit) {
    OrderData buyOrder1(Side::Buy, 35, 24.90, OrderType::Limit); // first best buy
    OrderData buyOrder2(Side::Buy, 100, 30, OrderType::Limit); // new best buy
    OrderData sellOrder(Side::Sell, 115, 20, OrderType::Limit); // sell that crosses the spread

    orderBook->addOrderToBook(buyOrder1, orderIdSequence);
    orderBook->addOrderToBook(buyOrder2, orderIdSequence);
    orderBook->addOrderToBook(sellOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2490);
}

// Test for limit order crossing the first level
TEST_F(LimitOrderTest, TestLimitCrossFirstLevel) {
    OrderData sellOrder1(Side::Sell, 10, 45, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 5, 40, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 7, 42.50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1, orderIdSequence);
    orderBook->addOrderToBook(sellOrder2, orderIdSequence);
    orderBook->addOrderToBook(buyOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4500);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 2);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 4250);
}

// Test for canceling the full book
TEST_F(LimitOrderTest, LimitCancelFullBook) {
    OrderData sellOrder1(Side::Sell, 10, 45, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 5, 40, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 15, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1, orderIdSequence);
    orderBook->addOrderToBook(sellOrder2, orderIdSequence);
    orderBook->addOrderToBook(buyOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
    EXPECT_TRUE(orderBook->getSellSide()->getSideTree().empty());
}

// Test for canceling a book and placing an order
TEST_F(LimitOrderTest, LimitCancelBookAndPlaceOrder) {
    OrderData sellOrder1(Side::Sell, 10, 45, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 5, 40, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 20, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1, orderIdSequence);
    orderBook->addOrderToBook(sellOrder2, orderIdSequence);
    orderBook->addOrderToBook(buyOrder, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 5);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 5000);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
    EXPECT_TRUE(orderBook->getSellSide()->getSideTree().empty());
}

// Test for canceling a middle order in a limit
TEST_F(LimitOrderTest, TestCancelMiddleOrderInLimit) {
    OrderData order1(Side::Buy, 10, 47, OrderType::Limit);
    OrderData order2(Side::Buy, 20, 47, OrderType::Limit);
    OrderData order3(Side::Buy, 30, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);
    orderBook->addOrderToBook(order3, orderIdSequence);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getSize(), 2);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), 0);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getNextOrder()->getOrderId(), 2);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getTotalVolume(), 40);
    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
}

// Test for canceling the head order
TEST_F(LimitOrderTest, TestCancelHeadOrder) {
    OrderData order1(Side::Buy, 10, 47, OrderType::Limit);
    OrderData order2(Side::Buy, 20, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), 1);
    EXPECT_EQ(orderBook->getAllOrders()->find(0), orderBook->getAllOrders()->end());
}

// Test for canceling the tail order
TEST_F(LimitOrderTest, TestCancelTailOrder) {
    OrderData order1(Side::Sell, 10, 47, OrderType::Limit);
    OrderData order2(Side::Sell, 20, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getHeadOrder()->getOrderId(), 0);
    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
}

// Test for canceling order that deletes a limit level
TEST_F(LimitOrderTest, TestCancelOrderThatDeletsLimitLevel) {
    OrderData order1(Side::Sell, 10, 47, OrderType::Limit);
    OrderData order2(Side::Sell, 20, 45, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4500), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4700);
}

// Test for canceling order that deletes the book
TEST_F(LimitOrderTest, TestCancelOrderThatDeletsBook) {
    OrderData order1(Side::Sell, 10, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getSideVolume(), 0);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
}

// Test for canceling an order not in the book
TEST_F(LimitOrderTest, CancelOrderNotInBook) {
    EXPECT_THROW({
        orderBook->cancelOrder(10); // order that is not in the book
    }, std::invalid_argument);
}

// Test for modifying order limit
TEST_F(LimitOrderTest, TestModifyingOrderLimit) {
    OrderData orderData(Side::Sell, 20, 50, OrderType::Limit);
    orderBook->addOrderToBook(orderData, orderIdSequence);

    orderBook->modifyOrderLimitPrice(0, 40, orderIdSequence);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4750), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->findLimit(4000)->getSize(), 1);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4000);
}

// Test for modifying order limit into an existing limit
TEST_F(LimitOrderTest, TestModifyingOrderLimitIntoExistingLimit) {
    OrderData order1(Side::Buy, 10, 47, OrderType::Limit);
    OrderData order2(Side::Buy, 10, 45, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->addOrderToBook(order2, orderIdSequence);

    orderBook->modifyOrderLimitPrice(0, 45, orderIdSequence);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700), nullptr);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getSize(), 2);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getTotalVolume(), 20);
}

// Test for changing order size
TEST_F(LimitOrderTest, TestChangingOrderSize) {
    OrderData order1(Side::Buy, 10, 45, OrderType::Limit);

    orderBook->addOrderToBook(order1, orderIdSequence);
    orderBook->modifyOrderSize(0, 20);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getSize(), 1);
}
