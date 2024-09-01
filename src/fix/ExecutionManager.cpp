#include "ExecutionManager.h"
#include <iostream>
#include <string>

ExecutionManager::ExecutionManager(Exchange& exchange) : exchange(exchange){}

void ExecutionManager::reportExecution() {
    auto firstExecution = exchange.popNextExecution();
    while (firstExecution) {
        updateOrder(std::move(firstExecution));
        firstExecution = exchange.popNextExecution();
    }
}

void ExecutionManager::updateOrder(std::unique_ptr<Execution> executionToReport) {
    
    // Set common fields for maker and taker
    FIX::ExecTransType execTransType(FIX::ExecTransType_NEW);
    
    // --------- Update Maker's Order ---------
    {
        FIX::TargetCompID targetCompID("1"); // Assuming "1" is a placeholder for the correct ID
        FIX::SenderCompID senderCompID(std::to_string(executionToReport->makerClientId));

        FIX::OrdStatus makerStatus = (executionToReport->makerExecType == ExecutionType::FullFill)
            ? FIX::OrdStatus_FILLED
            : FIX::OrdStatus_PARTIALLY_FILLED;

        FIX::Side makerFixSide = (executionToReport->sideMaker == Side::Buy)
            ? FIX::Side_BUY
            : FIX::Side_SELL;

        FIX42::ExecutionReport makerFixOrder(
            FIX::OrderID(std::to_string(executionToReport->orderMakerId)),
            FIX::ExecID(std::to_string(executionToReport->executionId)),
            execTransType,
            FIX::ExecType(makerStatus),
            makerStatus,
            FIX::Symbol(executionToReport->symbol),
            makerFixSide,
            FIX::LeavesQty(executionToReport->makerOpenQuantity),
            FIX::CumQty(executionToReport->makerExecutedQuantity),
            FIX::AvgPx(executionToReport->makerAvgPrice)
        );

        makerFixOrder.set(FIX::ClOrdID(std::to_string(executionToReport->makerClientId)));
        makerFixOrder.set(FIX::OrderQty(executionToReport->makerOpenQuantity));

        if (makerStatus == FIX::OrdStatus_FILLED || makerStatus == FIX::OrdStatus_PARTIALLY_FILLED)
        {
            makerFixOrder.set(FIX::LastShares(executionToReport->executionSize));
            makerFixOrder.set(FIX::LastPx(executionToReport->executionPrice));
        }

        try
        {
            FIX::Session::sendToTarget(makerFixOrder, senderCompID, targetCompID);
        }
        catch (const FIX::SessionNotFound&)
        {
            std::cerr << "Error: Session not found for maker's execution report." << std::endl;
        }
    }

    // --------- Update Taker's Order ---------
    {
        FIX::TargetCompID targetCompID("1"); // Assuming "1" is a placeholder for the correct ID
        FIX::SenderCompID senderCompID(std::to_string(executionToReport->takerClientId));

        FIX::OrdStatus takerStatus = (executionToReport->takerExecType == ExecutionType::FullFill)
            ? FIX::OrdStatus_FILLED
            : FIX::OrdStatus_PARTIALLY_FILLED;

        FIX::Side takerFixSide = (executionToReport->sideTaker == Side::Buy)
            ? FIX::Side_BUY
            : FIX::Side_SELL;

        FIX42::ExecutionReport takerFixOrder(
            FIX::OrderID(std::to_string(executionToReport->orderTakerId)),
            FIX::ExecID(std::to_string(executionToReport->executionId)),
            execTransType,
            FIX::ExecType(takerStatus),
            takerStatus,
            FIX::Symbol(executionToReport->symbol),
            takerFixSide,
            FIX::LeavesQty(executionToReport->takerOpenQuantity),
            FIX::CumQty(executionToReport->takerExecutedQuantity),
            FIX::AvgPx(executionToReport->takerAvgPrice)
        );

        takerFixOrder.set(FIX::ClOrdID(std::to_string(executionToReport->takerClientId)));
        takerFixOrder.set(FIX::OrderQty(executionToReport->takerOpenQuantity));

        if (takerStatus == FIX::OrdStatus_FILLED || takerStatus == FIX::OrdStatus_PARTIALLY_FILLED)
        {
            takerFixOrder.set(FIX::LastShares(executionToReport->executionSize));
            takerFixOrder.set(FIX::LastPx(executionToReport->executionPrice));
        }

        try
        {
            FIX::Session::sendToTarget(takerFixOrder, senderCompID, targetCompID);
        }
        catch (const FIX::SessionNotFound&)
        {
            std::cerr << "Error: Session not found for taker's execution report." << std::endl;
        }
    }
}
