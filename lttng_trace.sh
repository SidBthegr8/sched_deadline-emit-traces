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
sudo lttng start
sudo ./simulate_tasks taskset.txt 5 0 > temp
sudo lttng stop
# sudo lttng view
sudo lttng destroy sched_trace
sudo babeltrace2 lttng_trace > lttng_trace/bt2.txt
