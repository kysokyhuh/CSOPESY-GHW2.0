#include "consoleManager.h"
#include "gpuManager.h"
#include "process.h"
#include "marqueeManager.h"
#include "baseScreen.h"
#include "screenProcess.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <iomanip>
#include <chrono>
#include <ncurses.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h> 
#include <mutex>

std::mutex processMutex;

// Function to clear the console screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to get the current time as a string
std::string getCurrentTimeString() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%m/%d/%Y %I:%M:%S%p", ltm);
    return timestamp;
}

void consoleManager::displayMenu() {    
    std::cout << "\033[32mDeveloped by:\033[0m\n";
    std::cout << "Alyza Reynado \n";
    std::cout << "Kaye Suba\n";
    std::cout << "Jasper Adrada\n";
    std::cout << "\033[32mLast Updated:\033[0m October 29, 2024\n";
    std::cout << "------------------------------------------------------------\n";
}

void consoleManager::displayHeader() {
    std::cout << "\n";
    std::cout << "   ______   ______     ___   _______  ________   ______  ____  ____  \n";
    std::cout << " .' ___  |.' ____ \\  .'   `.|_   __ \\|_   __  |.' ____ \\|_  _||_  _| \n";
    std::cout << "/ .'   \\_|| (___ \\_|/  .-.  \\ | |__) | | |_ \\_|| (___ \\_| \\ \\  / /   \n";
    std::cout << "| |        _.____`. | |   | | |  ___/  |  _| _  _.____`.   \\ \\/ /    \n";
    std::cout << "\\ `.___.'\\| \\____) |\\  `-'  /_| |_    _| |__/ || \\____) |  _|  |_    \n";
    std::cout << " `.____ .' \\______.' `.___.'|_____|  |________| \\______.' |______|   \n";
    std::cout << "                                                                     \n";
    displayMenu();
}

int consoleManager::validateCommand(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    if (cmd == "INITIALIZE") return 1;
    else if (cmd == "SCHEDULER-TEST") return 3;
    else if (cmd == "SCHEDULER-STOP") return 4;
    else if (cmd == "REPORT-UTIL") return 5;
    else if (cmd == "SCREEN") return 6;
    else if (cmd == "SCREEN -LS") return 7;
    else if (cmd.rfind("SCREEN -S ", 0) == 0) return 8;  // Check for "SCREEN -S <process name>"
    else if (cmd.rfind("SCREEN -R ", 0) == 0) return 9;  // Check for "SCREEN -R <process name>"
    else if (cmd == "EXIT") return 10;
    return 11; // Invalid command
}

void consoleManager::loadConfig() {
    std::ifstream configFile("config.txt");
    if (configFile.is_open()) {
        std::string key;
        while (configFile >> key) {
            if (key == "num-cpu") configFile >> numCPUs;
            else if (key == "scheduler") configFile >> schedulerType;
            else if (key == "quantum-cycles") configFile >> quantumCycles;
            else if (key == "batch-process-freq") configFile >> batchProcessFreq;
            else if (key == "min-ins") configFile >> minInstructions;
            else if (key == "max-ins") configFile >> maxInstructions;
            else if (key == "delay-per-exec") configFile >> delayPerExec;
        }
        configFile.close();
        std::cout << "Config loaded successfully.\n";
    } else {
        std::cout << "Error: Could not open config.txt. Using default values.\n";
    }
}

void consoleManager::generateProcessReport(const std::string& filename) {
    std::ostringstream reportContent;
    reportContent << "root:> report-util\n";
    reportContent << "CPU utilization: 100%\n";
    reportContent << "Cores used: " << runningProcesses.size() << "\n";
    reportContent << "Cores available: " << (numCPUs - runningProcesses.size()) << "\n";
    reportContent << "------------------------------------------------------------\n\n";

    reportContent << "Running processes:\n";
    for (const auto& process : runningProcesses) {
        reportContent << process->getProcessName() << "  (" << getCurrentTimeString() << ")  ";
        reportContent << "Core: " << process->getCoreId() << "   ";
        reportContent << process->getProgress() << " / " << process->getTotalWork() << "\n";
    }

    reportContent << "\nFinished processes:\n";
    for (const auto& process : finishedProcesses) {
        reportContent << process->getProcessName() << "  (" << getCurrentTimeString() << ")  ";
        reportContent << "Finished   " << process->getTotalWork() << " / " << process->getTotalWork() << "\n";
    }
    reportContent << "------------------------------------------------------------\n";

    std::cout << reportContent.str();

    std::ofstream reportFile(filename, std::ios::app);
    if (reportFile.is_open()) {
        reportFile << reportContent.str();
        reportFile.close();
        std::cout << "root:> Report saved to " << filename << "\n";
    } else {
        std::cerr << "Error: Could not open or create file " << filename << ".\n";
    }
}

