#ifndef EXCHANGERUNNER_H
#define EXCHANGERUNNER_H

#include "../core/Exchange.hpp"
#include "ExecutionManager.h"
#include <quickfix/SocketAcceptor.h>
#include <atomic>
#include <thread>

void runExchange(Exchange& exchange, ExecutionManager& executionManager, FIX::SocketAcceptor& acceptor, std::atomic<bool>& running) {
    // Start the FIX session
    acceptor.start();

    // Main event loop
    while (running.load()) {
        // Check if there are any new executions to report, if so report them
        executionManager.reportExecution();

        // Sleep or wait for a short duration to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Stop the FIX session
    acceptor.stop();
}

#endif // EXCHANGERUNNER_H
