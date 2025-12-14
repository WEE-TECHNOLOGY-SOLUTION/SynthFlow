#include "../../include/http_server.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <thread>
#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define closesocket close
#define SOCKET int
#define INVALID_SOCKET -1
#endif

namespace web {

// =============================================================================
// RouteRegistry Implementation
// =============================================================================

RouteRegistry& RouteRegistry::instance() {
    static RouteRegistry instance;
    return instance;
}

void RouteRegistry::addRoute(const std::string& method, const std::string& path,
                              const std::string& handler) {
    Route route;
    route.method = method;
    route.path = path;
    route.handlerName = handler;
    
    // Extract param names from path like /users/:id
    std::regex paramRegex(":([a-zA-Z_][a-zA-Z0-9_]*)");
    std::smatch match;
    std::string p = path;
    while (std::regex_search(p, match, paramRegex)) {
        route.paramNames.push_back(match[1].str());
        p = match.suffix();
    }
    
    routes.push_back(route);
    std::cout << "[Web] Route added: " << method << " " << path << std::endl;
}

void RouteRegistry::addMiddleware(const std::string& name) {
    middleware.push_back(name);
    std::cout << "[Web] Middleware added: " << name << std::endl;
}

Route* RouteRegistry::matchRoute(const std::string& method, const std::string& path,
                                  std::map<std::string, std::string>& params) {
    for (auto& route : routes) {
        // Check method
        if (route.method != method && route.method != "*") continue;
        
        // Convert route pattern to regex
        // /users/:id -> ^/users/([^/]+)$
        std::string pattern = "^" + route.path + "$";
        pattern = std::regex_replace(pattern, 
            std::regex(":([a-zA-Z_][a-zA-Z0-9_]*)"), 
            "([^/]+)");
        
        try {
            std::regex routeRegex(pattern);
            std::smatch match;
            
            if (std::regex_match(path, match, routeRegex)) {
                // Extract params
                params.clear();
                for (size_t i = 0; i < route.paramNames.size() && i + 1 < match.size(); i++) {
                    params[route.paramNames[i]] = match[i + 1].str();
                }
                return &route;
            }
        } catch (const std::regex_error&) {
            // Invalid regex, skip
        }
    }
    return nullptr;
}

// =============================================================================
// Response Implementation
// =============================================================================

std::string Response::build() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << getStatusText(statusCode) << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.length() << "\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
    oss << "Access-Control-Allow-Headers: Content-Type\r\n";
    oss << "Connection: close\r\n";
    
    for (const auto& [key, value] : headers) {
        oss << key << ": " << value << "\r\n";
    }
    
    oss << "\r\n";
    oss << body;
    
    return oss.str();
}

// =============================================================================
// HttpServer Implementation
// =============================================================================

HttpServer::HttpServer() : running(false), serverSocket(-1) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

HttpServer::~HttpServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void HttpServer::setRequestCallback(RequestHandler handler) {
    requestHandler = handler;
}

void HttpServer::start(int port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "[Web] Failed to create socket" << std::endl;
        return;
    }
    
    // Allow address reuse
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[Web] Failed to bind to port " << port << std::endl;
        closesocket(serverSocket);
        return;
    }
    
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "[Web] Failed to listen" << std::endl;
        closesocket(serverSocket);
        return;
    }
    
    running = true;
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SynthFlow Web Server v0.0.27" << std::endl;
    std::cout << "  Running on http://localhost:" << port << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    acceptLoop(port);
}

void HttpServer::stop() {
    running = false;
    if (serverSocket != -1) {
        closesocket(serverSocket);
        serverSocket = -1;
    }
}

void HttpServer::acceptLoop(int port) {
    while (running) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == INVALID_SOCKET) {
            if (running) {
                std::cerr << "[Web] Accept failed" << std::endl;
            }
            continue;
        }
        
        handleConnection(clientSocket);
        closesocket(clientSocket);
    }
}

void HttpServer::handleConnection(int clientSocket) {
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));
    
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) return;
    
    std::string rawRequest(buffer, bytesRead);
    Request req = parseRequest(rawRequest);
    
    std::cout << "[" << req.method << "] " << req.path << std::endl;
    
    Response res;
    
    if (requestHandler) {
        res = requestHandler(req);
    } else {
        res.statusCode = 404;
        res.contentType = "text/plain";
        res.body = "Not Found";
    }
    
    std::string responseStr = res.build();
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
}

Request HttpServer::parseRequest(const std::string& rawRequest) {
    Request req;
    std::istringstream stream(rawRequest);
    std::string line;
    
    // Parse request line: GET /path HTTP/1.1
    if (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        lineStream >> req.method >> req.path;
        
        // Remove trailing \r if present
        if (!req.path.empty() && req.path.back() == '\r') {
            req.path.pop_back();
        }
        
        // Split path and query string
        size_t queryPos = req.path.find('?');
        if (queryPos != std::string::npos) {
            req.query = req.path.substr(queryPos + 1);
            req.path = req.path.substr(0, queryPos);
            parseQueryString(req.query, req.queryParams);
        }
    }
    
    // Parse headers
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (line.back() == '\r') line.pop_back();
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // Trim leading space
            if (!value.empty() && value[0] == ' ') value = value.substr(1);
            req.headers[key] = value;
        }
    }
    
    // Parse body (rest of request)
    std::ostringstream bodyStream;
    while (std::getline(stream, line)) {
        bodyStream << line;
    }
    req.body = bodyStream.str();
    
    return req;
}

// =============================================================================
// Helper Functions
// =============================================================================

std::string jsonResponse(int status, const std::string& data) {
    Response res;
    res.statusCode = status;
    res.contentType = "application/json";
    res.body = data;
    return res.build();
}

std::string htmlResponse(int status, const std::string& content) {
    Response res;
    res.statusCode = status;
    res.contentType = "text/html; charset=utf-8";
    res.body = content;
    return res.build();
}

std::string textResponse(int status, const std::string& text) {
    Response res;
    res.statusCode = status;
    res.contentType = "text/plain; charset=utf-8";
    res.body = text;
    return res.build();
}

std::string redirectResponse(const std::string& url) {
    Response res;
    res.statusCode = 302;
    res.headers["Location"] = url;
    res.body = "";
    return res.build();
}

void parseQueryString(const std::string& query, 
                      std::map<std::string, std::string>& params) {
    std::istringstream stream(query);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        size_t eqPos = pair.find('=');
        if (eqPos != std::string::npos) {
            std::string key = urlDecode(pair.substr(0, eqPos));
            std::string value = urlDecode(pair.substr(eqPos + 1));
            params[key] = value;
        }
    }
}

std::string urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hex;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> hex) {
                result += static_cast<char>(hex);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string getStatusText(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

} // namespace web
