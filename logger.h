#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <atomic>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    Info,
    Warning,
    Error
};

class Logger {
public:
    Logger(const std::string& filename, LogLevel defaultLevel);
    ~Logger();
    void setLevel(LogLevel level); // Функция смены уровня важности по умолчанию
    LogLevel getLevel() const; // Функция получения уровня важности по умолчанию
    std::string getLevelString() const; // Функция получения строки с уровнем важности
    void WriteLogMessage(const std::string message, LogLevel level); // Функция записи сообщения в лог файл

private:
    std::string filename;
    std::ofstream logFile;
    std::atomic<LogLevel> currentLevel;
    std::mutex mtx;
    std::string levelToString(LogLevel level) const;
};

#endif