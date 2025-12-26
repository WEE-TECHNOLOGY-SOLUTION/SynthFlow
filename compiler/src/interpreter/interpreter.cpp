#include "../../include/interpreter.h"
#include "../../include/http_client.h"
#include "../../include/http_server.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <regex>
#include <ctime>

// Platform-specific includes for OS/subprocess functionality
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <direct.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
#define getcwd _getcwd
#define chdir _chdir
#define popen _popen
#define pclose _pclose
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <pwd.h>
#endif

#include <cstring>  // Required for memset, memcpy on Linux

// Value methods
std::string Value::toString() const {
    if (isNull()) return "null";
    if (isInt()) return std::to_string(asInt());
    if (isFloat()) {
        std::ostringstream oss;
        oss << asFloat();
        return oss.str();
    }
    if (isString()) return asString();
    if (isBool()) return asBool() ? "true" : "false";
    if (isArray()) {
        std::string result = "[";
        auto arr = asArray();
        for (size_t i = 0; i < arr->size(); ++i) {
            if (i > 0) result += ", ";
            result += (*arr)[i].toString();
        }
        result += "]";
        return result;
    }
    if (isMap()) {
        std::string result = "{";
        auto m = asMap();
        bool first = true;
        for (const auto& [key, val] : *m) {
            if (!first) result += ", ";
            result += "\"" + key + "\": " + val.toString();
            first = false;
        }
        result += "}";
        return result;
    }
    if (isFunction()) return "<function>";
    return "<unknown>";
}

bool Value::isTruthy() const {
    if (isNull()) return false;
    if (isBool()) return asBool();
    if (isInt()) return asInt() != 0;
    if (isFloat()) return asFloat() != 0.0;
    if (isString()) return !asString().empty();
    if (isArray()) return !asArray()->empty();
    return true;
}

// Environment methods
void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;
}

Value Environment::get(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    if (parent) {
        return parent->get(name);
    }
    throw std::runtime_error("Undefined variable: " + name);
}

void Environment::set(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second = value;
        return;
    }
    if (parent) {
        parent->set(name, value);
        return;
    }
    throw std::runtime_error("Undefined variable: " + name);
}

bool Environment::exists(const std::string& name) const {
    if (variables.find(name) != variables.end()) return true;
    if (parent) return parent->exists(name);
    return false;
}

// Interpreter constructor
Interpreter::Interpreter() {
    globalEnv = std::make_shared<Environment>();
    currentEnv = globalEnv;
    registerBuiltins();
}

