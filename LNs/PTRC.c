#include "iec61850_model_extensions.h"
#include "inputs_api.h"

void PTRC_callback(void* param)
{
  //check if value is outside allowed band
  //if so send GOOSE with Tr command
}

void PTRC_init(Input* input)
{
  //register callback for input
}