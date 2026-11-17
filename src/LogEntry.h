#pragma once
#include <string>

// One parsed log line.
struct LogEntry
{
    std::string timestamp;  // "YYYY-MM-DD HH:MM:SS" - comparable as a string
    std::string level;      // ERROR, WARNING, INFO, DEBUG
    std::string message;
    int lineNumber = 0;

    int hour() const { return timestamp.size() >= 13 ? std::stoi(timestamp.substr(11, 2)) : -1; }
};