// Register built-in functions
void Interpreter::registerBuiltins() {
    // print function
    globalEnv->define("print", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            for (size_t i = 0; i < args.size(); ++i) {
                if (i > 0) std::cout << " ";
                std::cout << args[i].toString();
            }
            std::cout << std::endl;
            return Value();
        }
    )));
    
    // input function
    globalEnv->define("input", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (!args.empty()) {
                std::cout << args[0].toString();
            }
            std::string line;
            std::getline(std::cin, line);
            return Value(line);
        }
    )));
    
    // len function
    globalEnv->define("len", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("len() requires an argument");
            if (args[0].isString()) {
                return Value(static_cast<int64_t>(args[0].asString().length()));
            }
            if (args[0].isArray()) {
                return Value(static_cast<int64_t>(args[0].asArray()->size()));
            }
            throw std::runtime_error("len() requires a string or array");
        }
    )));
    
    // str function
    globalEnv->define("str", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value("");
            return Value(args[0].toString());
        }
    )));
    
    // int function
    globalEnv->define("int", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value(int64_t(0));
            if (args[0].isInt()) return args[0];
            if (args[0].isFloat()) return Value(static_cast<int64_t>(args[0].asFloat()));
            if (args[0].isString()) {
                try {
                    return Value(static_cast<int64_t>(std::stoll(args[0].asString())));
                } catch (...) {
                    throw std::runtime_error("Cannot convert string to int");
                }
            }
            if (args[0].isBool()) return Value(args[0].asBool() ? int64_t(1) : int64_t(0));
            throw std::runtime_error("Cannot convert to int");
        }
    )));
    
    // float function
    globalEnv->define("float", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value(0.0);
            if (args[0].isFloat()) return args[0];
            if (args[0].isInt()) return Value(static_cast<double>(args[0].asInt()));
            if (args[0].isString()) {
                try {
                    return Value(std::stod(args[0].asString()));
                } catch (...) {
                    throw std::runtime_error("Cannot convert string to float");
                }
            }
            throw std::runtime_error("Cannot convert to float");
        }
    )));
    
    // read_file function
    globalEnv->define("read_file", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("read_file() requires a string path");
            }
            std::ifstream file(args[0].asString());
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file: " + args[0].asString());
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return Value(buffer.str());
        }
    )));
    
    // write_file function
    globalEnv->define("write_file", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString()) {
                throw std::runtime_error("write_file() requires path and content");
            }
            std::ofstream file(args[0].asString());
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + args[0].asString());
            }
            file << args[1].toString();
            return Value(true);
        }
    )));
    
    // ===== Gemini API Built-in Functions =====
    
    // gemini_set_api_key(key) - Set the Gemini API key
    globalEnv->define("gemini_set_api_key", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("gemini_set_api_key() requires a string API key");
            }
            http::gemini::setApiKey(args[0].asString());
            return Value(true);
        }
    )));
    
    // gemini_has_api_key() - Check if API key is set
    globalEnv->define("gemini_has_api_key", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            return Value(http::gemini::hasApiKey());
        }
    )));
    
    // gemini_complete(prompt) - Generate text from prompt
    globalEnv->define("gemini_complete", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("gemini_complete() requires a prompt string");
            }
            std::string model = "gemini-2.0-flash";
            if (args.size() > 1 && args[1].isString()) {
                model = args[1].asString();
            }
            std::string result = http::gemini::generateContent(args[0].asString(), model);
            return Value(result);
        }
    )));
    
    // gemini_chat(systemPrompt, userMessage) - Chat with system instruction
    globalEnv->define("gemini_chat", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                throw std::runtime_error("gemini_chat() requires systemPrompt and userMessage strings");
            }
            std::string model = "gemini-2.0-flash";
            if (args.size() > 2 && args[2].isString()) {
                model = args[2].asString();
            }
            std::string result = http::gemini::generateContentWithSystem(
                args[0].asString(),  // system instruction
                args[1].asString(),  // user prompt
                model
            );
            return Value(result);
        }
    )));
    
    // http_get(url) - Perform HTTP GET request
    globalEnv->define("http_get", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("http_get() requires a URL string");
            }
            http::Client client;
            http::Response response = client.get(args[0].asString());
            
            // Return a map with status, body, and error
            auto resultMap = std::make_shared<Value::MapType>();
            (*resultMap)["status"] = Value(static_cast<int64_t>(response.statusCode));
            (*resultMap)["body"] = Value(response.body);
            (*resultMap)["error"] = Value(response.error);
            return Value(resultMap);
        }
    )));
    
    // http_post(url, body) - Perform HTTP POST request
    globalEnv->define("http_post", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                throw std::runtime_error("http_post() requires URL and body strings");
            }
            http::Client client;
            http::Response response = client.post(args[0].asString(), args[1].asString());
            
            // Return a map with status, body, and error
            auto resultMap = std::make_shared<Value::MapType>();
            (*resultMap)["status"] = Value(static_cast<int64_t>(response.statusCode));
            (*resultMap)["body"] = Value(response.body);
            (*resultMap)["error"] = Value(response.error);
            return Value(resultMap);
        }
    )));
    
    // ===== Array Functions =====
    
    // append(array, element) - Append element to array, return new array
    globalEnv->define("append", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2) {
                throw std::runtime_error("append() requires array and element arguments");
            }
            if (!args[0].isArray()) {
                throw std::runtime_error("append() first argument must be an array");
            }
            auto arr = std::make_shared<Value::ArrayType>(*args[0].asArray());
            arr->push_back(args[1]);
            return Value(arr);
        }
    )));
    
    // push(array, element) - Alias for append
    globalEnv->define("push", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2) {
                throw std::runtime_error("push() requires array and element arguments");
            }
            if (!args[0].isArray()) {
                throw std::runtime_error("push() first argument must be an array");
            }
            auto arr = std::make_shared<Value::ArrayType>(*args[0].asArray());
            arr->push_back(args[1]);
            return Value(arr);
        }
    )));
    
    // typeof(value) - Return type of value as string
    globalEnv->define("typeof", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value("undefined");
            const Value& v = args[0];
            if (v.isNull()) return Value("null");
            if (v.isBool()) return Value("bool");
            if (v.isInt()) return Value("int");
            if (v.isFloat()) return Value("float");
            if (v.isString()) return Value("string");
            if (v.isArray()) return Value("array");
            if (v.isMap()) return Value("map");
            if (v.isFunction()) return Value("function");
            return Value("unknown");
        }
    )));
    
    // range(start, end) or range(end) - Create array of integers
    globalEnv->define("range", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) {
                throw std::runtime_error("range() requires at least one argument");
            }
            int64_t start = 0, end = 0;
            if (args.size() == 1) {
                end = args[0].asInt();
            } else {
                start = args[0].asInt();
                end = args[1].asInt();
            }
            auto arr = std::make_shared<Value::ArrayType>();
            for (int64_t i = start; i < end; ++i) {
                arr->push_back(Value(i));
            }
            return Value(arr);
        }
    )));
    
    // ===== Math Functions =====
    
    // abs(x) - Absolute value
    globalEnv->define("abs", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("abs() requires an argument");
            if (args[0].isInt()) return Value(std::abs(args[0].asInt()));
            return Value(std::abs(args[0].asFloat()));
        }
    )));
    
    // sqrt(x) - Square root
    globalEnv->define("sqrt", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("sqrt() requires an argument");
            return Value(std::sqrt(args[0].asFloat()));
        }
    )));
    
    // pow(base, exp) - Power function
    globalEnv->define("pow", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2) throw std::runtime_error("pow() requires two arguments");
            return Value(std::pow(args[0].asFloat(), args[1].asFloat()));
        }
    )));
    
    // sin(x), cos(x), exp(x), ln(x)
    globalEnv->define("sin", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("sin() requires an argument");
            return Value(std::sin(args[0].asFloat()));
        }
    )));
    
    globalEnv->define("cos", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("cos() requires an argument");
            return Value(std::cos(args[0].asFloat()));
        }
    )));
    
    globalEnv->define("exp", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("exp() requires an argument");
            return Value(std::exp(args[0].asFloat()));
        }
    )));
    
    globalEnv->define("ln", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("ln() requires an argument");
            return Value(std::log(args[0].asFloat()));
        }
    )));
    
    // floor, ceil, round
    globalEnv->define("floor", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("floor() requires an argument");
            return Value(static_cast<int64_t>(std::floor(args[0].asFloat())));
        }
    )));
    
    globalEnv->define("ceil", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("ceil() requires an argument");
            return Value(static_cast<int64_t>(std::ceil(args[0].asFloat())));
        }
    )));
    
    globalEnv->define("round", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) throw std::runtime_error("round() requires an argument");
            return Value(static_cast<int64_t>(std::round(args[0].asFloat())));
        }
    )));
    
    // ===== Web Framework Builtins =====
    
    // Global route handler counter
    static int routeHandlerCounter = 0;
    
    // route(path, handler) or route("METHOD path", handler)
    // Minimal API: route("/api/users", json(users))
    globalEnv->define("route", Value(std::make_shared<Value::FunctionType>(
        [this](std::vector<Value>& args, Interpreter& interp) -> Value {
            if (args.size() < 2) {
                throw std::runtime_error("route() requires path and handler");
            }
            
            std::string pathSpec = args[0].asString();
            std::string method = "GET";
            std::string path = pathSpec;
            
            // Parse "POST /api/users" format
            size_t space = pathSpec.find(' ');
            if (space != std::string::npos) {
                method = pathSpec.substr(0, space);
                path = pathSpec.substr(space + 1);
            }
            
            // Store handler with unique name
            std::string handlerName = "__web_handler_" + std::to_string(routeHandlerCounter++);
            globalEnv->define(handlerName, args[1]);
            
            // Register route
            web::RouteRegistry::instance().addRoute(method, path, handlerName);
            
            return Value();
        }
    )));
    
    // serve(port) - Start HTTP server
    globalEnv->define("serve", Value(std::make_shared<Value::FunctionType>(
        [this](std::vector<Value>& args, Interpreter& interp) -> Value {
            int port = 3000;
            if (!args.empty()) {
                if (args[0].isInt()) {
                    port = static_cast<int>(args[0].asInt());
                } else if (args[0].isFloat()) {
                    port = static_cast<int>(args[0].asFloat());
                }
            }
            
            // Create server and set request handler
            web::HttpServer server;
            
            server.setRequestCallback([this](const web::Request& req) -> web::Response {
                web::Response res;
                
                // Match route
                std::map<std::string, std::string> params;
                web::Route* route = web::RouteRegistry::instance().matchRoute(
                    req.method, req.path, params);
                
                if (!route) {
                    res.statusCode = 404;
                    res.contentType = "application/json";
                    res.body = "{\"error\": \"Not Found\", \"path\": \"" + req.path + "\"}";
                    return res;
                }
                
                // Get handler
                try {
                    Value handler = globalEnv->get(route->handlerName);
                    
                    if (handler.isFunction()) {
                        // Create request object for handler
                        auto reqMap = std::make_shared<std::map<std::string, Value>>();
                        (*reqMap)["method"] = Value(req.method);
                        (*reqMap)["path"] = Value(req.path);
                        (*reqMap)["body"] = Value(req.body);
                        
                        // Add params
                        auto paramsMap = std::make_shared<std::map<std::string, Value>>();
                        for (const auto& [k, v] : params) {
                            (*paramsMap)[k] = Value(v);
                        }
                        (*reqMap)["params"] = Value(paramsMap);
                        
                        // Add query params
                        auto queryMap = std::make_shared<std::map<std::string, Value>>();
                        for (const auto& [k, v] : req.queryParams) {
                            (*queryMap)[k] = Value(v);
                        }
                        (*reqMap)["query"] = Value(queryMap);
                        
                        // Call handler with request
                        std::vector<Value> handlerArgs;
                        handlerArgs.push_back(Value(reqMap));
                        
                        auto fn = handler.asFunction();
                        Value result = (*fn)(handlerArgs, *this);
                        
                        // Process result
                        if (result.isMap()) {
                            auto map = result.asMap();
                            auto typeIt = map->find("__type");
                            if (typeIt != map->end()) {
                                std::string type = typeIt->second.asString();
                                auto contentIt = map->find("content");
                                if (type == "json") {
                                    res.contentType = "application/json";
                                    res.body = contentIt != map->end() ? contentIt->second.toString() : "{}";
                                } else if (type == "html") {
                                    res.contentType = "text/html; charset=utf-8";
                                    res.body = contentIt != map->end() ? contentIt->second.asString() : "";
                                } else {
                                    res.body = contentIt != map->end() ? contentIt->second.toString() : "";
                                }
                            } else {
                                res.contentType = "application/json";
                                res.body = result.toString();
                            }
                        } else {
                            res.body = result.toString();
                        }
                    } else if (handler.isMap()) {
                        // Direct response map
                        auto map = handler.asMap();
                        auto typeIt = map->find("__type");
                        if (typeIt != map->end()) {
                            std::string type = typeIt->second.asString();
                            auto contentIt = map->find("content");
                            if (type == "json") {
                                res.contentType = "application/json";
                                res.body = contentIt != map->end() ? contentIt->second.toString() : "{}";
                            } else if (type == "html") {
                                res.contentType = "text/html; charset=utf-8";
                                res.body = contentIt != map->end() ? contentIt->second.asString() : "";
                            }
                        } else {
                            res.contentType = "application/json";
                            res.body = handler.toString();
                        }
                    } else if (handler.isString()) {
                        res.contentType = "text/plain";
                        res.body = handler.asString();
                    } else {
                        res.contentType = "application/json";
                        res.body = handler.toString();
                    }
                    
                    res.statusCode = 200;
                } catch (const std::exception& e) {
                    res.statusCode = 500;
                    res.contentType = "application/json";
                    res.body = "{\"error\": \"" + std::string(e.what()) + "\"}";
                }
                
                return res;
            });
            
            // Start server (blocking)
            server.start(port);
            
            return Value();
        }
    )));
    
    // json(data) - Create JSON response block
    globalEnv->define("json", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            auto resultMap = std::make_shared<std::map<std::string, Value>>();
            (*resultMap)["__type"] = Value("json");
            if (!args.empty()) {
                (*resultMap)["content"] = args[0];
            } else {
                (*resultMap)["content"] = Value("{}");
            }
            return Value(resultMap);
        }
    )));
    
    // html(content) - Create HTML response block
    globalEnv->define("html", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            auto resultMap = std::make_shared<std::map<std::string, Value>>();
            (*resultMap)["__type"] = Value("html");
            if (!args.empty()) {
                (*resultMap)["content"] = args[0];
            } else {
                (*resultMap)["content"] = Value("");
            }
            return Value(resultMap);
        }
    )));
    
    // text(content) - Create text response
    globalEnv->define("text", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value("");
            return args[0];
        }
    )));
    
    // use(middleware...) - Add middleware
    globalEnv->define("use", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            for (auto& arg : args) {
                if (arg.isString()) {
                    web::RouteRegistry::instance().addMiddleware(arg.asString());
                }
            }
            return Value();
        }
    )));
    
    // ===== OS & Subprocess Built-in Functions =====
    
    // __builtin_exec(cmd) - Execute command and return result map
    globalEnv->define("__builtin_exec", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_exec() requires a command string");
            }
            
            std::string cmd = args[0].asString();
            std::string output;
            std::string error;
            int returnCode = 0;
            
            #ifdef _WIN32
            // Windows: Use _popen
            FILE* pipe = _popen(cmd.c_str(), "r");
            #else
            // Unix: Redirect stderr to stdout
            FILE* pipe = popen((cmd + " 2>&1").c_str(), "r");
            #endif
            
            if (!pipe) {
                error = "Failed to execute command";
                returnCode = -1;
            } else {
                char buffer[256];
                while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    output += buffer;
                }
                #ifdef _WIN32
                returnCode = _pclose(pipe);
                #else
                returnCode = pclose(pipe);
                returnCode = WEXITSTATUS(returnCode);
                #endif
            }
            
            auto resultMap = std::make_shared<Value::MapType>();
            (*resultMap)["stdout"] = Value(output);
            (*resultMap)["stderr"] = Value(error);
            (*resultMap)["returncode"] = Value(static_cast<int64_t>(returnCode));
            return Value(resultMap);
        }
    )));
    
    // __builtin_shell(cmd) - Execute through shell
    globalEnv->define("__builtin_shell", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter& interp) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_shell() requires a command string");
            }
            
            std::string cmd = args[0].asString();
            #ifdef _WIN32
            cmd = "cmd /c " + cmd;
            #else
            cmd = "sh -c \"" + cmd + "\"";
            #endif
            
            std::vector<Value> execArgs;
            execArgs.push_back(Value(cmd));
            return interp.getGlobalEnv()->get("__builtin_exec").asFunction()->operator()(execArgs, interp);
        }
    )));
    
    // __builtin_env_get(key) - Get environment variable
    globalEnv->define("__builtin_env_get", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_env_get() requires a key string");
            }
            
            const char* val = std::getenv(args[0].asString().c_str());
            return val ? Value(std::string(val)) : Value("");
        }
    )));
    
    // __builtin_env_set(key, value) - Set environment variable
    globalEnv->define("__builtin_env_set", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                throw std::runtime_error("__builtin_env_set() requires key and value strings");
            }
            
            #ifdef _WIN32
            _putenv_s(args[0].asString().c_str(), args[1].asString().c_str());
            #else
            setenv(args[0].asString().c_str(), args[1].asString().c_str(), 1);
            #endif
            return Value(true);
        }
    )));
    
    // __builtin_getcwd() - Get current working directory
    globalEnv->define("__builtin_getcwd", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            char buffer[4096];
            if (getcwd(buffer, sizeof(buffer)) != nullptr) {
                return Value(std::string(buffer));
            }
            return Value("");
        }
    )));
    
    // __builtin_chdir(path) - Change working directory
    globalEnv->define("__builtin_chdir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_chdir() requires a path string");
            }
            return Value(chdir(args[0].asString().c_str()) == 0);
        }
    )));
    
    // __builtin_platform() - Get OS name
    globalEnv->define("__builtin_platform", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #ifdef _WIN32
            return Value("windows");
            #elif __APPLE__
            return Value("darwin");
            #else
            return Value("linux");
            #endif
        }
    )));
    
    // __builtin_arch() - Get architecture
    globalEnv->define("__builtin_arch", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #if defined(__x86_64__) || defined(_M_X64)
            return Value("x86_64");
            #elif defined(__aarch64__) || defined(_M_ARM64)
            return Value("arm64");
            #elif defined(__i386__) || defined(_M_IX86)
            return Value("x86");
            #else
            return Value("unknown");
            #endif
        }
    )));
    
    // __builtin_hostname() - Get hostname
    globalEnv->define("__builtin_hostname", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            char hostname[256];
            #ifdef _WIN32
            DWORD size = sizeof(hostname);
            GetComputerNameA(hostname, &size);
            #else
            gethostname(hostname, sizeof(hostname));
            #endif
            return Value(std::string(hostname));
        }
    )));
    
    // __builtin_username() - Get current username
    globalEnv->define("__builtin_username", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #ifdef _WIN32
            char username[256];
            DWORD size = sizeof(username);
            GetUserNameA(username, &size);
            return Value(std::string(username));
            #else
            struct passwd* pw = getpwuid(getuid());
            return pw ? Value(std::string(pw->pw_name)) : Value("");
            #endif
        }
    )));
    
    // __builtin_homedir() - Get home directory
    globalEnv->define("__builtin_homedir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #ifdef _WIN32
            const char* home = std::getenv("USERPROFILE");
            #else
            const char* home = std::getenv("HOME");
            #endif
            return home ? Value(std::string(home)) : Value("");
        }
    )));
    
    // __builtin_tempdir() - Get temp directory
    globalEnv->define("__builtin_tempdir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #ifdef _WIN32
            char temp[MAX_PATH];
            GetTempPathA(MAX_PATH, temp);
            return Value(std::string(temp));
            #else
            const char* tmp = std::getenv("TMPDIR");
            return tmp ? Value(std::string(tmp)) : Value("/tmp");
            #endif
        }
    )));
    
    // __builtin_path_exists(path) - Check if path exists
    globalEnv->define("__builtin_path_exists", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_path_exists() requires a path string");
            }
            std::ifstream f(args[0].asString());
            return Value(f.good());
        }
    )));
    
    // __builtin_is_file(path) - Check if path is a file
    globalEnv->define("__builtin_is_file", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_is_file() requires a path string");
            }
            #ifdef _WIN32
            DWORD attr = GetFileAttributesA(args[0].asString().c_str());
            return Value(attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
            #else
            struct stat st;
            if (stat(args[0].asString().c_str(), &st) != 0) return Value(false);
            return Value(S_ISREG(st.st_mode));
            #endif
        }
    )));
    
    // __builtin_is_dir(path) - Check if path is a directory
    globalEnv->define("__builtin_is_dir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_is_dir() requires a path string");
            }
            #ifdef _WIN32
            DWORD attr = GetFileAttributesA(args[0].asString().c_str());
            return Value(attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
            #else
            struct stat st;
            if (stat(args[0].asString().c_str(), &st) != 0) return Value(false);
            return Value(S_ISDIR(st.st_mode));
            #endif
        }
    )));
    
    // __builtin_listdir(path) - List directory contents
    globalEnv->define("__builtin_listdir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_listdir() requires a path string");
            }
            
            auto entries = std::make_shared<Value::ArrayType>();
            std::string path = args[0].asString();
            
            #ifdef _WIN32
            WIN32_FIND_DATAA findData;
            std::string searchPath = path + "\\*";
            HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
            
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    std::string name = findData.cFileName;
                    if (name != "." && name != "..") {
                        entries->push_back(Value(name));
                    }
                } while (FindNextFileA(hFind, &findData));
                FindClose(hFind);
            }
            #else
            DIR* dir = opendir(path.c_str());
            if (dir) {
                struct dirent* entry;
                while ((entry = readdir(dir)) != nullptr) {
                    std::string name = entry->d_name;
                    if (name != "." && name != "..") {
                        entries->push_back(Value(name));
                    }
                }
                closedir(dir);
            }
            #endif
            
            return Value(entries);
        }
    )));
    
    // __builtin_mkdir(path) - Create directory
    globalEnv->define("__builtin_mkdir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_mkdir() requires a path string");
            }
            #ifdef _WIN32
            return Value(CreateDirectoryA(args[0].asString().c_str(), nullptr) != 0);
            #else
            return Value(mkdir(args[0].asString().c_str(), 0755) == 0);
            #endif
        }
    )));
    
    // __builtin_remove(path) - Remove file
    globalEnv->define("__builtin_remove", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_remove() requires a path string");
            }
            return Value(std::remove(args[0].asString().c_str()) == 0);
        }
    )));
    
    // __builtin_rmdir(path) - Remove directory
    globalEnv->define("__builtin_rmdir", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_rmdir() requires a path string");
            }
            #ifdef _WIN32
            return Value(RemoveDirectoryA(args[0].asString().c_str()) != 0);
            #else
            return Value(rmdir(args[0].asString().c_str()) == 0);
            #endif
        }
    )));
    
    // __builtin_rename(src, dest) - Rename/move file
    globalEnv->define("__builtin_rename", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                throw std::runtime_error("__builtin_rename() requires source and dest strings");
            }
            return Value(std::rename(args[0].asString().c_str(), args[1].asString().c_str()) == 0);
        }
    )));
    
    // __builtin_getpid() - Get process ID
    globalEnv->define("__builtin_getpid", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #ifdef _WIN32
            return Value(static_cast<int64_t>(GetCurrentProcessId()));
            #else
            return Value(static_cast<int64_t>(getpid()));
            #endif
        }
    )));
    
    // __builtin_exit(code) - Exit program
    globalEnv->define("__builtin_exit", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            int code = 0;
            if (!args.empty() && args[0].isInt()) {
                code = static_cast<int>(args[0].asInt());
            }
            std::exit(code);
            return Value();
        }
    )));
    
    // __builtin_time() - Get Unix timestamp in seconds
    globalEnv->define("__builtin_time", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            auto now = std::chrono::system_clock::now();
            auto epoch = now.time_since_epoch();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            return Value(static_cast<int64_t>(seconds.count()));
        }
    )));
    
    // __builtin_time_ms() - Get Unix timestamp in milliseconds
    globalEnv->define("__builtin_time_ms", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            auto now = std::chrono::system_clock::now();
            auto epoch = now.time_since_epoch();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
            return Value(static_cast<int64_t>(ms.count()));
        }
    )));
    
    // __builtin_sleep(ms) - Sleep for milliseconds
    globalEnv->define("__builtin_sleep", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value();
            int64_t ms = args[0].isInt() ? args[0].asInt() : static_cast<int64_t>(args[0].asFloat());
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            return Value();
        }
    )));
    
    // __builtin_substring(str, start, end) - Get substring
    globalEnv->define("__builtin_substring", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3 || !args[0].isString()) {
                throw std::runtime_error("__builtin_substring() requires string, start, end");
            }
            std::string str = args[0].asString();
            int64_t start = args[1].asInt();
            int64_t end = args[2].asInt();
            if (start < 0) start = 0;
            if (end > static_cast<int64_t>(str.length())) end = str.length();
            if (start >= end) return Value("");
            return Value(str.substr(start, end - start));
        }
    )));
    
    // __builtin_which(program) - Find executable
    globalEnv->define("__builtin_which", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter& interp) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_which() requires a program name");
            }
            
            std::string cmd;
            #ifdef _WIN32
            cmd = "where " + args[0].asString() + " 2>nul";
            #else
            cmd = "which " + args[0].asString() + " 2>/dev/null";
            #endif
            
            std::vector<Value> execArgs;
            execArgs.push_back(Value(cmd));
            Value result = interp.getGlobalEnv()->get("__builtin_exec").asFunction()->operator()(execArgs, interp);
            
            if (result.isMap()) {
                auto map = result.asMap();
                auto it = map->find("stdout");
                if (it != map->end()) {
                    std::string path = it->second.asString();
                    // Trim newline
                    while (!path.empty() && (path.back() == '\n' || path.back() == '\r')) {
                        path.pop_back();
                    }
                    return Value(path);
                }
            }
            return Value("");
        }
    )));
    
    // ========================================
    // NETWORKING BUILT-INS
    // ========================================
    
    // __builtin_tcp_connect(host, port) - Connect TCP socket
    globalEnv->define("__builtin_tcp_connect", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isInt()) {
                throw std::runtime_error("__builtin_tcp_connect(host, port) requires string and int");
            }
            
            std::string host = args[0].asString();
            int port = static_cast<int>(args[1].asInt());
            
            #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            #endif
            
            int sock = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
            if (sock < 0) {
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["connected"] = Value(false);
                (*result)["error"] = Value("Failed to create socket");
                return Value(result);
            }
            
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            
            // Resolve hostname
            struct hostent* he = gethostbyname(host.c_str());
            if (he == nullptr) {
                #ifdef _WIN32
                closesocket(sock);
                #else
                close(sock);
                #endif
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["connected"] = Value(false);
                (*result)["error"] = Value("Failed to resolve hostname");
                return Value(result);
            }
            memcpy(&serverAddr.sin_addr, he->h_addr_list[0], he->h_length);
            
            int connResult = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            if (connResult < 0) {
                #ifdef _WIN32
                closesocket(sock);
                #else
                close(sock);
                #endif
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["connected"] = Value(false);
                (*result)["error"] = Value("Connection failed");
                return Value(result);
            }
            
            auto result = std::make_shared<std::map<std::string, Value>>();
            (*result)["fd"] = Value(static_cast<int64_t>(sock));
            (*result)["connected"] = Value(true);
            (*result)["host"] = Value(host);
            (*result)["port"] = Value(static_cast<int64_t>(port));
            return Value(result);
        }
    )));
    
    // __builtin_tcp_send(fd, data) - Send data over TCP
    globalEnv->define("__builtin_tcp_send", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isInt() || !args[1].isString()) {
                throw std::runtime_error("__builtin_tcp_send(fd, data) requires int and string");
            }
            int sock = static_cast<int>(args[0].asInt());
            std::string data = args[1].asString();
            int sent = send(sock, data.c_str(), static_cast<int>(data.length()), 0);
            return Value(static_cast<int64_t>(sent));
        }
    )));
    
    // __builtin_tcp_recv(fd, maxBytes) - Receive data from TCP
    globalEnv->define("__builtin_tcp_recv", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isInt() || !args[1].isInt()) {
                throw std::runtime_error("__builtin_tcp_recv(fd, maxBytes) requires two ints");
            }
            int sock = static_cast<int>(args[0].asInt());
            int maxBytes = static_cast<int>(args[1].asInt());
            std::vector<char> buffer(maxBytes + 1, 0);
            int received = recv(sock, buffer.data(), maxBytes, 0);
            if (received <= 0) return Value("");
            return Value(std::string(buffer.data(), received));
        }
    )));
    
    // __builtin_tcp_close(fd) - Close TCP socket
    globalEnv->define("__builtin_tcp_close", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) return Value();
            int sock = static_cast<int>(args[0].asInt());
            #ifdef _WIN32
            closesocket(sock);
            #else
            close(sock);
            #endif
            return Value();
        }
    )));
    
    // __builtin_tcp_listen(port) - Create TCP server
    globalEnv->define("__builtin_tcp_listen", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) {
                throw std::runtime_error("__builtin_tcp_listen(port) requires int");
            }
            int port = static_cast<int>(args[0].asInt());
            
            #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            #endif
            
            int sock = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
            if (sock < 0) {
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["listening"] = Value(false);
                return Value(result);
            }
            
            int opt = 1;
            #ifdef _WIN32
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
            #else
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            #endif
            
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = INADDR_ANY;
            serverAddr.sin_port = htons(port);
            
            if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                #ifdef _WIN32
                closesocket(sock);
                #else
                close(sock);
                #endif
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["listening"] = Value(false);
                (*result)["error"] = Value("Bind failed");
                return Value(result);
            }
            
            if (listen(sock, 10) < 0) {
                #ifdef _WIN32
                closesocket(sock);
                #else
                close(sock);
                #endif
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["listening"] = Value(false);
                (*result)["error"] = Value("Listen failed");
                return Value(result);
            }
            
            auto result = std::make_shared<std::map<std::string, Value>>();
            (*result)["fd"] = Value(static_cast<int64_t>(sock));
            (*result)["listening"] = Value(true);
            (*result)["port"] = Value(static_cast<int64_t>(port));
            return Value(result);
        }
    )));
    
    // __builtin_tcp_accept(fd) - Accept incoming connection
    globalEnv->define("__builtin_tcp_accept", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) {
                throw std::runtime_error("__builtin_tcp_accept(fd) requires int");
            }
            int serverSock = static_cast<int>(args[0].asInt());
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSock = static_cast<int>(accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen));
            
            if (clientSock < 0) {
                auto result = std::make_shared<std::map<std::string, Value>>();
                (*result)["fd"] = Value(static_cast<int64_t>(-1));
                (*result)["connected"] = Value(false);
                return Value(result);
            }
            
            auto result = std::make_shared<std::map<std::string, Value>>();
            (*result)["fd"] = Value(static_cast<int64_t>(clientSock));
            (*result)["connected"] = Value(true);
            (*result)["remote_addr"] = Value(std::string(inet_ntoa(clientAddr.sin_addr)));
            (*result)["remote_port"] = Value(static_cast<int64_t>(ntohs(clientAddr.sin_port)));
            return Value(result);
        }
    )));
    
    // __builtin_dns_lookup(hostname) - DNS resolve
    globalEnv->define("__builtin_dns_lookup", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) {
                throw std::runtime_error("__builtin_dns_lookup(hostname) requires string");
            }
            std::string hostname = args[0].asString();
            
            #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            #endif
            
            struct hostent* he = gethostbyname(hostname.c_str());
            if (he == nullptr) return Value("");
            
            struct in_addr addr;
            memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
            return Value(std::string(inet_ntoa(addr)));
        }
    )));
    
    // __builtin_port_check(host, port, timeout_ms) - Check if port is open
    globalEnv->define("__builtin_port_check", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3) {
                throw std::runtime_error("__builtin_port_check(host, port, timeout) requires 3 args");
            }
            std::string host = args[0].asString();
            int port = static_cast<int>(args[1].asInt());
            int timeout_ms = static_cast<int>(args[2].asInt());
            
            #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            #endif
            
            int sock = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
            if (sock < 0) return Value(false);
            
            // Set timeout
            #ifdef _WIN32
            DWORD tv = timeout_ms;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
            #else
            struct timeval tv;
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (timeout_ms % 1000) * 1000;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
            #endif
            
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            
            struct hostent* he = gethostbyname(host.c_str());
            if (he == nullptr) {
                #ifdef _WIN32
                closesocket(sock);
                #else
                close(sock);
                #endif
                return Value(false);
            }
            memcpy(&serverAddr.sin_addr, he->h_addr_list[0], he->h_length);
            
            int result = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            #ifdef _WIN32
            closesocket(sock);
            #else
            close(sock);
            #endif
            
            return Value(result == 0);
        }
    )));
    
    // __builtin_get_local_ip() - Get local IP address
    globalEnv->define("__builtin_get_local_ip", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            char hostname[256];
            if (gethostname(hostname, sizeof(hostname)) != 0) {
                return Value("127.0.0.1");
            }
            
            #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            #endif
            
            struct hostent* he = gethostbyname(hostname);
            if (he == nullptr) return Value("127.0.0.1");
            
            struct in_addr addr;
            memcpy(&addr, he->h_addr_list[0], sizeof(struct in_addr));
            return Value(std::string(inet_ntoa(addr)));
        }
    )));
    
    // __builtin_udp_create() - Create UDP socket
    globalEnv->define("__builtin_udp_create", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            #endif
            
            int sock = static_cast<int>(socket(AF_INET, SOCK_DGRAM, 0));
            auto result = std::make_shared<std::map<std::string, Value>>();
            (*result)["fd"] = Value(static_cast<int64_t>(sock));
            (*result)["type"] = Value("udp");
            return Value(result);
        }
    )));
    
    // __builtin_udp_sendto(fd, host, port, data) - Send UDP packet
    globalEnv->define("__builtin_udp_sendto", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 4) {
                throw std::runtime_error("__builtin_udp_sendto requires fd, host, port, data");
            }
            int sock = static_cast<int>(args[0].asInt());
            std::string host = args[1].asString();
            int port = static_cast<int>(args[2].asInt());
            std::string data = args[3].asString();
            
            struct sockaddr_in destAddr;
            memset(&destAddr, 0, sizeof(destAddr));
            destAddr.sin_family = AF_INET;
            destAddr.sin_port = htons(port);
            
            struct hostent* he = gethostbyname(host.c_str());
            if (he == nullptr) return Value(static_cast<int64_t>(-1));
            memcpy(&destAddr.sin_addr, he->h_addr_list[0], he->h_length);
            
            int sent = sendto(sock, data.c_str(), static_cast<int>(data.length()), 0,
                             (struct sockaddr*)&destAddr, sizeof(destAddr));
            return Value(static_cast<int64_t>(sent));
        }
    )));
    
    // __builtin_udp_close(fd) - Close UDP socket
    globalEnv->define("__builtin_udp_close", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) return Value();
            int sock = static_cast<int>(args[0].asInt());
            #ifdef _WIN32
            closesocket(sock);
            #else
            close(sock);
            #endif
            return Value();
        }
    )));
    
    // ========================================
    // SECURITY BUILT-INS
    // ========================================
    
    // __builtin_base64url_encode(data) - Base64 URL-safe encode
    globalEnv->define("__builtin_base64url_encode", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) return Value("");
            std::string data = args[0].asString();
            
            static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
            std::string encoded;
            int val = 0, bits = -6;
            const unsigned int mask = 0x3F;
            
            for (unsigned char c : data) {
                val = (val << 8) + c;
                bits += 8;
                while (bits >= 0) {
                    encoded.push_back(base64_chars[(val >> bits) & mask]);
                    bits -= 6;
                }
            }
            if (bits > -6) {
                encoded.push_back(base64_chars[((val << 8) >> (bits + 8)) & mask]);
            }
            // Remove padding
            while (!encoded.empty() && encoded.back() == '=') {
                encoded.pop_back();
            }
            return Value(encoded);
        }
    )));
    
    // __builtin_base64url_decode(data) - Base64 URL-safe decode
    globalEnv->define("__builtin_base64url_decode", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) return Value("");
            std::string data = args[0].asString();
            
            static const int decode_table[256] = {
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,
                52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
                -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
                15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,63,
                -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
                41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
            };
            
            std::string decoded;
            int val = 0, bits = -8;
            for (unsigned char c : data) {
                int d = decode_table[c];
                if (d == -1) continue;
                val = (val << 6) + d;
                bits += 6;
                if (bits >= 0) {
                    decoded.push_back((val >> bits) & 0xFF);
                    bits -= 8;
                }
            }
            return Value(decoded);
        }
    )));
    
    // __builtin_regex_test(pattern, text) - Test regex match
    globalEnv->define("__builtin_regex_test", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                return Value(false);
            }
            try {
                std::regex pattern(args[0].asString());
                return Value(std::regex_search(args[1].asString(), pattern));
            } catch (...) {
                return Value(false);
            }
        }
    )));
    
    // __builtin_split(str, delimiter) - Split string
    globalEnv->define("__builtin_split", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                return Value(std::make_shared<std::vector<Value>>());
            }
            std::string str = args[0].asString();
            std::string delim = args[1].asString();
            auto result = std::make_shared<std::vector<Value>>();
            
            size_t pos = 0, prev = 0;
            while ((pos = str.find(delim, prev)) != std::string::npos) {
                result->push_back(Value(str.substr(prev, pos - prev)));
                prev = pos + delim.length();
            }
            result->push_back(Value(str.substr(prev)));
            return Value(result);
        }
    )));
    
    // __builtin_join(arr, delimiter) - Join array
    globalEnv->define("__builtin_join", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isArray() || !args[1].isString()) {
                return Value("");
            }
            auto arr = args[0].asArray();
            std::string delim = args[1].asString();
            std::string result;
            for (size_t i = 0; i < arr->size(); ++i) {
                if (i > 0) result += delim;
                result += (*arr)[i].toString();
            }
            return Value(result);
        }
    )));
    
    // __builtin_trim(str) - Trim whitespace
    globalEnv->define("__builtin_trim", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) return Value("");
            std::string str = args[0].asString();
            size_t start = str.find_first_not_of(" \t\n\r");
            if (start == std::string::npos) return Value("");
            size_t end = str.find_last_not_of(" \t\n\r");
            return Value(str.substr(start, end - start + 1));
        }
    )));
    
    // __builtin_lowercase(str) - Convert to lowercase
    globalEnv->define("__builtin_lowercase", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) return Value("");
            std::string str = args[0].asString();
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            return Value(str);
        }
    )));
    
    // __builtin_starts_with(str, prefix) - Check prefix
    globalEnv->define("__builtin_starts_with", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                return Value(false);
            }
            std::string str = args[0].asString();
            std::string prefix = args[1].asString();
            return Value(str.length() >= prefix.length() && 
                        str.compare(0, prefix.length(), prefix) == 0);
        }
    )));
    
    // __builtin_contains(str, search) - Check if contains
    globalEnv->define("__builtin_contains", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                return Value(false);
            }
            return Value(args[0].asString().find(args[1].asString()) != std::string::npos);
        }
    )));
    
    // __builtin_replace_all(str, from, to) - Replace all occurrences
    globalEnv->define("__builtin_replace_all", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3 || !args[0].isString() || !args[1].isString() || !args[2].isString()) {
                return Value("");
            }
            std::string str = args[0].asString();
            std::string from = args[1].asString();
            std::string to = args[2].asString();
            if (from.empty()) return Value(str);
            
            size_t pos = 0;
            while ((pos = str.find(from, pos)) != std::string::npos) {
                str.replace(pos, from.length(), to);
                pos += to.length();
            }
            return Value(str);
        }
    )));
    
    // __builtin_json_stringify(map) - Convert map to JSON string
    globalEnv->define("__builtin_json_stringify", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty()) return Value("{}");
            return Value(args[0].toString());
        }
    )));
    
    // __builtin_json_parse(str) - Parse JSON to map (simplified)
    globalEnv->define("__builtin_json_parse", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            // Simplified - just return empty map for now
            return Value(std::make_shared<std::map<std::string, Value>>());
        }
    )));
    
    // __builtin_random_bytes(length) - Generate random bytes (hex)
    globalEnv->define("__builtin_random_bytes", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) return Value("");
            int length = static_cast<int>(args[0].asInt());
            
            static const char hex_chars[] = "0123456789abcdef";
            std::string result;
            result.reserve(length * 2);
            
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            for (int i = 0; i < length; ++i) {
                unsigned char byte = static_cast<unsigned char>(std::rand() % 256);
                result += hex_chars[byte >> 4];
                result += hex_chars[byte & 0x0F];
            }
            return Value(result);
        }
    )));
    
    // __builtin_uuid() - Generate UUID v4
    globalEnv->define("__builtin_uuid", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            static const char hex_chars[] = "0123456789abcdef";
            std::string uuid;
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            
            for (int i = 0; i < 36; ++i) {
                if (i == 8 || i == 13 || i == 18 || i == 23) {
                    uuid += '-';
                } else if (i == 14) {
                    uuid += '4';  // Version 4
                } else if (i == 19) {
                    uuid += hex_chars[(std::rand() % 4) + 8];  // Variant
                } else {
                    uuid += hex_chars[std::rand() % 16];
                }
            }
            return Value(uuid);
        }
    )));
    
    // __builtin_secure_compare(a, b) - Constant-time comparison
    globalEnv->define("__builtin_secure_compare", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                return Value(false);
            }
            std::string a = args[0].asString();
            std::string b = args[1].asString();
            if (a.length() != b.length()) return Value(false);
            
            volatile int result = 0;
            for (size_t i = 0; i < a.length(); ++i) {
                result |= (a[i] ^ b[i]);
            }
            return Value(result == 0);
        }
    )));
    
    // ========================================
    // API MANAGEMENT BUILT-INS
    // ========================================
    
    // __builtin_keys(map) - Get map keys as array
    globalEnv->define("__builtin_keys", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isMap()) {
                return Value(std::make_shared<std::vector<Value>>());
            }
            auto map = args[0].asMap();
            auto keys = std::make_shared<std::vector<Value>>();
            for (const auto& [key, _] : *map) {
                keys->push_back(Value(key));
            }
            return Value(keys);
        }
    )));
    
    // __builtin_index_of(str, search, start) - Find index of substring
    globalEnv->define("__builtin_index_of", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3 || !args[0].isString() || !args[1].isString()) {
                return Value(static_cast<int64_t>(-1));
            }
            std::string str = args[0].asString();
            std::string search = args[1].asString();
            int64_t start = args[2].isInt() ? args[2].asInt() : 0;
            
            if (start < 0 || start >= static_cast<int64_t>(str.length())) {
                return Value(static_cast<int64_t>(-1));
            }
            
            size_t pos = str.find(search, static_cast<size_t>(start));
            if (pos == std::string::npos) {
                return Value(static_cast<int64_t>(-1));
            }
            return Value(static_cast<int64_t>(pos));
        }
    )));
    
    // __builtin_uppercase(str) - Convert to uppercase
    globalEnv->define("__builtin_uppercase", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isString()) return Value("");
            std::string str = args[0].asString();
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return Value(str);
        }
    )));
    
    // __builtin_ends_with(str, suffix) - Check suffix
    globalEnv->define("__builtin_ends_with", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
                return Value(false);
            }
            std::string str = args[0].asString();
            std::string suffix = args[1].asString();
            if (suffix.length() > str.length()) return Value(false);
            return Value(str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0);
        }
    )));
    
    // ========================================
    // FFI / SYSTEMS BUILT-INS
    // ========================================
    
    // Memory buffer storage
    static std::map<int64_t, std::vector<uint8_t>> memoryBuffers;
    static int64_t nextBufferId = 1;
    
    // __builtin_alloc_buffer(size) - Allocate memory buffer
    globalEnv->define("__builtin_alloc_buffer", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) return Value(static_cast<int64_t>(0));
            int64_t size = args[0].asInt();
            if (size <= 0 || size > 1024 * 1024 * 100) return Value(static_cast<int64_t>(0)); // Max 100MB
            
            int64_t id = nextBufferId++;
            memoryBuffers[id] = std::vector<uint8_t>(size, 0);
            return Value(id);
        }
    )));
    
    // __builtin_free_buffer(id) - Free memory buffer
    globalEnv->define("__builtin_free_buffer", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) return Value(false);
            int64_t id = args[0].asInt();
            auto it = memoryBuffers.find(id);
            if (it != memoryBuffers.end()) {
                memoryBuffers.erase(it);
                return Value(true);
            }
            return Value(false);
        }
    )));
    
    // __builtin_buffer_write(id, offset, data) - Write to buffer
    globalEnv->define("__builtin_buffer_write", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3 || !args[0].isInt() || !args[1].isInt() || !args[2].isArray()) {
                return Value(false);
            }
            int64_t id = args[0].asInt();
            int64_t offset = args[1].asInt();
            auto data = args[2].asArray();
            
            auto it = memoryBuffers.find(id);
            if (it == memoryBuffers.end()) return Value(false);
            
            for (size_t i = 0; i < data->size() && offset + i < it->second.size(); ++i) {
                it->second[offset + i] = static_cast<uint8_t>((*data)[i].asInt());
            }
            return Value(true);
        }
    )));
    
    // __builtin_buffer_read(id, offset, length) - Read from buffer
    globalEnv->define("__builtin_buffer_read", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3 || !args[0].isInt() || !args[1].isInt() || !args[2].isInt()) {
                return Value(std::make_shared<std::vector<Value>>());
            }
            int64_t id = args[0].asInt();
            int64_t offset = args[1].asInt();
            int64_t length = args[2].asInt();
            
            auto result = std::make_shared<std::vector<Value>>();
            auto it = memoryBuffers.find(id);
            if (it == memoryBuffers.end()) return Value(result);
            
            for (int64_t i = 0; i < length && offset + i < static_cast<int64_t>(it->second.size()); ++i) {
                result->push_back(Value(static_cast<int64_t>(it->second[offset + i])));
            }
            return Value(result);
        }
    )));
    
    // __builtin_time_ms() - Get current time in milliseconds
    globalEnv->define("__builtin_time_ms", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            return Value(static_cast<int64_t>(ms));
        }
    )));
    
    // __builtin_sleep(ms) - Sleep for milliseconds
    globalEnv->define("__builtin_sleep", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.empty() || !args[0].isInt()) return Value();
            int64_t ms = args[0].asInt();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            return Value();
        }
    )));
    
    // __builtin_substring(str, start, end) - Get substring
    globalEnv->define("__builtin_substring", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>& args, Interpreter&) -> Value {
            if (args.size() < 3 || !args[0].isString() || !args[1].isInt() || !args[2].isInt()) {
                return Value("");
            }
            std::string str = args[0].asString();
            int64_t start = args[1].asInt();
            int64_t end = args[2].asInt();
            
            if (start < 0) start = 0;
            if (end > static_cast<int64_t>(str.length())) end = str.length();
            if (start >= end) return Value("");
            
            return Value(str.substr(start, end - start));
        }
    )));
    
    // Placeholder FFI functions (would need native implementation)
    globalEnv->define("__builtin_load_library", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(static_cast<int64_t>(0)); }
    )));
    globalEnv->define("__builtin_unload_library", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(); }
    )));
    globalEnv->define("__builtin_get_proc_address", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(static_cast<int64_t>(0)); }
    )));
    globalEnv->define("__builtin_ffi_call", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(); }
    )));
    globalEnv->define("__builtin_mmap", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(static_cast<int64_t>(0)); }
    )));
    globalEnv->define("__builtin_munmap", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(false); }
    )));
    
    // GPIO/I2C/SPI placeholders
    globalEnv->define("__builtin_gpio_mode", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(true); }
    )));
    globalEnv->define("__builtin_gpio_write", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(true); }
    )));
    globalEnv->define("__builtin_gpio_read", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(static_cast<int64_t>(0)); }
    )));
    globalEnv->define("__builtin_i2c_open", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(static_cast<int64_t>(1)); }
    )));
    globalEnv->define("__builtin_i2c_write", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(true); }
    )));
    globalEnv->define("__builtin_i2c_read", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(std::make_shared<std::vector<Value>>()); }
    )));
    globalEnv->define("__builtin_i2c_close", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(true); }
    )));
    globalEnv->define("__builtin_spi_open", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(static_cast<int64_t>(1)); }
    )));
    globalEnv->define("__builtin_spi_transfer", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(std::make_shared<std::vector<Value>>()); }
    )));
    globalEnv->define("__builtin_spi_close", Value(std::make_shared<Value::FunctionType>(
        [](std::vector<Value>&, Interpreter&) -> Value { return Value(true); }
    )));
}

