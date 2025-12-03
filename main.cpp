#include "CDNServer.hpp"
#include "Database.hpp"
#include "GraphQLEngine.hpp"
#include "HTTPServer.hpp"

int main() {
  Base64::SetSecret("_.:Development:._");
  JWT::SetJWTSecret("_.::JWT_Development:._");

  net::io_context ioc;

  std::shared_ptr<omnicore::service::Database> database =
      std::make_shared<omnicore::service::Database>();
  database->ServerName("192.168.1.253");
  database->UserName("sa");
  database->Password("Development..");
  database->DatabaseName("OmniDB");
  database->TrustServerCertificate(true);

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

  auto cdnServer = std::make_shared<CDNServer>();
  cdnServer->setDatabase(database);
  cdnServer->loadConfiguration();

  auto server = std::make_shared<HTTPServer>(ioc);
  server->setGraphQLEngine(graphqlEngine);
  server->setDatabase(database);
  server->setCDNServer(cdnServer);
  server->start(8081, 8082);

  std::vector<std::thread> threads;
  const int thread_count = std::thread::hardware_concurrency();

  for (int i = 0; i < thread_count; ++i)
    threads.emplace_back([&ioc]() { ioc.run(); });

  for (auto &t : threads)
    t.join();

  return 0;
}