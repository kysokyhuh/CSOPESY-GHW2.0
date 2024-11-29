#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <deque>

int num_cpu; // Move this to the global scope

 
struct Process {
    int id;
    int core_id;
    int current_step;
    int total_instructions;
    std::string start_time;
    std::atomic<bool> finished;
    bool is_running = false;
    bool in_memory = false;
    std::vector<int> allocated_frames; // Tracks memory frames allocated to this process

    Process(int id, int total_instructions, int core_id)
        : id(id), core_id(core_id), current_step(0), total_instructions(total_instructions), finished(false) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now), "%m/%d/%Y %I:%M:%S%p");
        start_time = ss.str();
    }

    void runQuantum(int quantum_cycles) {
        int cycles = 0;
        is_running = true;
        while (current_step < total_instructions && cycles < quantum_cycles) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++current_step;
            ++cycles;
        }
        if (current_step >= total_instructions) {
            finished.store(true);
        }
        is_running = false;
    }

    std::string getStatus() const {
        std::ostringstream status;
        status << "Process " << id << " (" << start_time << ") Core: " << core_id
               << "   " << current_step << " / " << total_instructions;
        if (finished.load()) {
            status << " [Finished]";
        }
        return status.str();
    }
};

class MemoryManager {
public:
    MemoryManager(int maxMemory, int memoryPerFrame)
        : max_memory(maxMemory), mem_per_frame(memoryPerFrame), used_memory(0) {
        num_frames = max_memory / mem_per_frame;
        memory_frames.resize(num_frames, false); // Initialize all frames as free
    }

    int getMemPerFrame() const {
        return mem_per_frame;
    }

    bool allocateMemory(std::shared_ptr<Process> process) {
        int frames_needed = (process->total_instructions + mem_per_frame - 1) / mem_per_frame;

        // Debug: Log frames needed and available frames
        std::cout << "Debug: Allocating memory for Process ID " << process->id
                << " | Frames Needed = " << frames_needed
                << " | Available Frames = " << availableFrames() << "\n";

        if (availableFrames() >= frames_needed) {
            allocateFrames(process, frames_needed);
            return true;
        } else {
            return false; // Not enough memory
        }
    }


    void releaseMemory(std::shared_ptr<Process> process) {
        for (auto frame : process->allocated_frames) {
            memory_frames[frame] = false; // Free the frames
        }
        used_memory -= process->allocated_frames.size() * mem_per_frame;
        process->allocated_frames.clear();
        process->in_memory = false;
    }

    void addToBackingStore(std::shared_ptr<Process> process) {
        if (process->in_memory) {
            releaseMemory(process);
        }
        backing_store.push_back(process);
    }

    void loadFromBackingStore(std::shared_ptr<Process> process) {
        if (allocateMemory(process)) {
            // Successfully brought back into memory
            backing_store.erase(
                std::remove(backing_store.begin(), backing_store.end(), process),
                backing_store.end()
            );
        }
    }

    bool allocatePages(std::shared_ptr<Process> process, int pages_needed) {
        if (availableFrames() < pages_needed) {
            return false; // Not enough frames
        }
        for (int i = 0; i < memory_frames.size() && pages_needed > 0; ++i) {
            if (!memory_frames[i]) {
                memory_frames[i] = true;
                process->allocated_frames.push_back(i);
                --pages_needed;
            }
        }
        used_memory += pages_needed * mem_per_frame;
        process->in_memory = true;
        return true;
    }

    void deallocatePages(std::shared_ptr<Process> process) {
        for (auto frame : process->allocated_frames) {
            memory_frames[frame] = false; // Free the frame
        }
        used_memory -= process->allocated_frames.size() * mem_per_frame;
        process->allocated_frames.clear();
        process->in_memory = false;
    }

    void loadPageFromBackingStore(std::shared_ptr<Process> process, int page_index) {
        if (allocatePages(process, 1)) {
            backing_store.erase(
                std::remove_if(backing_store.begin(), backing_store.end(),
                    [process, page_index](const std::shared_ptr<Process>& p) {
                        return p->id == process->id && page_index == p->allocated_frames[0];
                    }),
                backing_store.end()
            );
        }
    }


