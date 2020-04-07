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

void attachLogicalNodes(IedServer server, IedModel_extensions* model)
{
  //iterate over struct that attaches model-instances to LogicalNode Classes
  LogicalNodeClass* lnClass = model->logicalNodes;
  while(lnClass != NULL)
  {
    Input* input = getInput( model, lnClass->parent); 
    if(strcmp(lnClass->lnClass,"LLN0") == 0)
    {
      printf("Found mandatory Class LLN0\n");
    }
    else if(strcmp(lnClass->lnClass,"LPHD") == 0)
    {
      printf("Found mandatory Class LPHD\n");
    }
    else if(strcmp(lnClass->lnClass,"XSWI") == 0)
    {
      XSWI_init(server, input); // call init, to attach input-nodes of this instance to callback-items
    }
    else if(strcmp(lnClass->lnClass,"XCBR") == 0)
    {
      XCBR_init(server, input);
    }
    else if(strcmp(lnClass->lnClass,"RADR") == 0)
    {
      RADR_init(input);
    }
    else if(strcmp(lnClass->lnClass,"PTRC") == 0)
    {
      PTRC_init(server, input);
    }
    else if(strcmp(lnClass->lnClass,"PTOC") == 0)
    {
      PTOC_init(server, input);
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


