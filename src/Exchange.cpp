#include "Exchange.hpp"

/**
 * @brief Constructs a new Exchange with a specified name.
 * @param exchangeName The name of the exchange.
 */
Exchange::Exchange(const std::string& exchangeName) : exchangeName(exchangeName) {}

/**
 * @brief Adds an order to the order book of a specific ticker. Supports both limit and market orders.
 * @param ticker The ticker symbol of the instrument.
 * @param orderData Reference to the order data containing the order details.
 * @throws std::invalid_argument if a limit price is not provided for limit orders and std::runtime_error if the instrument is not available on the exchange.
 */
void Exchange::addOrder(const std::string& ticker, OrderData& orderData) {
    
    Book* instrumentBook = getOrderBook(ticker);
    assert(instrumentBook != nullptr);
    
    if (instrumentBook){
        if (orderData.orderType == OrderType::Limit){
            if (!orderData.limit.has_value()) {
                throw std::invalid_argument("Limit price must be provided for limit orders.");
            }
            
            instrumentBook->addOrderToBook(orderData, globalOrderId);
        } else if (orderData.orderType == OrderType::Market){
            
            instrumentBook->placeMarketOrder(orderData.shares, orderData.orderSide);
        }
    } else {
        throw std::runtime_error("Can't add order to Exchange. The insturment is not covered by the exchange.");
    }
}

/**
 * @brief Modifies the limit price of an order.
 * @param ticker The ticker symbol of the stock.
 * @param orderId The ID of the order to be modified.
 * @param newLimitPrice The new limit price.
 */
void Exchange::modifyLimitPrice(const std::string& ticker, int64_t orderId, int newLimitPrice) {
    
    Book* instrumentBook = getOrderBook(ticker);
    assert(instrumentBook != nullptr);
    instrumentBook->modifyOrderLimitPrice(orderId, newLimitPrice, globalOrderId);
}

/**
 * @brief Modifies the size of an order.
 * @param ticker The ticker symbol of the stock.
 * @param orderId The ID of the order to be modified.
 * @param newSize The new size of the order.
 */
void Exchange::modifyOrderSize(const std::string& ticker, int64_t orderId, int newSize) {
    
    Book* instrumentBook = getOrderBook(ticker);
    assert(instrumentBook != nullptr);
    instrumentBook->modifyOrderSize(orderId, newSize);
}

/**
 * @brief Adds a new ticker to the exchange.
 * @param newTicker The ticker symbol of the new stock.
 */
void Exchange::addInstrument(const std::string& newTicker) {
    
    tickerLob.emplace(newTicker, std::make_unique<Book>());
}

/**
 * @brief Removes an existing ticker from the exchange.
 * @param ticker The ticker symbol of the stock to be removed.
 */
void Exchange::removeInstrument(const std::string& ticker){
    
    tickerLob.erase(ticker);
}


/**
 * @brief Retrieves a list of all tickers currently traded on the exchange.
 * @return A vector of ticker symbols.
 */
std::vector<std::string> Exchange::getTickerList() const {
    
    std::vector<std::string> tickers;
    for (const auto& pair : tickerLob) {
        tickers.push_back(pair.first);
    }
    return tickers;
}

/**
 * @brief Retrieves the order book for a specific ticker.
 * @param ticker The ticker symbol of the instrument.
 * @return Pointer to the order book associated with the ticker, or nullptr if the ticker is not found.
 */
Book* Exchange::getOrderBook(const std::string& ticker) const {
    
    auto it = tickerLob.find(ticker);
    if (it != tickerLob.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Retrieves the National Best Bid and Offer (NBBO) for a specific instrument.
 * @param ticker The ticker symbol of the instrument.
 * @return A pair of optional integers representing the best bid and best offer prices.
 *         If either the bid or offer is unavailable, the corresponding optional will be nullopt.
 * @throws std::invalid_argument if the instrument is not available on the exchange.
 */
std::pair<std::optional<int>, std::optional<int>> Exchange::getNBBO(const std::string& ticker) const {
    
    Book* instrumentBook = getOrderBook(ticker);
    assert(instrumentBook != nullptr);
    
    if (instrumentBook == nullptr){
        
        throw std::invalid_argument("The instrument is not covered by the exchange.");
    }
    std::optional<int> bestBid;
    std::optional<int> bestOffer;
    
    if (const Limit* bestBidLimit = instrumentBook->getBuySide()->getBestLimit()) {
        bestBid = bestBidLimit->getLimitPrice();
    } else {
        bestBid = std::nullopt;
    }

    if (const Limit* bestOfferLimit = instrumentBook->getSellSide()->getBestLimit()) {
        bestOffer = bestOfferLimit->getLimitPrice();
    } else {
        bestOffer = std::nullopt;
    }

    return {bestBid, bestOffer};
}
