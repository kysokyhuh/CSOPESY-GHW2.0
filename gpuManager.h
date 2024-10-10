#ifndef GPU_MANAGER_H
#define GPU_MANAGER_H

#include <ncurses.h>
#include <vector>
#include <string>

// Struct to hold process info
struct ProcessInfo {
    int gpu;
    std::string gi_id;
    std::string ci_id;
    int pid;
    std::string type;
    std::string processName;
    std::string gpuMemoryUsage;

    // Constructor to allow initialization with values
    ProcessInfo(int g, std::string gi, std::string ci, int p, std::string t, std::string pn, std::string gmu)
        : gpu(g), gi_id(gi), ci_id(ci), pid(p), type(t), processName(pn), gpuMemoryUsage(gmu) {}
};

// Function declarations for GPU display
void displayMessage(const std::string& header1, const std::string& header2, const std::string& header3, const std::string& userPath);
void showGPUInfo();
void showProcesses(const std::vector<ProcessInfo>& processes);
void handleCommandInput(const std::vector<ProcessInfo>& processes);

#endif // GPU_MANAGER_H
