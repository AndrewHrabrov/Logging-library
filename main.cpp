#include "app.h"
#include <iostream>
#include <algorithm>

std::unique_ptr<LogApp> app;

std::string toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

LogLevel parseLogLevel(const std::string& levelStr) {
    std::string upperLevel = toUpperCase(levelStr);

    if (upperLevel == "INFO") return LogLevel::Info;
    if (upperLevel == "WARNING") return LogLevel::Warning;
    if (upperLevel == "ERROR") return LogLevel::Error;
    throw std::invalid_argument("Unknown log level: '" + levelStr + "'. Available levels: INFO, WARNING, ERROR");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <logfile> <DEFAULT_LEVEL>" << std::endl;
        std::cerr << "Available levels: INFO, WARNING, ERROR" << std::endl;
        std::cerr << "Default level is Info" << std::endl;
        return 1;
    }

    try {
        LogLevel defaultLevel = LogLevel::Info;
        if (argc == 3) {
            defaultLevel = parseLogLevel(argv[2]);
        }

        app = std::make_unique<LogApp>(argv[1], defaultLevel);
        app->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}