// Helper function to format time
std::string formatTime(const std::chrono::system_clock::time_point& timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* tm = std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(tm, "%m/%d/%Y %I:%M:%S%p");
    return oss.str();
}

void consoleManager::listProcesses(std::ostream& os) {
    std::lock_guard<std::mutex> lock(processMutex);

    os << "root:> screen -ls\n";
    os << "CPU utilization: " << (runningProcesses.empty() ? 0 : 100) << "%\n";
    os << "Cores used: " << runningProcesses.size() << "\n";
    
    int coresAvailable = numCPUs > runningProcesses.size() ? numCPUs - runningProcesses.size() : 0;
    os << "Cores available: " << coresAvailable << "\n";
    os << "------------------------------------------------------------\n\n";
    
    os << "Running processes:\n";
    for (const auto& process : runningProcesses) {
        os << process->getProcessName() << "  (" << formatTime(process->getStartTime()) << ")  ";
        os << "Core: " << process->getCoreId() << "   ";
        os << process->getProgress() << " / " << process->getTotalWork() << "\n";
    }

    os << "\nFinished processes:\n";
    for (const auto& process : finishedProcesses) {
        os << process->getProcessName() << "  (" << formatTime(process->getEndTime()) << ")  ";
        os << "Finished   " << process->getTotalWork() << " / " << process->getTotalWork() << "\n";
    }
    os << "------------------------------------------------------------\n";
}





void consoleManager::spawnNewProcess() {
    std::string processName = "process" + std::to_string(processCount++);  // Default process name
    startProcessScreen(processName);
}

void consoleManager::startCustomProcess(const std::string& processName) {
    startProcessScreen(processName);  // Start process with the custom name
}

void consoleManager::startProcessScreen(const std::string& processName) {
    int totalLines = 1000;  // Set total lines to 1000 for each process

    // Create a new process with the specified name and total lines
    Process* newProcess = new Process(processCount - 1, totalLines);  // Use processCount - 1 for ID consistency
    newProcess->processName = processName;

    // Lock and add to running processes
    {
        std::lock_guard<std::mutex> lock(processMutex);
        runningProcesses.push_back(newProcess);
    }

    std::string filename = processName + ".txt";
    std::ofstream processFile(filename);

    // Launch the logging process in a detached thread to run in the background
    std::thread([this, newProcess, filename, totalLines](std::ofstream processFile) mutable {
        if (processFile.is_open()) {
            for (int i = 0; i < totalLines; ++i) {
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                processFile << "Welcome to CSOPESY command line! " << std::ctime(&now);
                newProcess->incrementProgress();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            processFile.close();
        }

        // Move process to finished processes once it completes
        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(std::remove(runningProcesses.begin(), runningProcesses.end(), newProcess), runningProcesses.end());
            finishedProcesses.push_back(newProcess);
        }
    }, std::move(processFile)).detach();

    // Clear the screen and display the new process prompt
    clearScreen();
    std::cout << "Process " << newProcess->processName << ":> ";

    std::string userCommand;
    while (true) {
        std::getline(std::cin, userCommand);
        if (userCommand == "exit") {
            break;
        } else if (userCommand == "process-smi") {
            std::cout << "Process: " << newProcess->getProcessName() << "\n";
            std::cout << "ID: " << newProcess->getId() << "\n";  // Display the process ID
            std::cout << "Current instruction line: " << newProcess->getProgress() << "\n";
            std::cout << "Lines of code: " << newProcess->getTotalWork() << "\n";
            std::cout << "Process " << newProcess->processName << ":> ";
        } else {
            std::cout << "Invalid command. Type 'process-smi' to view status or 'exit' to go back to the main menu.\n";
            std::cout << "Process " << newProcess->processName << ":> ";
        }
    }
}


