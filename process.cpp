#include "process.h"

// Constructor
Process::Process(int id, int instructionLength) 
    : id(id), instructionLength(instructionLength), progress(0), totalWork(instructionLength) {}

// Getters for process name, core ID, progress, and total work
std::string Process::getProcessName() const { return processName; }
int Process::getCoreId() const { return coreId; }
int Process::getProgress() const { return progress; }
int Process::getTotalWork() const { return totalWork; }
bool Process::isFinished() const { return progress >= totalWork; }
void Process::incrementProgress() { if (progress < totalWork) ++progress; }

// Time-related methods
void Process::setStartTime(const std::chrono::system_clock::time_point& startTime) {
    this->startTime = startTime;
}

void Process::setEndTime(const std::chrono::system_clock::time_point& endTime) {
    this->endTime = endTime;
}

std::chrono::system_clock::time_point Process::getStartTime() const {
    return startTime;
}

std::chrono::system_clock::time_point Process::getEndTime() const {
    return endTime;
}
