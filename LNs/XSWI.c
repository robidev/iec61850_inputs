#include "iec61850_model_extensions.h"
#include "iec61850_server.h"
#include "inputs_api.h"
#include "hal_thread.h"
#include "XSWI.h"

//process simulator
void XSWI_simulate_switch(Input* input);

typedef struct sXSWI
{
  IedServer server;
  DataAttribute* Pos_stVal;
  bool conducting;
} XSWI;


//open the circuit breaker(i.e. make it isolating)
void XSWI_open(XSWI * inst)
{
  inst->conducting = false;
}

//close the circuit breaker switch(i.e. make it conducting)
void XSWI_close(XSWI * inst)
{
  inst->conducting = true;
}

//callback for open/close signal from GOOSE-> will trigger process simulator threat
void XSWI_callback(InputEntry* extRef )
{
  //only one type of extref is expected: ctlVal
  bool state = MmsValue_getBoolean(extRef->value);
  if(state == true)
    XSWI_open(extRef->callBackParam);
  else
    XSWI_close(extRef->callBackParam);
}

//initialise XSWI instance for process simulation, and publish/subscription of GOOSE
void XSWI_init(IedServer server, Input* input)
{
  XSWI* inst = (XSWI *) malloc(sizeof(XSWI));//create new instance with MALLOC
  inst->server = server;
  inst->Pos_stVal = (DataAttribute*) ModelNode_getChild((ModelNode*) input->parent, "Pos.stVal");
  inst->conducting = true;


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
  //start simulation threat
  Thread thread = Thread_create((ThreadExecutionFunction)XSWI_simulate_switch, input, true);
  Thread_start(thread);
}

void XSWI_change_switch(XSWI * inst, Dbpos value)
{
  IedServer_updateDbposValue(inst->server,inst->Pos_stVal,value);
}

//threath for process-simulation: open/close switch
void XSWI_simulate_switch(Input* input)
{
  XSWI* inst = input->extRefs->callBackParam;//take the initial callback, as they all contain the same object instance

  inst->conducting = true;//initial state
  XSWI_change_switch(inst,DBPOS_ON);//initial state
  while(1)
  {
    while(inst->conducting == true){ Thread_sleep(1000); }
    printf("XSWI: opening, ZZZZT\n");
    XSWI_change_switch(inst,DBPOS_INTERMEDIATE_STATE);

    Thread_sleep(2000);

    printf("XSWI: opened\n");
    XSWI_change_switch(inst,DBPOS_OFF);

    while(inst->conducting == false){ Thread_sleep(2000); }
    printf("XSWI: closing\n");
    XSWI_change_switch(inst,DBPOS_INTERMEDIATE_STATE);

    Thread_sleep(2000);

    printf("XSWI: closed\n");
    XSWI_change_switch(inst,DBPOS_ON);
  }
}