// used for screen -r <process name> command 
void consoleManager::reattachProcessScreen(const std::string& processName) {
    Process* targetProcess = nullptr;

    // Search for the process in the runningProcesses
    {
        std::lock_guard<std::mutex> lock(processMutex);
        for (auto process : runningProcesses) {
            if (process->getProcessName() == processName && !process->isFinished()) {
                targetProcess = process;
                break;
            }
        }
    }

    if (targetProcess) {
        clearScreen();
        std::cout << "Reattaching to process: " << processName << "\n";
        std::cout << "Process " << targetProcess->getProcessName() << ":> ";

        int remainingLines = targetProcess->getTotalWork() - targetProcess->getProgress();
        std::string filename = targetProcess->getProcessName() + ".txt";
        std::ofstream processFile(filename, std::ios::app); // Open in append mode to continue logging

        // Relaunch the logging in a detached thread to continue printing the remaining lines
        std::thread([this, targetProcess, filename, remainingLines](std::ofstream processFile) mutable {
            if (processFile.is_open()) {
                for (int i = 0; i < remainingLines; ++i) {
                    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    processFile << "Welcome to CSOPESY command line! " << std::ctime(&now);
                    targetProcess->incrementProgress();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                processFile.close();
            }

            // Move process to finished processes once it completes
            {
                std::lock_guard<std::mutex> lock(processMutex);
                runningProcesses.erase(std::remove(runningProcesses.begin(), runningProcesses.end(), targetProcess), runningProcesses.end());
                finishedProcesses.push_back(targetProcess);
            }
        }, std::move(processFile)).detach();

        // Display process prompt and handle user input
        std::string userCommand;
        while (true) {
            std::getline(std::cin, userCommand);
            if (userCommand == "exit") {
                break;
            } else if (userCommand == "process-smi") {
                std::cout << "Process: " << targetProcess->getProcessName() << "\n";
                std::cout << "ID: " << targetProcess->getId() << "\n";
                std::cout << "Current instruction line: " << targetProcess->getProgress() << "\n";
                std::cout << "Lines of code: " << targetProcess->getTotalWork() << "\n";
                std::cout << "Process " << targetProcess->getProcessName() << ":> ";
            } else {
                std::cout << "Invalid command. Type 'process-smi' to view status or 'exit' to go back to the main menu.\n";
                std::cout << "Process " << targetProcess->getProcessName() << ":> ";
            }
        }
    } else {
        std::cout << "screen '" << processName << "' does not exist or has finished.\n";
    }
}

void consoleManager::createDummyProcess() {
    Process* dummyProcess = new Process(processCount++, 1000);  // Initialize with 1000 lines of code
    dummyProcess->processName = "dummy_process" + std::to_string(dummyProcess->getId());

    // Lock and add to runningProcesses
    {
        std::lock_guard<std::mutex> lock(processMutex);
        runningProcesses.push_back(dummyProcess);
    }

    // Background thread for the dummy process execution
    std::thread([this, dummyProcess]() {
        for (int i = 0; i < dummyProcess->getTotalWork() && !stopScheduler; ++i) {
            dummyProcess->incrementProgress();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Simulate work delay
        }

        // Move the process to finishedProcesses once completed
        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(std::remove(runningProcesses.begin(), runningProcesses.end(), dummyProcess), runningProcesses.end());
            finishedProcesses.push_back(dummyProcess);
        }
    }).detach();  // Detach to let it run in the background
}




void consoleManager::startSchedulerTest() {
    if (generatingProcesses) {
        std::cout << "Scheduler is already running.\n";
        return;
    }

    stopScheduler = false;
    generatingProcesses = true;

    // Start the scheduler in a new thread
    processGeneratorThread = std::thread([this]() {
        std::cout << "Scheduler test started: generating dummy processes...\n";
        
        while (!stopScheduler.load()) {
            // Use a separate thread for each process generation for faster parallelism
            std::thread(&consoleManager::createDummyProcess, this).detach();
        }

        std::cout << "Exiting scheduler generation loop.\n";
    });
}

