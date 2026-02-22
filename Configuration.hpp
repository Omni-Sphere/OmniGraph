#pragma once
#include <boost/json.hpp>
#include <string>

struct DatabaseConfig {
  std::string Server;
  std::string User;
  std::string Password;
  std::string DatabaseName;
  bool TrustCertificate;
};

struct WhatsAppConfig {
  std::string Token;
  std::string PhoneId;
  std::string AppSecret;
};

struct LicenseConfig {
  std::string Token;
  std::string Secret;
};

class Configuration {
public:
  static bool Exists(const std::string &filePath = "Config.json");
  static void Load(const std::string &filePath = "Config.json");
  static void Save(const std::string &filePath = "Config.json");

  static DatabaseConfig GetDatabaseConfig();
  static WhatsAppConfig GetWhatsAppConfig();
  static LicenseConfig GetLicenseConfig();

  static void SetDatabaseConfig(const DatabaseConfig &config);
  static void SetWhatsAppConfig(const WhatsAppConfig &config);
  static void SetLicenseConfig(const LicenseConfig &config);

private:
  static boost::json::value _config;
};
