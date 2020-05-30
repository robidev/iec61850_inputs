#include "iec61850_model_extensions.h"
#include "inputs_api.h"

typedef struct sCSWI
{
  IedServer server;
  DataAttribute* Pos_stVal;
  void * Pos_stVal_callback;
} CSWI;


//reveice status from circuit breaker
void CSWI_xcbr_callback(InputEntry* extRef)
{
  CSWI* inst = extRef->callBackParam;

  if(extRef->value != NULL)
  {
    char printBuf[1024];

    MmsValue_printToBuffer(extRef->value, printBuf, 1024);
    printf("CSWI: Received Breaker position: %s\n", printBuf);
  }
}


void CSWI_init(IedServer server, LogicalNode* ln, Input* input, LinkedList allInputValues)
{
  CSWI* inst = (CSWI *) malloc(sizeof(CSWI));//create new instance with MALLOC
  return;
  inst->server = server;
  inst->Pos_stVal = (DataAttribute*) ModelNode_getChild((ModelNode*) ln, "Pos.stVal");//the node to operate on when a operate is triggered
  inst->Pos_stVal_callback = _findAttributeValueEx(inst->Pos_stVal, allInputValues);//find node that this element was subscribed to, so that it will be called during an update
 
  //find extref for the last SMV, using the intaddr
  if(input != NULL)
  {
    InputEntry* extRef = input->extRefs;

    while(extRef != NULL)
    {
      //receive status of associated XCBR
      if(strcmp(extRef->intAddr,"xcbr_stval") == 0)
      {
        extRef->callBack = (callBackFunction) CSWI_xcbr_callback;
        extRef->callBackParam = inst;
      }
      extRef = extRef->sibling;
    }
  }
  //TODO initialise control logic
  //during an operate, a certain element will need to update in the CSWI model(opOk element? ctlVal is not subscribable), to which the XCBR is subscribed (goose or directly)
}