    int calculateExternalFragmentation() const {
        int fragmentation = 0;
        int free_contiguous = 0;

        for (bool frame : memory_frames) {
            if (!frame) {
                ++free_contiguous;
            } else if (free_contiguous > 0) {
                fragmentation += free_contiguous * mem_per_frame;
                free_contiguous = 0;
            }
        }

        if (free_contiguous > 0) {
            fragmentation += free_contiguous * mem_per_frame;
        }

        return fragmentation;
    }

    int getAvailableMemory() const {
        return max_memory - used_memory;
    }

    int getMaxMemory() const {
        return max_memory;
    }

    int getUsedMemory() const {
        return used_memory;
    }

    int availableFrames() const {
        return std::count(memory_frames.begin(), memory_frames.end(), false);
    }

private:
    int max_memory, mem_per_frame, num_frames, used_memory;
    std::vector<bool> memory_frames;
    std::deque<std::shared_ptr<Process>> backing_store;

    void allocateFrames(std::shared_ptr<Process> process, int frames_needed) {
        process->allocated_frames.clear();
        for (int i = 0; i < memory_frames.size() && frames_needed > 0; ++i) {
            if (!memory_frames[i]) {
                memory_frames[i] = true;
                process->allocated_frames.push_back(i);
                --frames_needed;
            }
        }
        used_memory += frames_needed * mem_per_frame;
        process->in_memory = true;
    }
};

// Function prototypes for commands
void processSMI(const MemoryManager& memory_manager, const std::vector<std::shared_ptr<Process>>& processes);
void vmStat(const MemoryManager& memory_manager, int idle_ticks, int active_ticks);

// Other parts of the program remain unchanged from your provided code.
// Add or integrate these functions as required.


class Scheduler {
public:
    virtual void startScheduler() = 0;
    virtual void stopScheduler() = 0;
    virtual void displayStatus() = 0;
    virtual void generateUtilizationReport() = 0;
    virtual ~Scheduler() = default;
};

class RoundRobinScheduler : public Scheduler {
    int quantum_cycles, min_ins, max_ins, batch_process_freq, num_cores, delays_per_exec;
    std::atomic<bool> scheduler_running{false};
    std::atomic<bool> generator_running{false};
    mutable std::mutex mtx; // Make sure it's mutable if accessed by const methods.
    std::thread scheduler_thread, generator_thread;
    std::vector<std::shared_ptr<Process>> process_queue, finished_processes;
    MemoryManager memory_manager;
    int quantum_cycle_counter = 0;

public:
    RoundRobinScheduler(int quantum, int minIns, int maxIns, int batchFreq, int numCores, int delaysExec, MemoryManager memoryManager)
        : quantum_cycles(quantum), min_ins(minIns), max_ins(maxIns), batch_process_freq(batchFreq),
          num_cores(numCores), delays_per_exec(delaysExec), memory_manager(memoryManager) {}

    std::vector<std::shared_ptr<Process>> getProcessQueue() const {
        std::lock_guard<std::mutex> lock(mtx);
        return process_queue;
    }

    int getQuantumCycles() const {
        return quantum_cycles;
    }


    int getActiveTicks() const {
        std::lock_guard<std::mutex> lock(mtx);
        return quantum_cycle_counter * quantum_cycles; // Example: Total quantum cycles * ticks per quantum
    }
    
    void generateProcess(int process_id) {
        int instructions = min_ins + rand() % (max_ins - min_ins + 1);
        int core_id = process_id % num_cores;
        auto process = std::make_shared<Process>(process_id, instructions, core_id);
        std::lock_guard<std::mutex> lock(mtx);
        process_queue.push_back(process);
    }

    void processGenerator() {
        int process_id = 1;
        while (generator_running.load()) {
            generateProcess(process_id++);
            std::this_thread::sleep_for(std::chrono::seconds(batch_process_freq));
        }
    }

