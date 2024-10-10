#include "consoleManager.h"
#include "gpuManager.h"
#include <iostream>
#include <algorithm>
#include "baseScreen.h"
#include "ncurses.h"
#include <sstream>


consoleManager::consoleManager() {}

void consoleManager::displayMenu() {
    std::cout << "\033[32m" << "Welcome to CSOPESY commandline!" << "\033[0m" << std::endl; // Set green text color
    std::cout << "Type 'exit' to quit, 'clear' to clear screen, 'gpu-info' for GPU information." << std::endl; 
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
    else if (cmd == "GPU-INFO" || cmd == "NVIDIA-SMI") return 7; // Fix comparison here
    else if (cmd == "EXIT") return 8;
    else return 9; 
}

int consoleManager::handleCommand(int command) {
    switch (command) {
        case 1:
            std::cout << "Initialize command recognized." << std::endl;
            break;
        case 2:
            screenCLI();
            break;
        case 3:
            std::cout << "Scheduler Test command recognized." << std::endl;
            break;
        case 4:
            std::cout << "Scheduler Stop command recognized." << std::endl;
            break;
        case 5:
            BaseScreen::gpuStatusScreen(); // Assuming this function is defined in baseScreen.h
            break;
        case 6:
            std::system("clear"); 
            break;
        case 7:
            gpuCLI();  // Execute the GPU-related command
            break;
        case 8:
            return 1;  // Exit
        default:
            std::cout << "Invalid command! Try again." << std::endl;
    }
    return 0; // Continue the loop
}

// Function for screen-related commands
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

void consoleManager::gpuCLI() {
    initscr();  // Start ncurses mode

    std::string header1 = "Terminal";
    std::string header2 = "Welcome to macOS terminal environment.";
    std::string header3 = "For GPU information, use the 'nvidia-smi' or 'gpu-smi' command.";
    std::string userPath = "/Users/username> ";

    displayMessage(header1, header2, header3, userPath);
    refresh();

    // Predefined process information with explicit initialization
    std::vector<ProcessInfo> processes;
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/System/Library/CoreServices/Finder.app", "N/A"));
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/Applications/Google Chrome.app", "N/A"));
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/System/Library/CoreServices/SystemUIServer.app", "N/A"));
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/Applications/Safari.app", "N/A"));
    processes.push_back(ProcessInfo(0, "N/A", "N/A", 1368, "C+G", "/Applications/Xcode.app", "N/A"));

    handleCommandInput(processes);  // Call GPU command handler

    getch();
    endwin();  // End ncurses mode
}
