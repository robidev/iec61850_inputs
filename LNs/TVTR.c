#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "TVTR.h"

#include "iec61850_server.h"
#include "hal_thread.h"
#include <sys/socket.h> 

typedef void (*simulationFunction) (int sd, char * buffer, void* param);

typedef struct sTVTR
{
  simulationFunction call_simulation; //as long as we place the function on top, it can be recast into a generic struct(TODO: make this nicer)
} TVTR;

void TVTR_updateValue(int sd, char * buffer, void* param)
{
  printf("TVTR buf= %s\n",buffer);
  if( send(sd, "OK\n", 3, 0) != 3 ) { 
		perror("send"); 
	} 
}

void *TVTR_init(Input* input)
{
  TVTR* inst = (TVTR *) malloc(sizeof(TVTR));//create new instance with MALLOC
  //register callback for input
  inst->call_simulation = TVTR_updateValue;
  return inst;
}