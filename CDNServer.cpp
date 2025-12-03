#include "CDNServer.hpp"
#include <filesystem>
#include <fstream>

CDNServer::CDNServer() {
  if (!std::filesystem::exists(imgStoragePath)) {
    std::filesystem::create_directory(imgStoragePath);
  }
}

void CDNServer::handle_upload(const http::request<http::string_body> &req,
                              std::shared_ptr<tcp::socket> socket) {
  if (req.method() != http::verb::post) {
    http::response<http::string_body> res{http::status::method_not_allowed,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Only POST method is allowed for upload";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  std::string target = std::string(req.target());
  std::string filename = target.substr(std::string("/cdn/upload/").length());

  if (filename.empty()) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Filename is missing";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  if (filename.find("..") != std::string::npos ||
      filename.find("/") != std::string::npos ||
      filename.find("\\") != std::string::npos) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Invalid filename";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  std::filesystem::path storagePath = imgStoragePath;

  if (filename.ends_with(".jpg") || filename.ends_with(".jpeg") ||
      filename.ends_with(".png"))
    storagePath = imgStoragePath;

  if (filename.ends_with(".pdf"))
    storagePath = pdfStoragePath;

  if (filename.ends_with(".xml"))
    storagePath = xmlStoragePath;

  std::filesystem::path filepath = storagePath / filename;
  std::ofstream ofs(filepath, std::ios::binary);
  if (!ofs) {
    http::response<http::string_body> res{http::status::internal_server_error,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Could not save file";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  ofs << req.body();
  ofs.close();

  http::response<http::string_body> res{http::status::ok, req.version()};
  res.set(http::field::content_type, "text/plain");
  res.body() = "File uploaded successfully";
  res.prepare_payload();
  http::write(*socket, res);
  socket->shutdown(tcp::socket::shutdown_send);
}

void CDNServer::handle_download(const http::request<http::string_body> &req,
                                std::shared_ptr<tcp::socket> socket) {
  if (req.method() != http::verb::get) {
    http::response<http::string_body> res{http::status::method_not_allowed,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Only GET method is allowed for download";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  std::string target = std::string(req.target());
  std::string filename = target.substr(std::string("/cdn/download/").length());

  if (filename.empty()) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Filename is missing";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  if (filename.find("..") != std::string::npos ||
      filename.find("/") != std::string::npos ||
      filename.find("\\") != std::string::npos) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Invalid filename";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  std::filesystem::path storagePath;

  if (filename.ends_with(".jpg") || filename.ends_with(".jpeg") ||
      filename.ends_with(".png"))
    storagePath = imgStoragePath;

  if (filename.ends_with(".pdf"))
    storagePath = pdfStoragePath;

  if (filename.ends_with(".xml"))
    storagePath = xmlStoragePath;

  std::filesystem::path filepath = storagePath / filename;
  if (!std::filesystem::exists(filepath)) {
    http::response<http::string_body> res{http::status::not_found,
                                          req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "File not found";
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
    return;
  }

  std::ifstream ifs(filepath, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  ifs.close();

  http::response<http::string_body> res{http::status::ok, req.version()};
  res.set(http::field::content_type,
          "application/octet-stream"); // Or guess based on extension
  res.body() = content;
  res.prepare_payload();
  http::write(*socket, res);
  socket->shutdown(tcp::socket::shutdown_send);
}

void CDNServer::loadConfiguration() {
  if (!database) {
    std::cerr
        << "[WARNING] Database not set for CDNServer. Using default CDN path: "
        << imgStoragePath << "\n";
    return;
  }

  try {
    omnicore::service::GlobalConfiguration configService(database);
    auto config = configService.Get(1);

    if (config.ImagePath.has_value() && !config.ImagePath.value().empty()) {
      std::string path = config.ImagePath.value();
      if (!std::filesystem::exists(path)) {
        std::cerr << "[WARNING] Configured image path does not exist: " << path
                  << ". Attempting to create it.\n";
        try {
          std::filesystem::create_directories(path);
          imgStoragePath = path;
          std::cout << "[INFO] Image storage path loaded and created: "
                    << imgStoragePath << "\n";
        } catch (const std::exception &e) {
          std::cerr << "[ERROR] Failed to create image path: " << e.what()
                    << ". Using default: " << imgStoragePath << "\n";
        }
      } else if (std::filesystem::is_directory(path)) {
        imgStoragePath = path;
        std::cout << "[INFO] Image storage path loaded: " << imgStoragePath
                  << "\n";
      }
    }

    if (config.PDFPath.has_value() && !config.PDFPath.value().empty()) {
      std::string pdfPath = config.PDFPath.value();
      if (!std::filesystem::exists(pdfPath)) {
        std::cerr << "[WARNING] Configured PDF path does not exist: " << pdfPath
                  << ". Attempting to create it.\n";
        try {
          std::filesystem::create_directories(pdfPath);
          pdfStoragePath = pdfPath;
          std::cout << "[INFO] PDF storage path loaded and created: "
                    << pdfStoragePath << "\n";
        } catch (const std::exception &e) {
          std::cerr << "[ERROR] Failed to create PDF path: " << e.what()
                    << ". Using default: " << pdfStoragePath << "\n";
        }
      } else if (std::filesystem::is_directory(pdfPath)) {
        pdfStoragePath = pdfPath;
        std::cout << "[INFO] PDF storage path loaded: " << pdfStoragePath
                  << "\n";
      }
    }

    if (config.XMLPath.has_value() && !config.XMLPath.value().empty()) {
      std::string xmlPath = config.XMLPath.value();
      if (!std::filesystem::exists(xmlPath)) {
        std::cerr << "[WARNING] Configured XML path does not exist: " << xmlPath
                  << ". Attempting to create it.\n";
        try {
          std::filesystem::create_directories(xmlPath);
          xmlStoragePath = xmlPath;
          std::cout << "[INFO] XML storage path loaded and created: "
                    << xmlStoragePath << "\n";
        } catch (const std::exception &e) {
          std::cerr << "[ERROR] Failed to create XML path: " << e.what()
                    << ". Using default: " << xmlStoragePath << "\n";
        }
      } else if (std::filesystem::is_directory(xmlPath)) {
        xmlStoragePath = xmlPath;
        std::cout << "[INFO] XML storage path loaded: " << xmlStoragePath
                  << "\n";
      }
    }
  } catch (const std::exception &e) {
    Logger::Log("Failed to load CDN configuration: " + std::string(e.what()) +
                    ". Using defaults.",
                LogType::ERROR);
  }

  if (!std::filesystem::exists(imgStoragePath))
    std::filesystem::create_directories(imgStoragePath);
  if (!std::filesystem::exists(pdfStoragePath))
    std::filesystem::create_directories(pdfStoragePath);
  if (!std::filesystem::exists(xmlStoragePath))
    std::filesystem::create_directories(xmlStoragePath);
}
