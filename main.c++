#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>

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

// https://patorjk.com/software/taag/#p=display&h=2&v=3&f=Sub-Zero&t=CSOPESY
void displayHeader() {
    std::cout << " ______  ______  ______  ______  ______  ______  __  __    \n";
    std::cout << "/\\  ___\\/\\  ___\\/\\  __ \\/\\  == \\/\\  ___\\/\\  ___\\/\\ \\_\\ \\   \n";
    std::cout << "\\ \\ \\___\\ \\___  \\ \\ \\/\\ \\ \\  _-/\\ \\  __\\\\ \\___  \\ \\____ \\  \n";
    std::cout << " \\ \\_____\\/\\_____\\ \\_____\\ \\_\\   \\ \\_____\\/\\_____\\/\\_____\\ \n";
    std::cout << "  \\/_____/\\/_____/\\/_____/\\/_/    \\/_____/\\/_____/\\/_____/ \n";
    std::cout << "                                                          \n";
    displayMenu();
}

int validateCommand(std::string str) {

    std::string command;
    int result;
    command = str;
    // for (char &c : command) {
    //     c = toupper(c);
    // }
    
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);


    //std::cout << command << std::endl; FOR TEST PURPOSES 

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

int commandFunctions(int command) {
    std::string tryAgain;

    // std::cout << command << std::endl; FOR TEST PURPOSES 

    switch (command) {
        case 1: 
            std::cout << "           Initialize command recognized. Doing something." << std::endl;
            break;

        case 2:
            std::cout << "           Screen command recognized. Doing something." << std::endl;
            break;

        case 3:
            std::cout << "           Scheduler-test command recognized. Doing something." << std::endl;
            break;

        case 4:
            std::cout << "           Scheduler-stop command recognized. Doing something." << std::endl;
            break;

        case 5:
            std::cout << "           Report-utilcommand recognized. Doing something." << std::endl;
            break;
        
        case 6: 
            std::system("clear");
            break;

        case 7:
            return 1;
            break;

        default: 
           std::cout << "This command does not exist! Please enter a valid command!" << std::endl << std::endl;
    
    }
    return 0;
}

int main() {

    int exit = 0;
    int result;
    std::string userInput;

    while (exit == 0) {
        do{
            displayHeader();
            std::getline(std::cin, userInput);
            std::cout << "" << std::endl;
            int result = validateCommand(userInput); 
            exit = commandFunctions(result);
        } while (result == 101);
    }

    
}
