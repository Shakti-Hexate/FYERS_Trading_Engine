#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <simdjson.h>
#include "MyFunctions.h"


OrdersResponse getOrders(const std::string& appId, const std::string& accessToken) {
    const std::string url = "https://api-t1.fyers.in/api/v3/orders";
    std::string authHeader = "Authorization: " + appId + ":" + accessToken;

    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Prepare headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, authHeader.c_str());

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the CURL request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // Parse the response
    simdjson::dom::parser parser;
    simdjson::dom::element doc;
    auto error = parser.parse(response).get(doc);
    if (error) {
        throw std::runtime_error("Failed to parse JSON response: " + std::string(simdjson::error_message(error)));
    }

    // Check response status
    int64_t code;
    std::string_view status;
    if (doc["code"].get_int64().get(code) || doc["s"].get_string().get(status) || code != 200 || status != "ok") {
        throw std::runtime_error("API response indicates failure.");
    }

    OrdersResponse ordersResponse;

    // Extract orders
    simdjson::dom::array orderBookArray;
    if (doc["orderBook"].get_array().get(orderBookArray)) {
        throw std::runtime_error("Failed to extract 'orderBook' array from response.");
    }

    for (auto orderElement : orderBookArray) {
        Order order;

        // Extract fields with explicit error handling
        auto clientIdRes = orderElement["clientId"].get_string();
        order.clientId = clientIdRes.error() ? "" : std::string(clientIdRes.value());

        auto idRes = orderElement["id"].get_string();
        order.id = idRes.error() ? "" : std::string(idRes.value());

        auto exchOrdIdRes = orderElement["exchOrdId"].get_string();
        order.exchOrdId = exchOrdIdRes.error() ? "" : std::string(exchOrdIdRes.value());

        auto qtyRes = orderElement["qty"].get_int64();
        order.qty = qtyRes.error() ? 0 : static_cast<int>(qtyRes.value());

        auto remainingQuantityRes = orderElement["remainingQuantity"].get_int64();
        order.remainingQuantity = remainingQuantityRes.error() ? 0 : static_cast<int>(remainingQuantityRes.value());

        auto filledQtyRes = orderElement["filledQty"].get_int64();
        order.filledQty = filledQtyRes.error() ? 0 : static_cast<int>(filledQtyRes.value());

        auto discloseQtyRes = orderElement["discloseQty"].get_int64();
        order.discloseQty = discloseQtyRes.error() ? 0 : static_cast<int>(discloseQtyRes.value());

        auto limitPriceRes = orderElement["limitPrice"].get_double();
        order.limitPrice = limitPriceRes.error() ? 0.0 : limitPriceRes.value();

        auto stopPriceRes = orderElement["stopPrice"].get_double();
        order.stopPrice = stopPriceRes.error() ? 0.0 : stopPriceRes.value();

        auto tradedPriceRes = orderElement["tradedPrice"].get_double();
        order.tradedPrice = tradedPriceRes.error() ? 0.0 : tradedPriceRes.value();

        auto typeRes = orderElement["type"].get_int64();
        order.type = typeRes.error() ? 0 : static_cast<int>(typeRes.value());

        auto fyTokenRes = orderElement["fyToken"].get_string();
        order.fyToken = fyTokenRes.error() ? "" : std::string(fyTokenRes.value());

        auto exchangeRes = orderElement["exchange"].get_int64();
        order.exchange = exchangeRes.error() ? 0 : static_cast<int>(exchangeRes.value());

        auto segmentRes = orderElement["segment"].get_int64();
        order.segment = segmentRes.error() ? 0 : static_cast<int>(segmentRes.value());

        auto symbolRes = orderElement["symbol"].get_string();
        order.symbol = symbolRes.error() ? "" : std::string(symbolRes.value());

        auto instrumentRes = orderElement["instrument"].get_int64();
        order.instrument = instrumentRes.error() ? 0 : static_cast<int>(instrumentRes.value());

        auto messageRes = orderElement["message"].get_string();
        order.message = messageRes.error() ? "" : std::string(messageRes.value());

        auto offlineOrderRes = orderElement["offlineOrder"].get_bool();
        order.offlineOrder = offlineOrderRes.error() ? false : offlineOrderRes.value();

        auto orderDateTimeRes = orderElement["orderDateTime"].get_string();
        order.orderDateTime = orderDateTimeRes.error() ? "" : std::string(orderDateTimeRes.value());

        auto orderValidityRes = orderElement["orderValidity"].get_string();
        order.orderValidity = orderValidityRes.error() ? "" : std::string(orderValidityRes.value());

        auto panRes = orderElement["pan"].get_string();
        order.pan = panRes.error() ? "" : std::string(panRes.value());

        auto productTypeRes = orderElement["productType"].get_string();
        order.productType = productTypeRes.error() ? "" : std::string(productTypeRes.value());

        auto sideRes = orderElement["side"].get_int64();
        order.side = sideRes.error() ? 0 : static_cast<int>(sideRes.value());

        auto statusRes = orderElement["status"].get_int64();
        order.status = statusRes.error() ? 0 : static_cast<int>(statusRes.value());

        auto sourceRes = orderElement["source"].get_string();
        order.source = sourceRes.error() ? "" : std::string(sourceRes.value());

        auto ex_symRes = orderElement["ex_sym"].get_string();
        order.ex_sym = ex_symRes.error() ? "" : std::string(ex_symRes.value());

        auto descriptionRes = orderElement["description"].get_string();
        order.description = descriptionRes.error() ? "" : std::string(descriptionRes.value());

        auto chRes = orderElement["ch"].get_double();
        order.ch = chRes.error() ? 0.0 : chRes.value();

        auto chpRes = orderElement["chp"].get_double();
        order.chp = chpRes.error() ? 0.0 : chpRes.value();

        auto lpRes = orderElement["lp"].get_double();
        order.lp = lpRes.error() ? 0.0 : lpRes.value();

        auto slNoRes = orderElement["slNo"].get_int64();
        order.slNo = slNoRes.error() ? 0 : static_cast<int>(slNoRes.value());

        auto dqQtyRemRes = orderElement["dqQtyRem"].get_int64();
        order.dqQtyRem = dqQtyRemRes.error() ? 0 : static_cast<int>(dqQtyRemRes.value());

        auto orderNumStatusRes = orderElement["orderNumStatus"].get_string();
        order.orderNumStatus = orderNumStatusRes.error() ? "" : std::string(orderNumStatusRes.value());

        auto disclosedQtyRes = orderElement["disclosedQty"].get_int64();
        order.disclosedQty = disclosedQtyRes.error() ? 0 : static_cast<int>(disclosedQtyRes.value());

        auto orderTagRes = orderElement["orderTag"].get_string();
        order.orderTag = orderTagRes.error() ? "" : std::string(orderTagRes.value());

        ordersResponse.orderBook.push_back(order);
    }

    return ordersResponse;
}