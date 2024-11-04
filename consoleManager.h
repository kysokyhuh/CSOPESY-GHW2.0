#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <fstream>
#include "initializer.h"
#include "process.h"

class consoleManager {
public:
    consoleManager();
    ~consoleManager();

    void displayMenu();
    void displayHeader();
    int validateCommand(const std::string& command);
    int handleCommand(int command, const std::string& commandStr);
    void loadConfig();
    void generateProcessReport(const std::string& filename);
    void listProcesses(std::ostream& os);
    void spawnNewProcess();
    void startCustomProcess(const std::string& processName);
    void handleProcessSmi(const std::string& processName);

    // Scheduler commands
    void startSchedulerTest();
    void stopSchedulerTest();
    void reattachProcessScreen(const std::string& processName);
    void createDummyProcess();  

private:
    void processGenerator();
    void startProcessScreen(const std::string& processName);
    void workerThread();

    // Configuration parameters
    int numCPUs = 4;
    std::string schedulerType = "rr";
    int quantumCycles = 5;
    int batchProcessFreq = 1;
    int minInstructions = 1000;
    int maxInstructions = 2000;
    int delayPerExec = 0;

    // Process management variables
    Initializer initializer;
    std::vector<Process*> finishedProcesses;
    std::vector<Process*> runningProcesses;
    std::atomic<bool> stopScheduler;
    bool generatingProcesses = false;
    std::thread processGeneratorThread;
    int processGenerationFrequency = 1;
    int processCount = 1;
    std::mutex processMutex;

    // Thread pool and task queue for managing background tasks
    std::vector<std::thread> threadPool;
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
};

#endif // CONSOLE_MANAGER_H
