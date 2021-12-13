
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
#define CHANNEL_NAME "mychan"

int main(int argc, char** argv){

  printf("Using device: [%s]\n", DAQ_DEVICE);

  int32       error = 0;
  TaskHandle  taskHandle = 0;
  char        errBuff[2048]={'\0'};
  uInt32 year, month, day, hour, minute;

  printf("Starting task creation!\n");

  DAQmxErrChk (DAQmxCreateTask(DAQ_TASK_NAME, &taskHandle));

  printf("Task created!\n");

  DAQmxErrChk(DAQmxGetSelfCalLastDateAndTime(DAQ_DEVICE, &year, &month, &day, &hour, &minute));

  printf("Last Device Self Calibration: year: %u month: %u day: %u hour: %u, min: %u\n", year, month, day, hour, minute);

  printf("Calibrating...\n");

  DAQmxSelfCal(DAQ_DEVICE);

  printf("Done Calibrating! \n");

  DAQmxErrChk(DAQmxGetSelfCalLastDateAndTime(DAQ_DEVICE, &year, &month, &day, &hour, &minute));

  printf("New Last Device Self Calibration: year: %u month: %u day: %u hour: %u, min: %u\n", year, month, day, hour, minute);

  // DAQmx Stop and clear task
Error:
  if( DAQmxFailed(error) ){
    DAQmxGetExtendedErrorInfo(errBuff,2048);
  }
  if( taskHandle != 0 )  {
    printf("NIDAQ task complete!\n");

    DAQmxStopTask(taskHandle);
    DAQmxClearTask(taskHandle);

  }
  if( DAQmxFailed(error) ){
    printf("DAQmx Error: %s\n",errBuff);
    return 0;
  }

  return 0;
}


