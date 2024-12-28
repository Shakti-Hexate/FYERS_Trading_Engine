#include <curl/curl.h>
#include <simdjson.h>
#include <stdexcept>
#include <sstream>
#include "MyFunctions.h"
#include <string>


HoldingsResponse getHoldings(const std::string& appId, const std::string& accessToken) {
    const std::string url = "https://api-t1.fyers.in/api/v3/holdings";
    std::string authHeader = "Authorization: " + appId + ":" + accessToken;

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

    int64_t code;
    std::string_view status;
    if (doc["code"].get_int64().get(code) || doc["s"].get_string().get(status) || code != 200 || status != "ok") {
        throw std::runtime_error("API response indicates failure.");
    }

    HoldingsResponse holdingsResponse;

    simdjson::dom::array holdingsArray;
    if (doc["holdings"].get_array().get(holdingsArray)) {
        throw std::runtime_error("Failed to extract 'holdings' array from response.");
    }

    for (auto holdingElement : holdingsArray) {
        Holding holding;

        auto holdingTypeRes = holdingElement["holdingType"].get_string();
        holding.holdingType = holdingTypeRes.error() ? "HLD" : std::string(holdingTypeRes.value());

        auto quantityRes = holdingElement["quantity"].get_int64();
        holding.quantity = quantityRes.error() ? 0 : static_cast<int>(quantityRes.value());

        auto costPriceRes = holdingElement["costPrice"].get_double();
        holding.costPrice = costPriceRes.error() ? 0.0 : costPriceRes.value();

        auto marketValRes = holdingElement["marketVal"].get_double();
        holding.marketVal = marketValRes.error() ? 0.0 : marketValRes.value();

        auto remainingQuantityRes = holdingElement["remainingQuantity"].get_int64();
        holding.remainingQuantity = remainingQuantityRes.error() ? 0 : static_cast<int>(remainingQuantityRes.value());

        auto plRes = holdingElement["pl"].get_double();
        holding.pl = plRes.error() ? 0.0 : plRes.value();

        auto ltpRes = holdingElement["ltp"].get_double();
        holding.ltp = ltpRes.error() ? 0.0 : ltpRes.value();

        auto idRes = holdingElement["id"].get_int64();
        holding.id = idRes.error() ? 0 : static_cast<int>(idRes.value());

        auto fyTokenRes = holdingElement["fyToken"].get_int64();
        holding.fyToken = fyTokenRes.error() ? 0 : fyTokenRes.value();

        auto exchangeRes = holdingElement["exchange"].get_int64();
        holding.exchange = exchangeRes.error() ? 0 : static_cast<int>(exchangeRes.value());

        auto symbolRes = holdingElement["symbol"].get_string();
        holding.symbol = symbolRes.error() ? "" : std::string(symbolRes.value());

        auto segmentRes = holdingElement["segment"].get_int64();
        holding.segment = segmentRes.error() ? 0 : static_cast<int>(segmentRes.value());

        auto isinRes = holdingElement["isin"].get_string();
        holding.isin = isinRes.error() ? "" : std::string(isinRes.value());

        auto qty_t1Res = holdingElement["qty_t1"].get_int64();
        holding.qty_t1 = qty_t1Res.error() ? 0 : static_cast<int>(qty_t1Res.value());

        auto remainingPledgeQuantityRes = holdingElement["remainingPledgeQuantity"].get_int64();
        holding.remainingPledgeQuantity = remainingPledgeQuantityRes.error() ? 0 : static_cast<int>(remainingPledgeQuantityRes.value());

        auto collateralQuantityRes = holdingElement["collateralQuantity"].get_int64();
        holding.collateralQuantity = collateralQuantityRes.error() ? 0 : static_cast<int>(collateralQuantityRes.value());

        holdingsResponse.holdings.push_back(holding);
    }

    auto overall = doc["overall"];
    if (!overall.is_object()) {
        throw std::runtime_error("Failed to extract 'overall' data from response.");
    }

    auto countTotalRes = overall["count_total"].get_int64();
    holdingsResponse.overall.count_total = countTotalRes.error() ? 0 : static_cast<int>(countTotalRes.value());

    auto totalInvestmentRes = overall["total_investment"].get_double();
    holdingsResponse.overall.total_investment = totalInvestmentRes.error() ? 0.0 : totalInvestmentRes.value();

    auto totalCurrentValueRes = overall["total_current_value"].get_double();
    holdingsResponse.overall.total_current_value = totalCurrentValueRes.error() ? 0.0 : totalCurrentValueRes.value();

    auto totalPlRes = overall["total_pl"].get_double();
    holdingsResponse.overall.total_pl = totalPlRes.error() ? 0.0 : totalPlRes.value();

    auto pnlPercRes = overall["pnl_perc"].get_double();
    holdingsResponse.overall.pnl_perc = pnlPercRes.error() ? 0.0 : pnlPercRes.value();

    return holdingsResponse;
}
