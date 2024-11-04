#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string> 

class ConfigManager {
public:
    bool loadConfig(const std::string& filename); 
    std::string getSchedulerType() const;
    int getNumCPUs() const;
    int getQuantumCycles() const;
    int getBatchProcessFreq() const;
    int getMinInstructions() const;
    int getMaxInstructions() const;
    int getDelayPerExec() const;

private:
    std::string schedulerType;
    int numCPUs;
    int quantumCycles;
    int batchProcessFreq;
    int minInstructions;
    int maxInstructions;
    int delayPerExec;
};

#endif // CONFIG_MANAGER_H
