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

#include <graphqlservice/GraphQLResponse.h>
#include <graphqlservice/GraphQLService.h>

#include <graphqlservice/internal/Grammar.h>
#include <graphqlservice/internal/SyntaxTree.h>

#include "Resolvers/Item/Mutation.hpp"
#include "Resolvers/Item/Query.hpp"
#include "Resolvers/Session/Mutation.hpp"
#include "Resolvers/User/Mutation.hpp"
#include "Resolvers/User/Query.hpp"

#include "ServiceInyector.hpp"

#include "RootMutation.hpp"
#include "RootQuery.hpp"

#include "Generated/MutationObject.h"
#include "Generated/QueryObject.h"

#include "Base64.hpp"
#include "Database.hpp"
#include "Item/Item.hpp"
#include "JWT.hpp"
#include "Session/Session.hpp"
#include "User/User.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace net = boost::asio;
namespace json = boost::json;

using tcp = boost::asio::ip::tcp;

class GraphQLEngine {
public:
  GraphQLEngine(std::shared_ptr<omnisphere::omnidata::services::Database> db);
  http::response<http::string_body>
  handleGraphQLRequest(const http::request<http::string_body> &req);

private:
  std::shared_ptr<omnisphere::omnidata::services::Database> database;

  boost::json::value convert(const graphql::response::Value &v);
  boost::json::object astNodeToJson(const graphql::peg::ast_node *node);
  void printNode(const graphql::peg::ast_node *node, int indent = 0);
};