// Execute statements
void Interpreter::execute(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
}

// Helper to evaluate expression
Value Interpreter::evaluate(Expression* expr) {
    expr->accept(*this);
    return lastValue;
}

// Call a function
Value Interpreter::callFunction(const std::string& name, std::vector<Value>& args) {
    // Check for user-defined function
    auto it = userFunctions.find(name);
    if (it != userFunctions.end()) {
        const UserFunction& func = it->second;
        
        // Create new environment with closure
        auto funcEnv = std::make_shared<Environment>(func.closure);
        
        // Bind parameters
        for (size_t i = 0; i < func.parameters.size(); ++i) {
            if (i < args.size()) {
                funcEnv->define(func.parameters[i], args[i]);
            } else {
                funcEnv->define(func.parameters[i], Value());
            }
        }
        
        // Save current environment and switch
        auto prevEnv = currentEnv;
        currentEnv = funcEnv;
        
        Value result;
        try {
            func.body->accept(*this);
        } catch (const ReturnException& ret) {
            // Handle return value
            if (ret.hasValue) {
                if (ret.isArray) {
                    result = Value(std::static_pointer_cast<Value::ArrayType>(ret.complexValue));
                } else if (ret.isMap) {
                    result = Value(std::static_pointer_cast<Value::MapType>(ret.complexValue));
                } else if (std::holds_alternative<int64_t>(ret.primitiveValue)) {
                    result = Value(std::get<int64_t>(ret.primitiveValue));
                } else if (std::holds_alternative<double>(ret.primitiveValue)) {
                    result = Value(std::get<double>(ret.primitiveValue));
                } else if (std::holds_alternative<std::string>(ret.primitiveValue)) {
                    result = Value(std::get<std::string>(ret.primitiveValue));
                } else if (std::holds_alternative<bool>(ret.primitiveValue)) {
                    result = Value(std::get<bool>(ret.primitiveValue));
                }
            }
        }
        
        // Restore environment
        currentEnv = prevEnv;
        return result;
    }
    
    // Check for built-in function
    if (globalEnv->exists(name)) {
        Value funcVal = globalEnv->get(name);
        if (funcVal.isFunction()) {
            return (*funcVal.asFunction())(args, *this);
        }
    }
    
    throw std::runtime_error("Undefined function: " + name);
}

