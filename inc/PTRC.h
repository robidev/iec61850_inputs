#ifndef PTRC_H_
#define PTRC_H_

#include "iec61850_model_extensions.h"
#include "inputs_api.h"


#ifdef __cplusplus
extern "C" {
#endif

void PTRC_callback(void* param);

void PTRC_init(Input* input);

#ifdef __cplusplus
}
#endif


#endif /* PTRC_H_ */