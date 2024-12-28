#include "MyFunctions.h"
#include <curl/curl.h>
#include <iostream>
#include <simdjson.h>


size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string getTradeBook(const std::string& access_token) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        std::string url = "https://api-t1.fyers.in/api/v3/tradebook";
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: " + access_token).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    return readBuffer;
}

bool parseTradeBook(const std::string& json_response, std::vector<Trade>& trades) {
    simdjson::dom::parser parser;
    simdjson::dom::element doc = parser.parse(json_response);

    if (std::string(doc["s"]) != "ok") {
        std::cerr << "Error fetching tradebook data: " << std::string(doc["message"]) << std::endl;
        return false;
    }

    auto tradeBook = doc["tradeBook"].get_array();
    for (auto trade : tradeBook) { 
        Trade t;

        t.orderDateTime = std::string(trade["orderDateTime"]);
        t.orderNumber = std::string(trade["orderNumber"]);
        t.exchangeOrderNo = std::string(trade["exchangeOrderNo"]);
        t.exchange = static_cast<int>(trade["exchange"]);
        t.side = static_cast<int>(trade["side"]);
        t.segment = static_cast<int>(trade["segment"]);
        t.orderType = static_cast<int>(trade["orderType"]);
        t.fyToken = std::string(trade["fyToken"]);
        t.productType = std::string(trade["productType"]);
        t.tradedQty = static_cast<int>(trade["tradedQty"]);
        t.tradePrice = static_cast<double>(trade["tradePrice"]);
        t.tradeValue = static_cast<double>(trade["tradeValue"]);
        t.tradeNumber = std::string(trade["tradeNumber"]);
        t.row = static_cast<int>(trade["row"]);
        t.symbol = std::string(trade["symbol"]);
        t.orderTag = std::string(trade["orderTag"]);

        trades.push_back(t);
    }

    return true;
}


void displayTradeBook(const std::vector<Trade>& trades) {
    for (const auto& trade : trades) {
        std::cout << "Order Number: " << trade.orderNumber
            << ", Symbol: " << trade.symbol
            << ", Traded Qty: " << trade.tradedQty
            << ", Trade Price: " << trade.tradePrice
            << ", Trade Value: " << trade.tradeValue
            << ", Trade Number: " << trade.tradeNumber
            << ", Order Date: " << trade.orderDateTime << std::endl;
    }
}