    void roundRobinExecute() {
        while (scheduler_running.load()) {
            std::lock_guard<std::mutex> lock(mtx);

            std::vector<std::shared_ptr<Process>> allocated_processes;

            for (auto &process : process_queue) {
                if (!process->in_memory) {
                    if (memory_manager.allocateMemory(process)) {
                        std::cout << "Process " << process->id << " loaded into memory.\n";
                    } else {
                        std::cout << "Process " << process->id << " could not be loaded into memory.\n";
                    }
                }
                if (process->in_memory && !process->finished.load()) {
                    process->runQuantum(quantum_cycles);
                    if (process->finished.load()) {
                        memory_manager.releaseMemory(process);
                        finished_processes.push_back(process);
                    } else {
                        allocated_processes.push_back(process);
                    }
                }
            }

            process_queue.erase(std::remove_if(process_queue.begin(), process_queue.end(), [](std::shared_ptr<Process> p) {
                return p->finished.load();
            }), process_queue.end());

            std::this_thread::sleep_for(std::chrono::milliseconds(delays_per_exec));
        }
    }


    void startScheduler() override {
        scheduler_running.store(true);
        generator_running.store(true);
        scheduler_thread = std::thread(&RoundRobinScheduler::roundRobinExecute, this);
        generator_thread = std::thread(&RoundRobinScheduler::processGenerator, this);
    }

    void stopScheduler() override {
        generator_running.store(false);

        if (generator_thread.joinable()) {
            generator_thread.join();
        }
    }

    void displayStatus() override {
        std::vector<std::shared_ptr<Process>> process_queue_copy;
        std::vector<std::shared_ptr<Process>> finished_processes_copy;
        int cores_used, cores_available;

        {
            std::lock_guard<std::mutex> lock(mtx);
            process_queue_copy = process_queue;
            finished_processes_copy = finished_processes;
            cores_used = std::min(static_cast<int>(process_queue_copy.size()), num_cores);
            cores_available = num_cores - cores_used;
        }

        std::cout << "\nCPU utilization: " << (cores_used * 100 / num_cores) << "%\n";
        std::cout << "Cores used: " << cores_used << "\n";
        std::cout << "Cores available: " << cores_available << "\n";
        std::cout << "-------------------------------------------------------------------------\n";
        std::cout << "Ready Queue (waiting to run in next cycle):\n";
        for (const auto& process : process_queue_copy) {
            if (!process->finished && !process->is_running) {
                std::cout << process->getStatus() << "\n";
            }
        }
        std::cout << "-------------------------------------------------------------------------\n";
        std::cout << "Running Processes (currently active in quantum cycle):\n";
        for (const auto& process : process_queue_copy) {
            if (process->is_running && !process->finished) {
                std::cout << process->getStatus() << "\n";
            }
        }
        std::cout << "-------------------------------------------------------------------------\n";
        std::cout << "Finished Processes:\n";
        for (const auto& process : finished_processes_copy) {
            std::cout << process->getStatus() << "\n";
        }
    }

    void generateUtilizationReport() override {
        std::ofstream report_file("csopesy-log.txt");

        std::vector<std::shared_ptr<Process>> process_queue_copy;
        std::vector<std::shared_ptr<Process>> finished_processes_copy;
        int cores_used, cores_available;

        {
            std::lock_guard<std::mutex> lock(mtx);
            process_queue_copy = process_queue;
            finished_processes_copy = finished_processes;
            cores_used = process_queue_copy.size();
            cores_available = num_cores - cores_used;
        }

        // Process Summary (similar to `process-smi`)
        report_file << "CPU utilization: " << (cores_used * 100 / num_cores) << "%\n";
        report_file << "Cores used: " << cores_used << "\n";
        report_file << "Cores available: " << cores_available << "\n";
        report_file << "-------------------------------------------------------------------------\n";

        // Detailed Memory/CPU Stats (similar to `vmstat`)
        int max_memory = memory_manager.getAvailableMemory();
        int total_memory = max_memory;
        int active_cpu_ticks = cores_used * 1000;  // Placeholder for actual CPU tick calculation
        int idle_cpu_ticks = (num_cores - cores_used) * 1000;  // Placeholder

        report_file << "Total memory: " << total_memory << " KB\n";
        report_file << "Used memory: " << (total_memory - max_memory) << " KB\n";
        report_file << "Free memory: " << max_memory << " KB\n";
        report_file << "Idle CPU ticks: " << idle_cpu_ticks << "\n";
        report_file << "Active CPU ticks: " << active_cpu_ticks << "\n";
        report_file << "-------------------------------------------------------------------------\n";
        report_file << "Processes in memory:\n";
        for (const auto& process : process_queue_copy) {
            report_file << process->getStatus() << "\n";
        }

        report_file.close();
        std::cout << "CPU utilization report has been saved to csopesy-log.txt\n";
    }

};

