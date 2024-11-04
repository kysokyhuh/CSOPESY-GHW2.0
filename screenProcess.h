#ifndef SCREEN_PROCESS_H
#define SCREEN_PROCESS_H

#include <string>

class ScreenProcess {
public:
    explicit ScreenProcess(const std::string& name); // Constructor with process name
    void execute(); // Public method to start the process execution

private:
    std::string processName; // Member variable to store process name
    void writeToFile(const std::string& filename); // Private method to write output to a file
};

#endif // SCREEN_PROCESS_H