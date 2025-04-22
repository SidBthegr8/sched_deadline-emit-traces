user=$(whoami)
sh build.sh || exit

[ -d lttng_trace_scx_edf ] && rm -rf lttng_trace_scx_edf
[ -d parse_output_scx_edf ] && rm -rf parse_output_scx_edf

[ -d lttng_trace_dl ] && rm -rf lttng_trace_dl
[ -d parse_output_dl ] && rm -rf parse_output_dl

[ -d lttng_trace_scx_simple ] && rm -rf lttng_trace_scx_simple
[ -d parse_output_scx_simple ] && rm -rf parse_output_scx_simple

[ -d lttng_trace_fifo ] && rm -rf lttng_trace_fifo
[ -d parse_output_fifo ] && rm -rf parse_output_fifo

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
  python3 ../sched_deadline-trace-parser/parse.py -o parse_output_$SCHED -rv lttng_trace_$SCHED || exit 1
}

sched_test scx_edf
sched_test dl
sched_test scx_simple
sched_test fifo
