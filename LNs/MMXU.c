#include "iec61850_model_extensions.h"
#include "inputs_api.h"

typedef struct sMMXU
{
  void *da;
  IedServer server;
  void * da_callback;
} MMXU;

void *MMXU_init(IedServer server, LogicalNode* ln, Input* input, LinkedList allInputValues )
{
  MMXU* inst = (MMXU *) malloc(sizeof(MMXU));//create new instance with MALLOC
  inst->server = server;
  inst->da = (DataAttribute*) ModelNode_getChild((ModelNode*) ln, "AvAPhs.instMag.i");//the node to operate on
  inst->da_callback = _findAttributeValueEx(inst->da, allInputValues);

  //register callback for input
  //inst->call_simulation = TCTR_updateValue;
  return inst;
}