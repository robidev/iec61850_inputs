#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "PTRC.h"

typedef struct sPTRC
{
  IedServer server;
  DataAttribute* Tr_general;
  void * Tr_general_callback;
} PTRC;

//receive trip command from input LN's
void PTRC_input_callback(InputEntry* extRef)
{
  PTRC* inst = extRef->callBackParam;

  if(extRef->value != NULL)
  {
    MmsValue* tripValue = MmsValue_newBoolean(true);
    IedServer_updateAttributeValue(inst->server,inst->Tr_general,tripValue);
    InputValueHandleExtensionCallbacks(inst->Tr_general_callback); //update the associated callbacks with this Data Element
    MmsValue_delete(tripValue);
  }
}

//reveice status from circuit breaker
void PTRC_xcbr_callback(InputEntry* extRef)
{
  PTRC* inst = extRef->callBackParam;

  if(extRef->value != NULL)
  {
    char printBuf[1024];

    MmsValue_printToBuffer(extRef->value, printBuf, 1024);
    printf("PTRC: Received Breaker position: %s\n", printBuf);
  }
}

void PTRC_init(IedServer server, LogicalNode* ln, Input* input, LinkedList allInputValues)
{
  PTRC* inst = (PTRC *) malloc(sizeof(PTRC));//create new instance with MALLOC
  inst->server = server;
  inst->Tr_general = (DataAttribute*) ModelNode_getChild((ModelNode*) ln, "Tr.general");//the node to operate on
  inst->Tr_general_callback = _findAttributeValueEx(inst->Tr_general, allInputValues);
 
  //find extref for the last SMV, using the intaddr
  if(input != NULL)
  {
    InputEntry* extRef = input->extRefs;

    while(extRef != NULL)
    {
      if(strcmp(extRef->intAddr,"PTOC_Op") == 0)
      {
        extRef->callBack = (callBackFunction) PTRC_input_callback;
        extRef->callBackParam = inst;
      }
      if(strcmp(extRef->intAddr,"RREC_Op") == 0)
      {
        extRef->callBack = (callBackFunction) PTRC_input_callback;
        extRef->callBackParam = inst;
      }
      if(strcmp(extRef->intAddr,"xcbr_stval") == 0)
      {
        extRef->callBack = (callBackFunction) PTRC_xcbr_callback;
        extRef->callBackParam = inst;
      }
      extRef = extRef->sibling;
    }
  }
}