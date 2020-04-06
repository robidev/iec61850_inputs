#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "hal_thread.h"

void XCBR_simulate_switch(InputValue* input);

//callback for open/close signal from GOOSE-> will trigger process threat

static char switch_open = 0;

void XCBR_open(void * param)
{
  switch_open = 1;
}

void XCBR_close(void * param)
{
  switch_open = 0;
}

void XCBR_callback(InputEntry* extRef )
{
  //only one type of extref is expected: ctlVal
  bool state = MmsValue_getBoolean(extRef->value);
  if(state == true)
    XCBR_open(extRef->callBackParam);
  else
    XCBR_close(extRef->callBackParam);
}

void XCBR_init(Input* input)
{
  int* inst = malloc(sizeof(int));//create new instance with MALLOC

  if(input != NULL)
  {
    InputEntry* extref = input->extRefs;
    while(extref != NULL)
    {
      //register callbacks for GOOSE-subscription
      extref->callBack = (callBackFunction) XCBR_callback;
      extref->callBackParam = inst;//pass instance in param

      extref = extref->sibling;
    }
  }
  //TODO start simulation threat
  Thread thread = Thread_create((ThreadExecutionFunction)XCBR_simulate_switch, input, true);
  Thread_start(thread);
}

//threath for process-simulation: open/close switch
void XCBR_simulate_switch(InputValue* input)
{
  while(1)
  {
    while(switch_open == 0){ Thread_sleep(10); }
    printf("XCBR: opening\n");
    //stVal = 00
    //send GOOSE stVal
    Thread_sleep(20);
    printf("XCBR: opened\n");
    //stVal = 01
    while(switch_open == 1){ Thread_sleep(10); }
    printf("XCBR: closing\n");
    //stVal = 00
    //send GOOSE stVal
    Thread_sleep(20);
    printf("XCBR: closed\n");
    //stVal = 10
    //send GOOSE stVal
  }
}