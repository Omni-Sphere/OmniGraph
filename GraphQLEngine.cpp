#include "GraphQLEngine.hpp"

using namespace graphql;

GraphQLEngine::GraphQLEngine(
    std::shared_ptr<::omnisphere::omnidata::services::Database> db)
    : database(std::move(db)) {
  std::system("clear");

  const char *R = "\033[31m";
  const char *O = "\033[38;5;208m"; // naranja real
  const char *Y = "\033[93m";
  const char *G = "\033[32m";
  const char *C = "\033[36m";
  const char *B = "\033[34m";
  const char *M = "\033[35m";
  const char *Z = "\033[0m";

  std::cout << R
            << "  ░██████                              ░██               "
               "░██████                                 ░██        \n"
            << O
            << " ░██   ░██                                              ░██   "
               "░██                                ░██        \n"
            << Y
            << "░██     ░██ ░█████████████  ░████████  ░██             ░██     "
               "   ░██░████  ░██████   ░████████  ░████████  \n"
            << G
            << "░██     ░██ ░██   ░██   ░██ ░██    ░██ ░██   ░██████   ░██  "
               "█████ ░███           ░██  ░██    ░██ ░██    ░██ \n"
            << C
            << "░██     ░██ ░██   ░██   ░██ ░██    ░██ ░██             ░██     "
               "██ ░██       ░███████  ░██    ░██ ░██    ░██ \n"
            << B
            << " ░██   ░██  ░██   ░██   ░██ ░██    ░██ ░██              ░██  "
               "░███ ░██      ░██   ░██  ░███   ░██ ░██    ░██ \n"
            << M
            << "  ░██████   ░██   ░██   ░██ ░██    ░██ ░██               "
               "░█████░█ ░██       ░█████░██ ░██░█████  ░██    ░██ \n"
            << M
            << "                                                               "
               "                       ░██                   \n"
            << R
            << "                                                               "
               "                       ░██                   \n"
            << Z << std::flush;
}

boost::json::value GraphQLEngine::convert(const graphql::response::Value &v) {
  switch (v.type()) {
  case response::Type::Null:
    return nullptr;

  case response::Type::Boolean:
    return v.get<graphql::response::BooleanType>();

  case response::Type::Int:
    return v.get<graphql::response::IntType>();

  case response::Type::Float:
    return v.get<graphql::response::FloatType>();

  case response::Type::String:
  case response::Type::EnumValue:
  case response::Type::ID:
    return boost::json::string(v.get<graphql::response::StringType>());

  case response::Type::List: {
    boost::json::array arr;

    for (auto &elem : v.get<graphql::response::ListType>()) {
      arr.push_back(convert(elem));
    }
    return arr;
  }

  case response::Type::Map: {
    boost::json::object obj;
    for (auto &pair : v.get<graphql::response::MapType>()) {
      obj[pair.first] = convert(pair.second);
    }
    return obj;
  }

  default:
    return nullptr;
  }
}

boost::json::object
GraphQLEngine::astNodeToJson(const graphql::peg::ast_node *node) {
  boost::json::object jsonNode;

  if (!node)
    return jsonNode;

  jsonNode["rule"] = node->type;

  if (node->has_content()) {
    std::string content = std::string(node->string_view());
    if (!content.empty()) {
      jsonNode["content"] = content;
    }
  }

  if (!node->children.empty()) {
    boost::json::array children;
    for (const auto &child : node->children) {
      children.emplace_back(astNodeToJson(child.get()));
    }
    jsonNode["children"] = std::move(children);
  }

  return jsonNode;
}

void GraphQLEngine::printNode(const graphql::peg::ast_node *node, int indent) {
  if (!node)
    return;

  std::string indentStr(indent * 2, ' ');
  std::cout << indentStr << "Rule: " << node->type << "\n";

  if (node->has_content()) {
    const auto content = node->string_view();
    if (!content.empty()) {
      std::cout << indentStr << "Content: " << content << "\n";
    }
  }

  for (const auto &child : node->children) {
    printNode(child.get(), indent + 1);
  }
}

