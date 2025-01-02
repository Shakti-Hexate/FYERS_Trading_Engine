#include <iostream>
#include <curl/curl.h>
#include <simdjson.h>
#include "MyFunctions.h"

bool placeOrder(const std::string& auth_header, const PlaceOrder& order, std::string& orderRefNo) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL." << std::endl;
        return false;
    }

    // URL and headers
    std::string url = "https://api-t1.fyers.in/api/v3/orders/sync";
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: " + auth_header).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // JSON payload
    std::string payload = R"({
        "symbol":")" + order.symbol + R"(",
        "qty":)" + std::to_string(order.qty) + R"(,
        "type":)" + std::to_string(order.type) + R"(,
        "side":)" + std::to_string(order.side) + R"(,
        "productType":")" + order.productType + R"(",
        "limitPrice":)" + std::to_string(order.limitPrice) + R"(,
        "stopPrice":)" + std::to_string(order.stopPrice) + R"(,
        "validity":")" + order.validity + R"(",
        "disclosedQty":)" + std::to_string(order.disclosedQty) + R"(,
        "offlineOrder":)" + (order.offlineOrder ? "true" : "false") + R"(,
        "stopLoss":)" + std::to_string(order.stopLoss) + R"(,
        "takeProfit":)" + std::to_string(order.takeProfit) + R"(,
        "orderTag":")" + order.orderTag + R"("
    })";

    // Log payload
    std::cout << "Payload: " << payload << std::endl;

    // CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Response handling
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* data, size_t size, size_t nmemb, void* userp) -> size_t {
        ((std::string*)userp)->append((char*)data, size * nmemb);
        return size * nmemb;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    // Log raw response
    if (response.empty()) {
        std::cerr << "Empty response from server. Please check the API endpoint and access token." << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }
    std::cout << "Raw Response: " << response << std::endl;

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // Parse the JSON response
    try {
        simdjson::dom::parser parser;
        simdjson::dom::element doc = parser.parse(response);

        // Check for success
        std::string status = std::string(doc["s"]);
        if (status != "ok") {
            std::cerr << "Error placing order: " << std::string(doc["message"]) << std::endl;
            return false;
        }

        // Extract the order reference number
        orderRefNo = std::string(doc["id"]);
        std::cout << "Order placed successfully. Order Ref No: " << orderRefNo << std::endl;
        return true;
    }
    catch (const simdjson::simdjson_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

