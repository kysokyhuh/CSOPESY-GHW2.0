#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>

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

    Process(int id) : process_id(id) {
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
        mtx.lock();
        log_file << timestamp << " Core:" << core_id << " \"" << command << "\"\n";
        log_file.flush();
        mtx.unlock();
    }
};

// CPU worker function to handle process execution on a given core
void cpuWorker(int core_id, std::vector<Process*>& process_queue) {
    while (true) {
        Process* process = nullptr;

        // Acquire lock to safely access the queue
        mtx.lock();
        if (!process_queue.empty()) {
            process = process_queue.front();
            process_queue.erase(process_queue.begin());
        }
        mtx.unlock();

        // If no more processes, exit the thread
        if (process == nullptr) {
            break;
        }

        // Execute all commands for the process
        for (const auto& command : process->commands) {
            process->executeCommand(core_id, command);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate command execution time
        }
    }
}

int main() {
    // Create a queue of processes (FCFS)
    std::vector<Process*> process_queue;
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        process_queue.push_back(new Process(i));
    }

    // Create threads for the CPU cores
    std::vector<std::thread> core_threads;
    for (int core_id = 0; core_id < NUM_CORES; ++core_id) {
        core_threads.emplace_back(cpuWorker, core_id, std::ref(process_queue));
    }

    // Join all threads to ensure all cores finish their work
    for (auto& t : core_threads) {
        t.join();
    }

    // Clean up process objects
    for (auto process : process_queue) {
        delete process;
    }

    return 0;
}