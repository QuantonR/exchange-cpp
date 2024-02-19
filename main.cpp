#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>


int retrieveBitcoinOrderBook();
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

int main() {
    std::cout << retrieveBitcoinOrderBook() << std::endl;
    return 0;
}

int retrieveBitcoinOrderBook(){

    const std::string apiKey = "0B2BC73A-3FD0-4A1A-8980-495716F16749";
    const std::string url = "https://rest.coinapi.io/v1/orderbooks/current?symbol=KRAKEN_SPOT_BTC_USD&apikey=" + apiKey;

    CURL* curl = curl_easy_init();
    CURLcode res;
    std::string readBuffer;

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else if (http_code == 200) {
            // Parse the JSON string
            auto json = nlohmann::json::parse(readBuffer);

            // Save the data to a file
            std::ofstream file("order-books-kraken-spot-btc-usd.json");
            if (file.is_open()) {
                file << json.dump(4);
                file.close();
                std::cout << "Data saved to order-books-kraken-spot-btc-usd.json" << std::endl;
            } else {
                std::cerr << "Unable to open file" << std::endl;
            }
        } else {
            std::cerr << "Error: Status code " << http_code << std::endl;
        }

        // Always cleanup
        curl_easy_cleanup(curl);
        return 0;
    }
};
// Callback function to write the data received by libcurl into a stringstream
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
