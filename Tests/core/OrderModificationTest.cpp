#include <gtest/gtest.h>

#include "../../src/core/Exchange.hpp"
#include "../../src/core/OrderData.h"
#include "../../src/core/Side.hpp"
#include "../../src/core/OrderType.h"
#include "../../src/core/Order.h"
#include "../../src/core/Book.h"

class OrderModificationTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange>exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("myExchange");
        orderBook = std::make_unique<Book>(*exchange, "AAPL");
    }
};

// Test for canceling a middle order in a limit
TEST_F(OrderModificationTest, TestCancelMiddleOrderInLimit) {
    OrderData order1(Side::Buy, 10, 48, 47, OrderType::Limit);
    OrderData order2(Side::Buy, 20, 49, 47, OrderType::Limit);
    OrderData order3(Side::Buy, 30, 50, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);
    orderBook->addOrderToBook(order3);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getSize(), 2);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), 0);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getNextOrder()->getOrderId(), 2);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getTotalVolume(), 40);
    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
}

// Test for canceling the head order
TEST_F(OrderModificationTest, TestCancelHeadOrder) {
    OrderData order1(Side::Buy, 10, 51, 47, OrderType::Limit);
    OrderData order2(Side::Buy, 20, 52, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700)->getHeadOrder()->getOrderId(), 1);
    EXPECT_EQ(orderBook->getAllOrders()->find(0), orderBook->getAllOrders()->end());
}

// Test for canceling the tail order
TEST_F(OrderModificationTest, TestCancelTailOrder) {
    OrderData order1(Side::Sell, 10, 53, 47, OrderType::Limit);
    OrderData order2(Side::Sell, 20, 54, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getHeadOrder()->getOrderId(), 0);
    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
}

// Test for canceling order that deletes a limit level
TEST_F(OrderModificationTest, TestCancelOrderThatDeletsLimitLevel) {
    OrderData order1(Side::Sell, 10, 55, 47, OrderType::Limit);
    OrderData order2(Side::Sell, 20, 56, 45, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);

    orderBook->cancelOrder(1);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4500), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700)->getSize(), 1);
    EXPECT_EQ(orderBook->getAllOrders()->find(1), orderBook->getAllOrders()->end());
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4700);
}

// Test for canceling order that deletes the book
TEST_F(OrderModificationTest, TestCancelOrderThatDeletsBook) {
    OrderData order1(Side::Sell, 10, 57, 47, OrderType::Limit);

    orderBook->addOrderToBook(order1);

    orderBook->cancelOrder(0);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4700), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->getSideVolume(), 0);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit(), nullptr);
}

// Test for canceling an order not in the book
TEST_F(OrderModificationTest, CancelOrderNotInBook) {
    EXPECT_THROW({
        orderBook->cancelOrder(10); // order that is not in the book
    }, std::invalid_argument);
}

// Test for modifying order limit
TEST_F(OrderModificationTest, TestModifyingOrderLimit) {
    OrderData orderData(Side::Sell, 20, 58, 50, OrderType::Limit);
    orderBook->addOrderToBook(orderData);

    orderBook->modifyOrderLimitPrice(0, 40);

    EXPECT_EQ(orderBook->getSellSide()->findLimit(4750), nullptr);
    EXPECT_EQ(orderBook->getSellSide()->findLimit(4000)->getSize(), 1);
    EXPECT_EQ(orderBook->getSellSide()->getBestLimit()->getLimitPrice(), 4000);
}

// Test for modifying order limit into an existing limit
TEST_F(OrderModificationTest, TestModifyingOrderLimitIntoExistingLimit) {
    OrderData order1(Side::Buy, 10, 59, 47, OrderType::Limit);
    OrderData order2(Side::Buy, 10, 60, 45, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->addOrderToBook(order2);

    orderBook->modifyOrderLimitPrice(0, 45);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4700), nullptr);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getSize(), 2);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getTotalVolume(), 20);
}

// Test for changing order size
TEST_F(OrderModificationTest, TestChangingOrderSize) {
    OrderData order1(Side::Buy, 10, 61, 45, OrderType::Limit);

    orderBook->addOrderToBook(order1);
    orderBook->modifyOrderSize(0, 20);

    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getTotalVolume(), 20);
    EXPECT_EQ(orderBook->getBuySide()->findLimit(4500)->getSize(), 1);
}

