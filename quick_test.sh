user=$(whoami)
sh build.sh || exit

sched_test() {
  SCHED=$1

  # setup
  [ -d lttng_trace_$SCHED ] && rm -rf lttng_trace_$SCHED
  [ -d parse_output_$SCHED ] && rm -rf parse_output_$SCHED

  # trace
  sudo sh lttng_trace.sh $SCHED
  sudo chown -R "$user" lttng_trace
  mv lttng_trace lttng_trace_$SCHED

  # parse
  python3 ../sched_deadline-trace-parser/parse.py -o parse_output_$SCHED -rv lttng_trace_$SCHED
}

sched_test scx_edf
sched_test dl
sched_test scx_simple
sched_test fifo
