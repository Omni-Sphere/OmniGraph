#include "Configuration.hpp"
#include "Base64.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Initialize static member
boost::json::value Configuration::_config = boost::json::object();

bool Configuration::Exists(const std::string &filePath) {
  return std::filesystem::exists(filePath);
}

void Configuration::Load(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open configuration file: " + filePath);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  // Parse JSON directly
  _config = boost::json::parse(content);
}

void Configuration::Save(const std::string &filePath) {
  // Serialize JSON directly
  std::string content = boost::json::serialize(_config);

  std::ofstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open configuration file for writing: " +
                             filePath);
  }
  file << content;
}

DatabaseConfig Configuration::GetDatabaseConfig() {
  auto db = _config.at("database");
  return {
      omnisphere::utils::Base64::Decode(
          std::string(db.at("server").as_string())),
      omnisphere::utils::Base64::Decode(std::string(db.at("user").as_string())),
      omnisphere::utils::Base64::Decode(
          std::string(db.at("password").as_string())),
      omnisphere::utils::Base64::Decode(std::string(db.at("name").as_string())),
      db.at("trust_certificate").as_bool()};
}

LicenseConfig Configuration::GetLicenseConfig() {
  auto lic = _config.at("license");
  return {omnisphere::utils::Base64::Decode(
              std::string(lic.at("token").as_string())),
          omnisphere::utils::Base64::Decode(
              std::string(lic.at("secret").as_string()))};
}

WhatsAppConfig Configuration::GetWhatsAppConfig() {
  auto wa = _config.at("whatsapp");
  return {omnisphere::utils::Base64::Decode(
              std::string(wa.at("token").as_string())),
          omnisphere::utils::Base64::Decode(
              std::string(wa.at("phone_id").as_string())),
          omnisphere::utils::Base64::Decode(
              std::string(wa.at("app_secret").as_string()))};
}

void Configuration::SetDatabaseConfig(const DatabaseConfig &config) {
  boost::json::object dbObj;
  dbObj["server"] = omnisphere::utils::Base64::Encode(config.Server);
  dbObj["user"] = omnisphere::utils::Base64::Encode(config.User);
  dbObj["password"] = omnisphere::utils::Base64::Encode(config.Password);
  dbObj["name"] = omnisphere::utils::Base64::Encode(config.DatabaseName);
  dbObj["trust_certificate"] = config.TrustCertificate;
  _config.as_object()["database"] = dbObj;
}

void Configuration::SetWhatsAppConfig(const WhatsAppConfig &config) {
  boost::json::object waObj;
  waObj["token"] = omnisphere::utils::Base64::Encode(config.Token);
  waObj["phone_id"] = omnisphere::utils::Base64::Encode(config.PhoneId);
  waObj["app_secret"] = omnisphere::utils::Base64::Encode(config.AppSecret);
  _config.as_object()["whatsapp"] = waObj;
}

void Configuration::SetLicenseConfig(const LicenseConfig &config) {
  boost::json::object licObj;
  licObj["token"] = omnisphere::utils::Base64::Encode(config.Token);
  licObj["secret"] = omnisphere::utils::Base64::Encode(config.Secret);
  _config.as_object()["license"] = licObj;
}
