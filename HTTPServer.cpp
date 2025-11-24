#include "HTTPServer.hpp"
#include <fstream>
#include <filesystem>

HTTPServer::HTTPServer(net::io_context &ioc_)
    : ioc(ioc_)
{
    if (!std::filesystem::exists(imgStoragePath)) {
        std::filesystem::create_directory(imgStoragePath);
    }
}

void HTTPServer::start(unsigned short httpPort, unsigned short wsPort)
{
    loadConfiguration();
    start_http_server(httpPort);
    start_websocket_server(wsPort);
}

void HTTPServer::start_http_server(unsigned short port)
{
    auto acceptor = std::make_shared<tcp::acceptor>(ioc, tcp::endpoint{tcp::v4(), port});

    auto do_accept = std::make_shared<std::function<void()>>();

    *do_accept = [acceptor, &ioc = this->ioc, graphql = this->graphqlService, do_accept, self = shared_from_this()]()
    {
        auto socket = std::make_shared<tcp::socket>(ioc);

        acceptor->async_accept(*socket, [socket, &ioc, graphql, do_accept, self](boost::system::error_code ec)
                               {
            if (!ec)
            {
                net::post(ioc, [socket ,graphql, self]() {
                    
                        beast::flat_buffer buffer;
                        http::request<http::string_body> req;
                        http::read(*socket, buffer, req);

                        std::string traceId = UUIDv4::GenerateUUIDv4();
                        std::string remote_ip = socket->remote_endpoint().address().to_string();
                        
                        json::object headersObj;
                        for (const auto& field : req)
                        {
                            headersObj[std::string(field.name_string())] = std::string(field.value());
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
                                    if (c == '"') escaped += "\\\"";
                                    else if (c == '\\') escaped += "\\\\";
                                    else if (c == '\n') escaped += "\\n";
                                    else if (c == '\r') escaped += "\\r";
                                    else if (c == '\t') escaped += "\\t";
                                    else escaped += c;
                                }
                                bodyStr = "\"" + escaped + "\"";
                            }
                        }
                        
                        Logger::LogRequest(traceId, remote_ip, 
                                         std::string(req.method_string()), 
                                         std::string(req.target()),
                                         headersJson,
                                         bodyStr);
                        Logger::Log("New connection from: " + remote_ip + " [TraceID: " + traceId + "]", LogType::INFO);
                        Logger::Log("Method: " + std::string(req.method_string()), LogType::DEBUG);
                        Logger::Log("Target: " + std::string(req.target()), LogType::DEBUG);

                        std::string target = std::string(req.target());
                        if (target.starts_with("/cdn/upload/")) {
                             self->handle_upload(req, socket);
                             return;
                        }
                        if (target.starts_with("/cdn/download/")) {
                             self->handle_download(req, socket);
                             return;
                        }

                        if(req.method() != http::verb::post)
                        {
                            Logger::Log("Method not allowed: " + std::string(req.method_string()), LogType::WARNING);
                            http::response<http::string_body> res{http::status::method_not_allowed, req.version()};
                            res.set(http::field::content_type, "text/plain");
                            res.body() = "Only POST method is allowed";
                            res.prepare_payload();
                            http::write(*socket, res);
                            socket->shutdown(tcp::socket::shutdown_send);
                            return;
                        }

                        if (req[http::field::content_type] != "application/json")
                        {
                            Logger::Log("Unsupported Content-Type: " + std::string(req[http::field::content_type]), LogType::WARNING);
                            http::response<http::string_body> res{http::status::unsupported_media_type, req.version()};
                            res.set(http::field::content_type, "text/plain");
                            res.body() = "Unsupported Content-Type";
                            res.prepare_payload();
                            http::write(*socket, res);
                            socket->shutdown(tcp::socket::shutdown_send);
                            return;
                        }

                        std::string authHeader = req[http::field::authorization];
                        std::string jwtToken = "";

                        http::response<http::string_body> res = graphql->handleGraphQLRequest(req);
                        http::write(*socket, res);
                        socket->shutdown(tcp::socket::shutdown_send);

                });
            }
            (*do_accept)(); });
    };

    (*do_accept)();
}

void HTTPServer::start_websocket_server(unsigned short port)
{
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor = std::make_shared<tcp::acceptor>(ioc, tcp::endpoint{tcp::v4(), port});

    auto do_accept = std::make_shared<std::function<void()>>();

    *do_accept = [acceptor, &ioc = this->ioc, do_accept]()
    {
        auto socket = std::make_shared<tcp::socket>(ioc);
        acceptor->async_accept(*socket, [socket, &ioc, do_accept](boost::system::error_code ec)
                               {
            if (!ec)
            {
                net::post(ioc, [socket]() {
                    try
                    {
                        Logger::Log("Web socket server iniciado", LogType::INFO);
                    }
                    catch (const std::exception& e)
                    {
                        Logger::Log("[WebSocket Session Error] " + std::string(e.what()), LogType::ERROR);
                    }
                });
            }

            (*do_accept)(); });
    };

    (*do_accept)();
}

