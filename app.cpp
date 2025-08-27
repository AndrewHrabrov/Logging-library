#include "app.h"
#include <iostream>
#include <sstream>

LogApp::LogApp(const std::string& filename, LogLevel defaultLevel) : running(true) {
    logger = std::make_unique<Logger>(filename, defaultLevel);
    workerThread = std::thread(&LogApp::processLogs, this);
}

LogApp::~LogApp() {
    stop();
}

void LogApp::stop() {
    running = false;
    cv.notify_all();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void LogApp::addLogEntry(const std::string& message, LogLevel level) {
    std::lock_guard<std::mutex> lock(queueMutex);
    logQueue.push({message, level});
    cv.notify_one();
}

void LogApp::processLogs() {
    while (running || !logQueue.empty()) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this]() { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            auto entry = logQueue.front();
            logQueue.pop();
            lock.unlock();

            try {
                logger->WriteLogMessage(entry.message, entry.level);
            } catch(const std::exception& e) {
                std::cerr << "Logging error: " << e.what() << std::endl;
            }

            lock.lock();
        }
    }
}

void LogApp::run() {
    std::string input;
    std::cout << "Enter log message" << std::endl;
    std::cout << "Current default level is " << logger->getLevelString() << std::endl;
    std::cout << "If you want change level, enter: 'setlevel Level' or 'message Level'" << std::endl;
    std::cout << "Type 'exit' to quit" << std::endl;

    while (running) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "exit") {
            running = false;
            break;
        }

        if (input.find("setlevel ") == 0) {
            handleSetLevel(input);
        } else {
            handleLogMessage(input);
        }
    }
}

void LogApp::handleSetLevel(const std::string& input) {
    std::istringstream iss(input.substr(9));
    std::string levelStr;
    iss >> levelStr;

    LogLevel newLevel;
    if (levelStr == "INFO") newLevel = LogLevel::Info;
    else if (levelStr == "WARNING") newLevel = LogLevel::Warning;
    else if (levelStr == "ERROR") newLevel = LogLevel::Error;
    else {
        std::cout << "Unknown level" << std::endl;
        return;
    }

    logger->setLevel(newLevel);
    std::cout << "Log level set to " << levelStr << std::endl;
}

void LogApp::handleLogMessage(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> words;
    std::string word;
    
    while (iss >> word) {
        words.push_back(word);
    }

    if (words.empty()) return;

    LogLevel level = logger->getLevel();
    std::string message;

    // Проверяем последнее слово на соответствие уровню логирования
    if (!words.empty()) {
        const std::string& lastWord = words.back();
        if (lastWord == "INFO") {
            level = LogLevel::Info;
            words.pop_back();
        } else if (lastWord == "WARNING") {
            level = LogLevel::Warning;
            words.pop_back();
        } else if (lastWord == "ERROR") {
            level = LogLevel::Error;
            words.pop_back();
        }
    }

    for (const auto& w : words) {
        if (!message.empty()) message += " ";
        message += w;
    }

    if (!message.empty()) {
        addLogEntry(message, level);
    }
}