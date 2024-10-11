#include "process.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

std::mutex Process::logMutex;  // Define the static mutex

// Constructor for initializing the process
Process::Process(int processId) : id(processId) {
    processName = "screen_" + std::to_string(processId);  // Assign a process name
    logFileName = processName + ".txt";  // Create a unique log file for the process

    std::ofstream logFile(logFileName, std::ios::out);
    if (logFile.is_open()) {
        logFile << "Process name: " << processName << "\nLogs:\n\n";  // Header for each log
        logFile.close();
    }

    // Generate 100 print commands
    for (int i = 1; i <= 100; ++i) {
        printCommands.push("\"Hello world from " + processName + "!\"");
    }
}

// Get the current timestamp
std::string Process::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(localTime, "%m/%d/%Y %I:%M:%S %p");
    return oss.str();
}

// Log a command execution to a file in the given format
void Process::logCommand(const std::string& command, int coreId) {
    std::lock_guard<std::mutex> lock(logMutex);  // Ensure thread-safe access to the log file

    std::ofstream logFile(logFileName, std::ios::app);  // Open file in append mode
    if (logFile.is_open()) {
        logFile << "(" << getCurrentTime() << ") Core:" << coreId << " " << command << "\n";  // Log the command with core and timestamp
        logFile.close();
    }
}
