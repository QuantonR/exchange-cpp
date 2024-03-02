#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <utility>
#include <vector>

using json = nlohmann::json;

json retrieveBitcoinOrderBook();
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
std::pair<std::vector<std::pair<double, double>>, std::vector<std::pair<double, double>>> parseOrderBook(const json& orderBook) ;

int main() {
    json bOrderBook = retrieveBitcoinOrderBook();
    std::cout<<bOrderBook<<std::endl;
    auto res = parseOrderBook(bOrderBook);
    return 0;
}

json retrieveBitcoinOrderBook() {
    const std::string apiKey = "0B2BC73A-3FD0-4A1A-8980-495716F16749";
    const std::string symbol = "KRAKEN_SPOT_BTC_USD";
    const std::string url = "https://rest.coinapi.io/v1/orderbooks/current?filter_symbol_id=" + symbol + "&apikey=" + apiKey;

    CURL* curl = curl_easy_init();
    CURLcode res;
    std::string readBuffer;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else if (http_code == 200) {
            // Parse the JSON string
            return nlohmann::json::parse(readBuffer);
        }
        else {
            std::cerr << "Error: Status code " << http_code << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Return an empty JSON object if failed
    return {};
};
// Callback function to write the data received by libcurl into a stringstream
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::pair<std::vector<std::pair<double, double>>, std::vector<std::pair<double, double>>> parseOrderBook(const json& orderBook) {
    std::vector<std::pair<double, double>> bids;
    std::vector<std::pair<double, double>> asks;

    // Check if the order book is valid and contains bids and asks
    // Parse bids
    for (const auto& bid : orderBook["bids"]) {
        double price = bid[0]; // Assuming the first element is price
        double quantity = bid[1]; // Assuming the second element is quantity
        bids.push_back({ price, quantity });
    }

    // Parse asks
    for (const auto& ask : orderBook["asks"]) {
        double price = ask[0]; // Assuming the first element is price
        double quantity = ask[1]; // Assuming the second element is quantity
        asks.push_back({ price, quantity });
    }

    return { bids, asks };
};
