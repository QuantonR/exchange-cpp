//    #include "ExecutionManager.h"
//
//    void ExecutionManager::reportExecution(const Order& order, ExecutionType execType)
//    {
//        char status = (execType == ExecutionType::FullFill) ? FIX::OrdStatus_FILLED : FIX::OrdStatus_PARTIALLY_FILLED;
//        updateOrder(order, status);
//    }
//
//    void ExecutionManager::updateOrder(const Order& order, char status)
//    {
//        FIX::TargetCompID targetCompID(order.getOwner());
//        FIX::SenderCompID senderCompID(order.getTarget());
//
//        FIX42::ExecutionReport fixOrder(
//            FIX::OrderID(order.getClientID()),
//            FIX::ExecID(generateExecutionID()), // You will need a method to generate execution IDs
//            FIX::ExecTransType(FIX::ExecTransType_NEW),
//            FIX::ExecType(status),
//            FIX::OrdStatus(status),
//            FIX::Symbol(order.getSymbol()),
//            FIX::Side(convert(order.getSide())),
//            FIX::LeavesQty(order.getOpenQuantity()),
//            FIX::CumQty(order.getExecutedQuantity()),
//            FIX::AvgPx(order.getAvgExecutedPrice())
//        );
//
//        fixOrder.set(FIX::ClOrdID(order.getClientID()));
//        fixOrder.set(FIX::OrderQty(order.getQuantity()));
//
//        if (status == FIX::OrdStatus_FILLED || status == FIX::OrdStatus_PARTIALLY_FILLED)
//        {
//            fixOrder.set(FIX::LastShares(order.getLastExecutedQuantity()));
//            fixOrder.set(FIX::LastPx(order.getLastExecutedPrice()));
//        }
//
//        try
//        {
//            FIX::Session::sendToTarget(fixOrder, senderCompID, targetCompID);
//        }
//        catch (const FIX::SessionNotFound&)
//        {
//            std::cerr << "Error: Session not found for execution report." << std::endl;
//        }
//    }
