#ifndef MMXU_H_
#define MMXU_H_

#include "iec61850_model_extensions.h"
#include "inputs_api.h"


#ifdef __cplusplus
extern "C" {
#endif

void MMXU_callback(void* param);

void MMXU_init(Input* input);

#ifdef __cplusplus
}
#endif


#endif /* MMXU_H_ */