// Visitor implementations
void Interpreter::visit(IntegerLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(FloatLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(StringLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(BooleanLiteral* node) {
    lastValue = Value(node->value);
}

void Interpreter::visit(NullLiteral* node) {
    lastValue = Value();  // monostate = null
}

void Interpreter::visit(Identifier* node) {
    lastValue = currentEnv->get(node->name);
}

void Interpreter::visit(BinaryExpression* node) {
    Value left = evaluate(node->left.get());
    Value right = evaluate(node->right.get());
    const std::string& op = node->op;
    
    // Arithmetic operations
    if (op == "+") {
        if (left.isString() || right.isString()) {
            lastValue = Value(left.toString() + right.toString());
        } else if (left.isFloat() || right.isFloat()) {
            lastValue = Value(left.asFloat() + right.asFloat());
        } else {
            lastValue = Value(left.asInt() + right.asInt());
        }
    } else if (op == "-") {
        if (left.isFloat() || right.isFloat()) {
            lastValue = Value(left.asFloat() - right.asFloat());
        } else {
            lastValue = Value(left.asInt() - right.asInt());
        }
    } else if (op == "*") {
        if (left.isFloat() || right.isFloat()) {
            lastValue = Value(left.asFloat() * right.asFloat());
        } else {
            lastValue = Value(left.asInt() * right.asInt());
        }
    } else if (op == "/") {
        if (right.asFloat() == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        lastValue = Value(left.asFloat() / right.asFloat());
    } else if (op == "%") {
        lastValue = Value(left.asInt() % right.asInt());
    }
    // Comparison operations
    else if (op == "==") {
        if (left.isString() && right.isString()) {
            lastValue = Value(left.asString() == right.asString());
        } else if (left.isNumber() && right.isNumber()) {
            lastValue = Value(left.asFloat() == right.asFloat());
        } else if (left.isBool() && right.isBool()) {
            lastValue = Value(left.asBool() == right.asBool());
        } else {
            lastValue = Value(false);
        }
    } else if (op == "!=") {
        if (left.isString() && right.isString()) {
            lastValue = Value(left.asString() != right.asString());
        } else if (left.isNumber() && right.isNumber()) {
            lastValue = Value(left.asFloat() != right.asFloat());
        } else {
            lastValue = Value(true);
        }
    } else if (op == "<") {
        lastValue = Value(left.asFloat() < right.asFloat());
    } else if (op == ">") {
        lastValue = Value(left.asFloat() > right.asFloat());
    } else if (op == "<=") {
        lastValue = Value(left.asFloat() <= right.asFloat());
    } else if (op == ">=") {
        lastValue = Value(left.asFloat() >= right.asFloat());
    }
    // Logical operations
    else if (op == "&&" || op == "and") {
        lastValue = Value(left.isTruthy() && right.isTruthy());
    } else if (op == "||" || op == "or") {
        lastValue = Value(left.isTruthy() || right.isTruthy());
    }
    else {
        throw std::runtime_error("Unknown binary operator: " + op);
    }
}

void Interpreter::visit(UnaryExpression* node) {
    Value operand = evaluate(node->operand.get());
    const std::string& op = node->op;
    
    if (op == "-") {
        if (operand.isFloat()) {
            lastValue = Value(-operand.asFloat());
        } else {
            lastValue = Value(-operand.asInt());
        }
    } else if (op == "!" || op == "not") {
        lastValue = Value(!operand.isTruthy());
    } else {
        throw std::runtime_error("Unknown unary operator: " + op);
    }
}

void Interpreter::visit(AssignmentExpression* node) {
    Value value = evaluate(node->right.get());
    
    if (auto* id = dynamic_cast<Identifier*>(node->left.get())) {
        currentEnv->set(id->name, value);
        lastValue = value;
    } else {
        throw std::runtime_error("Invalid assignment target");
    }
}

void Interpreter::visit(CallExpression* node) {
    std::vector<Value> args;
    for (auto& arg : node->arguments) {
        args.push_back(evaluate(arg.get()));
    }
    lastValue = callFunction(node->callee, args);
}

void Interpreter::visit(ArrayLiteral* node) {
    auto arr = std::make_shared<Value::ArrayType>();
    for (auto& elem : node->elements) {
        arr->push_back(evaluate(elem.get()));
    }
    lastValue = Value(arr);
}

void Interpreter::visit(ArrayIndexExpression* node) {
    Value arr = evaluate(node->array.get());
    Value idx = evaluate(node->index.get());
    
    if (!arr.isArray()) {
        throw std::runtime_error("Cannot index non-array");
    }
    if (!idx.isInt()) {
        throw std::runtime_error("Array index must be integer");
    }
    
    size_t index = static_cast<size_t>(idx.asInt());
    if (index >= arr.asArray()->size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    
    lastValue = (*arr.asArray())[index];
}

void Interpreter::visit(ArrayAssignmentExpression* node) {
    Value arr = evaluate(node->array.get());
    Value idx = evaluate(node->index.get());
    Value val = evaluate(node->value.get());
    
    if (!arr.isArray()) {
        throw std::runtime_error("Cannot index non-array");
    }
    if (!idx.isInt()) {
        throw std::runtime_error("Array index must be integer");
    }
    
    size_t index = static_cast<size_t>(idx.asInt());
    if (index >= arr.asArray()->size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    
    (*arr.asArray())[index] = val;
    lastValue = val;
}

void Interpreter::visit(VariableDeclaration* node) {
    Value value;
    if (node->initializer) {
        value = evaluate(node->initializer.get());
    }
    currentEnv->define(node->name, value);
    
    // Track if this is a const variable
    if (node->isConst) {
        constVariables[node->name] = true;
    }
}

void Interpreter::visit(ExpressionStatement* node) {
    evaluate(node->expression.get());
}

void Interpreter::visit(BlockStatement* node) {
    auto blockEnv = std::make_shared<Environment>(currentEnv);
    auto prevEnv = currentEnv;
    currentEnv = blockEnv;
    
    for (auto& stmt : node->statements) {
        stmt->accept(*this);
    }
    
    currentEnv = prevEnv;
}

void Interpreter::visit(IfStatement* node) {
    Value condition = evaluate(node->condition.get());
    
    if (condition.isTruthy()) {
        node->thenBranch->accept(*this);
    } else if (node->elseBranch) {
        node->elseBranch->accept(*this);
    }
}

void Interpreter::visit(WhileStatement* node) {
    while (evaluate(node->condition.get()).isTruthy()) {
        try {
            node->body->accept(*this);
        } catch (const BreakException&) {
            break;
        } catch (const ContinueException&) {
            continue;
        }
    }
}

void Interpreter::visit(ForStatement* node) {
    auto forEnv = std::make_shared<Environment>(currentEnv);
    auto prevEnv = currentEnv;
    currentEnv = forEnv;
    
    if (node->initializer) {
        node->initializer->accept(*this);
    }
    
    while (!node->condition || evaluate(node->condition.get()).isTruthy()) {
        try {
            node->body->accept(*this);
        } catch (const BreakException&) {
            break;
        } catch (const ContinueException&) {
            // Continue to increment
        }
        
        if (node->increment) {
            evaluate(node->increment.get());
        }
    }
    
    currentEnv = prevEnv;
}

void Interpreter::visit(BreakStatement*) {
    throw BreakException();
}

void Interpreter::visit(ContinueStatement*) {
    throw ContinueException();
}

void Interpreter::visit(FunctionDeclaration* node) {
    UserFunction func;
    func.parameters = node->parameters;
    func.body = node->body.get();
    func.closure = currentEnv;
    
    userFunctions[node->name] = func;
}

void Interpreter::visit(ReturnStatement* node) {
    if (node->value) {
        Value val = evaluate(node->value.get());
        
        if (val.isInt()) {
            throw ReturnException(val.asInt());
        } else if (val.isFloat()) {
            throw ReturnException(val.asFloat());
        } else if (val.isString()) {
            throw ReturnException(val.asString());
        } else if (val.isBool()) {
            throw ReturnException(val.asBool());
        } else if (val.isArray()) {
            ReturnException ret;
            ret.setArray(val.asArray());
            throw ret;
        } else if (val.isMap()) {
            ReturnException ret;
            ret.setMap(val.asMap());
            throw ret;
        } else {
            throw ReturnException();
        }
    }
    throw ReturnException();
}

void Interpreter::visit(TryStatement* node) {
    try {
        // Execute try block
        if (node->tryBlock) {
            node->tryBlock->accept(*this);
        }
    } catch (const std::exception& e) {
        // Create catch environment with error variable
        auto catchEnv = std::make_shared<Environment>(currentEnv);
        catchEnv->define(node->errorVariable, Value(std::string(e.what())));
        
        auto prevEnv = currentEnv;
        currentEnv = catchEnv;
        
        // Execute catch block
        if (node->catchBlock) {
            node->catchBlock->accept(*this);
        }
        
        currentEnv = prevEnv;
    }
}

void Interpreter::visit(LambdaExpression* node) {
    // Create a callable value for the lambda
    // For now, store as a string representation
    (void)node;
    lastValue = Value(std::string("<lambda>"));
}

void Interpreter::visit(MatchExpression* node) {
    Value subject = evaluate(node->subject.get());
    
    for (auto& matchCase : node->cases) {
        // nullptr pattern means default case
        if (!matchCase.pattern) {
            lastValue = evaluate(matchCase.result.get());
            return;
        }
        
        Value pattern = evaluate(matchCase.pattern.get());
        
        // Simple equality check
        bool matches = false;
        if (subject.isInt() && pattern.isInt()) {
            matches = subject.asInt() == pattern.asInt();
        } else if (subject.isString() && pattern.isString()) {
            matches = subject.asString() == pattern.asString();
        } else if (subject.isBool() && pattern.isBool()) {
            matches = subject.asBool() == pattern.asBool();
        }
        
        if (matches) {
            lastValue = evaluate(matchCase.result.get());
            return;
        }
    }
    
    // No match found
    lastValue = Value();
}

void Interpreter::visit(CompoundAssignment* node) {
    auto* id = dynamic_cast<Identifier*>(node->target.get());
    if (!id) {
        throw std::runtime_error("Compound assignment target must be an identifier");
    }
    
    Value current = currentEnv->get(id->name);
    Value value = evaluate(node->value.get());
    Value result;
    
    if (node->op == "+=") {
        if (current.isString() || value.isString()) {
            result = Value(current.toString() + value.toString());
        } else if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() + value.asInt());
        } else {
            result = Value(current.asFloat() + value.asFloat());
        }
    } else if (node->op == "-=") {
        if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() - value.asInt());
        } else {
            result = Value(current.asFloat() - value.asFloat());
        }
    } else if (node->op == "*=") {
        if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() * value.asInt());
        } else {
            result = Value(current.asFloat() * value.asFloat());
        }
    } else if (node->op == "/=") {
        if (current.isInt() && value.isInt()) {
            result = Value(current.asInt() / value.asInt());
        } else {
            result = Value(current.asFloat() / value.asFloat());
        }
    }
    
    currentEnv->set(id->name, result);
    lastValue = result;
}

