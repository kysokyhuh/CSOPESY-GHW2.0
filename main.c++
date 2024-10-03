#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <map>
#include <ctime>
#include <sstream>

// Global storage for screens
std::map<std::string, std::map<std::string, std::string> > screens; // Added space between >>

void displayMenu() {
    std::cout << "           Welcome to CSOPESY commandline!!" << std::endl << std::endl;
    std::cout << "           ╔══════════════════════════════╗" << std::endl;
    std::cout << "           ║                              ║" << std::endl;
    std::cout << "           ║             MENU             ║" << std::endl;
    std::cout << "           ║                              ║" << std::endl;
    std::cout << "           ║          Initialize          ║" << std::endl;
    std::cout << "           ║            Screen            ║" << std::endl;
    std::cout << "           ║        Scheduler-test        ║" << std::endl;
    std::cout << "           ║        Scheduler-stop        ║" << std::endl;
    std::cout << "           ║         Report-util          ║" << std::endl;
    std::cout << "           ║            Clear             ║" << std::endl;
    std::cout << "           ║             Exit             ║" << std::endl;
    std::cout << "           ║                              ║" << std::endl;
    std::cout << "           ╚══════════════════════════════╝" << std::endl << std::endl;
    std::cout << "           Enter a command: ";
}

// ASCII Art for Header
void displayHeader() {
    std::cout << " ____  ____  ____  ____  ____  ____  _  _    \n";
    std::cout << "/\\  __\\/\\  ___\\/\\  _ \\/\\  == \\/\\  ___\\/\\  ___\\/\\ \\_\\ \\   \n";
    std::cout << "\\ \\ \\___\\ \\___  \\ \\ \\/\\ \\ \\  -/\\ \\  __\\\\ \\__  \\ \\____ \\  \n";
    std::cout << " \\ \\_____\\/\\_____\\ \\_____\\ \\_\\   \\ \\_____\\/\\_____\\/\\_____\\ \n";
    std::cout << "  \\/_____/\\/_____/\\/_____/\\/_/    \\/_____/\\/_____/\\/_____/ \n";
    std::cout << "                                                          \n";
    displayMenu();
}

// Function to create a new screen
void createScreen(const std::string& name) {
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

// Function to display a screen
void showScreen(const std::string& name) {
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

// Function to validate user commands
int validateCommand(std::string str) {
    std::string command;
    int result;
    command = str;

    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if(command == "INITIALIZE") {
        result = 1;
    }
    else if (command == "SCREEN") {
        result = 2;
    }
    else if (command == "SCHEDULER-TEST") {
        result = 3;
    }
    else if (command == "SCHEDULER-STOP") {
        result = 4;
    }
    else if (command == "REPORT-UTIL") {
        result = 5;
    }
    else if (command == "CLEAR") {
        result = 6;
    }
    else if (command == "EXIT") {
        result = 7;
    }
    else
        result = 9;

    return result;
}

// Execute user commands
int commandFunctions(int command) {
    std::string tryAgain;

    switch (command) {
        case 1: 
            std::cout << "           Initialize command recognized. Doing something." << std::endl;
            break;

        case 2: {
            std::cout << "           Screen command recognized. Opening screen manager." << std::endl;
            // Call to the CLI screen manager
            std::string screenCommand;
            while (true) {
                std::cout << "> ";
                std::getline(std::cin, screenCommand);
                std::istringstream iss(screenCommand);
                std::string subCommand;
                iss >> subCommand;
                if (subCommand == "screen") {
                    std::string option;
                    iss >> option;
                    if (option == "-s") {
                        std::string name;
                        iss >> name;
                        if (screens.find(name) != screens.end()) {
                            std::cout << "Screen '" << name << "' already exists." << std::endl;
                        } else {
                            createScreen(name);
                        }
                    } else if (option == "-r") {
                        std::string name;
                        iss >> name;
                        showScreen(name);
                    } else {
                        std::cout << "Unknown screen command. Please try again." << std::endl;
                    }
                } else if (screenCommand == "exit") {
                    break; // Exit the screen manager
                } else {
                    std::cout << "Unknown command. Please try again." << std::endl;
                }
            }
            break;
        }

        case 3:
            std::cout << "           Scheduler-test command recognized. Doing something." << std::endl;
            break;

        case 4:
            std::cout << "           Scheduler-stop command recognized. Doing something." << std::endl;
            break;

        case 5:
            std::cout << "           Report-util command recognized. Doing something." << std::endl;
            break;
        
        case 6: 
            std::system("clear");
            break;

        case 7:
            return 1; // Exit
            break;

        default: 
           std::cout << "This command does not exist! Please enter a valid command!" << std::endl << std::endl;
    }
    return 0;
}

// Main loop for handling user input
int main() {
    int exit = 0;
    int result;
    std::string userInput;

    while (exit == 0) {
        do {
            displayHeader();
            std::getline(std::cin, userInput);
            std::cout << "" << std::endl;
            result = validateCommand(userInput); 
            exit = commandFunctions(result);
        } while (result == 101);
    }

    return 0;
}
