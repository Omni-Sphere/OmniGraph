#include "HTTPServer.hpp"
#include <filesystem>
#include <fstream>

HTTPServer::HTTPServer(net::io_context &ioc_) : ioc(ioc_) {}

void HTTPServer::start(unsigned short httpPort, unsigned short wsPort) {
  loadConfiguration();
  start_http_server(httpPort);
  start_websocket_server(wsPort);
}

void HTTPServer::start_http_server(unsigned short port) {
  auto acceptor =
      std::make_shared<tcp::acceptor>(ioc, tcp::endpoint{tcp::v4(), port});

  auto do_accept = std::make_shared<std::function<void()>>();

  *do_accept = [acceptor, &ioc = this->ioc, graphql = this->graphqlService,
                do_accept, self = shared_from_this()]() {
    auto socket = std::make_shared<tcp::socket>(ioc);

    acceptor->async_accept(*socket, [socket, &ioc, graphql, do_accept,
                                     self](boost::system::error_code ec) {
      if (!ec) {
        net::post(ioc, [socket, graphql, self]() {
          beast::flat_buffer buffer;
          http::request<http::string_body> req;
          http::read(*socket, buffer, req);

          std::string traceId = UUIDv4::GenerateUUIDv4();
          std::string remote_ip =
              socket->remote_endpoint().address().to_string();

          json::object headersObj;
          for (const auto &field : req) {
            headersObj[std::string(field.name_string())] =
                std::string(field.value());
          }
          std::string headersJson = json::serialize(headersObj);

          std::string bodyStr = req.body();
          if (bodyStr.empty()) {
            bodyStr = "{}";
          } else {
            try {
              json::parse(bodyStr);
            } catch (...) {
              std::string escaped;
              for (char c : bodyStr) {
                if (c == '"')
                  escaped += "\\\"";
                else if (c == '\\')
                  escaped += "\\\\";
                else if (c == '\n')
                  escaped += "\\n";
                else if (c == '\r')
                  escaped += "\\r";
                else if (c == '\t')
                  escaped += "\\t";
                else
                  escaped += c;
              }
              bodyStr = "\"" + escaped + "\"";
            }
          }

          std::string target = std::string(req.target());
          if (target.starts_with("/cdn/upload/")) {
            if (self->cdnServer) {
              self->cdnServer->handle_upload(req, socket);
            } else {
              http::response<http::string_body> res{
                  http::status::internal_server_error, req.version()};
              res.set(http::field::content_type, "text/plain");
              res.body() = "CDN Server not initialized";
              res.prepare_payload();
              http::write(*socket, res);
              socket->shutdown(tcp::socket::shutdown_send);
            }
            return;
          }
          if (target.starts_with("/cdn/download/")) {
            if (self->cdnServer) {
              self->cdnServer->handle_download(req, socket);
            } else {
              http::response<http::string_body> res{
                  http::status::internal_server_error, req.version()};
              res.set(http::field::content_type, "text/plain");
              res.body() = "CDN Server not initialized";
              res.prepare_payload();
              http::write(*socket, res);
              socket->shutdown(tcp::socket::shutdown_send);
            }
            return;
          }

          if (req.method() != http::verb::post) {

            http::response<http::string_body> res{
                http::status::method_not_allowed, req.version()};
            res.set(http::field::content_type, "text/plain");
            res.body() = "Only POST method is allowed";
            res.prepare_payload();
            http::write(*socket, res);
            socket->shutdown(tcp::socket::shutdown_send);
            return;
          }

          if (req[http::field::content_type] != "application/json") {

            http::response<http::string_body> res{
                http::status::unsupported_media_type, req.version()};
            res.set(http::field::content_type, "text/plain");
            res.body() = "Unsupported Content-Type";
            res.prepare_payload();
            http::write(*socket, res);
            socket->shutdown(tcp::socket::shutdown_send);
            return;
          }

          std::string authHeader = req[http::field::authorization];
          std::string jwtToken = "";

          http::response<http::string_body> res =
              graphql->handleGraphQLRequest(req);
          http::write(*socket, res);
          socket->shutdown(tcp::socket::shutdown_send);
        });
      }
      (*do_accept)();
    });
  };

  (*do_accept)();
}

void HTTPServer::start_websocket_server(unsigned short port) {
  std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor =
      std::make_shared<tcp::acceptor>(ioc, tcp::endpoint{tcp::v4(), port});

  auto do_accept = std::make_shared<std::function<void()>>();

  *do_accept = [acceptor, &ioc = this->ioc, do_accept]() {
    auto socket = std::make_shared<tcp::socket>(ioc);
    acceptor->async_accept(
        *socket, [socket, &ioc, do_accept](boost::system::error_code ec) {
          if (!ec) {
            net::post(ioc, [socket]() {
              try {
              } catch (const std::exception &e) {
                // WebSocket session error handling
              }
            });
          }

          (*do_accept)();
        });
  };

  (*do_accept)();
}

void HTTPServer::loadConfiguration() {
  // HTTPServer configuration can be loaded here if needed in the future
}