void Interpreter::visit(UpdateExpression* node) {
    auto* id = dynamic_cast<Identifier*>(node->operand.get());
    if (!id) {
        throw std::runtime_error("Update expression operand must be an identifier");
    }
    
    Value current = currentEnv->get(id->name);
    Value result;
    
    if (node->op == "++") {
        if (current.isInt()) {
            result = Value(current.asInt() + 1);
        } else {
            result = Value(current.asFloat() + 1.0);
        }
    } else if (node->op == "--") {
        if (current.isInt()) {
            result = Value(current.asInt() - 1);
        } else {
            result = Value(current.asFloat() - 1.0);
        }
    }
    
    currentEnv->set(id->name, result);
    
    // For prefix, return new value; for postfix, return old value
    lastValue = node->prefix ? result : current;
}

void Interpreter::visit(InterpolatedString* node) {
    std::string result;
    
    for (auto& part : node->parts) {
        if (part.isExpression) {
            Value val = evaluate(part.expr.get());
            result += val.toString();
        } else {
            result += part.text;
        }
    }
    
    lastValue = Value(result);
}

// ========================================
// SADK (Agent Development Kit) Visitors
// ========================================

void Interpreter::visit(MapLiteral* node) {
    auto map = std::make_shared<Value::MapType>();
    
    for (auto& entry : node->entries) {
        // Evaluate key (should be a string or identifier)
        Value keyVal = evaluate(entry.first.get());
        std::string key;
        
        if (keyVal.isString()) {
            key = keyVal.asString();
        } else {
            // For identifiers used as keys, use their name
            if (auto* ident = dynamic_cast<Identifier*>(entry.first.get())) {
                key = ident->name;
            } else {
                key = keyVal.toString();
            }
        }
        
        // Evaluate value
        Value value = evaluate(entry.second.get());
        
        (*map)[key] = value;
    }
    
    lastValue = Value(map);
}

