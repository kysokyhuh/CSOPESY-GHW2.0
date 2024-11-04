#include <iostream>
#include <atomic>
#include <thread>
#include "consoleManager.h"

int main() {
    consoleManager consoleManager;
    bool exit = false;
    int result;
    std::string userInput;

    int cpuCycles = 0;  // Initialize CPU cycle counter
    std::atomic<bool> osRunning(true);

    // Thread to increment CPU cycles (simulating OS ticks)
    std::thread cpuCycleCounter([&]() {
        while (osRunning) {
            cpuCycles++;  // Increment CPU cycles for each tick
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate time passing for each tick
        }
    });

    while (!exit) {
        consoleManager.displayHeader();
        std::cout << "root:> ";  // Prompt for user input
        std::getline(std::cin, userInput);
        
        result = consoleManager.validateCommand(userInput);
        exit = consoleManager.handleCommand(result, userInput);  // Pass both arguments
    }
    
    osRunning = false;  // Stop the CPU cycle counter loop
    cpuCycleCounter.join();  // Wait for the CPU cycle counter thread to finish

    std::cout << "Total CPU cycles: " << cpuCycles << std::endl;

    return 0;
}

/*
    how to run 

    g++ -std=c++11 -lncurses \
    consoleManager.cpp initializer.cpp baseScreen.cpp gpuManager.cpp \
    marqueeManager.cpp process.cpp screenProcess.cpp main.cpp -o CSOPESYApp


    ./CSOPESSYApp
*/