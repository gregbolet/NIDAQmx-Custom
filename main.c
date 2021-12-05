
#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

// This is the name of the DAQ device
#define DAQ_DEVICE_NAME "cDAQ1"

// This is the name of the module port we measure from
#define DAQ_MODULE_NAME "Mod3"
#define DAQ_DEVICE DAQ_DEVICE_NAME DAQ_MODULE_NAME

// The END channel is INCLUSIVE
// These are the pin indices used for DIFFERENTIAL mode
// Pin X will automatically be paired with its corresponding
// x+8 pin. 
// This setup assumes we are using pins 0, 1, 2, and 3
// with their complementary pins 8, 9, 10, and 11
// which are automatically read by the NIDAQ library
// when we are in differential mode
#define DIFF_INPUTS_BEGIN "0"
#define DIFF_INPUTS_END   "3"

// We'll pass this into the CreateAIVoltageChan to specify
// the channels we want to work with. 
#define DIFF_CHANNELS DAQ_DEVICE "/ai" DIFF_INPUTS_BEGIN ":" DIFF_INPUTS_END 
#define PHYS_CHANNELS DIFF_CHANNELS

// The name assigned to our task
#define DAQ_TASK_NAME ""

// The name we assign to our voltage channel
#define CHANNEL_NAME ""

// The minimum and maximum volts we expect to measure
// The NIDAQ device only supports max of -10/10 volts
// This is not good if we have a 12V supply we want to measure
#define MIN_VOLTS -10.0
#define MAX_VOLTS  10.0

// Number of samples to collect each second for each channel
#define SAMPLES_PER_SEC 10

// The number of samples we want to take for each channel
//#define SAMPLES_PER_CHANNEL 16000
#define SAMPLES_PER_CHANNEL 200

// The amount of time to wait to read the samples
#define SAMPLES_WAIT_TIMEOUT_SECS 100

// The number of differential channel pairs we will read from
//#define NUM_CHANNEL_PAIRS 4
#define NUM_CHANNEL_PAIRS 4

// The number of samples we expect to collect
#define ARRAY_SIZE_IN_SAMPLES NUM_CHANNEL_PAIRS*SAMPLES_PER_CHANNEL

// The resistance of the resister that we measure the voltage diff over
#define RESISTOR_OHMS 0.003

// The voltage we assume that all the lines are running at
#define LINE_VOLTAGE 12


int main(int argc, char** argv){

  //printf("Starting tests of NIDAQ unit...\n");
  //printf("Using device: [%s]\n", DAQ_DEVICE);
  //printf("Using channels: [%s]\n", PHYS_CHANNELS);

  int32       error = 0;
  TaskHandle  taskHandle = 0;
  int32       samples_read_per_channel;
  float64     data[ARRAY_SIZE_IN_SAMPLES];
  char        errBuff[2048]={'\0'};

  //printf("Starting task creation!\n");

  DAQmxErrChk (DAQmxCreateTask(DAQ_TASK_NAME, &taskHandle));

  //printf("Task created!\n");

  // Start in differential mode
  DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, 
                                       PHYS_CHANNELS, 
                                       //"cDAQ1Mod3/ai0:3, cDAQ1Mod3/ai8:11",
                                       CHANNEL_NAME, 
                                       DAQmx_Val_Diff, 
                                       //DAQmx_Val_NRSE,
                                       //DAQmx_Val_RSE,
                                       MIN_VOLTS, MAX_VOLTS, 
                                       DAQmx_Val_Volts, NULL));

  //printf("Voltage Chan created!\n");

  // Setup the sample clock and the rate at which we collect samples
  DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, NULL, SAMPLES_PER_SEC, 
                                    DAQmx_Val_Rising, 
                                    DAQmx_Val_FiniteSamps, 
                                    SAMPLES_PER_CHANNEL ));

  //printf("Sampling rate set!\n");

  // DAQmx Start Code
  DAQmxErrChk(DAQmxStartTask(taskHandle));

  //printf("Task started!\n");

  // DAQmx Read Code -- i.e: take samples
  // The samples are written interleaved with the GroupByScanNumber
  DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, SAMPLES_PER_CHANNEL, 
                                 SAMPLES_WAIT_TIMEOUT_SECS, 
                                 DAQmx_Val_GroupByScanNumber,
                                 //DAQmx_Val_GroupByChannel, 
                                 data, ARRAY_SIZE_IN_SAMPLES, 
                                 &samples_read_per_channel, 
                                 NULL));

  // DAQmx Stop and clear task
Error:
  if( DAQmxFailed(error) ){
    DAQmxGetExtendedErrorInfo(errBuff,2048);
  }
  if( taskHandle != 0 )  {
    //printf("NIDAQ testing complete!\n");

    DAQmxStopTask(taskHandle);
    DAQmxClearTask(taskHandle);

    //printf("We read [%d] samples for each channel\n", samples_read_per_channel);
    //printf("We took [%d] samples per second\n", SAMPLES_PER_SEC);

    // Print out the data we collected on differences across the paired pins

    float64 power;
    float64 time;
    int i,j;

    // Print the header of the csv
    printf("time, ");
    for(i = 0; i < NUM_CHANNEL_PAIRS-1; i++){
      printf("line%d, ",i);
    }
    printf("line%d\n",i);

    // Print the data
    for(i = 0; i < ARRAY_SIZE_IN_SAMPLES; i+=NUM_CHANNEL_PAIRS){
      time = (float)i/(NUM_CHANNEL_PAIRS*SAMPLES_PER_SEC);
      printf("%2.6f, ", time);
      //printf("Sample %07d: [", i/NUM_CHANNEL_PAIRS);
      for(j = 0; j < NUM_CHANNEL_PAIRS-1; j++){
        //power = (data[i+j]/RESISTOR_OHMS)*(LINE_VOLTAGE - data[i+j]);
        power = (data[i+j]/RESISTOR_OHMS)*LINE_VOLTAGE;
        //power = (data[i+j]/RESISTOR_OHMS)*data[i+j];
        //power = data[i+j];
        printf("%2.6f, ", power);
      }
      power = (data[i+j]/RESISTOR_OHMS)*LINE_VOLTAGE;
      //printf("%2.6f]\n", power);
      printf("%2.6f\n", power);
    }
  }
  if( DAQmxFailed(error) ){
    printf("DAQmx Error: %s\n",errBuff);
    return 0;
  }

  return 0;
}