void Interpreter::visit(MemberExpression* node) {
    Value obj = evaluate(node->object.get());
    
    if (obj.isMap()) {
        auto map = obj.asMap();
        auto it = map->find(node->member);
        if (it != map->end()) {
            lastValue = it->second;
        } else {
            throw std::runtime_error("Map does not have member: " + node->member);
        }
    } else if (obj.isArray()) {
        // Array built-in properties
        if (node->member == "length") {
            lastValue = Value(static_cast<int64_t>(obj.asArray()->size()));
        } else {
            throw std::runtime_error("Array does not have member: " + node->member);
        }
    } else if (obj.isString()) {
        // String built-in properties
        if (node->member == "length") {
            lastValue = Value(static_cast<int64_t>(obj.asString().length()));
        } else {
            throw std::runtime_error("String does not have member: " + node->member);
        }
    } else {
        throw std::runtime_error("Cannot access member of non-object type");
    }
}

void Interpreter::visit(SelfExpression* node) {
    (void)node;  // Unused parameter
    // 'self' should be defined in the current environment when inside a method
    if (currentEnv->exists("self")) {
        lastValue = currentEnv->get("self");
    } else {
        throw std::runtime_error("'self' is not defined in current context");
    }
}

void Interpreter::visit(ImportStatement* node) {
    std::string moduleName = node->moduleName;
    std::string modulePath = node->modulePath;
    
    // Default path resolution for stdlib
    if (modulePath.empty()) {
        modulePath = "stdlib/" + moduleName + ".sf";
    }
    
    // Read module file
    std::ifstream file(modulePath);
    if (!file.is_open()) {
        // Try without stdlib/ prefix if it was implicit
        file.open(moduleName + ".sf");
        if (!file.is_open()) {
            throw std::runtime_error("Could not load module: " + moduleName + " (tried " + modulePath + ")");
        }
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    // Parse the module
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    auto statements = parser.parse();
    
    // Execute module in its own environment
    auto moduleEnv = std::make_shared<Environment>(globalEnv);
    auto oldEnv = currentEnv;
    currentEnv = moduleEnv;
    
    try {
        for (const auto& stmt : statements) {
            stmt->accept(*this);
        }
    } catch (...) {
        currentEnv = oldEnv;
        throw;
    }
    
    currentEnv = oldEnv;
    
    // Export symbols to a map
    auto moduleMap = std::make_shared<Value::MapType>();
    // For now, we export ALL variables from the module environment
    // In a real system, we'd only export symbols marked with 'export'
    for (const auto& [name, value] : moduleEnv->getVariables()) {
        (*moduleMap)[name] = value;
    }
    
    // Define the module object in the current environment
    std::string alias = node->alias.empty() ? moduleName : node->alias;
    currentEnv->define(alias, Value(moduleMap));
}

void Interpreter::visit(StructDeclaration* node) {
    // Store struct definition in a special registry
    // For now, we'll create a constructor function for the struct
    
    std::string structName = node->name;
    std::vector<std::string> fieldNames;
    
    for (const auto& field : node->fields) {
        fieldNames.push_back(field.name);
    }
    
    // Create a constructor function that creates map instances
    auto constructor = std::make_shared<Value::FunctionType>(
        [structName, fieldNames](std::vector<Value>& args, Interpreter& interp) -> Value {
            (void)interp;  // Unused
            auto instance = std::make_shared<Value::MapType>();
            
            // Set field values from arguments
            for (size_t i = 0; i < fieldNames.size() && i < args.size(); ++i) {
                (*instance)[fieldNames[i]] = args[i];
            }
            
            // Add a __type__ field to identify the struct type
            (*instance)["__type__"] = Value(structName);
            
            return Value(instance);
        }
    );
    
    // Define the constructor in global environment
    globalEnv->define(structName, Value(constructor));
}
