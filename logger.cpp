#include "logger.h"
#include <stdexcept>
#include <iostream>

Logger::Logger(const std::string& filename, LogLevel defaultLevel) 
    : filename(filename), currentLevel(defaultLevel) {

    logFile.open(filename, std::ios_base::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Can't open logfile!");
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLevel(LogLevel level) {
    currentLevel.store(level);
}

LogLevel Logger::getLevel() const {
    return currentLevel.load();
}

std::string Logger::getLevelString() const {
    return levelToString(currentLevel.load());
}

std::string Logger::levelToString(LogLevel level) const {
    std::string levelStr;
    switch (level) {
        case LogLevel::Info:    levelStr = "INFO";    break;
        case LogLevel::Warning: levelStr = "WARNING"; break;
        case LogLevel::Error:   levelStr = "ERROR";   break;
        default: return "UNKNOWN LEVEL";
    }
    return levelStr;
}

void Logger::WriteLogMessage(const std::string message, LogLevel level) {
    LogLevel current = currentLevel.load(std::memory_order_acquire);
    
    if (static_cast<int>(level) < static_cast<int>(current)) 
        return;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream stime;
    stime << std::put_time(std::localtime(&time), "%Y-%m-%d %X");

    std::string logString = "[" + stime.str() + "] [" + levelToString(level) + "] " + message + "\n";

    std::lock_guard<std::mutex> lock(mtx);
    if (logFile.is_open()) {
        logFile << logString;
        logFile.flush();
    } 
    
    else {
        throw std::runtime_error("Log file is not open");
    }
}