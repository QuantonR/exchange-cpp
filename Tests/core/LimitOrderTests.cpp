#include <gtest/gtest.h>

#include "../../src/core/Exchange.hpp"
#include "../../src/core/OrderData.h"
#include "../../src/core/Side.hpp"
#include "../../src/core/OrderType.h"
#include "../../src/core/Order.h"
#include "../../src/core/Book.h"


class LimitOrderTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange>exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("myExchange");
        orderBook = std::make_unique<Book>(*exchange);
    }
};

// Test for adding the first buy order
TEST_F(LimitOrderTest, AddingFirstBuyOrder) {
    float orderPrice = 25.09;
    int volume = 3;
    int clientId = 1;
    OrderType orderType = OrderType::Limit;
    
    OrderData orderData(Side::Buy, volume, clientId, orderPrice, orderType);

    orderBook->addOrderToBook(orderData);

    Limit* highestBuy = orderBook->getBuySide()->getBestLimit();
    ASSERT_NE(highestBuy, nullptr);
    EXPECT_EQ(highestBuy->getLimitPrice(), 2509);
    EXPECT_EQ(highestBuy->getSize(), 1);
    EXPECT_EQ(highestBuy->getTotalVolume(), volume);

    Order* headOrder = highestBuy->getHeadOrder();
    ASSERT_EQ(headOrder, highestBuy->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), volume);
}

// Test for adding the first sell order
TEST_F(LimitOrderTest, AddingFirstSellOrder) {
    float floatPrice = 30.052;
    int insertedSize = 5;
    int clientId = 2;
    OrderType orderType = OrderType::Limit;
    
    OrderData orderData(Side::Sell, insertedSize, clientId, floatPrice, orderType);

    orderBook->addOrderToBook(orderData);

    Limit* lowestSell = orderBook->getSellSide()->getBestLimit();
    ASSERT_NE(lowestSell, nullptr);
    EXPECT_EQ(lowestSell->getLimitPrice(), 3005);
    EXPECT_EQ(lowestSell->getSize(), 1);
    EXPECT_EQ(lowestSell->getTotalVolume(), insertedSize);

    Order* headOrder = lowestSell->getHeadOrder();
    ASSERT_EQ(headOrder, lowestSell->getTailOrder());
    EXPECT_EQ(headOrder->getShares(), insertedSize);
}

// Test for adding worse buy and sell orders
TEST_F(LimitOrderTest, AddingWorseBuyAndSellOrders) {
    OrderData buyOrderData(Side::Buy, 2, 3, 20.05, OrderType::Limit);
    OrderData sellOrderData(Side::Sell, 2, 4, 35.00, OrderType::Limit);
    
    orderBook->addOrderToBook(buyOrderData); // Lower buy price
    orderBook->addOrderToBook(sellOrderData); // Higher sell price

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2005);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 3500);
}

// Test for adding order to existing limit
TEST_F(LimitOrderTest, AddingOrderToExistingLimit) {
    float price = 25.03;
    OrderData firstOrderData(Side::Buy, 3, 5, price, OrderType::Limit);
    OrderData secondOrderData(Side::Buy, 2, 6, price, OrderType::Limit);

    orderBook->addOrderToBook(firstOrderData);
    orderBook->addOrderToBook(secondOrderData); // Same price

    Limit* limit = orderBook->getBuySide()->getBestLimit();
    EXPECT_EQ(limit->getSize(), 2); // Two orders at this limit
    EXPECT_EQ(limit->getTotalVolume(), 5); // Combined volume of orders
}

// Test for checking all orders in unordered map
TEST_F(LimitOrderTest, CheckAllOrdersUnorderedMap) {
    OrderData order1(Side::Buy, 3, 7, 25.04, OrderType::Limit);
    OrderData order2(Side::Sell, 3, 8, 30.00, OrderType::Limit);
    OrderData order3(Side::Buy, 2, 9, 26.02, OrderType::Limit);
    OrderData order4(Side::Sell, 2, 10, 29.14, OrderType::Limit);
    OrderData order5(Side::Buy, 2, 11, 26.02, OrderType::Limit);
    OrderData order6(Side::Sell, 2, 12, 29.14, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);
    orderBook->addOrderToBook(order3);
    orderBook->addOrderToBook(order4);
    orderBook->addOrderToBook(order5);
    orderBook->addOrderToBook(order6);

    const std::unordered_map<uint64_t, std::unique_ptr<Order>>* allOrders = orderBook->getAllOrders();
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
    OrderData buyOrder1(Side::Buy, 3, 13, 25.04, OrderType::Limit);
    OrderData sellOrder1(Side::Sell, 3, 14, 30.00, OrderType::Limit);
    OrderData buyOrder2(Side::Buy, 2, 15, 26.02, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 2, 16, 29.14, OrderType::Limit);

    orderBook->addOrderToBook(buyOrder1); // Initial best buy
    orderBook->addOrderToBook(sellOrder1); // Initial best sell
    orderBook->addOrderToBook(buyOrder2); // Better buy
    orderBook->addOrderToBook(sellOrder2); // Better sell

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2602);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 2914);
}

