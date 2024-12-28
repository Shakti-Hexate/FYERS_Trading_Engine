#pragma once
// my_function.h
#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H

#include <string>
#include <vector>

struct FundDetail {
    int id;
    std::string title;
    double equityAmount;
    double commodityAmount;
};
std::vector<FundDetail> getFunds(const std::string& appId, const std::string& accessToken);

struct Holding {
    std::string holdingType;
    int quantity;
    double costPrice;
    double marketVal;
    int remainingQuantity;
    double pl;
    double ltp;
    int id;
    int64_t fyToken;
    int exchange;
    std::string symbol;
    int segment;
    std::string isin;
    int qty_t1;
    int remainingPledgeQuantity;
    int collateralQuantity;
};

struct Overall {
    int count_total;
    double total_investment;
    double total_current_value;
    double total_pl;
    double pnl_perc;
};

struct HoldingsResponse {
    std::vector<Holding> holdings;
    Overall overall;
};
HoldingsResponse getHoldings(const std::string& appId, const std::string& accessToken);


struct Order {
    std::string clientId;
    std::string id;
    std::string exchOrdId;
    int qty;
    int remainingQuantity;
    int filledQty;
    int discloseQty;
    double limitPrice;
    double stopPrice;
    double tradedPrice;
    int type;
    std::string fyToken;
    int exchange;
    int segment;
    std::string symbol;
    int instrument;
    std::string message;
    bool offlineOrder;
    std::string orderDateTime;
    std::string orderValidity;
    std::string pan;
    std::string productType;
    int side;
    int status;
    std::string source;
    std::string ex_sym;
    std::string description;
    double ch;
    double chp;
    double lp;
    int slNo;
    int dqQtyRem;
    std::string orderNumStatus;
    int disclosedQty;
    std::string orderTag;
};

struct OrdersResponse {
    std::vector<Order> orderBook;
};
OrdersResponse getOrders(const std::string& appId, const std::string& accessToken);

struct OrderDetail {
    std::string id;
    std::string symbol;
    double limitPrice;
    double tradedPrice;
    int qty;
    int remainingQuantity;
    std::string status;
};
OrderDetail getOrderDetails(const std::string& access_token, const std::string& order_id);

struct Position {
    std::string symbol;
    double netQty;
    double avgPrice;
    double realizedProfit;
    double unrealizedProfit;
    double pl;
    double ltp;
    double buyQty;
    double sellQty;
    std::string productType;
};

struct OverallSummary {
    double countTotal;
    double plTotal;
    double plRealized;
    double plUnrealized;
};
std::string getPositions(const std::string& access_token);
bool parsePositionsAndSummary(const std::string& json_response, std::vector<Position>& positions, OverallSummary& overallSummary);
void displayPositions(const std::vector<Position>& positions);
void displayOverallSummary(const OverallSummary& summary);

struct Trade {
    std::string orderDateTime;
    std::string orderNumber;
    std::string exchangeOrderNo;
    int exchange;
    int side;
    int segment;
    int orderType;
    std::string fyToken;
    std::string productType;
    int tradedQty;
    double tradePrice;
    double tradeValue;
    std::string tradeNumber;
    int row;
    std::string symbol;
    std::string orderTag;
};
std::string getTradeBook(const std::string& access_token);
bool parseTradeBook(const std::string& json_response, std::vector<Trade>& trades);
void displayTradeBook(const std::vector<Trade>& trades);


std::string getAccessTokenFromRefreshToken(const std::string& appIdHash, const std::string& refreshToken, const std::string& pin); // Function prototype
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
void fetchClientDetails(const std::string& accessToken , const std::string& client_id);
void getLiveQuoteData(const std::string& accessToken);
std::string getRefreshToken(const std::string& appIdHash, const std::string& authCode);




#endif // MY_FUNCTION_H
