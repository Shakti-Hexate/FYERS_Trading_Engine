#include <curl/curl.h>
#include <simdjson.h>
#include <stdexcept>
#include <sstream>
#include "MyFunctions.h"
#include <string>

std::vector<FundDetail> getFunds(const std::string& auth_header) {
    const std::string url = "https://api-t1.fyers.in/api/v3/funds";
    std::string authHeader = "Authorization: " + auth_header;

    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_slist_free_all(headers); 
        curl_easy_cleanup(curl);     
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }

    curl_slist_free_all(headers); 
    curl_easy_cleanup(curl);

    simdjson::dom::parser parser;
    simdjson::dom::element doc;
    auto error = parser.parse(response).get(doc);
    if (error) {
        throw std::runtime_error("Failed to parse JSON response: " + std::string(simdjson::error_message(error)));
    }

    int64_t code = 0;
    std::string_view status;
    if (doc["code"].get_int64().get(code) || doc["s"].get_string().get(status) || code != 200 || status != "ok") {
        throw std::runtime_error("API response indicates failure.");
    }

    std::vector<FundDetail> fundDetails;
    simdjson::dom::array fundArray;
    if (doc["fund_limit"].get_array().get(fundArray)) {
        throw std::runtime_error("Failed to extract 'fund_limit' array from response.");
    }

    for (auto fundElement : fundArray) {
        FundDetail detail;

        int64_t id = 0;
        std::string_view title;
        double equityAmount = 0.0;
        double commodityAmount = 0.0;

        if (fundElement["id"].get_int64().get(id) ||
            fundElement["title"].get_string().get(title) ||
            fundElement["equityAmount"].get_double().get(equityAmount) ||
            fundElement["commodityAmount"].get_double().get(commodityAmount)) {
            throw std::runtime_error("Failed to extract fund details from an entry.");
        }

        detail.id = id;
        detail.title = std::string(title);
        detail.equityAmount = equityAmount;
        detail.commodityAmount = commodityAmount;

        fundDetails.push_back(detail);
    }

    return fundDetails;
}