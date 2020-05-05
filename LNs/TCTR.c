#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "TCTR.h"

#include "iec61850_server.h"
#include "hal_thread.h"
#include <sys/socket.h> 

typedef void (*simulationFunction) (int sd, char * buffer, void* param);

typedef struct sTCTR
{
  simulationFunction call_simulation; //as long as we place the function on top, it can be recast into a generic struct(TODO: make this nicer)
  void *da;
  IedServer server;
  void * da_callback;
} TCTR;

void TCTR_updateValue(int sd, char * buffer, void* param)
{
  printf("TCTR buf= %s\n",buffer);
  TCTR* inst = (TCTR *)param;
  // TODO update datamodel value
  MmsValue* simValue = MmsValue_newIntegerFromInt32(0);
  IedServer_updateAttributeValue(inst->server,inst->da,simValue);
  InputValueHandleExtensionCallbacks(inst->da_callback); //update the associated callbacks with this Data Element
  MmsValue_delete(simValue);

  if( send(sd, "OK\n", 3, 0) != 3 ) { 
		perror("send"); 
	} 
}

void *TCTR_init(IedServer server, Input* input, LinkedList allInputValues )
{
  TCTR* inst = (TCTR *) malloc(sizeof(TCTR));//create new instance with MALLOC
  inst->server = server;
  inst->da = (DataAttribute*) ModelNode_getChild((ModelNode*) input->parent, "Amp.instMag.i");//the node to operate on
  inst->da_callback = _findAttributeValueEx(inst->da, allInputValues);

  //register callback for input
  inst->call_simulation = TCTR_updateValue;
  return inst;
}