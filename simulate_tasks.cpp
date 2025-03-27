#include <iostream>
#include <thread>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <cstring>
#include <linux/sched.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>
#include <sstream>
#include <lttng/tracepoint.h>
#include "task_proc_tp.h"
#include "sched_rt_tp.h"
#include <csignal>

// task process tracepoints
namespace tp {
    void init_taskset() {
        tracepoint(task_proc, taskset_init);
    }

    void init_task(pid_t vpid, pid_t vtid, float period, float deadline, float wcet) {
        tracepoint(task_proc, task_init, vpid, vtid, period, deadline, wcet);
    }

    void release_job(pid_t vpid, pid_t vtid) {
        tracepoint(task_proc, job_release, vpid, vtid);
    }

    void complete_job(pid_t vpid, pid_t vtid) {
        tracepoint(task_proc, job_completion, vpid, vtid);
    }

    void threads_kill() {
        tracepoint(task_proc, kill_threads);
    }

    void overrun_deadline(pid_t vpid, pid_t vtid) {
        tracepoint(task_proc, deadline_overrun, vpid, vtid);
    }
}

struct LogMessage {
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string message;
};

std::queue<LogMessage> log_queue;
std::mutex log_mutex;

void log_message(const std::string& msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    log_queue.push({std::chrono::high_resolution_clock::now(), msg});
}


std::chrono::high_resolution_clock::time_point global_start_time;
bool verbose=0;

// Manually define sched_attr if necessary
struct sched_attr {
    uint32_t size;
    uint32_t sched_policy;
    uint64_t sched_flags;
    int32_t  sched_nice;
    uint32_t sched_priority;
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

#ifndef SYS_sched_setattr
#define SYS_sched_setattr 314 // System call number for x86_64
#endif

struct Task {
    int task_set;
    float period;
    float deadline;
    float wcet;
};

static int sched_setattr(pid_t pid, struct sched_attr *attr, unsigned int flags) {
    return syscall(SYS_sched_setattr, pid, attr, flags);
}

void sigxcpu_handler(int signum) {
    tp::overrun_deadline(getpid(), gettid());
    std::cout << "Thread " << gettid() << " exceeded its runtime" << std::endl;
}

struct ThreadArg {
    Task task;
    std::atomic<long long> job_id{0};
    int cpu_id;
};

std::atomic<bool> should_continue(true);
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void set_cpu_affinity(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    pthread_setaffinity_np(gettid(), sizeof(cpu_set_t), &cpuset);
}

void* task_function(void* arg) {
    ThreadArg* threadArg = (ThreadArg*) arg;
    Task task = threadArg->task;
    
    //// log task arrival
    //auto now = std::chrono::high_resolution_clock::now();
    //auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - global_start_time);
    //log_message << "task " << task.task_set << " arrived at " << elapsed.count() << " us" << std::endl;
    tp::init_task(getpid(), gettid(), task.period, task.deadline, task.wcet);

    int cpu_id = threadArg->cpu_id;

    if (cpu_id >= 0) {
        set_cpu_affinity(cpu_id);
    }

    struct sched_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.size = sizeof(attr);
    attr.sched_policy = SCHED_DEADLINE;
    attr.sched_runtime = static_cast<uint64_t>(task.wcet * 1e6);
    attr.sched_deadline = static_cast<uint64_t>(task.deadline * 1e6);
    attr.sched_period = static_cast<uint64_t>(task.period * 1e6);
    attr.sched_flags = SCHED_FLAG_DL_OVERRUN;

    if (sched_setattr(0, &attr, 0) < 0) {
        std::cerr << "failed to SCHED_DEADLINE: " << strerror(errno) << std::endl;
        pthread_exit(NULL);
    }

    pthread_rwlock_rdlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);

