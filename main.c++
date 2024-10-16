#include <iostream>
#include "consoleManager.h"

int main() {
    consoleManager consoleManager;
    bool exit = false;  // Changed to bool for clarity
    int result;
    std::string userInput;

    while (!exit) {
        consoleManager.displayHeader();  // Display the header
        std::getline(std::cin, userInput); // Get user input
        result = consoleManager.validateCommand(userInput); // Validate input
        exit = consoleManager.handleCommand(result); // Handle command
    }

    return 0;
}

/*
    How to run sa terminal?

    g++ -std=c++11 -lncurses consoleManager.c++ baseScreen.c++ screenManager.c++ gpuManager.c++ marqueeManager.c++ main.c++ -o App

    ./App

*/
