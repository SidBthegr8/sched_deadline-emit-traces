[ -f tracepoint_provider.o ] && rm tracepoint_provider.o
[ -f simulate_tasks ] && rm simulate_tasks

gcc  -c -I. tracepoint_provider.c -o tracepoint_provider.o -lttng-ust
g++ -o simulate_tasks simulate_tasks.cpp tracepoint_provider.o -llttng-ust -ldl -lpthread
