#ifndef __EMSCRIPTEN__

#include "pty_process.hpp"

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#ifdef __APPLE__
#include <util.h>
#else
#include <pty.h>
#endif

PtyProcess::~PtyProcess() {
    if (_childPid > 0) {
        kill(_childPid, SIGKILL);
        waitpid(_childPid, nullptr, 0);
        _childPid = -1;
    }
    if (_masterFd >= 0) {
        close(_masterFd);
        _masterFd = -1;
    }
}

bool PtyProcess::Spawn(const std::string& shell, const std::string& workingDir) {
    std::string shellPath = shell;
    if (shellPath.empty()) {
        const char* envShell = std::getenv("SHELL");
        shellPath = envShell ? envShell : "/bin/bash";
    }

    struct winsize ws{};
    ws.ws_col = 80;
    ws.ws_row = 24;

    _childPid = forkpty(&_masterFd, nullptr, nullptr, &ws);

    if (_childPid < 0) {
        return false;
    }

    if (_childPid == 0) {
        setenv("TERM", "xterm-256color", 1);
        if (!workingDir.empty()) {
            setenv("PROJECT_ROOT", workingDir.c_str(), 1);
            chdir(workingDir.c_str());
        }
        execl(shellPath.c_str(), shellPath.c_str(), nullptr);
        _exit(1);
    }

    int flags = fcntl(_masterFd, F_GETFL, 0);
    fcntl(_masterFd, F_SETFL, flags | O_NONBLOCK);

    return true;
}

int PtyProcess::Read(char* buf, int maxLen) {
    if (_masterFd < 0) return 0;
    ssize_t n = read(_masterFd, buf, static_cast<size_t>(maxLen));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }
    return static_cast<int>(n);
}

void PtyProcess::Write(const char* buf, int len) {
    if (_masterFd < 0) return;
    write(_masterFd, buf, static_cast<size_t>(len));
}

void PtyProcess::Write(const std::string& str) {
    Write(str.c_str(), static_cast<int>(str.size()));
}

void PtyProcess::Resize(int cols, int rows) {
    if (_masterFd < 0) return;
    struct winsize ws{};
    ws.ws_col = static_cast<unsigned short>(cols);
    ws.ws_row = static_cast<unsigned short>(rows);
    ioctl(_masterFd, TIOCSWINSZ, &ws);
}

bool PtyProcess::IsAlive() const {
    if (_childPid <= 0) return false;
    int status = 0;
    pid_t result = waitpid(_childPid, &status, WNOHANG);
    return result == 0;
}

#endif
