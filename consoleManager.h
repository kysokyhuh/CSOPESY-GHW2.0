#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>
#include <map>
#include <queue>
#include <mutex>
#include "process.h"

class consoleManager {
public:
    consoleManager(); 
    void displayMenu();  
    void displayHeader();  
    int validateCommand(const std::string& command);  
    int handleCommand(int command);  
    void createProcesses();  
    void handleSchedulerCommand();  
    void gpuCLI();  
    void screenCLI();  

private:
    void scheduler();  
    void cpuWorker(int coreId);  

    void createScreen(const std::string& name);  // Declare here
    void showScreen(const std::string& name);  // Declare here

    // Global variables for process scheduling
    std::queue<Process*> processQueue;
    std::mutex queueMutex;
    std::vector<Process*> finishedProcesses;  
    std::vector<Process*> runningProcesses;
    bool running = true;
};

#endif // CONSOLE_MANAGER_H
