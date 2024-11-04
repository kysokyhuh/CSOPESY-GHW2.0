#include "screenProcess.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <iostream>

ScreenProcess::ScreenProcess(const std::string& name) : processName(name) {}

void ScreenProcess::execute() {
    std::string filename = processName + ".txt"; // Generate filename based on process name
    writeToFile(filename); // Call the private method to write to file
}

void ScreenProcess::writeToFile(const std::string& filename) {
    std::ofstream processFile(filename);
    if (processFile.is_open()) {
        for (int i = 0; i < 100; ++i) {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            processFile << "Welcome to CSOPESY command line! " << std::ctime(&now); // Log with timestamp
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate work delay
        }
        processFile.close();
    } else {
        std::cerr << "Error: Could not open file " << filename << "\n";
    }
}
