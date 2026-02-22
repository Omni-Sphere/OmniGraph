#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>

#endif

#include "Configuration.hpp"
#include "Database.hpp"
#include "GraphQLEngine.hpp"
#include "HTTPServer.hpp"


int main() {
  omnisphere::utils::Base64::SetSecret("_.:Development:._");

  if (!Configuration::Exists("Config.json")) {
    DatabaseConfig dbConfig;
    dbConfig.Server = "127.0.0.1";
    dbConfig.User = "sa";
    dbConfig.Password = "Development..";
    dbConfig.DatabaseName = "OmniDB";
    dbConfig.TrustCertificate = true;
    Configuration::SetDatabaseConfig(dbConfig);

    LicenseConfig licConfig;
    licConfig.Token = "DevelopmentToken";
    licConfig.Secret =
        "_.::JWT_Development_Secret_Must_Be_32_Chars_Long::._"; // Need 32+
                                                                // chars
    Configuration::SetLicenseConfig(licConfig);

    Configuration::Save("Config.json");
    std::cerr << "[INFO] Created default Config.json. Please configure it and "
                 "restart.\n";
    return 0;
  }

  Configuration::Load("Config.json");
  auto dbConfig = Configuration::GetDatabaseConfig();
  auto licConfig = Configuration::GetLicenseConfig();

  omnisphere::utils::JWT::SetJWTSecret(licConfig.Secret);

  net::io_context ioc;

  std::shared_ptr<omnisphere::omnidata::services::Database> database =
      std::make_shared<omnisphere::omnidata::services::Database>();

  database->ServerName(dbConfig.Server);
  database->UserName(dbConfig.User);
  database->Password(dbConfig.Password);
  database->DatabaseName(dbConfig.DatabaseName);
  database->TrustServerCertificate(dbConfig.TrustCertificate);

  if (!database->Connect()) {
    std::cerr << "[ERROR] No se pudo conectar a la base de datos. Se intentará "
                 "reconectar en segundo plano.\n";
    std::thread([database]() {
      while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
        if (database->Connect()) {
          std::cout << "[INFO] Conexión a la base de datos establecida en "
                       "segundo plano.\n";
          break;
        }
        std::cerr
            << "[ERROR] Reintento de conexión a la base de datos fallido.\n";
      }
    }).detach();
  }

  std::shared_ptr<GraphQLEngine> graphqlEngine =
      std::make_shared<GraphQLEngine>(database);

  auto httpEndpoint = tcp::endpoint{tcp::v4(), 8081};
  HTTPServer server(ioc, httpEndpoint);
  server.setGraphQLEngine(graphqlEngine);
  server.setDatabase(database);
  server.run();

  std::vector<std::thread> threads;
  const int thread_count = std::thread::hardware_concurrency();

  for (int i = 0; i < thread_count; ++i)
    threads.emplace_back([&ioc]() { ioc.run(); });

  for (auto &t : threads)
    t.join();

  return 0;
}