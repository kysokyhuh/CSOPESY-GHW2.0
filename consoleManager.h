#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>
#include <map>    // Include for std::map

class consoleManager {
public:
    consoleManager(); 
    void displayMenu();  
    void displayHeader();  // Display header with ASCII art
    int validateCommand(const std::string& command);  // Validate input command
    int handleCommand(int command);  // Handle a command
    void screenCLI();  
    
private:
    // Function to create a new screen
    void createScreen(const std::string& name);

    // Function to show an existing screen
    void showScreen(const std::string& name);

    // Map to store screen data, where each screen has its name as the key
    // Added space between right angle brackets ('> >')
    std::map<std::string, std::map<std::string, std::string> > screens;
};

#endif // CONSOLE_MANAGER_H
