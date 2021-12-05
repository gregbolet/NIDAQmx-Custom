
NIDAQLIB_DIR="/usr/lib/x86_64-linux-gnu/"
NIDAQLIB_NAME="nidaqmx"

# Already captured by gcc
#INCLUDE_DIR="/usr/include/"

all: main calibrate

main : main.c
	gcc -o $@ $^ -L ${NIDAQLIB_DIR} -l ${NIDAQLIB_NAME}

calibrate : calibrate.c
	gcc -o $@ $^ -L ${NIDAQLIB_DIR} -l ${NIDAQLIB_NAME}

.PHONY: clean
clean:
	rm -f main	
	rm -f calibrate
