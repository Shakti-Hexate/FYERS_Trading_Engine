#include <iostream>
#include "credentials.h"
#include <curl/curl.h>
#include <string>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <fstream> 
#include <filesystem>  
#include "myFunctions.h"

namespace fs = std::filesystem;

std::string readRefreshTokenFromFile() {
    std::ifstream inputFile("refresh_token.txt");  

    std::string accessToken;
    if (inputFile.is_open()) {
        std::getline(inputFile, accessToken);  

        //std::cout << "Refresh Token from file: " << accessToken << std::endl;
        inputFile.close();  
    }
    else {
        std::cerr << "access.txt not found or failed to open the file." << std::endl;
    }
    return accessToken;
}

void writeRefreshTokenToFile(const std::string& refreshToken) {
    std::ofstream outputFile("refresh_token.txt");  

    if (outputFile.is_open()) {
        outputFile << refreshToken << std::endl;  
        outputFile.close();  
        std::cout << "Refresh token written to refresh_token.txt" << std::endl;
    }
    else {
        std::cerr << "Failed to open the file for writing." << std::endl;
    }
}


std::string generateAppIdHash(const std::string& clientId, const std::string& secretKey) {
    std::string input = clientId + ":" + secretKey;
    std::cout << input << "\n";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}


int main() {

    std::string appIdHash = generateAppIdHash(client_id, secret_key);
    //std::cout << "Generated App ID Hash: " << appIdHash << "\n";

    std::string access_token;
    std::string refresh_token;

    if (fs::exists("refresh_token.txt")) {
        std::cout << "File exists. Reading the refresh token..." << std::endl;
        refresh_token = readRefreshTokenFromFile();
    }
    else {
        std::cout << "File does not exist. Performing other actions..." << std::endl;

        refresh_token = getRefreshToken(appIdHash, auth_code);

        std::cout << "Refresh Token: " << refresh_token << std::endl;

        writeRefreshTokenToFile(refresh_token);
    }

    access_token = getAccessTokenFromRefreshToken(appIdHash, refresh_token, PIN);

    //std::cout << access_token;

    if (access_token.empty()) {
        std::cerr << "Access token is not available. Cannot fetch live data." << std::endl;
        return 1;
    }

    std::string auth_header = client_id + ":" + access_token;

    try {
        // Fetch live quote data
        std::string symbol = "NSE:SBIN-EQ";
        getLiveQuoteData(auth_header, symbol);
    }
    catch (const std::exception& e) {
        std::cerr << "Error fetching live quote data: " << e.what() << std::endl;
    }

    try {
        // Fetch client details
        fetchClientDetails(auth_header);
    }
    catch (const std::exception& e) {
        std::cerr << "Error fetching client details: " << e.what() << std::endl;
    }

    try {
        // Fetch funds data
        std::vector<FundDetail> funds = getFunds(auth_header);
        for (const auto& fund : funds) {
            std::cout << "ID: " << fund.id
                << ", Title: " << fund.title
                << ", Equity: " << fund.equityAmount
                << ", Commodity: " << fund.commodityAmount << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error fetching funds: " << e.what() << std::endl;
    }

    try {
        // Fetch holdings data
        HoldingsResponse Holdings = getHoldings(auth_header);

        // Print Holdings
        for (const auto& holding : Holdings.holdings) {
            std::cout << "Symbol: " << holding.symbol
                << ", Quantity: " << holding.quantity
                << ", Cost Price: " << holding.costPrice
                << ", Market Value: " << holding.marketVal
                << ", P&L: " << holding.pl << "\n";
        }

        // Print Overall
        std::cout << "\nOverall Summary:\n";
        std::cout << "Total Count: " << Holdings.overall.count_total
            << ", Total Investment: " << Holdings.overall.total_investment
            << ", Total Current Value: " << Holdings.overall.total_current_value
            << ", Total P&L: " << Holdings.overall.total_pl
            << ", P&L Percentage: " << Holdings.overall.pnl_perc << "%\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error fetching holdings: " << e.what() << std::endl;
    }
    
    try {
        // Get the orders from the API
        OrdersResponse orders = getOrders(auth_header);

        // Process the returned orders (for example, print details)
        std::cout << "Orders Retrieved: " << orders.orderBook.size() << std::endl;

        for (const Order& order : orders.orderBook) {
            std::cout << "Order ID: " << order.id << std::endl;
            std::cout << "Symbol: " << order.symbol << std::endl;
            std::cout << "Traded Price: " << order.tradedPrice << std::endl;
            std::cout << "Quantity: " << order.qty << std::endl;
            std::cout << "Order DateTime: " << order.orderDateTime << std::endl;
            std::cout << "Order Status: " << order.status << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error occurred: " << e.what() << std::endl;
    }

    try {
        OrderDetail order = getOrderDetails(auth_header, "TEST_ID");

        // Output the order details
        std::cout << "Order ID: " << order.id << std::endl;
        std::cout << "Symbol: " << order.symbol << std::endl;
        std::cout << "Limit Price: " << order.limitPrice << std::endl;
        std::cout << "Traded Price: " << order.tradedPrice << std::endl;
        std::cout << "Quantity: " << order.qty << std::endl;
        std::cout << "Remaining Quantity: " << order.remainingQuantity << std::endl;
        std::cout << "Status: " << order.status << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::string positionsResponse = getPositions(auth_header);

    if (!positionsResponse.empty()) {
        std::vector<Position> positions;
        OverallSummary overallSummary;
        if (parsePositionsAndSummary(positionsResponse, positions, overallSummary)) {
            displayPositions(positions);
            displayOverallSummary(overallSummary);
        }
    }
    else {
        std::cout << "No active positions found ...";
    }

    std::string tradesBookResponse = getTradeBook(auth_header);
    if (!tradesBookResponse.empty()) {
        std::vector<Trade> trades;
        if (parseTradeBook(tradesBookResponse, trades)) {
            displayTradeBook(trades);
        }
        else {
            std::cerr << "Failed to parse tradebook data." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to fetch tradebook data." << std::endl;
    }

    PlaceOrder order{
        "MCX:SILVERMIC20NOVFUT", // symbol
        1,                      // qty
        2,                      // type
        1,                      // side
        "INTRADAY",             // productType
        0,                      // limitPrice
        0,                      // stopPrice
        "DAY",                  // validity
        0,                      // disclosedQty
        false,                  // offlineOrder
        0,                      // stopLoss
        0,                      // takeProfit
        "tag1"                  // orderTag
    };

    std::string orderRefNo;
    if (placeOrder(auth_header, order, orderRefNo)) {
        std::cout << "Order Reference Number: " << orderRefNo << std::endl;
    }
    else {
        std::cerr << "Failed to place order." << std::endl;
    }


    return 0;
}