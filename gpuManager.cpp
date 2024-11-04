#include "gpuManager.h"
#include <ncurses.h>
#include <iostream>
#include <vector>

// Display welcome message with customizable content
void displayMessage(const std::string& header1, const std::string& header2, const std::string& header3, const std::string& userPath) {
    int y = 1;
    mvprintw(y++, 1, "%s", header1.c_str());
    mvprintw(y++, 1, "%s", header2.c_str());
    mvprintw(y++, 1, "");
    mvprintw(y++, 1, "%s", header3.c_str());
    mvprintw(y++, 1, "");
    mvprintw(y, 1, "%s", userPath.c_str());
}

// Display GPU information summary
void showGPUInfo() {
    std::vector<std::string> headers = {
        "+-------------------------------------------------------------------------------------+",
        "| NVIDIA-SMI 551.86       Driver Version: 551.86       CUDA Version: 12.4             |",
        "|---------------------------------------+----------------------+----------------------|",
        "| GPU  Name                     TCC/WDDM| Bus-Id        Disp.A | Volatile Uncorr. ECC |",
        "| Fan  Temp  Perf          Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |",
        "|                                       |                      |               MIG M. |",
        "|=======================================+======================+======================|",
        "|   0  NVIDIA GeForce GTX 1080     Off  | 00000000:26:00.0  On |                  N/A |",
        "| 30%   45C    P8            11W / 180W |    701MiB /  8192MiB |      0%      Default |",
        "|                                       |                      |                  N/A |",
        "+---------------------------------------+----------------------+----------------------+\n"
    };

    int y = 1;
    for (const auto& line : headers) {
        mvprintw(y++, 1, "%s", line.c_str());
    }
}

// Display a list of GPU processes
void showProcesses(const std::vector<ProcessInfo>& processes) {
    std::vector<std::string> processHeaders = {
        "+-------------------------------------------------------------------------------------+",
        "| Processes:                                                                          |",
        "|  GPU   GI   CI         PID  Type  Process name                     GPU Memory Usage |",
        "|        ID   ID                                                                      |",
        "|=====================================================================================|"
    };

    int y = 12; // Start position after GPU summary
    for (const auto& line : processHeaders) {
        mvprintw(y++, 1, "%s", line.c_str());
    }

    for (const ProcessInfo& proc : processes) {
        std::string truncatedName = (proc.processName.length() > 24) ? "..." + proc.processName.substr(proc.processName.length() - 20) : proc.processName;
        mvprintw(y++, 1, "|  %3d   %3s   %3s       %4d  %4s  %-23s                     %4s |",
                 proc.gpu, proc.gi_id.c_str(), proc.ci_id.c_str(), proc.pid, proc.type.c_str(),
                 truncatedName.c_str(), proc.gpuMemoryUsage.c_str());
    }

    std::string footer = "+-------------------------------------------------------------------------------------+\n";
    mvprintw(y, 1, "%s", footer.c_str());
}

// Handles input and processes commands
void handleCommandInput(const std::vector<ProcessInfo>& processes) {
    std::string input;
    char c;
    while ((c = getch()) != '\n') {
        input += c;
    }

    if (input == "nvidia-smi") {
        showGPUInfo();
        showProcesses(processes);
    } else {
        mvprintw(8, 1, "Command not recognized.");
    }
}