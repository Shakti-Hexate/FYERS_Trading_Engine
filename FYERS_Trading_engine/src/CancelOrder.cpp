#include <iostream>
#include <curl/curl.h>
#include <string>
#include "MyFunctions.h"


bool cancelOrder(const std::string& auth_header, const std::string& order_id) {

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return false;
    }

    const std::string url = "https://api-t1.fyers.in/api/v3/orders/sync";

    const std::string json_payload = "{\"id\":\"" + order_id + "\"}";

    std::string response_data;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, auth_header.c_str()));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/json"));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    std::cout << "Response: " << response_data << std::endl;

    curl_easy_cleanup(curl);

    return response_data.find("\"s\":\"ok\"") != std::string::npos;
}
