#include "iec61850_model_extensions.h"
#include "iec61850_server.h"
#include "inputs_api.h"
#include "hal_thread.h"
#include "XCBR.h"
//process simulator
void XCBR_simulate_switch(Input* input);

typedef struct sXCBR
{
  IedServer server;
  DataAttribute* Pos_stVal;
  bool conducting;
} XCBR;


//open the circuit breaker(i.e. make it isolating)
void XCBR_open(XCBR * inst)
{
  inst->conducting = false;
}

//close the circuit breaker switch(i.e. make it conducting)
void XCBR_close(XCBR * inst)
{
  inst->conducting = true;
}

//callback for open/close signal from GOOSE-> will trigger process simulator threat
void XCBR_callback(InputEntry* extRef )
{
  //only one type of extref is expected: ctlVal
  bool state = MmsValue_getBoolean(extRef->value);
  if(state == true)
    XCBR_open(extRef->callBackParam);
  else
    XCBR_close(extRef->callBackParam);
}

//initialise XCBR instance for process simulation, and publish/subscription of GOOSE
void XCBR_init(IedServer server, Input* input)
{
  XCBR* inst = (XCBR *) malloc(sizeof(XCBR));//create new instance with MALLOC
  inst->server = server;
  inst->Pos_stVal = (DataAttribute*) ModelNode_getChild((ModelNode*) input->parent, "Pos.stVal");
  inst->conducting = true;


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
  //start simulation threat
  Thread thread = Thread_create((ThreadExecutionFunction)XCBR_simulate_switch, input, true);
  Thread_start(thread);
}

void XCBR_change_switch(XCBR * inst, Dbpos value)
{
  IedServer_updateDbposValue(inst->server,inst->Pos_stVal,value);
}

//threath for process-simulation: open/close switch
void XCBR_simulate_switch(Input* input)
{
  XCBR* inst = input->extRefs->callBackParam;//take the initial callback, as they all contain the same object instance

  inst->conducting = true;//initial state
  XCBR_change_switch(inst,DBPOS_ON);//initial state
  while(1)
  {
    while(inst->conducting == true){ Thread_sleep(10); }
    printf("XCBR: opening, BANG!\n");
    XCBR_change_switch(inst,DBPOS_INTERMEDIATE_STATE);

    Thread_sleep(20);

    printf("XCBR: opened\n");
    XCBR_change_switch(inst,DBPOS_OFF);

    while(inst->conducting == false){ Thread_sleep(1000); }
    printf("XCBR: closing\n");
    XCBR_change_switch(inst,DBPOS_INTERMEDIATE_STATE);

    Thread_sleep(2000);

    printf("XCBR: closed\n");
    XCBR_change_switch(inst,DBPOS_ON);
  }
}