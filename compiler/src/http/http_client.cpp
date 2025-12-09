#include "../../include/http_client.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
// For non-Windows, we'd use libcurl
// #include <curl/curl.h>
#endif

namespace http {

// Global API key storage
static std::string g_geminiApiKey = "";

Client::Client() : timeout(30000) {
    defaultHeaders["Content-Type"] = "application/json";
}

Client::~Client() {}

void Client::setHeader(const std::string& key, const std::string& value) {
    defaultHeaders[key] = value;
}

void Client::setTimeout(int timeoutMs) {
    timeout = timeoutMs;
}

#ifdef _WIN32

// Windows implementation using WinHTTP
Response Client::get(const std::string& url) {
    Response response;
    response.statusCode = 0;
    
    // Parse URL
    std::wstring wUrl(url.begin(), url.end());
    
    URL_COMPONENTS urlComp;
    memset(&urlComp, 0, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwSchemeLength = (DWORD)-1;
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;
    
    if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) {
        response.error = "Failed to parse URL";
        return response;
    }
    
    std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
    std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength);
    
    HINTERNET hSession = WinHttpOpen(L"SynthFlow/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        response.error = "Failed to open WinHTTP session";
        return response;
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, hostName.c_str(), urlComp.nPort, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        response.error = "Failed to connect";
        return response;
    }
    
    DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlPath.c_str(),
                                             NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        response.error = "Failed to open request";
        return response;
    }
    
    // Add headers
    for (const auto& header : defaultHeaders) {
        std::wstring headerLine = std::wstring(header.first.begin(), header.first.end()) + L": " +
                                  std::wstring(header.second.begin(), header.second.end());
        WinHttpAddRequestHeaders(hRequest, headerLine.c_str(), (DWORD)headerLine.length(), WINHTTP_ADDREQ_FLAG_ADD);
    }
    
    // Send request
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        response.error = "Failed to send request";
        return response;
    }
    
    // Receive response
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        response.error = "Failed to receive response";
        return response;
    }
    
    // Get status code
    DWORD statusCode = 0;
    DWORD size = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &size, WINHTTP_NO_HEADER_INDEX);
    response.statusCode = statusCode;
    
    // Read response body
    std::string responseBody;
    DWORD bytesAvailable = 0;
    do {
        bytesAvailable = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &bytesAvailable)) break;
        if (bytesAvailable == 0) break;
        
        char* buffer = new char[bytesAvailable + 1];
        DWORD bytesRead = 0;
        if (WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead)) {
            buffer[bytesRead] = '\0';
            responseBody.append(buffer, bytesRead);
        }
        delete[] buffer;
    } while (bytesAvailable > 0);
    
    response.body = responseBody;
    
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return response;
}

Response Client::post(const std::string& url, const std::string& body) {
    return post(url, body, "application/json");
}

Response Client::post(const std::string& url, const std::string& body, const std::string& contentType) {
    Response response;
    response.statusCode = 0;
    
    // Parse URL
    std::wstring wUrl(url.begin(), url.end());
    
    URL_COMPONENTS urlComp;
    memset(&urlComp, 0, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwSchemeLength = (DWORD)-1;
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;
    
    if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) {
        response.error = "Failed to parse URL";
        return response;
    }
    
    std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
    std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength);
    
    HINTERNET hSession = WinHttpOpen(L"SynthFlow/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        response.error = "Failed to open WinHTTP session";
        return response;
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, hostName.c_str(), urlComp.nPort, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        response.error = "Failed to connect";
        return response;
    }
    
    DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath.c_str(),
                                             NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        response.error = "Failed to open request";
        return response;
    }
    
    // Add headers
    for (const auto& header : defaultHeaders) {
        std::wstring headerLine = std::wstring(header.first.begin(), header.first.end()) + L": " +
                                  std::wstring(header.second.begin(), header.second.end());
        WinHttpAddRequestHeaders(hRequest, headerLine.c_str(), (DWORD)headerLine.length(), WINHTTP_ADDREQ_FLAG_ADD);
    }
    
    // Add content type header
    std::wstring ctHeader = L"Content-Type: " + std::wstring(contentType.begin(), contentType.end());
    WinHttpAddRequestHeaders(hRequest, ctHeader.c_str(), (DWORD)ctHeader.length(), WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
    
    // Send request with body
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            (LPVOID)body.c_str(), (DWORD)body.length(), (DWORD)body.length(), 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        response.error = "Failed to send request: " + std::to_string(GetLastError());
        return response;
    }
    
    // Receive response
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        response.error = "Failed to receive response";
        return response;
    }
    
    // Get status code
    DWORD statusCode = 0;
    DWORD size = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &size, WINHTTP_NO_HEADER_INDEX);
    response.statusCode = statusCode;
    
    // Read response body
    std::string responseBody;
    DWORD bytesAvailable = 0;
    do {
        bytesAvailable = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &bytesAvailable)) break;
        if (bytesAvailable == 0) break;
        
        char* buffer = new char[bytesAvailable + 1];
        DWORD bytesRead = 0;
        if (WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead)) {
            buffer[bytesRead] = '\0';
            responseBody.append(buffer, bytesRead);
        }
        delete[] buffer;
    } while (bytesAvailable > 0);
    
    response.body = responseBody;
    
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return response;
}

#else

