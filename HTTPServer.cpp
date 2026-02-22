#include "HTTPServer.hpp"

// HttpSession implementation

HttpSession::HttpSession(tcp::socket &&socket,
                         std::shared_ptr<GraphQLEngine> engine)
    : stream_(std::move(socket)), graphqlEngine_(std::move(engine)) {}

void HttpSession::run(std::shared_ptr<HttpSession> self) {
  net::dispatch(stream_.get_executor(),
                beast::bind_front_handler(&HttpSession::do_read, self, self));
}

void HttpSession::do_read(std::shared_ptr<HttpSession> self) {
  req_ = {};
  stream_.expires_after(std::chrono::seconds(30));

  http::async_read(
      stream_, buffer_, req_,
      beast::bind_front_handler(&HttpSession::on_read, self, self));
}

void HttpSession::on_read(std::shared_ptr<HttpSession> self,
                          beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec == http::error::end_of_stream)
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

  if (ec) {
    if (ec != http::error::end_of_stream)
      std::cerr << "read: " << ec.message() << "\n";
    return;
  }

  handle_request(self);
}

void HttpSession::handle_request(std::shared_ptr<HttpSession> self) {
  // CORS preflight
  if (req_.method() == http::verb::options) {
    http::response<http::empty_body> res{http::status::ok, req_.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
    res.set(http::field::access_control_allow_headers,
            "Content-Type, Authorization, X-Requested-With");
    res.keep_alive(req_.keep_alive());
    res.prepare_payload();

    auto sp =
        std::make_shared<http::response<http::empty_body>>(std::move(res));
    http::async_write(stream_, *sp,
                      beast::bind_front_handler(&HttpSession::on_write, self,
                                                self, sp->need_eof()));
    return;
  }

  // Only POST allowed for GraphQL
  if (req_.method() != http::verb::post) {
    http::response<http::string_body> res{http::status::not_found,
                                          req_.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req_.keep_alive());
    res.body() =
        "The resource '" + std::string(req_.target()) + "' was not found.";
    res.prepare_payload();

    auto sp =
        std::make_shared<http::response<http::string_body>>(std::move(res));
    http::async_write(stream_, *sp,
                      beast::bind_front_handler(&HttpSession::on_write, self,
                                                self, sp->need_eof()));
    return;
  }

  // Content-Type check
  if (req_[http::field::content_type] != "application/json") {
    http::response<http::string_body> res{http::status::unsupported_media_type,
                                          req_.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Unsupported Content-Type";
    res.keep_alive(req_.keep_alive());
    res.prepare_payload();

    auto sp =
        std::make_shared<http::response<http::string_body>>(std::move(res));
    http::async_write(stream_, *sp,
                      beast::bind_front_handler(&HttpSession::on_write, self,
                                                self, sp->need_eof()));
    return;
  }

  // Handle GraphQL request
  auto res = graphqlEngine_->handleGraphQLRequest(req_);
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::access_control_allow_origin, "*");
  res.keep_alive(req_.keep_alive());
  res.prepare_payload();

  auto sp = std::make_shared<http::response<http::string_body>>(std::move(res));
  http::async_write(stream_, *sp,
                    beast::bind_front_handler(&HttpSession::on_write, self,
                                              self, sp->need_eof()));
}

void HttpSession::on_write(std::shared_ptr<HttpSession> self, bool close,
                           beast::error_code ec,
                           std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    std::cerr << "write: " << ec.message() << "\n";
    return;
  }

  if (close) {
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    return;
  }

  do_read(self);
}

// HTTPServer implementation

HTTPServer::HTTPServer(net::io_context &ioc, tcp::endpoint endpoint)
    : ioc_(ioc), acceptor_(ioc) {

  beast::error_code ec;

  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    std::cerr << "open: " << ec.message() << "\n";
    return;
  }

  acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    std::cerr << "set_option: " << ec.message() << "\n";
    return;
  }

  acceptor_.bind(endpoint, ec);
  if (ec) {
    std::cerr << "bind: " << ec.message() << "\n";
    return;
  }

  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    std::cerr << "listen: " << ec.message() << "\n";
    return;
  }
}

void HTTPServer::run() { do_accept(); }

void HTTPServer::do_accept() {
  acceptor_.async_accept(
      net::make_strand(ioc_),
      beast::bind_front_handler(&HTTPServer::on_accept, this));
}

void HTTPServer::on_accept(beast::error_code ec, tcp::socket socket) {
  if (ec) {
    std::cerr << "accept: " << ec.message() << "\n";
  } else {
    auto session =
        std::make_shared<HttpSession>(std::move(socket), graphqlEngine_);
    session->run(session);
  }

  do_accept();
}