    while (should_continue.load()) {
        auto job_start = std::chrono::high_resolution_clock::now();
        long long current_job_id = ++threadArg->job_id;
        tp::release_job(getpid(), gettid());
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(job_start - global_start_time);
        if (verbose){
        	// log job start
            std::stringstream ss;
            ss << "job (" << task.task_set << ", " << current_job_id << ") started at " 
                        << elapsed.count() << " us" << std::endl;
            log_message(ss.str());
	    }

        uint64_t cap = (uint64_t)(5759000.f * 0.36764705882 * task.wcet);
        for (volatile uint64_t i = 0; i < cap; ++i);
        tp::complete_job(getpid(), gettid());

        auto job_end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::microseconds>(job_end - global_start_time);

        if (verbose) {
            // log job completion
            std::stringstream ss;
            ss << "job (" << task.task_set << ", " << current_job_id << ") completed at "
                        << elapsed.count() << " us" << std::endl;
            log_message(ss.str());
        }
        auto next_release = job_start + std::chrono::microseconds((int)task.period * 1000);
        auto now = std::chrono::high_resolution_clock::now();

        if (now < next_release) {
            if (verbose) {
                // log suspension
                std::stringstream ss;
                ss <<  "job (" << task.task_set << ", " << current_job_id << ") suspended at "
                            << elapsed.count() << " us" << std::endl;
                    log_message(ss.str());
            }
            
            std::this_thread::sleep_until(next_release);
                now = std::chrono::high_resolution_clock::now();
                elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - global_start_time);
            if (verbose) {
                    // log resumption
                std::stringstream ss;
                ss << "job (" << task.task_set << ", " << current_job_id << ") resumed at "
                            << elapsed.count()  << " us" << std::endl;
                    log_message(ss.str());
            }
        }
    }

    pthread_exit(NULL);
}

std::vector<Task> parse_input_file(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Task> tasks;
    std::string line;
    int task_id = 1;

    while (getline(file, line)) {
        if (line[0] == '(') {
            std::istringstream ss(line.substr(1, line.size() - 2));
            Task task;
            char comma;
            ss >> task.task_set >> comma >> task.period >> comma >> task.deadline >> comma >> task.wcet;
            task.task_set = task_id++;  // Assign task ID based on line number
            tasks.push_back(task);
        }
    }
    return tasks;
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 5) {
        std::cerr << "usage: sudo " << argv[0] << " <taskset_file> <runtime_seconds> [emit logs] [num_cores]" << std::endl;
        return 1;
    }

    std::vector<Task> tasks = parse_input_file(argv[1]);
    tp::init_taskset();
    int runtime_seconds = std::stoi(argv[2]);
    verbose = (argc == 4) ? std::stoi(argv[3]) : 0;
    int num_cores = (argc == 5) ? std::stoi(argv[4]) : 1; // Default to 1 if not specified

    pthread_rwlock_wrlock(&rwlock);
    
    signal(SIGXCPU, sigxcpu_handler);

    std::vector<pthread_t> threads(tasks.size());
    std::vector<ThreadArg> threadargs(tasks.size());

    for (size_t i = 0; i < tasks.size(); i++) {
        threadargs[i].task = tasks[i];
        threadargs[i].job_id = 0;
        threadargs[i].cpu_id = (num_cores > 1) ? (i % num_cores) : -1;
        pthread_create(&threads[i], NULL, task_function, &threadargs[i]);
    }

    sleep(2);
    global_start_time = std::chrono::high_resolution_clock::now(); // Set global start time
    if (verbose) {
    	log_message("All tasks are released at 0 us\n");
    }
    pthread_rwlock_unlock(&rwlock);
    
    should_continue.store(true);

    sleep(runtime_seconds);
    
    should_continue.store(false);
    
    for (size_t i = 0; i < threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }
    tp::threads_kill();
    
    if (verbose) {
    	// Print collected log messages
    	while (!log_queue.empty()) {
    	    const auto& log = log_queue.front();
    	    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(log.timestamp - global_start_time);
    	    std::cout << "[" << elapsed.count() << " us] " << log.message;
    	    log_queue.pop();
    	}
    }

    return 0;
}
