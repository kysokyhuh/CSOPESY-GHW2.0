#include "consoleManager.h"
#include <iostream>
#include <algorithm>
#include "baseScreen.h"
#include "ncurses.h"

consoleManager::consoleManager() {}

void consoleManager::displayMenu() {
    std::cout << "\033[32m" << "Welcome to CSOPESY commandline!" << "\033[0m" << std::endl; // Set green text color
    std::cout << "Type 'exit' to quit, 'clear' to clear screen." << std::endl; 
    std::cout << "Enter a command: "; // Prompt for user input
}

void consoleManager::displayHeader() {
    std::cout << " ____  ____  ____  ____  ____  ____  _  _    \n";
    std::cout << "/\\  __\\/\\  ___\\/\\  _ \\/\\  == \\/\\  ___\\/\\  ___\\/\\ \\_\\ \\   \n";
    std::cout << "\\ \\ \\___\\ \\___  \\ \\ \\/\\ \\ \\  -/\\ \\  __\\\\ \\__  \\ \\____ \\  \n";
    std::cout << " \\ \\_____\\/\\_____\\ \\_____\\ \\_\\   \\ \\_____\\/\\_____\\/\\_____\\ \n";
    std::cout << "  \\/_____/\\/_____/\\/_____/\\/_/    \\/_____/\\/_____/\\/_____/ \n";
    displayMenu(); // Call to display menu after header
}

int consoleManager::validateCommand(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper); // Convert to uppercase for case-insensitive matching

    if (cmd == "INITIALIZE") return 1;
    else if (cmd == "SCREEN") return 2;
    else if (cmd == "SCHEDULER-TEST") return 3;
    else if (cmd == "SCHEDULER-STOP") return 4;
    else if (cmd == "REPORT-UTIL") return 5;
    else if (cmd == "CLEAR") return 6;
    else if (cmd == "EXIT") return 7;
    else return 9; // Invalid command
}

int consoleManager::handleCommand(int command) {
    switch (command) {
        case 1:
            std::cout << "Initialize command recognized." << std::endl;
            break;
        case 2:
            std::cout << "Screen command recognized." << std::endl;
            // Add screen management functionality here.
            break;
        case 3:
            // Scheduler Test Command
            std::cout << "Scheduler Test command recognized." << std::endl;
            break;
        case 4:
            // Scheduler Stop Command
            std::cout << "Scheduler Stop command recognized." << std::endl;
            break;
        case 5:
            BaseScreen::gpuStatusScreen(); // Assuming this function is defined in baseScreen.h
            break;
        case 6:
            std::system("clear"); // Use "cls" for Windows
            break;
        case 7:
            return 1; // Signal exit
        default:
            std::cout << "Invalid command! Try again." << std::endl;
    }
    return 0; // Continue the loop
}
