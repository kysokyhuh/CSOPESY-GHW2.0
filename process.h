#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <mutex>

class Process {
public:
    int id;  // Process ID
    std::string processName;  // Name of the process (e.g., "screen_01")
    std::queue<std::string> printCommands;
    std::string logFileName;  // Log file name for storing process log

    static std::mutex logMutex;  // Mutex for thread-safe logging

    // Constructor to initialize the process with a unique ID
    Process(int processId);

    // Function to log a print command executed on a specific core
    void logCommand(const std::string& command, int coreId);

    // Function to get the current timestamp
    std::string getCurrentTime() const;

    // Getter function for process name
    std::string getProcessName() const { return processName; }  // This is the missing function
};

#endif // PROCESS_H