void HTTPServer::handle_upload(const http::request<http::string_body>& req, std::shared_ptr<tcp::socket> socket)
{
    if (req.method() != http::verb::post)
    {
        http::response<http::string_body> res{http::status::method_not_allowed, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Only POST method is allowed for upload";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    std::string target = std::string(req.target());
    std::string filename = target.substr(std::string("/cdn/upload/").length());
    
    if (filename.empty())
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Filename is missing";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    if (filename.find("..") != std::string::npos || filename.find("/") != std::string::npos || filename.find("\\") != std::string::npos)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Invalid filename";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    std::filesystem::path storagePath = imgStoragePath;

    if (filename.ends_witg(".jpg") || filename.ends_witg(".jpeg") || filename.ends_witg(".png"))
        storagePath = imgStoragePath;

    if (filename.ends_with(".pdf")) 
        storagePath = pdfStoragePath;

    if (filename.ends_with(".xml"))
        storagePath = xmlStoragePath;

    std::filesystem::path filepath = storagePath / filename;
    std::ofstream ofs(filepath, std::ios::binary);
    if (!ofs)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
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

void HTTPServer::handle_download(const http::request<http::string_body>& req, std::shared_ptr<tcp::socket> socket)
{
    if (req.method() != http::verb::get)
    {
        http::response<http::string_body> res{http::status::method_not_allowed, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Only GET method is allowed for download";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    std::string target = std::string(req.target());
    std::string filename = target.substr(std::string("/cdn/download/").length());

    if (filename.empty())
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Filename is missing";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    if (filename.find("..") != std::string::npos || filename.find("/") != std::string::npos || filename.find("\\") != std::string::npos)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "Invalid filename";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    std::filesystem::path storagePath;
    
    if (filename.ends_witg(".jpg") || filename.ends_witg(".jpeg") || filename.ends_witg(".png"))
        storagePath = imgStoragePath;

    if (filename.ends_with(".pdf")) 
        storagePath = pdfStoragePath;

    if (filename.ends_with(".xml"))
        storagePath = xmlStoragePath;

    std::filesystem::path filepath = storagePath / filename;
    if (!std::filesystem::exists(filepath))
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::content_type, "text/plain");
        res.body() = "File not found";
        res.prepare_payload();
        http::write(*socket, res);
        socket->shutdown(tcp::socket::shutdown_send);
        return;
    }

    std::ifstream ifs(filepath, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::content_type, "application/octet-stream"); // Or guess based on extension
    res.body() = content;
    res.prepare_payload();
    http::write(*socket, res);
    socket->shutdown(tcp::socket::shutdown_send);
}

void HTTPServer::loadConfiguration()
{
    if (!database)
    {
        std::cerr << "[WARNING] Database not set for HTTPServer. Using default CDN path: " << imgStoragePath << "\n";
        return;
    }

    try
    {
        omnicore::service::GlobalConfiguration configService(database);
        auto config = configService.Get(1);
        
        if (config.ImagePath.has_value() && !config.ImagePath.value().empty())
        {
            std::string path = config.ImagePath.value();
            if (!std::filesystem::exists(path))
            {
                std::cerr << "[WARNING] Configured image path does not exist: " << path << ". Attempting to create it.\n";
                try {
                    std::filesystem::create_directories(path);
                    imgStoragePath = path;
                    std::cout << "[INFO] Image storage path loaded and created: " << imgStoragePath << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Failed to create image path: " << e.what() << ". Using default: " << imgStoragePath << "\n";
                }
            }
            else if (std::filesystem::is_directory(path))
            {
                imgStoragePath = path;
                std::cout << "[INFO] Image storage path loaded: " << imgStoragePath << "\n";
            }
        }

        if (config.PDFPath.has_value() && !config.PDFPath.value().empty())
        {
            std::string pdfPath = config.PDFPath.value();
            if (!std::filesystem::exists(pdfPath))
            {
                std::cerr << "[WARNING] Configured PDF path does not exist: " << pdfPath << ". Attempting to create it.\n";
                try {
                    std::filesystem::create_directories(pdfPath);
                    pdfStoragePath = pdfPath;
                    std::cout << "[INFO] PDF storage path loaded and created: " << pdfStoragePath << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Failed to create PDF path: " << e.what() << ". Using default: " << pdfStoragePath << "\n";
                }
            }
            else if (std::filesystem::is_directory(pdfPath))
            {
                pdfStoragePath = pdfPath;
                std::cout << "[INFO] PDF storage path loaded: " << pdfStoragePath << "\n";
            }
        }

        if (config.XMLPath.has_value() && !config.XMLPath.value().empty())
        {
            std::string xmlPath = config.XMLPath.value();
            if (!std::filesystem::exists(xmlPath))
            {
                std::cerr << "[WARNING] Configured XML path does not exist: " << xmlPath << ". Attempting to create it.\n";
                try {
                    std::filesystem::create_directories(xmlPath);
                    xmlStoragePath = xmlPath;
                    std::cout << "[INFO] XML storage path loaded and created: " << xmlStoragePath << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Failed to create XML path: " << e.what() << ". Using default: " << xmlStoragePath << "\n";
                }
            }
            else if (std::filesystem::is_directory(xmlPath))
            {
                xmlStoragePath = xmlPath;
                std::cout << "[INFO] XML storage path loaded: " << xmlStoragePath << "\n";
            }
        }
    }
    catch (const std::exception& e)
    {
        Logger::Log("Failed to load configuration: " + std::string(e.what()) + ". Using defaults.", LogType::ERROR);
    }
    
    if (!std::filesystem::exists(imgStoragePath)) std::filesystem::create_directories(imgStoragePath);
    if (!std::filesystem::exists(pdfStoragePath)) std::filesystem::create_directories(pdfStoragePath);
    if (!std::filesystem::exists(xmlStoragePath)) std::filesystem::create_directories(xmlStoragePath);
}