void readConfig(int &num_cpu, std::string &scheduler_type, unsigned int &quantum_cycles,
                unsigned int &batch_process_freq, unsigned int &min_ins, unsigned int &max_ins,
                unsigned int &delays_per_exec, int &max_mem, int &mem_per_frame, int &min_mem_proc, int &max_mem_proc, bool &use_paging) {
    num_cpu = -1;
    scheduler_type = "";
    quantum_cycles = 0;
    batch_process_freq = 0;
    min_ins = 0;
    max_ins = 0;
    delays_per_exec = 0;
    max_mem = 0;
    mem_per_frame = 0;
    min_mem_proc = 0;
    max_mem_proc = 0;
    use_paging = false;

    std::ifstream file("config.txt");
    std::string key;
    while (file >> key) {
        if (key == "num-cpu") file >> num_cpu;
        else if (key == "scheduler") file >> scheduler_type;
        else if (key == "quantum-cycles") file >> quantum_cycles;
        else if (key == "batch-process-freq") file >> batch_process_freq;
        else if (key == "min-ins") file >> min_ins;
        else if (key == "max-ins") file >> max_ins;
        else if (key == "delays-per-exec") file >> delays_per_exec;
        else if (key == "max-overall-mem") file >> max_mem;
        else if (key == "mem-per-frame") file >> mem_per_frame;
        else if (key == "min-mem-per-proc") file >> min_mem_proc;
        else if (key == "max-mem-per-proc") file >> max_mem_proc;
        else if (key == "paging") file >> use_paging;
    }
}

void processSMI(const MemoryManager& memory_manager, const std::vector<std::shared_ptr<Process>>& processes) {
    int total_memory = memory_manager.getMaxMemory();
    int used_memory = memory_manager.getUsedMemory();
    int free_memory = memory_manager.getAvailableMemory();
    int memory_utilization = (used_memory * 100) / total_memory;

    std::cout << "\n| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
    std::cout << "---------------------------------------------\n";
    std::cout << "CPU-Util: 100%\n";
    std::cout << "Memory Usage: " << (used_memory / 1024) << "MiB / " << (total_memory / 1024) << "MiB\n";
    std::cout << "Memory Util: " << memory_utilization << "%\n";
    std::cout << "\nRunning processes and memory usage:\n";

    for (const auto& process : processes) {
        if (process->in_memory) {
            int memory_used_by_process = process->allocated_frames.size() * memory_manager.getMemPerFrame();
            std::cout << "process" << std::setw(2) << std::setfill('0') << process->id
                      << " " << (memory_used_by_process / 1024) << "MiB\n";
        }
    }

    std::cout << "---------------------------------------------\n";
}



void vmStat(const MemoryManager& memory_manager, int idle_ticks, int active_ticks, int active_cores, int num_cpu) {
    std::cout << "VMSTAT: Detailed Memory and CPU Statistics\n";
    std::cout << "==========================================\n";
    std::cout << "Total Memory: " << memory_manager.getMaxMemory() << " KB\n";
    std::cout << "Used Memory: " << memory_manager.getUsedMemory() << " KB\n";
    std::cout << "Free Memory: " << memory_manager.getAvailableMemory() << " KB\n";
    std::cout << "External Fragmentation: " << memory_manager.calculateExternalFragmentation() << " KB\n";
    std::cout << "Idle CPU Ticks: " << idle_ticks << "\n";
    std::cout << "Active CPU Ticks: " << active_ticks << "\n";
    std::cout << "Active Cores: " << active_cores << " / " << num_cpu << "\n";
    std::cout << "------------------------------------------\n";
}


int calculateIdleTicks(int total_cores, int active_cores, int quantum_cycles) {
    return (total_cores - active_cores) * quantum_cycles;
}

