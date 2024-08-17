//#include "FixMessageHandler.h"
//
//void FixExchange::onMessage(const FIX42::NewOrderSingle& message, const FIX::SessionID& sessionID) {
//    try {
//        FIX::ClOrdID clOrdID;
//        FIX::Symbol symbol;
//        FIX::Side side;
//        FIX::OrdType ordType;
//        FIX::Price price;
//        FIX::OrderQty orderQty;
//        FIX::TimeInForce timeInForce(FIX::TimeInForce_DAY);
//
//        message.get(clOrdID);
//        message.get(symbol);
//        message.get(side);
//        message.get(ordType);
//        if (ordType == FIX::OrdType_LIMIT)
//            message.get(price);
//        message.get(orderQty);
//        message.getFieldIfSet(timeInForce);
//
//        if (timeInForce != FIX::TimeInForce_DAY)
//            throw std::logic_error("Unsupported TIF, use Day");
//
//        // Translate FIX message to internal order data structure
//        Side orderSide = (side == FIX::Side_BUY) ? Side::Buy : Side::Sell;
//        OrderType orderType = (ordType == FIX::OrdType_LIMIT) ? OrderType::Limit : OrderType::Market;
//        double limitPrice = (ordType == FIX::OrdType_LIMIT) ? price.getValue() : 0.0;
//
//        OrderData orderData(orderSide, static_cast<int>(orderQty), limitPrice, orderType);
//        exchange.addOrder(symbol.getValue(), orderData);
//
//        // Send back an execution report
//        FIX42::ExecutionReport execReport;
//        execReport.set(FIX::OrderID("1"));
//        execReport.set(FIX::ExecID("123"));
//        execReport.set(FIX::ExecTransType(FIX::ExecTransType_NEW));
//        execReport.set(FIX::ExecType(FIX::ExecType_NEW));
//        execReport.set(FIX::OrdStatus(FIX::OrdStatus_NEW));
//        execReport.set(clOrdID);
//        execReport.set(symbol);
//        execReport.set(side);
//        execReport.set(FIX::LeavesQty(orderQty));
//        execReport.set(FIX::Price(price));
//        execReport.set(FIX::Text("Order accepted"));
//
//        FIX::Session::sendToTarget(execReport, sessionID);
//    } catch (const std::exception& e) {
//        std::cerr << "Error processing NewOrderSingle: " << e.what() << std::endl;
//    }
//}
