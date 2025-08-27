#include "app.h"
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <fstream>
#include <sstream>
#include <cassert>

std::atomic<int> messagesLogged(0);

void concurrentWriter(LogApp* app, int threadId, int messagesPerThread) {
    for (int i = 0; i < messagesPerThread; ++i) {
        std::stringstream msg;
        msg << "Thread " << threadId << " message " << i;
        
        // Чередуем уровни логирования
        LogLevel level = static_cast<LogLevel>(i % 3);
        app->addLogEntry(msg.str(), level);
        messagesLogged++;
    }
}

void testConcurrentWriting() {
    const int numThreads = 10;
    const int messagesPerThread = 100;
    
    std::string testFile = "test_concurrent.log";
    remove(testFile.c_str()); // Удаляем предыдущий тестовый файл

    LogApp app(testFile, LogLevel::Info);
    std::vector<std::thread> threads;
    
    // Запускаем потоки
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(concurrentWriter, &app, i, messagesPerThread);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    app.stop();
    
    // Проверяем, что все сообщения были обработаны
    std::ifstream file("test_concurrent.log");
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    
    std::cout << "Expected: " << messagesLogged << ", Actual: " << lineCount << std::endl;
    assert(lineCount == messagesLogged && "All messages should be logged");
}

int main() {
    testConcurrentWriting();
    std::cout << "Concurrent test passed!" << std::endl;
    return 0;
}