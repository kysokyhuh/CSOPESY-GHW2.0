// screenManager.h
#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <string>
#include <map>

// Declare as extern so it’s referenced, not defined in other files
extern std::map<std::string, std::map<std::string, std::string>> screens;

// Function declarations
void createScreen(const std::string& name);
void showScreen(const std::string& name);

#endif // SCREEN_MANAGER_H
