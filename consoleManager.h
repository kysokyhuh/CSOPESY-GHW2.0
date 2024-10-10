#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>

class consoleManager {
public:
    consoleManager();
    void displayMenu();
    void displayHeader();
    int validateCommand(const std::string& command);
    int handleCommand(int command);
};

#endif // CONSOLE_MANAGER_H
