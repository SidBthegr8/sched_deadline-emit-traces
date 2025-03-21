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
    TP_ARGS(unsigned int, vtid),
    TP_FIELDS(
        ctf_integer(unsigned int, vtid, vtid)
    )
)

TRACEPOINT_EVENT(
    sched_rt,
    thread_preempt,
    TP_ARGS(unsigned int, vtid),
    TP_FIELDS(
        ctf_integer(unsigned int, vtid, vtid)
    )
)

TRACEPOINT_EVENT(
    sched_rt,
    thread_run,
    TP_ARGS(unsigned int, vtid, int, cpu_id),
    TP_FIELDS(
        ctf_integer(unsigned int, vtid, vtid)
        ctf_integer(int, cpu_id, cpu_id)
    )
)

TRACEPOINT_EVENT(
    sched_rt,
    deadline_overrun,
    TP_ARGS(unsigned int, vtid),
    TP_FIELDS(
        ctf_integer(unsigned int, vtid, vtid)
    )
)

#endif

#include <lttng/tracepoint-event.h>

