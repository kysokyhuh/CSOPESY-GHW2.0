#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <fstream>
#include "initializer.h"
#include "process.h"

class consoleManager {
public:
    consoleManager();
    void displayMenu();
    void displayHeader();
    int validateCommand(const std::string& command);
    int handleCommand(int command, const std::string& commandStr);
    void loadConfig();
    void generateProcessReport(const std::string& filename);
    void listProcesses(std::ostream& os);
    void spawnNewProcess();
    void startCustomProcess(const std::string& processName); // Declaration for custom process

private:
    // Other private methods and members
    void processGenerator();
    void startProcessScreen(const std::string& processName);
    void reattachProcessScreen(const std::string& processName);

    // Configuration parameters and member variables
    int numCPUs = 4;
    std::string schedulerType = "rr";
    int quantumCycles = 5;
    int batchProcessFreq = 1;
    int minInstructions = 1000;
    int maxInstructions = 2000;
    int delayPerExec = 0;

    Initializer initializer;
    std::queue<Process*> processQueue;
    std::mutex queueMutex;
    std::vector<Process*> finishedProcesses;
    std::vector<Process*> runningProcesses;
    bool generatingProcesses = false;
    std::thread processGeneratorThread;
    int processGenerationFrequency = 1;
    int processCount = 1;

    // Mutex to ensure thread safety when modifying process lists
    std::mutex processMutex;
};

#endif // CONSOLE_MANAGER_H
