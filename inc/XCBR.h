#ifndef XCBR_H_
#define XCBR_H_

#include "iec61850_model_extensions.h"
#include "inputs_api.h"


#ifdef __cplusplus
extern "C" {
#endif

void XCBR_callback(void* param);

void XCBR_init(Input* input);

#ifdef __cplusplus
}
#endif


#endif /* XCBR_H_ */