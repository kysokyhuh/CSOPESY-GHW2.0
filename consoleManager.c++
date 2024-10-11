#include "consoleManager.h"
#include "gpuManager.h"
#include "process.h"
#include "marqueeManager.h"
#include <iostream>
#include <algorithm>
#include "baseScreen.h"
#include "ncurses.h"
#include <sstream>
#include <map>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

// Global variables for process scheduling
std::map<std::string, std::map<std::string, std::string>> screens;  // Define the screens map globally
std::queue<Process*> processQueue;
std::mutex queueMutex;
std::vector<Process*> finishedProcesses;  // Store finished processes
std::vector<Process*> runningProcesses;   // Track running processes
bool running = true;

consoleManager::consoleManager() {}

void consoleManager::displayMenu() {
    std::cout << "\033[32m" << "Welcome to CSOPESY commandline!" << "\033[0m" << std::endl; // Set green text color
    std::cout << "Type 'exit' to quit, 'clear' to clear screen, 'gpu-info' for GPU information, 'marquee-console' for marquee." << std::endl;
    std::cout << "Type 'initialize' to start the FCFS scheduler and create processes." << std::endl;  // Add instruction for FCFS
    std::cout << "Enter a command: "; // Prompt for user input
}

void consoleManager::displayHeader() {
    std::cout << " ____  ____  ____  ____  ____  ____  _  _    \n";
    std::cout << "/\\  __\\/\\  ___\\/\\  _ \\/\\  == \\/\\  ___\\/\\  ___\\/\\ \\_\\ \\   \n";
    std::cout << "\\ \\ \\___\\ \\___  \\ \\ \\/\\ \\ \\  -/\\ \\  __\\\\ \\__  \\ \\____ \\  \n";
    std::cout << " \\ \\_____\\/\\_____\\ \\_____\\ \\_\\   \\ \\_____\\/\\_____\\/\\_____\\ \n";
    std::cout << "  \\/_____/\\/_____/\\/_____/\\/_/    \\/_____/\\/_____/\\/_____/ \n";
    displayMenu(); 
}

int consoleManager::validateCommand(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper); 

    if (cmd == "INITIALIZE") return 1;
    else if (cmd == "SCREEN") return 2;
    else if (cmd == "SCHEDULER-TEST") return 3;
    else if (cmd == "SCHEDULER-STOP") return 4;
    else if (cmd == "REPORT-UTIL") return 5;
    else if (cmd == "CLEAR") return 6;
    else if (cmd == "GPU-INFO" || cmd == "NVIDIA-SMI") return 7;
    else if (cmd == "MARQUEE-CONSOLE") return 8;  // Add Marquee command
    else if (cmd == "EXIT") return 9;
    else return 10;  // Invalid command
}

int consoleManager::handleCommand(int command) {
    switch (command) {
        case 1:
            std::cout << "Starting FCFS Scheduler and creating processes..." << std::endl;
            createProcesses();  // Initialize processes for FCFS
            handleSchedulerCommand();  // Start the FCFS scheduler and workers
            break;
        case 2:
            screenCLI();  // Trigger the screen commands
            break;
        case 3:
            std::cout << "Scheduler Test command recognized." << std::endl;
            break;
        case 4:
            std::cout << "Scheduler Stop command recognized." << std::endl;
            break;
        case 5:
            BaseScreen::gpuStatusScreen();  // Assuming this function is defined in baseScreen.h
            break;
        case 6:
            std::system("clear");
            break;
        case 7:
            gpuCLI();  // Trigger the GPU-related command
            break;
        case 8:
            bouncingMarquee("Hello World in Marquee!!!", 100);  // Trigger the marquee console
            break;
        case 9:
            return 1;  // Exit
        default:
            std::cout << "Invalid command! Try again." << std::endl;
    }
    return 0;  // Continue the loop
}

// Function to create processes for the FCFS scheduler
void consoleManager::createProcesses() {
    for (int i = 1; i <= 10; ++i) {
        Process* newProcess = new Process(i);  // Create a new process
        processQueue.push(newProcess);  // Add process to the queue
    }
}

