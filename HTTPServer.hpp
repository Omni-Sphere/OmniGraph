#pragma once

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/config.hpp>
#include <boost/json.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "Database.hpp"
#include "GlobalConfiguration.hpp"
#include "GraphQLEngine.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace net = boost::asio;
namespace json = boost::json;

using tcp = boost::asio::ip::tcp;

class HttpSession {
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> req_;
  std::shared_ptr<GraphQLEngine> graphqlEngine_;

public:
  HttpSession(tcp::socket &&socket, std::shared_ptr<GraphQLEngine> engine);

  void run(std::shared_ptr<HttpSession> self);
  void do_read(std::shared_ptr<HttpSession> self);
  void on_read(std::shared_ptr<HttpSession> self, beast::error_code ec,
               std::size_t bytes_transferred);
  void on_write(std::shared_ptr<HttpSession> self, bool close,
                beast::error_code ec, std::size_t bytes_transferred);
  void handle_request(std::shared_ptr<HttpSession> self);
};

class HTTPServer {
  net::io_context &ioc_;
  tcp::acceptor acceptor_;
  std::shared_ptr<GraphQLEngine> graphqlEngine_;
  std::shared_ptr<omnisphere::omnidata::services::Database> database_;

public:
  HTTPServer(net::io_context &ioc, tcp::endpoint endpoint);

  void run();

  void setGraphQLEngine(std::shared_ptr<GraphQLEngine> graphql) {
    graphqlEngine_ = graphql;
  }

  void
  setDatabase(std::shared_ptr<omnisphere::omnidata::services::Database> db) {
    database_ = db;
  }

private:
  void do_accept();
  void on_accept(beast::error_code ec, tcp::socket socket);
};
