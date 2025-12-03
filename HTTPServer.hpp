#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/json.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "CDNServer.hpp"
#include "Database.hpp"
#include "GlobalConfiguration.hpp"
#include "GraphQLEngine.hpp"
#include "Logger.hpp"
#include "UUIDv4.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace net = boost::asio;
namespace json = boost::json;

using tcp = boost::asio::ip::tcp;
using namespace omnicore::utils;

class HTTPServer : public std::enable_shared_from_this<HTTPServer> {
public:
  HTTPServer(net::io_context &ioc);

  void start(unsigned short httpPort, unsigned short wsPort);

  void setGraphQLEngine(std::shared_ptr<GraphQLEngine> graphql) {
    graphqlService = graphql;
  }

  void setCDNServer(std::shared_ptr<CDNServer> cdn) { cdnServer = cdn; }

private:
  void start_http_server(unsigned short port);
  void start_websocket_server(unsigned short port);

  void loadConfiguration();

  net::io_context &ioc;
  std::shared_ptr<GraphQLEngine> graphqlService;
  std::shared_ptr<omnicore::service::Database> database;
  std::shared_ptr<CDNServer> cdnServer;

public:
  void setDatabase(std::shared_ptr<omnicore::service::Database> db) {
    database = db;
  }
};
