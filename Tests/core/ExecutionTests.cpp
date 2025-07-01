#include <gtest/gtest.h>

#include "../../src/core/Exchange.hpp"
#include "../../src/core/OrderData.h"
#include "../../src/core/Side.hpp"
#include "../../src/core/OrderType.h"
#include "../../src/core/Order.h"
#include "../../src/core/Book.h"

class ExecutionTest : public ::testing::Test {
protected:
    std::unique_ptr<Book> orderBook;
    std::unique_ptr<Exchange>exchange;

    void SetUp() override {
        exchange = std::make_unique<Exchange>("myExchange");
        orderBook = std::make_unique<Book>(*exchange, "AAPL");
    }
};

// Test for limit order execution
TEST_F(ExecutionTest, TestExecutionForCrossSpreadLimit) {
    OrderData sellOrder(Side::Sell, 14, 45, 45.7, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 20, 47, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder);
    orderBook->addOrderToBook(buyOrder);

    std::unique_ptr<Execution> execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 1);
    EXPECT_EQ(execution->executionSize, 14);
    EXPECT_EQ(execution->executionId, 0);
    EXPECT_EQ(execution->executionPrice, 45.7);
    EXPECT_EQ(execution->makerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 47);
    EXPECT_EQ(execution->makerClientId, 45);
}

// Test for execution on limit orders that complitely fill each other
TEST_F(ExecutionTest, TestExecutionForSameOppositeLimits) {
    OrderData sellOrder(Side::Sell, 14, 45, 45.12, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 14, 46, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder);
    orderBook->addOrderToBook(buyOrder);

    std::unique_ptr<Execution> execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 1);
    EXPECT_EQ(execution->executionSize, 14);
    EXPECT_EQ(execution->executionId, 0);
    EXPECT_EQ(execution->executionPrice, 45.12);
    EXPECT_EQ(execution->makerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 46);
    EXPECT_EQ(execution->makerClientId, 45);
    EXPECT_EQ(execution->makerExecutedQuantity, 14);
    EXPECT_EQ(execution->takerExecutedQuantity, 14);
    EXPECT_NEAR(execution->takerAvgPrice, 45.12, 1e-5);
    EXPECT_NEAR(execution->takerAvgPrice, 45.12, 1e-5);
}

// Test for multiple execution
TEST_F(ExecutionTest, TestMultipleExecutions) {
    OrderData sellOrder1(Side::Sell, 10, 47, 50, OrderType::Limit);
    OrderData sellOrder2(Side::Sell, 14, 45, 45, OrderType::Limit);
    OrderData buyOrder1(Side::Buy, 20, 48, 55, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder1);
    orderBook->addOrderToBook(sellOrder2);
    orderBook->addOrderToBook(buyOrder1);

    std::unique_ptr<Execution> execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 1);
    EXPECT_EQ(execution->orderTakerId, 2);
    EXPECT_EQ(execution->executionSize, 14);
    EXPECT_EQ(execution->executionId, 0);
    EXPECT_EQ(execution->executionPrice, 45);
    EXPECT_EQ(execution->makerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 48);
    EXPECT_EQ(execution->makerClientId, 45);
    EXPECT_EQ(execution->makerExecutedQuantity, 14);
    EXPECT_EQ(execution->takerExecutedQuantity, 14);
    EXPECT_EQ(execution->makerAvgPrice, 45);
    EXPECT_EQ(execution->takerAvgPrice, 45);
    
    execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 2);
    EXPECT_EQ(execution->executionSize, 6);
    EXPECT_EQ(execution->executionId, 1);
    EXPECT_EQ(execution->executionPrice, 50);
    EXPECT_EQ(execution->makerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 48);
    EXPECT_EQ(execution->makerClientId, 47);
    EXPECT_EQ(execution->makerExecutedQuantity, 6);
    EXPECT_EQ(execution->takerExecutedQuantity, 20);
    EXPECT_EQ(execution->makerAvgPrice, 50);
    EXPECT_NEAR(execution->takerAvgPrice, 46.5, 1e-5);
}

