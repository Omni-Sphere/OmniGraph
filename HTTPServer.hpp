#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <functional>

#include "GraphQLEngine.hpp"
#include "Database.hpp"
#include "Logger.hpp"
#include "UUIDv4.hpp"
#include "GlobalConfiguration.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace net = boost::asio;
namespace json = boost::json;

using tcp = boost::asio::ip::tcp;
using namespace omnicore::utils;

class HTTPServer : public std::enable_shared_from_this<HTTPServer>
{
public:
    HTTPServer(net::io_context &ioc);

    void start(unsigned short httpPort, unsigned short wsPort);
    
    void setGraphQLEngine(std::shared_ptr<GraphQLEngine> graphql) {
        graphqlService = graphql;
    }

private:
    void start_http_server(unsigned short port);
    void start_websocket_server(unsigned short port);

    void handle_upload(const http::request<http::string_body>& req, std::shared_ptr<tcp::socket> socket);
    void handle_download(const http::request<http::string_body>& req, std::shared_ptr<tcp::socket> socket);

    void loadConfiguration();

    net::io_context &ioc;
    std::shared_ptr<GraphQLEngine> graphqlService;
    std::shared_ptr<omnicore::service::Database> database;
    std::string imgStoragePath = "img_storage";
    std::string pdfStoragePath = "pdf_storage";
    std::string xmlStoragePath = "xml_storage";

public:
    void setDatabase(std::shared_ptr<omnicore::service::Database> db) {
        database = db;
    }
};
