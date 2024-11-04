#include <iostream>
#include "consoleManager.h"

int main() {
    consoleManager consoleManager;
    bool exit = false;
    int result;
    std::string userInput;

    while (!exit) {
        consoleManager.displayHeader();
        std::cout << "root:> ";  // Prompt for user input
        std::getline(std::cin, userInput);
        
        result = consoleManager.validateCommand(userInput);
        exit = consoleManager.handleCommand(result, userInput);  // Pass both arguments
    }
    
    return 0;
}
/*
    how to run 

    g++ -std=c++11 -lncurses \
    consoleManager.cpp initializer.cpp baseScreen.cpp gpuManager.cpp \
    marqueeManager.cpp process.cpp screenProcess.cpp main.cpp -o CSOPESYApp


    ./CSOPESSYApp


*/