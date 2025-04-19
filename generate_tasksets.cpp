#include "generate_tasksets.h"

#include <set>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
std::default_random_engine TaskSetGenerator::gen;

TaskSet TaskSetGenerator::genModifiedKraemer(int precision, Fraction util, int task_count, int min_period, int max_period) {
    // input validation
    auto frac_valid = [precision](Fraction frac) {
        return (frac * precision).isInt();
    };
    if (precision < 1 || task_count < 1 || util <= 0 || !frac_valid(util)) return {};
    
    // scale up time values by precision (works on discrete time of units 1/precision)
    int scaled_util = (util * precision).getNum();
    if (scaled_util < task_count) return {};

    // modified Kraemers
    std::uniform_int_distribution<int> util_urand(1, scaled_util - 1), period_urand(min_period, max_period);
    std::vector<int> scaled_utils(task_count);
    while (true) {
        std::set<int> partitions;
        partitions.insert(0);
        partitions.insert(scaled_util);
        while (partitions.size() < task_count + 1)
            partitions.insert(util_urand(gen));
        bool valid = true;
        int i = -1;
        for (auto prev = partitions.begin(), curr = ++partitions.begin(); valid && curr != partitions.end(); ++prev, ++curr)
            valid = (scaled_utils[++i] = *curr - *prev) <= precision;
        if (valid) break;
    }

    // construct task set
    TaskSet task_set;
    task_set.reserve(task_count);
    for (int scaled_util : scaled_utils) {
        Fraction task_util = Fraction(scaled_util, precision);
        Fraction task_period = Fraction(period_urand(gen));
        task_set.emplace_back(task_period, task_util * task_period);
    }
    return task_set;
}

// UUniFast algorithm
std::vector<double> uunifast(std::default_random_engine& gen, double u, int n) {
    std::uniform_real_distribution<double> urand(0, 1);
    std::vector<double> s(n);
    s[n - 1] = u;
    for (int i = n - 1; i > 0; --i) {
        s[i-1] = s[i] * std::pow(urand(gen), 1.0 / i);
        s[i] -= s[i-1];
    }
    return s;
}

TaskSet TaskSetGenerator::genUUniFastDiscard(int precision, Fraction util, int task_count, int min_period, int max_period) {
    // input validation
    auto frac_valid = [precision](Fraction frac) {
        return (frac * precision).isInt();
    };
    if (precision < 1 || task_count < 1 || util <= 0 || !frac_valid(util)) return {};
    
    // scale up time values by precision (works on discrete time of units 1/precision)
    int scaled_util = (util * precision).getNum();
    if (scaled_util < task_count) return {};

    // sample <task_count> dimensional simplex to get utils, discard ones with invalid tasks
    std::vector<int> scaled_utils(task_count);
    Fraction target_util = util - Fraction(task_count, precision);
    while (true) {
        // uunifast
        std::vector<double> utils = uunifast(gen, (double)target_util.getNum() / (double)target_util.getDen(), task_count);
        int sum = 0;

        // round down to nearest discrete time point + bump all values by 1/precision to account for minimum utilization
        for (int i = 0; i < task_count; ++i) {
            scaled_utils[i] = (int)std::floor(utils[i] * precision) + 1;
            sum += scaled_utils[i];
        }

        // bump values and check if valid
        bool valid = true;
        for (int i = 0; valid && i < task_count; ++i) {
            // bump values to account for flooring to precision
            if (sum < scaled_util) {
                ++scaled_utils[i];
                ++sum;
            }

            // check if task within bounds
            assert(scaled_utils[i] >= 1);
            valid = scaled_utils[i] <= precision;
        }
        if (valid && sum == scaled_util) break;
    }

    // construct task set
    std::uniform_int_distribution<int> period_urand(min_period, max_period);
    TaskSet task_set;
    task_set.reserve(task_count);
    for (int scaled_util : scaled_utils) {
        Fraction task_util = Fraction(scaled_util, precision);
        Fraction task_period = Fraction(period_urand(gen));
        task_set.emplace_back(task_period, task_util * task_period);
    }
    std::shuffle(task_set.begin(), task_set.end(), gen); // done to ensure bumps are sufficiently random

    return task_set;
}

int main(int argc, char **argv) {
    // argument parsing
    // arg[1] = number of tasksets
    // arg[2] = tasks per taskset
    // arg[3] = utilization
    if (argc < 4 || argc > 7) {
        std::cerr << "usage: " << argv[0] << " <number of tasksets> <tasks per taskset> <taskset utilization> [min period (ms) (default 200)] [max period (ms) (default 1000)] [output path (default ./generated_tasksets.txt)]" << std::endl;
        return 1;
    }
    uint32_t nsets = std::stoul(argv[1]);
    uint32_t ntasks = std::stoul(argv[2]);
    Fraction util;
    {
        std::string util_str = argv[3];
        size_t dec = util_str.find('.');
        int64_t num = dec == std::string::npos ? std::stoll(util_str) : std::stoll(util_str.substr(0, dec-1) + util_str.substr(dec+1));
        int64_t den = 1;
        if (dec != std::string::npos) {
            for (size_t i = dec+1; i < util_str.size(); ++i) {
                den *= 10;
            }
        }
        util = Fraction(num, den);
    }
    uint64_t min_period = argc > 4 ? std::stoull(argv[4]) : 200;
    uint64_t max_period = argc > 5 ? std::stoull(argv[5]) : 1000;
    if (min_period > max_period) {
        std::cerr << "min period > max period" << std::endl;
        return 1;
    }
    std::string output_path = argc > 6 ? argv[6] : "./generated_tasksets.txt";
    std::cout << "Generating " << nsets << " random tasksets containing " << ntasks << " tasks with a total utilization of " << *util << " and a period within " << min_period << "-" << max_period << "ms" << std::endl;

    const int64_t NS_PER_SEC = 1000000000;
    const int64_t NS_PER_MS = 1000000;
    const int64_t MS_PER_SEC = 1000;

    // generate tasksets
    std::ofstream output;
    output.open(output_path);
    for (int i = 0; i < nsets; ++i) {
        // generate taskset using ms as time unit with ns precision
        TaskSet task_set = TaskSetGenerator::genModifiedKraemer(NS_PER_MS, util, ntasks, min_period, max_period);

        // output to file
        for (const Task &task : task_set) {
            output << "(" << i << "," << *task.period << "," << *task.relative_deadline << "," << *task.wcet << ")\n";
        }
    }
    output.close();
    std::cout << "Output written to " << output_path << std::endl;
}