#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <string>
#include <map>

// Declare the map for storing screen data
extern std::map<std::string, std::map<std::string, std::string> > screens;


// Function to create a new screen
void createScreen(const std::string& name);

// Function to show an existing screen
void showScreen(const std::string& name);

#endif // SCREEN_MANAGER_H