// Test for adding worst buy limits
TEST_F(LimitOrderTest, Adding3WorstBuyLimits) {
    OrderData order1(Side::Buy, 10, 17, 10.04, OrderType::Limit); // Best buy
    OrderData order2(Side::Buy, 2, 18, 9, OrderType::Limit); // Worst buy
    OrderData order3(Side::Buy, 5, 19, 9, OrderType::Limit); // Another worst buy

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);
    orderBook->addOrderToBook(order3);

    auto buyTree = orderBook->getBuySide();

    OrderData order4(Side::Buy, 10, 20, 9, OrderType::Limit); // Another worst buy
    orderBook->addOrderToBook(order4);

    EXPECT_EQ(buyTree->getBestLimit()->getLimitPrice(), 1004);
    EXPECT_EQ(buyTree->getSideVolume(), 27);
}

// Test for adding worst sell limits
TEST_F(LimitOrderTest, Adding3WorstSellLimits) {
    OrderData order1(Side::Sell, 30, 21, 30.15, OrderType::Limit); // Best sell
    OrderData order2(Side::Sell, 40, 22, 31.12, OrderType::Limit); // Worst sell
    OrderData order3(Side::Sell, 45, 23, 31.12, OrderType::Limit); // Another worst sell

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);
    orderBook->addOrderToBook(order3);

    auto sellTree = orderBook->getSellSide();

    OrderData order4(Side::Sell, 15, 24, 31.12, OrderType::Limit); // Another worst sell
    orderBook->addOrderToBook(order4);

    EXPECT_EQ(sellTree->getBestLimit()->getLimitPrice(), 3015);
    EXPECT_EQ(sellTree->getSideVolume(), 130);
}

// Test for negative order size
TEST_F(LimitOrderTest, NegativeSizeTest) {
    int clientId = 25;
    OrderData orderData(Side::Buy, -30, clientId, 30.00, OrderType::Limit); // Negative volume

    EXPECT_THROW({
        orderBook->addOrderToBook(orderData);
    }, std::invalid_argument);
}

// Test for negative limit price
TEST_F(LimitOrderTest, NegativeLimitPrice) {
    int clientId = 26;
    OrderData orderData(Side::Buy, 315, clientId, -100, OrderType::Limit); // Negative price

    EXPECT_THROW({
        orderBook->addOrderToBook(orderData);
    }, std::invalid_argument);
}

// Test for correct value in order
TEST_F(LimitOrderTest, CorrectValueInOrder) {
    int clientId1 = 27;
    int clientId2 = 28;
    OrderData sellOrder(Side::Sell, 30, clientId1, 30.15, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder);

    auto sellTree = orderBook->getSellSide()->getBestLimit();
    auto order = sellTree->getHeadOrder();

    EXPECT_EQ(3015, order->getLimit());
    EXPECT_EQ(30, order->getShares());
    EXPECT_EQ(Side::Sell, order->getOrderSide());

    OrderData secondSellOrder(Side::Sell, 5, clientId2, 30.15, OrderType::Limit);
    orderBook->addOrderToBook(secondSellOrder);
    sellTree = orderBook->getSellSide()->getBestLimit();
    order = sellTree->getHeadOrder();
    auto nextOrder = order->getNextOrder();

    EXPECT_EQ(3015, nextOrder->getLimit());
    EXPECT_EQ(Side::Sell, nextOrder->getOrderSide());
    EXPECT_EQ(5, nextOrder->getShares());
}

// Test for sell limit orders crossing the spread
TEST_F(LimitOrderTest, TestSellLimitOrdersCrossingSpread) {
    OrderData buyOrder(Side::Buy, 50, 29, 15, OrderType::Limit);
    OrderData sellOrder(Side::Sell, 40, 30, 7, OrderType::Limit);

    orderBook->addOrderToBook(buyOrder);
    orderBook->addOrderToBook(sellOrder);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1500);
}

// Test for sell limit orders crossing the spread
TEST_F(LimitOrderTest, LimitsCrossingSpreadInverseOrder) {
    OrderData buyOrder(Side::Buy, 50, 29, 15, OrderType::Limit);
    OrderData sellOrder(Side::Sell, 40, 30, 7, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder);
    orderBook->addOrderToBook(buyOrder);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 1500);
}

// Test for buy limit orders crossing the spread
TEST_F(LimitOrderTest, TestBuyLimitOrdersCrossingSpread) {
    OrderData sellOrder(Side::Sell, 60, 31, 24.00, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 70, 32, 30.00, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder);
    orderBook->addOrderToBook(buyOrder);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 3000);
}

// Test for limit order filling best sell limit
TEST_F(LimitOrderTest, TestLimitOrderFillsBestSellLimit) {
    OrderData sellOrder1(Side::Sell, 35, 33, 24.90, OrderType::Limit); // best sell
    OrderData sellOrder2(Side::Sell, 100, 34, 30, OrderType::Limit); // second level sell
    OrderData buyOrder(Side::Buy, 75, 35, 35.00, OrderType::Limit); // buy order that crosses the spread

    orderBook->addOrderToBook(sellOrder1);
    orderBook->addOrderToBook(sellOrder2);
    orderBook->addOrderToBook(buyOrder);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 60);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 3000);
}

