sh build.sh && \
sudo sh lttng_trace.sh dl && \
sudo chown -R alexy lttng_trace && \
python3 ../sched_deadline-trace-parser/parse.py -rv lttng_trace