#pragma once
#ifndef APP_H
#define APP_H

#include "logger.h"
#include <thread>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <queue>
#include <mutex>
#include <string>

class LogApp {
public:
    LogApp(const std::string& filename, LogLevel defaultLevel);
    ~LogApp();
    void run();
    void stop();
    void addLogEntry(const std::string& message, LogLevel level); 

private:
    struct LogEntry {
        std::string message;
        LogLevel level;
    };

    std::unique_ptr<Logger> logger;
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread workerThread;

    void processLogs();
    void handleSetLevel(const std::string& input);
    void handleLogMessage(const std::string& input);
};

#endif