// Function to handle the FCFS scheduler
void consoleManager::handleSchedulerCommand() {
    std::thread schedulerThread(&consoleManager::scheduler, this);  // Start the scheduler thread

    // Start CPU worker threads for 4 cores
    std::vector<std::thread> cpuThreads;
    for (int i = 0; i < 4; ++i) {
        cpuThreads.push_back(std::thread(&consoleManager::cpuWorker, this, i + 1));
    }

    // Periodically show the running and finished processes
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(2));  // Sleep for 2 seconds between updates

        // Display running and finished processes
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            std::cout << "Running processes:\n";
            for (const auto& process : runningProcesses) {
                std::cout << "Process: " << process->getProcessName() << "\n";
            }
            std::cout << "Finished processes:\n";
            for (const auto& process : finishedProcesses) {
                std::cout << "Process: " << process->getProcessName() << " Finished\n";
            }
        }

        // Stop if all processes are finished
        if (finishedProcesses.size() == 10) {
            running = false;
        }
    }

    // Wait for the threads to finish
    schedulerThread.join();
    for (auto& t : cpuThreads) {
        t.join();
    }
}

// Scheduler function to manage process execution
void consoleManager::scheduler() {
    while (running) {
        // Check if there are running processes
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (processQueue.empty() && runningProcesses.empty()) {
                running = false;  // Stop the scheduler if no more processes
            }
        }

        // Simulate scheduler running
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// CPU Worker function (representing each core)
void consoleManager::cpuWorker(int coreId) {
    while (running) {
        Process* process = nullptr;

        // Get a process from the queue
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!processQueue.empty()) {
                process = processQueue.front();
                processQueue.pop();
                runningProcesses.push_back(process);  // Add to running processes
            }
        }

        // Process the print commands
        if (process) {
            while (!process->printCommands.empty()) {
                std::string command = process->printCommands.front();
                process->printCommands.pop();
                
                // Log the print command with the core ID and timestamp in the correct format
                process->logCommand(command, coreId);

                // Simulate processing time
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            // Mark process as finished and move it to finished list
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                finishedProcesses.push_back(process);
                runningProcesses.erase(
                    std::remove(runningProcesses.begin(), runningProcesses.end(), process),
                    runningProcesses.end());
            }
        }
    }
}

// Function to display GPU information
void consoleManager::gpuCLI() {
    std::cout << "Displaying GPU Information..." << std::endl;
    initscr();  // Start ncurses mode
    noecho();  // Disable echoing

    std::string header1 = "Terminal";
    std::string header2 = "Welcome to macOS terminal environment.";
    std::string header3 = "For GPU information, use the 'nvidia-smi' or 'gpu-smi' command.";
    std::string userPath = "/Users/username> ";

    displayMessage(header1, header2, header3, userPath);  // Display a welcome message
    refresh();

    // Example predefined process info
    std::vector<ProcessInfo> processes;
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/System/Library/CoreServices/Finder.app", "N/A"));
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/Applications/Google Chrome.app", "N/A"));
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/System/Library/CoreServices/SystemUIServer.app", "N/A"));

    handleCommandInput(processes);  // Display and handle GPU processes

    getch();  // Wait for user input before exiting
    endwin();  // End ncurses mode
}

// Function to handle screen-related commands
void consoleManager::screenCLI() {
    std::string input;
    std::cout << "Screen Menu" << std::endl;
    std::cout << "Type 'screen -s <name>' to create a new screen." << std::endl;
    std::cout << "Type 'screen -r <name>' to reattach to a screen." << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::string subCommand, option, name;
        iss >> subCommand;

        if (subCommand == "screen") {
            iss >> option;
            if (option == "-s") {
                iss >> name;
                if (screens.find(name) != screens.end()) {
                    std::cout << "Screen '" << name << "' already exists." << std::endl;
                } else {
                    createScreen(name);
                }
            } else if (option == "-r") {
                iss >> name;
                showScreen(name);
            } else {
                std::cout << "Unknown screen command. Please try again." << std::endl;
            }
        } else if (input == "exit") {
            std::cout << "Exiting back to the main menu." << std::endl;
            break;
        } else {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }
}

// Function to create a new screen
void consoleManager::createScreen(const std::string& name) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%m/%d/%Y, %I:%M:%S %p", ltm);
    
    std::map<std::string, std::string> screenData;
    screenData["name"] = name;
    screenData["current_line"] = "0";
    screenData["total_lines"] = "100";
    screenData["timestamp"] = timestamp;
    
    screens[name] = screenData;
    std::cout << "Screen '" << name << "' created." << std::endl;
}

// Function to display a screen's details
void consoleManager::showScreen(const std::string& name) {
    if (screens.find(name) != screens.end()) {
        std::map<std::string, std::string>& screenData = screens[name];
        std::cout << "Screen Name: " << screenData["name"] << std::endl;
        std::cout << "Current Line: " << screenData["current_line"] << std::endl;
        std::cout << "Total Lines: " << screenData["total_lines"] << std::endl;
        std::cout << "Timestamp: " << screenData["timestamp"] << std::endl;
    } else {
        std::cout << "Screen '" << name << "' not found." << std::endl;
    }
}
