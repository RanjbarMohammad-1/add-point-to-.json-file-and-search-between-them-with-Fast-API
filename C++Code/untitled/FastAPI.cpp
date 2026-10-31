#include "FastAPI.h"
#include <curl/curl.h>
#include <stdexcept>
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace std;




FastAPI::FastAPI(std::string baseUrl)
    : baseUrl(std::move(baseUrl))
{
    while (!this->baseUrl.empty() &&
           this->baseUrl.back() == '/') {
        this->baseUrl.pop_back();
    }
}

std::size_t FastAPI::writeCallback(
        char* data,
        std::size_t size,
        std::size_t count,
        void* userData
        )
{
    const std::size_t totalBytes = size * count;

    auto* response =
            static_cast<std::string*>(userData);

    response->append(data, totalBytes);

    return totalBytes;
}

FastAPI::HttpResponse
FastAPI::sendRequest(
        const std::string& url,
        const std::string& method,
        const std::string& body
        ) const
{
    CURL* curl = curl_easy_init();

    if (curl == nullptr) {
        throw std::runtime_error(
                    "Could not initialize libcurl."
                    );
    }

    std::string responseBody;
    struct curl_slist* headers = nullptr;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(
                curl,
                CURLOPT_WRITEFUNCTION,
                &FastAPI::writeCallback
                );
    curl_easy_setopt(
                curl,
                CURLOPT_WRITEDATA,
                &responseBody
                );
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    if (method == "POST") {
        headers = curl_slist_append(
                    headers,
                    "Content-Type: application/json; charset=utf-8"
                    );

        curl_easy_setopt(
                    curl,
                    CURLOPT_HTTPHEADER,
                    headers
                    );
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(
                    curl,
                    CURLOPT_POSTFIELDS,
                    body.c_str()
                    );
        curl_easy_setopt(
                    curl,
                    CURLOPT_POSTFIELDSIZE,
                    static_cast<long>(body.size())
                    );
    }
    else if (method != "GET") {
        curl_easy_cleanup(curl);

        throw std::invalid_argument(
                    "Unsupported HTTP method: " + method
                    );
    }

    const CURLcode result = curl_easy_perform(curl);

    long statusCode = 0;

    if (result == CURLE_OK) {
        curl_easy_getinfo(
                    curl,
                    CURLINFO_RESPONSE_CODE,
                    &statusCode
                    );
    }

    if (headers != nullptr) {
        curl_slist_free_all(headers);
    }

    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw std::runtime_error(
                    std::string("HTTP request failed: ") +
                    curl_easy_strerror(result)
                    );
    }

    return {statusCode, responseBody};
}

bool FastAPI::addPoint(
        const std::string& name,
        double latitude,
        double longitude
        )
{
    const json requestBody = {
        {"name", name},
        {"latitude", latitude},
        {"longitude", longitude}
    };

    const HttpResponse response = sendRequest(
                baseUrl + "/points",
                "POST",
                requestBody.dump()
                );

    if (response.first == 201) {
        std::cout << "Point added successfully.\n";
        return true;
    }

    if (response.first == 409) {
        std::cout << "Point already exists: "
                  << name << '\n';
        return false;
    }

    throw std::runtime_error(
                "Could not add point. HTTP " +
                std::to_string(response.first) +
                ": " + response.second
                );
}

std::optional<json> FastAPI::searchPoint(
    const std::string& name
)
{
    CURL* curl = curl_easy_init();

    if (curl == nullptr) {
        throw std::runtime_error(
            "Could not initialize libcurl."
        );
    }

    char* escapedName = curl_easy_escape(
        curl,
        name.c_str(),
        static_cast<int>(name.size())
    );

    if (escapedName == nullptr) {
        curl_easy_cleanup(curl);

        throw std::runtime_error(
            "Could not URL-encode point name."
        );
    }

    const std::string url =
        baseUrl + "/points/" + escapedName;

    curl_free(escapedName);
    curl_easy_cleanup(curl);

    const HttpResponse response =
        sendRequest(url, "GET");

    if (response.first == 404) {
        return std::nullopt;
    }

    if (response.first != 200) {
        throw std::runtime_error(
            "Could not search for point. HTTP " +
            std::to_string(response.first) +
            ": " + response.second
        );
    }

    const json responseJson =
        json::parse(response.second);

    if (!responseJson.contains("point")) {
        throw std::runtime_error(
            "Unexpected API response."
        );
    }

    return std::optional<json>(
        responseJson.at("point")
    );
}
