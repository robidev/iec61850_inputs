#ifndef SMVPUBLISHER_H_
#define SMVPUBLISHER_H_

#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "sv_publisher.h"

#ifdef __cplusplus
extern "C" {
#endif

void* SMVP_init(SVPublisher SMVPublisher, IedServer server, IedModel* model, LogicalNode* logicalNode, char* dataSet, char* svcbName);

#ifdef __cplusplus
}
#endif


#endif /* SMVPUBLISHER_H_ */