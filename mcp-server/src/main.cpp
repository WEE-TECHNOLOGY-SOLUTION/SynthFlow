#include <iostream>
#include <string>
#include <vector>
#include "json_mcp.h"

// Placeholder for SynthFlow internals
// In a full implementation, you'd include "compiler/include/interpreter.h" etc.

void log(const std::string& msg) {
    std::cerr << "[MCP] " << msg << std::endl;
}

void send_response(const std::string& content) {
    std::cout << "Content-Length: " << content.length() << "\r\n\r\n" << content << std::flush;
}

// Tool Implementation: Kenneth
// "Kenneth" represents a program execution capability
std::string run_kenneth(const std::string& input_data) {
    // START_DEMO_LOGIC
    // This connects to your actual SynthFlow interpreter/compiler
    // For now, we simulate executing a SynthFlow program
    std::string result = "Kenneth Executed: Processed input [" + input_data + "] using SynthFlow runtime.";
    // END_DEMO_LOGIC
    return result;
}

int main() {
    log("SynthFlow MCP Server started.");

    while (true) {
        std::string line;
        int content_length = 0;
        
        // 1. Read Headers
        while (std::getline(std::cin, line)) {
            if (line == "\r" || line == "") break;
            if (line.find("Content-Length: ") == 0) {
                content_length = std::stoi(line.substr(16));
            }
        }

        if (content_length == 0) continue;

        // 2. Read Body
        std::vector<char> buffer(content_length);
        std::cin.read(buffer.data(), content_length);
        std::string body(buffer.begin(), buffer.end());
        
        log("Received request.");

        // 3. Handle Request
        std::string method = json::get_string(body, "method");
        std::string id = json::get_id(body);

        log("Method: " + method);

        if (method == "initialize") {
             std::string response = 
                "{\"jsonrpc\":\"2.0\",\"id\":" + id + ","
                "\"result\":{"
                    "\"protocolVersion\":\"2024-11-05\","
                    "\"capabilities\":{},"
                    "\"serverInfo\":{\"name\":\"synthflow-mcp\",\"version\":\"0.1.0\"}"
                "}}";
             send_response(response);
        }
        else if (method == "notifications/initialized") {
            // No response needed
            log("Client initialized.");
        }
        else if (method == "tools/list") {
             std::string response = 
                "{\"jsonrpc\":\"2.0\",\"id\":" + id + ","
                "\"result\":{"
                    "\"tools\":[{"
                        "\"name\":\"run_kenneth\","
                        "\"description\":\"Executes a Kenneth program within the SynthFlow environment.\","
                        "\"inputSchema\":{"
                            "\"type\":\"object\","
                            "\"properties\":{"
                                "\"input_data\":{\"type\":\"string\"}"
                            "},"
                            "\"required\":[\"input_data\"]"
                        "}"
                    "}]"
                "}}";
             send_response(response);
        }
        else if (method == "tools/call") {
             std::string name = json::get_string(body, "name");
             if (name == "run_kenneth") {
                // Parse arguments - currently simplistic
                // In a real JSON parser we'd extract params object
                std::string input = json::get_string(body, "input_data");
                std::string output = run_kenneth(input);
                
                std::string response = 
                   "{\"jsonrpc\":\"2.0\",\"id\":" + id + ","
                   "\"result\":{"
                       "\"content\":[{"
                           "\"type\":\"text\","
                           "\"text\":" + json::quote(output) +
                       "}]"
                   "}}";
                send_response(response);
             } else {
                 // Error: Tool not found
                 std::string response = "{\"jsonrpc\":\"2.0\",\"id\":" + id + ",\"error\":{\"code\":-32601,\"message\":\"Tool not found\"}}";
                 send_response(response);
             }
        }
        else {
            // Ignore other messages or return error
            // send_response("...");
        }
    }
    return 0;
}
