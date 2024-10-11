#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

const int NUM_CORES = 4;
const int NUM_PROCESSES = 10;
const int NUM_PRINT_COMMANDS = 100;

std::mutex mtx;  // Mutex for synchronizing access to the queue

// Function to get the current timestamp in the desired format
std::string getTimestamp() {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* now_tm = std::localtime(&now);
    std::ostringstream oss;
    oss << "(" << std::put_time(now_tm, "%m/%d/%Y %I:%M:%S%p") << ")";
    return oss.str();
}

// Process class to represent each process
class Process {
public:
    int process_id;  
    std::vector<std::string> commands;  
    std::ofstream log_file;  
    bool finished;  
    int printed_lines;  // Number of lines printed
    int max_resources;  // Total resources available (memory limit)
    int used_resources; // Memory used so far
    int command_count;  // Count of commands executed

    Process(int id) : process_id(id), finished(false), printed_lines(0), max_resources(100), used_resources(0), command_count(0) {
        log_file.open("process_" + std::to_string(process_id) + ".txt");
        for (int i = 0; i < NUM_PRINT_COMMANDS; ++i) {
            commands.push_back("Hello world from process " + std::to_string(process_id));
        }
    }

    ~Process() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    void executeCommand(int core_id, const std::string& command) {
        std::string timestamp = getTimestamp();
        
        {
            std::lock_guard<std::mutex> lock(mtx); // Use lock_guard for exception safety
            log_file << timestamp << " Core:" << core_id << " \"" << command << "\"\n";
            log_file.flush();
            printed_lines++;     // Increment printed lines for each command
            used_resources += 1; // Increment used memory (simulate resource usage)
            command_count++;
        }

        // Check if the process has finished executing all commands
        if (command_count >= NUM_PRINT_COMMANDS) {
            std::lock_guard<std::mutex> lock(mtx);
            finished = true; // Mark the process as finished
        }
    }

    std::string getCompletionStatus() const {
        return finished ? "Finished\t" + std::to_string(used_resources) + " / " + std::to_string(max_resources) : "";
    }
};

// CPU worker function to handle process execution on a given core
void cpuWorker(int core_id, std::vector<Process*>& process_queue, std::vector<Process*>& finished_processes) {
    while (true) {
        Process* process = nullptr;

        // Acquire lock to safely access the queue
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!process_queue.empty()) {
                process = process_queue.front();
                process_queue.erase(process_queue.begin());
            }
        }

        // If no more processes, exit the thread
        if (process == nullptr) {
            break;
        }

        // Execute all commands for the process
        for (const auto& command : process->commands) {
            process->executeCommand(core_id, command);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate command execution time
        }

        // After finishing commands, add to finished processes
        {
            std::lock_guard<std::mutex> lock(mtx);
            finished_processes.push_back(process); // Add finished process to finished_processes list
        }
    }
}

// Function to periodically check and print the status of processes
void checkProcessStatus(std::vector<Process*>& process_queue, std::vector<Process*>& finished_processes) {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            bool all_finished = true;

            // Print running processes
            std::cout << "\nRunning processes:\n";
            for (const auto& process : process_queue) {
                if (!process->finished) {
                    std::cout << "process" << std::setfill('0') << std::setw(2) << process->process_id 
                              << "\t" << getTimestamp() << "\t" 
                              << "Core : " << (process->printed_lines % NUM_CORES) + 1 << "\t"
                              << process->used_resources << " / " << process->max_resources << "\n";
                    all_finished = false;
                }
            }

            // Print finished processes
            std::cout << "\nFinished processes:\n";
            for (const auto& process : finished_processes) {
                if (process->finished) {
                    std::cout << "process" << std::setfill('0') << std::setw(2) << process->process_id 
                              << "\t" << getTimestamp() << "\t" 
                              << process->getCompletionStatus() << "\n";
                }
            }

            std::cout << "\n-----------------------------------------------------------\n";

            if (all_finished && finished_processes.size() == NUM_PROCESSES) {
                std::cout << "\nAll processes finished.\nType \"exit\" to quit...\n";
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));  // Wait before next status check
    }
}


int main() {
    // Create a queue of processes (FCFS)
    std::vector<Process*> process_queue;
    std::vector<Process*> finished_processes;  // Separate vector for finished processes

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        process_queue.push_back(new Process(i));
    }

    // Create threads for the CPU cores
    std::vector<std::thread> core_threads;
    for (int core_id = 0; core_id < NUM_CORES; ++core_id) {
        core_threads.emplace_back(cpuWorker, core_id, std::ref(process_queue), std::ref(finished_processes));
    }

    // Start the status checking in the main thread
    std::thread status_thread(checkProcessStatus, std::ref(process_queue), std::ref(finished_processes));

    // Join all threads to ensure all cores finish their work
    for (auto& t : core_threads) {
        t.join();
    }
    status_thread.join();

    // Clean up process objects
    for (auto process : process_queue) {
        delete process;
    }
    for (auto process : finished_processes) {
        delete process;  // Clean up finished processes
    }

    return 0;
}
