#include <iostream>
#include <string>
#include <curl/curl.h>
#include <simdjson.h>
#include "MyFunctions.h"


std::string read_buffer;

bool parsePositionsAndSummary(const std::string& json_response, std::vector<Position>& positions, OverallSummary& overallSummary) {

    simdjson::dom::parser parser;
    simdjson::dom::element root;

    auto error = parser.parse(json_response).get(root);
    if (error) {
        std::cerr << "Failed to parse JSON: " << error << std::endl;
        return false;
    }

    if (std::string(root["s"].get_string().value()) != "ok") {
        std::cerr << "Error: " << root["message"].get_string().value() << std::endl;
        return false;
    }

    auto netPositions = root["netPositions"];
    for (auto position : netPositions) {
        Position pos;
        pos.symbol = position["symbol"].get_string().value();
        pos.netQty = position["netQty"].get_double().value();
        pos.avgPrice = position["avgPrice"].get_double().value();
        pos.realizedProfit = position["realized_profit"].get_double().value();
        pos.unrealizedProfit = position["unrealized_profit"].get_double().value();
        pos.pl = position["pl"].get_double().value();
        pos.ltp = position["ltp"].get_double().value();
        pos.buyQty = position["buyQty"].get_double().value();
        pos.sellQty = position["sellQty"].get_double().value();
        pos.productType = position["productType"].get_string().value();
        positions.push_back(pos);
    }

    auto overall = root["overall"];
    overallSummary.countTotal = overall["count_total"].get_double().value();
    overallSummary.plTotal = overall["pl_total"].get_double().value();
    overallSummary.plRealized = overall["pl_realized"].get_double().value();
    overallSummary.plUnrealized = overall["pl_unrealized"].get_double().value();

    return true;
}

std::string getPositions(const std::string& auth_header) {
    CURL* curl;
    CURLcode res;
    std::string url = "https://api-t1.fyers.in/api/v3/positions";
    struct curl_slist* headers = NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        std::string authorization = "Authorization: " + auth_header;
        headers = curl_slist_append(headers, authorization.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&read_buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();

    return read_buffer;
}

void displayPositions(const std::vector<Position>& positions) {
    for (const auto& pos : positions) {
        std::cout << "Symbol: " << pos.symbol << std::endl;
        std::cout << "Net Quantity: " << pos.netQty << std::endl;
        std::cout << "Avg Price: " << pos.avgPrice << std::endl;
        std::cout << "Realized Profit: " << pos.realizedProfit << std::endl;
        std::cout << "Unrealized Profit: " << pos.unrealizedProfit << std::endl;
        std::cout << "PL: " << pos.pl << std::endl;
        std::cout << "LTP: " << pos.ltp << std::endl;
        std::cout << "Buy Quantity: " << pos.buyQty << std::endl;
        std::cout << "Sell Quantity: " << pos.sellQty << std::endl;
        std::cout << "Product Type: " << pos.productType << std::endl;
        std::cout << "----------------------" << std::endl;
    }
}

void displayOverallSummary(const OverallSummary& summary) {
    std::cout << "Overall Summary:" << std::endl;
    std::cout << "Total Count: " << summary.countTotal << std::endl;
    std::cout << "Total PL: " << summary.plTotal << std::endl;
    std::cout << "Realized PL: " << summary.plRealized << std::endl;
    std::cout << "Unrealized PL: " << summary.plUnrealized << std::endl;
}