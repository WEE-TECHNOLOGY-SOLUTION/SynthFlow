#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace web {

// =============================================================================
// Route Structure (like Flask's Rule)
// =============================================================================

struct Route {
    std::string method;                     // GET, POST, PUT, DELETE, *
    std::string path;                       // /api/users/:id
    std::string handlerName;                // Internal handler reference
    std::vector<std::string> paramNames;    // ["id"]
    
    Route() {}
    Route(const std::string& m, const std::string& p, const std::string& h)
        : method(m), path(p), handlerName(h) {}
};

// =============================================================================
// Request/Response Structures
// =============================================================================

struct Request {
    std::string method;
    std::string path;
    std::string query;
    std::string body;
    std::string remoteAddr;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> queryParams;
    
    std::string getHeader(const std::string& key) const {
        auto it = headers.find(key);
        return it != headers.end() ? it->second : "";
    }
    
    std::string getParam(const std::string& key) const {
        auto it = params.find(key);
        return it != params.end() ? it->second : "";
    }
};

struct Response {
    int statusCode = 200;
    std::string contentType = "text/plain";
    std::string body;
    std::map<std::string, std::string> headers;
    
    void status(int code) { statusCode = code; }
    void setHeader(const std::string& key, const std::string& value) { 
        headers[key] = value; 
    }
    
    std::string build() const;
};

// =============================================================================
// Route Registry (Global - like Flask's url_map)
// =============================================================================

class RouteRegistry {
public:
    static RouteRegistry& instance();
    
    void addRoute(const std::string& method, const std::string& path, 
                  const std::string& handler);
    void addMiddleware(const std::string& name);
    
    Route* matchRoute(const std::string& method, const std::string& path,
                      std::map<std::string, std::string>& params);
    
    const std::vector<Route>& getRoutes() const { return routes; }
    const std::vector<std::string>& getMiddleware() const { return middleware; }
    
    void clear() { routes.clear(); middleware.clear(); }
    
private:
    RouteRegistry() {}
    std::vector<Route> routes;
    std::vector<std::string> middleware;
};

// =============================================================================
// HTTP Server
// =============================================================================

// Handler callback type (called from interpreter)
using RequestHandler = std::function<Response(const Request&)>;

class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    
    void setRequestCallback(RequestHandler handler);
    void start(int port);
    void stop();
    bool isRunning() const { return running; }
    
private:
    bool running;
    int serverSocket;
    RequestHandler requestHandler;
    
    void acceptLoop(int port);
    void handleConnection(int clientSocket);
    Request parseRequest(const std::string& rawRequest);
    std::string buildResponse(const Response& res);
};

// =============================================================================
// Helper Functions
// =============================================================================

// Response builders for minimal API
std::string jsonResponse(int status, const std::string& data);
std::string htmlResponse(int status, const std::string& content);
std::string textResponse(int status, const std::string& text);
std::string redirectResponse(const std::string& url);

// URL parsing
void parseQueryString(const std::string& query, 
                      std::map<std::string, std::string>& params);
std::string urlDecode(const std::string& str);

// HTTP status text
std::string getStatusText(int code);

} // namespace web

#endif // HTTP_SERVER_H
