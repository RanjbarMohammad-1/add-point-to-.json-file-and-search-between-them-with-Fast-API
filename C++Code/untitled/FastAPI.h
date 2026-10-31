#ifndef FASTAPI_H
#define FASTAPI_H
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <utility>

class FastAPI
{


public:
    explicit FastAPI(
        std::string baseUrl = "http://127.0.0.1:8000"
    );

    bool addPoint(
        const std::string& name,
        double latitude,
        double longitude
    );

    std::optional<nlohmann::json> searchPoint(
        const std::string& name
    );

private:
    std::string baseUrl;

    using HttpResponse = std::pair<long, std::string>;

    static std::size_t writeCallback(
        char* data,
        std::size_t size,
        std::size_t count,
        void* userData
    );

    HttpResponse sendRequest(
        const std::string& url,
        const std::string& method,
        const std::string& body = ""
    ) const;
};

#endif // FASTAPI_H
