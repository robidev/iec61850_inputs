#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "LNParse.h"
#include "XSWI.h"
#include "XCBR.h"
#include "RADR.h"
#include "PTRC.h"
#include "PTOC.h"
#include "MMXU.h"
#include "CSWI.h"
#include "CILO.h"
/*
LN's:

XSWI
XCBR
RADR
PTRC
PTOC
MMXU
CSWI
CILO
*/

void attachLogicalNodes(IedModel_extensions* model)
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
      XCBR_init(input);
    }
    else if(strcmp(lnClass->lnClass,"RADR") == 0)
    {
      RADR_init(input);
    }
    else if(strcmp(lnClass->lnClass,"PTRC") == 0)
    {
      PTRC_init(input);
    }
    else if(strcmp(lnClass->lnClass,"PTOC") == 0)
    {
      PTOC_init(input);
    }
    else if(strcmp(lnClass->lnClass,"MMXU") == 0)
    {
      MMXU_init(input);
    }
    else if(strcmp(lnClass->lnClass,"CSWI") == 0)
    {
      CSWI_init(input);
    }
    else if(strcmp(lnClass->lnClass,"CILO") == 0)
    {
      CILO_init(input);
    }
    else
    {
      printf("ERROR: Class not supported\n");
    }
    lnClass = lnClass->sibling;
  }
  
}


