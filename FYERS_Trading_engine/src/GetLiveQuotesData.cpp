#include <iostream>
#include <curl/curl.h>
#include <string>
#include "myFunctions.h"


void getLiveQuoteData(const std::string& accessToken , const std::string& symbol) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api-t1.fyers.in/data/quotes?symbols=" + symbol;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            std::cout << "Live Quote Data for NSE:SBIN-EQ:\n" << readBuffer << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }
}