int calculateActiveTicks(int active_cores, int quantum_cycles) {
    return active_cores * quantum_cycles;
}




int main() {
    std::atomic<bool> scheduler_running{false};
    Scheduler* scheduler = nullptr;
    MemoryManager memory_manager(16384, 4096); // Initialize memory manager
    std::vector<std::shared_ptr<Process>> process_queue;

    while (true) {
        std::string command;
        std::cout << "\n\n=========================================================================\nEnter command: ";
        std::getline(std::cin, command);

        if (command == "initialize") {
            int num_cpu;
            std::string scheduler_type;
            unsigned int quantum_cycles, batch_process_freq, min_ins, max_ins, delays_per_exec;
            int max_mem = 16384;
            int mem_per_frame = 4096;
            int min_mem_proc, max_mem_proc;
            bool use_paging;

            readConfig(
                num_cpu, scheduler_type, quantum_cycles, batch_process_freq, 
                min_ins, max_ins, delays_per_exec, max_mem, mem_per_frame, 
                min_mem_proc, max_mem_proc, use_paging
            );

            MemoryManager memoryManager(max_mem, mem_per_frame);

            if (scheduler_type == "rr" && quantum_cycles > 0) {
                scheduler = new RoundRobinScheduler(
                            quantum_cycles, 
                            min_ins, 
                            max_ins, 
                            batch_process_freq, 
                            num_cpu, 
                            delays_per_exec, 
                            memory_manager
                        );
            } else {
                std::cerr << "Error: Invalid scheduler type or configuration parameters.\n";
                continue;
            }

            std::cout << "Initialization complete. You can now use other commands.\n";
        } else if (command == "scheduler-test") {
            if (scheduler_running.load()) {
                std::cout << "Scheduler is already running.\n";
                continue;
            }
            if (scheduler) {
                scheduler_running.store(true);
                scheduler->startScheduler();
                std::cout << "Scheduler started.\n";
            } else {
                std::cout << "Initialize the scheduler first using the 'initialize' command.\n";
            }
        } else if (command == "scheduler-stop") {
            if (!scheduler_running.load()) {
                std::cout << "Scheduler is not running.\n";
                continue;
            }
            scheduler->stopScheduler();
            scheduler_running.store(false);
            std::cout << "Scheduler stopped.\n";
        } else if (command == "screen -ls") {
            if (scheduler) {
                scheduler->displayStatus();
            } else {
                std::cout << "No scheduler running.\n";
            }
        } else if (command == "report-util") {
            if (scheduler) {
                scheduler->generateUtilizationReport();
            } else {
                std::cout << "No scheduler running.\n";
            }
        } else if (command == "exit") {
            if (scheduler_running.load()) {
                scheduler->stopScheduler();
                delete scheduler;
                scheduler = nullptr;
                scheduler_running.store(false);
            }
            std::cout << "Exiting program.\n";
            break;
        } else if (command == "process-smi") {
            if (scheduler) {
                auto rrScheduler = dynamic_cast<RoundRobinScheduler*>(scheduler);
                if (rrScheduler) {
                    processSMI(memory_manager, rrScheduler->getProcessQueue());
                }
            } else {
                std::cout << "No scheduler initialized. Run 'initialize' first.\n";
            }
        } else if (command == "vmstat") {
            if (scheduler) {
                auto rrScheduler = dynamic_cast<RoundRobinScheduler*>(scheduler);
                if (rrScheduler) {
                    int active_cores = std::min(static_cast<int>(rrScheduler->getProcessQueue().size()), num_cpu);
                    int idle_ticks = calculateIdleTicks(num_cpu, active_cores, rrScheduler->getQuantumCycles());
                    int active_ticks = calculateActiveTicks(active_cores, rrScheduler->getQuantumCycles());
                    vmStat(memory_manager, idle_ticks, active_ticks, active_cores, num_cpu);
                } else {
                    std::cerr << "Scheduler type is not Round Robin.\n";
                }
            } else {
                std::cerr << "No scheduler initialized. Run 'initialize' first.\n";
            }
        } else {
            std::cout << "Invalid command. Available commands: initialize, scheduler-test, scheduler-stop, screen -ls, report-util, exit\n";
        }
    }

    return 0;
}
