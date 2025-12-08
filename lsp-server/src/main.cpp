#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "json_utils.h"
#include "../../compiler/include/parser.h"
#include "../../compiler/include/lexer.h"

// Simple Logging
void log(const std::string& msg) {
    std::cerr << "[LSP] " << msg << std::endl;
}

// LSP Response Helper
void send_response(const std::string& content) {
    std::cout << "Content-Length: " << content.length() << "\r\n\r\n" << content << std::flush;
}

int main() {
    log("SynthFlow LSP started.");

    while (true) {
        std::string line;
        int content_length = 0;
        
        // Read headers
        while (std::getline(std::cin, line)) {
            if (line == "\r" || line == "") break; // End of headers
            if (line.find("Content-Length: ") == 0) {
                content_length = std::stoi(line.substr(16));
            }
        }

        if (content_length == 0) continue;

        // Read body
        std::vector<char> buffer(content_length);
        std::cin.read(buffer.data(), content_length);
        std::string body(buffer.begin(), buffer.end());

        log("Received: " + body);

        std::string method = json::get_method(body);
        log("Method: " + method);

        if (method == "initialize") {
            // Respond with capabilities
            std::string response = 
                "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{"
                "\"textDocumentSync\":1," // Full sync
                "\"completionProvider\":{\"resolveProvider\":false,\"triggerCharacters\":[\".\"]}"
                "}}}";
            send_response(response);
        }
        else if (method == "shutdown") {
             std::string response = "{\"jsonrpc\":\"2.0\",\"id\":null,\"result\":null}";
             send_response(response);
             break;
        }
        else if (method == "textDocument/completion") {
            // Static completion list for verification
             std::string response = 
                "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":["
                "{\"label\":\"fn\",\"kind\":14,\"detail\":\"Keyword\"},"
                "{\"label\":\"let\",\"kind\":14,\"detail\":\"Keyword\"},"
                "{\"label\":\"if\",\"kind\":14,\"detail\":\"Keyword\"},"
                "{\"label\":\"else\",\"kind\":14,\"detail\":\"Keyword\"},"
                "{\"label\":\"struct\",\"kind\":14,\"detail\":\"Keyword\"}"
                "]}";
             // Note: ID should match request ID, but for MVP we hardcode or parse it.
             // Parsing ID is needed for correctness. I'll stick to a simple response for now or improve ID parsing.
             // But valid clients expect matching ID.
             
             // Improve ID parsing:
             size_t id_pos = body.find("\"id\"");
             std::string id_val = "null";
             if (id_pos != std::string::npos) {
                 size_t start = body.find_first_of("0123456789", id_pos);
                 if (start != std::string::npos) {
                     size_t end = body.find_first_not_of("0123456789", start);
                     id_val = body.substr(start, end - start);
                 }
             }
             
             response = 
                "{\"jsonrpc\":\"2.0\",\"id\":" + id_val + ",\"result\":["
                "{\"label\":\"fn\",\"kind\":14,\"detail\":\"Keyword\"},"
                "{\"label\":\"let\",\"kind\":14,\"detail\":\"var\"}"
                "]}";
             
             send_response(response);
        }
    }

    return 0;
}
