#include "app.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <thread>
#include <chrono>

void testBasicFunctionality() {
    std::string testFile = "test_basic.log";
    
    // Удаляем предыдущий тестовый файл
    remove(testFile.c_str());
    
    // Тест 1: Создание логгера и запись сообщений
    {
        LogApp app(testFile, LogLevel::Info);
        app.addLogEntry("Test info message", LogLevel::Info);
        app.addLogEntry("Test warning message", LogLevel::Warning);
        app.addLogEntry("Test error message", LogLevel::Error);
        
        // Даем время для обработки сообщений
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        app.stop();
    }
    
    // Проверка содержимого файла
    std::ifstream file(testFile);
    std::string line;
    int lineCount = 0;
    
    while (std::getline(file, line)) {
        lineCount++;
        std::cout << "Line " << lineCount << ": " << line << std::endl;
        
        // Проверяем, что каждая строка содержит правильный формат
        assert(line.find("[") != std::string::npos && "Missing timestamp");
        assert(line.find("]") != std::string::npos && "Missing level");
    }
    
    std::cout << "Total lines: " << lineCount << std::endl;
    assert(lineCount >= 3 && "Should have at least 3 log entries");
}

void testLevelFiltering() {
    std::string testFile = "test_filter.log";
    remove(testFile.c_str());
    
    // Тест 2: Фильтрация по уровню 
    {
        LogApp app(testFile, LogLevel::Warning);
        app.addLogEntry("This info should not appear", LogLevel::Info);
        app.addLogEntry("This warning should appear", LogLevel::Warning);
        app.addLogEntry("This error should appear", LogLevel::Error);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        app.stop();
    }
    
    // Проверка содержимого файла
    std::ifstream file(testFile);
    std::string line;
    int lineCount = 0;
    
    while (std::getline(file, line)) {
        lineCount++;
        // INFO сообщения не должны появляться
        assert(line.find("INFO") == std::string::npos && "INFO message should be filtered");
    }
    
    std::cout << "Filter test lines: " << lineCount << std::endl;
    assert(lineCount == 2 && "Should have 2 log entries (warning and error)");
}

int main() {
    try {
        testBasicFunctionality();
        testLevelFiltering();
        std::cout << "All basic tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
