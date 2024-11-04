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

// Constructor for consoleManager
consoleManager::consoleManager() : initializer() {}

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

// int consoleManager::validateCommand(const std::string& command) {
//     if (command == "initialize") return 1;
//     else if (command == "scheduler-test") return 3;
//     else if (command == "scheduler-stop") return 4;
//     else if (command == "report-util") return 5;
//     else if (command == "screen") return 6;
//     else if (command == "screen -ls") return 7;
//     else if (command.substr(0, 9) == "screen -s") return 8;
//     else if (command.substr(0, 9) == "screen -r") return 9;
//     else if (command == "exit") return 10;
//     return 11; // Invalid command
// }

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
    os << "root:> screen -ls\n";
    os << "CPU utilization: 100%\n";
    os << "Cores used: " << runningProcesses.size() << "\n";
    os << "Cores available: " << (numCPUs - runningProcesses.size()) << "\n";
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
    int totalLines = 1000;  // Set total lines to 1000 for each process

    // Increment process count and create a new process with a unique name and total lines
    Process* newProcess = new Process(processCount++, totalLines);
    newProcess->processName = "process" + std::to_string(processCount);

    // Lock and add to running processes
    {
        std::lock_guard<std::mutex> lock(processMutex);
        runningProcesses.push_back(newProcess);
    }

    std::string filename = newProcess->processName + ".txt";
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
            std::cout << "Current instruction line: " << newProcess->getProgress() << "\n";
            std::cout << "Lines of code: " << newProcess->getTotalWork() << "\n";
            std::cout << "Process " << newProcess->processName << ":> ";
        } else {
            std::cout << "Invalid command. Type 'process-smi' to view status or 'exit' to go back to the main menu.\n";
            std::cout << "Process " << newProcess->processName << ":> ";
        }
    }
}



void consoleManager::startProcessScreen(const std::string& processName) {
    clearScreen();
    Process* newProcess = new Process(processCount++, 10);
    newProcess->processName = processName;
    processQueue.push(newProcess);

    std::cout << "Switched to process screen for: " << processName << "\n";
    std::string command;
    while (true) {
        std::cout << "Process " << processName << "> ";
        std::getline(std::cin, command);
        if (command == "process-smi") {
            if (newProcess->isFinished()) {
                std::cout << "Process " << newProcess->processName << " - Finished!\n";
            } else {
                newProcess->incrementProgress();
                std::cout << "Progress: " << newProcess->getProgress() << " / " << newProcess->getTotalWork() << "\n";
            }
        } else if (command == "exit") {
            if (newProcess->isFinished()) {
                finishedProcesses.push_back(newProcess);
            } else {
                runningProcesses.push_back(newProcess);
            }
            break;
        } else {
            std::cout << "Invalid command.\n";
        }
    }
}

// void consoleManager::reattachProcessScreen(const std::string& processName) {
//     for (auto process : runningProcesses) {
//         if (process->processName == processName && !process->isFinished()) {
//             clearScreen();
//             std::cout << "Reattaching to process: " << processName << "\n";
//             startProcessScreen(processName);
//             return;
//         }
//     }
//     std::cout << "Process " << processName << " not found or has finished.\n";
// }

void consoleManager::reattachProcessScreen(const std::string& processName) {
    bool processFound = false;

    for (auto process : runningProcesses) {
        if (process->processName == processName && !process->isFinished()) {
            clearScreen();
            std::cout << "Process Name: " << process->processName << "\n";
            std::cout << "Current Instruction Line: " << process->getProgress() << "\n";
            std::cout << "Lines of Code: " << process->getTotalWork() << "\n";
            std::cout << "Process created at: " << formatTime(process->getStartTime()) << "\n";
            processFound = true;
            break;
        }
    }

    // If the process was not found in running processes
    if (!processFound) {
        std::cout << "screen '" << processName << "' does not exist or has finished.\n";
    }
}


void consoleManager::processGenerator() {
    while (generatingProcesses) {
        std::this_thread::sleep_for(std::chrono::seconds(processGenerationFrequency));

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            std::string processName = "p" + std::to_string(processCount++);
            int instructionLength = 10;
            Process* newProcess = new Process(processCount, instructionLength);
            newProcess->processName = processName;
            processQueue.push(newProcess);
            std::cout << "Generated Process: " << processName << std::endl;
        }
    }
    std::cout << "Process generation stopped." << std::endl;
}

void consoleManager::startCustomProcess(const std::string& processName) {
    int totalLines = 1000;  // Set the total lines of output for each process

    // Create a new process with the user-specified name
    Process* newProcess = new Process(processCount++, totalLines);
    newProcess->processName = processName; // Use custom name from the user

    // Add the new process to the running processes list
    runningProcesses.push_back(newProcess);

    // Filename format: "<processName>.txt"
    std::string filename = processName + ".txt";
    std::ofstream processFile(filename);

    // Launch the process in a detached thread to allow background execution
    std::thread([this, newProcess, filename, totalLines](std::ofstream processFile) mutable {
        if (processFile.is_open()) {
            for (int i = 0; i < totalLines; ++i) {
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                processFile << "Welcome to CSOPESY command line! " << std::ctime(&now); // Log message with timestamp
                newProcess->incrementProgress();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            processFile.close();
        }

        // Once completed, move the process to the finished list
        std::lock_guard<std::mutex> lock(processMutex);
        runningProcesses.erase(std::remove(runningProcesses.begin(), runningProcesses.end(), newProcess), runningProcesses.end());
        finishedProcesses.push_back(newProcess);

    }, std::move(processFile)).detach();

    // Display custom process prompt on screen
    clearScreen();
    std::cout << "Process " << newProcess->processName << ":> ";

    std::string userCommand;
    while (true) {
        std::getline(std::cin, userCommand);
        if (userCommand == "exit") {
            break;
        } else if (userCommand == "process-smi") {
            std::cout << "Current instruction line: " << newProcess->getProgress() << "\n";
            std::cout << "Lines of code: " << newProcess->getTotalWork() << "\n";
            std::cout << "Process " << newProcess->processName << ":> ";
        } else {
            std::cout << "Invalid command. Type 'process-smi' to view status or 'exit' to go back to the main menu.\n";
            std::cout << "Process " << newProcess->processName << ":> ";
        }
    }
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


