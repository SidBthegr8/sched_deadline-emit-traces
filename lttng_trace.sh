[ -d trace ] && sudo rm -rf lttng_trace
sudo lttng destroy sched_trace
sudo lttng create sched_trace --output lttng_trace
sudo lttng enable-event -u 'task_proc:*'
sudo lttng enable-event -k 'sched*'
sudo lttng start
sudo ./simulate_tasks taskset.txt 5 > temp
sudo lttng stop
# sudo lttng view
sudo lttng destroy sched_trace
sudo babeltrace2 lttng_trace > lttng_trace/bt2.txt
