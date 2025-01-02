#include <iostream>
#include <curl/curl.h>
#include <string>
#include <simdjson.h>
#include "myFunctions.h"

void fetchClientDetails(const std::string& auth_header) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api-t1.fyers.in/api/v3/profile";  
        std::string authorization = "Authorization: " + auth_header;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, authorization.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout << "HTTP Response Code: " << response_code << std::endl;

            if (response_code == 200) {
                simdjson::dom::parser parser;
                simdjson::dom::element root = parser.parse(readBuffer);

                try {
                    std::cout << "Client Name: " << root["data"]["name"].get_string() << std::endl;
                    //std::cout << "Display Name: " << root["data"]["display_name"].get_string() << std::endl;
                    std::cout << "Email ID: " << root["data"]["email_id"].get_string() << std::endl;
                    std::cout << "Mobile Number: " << root["data"]["mobile_number"].get_string() << std::endl;
                    std::cout << "PAN: " << root["data"]["PAN"].get_string() << std::endl;
                    std::cout << "TOTP Enabled: " << root["data"]["totp"].get_bool() << std::endl;
                    std::cout << "Password Change Date: " << root["data"]["pwd_change_date"].get_string() << std::endl;
                }
                catch (const simdjson::simdjson_error& e) {
                    std::cerr << "Error parsing JSON: " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "Error: " << readBuffer << std::endl;
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}