#include "iec61850_model_input.h"
#include "inputs_api.h"
/*
LN's:

XSWI
XCBR
RADR
PTCR
PTOC
MMXU
CSWI
CILO
*/

void attachLogicalNodes(IedModel_inputs* model)
{
  //iterate over struct that attaches model-instances to LogicalNode Classes
  LogicalNodeClass* lnClass = model->logicalNodes;
  while(lnClass != NULL)
  {
    Input* input = getInput( model, lnClass->parent); 
    if(strcmp(lnClass->lnClass,"XSWI") == 0)
    {
      XSWI_init(input); // call init, to attach input-nodes of this instance to callback-items
    }
    else if(strcmp(lnClass->lnClass,"XCBR") == 0)
    {

    }
    else if(strcmp(lnClass->lnClass,"RADR") == 0)
    {

    }
    else if(strcmp(lnClass->lnClass,"PTCR") == 0)
    {

    }
    else if(strcmp(lnClass->lnClass,"PTOC") == 0)
    {

    }
    else if(strcmp(lnClass->lnClass,"MMXU") == 0)
    {

    }
    else if(strcmp(lnClass->lnClass,"CSWI") == 0)
    {

    }
    else if(strcmp(lnClass->lnClass,"CILO") == 0)
    {

    }
    else
    {
      printf("ERROR: Class not supported");
    }
    lnClass = lnClass->sibling;
  }
  
}


