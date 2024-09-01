#include "quickfix/SessionSettings.h"
#include "quickfix/FileStore.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Log.h"

#include "ExchangeRunner.h"
#include "FixMessageHandler.h"
#include "ExecutionManager.h"

#include <thread>
#include <atomic>


int main(int argc, char** argv) {
    try {
        // Load FIX configuration settings
        FIX::SessionSettings settings("exchangeConfig.cfg");

        // Create the Exchange and ExecutionManager
        Exchange exchange("MyExchange");
        ExecutionManager executionManager(exchange);

        // Create the FIX Message Handler (Application)
        FixMessageHandler fixMessageHandler(exchange);

        // Initialize QuickFIX components
        FIX::FileStoreFactory storeFactory(settings);
        FIX::ScreenLogFactory logFactory(settings);
        FIX::SocketInitiator initiator(fixMessageHandler, storeFactory, settings, logFactory);

        std::atomic<bool> running(true);

        // Run the exchange in a separate thread
        std::thread exchangeThread(runExchange, std::ref(exchange), std::ref(executionManager), std::ref(initiator), std::ref(running));

        // Allow the exchange to run indefinitely
        exchangeThread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