http::response<http::string_body> GraphQLEngine::handleGraphQLRequest(
    const http::request<http::string_body> &req) {
  try {
    std::string authHeader;
    std::string token;
    std::string operationName;
    boost::json::value jv = json::parse(req.body());
    boost::json::object obj = jv.as_object();
    response::Value variables{graphql::response::Type::Map};

    if (obj.contains("operationName") && !obj["operationName"].is_null()) {
      operationName = obj["operationName"].as_string().c_str();
      std::cout << operationName << std::endl;
    }

    // if (operationName != "LoginUser")
    // {
    //     if (req.find(http::field::authorization) != req.end())
    //         authHeader = req.at(http::field::authorization);

    //     if (!authHeader.empty())
    //     {
    //         const std::string bearer = "Bearer ";

    //         if (authHeader.rfind(bearer, 0) == 0)
    //         {
    //             token = authHeader.substr(bearer.size());
    //         }
    //         else
    //         {
    //             token = authHeader;
    //         }
    //     }

    //     if (token.empty())
    //         throw std::runtime_error("Missing Authorization token");

    //     JWT::ValidateToken(token);
    // }

    if (obj.contains("variables") && !obj["variables"].is_null()) {
      std::function<response::Value(const json::value &)>
          convertJsonToResponseValue;
      convertJsonToResponseValue =
          [&](const json::value &v) -> response::Value {
        switch (v.kind()) {
        case json::kind::object: {
          response::Value map{graphql::response::Type::Map};

          for (auto &[key, val] : v.as_object()) {
            map.emplace_back(std::string(key), convertJsonToResponseValue(val));
          }
          return map;
        }
        case json::kind::array: {
          response::Value arr{graphql::response::Type::List};
          for (auto &elem : v.as_array()) {
            arr.emplace_back(convertJsonToResponseValue(elem));
          }
          return arr;
        }
        case json::kind::string:
          return response::Value{std::string(v.as_string().c_str())};
        case json::kind::int64:
          return response::Value{static_cast<response::IntType>(v.as_int64())};
        case json::kind::uint64:
          return response::Value{static_cast<response::IntType>(v.as_uint64())};
        case json::kind::double_:
          return response::Value{v.as_double()};
        case json::kind::bool_:
          return response::Value{v.as_bool()};
        case json::kind::null:
        default:
          return response::Value{};
        }
      };

      variables = convertJsonToResponseValue(obj["variables"]);
    }

    std::shared_ptr<graphql::service::Request> request;
    std::shared_ptr<ServiceInyector> serviceInyector =
        std::make_shared<ServiceInyector>(database);

    std::shared_ptr<graphql::resolvers::RootQueryResolver> rootQ =
        std::make_shared<graphql::resolvers::RootQueryResolver>(
            serviceInyector);
    std::shared_ptr<graphql::resolvers::RootMutationResolver> rootM =
        std::make_shared<graphql::resolvers::RootMutationResolver>(
            serviceInyector);

    std::shared_ptr<graphql::omnisphere::object::Query> qObj =
        std::make_shared<graphql::omnisphere::object::Query>(rootQ);
    std::shared_ptr<graphql::omnisphere::object::Mutation> mObj =
        std::make_shared<graphql::omnisphere::object::Mutation>(rootM);

    request = std::make_shared<graphql::omnisphere::Operations>(qObj, mObj);

    std::string queryStr = obj["query"].as_string().c_str();

    std::cerr << "Ejecutando Query:\n" << queryStr << "\n";

    graphql::peg::ast ast = graphql::peg::parseString(queryStr);

    if (!ast.root) {
      throw std::runtime_error("Error al parsear la consulta GraphQL.");
    }

    std::cout << "AST parseado correctamente\n";

    graphql::service::RequestResolveParams params{
        ast, operationName, std::move(variables),
        graphql::service::await_async{}};

    graphql::response::Value result = request->resolve(params).get();

    boost::json::value jsonValue = convert(result);
    std::string responseBody = boost::json::serialize(jsonValue);

    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::content_type, "application/json");
    res.body() = responseBody;
    res.prepare_payload();

    return res;
  } catch (graphql::service::schema_exception &e) {
    http::response<http::string_body> res{http::status::forbidden, 11};
    res.set(http::field::content_type, "application/json");
    std::string errorJson = boost::json::serialize(convert(e.getErrors()));
    res.body() = errorJson;
    res.prepare_payload();
    return res;
  } catch (const std::exception &ex) {
    boost::json::object out;
    boost::json::array errors;

    boost::json::object e0;
    e0["message"] = ex.what();
    e0["extensions"] = {{"code", "UNAUTHENTICATED"}};
    errors.push_back(e0);

    out["errors"] = errors;
    out["data"] = nullptr;

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::content_type, "application/json");
    res.body() = boost::json::serialize(out);
    res.prepare_payload();
    return res;
  }
}
