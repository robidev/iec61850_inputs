#ifndef CSWI_H_
#define CSWI_H_

#include "iec61850_model_extensions.h"
#include "inputs_api.h"


#ifdef __cplusplus
extern "C" {
#endif

void CSWI_callback(void* param);

void CSWI_init(Input* input);

#ifdef __cplusplus
}
#endif


#endif /* CSWI_H_ */