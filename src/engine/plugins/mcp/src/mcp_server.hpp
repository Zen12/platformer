#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "command_queue.hpp"

class AssetManager;
class FsmController;
class ResourceCache;

class McpServer final {
private:
    uint16_t _port;
    std::atomic<bool> _running{false};
    std::thread _listenThread;
    int _serverFd = -1;

    CommandQueue _commandQueue;

    std::shared_ptr<AssetManager> _assetManager;
    std::shared_ptr<FsmController> _fsmController;
    std::shared_ptr<ResourceCache> _resourceCache;

    void ListenLoop();
    void HandleClient(int clientFd);

    std::string HandleJsonRpc(const std::string& body);
    std::string HandleInitialize(const std::string& id);
    std::string HandleToolsList(const std::string& id);
    std::string HandleToolsCall(const std::string& id, const std::string& toolName);

    static std::string ReadHttpBody(int fd);
    static void SendHttpResponse(int fd, const std::string& body);

public:
    McpServer(uint16_t port,
              std::shared_ptr<AssetManager> assetManager,
              std::shared_ptr<FsmController> fsmController,
              std::shared_ptr<ResourceCache> resourceCache);

    ~McpServer();

    void Start();
    void Stop();
    void DrainCommands();
};
