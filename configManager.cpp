#include "configManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

bool ConfigManager::loadConfig(const std::string& filename) {
    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open config file: " << filename << std::endl;
        return false;
    }

    std::string key;
    while (configFile >> key) {
        if (key == "num-cpu") configFile >> numCPUs;
        else if (key == "scheduler") configFile >> schedulerType;
        else if (key == "quantum-cycles") configFile >> quantumCycles;
        else if (key == "batch-process-freq") configFile >> batchProcessFreq;
        else if (key == "min-ins") configFile >> minInstructions;
        else if (key == "max-ins") configFile >> maxInstructions;
        else if (key == "delay-per-exec") configFile >> delayPerExec;
    }
    configFile.close();
    return true;
}

int ConfigManager::getNumCPUs() const {
    return numCPUs;
}

std::string ConfigManager::getSchedulerType() const {
    return schedulerType;
}

int ConfigManager::getQuantumCycles() const {
    return quantumCycles;
}

int ConfigManager::getBatchProcessFreq() const {
    return batchProcessFreq;
}

int ConfigManager::getMinInstructions() const {
    return minInstructions;
}

int ConfigManager::getMaxInstructions() const {
    return maxInstructions;
}

int ConfigManager::getDelayPerExec() const {
    return delayPerExec;
}
