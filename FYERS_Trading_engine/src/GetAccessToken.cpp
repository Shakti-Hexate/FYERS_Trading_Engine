#include <iostream>
#include <curl/curl.h>
#include <string>
#include <simdjson.h>
#include "myFunctions.h"


std::string getAccessTokenFromRefreshToken(const std::string& appIdHash, const std::string& refreshToken, const std::string& pin) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api-t1.fyers.in/api/v3/validate-refresh-token";

        std::string jsonData = R"({
            "grant_type": "refresh_token",
            "appIdHash": ")" + appIdHash + R"(",
            "refresh_token": ")" + refreshToken + R"(",
            "pin": ")" + pin + R"("
        })";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            return "";
        }

        try {
            simdjson::dom::parser parser;
            simdjson::dom::element root = parser.parse(readBuffer);

            auto status = root["s"];
            if (status.error() == simdjson::error_code::SUCCESS) {
                std::string_view statusStr = status.get_string();  
                if (statusStr == "ok") {
                    auto accessToken = root["access_token"];
                    if (accessToken.error() == simdjson::error_code::SUCCESS) {
                         std::string accessTokenStr = std::string(accessToken.get_string().value()); 
                        curl_easy_cleanup(curl);
                        curl_slist_free_all(headers);
                        return accessTokenStr;
                    }
                }
            }

            std::cerr << "Error in response: " << readBuffer << std::endl;
        }
        catch (const simdjson::simdjson_error& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return "";
}