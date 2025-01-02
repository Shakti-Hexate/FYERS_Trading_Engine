#include <iostream>
#include <curl/curl.h>
#include <string>
#include <simdjson.h> 
#include "myFunctions.h"

std::string extractRefreshToken(const std::string& jsonResponse) {
    simdjson::dom::parser parser;
    auto result = parser.parse(jsonResponse);

    if (result.error() != simdjson::error_code::SUCCESS) {
        std::cerr << "Error parsing JSON" << std::endl;
        return "";
    }

    std::string refreshToken;
    auto token = result.value()["refresh_token"];
    if (token.is_string()) {
        refreshToken = token.get_string().value();
    }

    return refreshToken;
}


std::string getRefreshToken(const std::string& appIdHash, const std::string& authCode) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";  

    std::string url = "https://api-t1.fyers.in/api/v3/validate-authcode";
    std::string jsonData = R"({
        "grant_type": "authorization_code",
        "appIdHash": ")" + appIdHash + R"(",
        "code": ")" + authCode + R"(",
        "code_verifier": ""
    })";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return extractRefreshToken(readBuffer);
}