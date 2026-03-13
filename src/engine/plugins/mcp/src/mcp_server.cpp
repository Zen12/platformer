#include "mcp_server.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <future>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <nlohmann/json.hpp>

#include "core/src/asset/asset_manager.hpp"
#include "core/src/fsm/fsm_controller.hpp"
#include "core/src/resource/resource_cache.hpp"

using json = nlohmann::json;

McpServer::McpServer(uint16_t port,
                     std::shared_ptr<AssetManager> assetManager,
                     std::shared_ptr<FsmController> fsmController,
                     std::shared_ptr<ResourceCache> resourceCache)
    : _port(port),
      _assetManager(std::move(assetManager)),
      _fsmController(std::move(fsmController)),
      _resourceCache(std::move(resourceCache))
{}

McpServer::~McpServer() {
    Stop();
}

void McpServer::Start() {
    if (_running.load()) return;
    _running.store(true);
    _listenThread = std::thread([this]() { ListenLoop(); });
}

void McpServer::Stop() {
    _running.store(false);

    if (_serverFd >= 0) {
        ::shutdown(_serverFd, SHUT_RDWR);
        ::close(_serverFd);
        _serverFd = -1;
    }

    if (_listenThread.joinable()) {
        _listenThread.join();
    }
}

void McpServer::DrainCommands() {
    _commandQueue.Drain();
}

void McpServer::ListenLoop() {
    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0) {
        std::cerr << "[MCP] Failed to create socket: " << strerror(errno) << std::endl;
        return;
    }

    int opt = 1;
    ::setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(_port);

    if (::bind(_serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[MCP] Failed to bind on port " << _port << ": " << strerror(errno) << std::endl;
        ::close(_serverFd);
        _serverFd = -1;
        return;
    }

    if (::listen(_serverFd, 4) < 0) {
        std::cerr << "[MCP] Failed to listen: " << strerror(errno) << std::endl;
        ::close(_serverFd);
        _serverFd = -1;
        return;
    }

    std::cout << "[MCP] Server listening on 127.0.0.1:" << _port << std::endl;

    while (_running.load()) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = ::accept(_serverFd, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientFd < 0) {
            if (_running.load()) {
                std::cerr << "[MCP] Accept error: " << strerror(errno) << std::endl;
            }
            continue;
        }
        HandleClient(clientFd);
        ::close(clientFd);
    }
}

std::string McpServer::ReadHttpBody(int fd) {
    std::string headerBuf;
    int contentLength = 0;

    char c;
    while (true) {
        ssize_t n = ::recv(fd, &c, 1, 0);
        if (n <= 0) return "";
        headerBuf += c;
        if (headerBuf.size() >= 4 &&
            headerBuf.substr(headerBuf.size() - 4) == "\r\n\r\n") {
            break;
        }
        if (headerBuf.size() > 8192) return "";
    }

    std::string lowerHeaders = headerBuf;
    for (auto& ch : lowerHeaders) ch = static_cast<char>(std::tolower(ch));
    auto pos = lowerHeaders.find("content-length:");
    if (pos != std::string::npos) {
        auto start = pos + 15;
        while (start < lowerHeaders.size() && lowerHeaders[start] == ' ') start++;
        auto end = lowerHeaders.find("\r\n", start);
        contentLength = std::stoi(lowerHeaders.substr(start, end - start));
    }

    if (contentLength <= 0) return "";

    std::string body(contentLength, '\0');
    size_t totalRead = 0;
    while (totalRead < static_cast<size_t>(contentLength)) {
        ssize_t n = ::recv(fd, &body[totalRead], contentLength - totalRead, 0);
        if (n <= 0) return "";
        totalRead += n;
    }

    return body;
}

void McpServer::SendHttpResponse(int fd, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: application/json\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n"
             << body;

    std::string responseStr = response.str();
    size_t totalSent = 0;
    while (totalSent < responseStr.size()) {
        ssize_t n = ::send(fd, responseStr.data() + totalSent, responseStr.size() - totalSent, 0);
        if (n <= 0) return;
        totalSent += n;
    }
}