void consoleManager::stopSchedulerTest() {
    if (!generatingProcesses) {
        std::cout << "Scheduler is not running.\n";
        return;
    }

    stopScheduler = true;  // Signal the generator loop to stop
    if (processGeneratorThread.joinable()) {
        processGeneratorThread.join();  // Wait for the generation loop to complete
    }
    
    generatingProcesses = false;
    std::cout << "Scheduler test stopped: no more dummy processes will be generated.\n";
}


void consoleManager::processGenerator() {
    while (!stopScheduler.load()) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push([this]() { createDummyProcess(); });
        }
        condition.notify_one();  // Notify a worker thread to execute the task

        if (stopScheduler) break; // Exit if stopScheduler is set
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Process generation loop exiting.\n";
}




// Constructor: initialize thread pool and set stopScheduler to false
consoleManager::consoleManager() : initializer(), stopScheduler(false) {
    int threadCount = std::thread::hardware_concurrency();
    for (int i = 0; i < threadCount; ++i) {
        threadPool.emplace_back(&consoleManager::workerThread, this);
    }
}

consoleManager::~consoleManager() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopScheduler = true;
    }
    condition.notify_all();
    for (std::thread &worker : threadPool) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void consoleManager::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stopScheduler.load() || !taskQueue.empty(); });

            // Exit if stopScheduler is true and no tasks are left
            if (stopScheduler.load() && taskQueue.empty()) {
                std::cout << "Exiting worker thread as stop signal received and queue is empty.\n";
                return;
            }

            if (!taskQueue.empty()) {
                task = std::move(taskQueue.front());
                taskQueue.pop();
            }
        }
        
        if (task) {
            task();  // Execute the task outside of lock
        }
    }
}


void consoleManager::handleProcessSmi(const std::string& processName) {
    for (auto& process : runningProcesses) {
        if (process->getProcessName() == processName) {
            std::cout << "Process: " << process->getProcessName() << std::endl;
            std::cout << "ID: " << process->getId() << std::endl;
            std::cout << "Current Instruction Line: " << process->getProgress() << std::endl;
            std::cout << "Lines of Code: " << process->getTotalWork() << std::endl;
            return;
        }
    }
    std::cout << "Process " << processName << " not found.\n";
}


int consoleManager::handleCommand(int command, const std::string& commandStr) {
    if (!initializer.isInitialized() && command != 1 && command != 10) {
        std::cout << "Please type 'initialize' to unlock the application before using other commands." << std::endl;
        return 0;
    }

    switch (command) {
        case 1:
            std::cout << "Initialization successful. You can now access all commands." << std::endl;
            initializer.setInitialized();
            loadConfig();
            break;
        case 3:
            if (!generatingProcesses) {
                generatingProcesses = true;
                processGeneratorThread = std::thread(&consoleManager::processGenerator, this);
                std::cout << "Started process generation for scheduler-test." << std::endl;
            } else {
                std::cout << "Process generation is already running." << std::endl;
            }
            break;
        case 4:
            if (generatingProcesses) {
                generatingProcesses = false;
                if (processGeneratorThread.joinable()) {
                    processGeneratorThread.join();
                }
                std::cout << "Stopped process generation for scheduler-stop." << std::endl;
            } else {
                std::cout << "Process generation is not currently running." << std::endl;
            }
            break;
        case 5:
            generateProcessReport("csopesy-log.txt");
            break;
        case 6:
            spawnNewProcess();
            break;
        case 7:
            listProcesses(std::cout);
            break;
        case 8: {
            std::string processName = commandStr.substr(10);  // Extract everything after "SCREEN -S "
            if (!processName.empty()) {
                startCustomProcess(processName);
            } else {
                std::cout << "Error: Please provide a process name with 'screen -s <process name>'\n";
            }
            break;
        }
        case 9: {
            std::string processName = commandStr.substr(10);  // Extract everything after "SCREEN -R "
            if (!processName.empty()) {
                reattachProcessScreen(processName);
            } else {
                std::cout << "Error: Please provide a process name with 'screen -r <process name>'\n";
            }
            break;
        }
        case 10:
            return 1;
        default:
            std::cout << "Invalid command! Try again." << std::endl;
    }
    return 0;
}


