#include "FixMessageHandler.h"

// Handle logon event - this might be used to initialize state or log the event
void FixMessageHandler::onLogon(const FIX::SessionID& sessionID) {
    std::cout << "Logon - Session: " << sessionID << std::endl;
}

// Handle logout event - this might be used to clean up state or log the event
void FixMessageHandler::onLogout(const FIX::SessionID& sessionID) {
    std::cout << "Logout - Session: " << sessionID << std::endl;
}

// Handle messages received from the application (FIX session)
void FixMessageHandler::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
    crack(message, sessionID);  // Direct the message to the appropriate onMessage handler
}

void FixMessageHandler::onMessage(const FIX42::NewOrderSingle& message, const FIX::SessionID& sessionID) {
    try {
        FIX::ClOrdID clOrdID;
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::Price price;
        FIX::OrderQty orderQty;
        FIX::TimeInForce timeInForce(FIX::TimeInForce_DAY);

        message.get(clOrdID);
        message.get(symbol);
        message.get(side);
        message.get(ordType);
        if (ordType == FIX::OrdType_LIMIT) {
            message.get(price);
        }
        message.get(orderQty);
        message.getFieldIfSet(timeInForce);

        if (timeInForce != FIX::TimeInForce_DAY) {
            throw std::logic_error("Unsupported TIF, use Day");
        }

        Side orderSide = (side == FIX::Side_BUY) ? Side::Buy : Side::Sell;
        OrderType orderType = (ordType == FIX::OrdType_LIMIT) ? OrderType::Limit : OrderType::Market;
        double limitPrice = (ordType == FIX::OrdType_LIMIT) ? price.getValue() : 0.0;

        OrderData orderData(orderSide, static_cast<int>(orderQty.getValue()), limitPrice, orderType);
        exchange.addOrder(symbol.getValue(), orderData);

        // Send an execution report
        FIX42::ExecutionReport execReport;
        execReport.set(FIX::OrderID(clOrdID.getValue()));
        execReport.set(FIX::ExecID("123")); // This should be generated uniquely
        execReport.set(FIX::ExecTransType(FIX::ExecTransType_NEW));
        execReport.set(FIX::ExecType(FIX::ExecType_NEW));
        execReport.set(FIX::OrdStatus(FIX::OrdStatus_NEW));
        execReport.set(clOrdID);
        execReport.set(symbol);
        execReport.set(side);
        execReport.set(FIX::LeavesQty(orderQty));
        if (ordType == FIX::OrdType_LIMIT) {
            execReport.set(FIX::Price(price));
        }
        execReport.set(FIX::Text("Order accepted"));

        FIX::Session::sendToTarget(execReport, sessionID);
    } catch (const std::exception& e) {
        std::cerr << "Error processing NewOrderSingle: " << e.what() << std::endl;
    }
}

void FixMessageHandler::onMessage(const FIX42::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID) {
    try {
        FIX::ClOrdID clOrdID;
        FIX::OrigClOrdID origClOrdID;
        FIX::Symbol symbol;
        FIX::Price newPrice;

        // Extract fields from the FIX message
        message.get(clOrdID);
        message.get(origClOrdID);
        message.get(symbol);
        message.get(newPrice);

        int64_t orderId = std::stoll(origClOrdID.getValue());  // Assuming the order ID is numeric and stored as a string
        std::string ticker = symbol.getValue();
        int newLimitPrice = static_cast<int>(newPrice.getValue());

        // Modify the limit price in the exchange
        exchange.modifyLimitPrice(ticker, orderId, newLimitPrice);

        // Send an execution report indicating the modification
        FIX42::ExecutionReport execReport;
        execReport.set(FIX::OrderID(origClOrdID.getValue()));
        execReport.set(FIX::ExecID("124"));  // This should be generated uniquely
        execReport.set(FIX::ExecTransType(FIX::ExecTransType_NEW));
        execReport.set(FIX::ExecType(FIX::ExecType_REPLACED));
        execReport.set(FIX::OrdStatus(FIX::OrdStatus_REPLACED));
        execReport.set(clOrdID);
        execReport.set(symbol);
        execReport.set(FIX::Price(newPrice));
        execReport.set(FIX::Text("Order price modified"));

        FIX::Session::sendToTarget(execReport, sessionID);
    } catch (const std::exception& e) {
        std::cerr << "Error processing OrderCancelReplaceRequest: " << e.what() << std::endl;
    }
}

void FixMessageHandler::onMessage(const FIX42::OrderCancelRequest& message, const FIX::SessionID& sessionID) {
    FIX::ClOrdID clOrdID;  // Declare clOrdID here so it's accessible in both try and catch blocks
    FIX::Symbol symbol;
    FIX::Side side;

    try {
        // Extract relevant fields from the FIX message
        FIX::OrigClOrdID origClOrdID;

        message.get(origClOrdID);
        message.get(clOrdID);  // Now this is in scope
        message.get(symbol);
        message.get(side);

        // Convert fields for internal use
        int64_t orderId = std::stoll(origClOrdID.getValue());  // Assuming the order ID is numeric and stored as a string
        std::string ticker = symbol.getValue();

        // Process the cancellation in the exchange
        exchange.cancelOrder(ticker, orderId);

        // Send an execution report indicating the order was canceled
        FIX42::ExecutionReport cancelReport(
            FIX::OrderID(origClOrdID.getValue()),
            FIX::ExecID("125"),  // This should be generated uniquely
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_CANCELED),
            FIX::OrdStatus(FIX::OrdStatus_CANCELED),
            FIX::Symbol(symbol.getValue()),
            side,
            FIX::LeavesQty(0),
            FIX::CumQty(0),
            FIX::AvgPx(0)
        );

        cancelReport.set(clOrdID);
        cancelReport.set(FIX::Text("Order canceled"));

        FIX::Session::sendToTarget(cancelReport, sessionID);
    } catch (const std::exception& e) {
        std::cerr << "Error processing OrderCancelRequest: " << e.what() << std::endl;

        // Optionally, send a reject message back to the client
        FIX42::ExecutionReport rejectReport(
            FIX::OrderID(clOrdID.getValue()),  // clOrdID is now in scope
            FIX::ExecID("126"),  // Generate a unique ID
            FIX::ExecTransType(FIX::ExecTransType_NEW),
            FIX::ExecType(FIX::ExecType_REJECTED),
            FIX::OrdStatus(FIX::OrdStatus_REJECTED),
            FIX::Symbol(symbol.getValue()),
            side,
            FIX::LeavesQty(0),
            FIX::CumQty(0),
            FIX::AvgPx(0)
        );

        rejectReport.set(clOrdID);
        rejectReport.set(FIX::Text("Order cancellation failed: " + std::string(e.what())));

        try {
            FIX::Session::sendToTarget(rejectReport, sessionID);
        } catch (const FIX::SessionNotFound&) {
            std::cerr << "Error: Session not found for cancellation reject report." << std::endl;
        }
    }
}
