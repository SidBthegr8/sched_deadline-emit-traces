# sched_deadline-emit-traces

This repository contains code for simulating tasks and generating LTTng traces on a Linux system. This guide provides instructions to compile and run the code from the `lttng` branch.

## Prerequisites

Before you begin, ensure you have the following installed on your system:
- GCC (GNU Compiler Collection)
- LTTng (Linux Trace Toolkit Next Generation)
- pthread library
- lttng-ust development library

### Installing LTTng and Libraries

```sh
sudo apt-get update
sudo apt-get install lttng-tools lttng-modules-dkms liblttng-ust-dev build-essential

### Cloning the Repository


git clone https://github.com/SidBthegr8/sched_deadline-emit-traces.git
cd sched_deadline-emit-traces
git checkout lttng

### Compiling the code:
gcc  -c -I. tracepoint_provider.c -o tracepoint_provider.o -lttng-ust
g++ -o simulate_tasks simulate_tasks.cpp tracepoint_provider.o -llttng-ust -ldl -lpthread

### Running lttng:
sudo lttng create my_session

sudo lttng enable-event -u 'task_proc:*'

sudo lttng enable-event -u 'sched_rt:*'

sudo lttng start
sudo ./simulate_tasks taskset.txt 5 > temp
sudo lttng stop
sudo lttng view
