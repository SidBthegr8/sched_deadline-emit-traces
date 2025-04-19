#include "fraction.h"

#include <random>
#include <vector>

struct Task {
    Fraction phase, period, wcet, relative_deadline;
    Task(Fraction phase, Fraction period, Fraction wcet, Fraction relative_deadline) : phase(phase), period(period), wcet(wcet), relative_deadline(relative_deadline) {}
    Task() : Task(0, 0, 0, 0) {}
    Task(Fraction period, Fraction wcet, Fraction relative_deadline) : Task(0, period, wcet, relative_deadline) {}
    Task(Fraction period, Fraction wcet) : Task(0, period, wcet, period) {}
};

using TaskSet = std::vector<Task>;

struct TaskSetGenerator {
    static std::default_random_engine gen;
    TaskSetGenerator() = delete;

    // generates a periodic synchronous implicit-deadline task set t of size <task_count> using discrete time of length <1/precision>
    // all util vals need a denominator divisible by <precision>
    // for each task, uniformly chooses a period from the closed-interval [min_period, max_period]
    // if input invalid, returns an empty set

    // uses modified Kraemer Algorithm defined here https://www.cs.cmu.edu/~nasmith/papers/smith+tromble.tr04.pdf
    static TaskSet genModifiedKraemer(int precision, Fraction util, int task_count, int min_period, int max_period);

    // uses UUniFast-Discard
    static TaskSet genUUniFastDiscard(int precision, Fraction util, int task_count, int min_period, int max_period);

    // both genURPartition and genUUniFastDiscard should be indistinguishable, but genUUniFastDiscard is the formalized method
};
