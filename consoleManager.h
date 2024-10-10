#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>
#include <map>

class consoleManager {
public:
    consoleManager();
    void displayMenu();
    void displayHeader();
    int validateCommand(const std::string& command);
    int handleCommand(int command);
    void screenCLI();
    void gpuCLI();

private:
    void createScreen(const std::string& name);  // Declaration for createScreen
    void showScreen(const std::string& name);    // Declaration for showScreen

    std::map<std::string, std::map<std::string, std::string> > screens;
};

#endif // CONSOLE_MANAGER_H
