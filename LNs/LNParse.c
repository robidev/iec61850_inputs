#include "iec61850_model_input.h"

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
    //  call init, to attach input-nodes of this instance to callback-items
    lnClass = lnClass->sibling;
  }
  
}