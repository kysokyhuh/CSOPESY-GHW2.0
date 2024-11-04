#include "screenManager.h"
#include <iostream>
#include <ctime>
#include <sstream>

//global screens map
std::map<std::string, std::map<std::string, std::string>> screens;

//Functions to create and display screens
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

void showScreen(const std::string& name) {
    if (screens.find(name) != screens.end()) {
        const auto& screenData = screens[name];
        std::cout << "Screen Name: " << screenData.at("name") << "\n";
        std::cout << "Current Line: " << screenData.at("current_line") << "\n";
        std::cout << "Total Lines: " << screenData.at("total_lines") << "\n";
        std::cout << "Timestamp: " << screenData.at("timestamp") << "\n";
    } else {
        std::cout << "Screen '" << name << "' not found." << std::endl;
    }
}
