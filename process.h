#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <chrono>

class Process {
public:
    Process(int id, int instructionLength);  // Constructor with instruction length
    std::string getProcessName() const;      // Returns the process name
    int getCoreId() const;                   // Returns the core ID
    int getProgress() const;                 // Returns current progress
    int getTotalWork() const;                // Returns total work
    bool isFinished() const;                 // Checks if process is finished
    void incrementProgress();                // Simulates executing an instruction

    // Time-related functions
    void setStartTime(const std::chrono::system_clock::time_point& startTime);
    void setEndTime(const std::chrono::system_clock::time_point& endTime);
    std::chrono::system_clock::time_point getStartTime() const;
    std::chrono::system_clock::time_point getEndTime() const;

    std::string processName;                 // Public for easy access in this example

private:
    int id;
    int coreId;
    int progress;
    int totalWork;
    int instructionLength;
    std::chrono::system_clock::time_point startTime;  // Start time of the process
    std::chrono::system_clock::time_point endTime;    // End time of the process
};

#endif // PROCESS_H
