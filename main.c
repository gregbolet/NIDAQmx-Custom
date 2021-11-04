
#include <stdio.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

// This is the name of the DAQ device
#define DAQ_DEVICE_NAME "cDAQ1"

// This is the name of the module port we measure from
#define DAQ_MODULE_NAME "Mod3"
#define DAQ_DEVICE DAQ_DEVICE_NAME DAQ_MODULE_NAME

#define INPUTLINES_BEGIN "0"
#define INPUTLINES_END   "4"

#define OUTPUTLINES_BEGIN "8"
#define OUTPUTLINES_END   "12"

// We'll pass this into the CreateAIVoltageChan to specify
// the pins we want to work with. It'll create virtual channels
// for us to measure on each of the pins
#define INPUT_CHANNELS DAQ_DEVICE "/ai" INPUTLINES_BEGIN ":" INPUTLINES_END 
#define OUTPUT_CHANNELS DAQ_DEVICE "/ai" OUTPUTLINES_BEGIN ":" OUTPUTLINES_END
#define PHYS_CHANNELS INPUT_CHANNELS "," OUTPUT_CHANNELS

// The name assigned to our task
#define DAQ_TASK_NAME "MYMEASUREMENTTASK"

// The name we assign to our voltage channel
#define CHANNEL_NAME "MYVOLTCHAN"

// The minimum and maximum volts we expect to measure
#define MIN_VOLTS -12.0
#define MAX_VOLTS  12.0

int main(int argc, char** argv){

  printf("Starting tests of NIDAQ unit...\n");
  printf("Using device: [%s]\n", DAQ_DEVICE);
  printf("Using channels: [%s]\n", PHYS_CHANNELS);

  int32       error=0;
  TaskHandle  taskHandle=0;
  int32       read;
  float64     data[1000];
  char        errBuff[2048]={'\0'};

  printf("Starting task creation!\n");

  DAQmxErrChk (DAQmxCreateTask(DAQ_TASK_NAME, &taskHandle));

  printf("Task created!\n");

  // Start in differential mode
  DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, PHYS_CHANNELS, CHANNEL_NAME, 
                                       DAQmx_Val_Diff, MIN_VOLTS, 
                                       MAX_VOLTS, DAQmx_Val_Volts, NULL));

  printf("Voltage Chan created!\n");

  DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 10000.0, 
                                    DAQmx_Val_Rising, 
                                    DAQmx_Val_FiniteSamps, 
                                    1000));

  // DAQmx Start Code
  DAQmxErrChk(DAQmxStartTask(taskHandle));

  // DAQmx Read Code
  DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1000, 10.0, 
                                 DAQmx_Val_GroupByChannel, 
                                 data, 1000, &read, NULL));

  // Stop and clear task

Error:
  if( DAQmxFailed(error) ){
    DAQmxGetExtendedErrorInfo(errBuff,2048);
  }
  if( taskHandle != 0 )  {
    DAQmxStopTask(taskHandle);
    DAQmxClearTask(taskHandle);
  }
  if( DAQmxFailed(error) ){
    printf("DAQmx Error: %s\n",errBuff);
    return 0;
  }


  printf("NIDAQ testing complete!\n");
  return 0;
}


