#pragma once

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "Database.hpp"
#include "GlobalConfiguration.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;

class CDNServer {
public:
  CDNServer();

  void handle_upload(const http::request<http::string_body> &req,
                     std::shared_ptr<tcp::socket> socket);
  void handle_download(const http::request<http::string_body> &req,
                       std::shared_ptr<tcp::socket> socket);

  void setDatabase(std::shared_ptr<omnicore::service::Database> db) {
    database = db;
  }

  void loadConfiguration();

private:
  std::shared_ptr<omnicore::service::Database> database;
  std::string imgStoragePath = "img_storage";
  std::string pdfStoragePath = "pdf_storage";
  std::string xmlStoragePath = "xml_storage";
};
