TARGET_CPUS="2,3"
TYPE=$1

[ "$TYPE" = "scx_edf" ] && T=0
[ "$TYPE" = "dl" ] && T=1
[ "$TYPE" = "scx_simple" ] && T=2
[ "$TYPE" = "fifo" ] && T=3

[ -d lttng_trace ] && sudo rm -rf lttng_trace
sudo lttng destroy sched_trace
sudo lttng create sched_trace --output lttng_trace

sudo lttng enable-event -u 'task_proc:*'
sudo lttng enable-event -k 'sched*'
# sudo lttng enable-event -k 'x86_irq_vectors_reschedule*'
# sudo lttng enable-event -k '*'
# sudo lttng set-kernel --buffer-size=8M
# sudo lttng enable-event -k --loglevel=TRACE_INFO sf2
sudo lttng enable-event -k 'timer_hr*'
sudo lttng enable-event -k --function=replenish_dl_entity replenish_dl_entity
# sudo lttng enable-event -k --function=update_curr_dl_se update_curr_dl_se
sudo lttng enable-event -k 'rcu_utilization'

if [ "$TYPE" = "scx_edf" ] || [ "$TYPE" = "scx_simple" ]; then
  # enable scheduler
  if [ "$TYPE" = "scx_edf" ]; then
    sudo ../pure-edf/build/scheds/c/scx_edf &
  elif [ "$TYPE" = "scx_simple" ]; then
    sudo ../pure-edf/build/scheds/c/scx_simple &
  fi
  SCX_PID=$!
fi

if [ "$TYPE" = "scx_edf" ] || [ "$TYPE" = "scx_simple" ]; then
  sudo lttng enable-event -k --function=enqueue_task_scx enqueue_task_scx
  sudo lttng enable-event -k --function=dequeue_task_scx dequeue_task_scx
  sudo lttng enable-event -k --function=yield_task_scx yield_task_scx
  sudo lttng enable-event -k --function=yield_to_task_scx yield_to_task_scx
  sudo lttng enable-event -k --function=wakeup_preempt_scx wakeup_preempt_scx
  sudo lttng enable-event -k --function=balance_scx balance_scx
  sudo lttng enable-event -k --function=pick_task_scx pick_task_scx
  sudo lttng enable-event -k --function=put_prev_task_scx put_prev_task_scx
  sudo lttng enable-event -k --function=set_next_task_scx set_next_task_scx
  sudo lttng enable-event -k --function=select_task_rq_scx select_task_rq_scx
  sudo lttng enable-event -k --function=task_woken_scx task_woken_scx
  sudo lttng enable-event -k --function=set_cpus_allowed_scx set_cpus_allowed_scx
  sudo lttng enable-event -k --function=rq_online_scx rq_online_scx
  sudo lttng enable-event -k --function=rq_offline_scx rq_offline_scx
  sudo lttng enable-event -k --function=task_tick_scx task_tick_scx
  sudo lttng enable-event -k --function=switching_to_scx switching_to_scx
  sudo lttng enable-event -k --function=switched_from_scx switched_from_scx
  sudo lttng enable-event -k --function=switched_to_scx switched_to_scx
  sudo lttng enable-event -k --function=reweight_task_scx reweight_task_scx
  sudo lttng enable-event -k --function=prio_changed_scx prio_changed_scx
  sudo lttng enable-event -k --function=update_curr_scx update_curr_scx
