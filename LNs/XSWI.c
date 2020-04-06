#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "hal_thread.h"

void XSWI_simulate_switch(InputValue* input);
//callback for open/close signal from GOOSE-> will trigger process threat
void XSWI_open(int * switch_open)
{
  *switch_open = true;
}

void XSWI_close(int * switch_open)
{
  *switch_open = false;
}

void XSWI_callback(InputEntry* extRef)
{
  //only one type of extref is expected: ctlVal
  int state = MmsValue_toUint32(extRef->value);
  if(state == 1)
    XSWI_open(extRef->callBackParam);
  else
    XSWI_close(extRef->callBackParam);
}

void XSWI_init(Input* input)
{
  int* inst = malloc(sizeof(int));//create new instance with MALLOC

  if(input != NULL)
  {
    InputEntry* extref = input->extRefs;
    while(extref != NULL)
    {
      //register callbacks for GOOSE-subscription
      extref->callBack = (callBackFunction) XSWI_callback;
      extref->callBackParam = inst;//pass instance in param

      extref = extref->sibling;
    }
  }
  //TODO start simulation threat
  Thread thread = Thread_create((ThreadExecutionFunction)XSWI_simulate_switch, input, true);
  Thread_start(thread);
}

//threath for process-simulation: open/close switch
void XSWI_simulate_switch(InputValue* input)
{
  int *switch_open = input->extRef->callBackParam;
  while(1)
  {
    while(*switch_open == false){ Thread_sleep(100); }
    printf("XSWI: opening\n");
    //stVal = 00
    //send GOOSE stVal
    Thread_sleep(2000);
    printf("XSWI: opened\n");
    //stVal = 01
    while(*switch_open == true){ Thread_sleep(100); }
    printf("XSWI: closing\n");
    //stVal = 00
    //send GOOSE stVal
    Thread_sleep(2000);
    printf("XSWI: closed\n");
    //stVal = 10
    //send GOOSE stVal
  }
}