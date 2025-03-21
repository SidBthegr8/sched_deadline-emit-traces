#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER task_proc

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./task_proc_tp.h"

#if !defined(_TASK_PROC_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _TASK_PROC_TP_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    task_proc,
    taskset_init,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    task_proc,
    task_init,
    TP_ARGS(int, period, int, deadline, int, wcet),
    TP_FIELDS(
        ctf_integer(int, period, period)
        ctf_integer(int, deadline, deadline)
        ctf_integer(int, wcet, wcet)
    )
)

TRACEPOINT_EVENT(
    task_proc,
    job_release,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    task_proc,
    job_completion,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    task_proc,
    kill_threads,
    TP_ARGS(),
    TP_FIELDS()
)

#endif

#include <lttng/tracepoint-event.h>

