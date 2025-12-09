#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <map>
#include <vector>

// Simple HTTP client for Windows using WinHTTP
// Provides basic HTTP GET/POST functionality for Gemini API integration

namespace http {

// HTTP Response structure
struct Response {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;
    
    bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
};

// HTTP Client class
class Client {
public:
    Client();
    ~Client();
    
    // Set default headers
    void setHeader(const std::string& key, const std::string& value);
    
    // Set timeout in milliseconds
    void setTimeout(int timeoutMs);
    
    // Perform GET request
    Response get(const std::string& url);
    
    // Perform POST request with JSON body
    Response post(const std::string& url, const std::string& body);
    
    // Perform POST request with custom content type
    Response post(const std::string& url, const std::string& body, const std::string& contentType);
    
private:
    std::map<std::string, std::string> defaultHeaders;
    int timeout;
};

// Gemini API helper functions
namespace gemini {
    // Set the global API key
    void setApiKey(const std::string& key);
    
    // Get the current API key
    std::string getApiKey();
    
    // Check if API key is set
    bool hasApiKey();
    
    // Generate content using Gemini API
    // Returns the generated text or error message
    std::string generateContent(
        const std::string& prompt,
        const std::string& model = "gemini-2.0-flash"
    );
    
    // Generate content with system instruction
    std::string generateContentWithSystem(
        const std::string& systemInstruction,
        const std::string& userPrompt,
        const std::string& model = "gemini-2.0-flash"
    );
    
    // Chat with message history
    // messages format: [{"role": "user/model", "content": "text"}, ...]
    std::string chat(
        const std::vector<std::pair<std::string, std::string>>& messages,
        const std::string& systemInstruction = "",
        const std::string& model = "gemini-2.0-flash"
    );
    
    // Parse Gemini API response and extract text
    std::string parseResponse(const std::string& jsonResponse);
    
    // Build JSON request body for generateContent
    std::string buildRequest(
        const std::string& prompt,
        const std::string& systemInstruction = ""
    );
    
    // Build JSON request body for chat
    std::string buildChatRequest(
        const std::vector<std::pair<std::string, std::string>>& messages,
        const std::string& systemInstruction = ""
    );
}

} // namespace http

#endif // HTTP_CLIENT_H
