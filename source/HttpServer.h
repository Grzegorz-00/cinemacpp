#pragma once

#include <cstdint>
#include <functional>
#include <httpserver.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

using HttpRequestHandler = std::function<void(const std::string&)>;

class InvalidHttpRequestException: public std::exception
{
public:
    InvalidHttpRequestException(const std::string& what) :
            _what(what)
    {
    }
    InvalidHttpRequestException() = default;
    const char* what()
    {
        return _what.c_str();
    }
private:
    std::string _what;
};

class HttpServer: public httpserver::http_resource
{
public:
    HttpServer(const uint16_t port, const std::string httpFolerPath, HttpRequestHandler requestHandler);
    const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req);
    const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request& req);
    void start();

private:
    static std::string extractCommandFromJson(const std::string& json);
    void registerFiles();
    std::string readFile(std::string filename) const;
    std::string getFileType(std::string filename) const;

    uint16_t _port { 0 };
    std::string _httpFolderPath;
    HttpRequestHandler _requestHandler;
    httpserver::webserver _httpserver;
    bool _isHttpWeb{false};
};
