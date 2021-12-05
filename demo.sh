#!/bin/bash

make

echo "Starting Demo! Writing to file: ${OUTPUT_FILE}"

# Start taking samples
./main 5 30 > ${OUTPUT_FILE} &

# Wait 10 seconds and run the benchmark
sleep 10
./NPB/SNU_NPB_2019/NPB3.3-OMP-C/bin/ft.B.x

# Wait for sampling to finish

echo "Demo complete!"

