#ifndef LNS_H_
#define LNS_H_

#include "iec61850_model_extensions.h"


#ifdef __cplusplus
extern "C" {
#endif

void attachLogicalNodes(IedServer server, IedModel_extensions* model);

#ifdef __cplusplus
}
#endif


#endif /* LNS_H_ */

