#pragma once

#include <string>

#ifndef __EMSCRIPTEN__

#include <sys/types.h>

class PtyProcess {
public:
    PtyProcess() = default;
    ~PtyProcess();

    PtyProcess(const PtyProcess&) = delete;
    PtyProcess& operator=(const PtyProcess&) = delete;

    bool Spawn(const std::string& shell = "", const std::string& workingDir = "");
    int Read(char* buf, int maxLen);
    void Write(const char* buf, int len);
    void Write(const std::string& str);
    void Resize(int cols, int rows);
    [[nodiscard]] bool IsAlive() const;

private:
    int _masterFd = -1;
    pid_t _childPid = -1;
};

#endif