// Test for limit order filling best buy limit
TEST_F(LimitOrderTest, TestLimitOrderFillsBestBuyLimit) {
    OrderData buyOrder1(Side::Buy, 35, 36, 24.90, OrderType::Limit); // first best buy
    OrderData buyOrder2(Side::Buy, 100, 37, 30, OrderType::Limit); // new best buy
    OrderData sellOrder(Side::Sell, 115, 38, 20, OrderType::Limit); // sell that crosses the spread

    orderBook->addOrderToBook(buyOrder1);
    orderBook->addOrderToBook(buyOrder2);
    orderBook->addOrderToBook(sellOrder);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 2490);
}

// Test for limit order crossing the first level
TEST_F(LimitOrderTest, TestLimitCrossFirstLevel) {
    OrderData sellOrder1(Side::Sell, 10, 39, 45, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 5, 40, 40, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 7, 41, 42.50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1);
    orderBook->addOrderToBook(sellOrder2);
    orderBook->addOrderToBook(buyOrder);

    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getTotalVolume(), 10);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4500);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 2);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 4250);
}

// Test for canceling the full book
TEST_F(LimitOrderTest, LimitCancelFullBook) {
    OrderData sellOrder1(Side::Sell, 10, 42, 45, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 5, 43, 40, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 15, 44, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1);
    orderBook->addOrderToBook(sellOrder2);
    orderBook->addOrderToBook(buyOrder);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit(), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
    EXPECT_TRUE(orderBook->getSellSide()->getSideTree().empty());
}

// Test for canceling a book and placing an order
TEST_F(LimitOrderTest, LimitFillsBookAndPlaceOrder) {
    OrderData sellOrder1(Side::Sell, 10, 45, 45, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 5, 46, 40, OrderType::Limit);
    OrderData buyOrder1(Side::Buy, 15, 47, 50, OrderType::Limit);
    OrderData buyOrder2(Side::Buy, 5, 48, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1);
    orderBook->addOrderToBook(sellOrder2);
    orderBook->addOrderToBook(buyOrder1);
    orderBook->addOrderToBook(buyOrder2);

    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getTotalVolume(), 5);
    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 5000);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
    EXPECT_TRUE(orderBook->getSellSide()->getSideTree().empty());
}

// Test for orders with same client id
//TEST_F(LimitOrderTest, TestOrdersSameClientIdNoExecutionBestLimits) {
//    OrderData orderData1(Side::Sell, 20, 58, 50, OrderType::Limit);
//    OrderData orderData2(Side::Buy, 10, 58, 60, OrderType::Limit); // Order that crosses the spread
//
//    orderBook->addOrderToBook(orderData1);
//    orderBook->addOrderToBook(orderData2);
//
//    EXPECT_EQ(orderBook->getSellSide()->findLimit(5000)->getSize(), 1);
//    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 5000);
//    
//    EXPECT_EQ(orderBook->getBuySide()->findLimit(6000)->getSize(), 1);
//    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 6000);
//}
//    
//// Test for orders with same client id
//TEST_F(LimitOrderTest, TestOrdersSameClientIdNoExecution) {
//    OrderData orderData1(Side::Sell, 20, 58, 50, OrderType::Limit);
//    OrderData orderData2(Side::Sell, 30, 59, 45, OrderType::Limit);
//
//    OrderData orderData3(Side::Buy, 40, 58, 60, OrderType::Limit); // Order that crosses the spread
//
//    orderBook->addOrderToBook(orderData1);
//    orderBook->addOrderToBook(orderData2);
//    orderBook->addOrderToBook(orderData3);
//    
//    EXPECT_EQ(orderBook->getSellSide()->findLimit(4500), nullptr);
//    
//    EXPECT_EQ(orderBook->getSellSide()->findLimit(5000)->getSize(), 1);
//    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 5000);
//    
//    EXPECT_EQ(orderBook->getBuySide()->findLimit(6000)->getSize(), 1);
//    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 6000);
//}
//
//// Test for orders with same client id
//TEST_F(LimitOrderTest, TestOrdersSameClientIdWithOtherOrdersInBook) {
//    OrderData orderData1(Side::Sell, 30, 59, 45, OrderType::Limit);
//    OrderData orderData2(Side::Sell, 20, 58, 45, OrderType::Limit);
//    OrderData orderData3(Side::Sell, 30, 60, 45, OrderType::Limit);
//
//    OrderData orderData4(Side::Buy, 70, 58, 60, OrderType::Limit); // Order that crosses the spread
//
//    orderBook->addOrderToBook(orderData1);
//    orderBook->addOrderToBook(orderData2);
//    orderBook->addOrderToBook(orderData3);
//    orderBook->addOrderToBook(orderData4);
//    
//    EXPECT_EQ(orderBook->getSellSide()->findLimit(4500)->getSize(), 1);
//    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 5000);
//    
//    EXPECT_EQ(orderBook->getBuySide()->findLimit(6000)->getSize(), 1);
//    EXPECT_EQ(orderBook->getBuySide()->getBestLimit()->getLimitPrice(), 6000);
//}