elif [ "$TYPE" = "dl" ]; then
  sudo lttng enable-event -k --function=dl_task_timer dl_task_timer

  sudo lttng enable-event -k --function=enqueue_task_dl enqueue_task_dl
  sudo lttng enable-event -k --function=dequeue_task_dl dequeue_task_dl
  sudo lttng enable-event -k --function=yield_task_dl yield_task_dl
  sudo lttng enable-event -k --function=wakeup_preempt_dl wakeup_preempt_dl
  sudo lttng enable-event -k --function=pick_task_dl pick_task_dl
  sudo lttng enable-event -k --function=put_prev_task_dl put_prev_task_dl
  sudo lttng enable-event -k --function=set_next_task_dl set_next_task_dl
  sudo lttng enable-event -k --function=balance_dl balance_dl
  sudo lttng enable-event -k --function=select_task_rq_dl select_task_rq_dl
  sudo lttng enable-event -k --function=migrate_task_rq_dl migrate_task_rq_dl
  sudo lttng enable-event -k --function=set_cpus_allowed_dl set_cpus_allowed_dl
  sudo lttng enable-event -k --function=rq_online_dl rq_online_dl
  sudo lttng enable-event -k --function=rq_offline_dl rq_offline_dl
  sudo lttng enable-event -k --function=task_woken_dl task_woken_dl
  sudo lttng enable-event -k --function=find_lock_later_rq find_lock_later_rq
  sudo lttng enable-event -k --function=task_tick_dl task_tick_dl
  sudo lttng enable-event -k --function=task_fork_dl task_fork_dl
  sudo lttng enable-event -k --function=prio_changed_dl prio_changed_dl
  sudo lttng enable-event -k --function=switched_from_dl switched_from_dl
  sudo lttng enable-event -k --function=switched_to_dl switched_to_dl
  sudo lttng enable-event -k --function=update_curr_dl update_curr_dl
  sudo lttng enable-event -k --function=task_is_throttled_dl task_is_throttled_dl
elif [ "$TYPE" = "fifo" ]; then
  sudo lttng enable-event -k --function=enqueue_task_rt enqueue_task_rt
  sudo lttng enable-event -k --function=dequeue_task_rt dequeue_task_rt
  sudo lttng enable-event -k --function=yield_task_rt yield_task_rt
  sudo lttng enable-event -k --function=wakeup_preempt_rt wakeup_preempt_rt
  sudo lttng enable-event -k --function=pick_task_rt pick_task_rt
  sudo lttng enable-event -k --function=put_prev_task_rt put_prev_task_rt
  sudo lttng enable-event -k --function=set_next_task_rt set_next_task_rt
  sudo lttng enable-event -k --function=balance_rt balance_rt
  sudo lttng enable-event -k --function=select_task_rq_rt select_task_rq_rt
  sudo lttng enable-event -k --function=set_cpus_allowed_common set_cpus_allowed_common
  sudo lttng enable-event -k --function=rq_online_rt rq_online_rt
  sudo lttng enable-event -k --function=rq_offline_rt rq_offline_rt
  sudo lttng enable-event -k --function=task_woken_rt task_woken_rt
  sudo lttng enable-event -k --function=switched_from_rt switched_from_rt
  sudo lttng enable-event -k --function=find_lock_lowest_rq find_lock_lowest_rq
  sudo lttng enable-event -k --function=task_tick_rt task_tick_rt
  sudo lttng enable-event -k --function=get_rr_interval_rt get_rr_interval_rt
  sudo lttng enable-event -k --function=prio_changed_rt prio_changed_rt
  sudo lttng enable-event -k --function=switched_to_rt switched_to_rt
  sudo lttng enable-event -k --function=update_curr_rt update_curr_rt
  sudo lttng enable-event -k --function=task_is_throttled_rt task_is_throttled_rt
else
  echo "Invalid scheduler type $TYPE (expected scx_edf, scx_simple, dl, fifo)"
  exit
fi
sudo lttng start
# echo "$TARGET_CPUS" | sudo tee "/sys/fs/cgroup/cpuset/cpuset.cpus"
# PEP=$(cat /proc/sys/kernel/perf_event_paranoid)
# sudo sysctl -w kernel.perf_event_paranoid=0
# sudo setcap cap_sys_nice=eip ./simulate_tasks
sudo ./simulate_tasks taskset.txt 5 0 0 $T | tee temp
# sudo ./simulate_tasks taskset.txt 5 0 0 $T > temp &
# PID=$!
# echo $PID | sudo tee /sys/fs/cgroup/cpuset/cpuset.procs
# taskset -p $PID
# echo $PID | sudo tee "/sys/fs/cgroup/edf_group/cgroup.procs"
# cat "/sys/fs/cgroup/cpuset/cpuset.cpus"
# wait $PID
# sudo sysctl -w kernel.perf_event_paranoid=$PEP
# echo "" | sudo tee "/sys/fs/cgroup/cpuset/cpuset.cpus"
sudo lttng stop

if [ "$TYPE" = "scx_edf" ] || [ "$TYPE" = "scx_simple" ]; then
  # disable scheduler
  kill $SCX_PID
fi

# sudo lttng view
sudo lttng destroy sched_trace
sudo babeltrace2 lttng_trace > lttng_trace/bt2.txt
