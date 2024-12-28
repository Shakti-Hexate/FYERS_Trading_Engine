#include <iostream>
#include <string>
#include <curl/curl.h>
#include <simdjson.h>  // Include simdjson header
#include "MyFunctions.h"


// Function to fetch order details
OrderDetail getOrderDetails(const std::string& access_token, const std::string& order_id) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string url = "https://api-t1.fyers.in/api/v3/orders?id=" + order_id;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set up cURL options
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: " + access_token).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::runtime_error("Failed to fetch order details");
        }

        // Parse the response using simdjson
        simdjson::dom::parser parser;
        simdjson::dom::element root;
        auto error = parser.parse(readBuffer).get(root);

        if (error) {
            std::cerr << "Failed to parse JSON response: " << error << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::runtime_error("Failed to parse JSON response");
        }

        // Handle the response and extract order details
        if (root["s"].get_string().value() == "ok" && root["code"].get_int64().value() == 200) {
            // Use `at` instead of `operator[]` to access the array element
            auto orderBook = root["orderBook"].get_array();
            if (orderBook.size() > 0) {
                auto order = orderBook.at(0);  // Accessing the first order in the array

                OrderDetail orderDetail;

                // Extract values using the correct methods for simdjson types
                orderDetail.id = order["id"].get_string().value();
                orderDetail.symbol = order["symbol"].get_string().value();
                orderDetail.limitPrice = order["limitPrice"].get_double().value();
                orderDetail.tradedPrice = order["tradedPrice"].get_double().value();
                orderDetail.qty = static_cast<int>(order["qty"].get_int64().value());  // Explicit cast to int
                orderDetail.remainingQuantity = static_cast<int>(order["remainingQuantity"].get_int64().value());  // Explicit cast to int
                orderDetail.status = order["status"].get_string().value();

                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return orderDetail;
            }
            else {
                std::cerr << "Order not found in orderBook" << std::endl;
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                throw std::runtime_error("Order not found in orderBook");
            }
        }
        else {
            std::cerr << "Error: " << root["message"].get_string().value() << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::runtime_error("Failed to retrieve valid order data");
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    throw std::runtime_error("Failed to initialize cURL");
}