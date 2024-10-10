#include "screenManager.h"
#include <iostream>
#include <ctime>
#include <sstream>

// Define the global screens map
std::map<std::string, std::map<std::string, std::string> > screens;

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


