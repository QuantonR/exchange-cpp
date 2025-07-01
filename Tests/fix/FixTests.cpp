#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <memory>

#include "../../src/fix/ExecutionManager.h"
#include "../../src/fix/FixMessageHandler.h"
#include "../../src/fix/ExchangeRunner.h"
#include "../../src/core/Exchange.hpp"
#include "../../src/fix/FixClient.h"

// Declare the runExchange function as extern
extern void runExchange(Exchange& exchange, ExecutionManager& executionManager, FIX::SocketInitiator& initiator, std::atomic<bool>& running);

class FixTest : public ::testing::Test {
protected:
    void SetUp() override {
        running.store(true);

        // Start the exchange process in a thread
        exchangeThread = std::thread(&FixTest::startExchange, this);

        // Wait briefly to ensure the exchange is fully started
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Initialize FixClient with the path to the config file
        fixClient = std::make_unique<FixClient>("../../src/fix/clientConfig.cfg");
        fixClient->start();
    }

    void TearDown() override {
        // Stop the FixClient
        fixClient->stop();
        fixClient.reset();  // Smart pointer will automatically clean up

        // Stop the exchange
        running.store(false);
        if (exchangeThread.joinable()) {
            exchangeThread.join();
        }

        // Clean up the exchange
        exchange.reset();
    }

    void startExchange() {
        FIX::SessionSettings settings("../../src/fix/exchangeConfig.cfg");

        exchange = std::make_unique<Exchange>("MyExchange");
        auto executionManager = std::make_unique<ExecutionManager>(*exchange);
        auto fixMessageHandler = std::make_unique<FixMessageHandler>(*exchange);
        FIX::FileStoreFactory storeFactory(settings);
        FIX::ScreenLogFactory logFactory(settings);
        
        FIX::SocketAcceptor acceptor(*fixMessageHandler, storeFactory, settings, logFactory);

        runExchange(*exchange, *executionManager, acceptor, running);
    }

    std::thread exchangeThread;
    std::atomic<bool> running;
    std::unique_ptr<FixClient> fixClient;
    std::unique_ptr<Exchange> exchange;  // Make exchange a member variable
};

TEST_F(FixTest, SendNewOrderSingleTest) {
    
    // Send a limit order to buy 100 shares of AAPL at $150
    fixClient->sendNewOrderSingle("AAPL", FIX::Side_BUY, 150.00, 100, FIX::OrdType_LIMIT);
    
    // Verify that the order book for "AAPL" was created
    ASSERT_TRUE(exchange->getOrderBook("AAPL") != nullptr);
}
