#!/bin/bash

SAMPLES_PER_SEC=5
SAMPLING_TIME_IN_SECS=30
OUTPUT_FILE=testout.csv

echo "Starting Demo! Writing to file: ${OUTPUT_FILE}"

# Start taking samples
./main $SAMPLES_PER_SEC $SAMPLING_TIME_IN_SECS > ${OUTPUT_FILE} &

waitpid=$!

# Wait 10 seconds and run the benchmark
sleep 10
./NPB/SNU_NPB_2019/NPB3.3-OMP-C/bin/ft.B.x

echo "FT Benchmark Complete!"
echo "Waiting for sampling to finish..."

# Wait for sampling to finish
wait $waitpid

echo "Demo complete!"
