#include "app.h"
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <cassert>

void stressTest() {
    const int numThreads = 20;
    const int messagesPerThread = 1000;

    std::string testFile = "test_stress.log";
    remove(testFile.c_str()); // Удаляем предыдущий тестовый файл
    
    LogApp app(testFile, LogLevel::Info);
    std::vector<std::thread> threads;
    
    // Запускаем потоки
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&app, i]() {
            for (int j = 0; j < messagesPerThread; ++j) {
                std::string msg = "Stress test message " + std::to_string(j) + " from thread " + std::to_string(i);
                app.addLogEntry(msg, static_cast<LogLevel>(j % 3));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    app.stop();
    
    // Проверяем, что все сообщения были обработаны
    std::ifstream file("test_stress.log");
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    
    std::cout << "Stress test: " << lineCount << " messages logged" << std::endl;
    assert(lineCount == numThreads * messagesPerThread && "All messages should be logged");
}

int main() {
    stressTest();
    std::cout << "Stress test passed!" << std::endl;
    return 0;
}