void McpServer::HandleClient(int clientFd) {
    std::string body = ReadHttpBody(clientFd);
    if (body.empty()) return;

    std::string response = HandleJsonRpc(body);
    if (!response.empty()) {
        SendHttpResponse(clientFd, response);
    }
}

std::string McpServer::HandleJsonRpc(const std::string& body) {
    try {
        auto request = json::parse(body);

        std::string method = request.value("method", "");
        std::string id;
        if (request.contains("id")) {
            if (request["id"].is_string()) {
                id = request["id"].get<std::string>();
            } else {
                id = std::to_string(request["id"].get<int>());
            }
        }

        if (method == "initialize") {
            return HandleInitialize(id);
        }
        if (method == "notifications/initialized") {
            return "";
        }
        if (method == "tools/list") {
            return HandleToolsList(id);
        }
        if (method == "tools/call") {
            std::string toolName;
            if (request.contains("params") && request["params"].contains("name")) {
                toolName = request["params"]["name"].get<std::string>();
            }
            return HandleToolsCall(id, toolName);
        }

        json error;
        error["jsonrpc"] = "2.0";
        error["id"] = id;
        error["error"]["code"] = -32601;
        error["error"]["message"] = "Method not found: " + method;
        return error.dump();

    } catch (const std::exception& e) {
        json error;
        error["jsonrpc"] = "2.0";
        error["id"] = nullptr;
        error["error"]["code"] = -32700;
        error["error"]["message"] = std::string("Parse error: ") + e.what();
        return error.dump();
    }
}

std::string McpServer::HandleInitialize(const std::string& id) {
    json result;
    result["jsonrpc"] = "2.0";
    result["id"] = id;
    result["result"]["protocolVersion"] = "2024-11-05";
    result["result"]["capabilities"]["tools"]["listChanged"] = false;
    result["result"]["serverInfo"]["name"] = "game-engine-mcp";
    result["result"]["serverInfo"]["version"] = "0.1.0";
    return result.dump();
}

std::string McpServer::HandleToolsList(const std::string& id) {
    json tool;
    tool["name"] = "restart_fsm_node";
    tool["description"] = "Restart the current FSM node: clears resource cache, re-scans asset metadata, and re-enters the current state. Use after modifying assets on disk.";
    tool["inputSchema"]["type"] = "object";
    tool["inputSchema"]["properties"] = json::object();
    tool["inputSchema"]["required"] = json::array();

    json result;
    result["jsonrpc"] = "2.0";
    result["id"] = id;
    result["result"]["tools"] = json::array({tool});
    return result.dump();
}

std::string McpServer::HandleToolsCall(const std::string& id, const std::string& toolName) {
    if (toolName == "restart_fsm_node") {
        auto promise = std::make_shared<std::promise<std::string>>();
        auto future = promise->get_future();

        _commandQueue.Push([this, promise]() {
            try {
                _resourceCache->Clear();

                _assetManager->UnLoadAll();
                _assetManager->Init();

                _fsmController->RestartCurrentNode();

                promise->set_value("FSM node restarted. Resource cache cleared and assets re-scanned.");
            } catch (const std::exception& e) {
                promise->set_value(std::string("Error: ") + e.what());
            }
        });

        auto status = future.wait_for(std::chrono::seconds(5));
        std::string message;
        if (status == std::future_status::ready) {
            message = future.get();
        } else {
            message = "Timeout: command was queued but main thread did not execute within 5 seconds.";
        }

        json content;
        content["type"] = "text";
        content["text"] = message;

        json result;
        result["jsonrpc"] = "2.0";
        result["id"] = id;
        result["result"]["content"] = json::array({content});
        result["result"]["isError"] = false;
        return result.dump();
    }

    json content;
    content["type"] = "text";
    content["text"] = "Unknown tool: " + toolName;

    json result;
    result["jsonrpc"] = "2.0";
    result["id"] = id;
    result["result"]["content"] = json::array({content});
    result["result"]["isError"] = true;
    return result.dump();
}
