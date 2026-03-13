#pragma once

#include <deque>
#include <functional>
#include <mutex>

class CommandQueue final {
private:
    std::deque<std::function<void()>> _commands;
    std::mutex _mutex;

public:
    void Push(std::function<void()> command) {
        std::lock_guard<std::mutex> lock(_mutex);
        _commands.push_back(std::move(command));
    }

    void Drain() {
        std::deque<std::function<void()>> batch;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            batch.swap(_commands);
        }
        for (auto& cmd : batch) {
            cmd();
        }
    }
};