// Non-Windows placeholder
Response Client::get(const std::string& url) {
    Response response;
    response.statusCode = 501;
    response.error = "HTTP client not implemented for this platform";
    return response;
}

Response Client::post(const std::string& url, const std::string& body) {
    return post(url, body, "application/json");
}

Response Client::post(const std::string& url, const std::string& body, const std::string& contentType) {
    Response response;
    response.statusCode = 501;
    response.error = "HTTP client not implemented for this platform";
    return response;
}

#endif

// ============================================
// Gemini API Functions
// ============================================

namespace gemini {

void setApiKey(const std::string& key) {
    g_geminiApiKey = key;
}

std::string getApiKey() {
    return g_geminiApiKey;
}

bool hasApiKey() {
    return !g_geminiApiKey.empty();
}

// Simple JSON string escaping
static std::string escapeJson(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string buildRequest(const std::string& prompt, const std::string& systemInstruction) {
    std::ostringstream json;
    json << "{";
    
    // Add system instruction if provided
    if (!systemInstruction.empty()) {
        json << "\"system_instruction\": {";
        json << "\"parts\": [{\"text\": \"" << escapeJson(systemInstruction) << "\"}]";
        json << "},";
    }
    
    // Add content
    json << "\"contents\": [{";
    json << "\"parts\": [{\"text\": \"" << escapeJson(prompt) << "\"}]";
    json << "}]";
    
    json << "}";
    return json.str();
}

std::string buildChatRequest(
    const std::vector<std::pair<std::string, std::string>>& messages,
    const std::string& systemInstruction
) {
    std::ostringstream json;
    json << "{";
    
    // Add system instruction if provided
    if (!systemInstruction.empty()) {
        json << "\"system_instruction\": {";
        json << "\"parts\": [{\"text\": \"" << escapeJson(systemInstruction) << "\"}]";
        json << "},";
    }
    
    // Add messages as contents
    json << "\"contents\": [";
    for (size_t i = 0; i < messages.size(); ++i) {
        if (i > 0) json << ",";
        json << "{";
        json << "\"role\": \"" << (messages[i].first == "assistant" ? "model" : messages[i].first) << "\",";
        json << "\"parts\": [{\"text\": \"" << escapeJson(messages[i].second) << "\"}]";
        json << "}";
    }
    json << "]";
    
    json << "}";
    return json.str();
}

std::string parseResponse(const std::string& jsonResponse) {
    // Simple regex-based JSON parsing for the text field
    // Looking for: "text": "..."
    std::regex textRegex("\"text\"\\s*:\\s*\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"");
    std::smatch match;
    
    if (std::regex_search(jsonResponse, match, textRegex) && match.size() > 1) {
        std::string text = match[1].str();
        
        // Unescape JSON string
        std::string result;
        for (size_t i = 0; i < text.length(); ++i) {
            if (text[i] == '\\' && i + 1 < text.length()) {
                switch (text[i + 1]) {
                    case 'n': result += '\n'; ++i; break;
                    case 'r': result += '\r'; ++i; break;
                    case 't': result += '\t'; ++i; break;
                    case '"': result += '"'; ++i; break;
                    case '\\': result += '\\'; ++i; break;
                    default: result += text[i]; break;
                }
            } else {
                result += text[i];
            }
        }
        return result;
    }
    
    // Check for error
    std::regex errorRegex("\"message\"\\s*:\\s*\"([^\"]+)\"");
    if (std::regex_search(jsonResponse, match, errorRegex) && match.size() > 1) {
        return "[Gemini API Error: " + match[1].str() + "]";
    }
    
    return "[Error: Could not parse Gemini response]";
}

std::string generateContent(const std::string& prompt, const std::string& model) {
    return generateContentWithSystem("", prompt, model);
}

std::string generateContentWithSystem(
    const std::string& systemInstruction,
    const std::string& userPrompt,
    const std::string& model
) {
    if (!hasApiKey()) {
        return "[Error: Gemini API key not set. Call gemini_set_api_key(\"your-key\") first.]";
    }
    
    Client client;
    client.setHeader("x-goog-api-key", g_geminiApiKey);
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model + ":generateContent";
    std::string body = buildRequest(userPrompt, systemInstruction);
    
    Response response = client.post(url, body);
    
    if (!response.error.empty()) {
        return "[Error: " + response.error + "]";
    }
    
    if (!response.isSuccess()) {
        return "[Error: HTTP " + std::to_string(response.statusCode) + "] " + parseResponse(response.body);
    }
    
    return parseResponse(response.body);
}

std::string chat(
    const std::vector<std::pair<std::string, std::string>>& messages,
    const std::string& systemInstruction,
    const std::string& model
) {
    if (!hasApiKey()) {
        return "[Error: Gemini API key not set. Call gemini_set_api_key(\"your-key\") first.]";
    }
    
    Client client;
    client.setHeader("x-goog-api-key", g_geminiApiKey);
    
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model + ":generateContent";
    std::string body = buildChatRequest(messages, systemInstruction);
    
    Response response = client.post(url, body);
    
    if (!response.error.empty()) {
        return "[Error: " + response.error + "]";
    }
    
    if (!response.isSuccess()) {
        return "[Error: HTTP " + std::to_string(response.statusCode) + "] " + parseResponse(response.body);
    }
    
    return parseResponse(response.body);
}

} // namespace gemini

} // namespace http