// Test for multiple execution
TEST_F(ExecutionTest, TestExecutionFor2LimitCrossSpread) {
    OrderData sellOrder(Side::Sell, 14, 45, 45, OrderType::Limit);
    OrderData buyOrder(Side::Buy, 10, 47, 50, OrderType::Limit);

    orderBook->addOrderToBook(sellOrder);
    orderBook->addOrderToBook(buyOrder);

    std::unique_ptr<Execution> execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 1);
    EXPECT_EQ(execution->executionSize, 10);
    EXPECT_EQ(execution->executionId, 0);
    EXPECT_EQ(execution->executionPrice, 45);
    EXPECT_EQ(execution->makerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 47);
    EXPECT_EQ(execution->makerClientId, 45);
    EXPECT_EQ(execution->makerExecutedQuantity, 10);
    EXPECT_EQ(execution->takerExecutedQuantity, 10);
    EXPECT_EQ(execution->makerAvgPrice, 45);
    EXPECT_EQ(execution->takerAvgPrice, 45);
    
    OrderData buyOrder1(Side::Buy, 20, 48, 50, OrderType::Limit);
    orderBook->addOrderToBook(buyOrder1);

    execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 2);
    EXPECT_EQ(execution->executionSize, 4);
    EXPECT_EQ(execution->executionId, 1);
    EXPECT_EQ(execution->executionPrice, 45);
    EXPECT_EQ(execution->makerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 48);
    EXPECT_EQ(execution->makerClientId, 45);
    EXPECT_EQ(execution->makerExecutedQuantity, 14);
    EXPECT_EQ(execution->takerExecutedQuantity, 4);
    EXPECT_EQ(execution->makerAvgPrice, 45);
    EXPECT_EQ(execution->takerAvgPrice, 45);
}

// Market orders execution tests
TEST_F(ExecutionTest, ExecutionSingleMarketOrder) {
    
    OrderData orderData1 = OrderData(Side::Sell, 3, 1, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Buy, 1, 3, OrderType::Market);
    orderBook->addOrderToBook(orderData1);
    orderBook->placeMarketOrder(orderData2);
    
    std::unique_ptr<Execution> execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 1);
    EXPECT_EQ(execution->executionSize, 1);
    EXPECT_EQ(execution->executionId, 0);
    EXPECT_EQ(execution->executionPrice, 30);
    EXPECT_EQ(execution->makerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 3);
    EXPECT_EQ(execution->makerClientId, 1);
    EXPECT_EQ(execution->makerExecutedQuantity, 1);
    EXPECT_EQ(execution->takerExecutedQuantity, 1);
    EXPECT_EQ(execution->makerAvgPrice, 30);
    EXPECT_EQ(execution->takerAvgPrice, 30);
}

// Market orders execution tests
TEST_F(ExecutionTest, ExecutionSingleMarketOrderThatFills2Limits) {
    
    OrderData orderData1 = OrderData(Side::Sell, 3, 1, 30, OrderType::Limit);
    OrderData orderData2 = OrderData(Side::Sell, 10, 2, 40, OrderType::Limit);
    OrderData orderData3 = OrderData(Side::Buy, 7, 3, OrderType::Market);
    
    orderBook->addOrderToBook(orderData1);
    orderBook->addOrderToBook(orderData2);
    orderBook->placeMarketOrder(orderData3);
    
    std::unique_ptr<Execution> execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 0);
    EXPECT_EQ(execution->orderTakerId, 2);
    EXPECT_EQ(execution->executionSize, 3);
    EXPECT_EQ(execution->executionId, 0);
    EXPECT_EQ(execution->executionPrice, 30);
    EXPECT_EQ(execution->makerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy); 
    EXPECT_EQ(execution->takerClientId, 3);
    EXPECT_EQ(execution->makerClientId, 1);
    EXPECT_EQ(execution->makerExecutedQuantity, 3);
    EXPECT_EQ(execution->takerExecutedQuantity, 3);
    EXPECT_EQ(execution->makerAvgPrice, 30);
    EXPECT_EQ(execution->takerAvgPrice, 30);
    
    execution = exchange->popNextExecution();
    EXPECT_EQ(execution->orderMakerId, 1);
    EXPECT_EQ(execution->orderTakerId, 2);
    EXPECT_EQ(execution->executionSize, 4);
    EXPECT_EQ(execution->executionId, 1);
    EXPECT_EQ(execution->executionPrice, 40);
    EXPECT_EQ(execution->makerExecType, ExecutionType::PartialFill);
    EXPECT_EQ(execution->takerExecType, ExecutionType::FullFill);
    EXPECT_EQ(execution->sideMaker, Side::Sell);
    EXPECT_EQ(execution->sideTaker, Side::Buy);
    EXPECT_EQ(execution->takerClientId, 3);
    EXPECT_EQ(execution->makerClientId, 2);
    EXPECT_EQ(execution->makerExecutedQuantity, 4);
    EXPECT_EQ(execution->takerExecutedQuantity, 7);
    EXPECT_EQ(execution->makerAvgPrice, 40);
    EXPECT_NEAR(execution->takerAvgPrice, 35.71, 1e-5);
}
