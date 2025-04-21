# setup
user=$(whoami)
sh build.sh || exit
[ -d lttng_trace_dl ] && rm -rf lttng_trace_dl
[ -d lttng_trace_scx ] && rm -rf lttng_trace_scx
[ -d parse_output_dl ] && rm -rf parse_output_dl
[ -d parse_output_scx ] && rm -rf parse_output_scx

# trace
sudo sh lttng_trace.sh dl
sudo chown -R "$user" lttng_trace
mv lttng_trace lttng_trace_dl
sudo sh lttng_trace.sh scx
sudo chown -R "$user" lttng_trace
mv lttng_trace lttng_trace_scx

# parse
python3 ../sched_deadline-trace-parser/parse.py -o parse_output_dl -rv lttng_trace_dl || exit
python3 ../sched_deadline-trace-parser/parse.py -o parse_output_scx -rv lttng_trace_scx || exit