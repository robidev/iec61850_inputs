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
} TCTR;

void TCTR_updateValue(int sd, char * buffer, void* param)
{
  printf("TCTR buf= %s\n",buffer);
  if( send(sd, "OK\n", 3, 0) != 3 ) { 
		perror("send"); 
	} 
}

void *TCTR_init(Input* input)
{
  TCTR* inst = (TCTR *) malloc(sizeof(TCTR));//create new instance with MALLOC
  //register callback for input
  inst->call_simulation = TCTR_updateValue;
  return inst;
}