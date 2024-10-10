#include "consoleManager.h"
#include <iostream>
#include <algorithm>
#include "baseScreen.h"
#include "ncurses.h"
#include <sstream>


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
    else if (cmd == "EXIT") return 7;
    else return 9; 
}

int consoleManager::handleCommand(int command) {
    switch (command) {
        case 1:
            std::cout << "Initialize command recognized." << std::endl;
            break;
        case 2:
            screenCLI();
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
            std::system("clear"); 
            break;
        case 7:
            return 1; // Signal exit
        default:
            std::cout << "Invalid command! Try again." << std::endl;
    }
    return 0; // Continue the loop
}

void consoleManager::screenCLI() {
    std::string input;
    std::cout << "Main Menu" << std::endl;
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

// Function to show the screen
void consoleManager::showScreen(const std::string& name) {
    if (screens.find(name) != screens.end()) {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Process Name: " << screens[name]["name"] << std::endl;
        std::cout << "Instruction: " << screens[name]["current_line"] << "/" << screens[name]["total_lines"] << std::endl;
        std::cout << "Created On: " << screens[name]["timestamp"] << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Type 'exit' to go back to the main menu." << std::endl;
    } else {
        std::cout << "Screen '" << name << "' does not exist." << std::endl;
    }
}
