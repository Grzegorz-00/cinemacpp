#include "HttpServer.h"

#include "Logger.h"

#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <filesystem>

namespace fs = std::filesystem;

namespace
{
constexpr const char *LOG_SOURCE_ID = "HTTP_SERVER";
}

std::string HttpServer::readFile(std::string filename) const
{
    filename = _httpFolderPath + "/" + filename;
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "filename: ", filename);
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    return str;
}

std::string HttpServer::getFileType(std::string filename) const
{
    if (filename.find(".html") != std::string::npos)
        return "text/html";
    if (filename.find(".css") != std::string::npos)
        return "text/css";

    return "text";
}

const std::shared_ptr<httpserver::http_response> HttpServer::render_POST(const httpserver::http_request& req)
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call render_POST() req:[", req.get_content(),
            "]");
    try
    {
        std::string command = extractCommandFromJson(req.get_content());
        _requestHandler(command);
    }
    catch (const std::exception &ex)
    {
        Logger::getInstance().print(Logger::Level::ERROR, LOG_SOURCE_ID, "cannot parse JSON");
    }
    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("OK"));
}

const std::shared_ptr<httpserver::http_response> HttpServer::render_GET(const httpserver::http_request& req)
{
    Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "call render_GET() req:[", req.get_content(),
            "]");
    try
    {
        if (!_isHttpWeb)
        {
            Logger::getInstance().print(Logger::Level::ERROR, LOG_SOURCE_ID, "Web server disabled");
            return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("", httpserver::http::http_utils::http_not_found, "text/html"));
        }

        std::string relativePath = req.get_path();
        Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "PATH: ", req.get_path());
        if (relativePath.compare("/") == 0)
            relativePath = "index.html";

        std::string fileContent = readFile(relativePath);
        std::string fileType = getFileType(relativePath);
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(fileContent, httpserver::http::http_utils::http_ok, fileType));
    }
    catch (const std::exception &ex)
    {
        Logger::getInstance().print(Logger::Level::ERROR, LOG_SOURCE_ID, "ERROR: ", ex.what());
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("", httpserver::http::http_utils::http_bad_request, "text/html"));
    }
}

HttpServer::HttpServer(const uint16_t port, std::string httpFolderPath, HttpRequestHandler requestHandler) : _port(port), _httpFolderPath(httpFolderPath), _requestHandler(requestHandler),
        _httpserver(httpserver::create_webserver(port))
{
    Logger::getInstance().print(Logger::Level::INFO, LOG_SOURCE_ID, "Starting http server on port: ", port);

    registerFiles();
}

void HttpServer::start()
{
    _httpserver.start(true);
}

void HttpServer::registerFiles()
{
    if (_httpFolderPath.empty())
    {
        _isHttpWeb = false;
        return;
    }
    _isHttpWeb = true;

    const size_t basePathLength = _httpFolderPath.size();

    _httpserver.register_resource("/", this);

    for (const auto & entry : fs::recursive_directory_iterator(_httpFolderPath))
    {
        const std::string relativePath = entry.path().string().substr(basePathLength);
        Logger::getInstance().print(Logger::Level::DEBUG, LOG_SOURCE_ID, "Registering file:", relativePath);
        _httpserver.register_resource(relativePath, this);
    }
}

std::string HttpServer::extractCommandFromJson(const std::string& json)
{
    try
    {
        std::istringstream iStream(json);
        pt::ptree root;
        pt::read_json(iStream, root);
        return root.get<std::string>("command");
    }
    catch (const pt::ptree_bad_path &ex)
    {
        throw InvalidHttpRequestException("Cannot parse request");
    }
}
