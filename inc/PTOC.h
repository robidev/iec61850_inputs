#ifndef PTOC_H_
#define PTOC_H_

#include "iec61850_model_extensions.h"
#include "inputs_api.h"


#ifdef __cplusplus
extern "C" {
#endif

void PTOC_init(IedServer server, Input* input);

#ifdef __cplusplus
}
#endif


#endif /* PTOC_H_ */