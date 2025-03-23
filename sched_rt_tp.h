#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER sched_rt

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./sched_rt_tp.h"

#if !defined(_SCHED_RT_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _SCHED_RT_TP_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    sched_rt,
    sched_begin,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    sched_rt,
    sched_end,
    TP_ARGS(),
    TP_FIELDS()
)

TRACEPOINT_EVENT(
    sched_rt,
    job_release_recv,
    TP_ARGS(pid_t, vpid, pthread_t, vtid),
    TP_FIELDS(
        ctf_integer(pid_t, vpid, vpid)
        ctf_integer(pthread_t, vtid, vtid)
    )
)

TRACEPOINT_EVENT(
    sched_rt,
    thread_preempt,
    TP_ARGS(pid_t, vpid, pthread_t, vtid),
    TP_FIELDS(
        ctf_integer(pid_t, vpid, vpid)
        ctf_integer(pthread_t, vtid, vtid)
    )
)

TRACEPOINT_EVENT(
    sched_rt,
    thread_suspend,
    TP_ARGS(pid_t, vpid, pthread_t, vtid),
    TP_FIELDS(
        ctf_integer(pid_t, vpid, vpid)
        ctf_integer(pthread_t, vtid, vtid)
    )
)

TRACEPOINT_EVENT(
    sched_rt,
    thread_run,
    TP_ARGS(pid_t, vpid, pthread_t, vtid),
    TP_FIELDS(
        ctf_integer(pid_t, vpid, vpid)
        ctf_integer(pthread_t, vtid, vtid)
    )
)

#endif

#include <lttng/tracepoint-event.h>

