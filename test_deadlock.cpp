#include "app.h"
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <cassert>

std::atomic<bool> stopTest(false);

void levelChanger(LogApp* app, int threadId) {
    while (!stopTest) {
        // Чередуем уровни логирования
        static const LogLevel levels[] = {LogLevel::Info, LogLevel::Warning, LogLevel::Error};
        app->addLogEntry("Test message", levels[threadId % 3]);
    }
}

void testForDeadlocks() {
    const int numThreads = 10;
    
    std::string testFile = "test_deadlock.log";
    remove(testFile.c_str()); // Удаляем предыдущий тестовый файл

    LogApp app(testFile, LogLevel::Info);
    std::vector<std::thread> threads;
    
    // Запускаем потоки
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(levelChanger, &app, i);
    }
    
    // Даем поработать 5 секунд
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    stopTest = true;
    
    for (auto& t : threads) {
        t.join();
    }
    
    app.stop();
    
    std::cout << "Deadlock test completed without hanging!" << std::endl;
}

int main() {
    testForDeadlocks();
    return 0;
}