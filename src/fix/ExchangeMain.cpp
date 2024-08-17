//#include "quickfix/SessionSettings.h"
//#include "quickfix/FileStore.h"
//#include "quickfix/SocketInitiator.h"
//#include "quickfix/Log.h"
//#include "FixMessageHandler.h"
//#include "ExecutionManager.h"
//#include "../core/Exchange.hpp"
//
//int main(int argc, char** argv) {
//    try {
//        // Load FIX configuration settings
//        FIX::SessionSettings settings("config.cfg");
//
//        // Create the Exchange and ExecutionManager
//        Exchange exchange("MyExchange");
//        ExecutionManager executionManager(exchange);
//
//        // Create the FIX Message Handler (Application)
//        FixMessageHandler fixMessageHandler(exchange, executionManager);
//
//        // Initialize QuickFIX components
//        FIX::FileStoreFactory storeFactory(settings);
//        FIX::ScreenLogFactory logFactory(settings);
//        FIX::SocketInitiator initiator(fixMessageHandler, storeFactory, settings, logFactory);
//
//        // Start the FIX session
//        initiator.start();
//
//        // Main event loop
//        while (true) {
//            // Check if there are any new executions to report
//            std::unique_ptr<Execution> execution = executionManager.popNextExecution();
//            if (execution) {
//                Order relatedOrder = exchange.getOrder(execution->orderTakerId); // Assuming you have this function
//                executionManager.reportExecution(relatedOrder, execution->executionType);
//            }
//
//            // Sleep or wait for a short duration to avoid busy-waiting
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        }
//
//        // Stop the FIX session
//        initiator.stop();
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Exception: " << e.what() << std::endl;
//        return 1;
//    }
//
//    return 0;
//}
//
