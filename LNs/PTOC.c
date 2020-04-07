#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "hal_thread.h"
#include "../static_model.h"
#include "PTOC.h"

#include <time.h>

typedef struct sPTOC
{
  IedServer server;
  DataAttribute* Op_general;
} PTOC;

void
Conversions_intToStringBuffer2(int intValue, int numberOfDigits, uint8_t* buffer)
{
    int digitBase = 1;

    int i = 1;

    while (i < numberOfDigits) {
        digitBase = digitBase * 10;
        i++;
    }

    int remainder = intValue;

    for (i = 0; i < numberOfDigits; i++) {
        int digit = remainder / digitBase;

        buffer[i] = (uint8_t) (digit + 48);

        remainder = remainder % digitBase;

        digitBase = digitBase / 10;
    }

    buffer[i] = 0;
}

void
Conversions_msTimeToGeneralizedTime2(uint64_t msTime, uint8_t* buffer)
{
    int msPart = (msTime % 1000);

    time_t unixTime = (msTime / 1000);

    struct tm tmTime;

    gmtime_r(&unixTime, &tmTime);

    Conversions_intToStringBuffer2(tmTime.tm_year + 1900, 4, buffer);

    Conversions_intToStringBuffer2(tmTime.tm_mon + 1, 2, buffer + 4);
    Conversions_intToStringBuffer2(tmTime.tm_mday, 2, buffer + 6);
    Conversions_intToStringBuffer2(tmTime.tm_hour, 2, buffer + 8);
    Conversions_intToStringBuffer2(tmTime.tm_min, 2, buffer + 10);
    Conversions_intToStringBuffer2(tmTime.tm_sec, 2, buffer + 12);

    buffer[14] = '.';

    Conversions_intToStringBuffer2(msPart, 3, buffer + 15);

    buffer[18] = 'Z';

    buffer[19] = 0;
}

//callback when GOOSE is received
void PTOC_callback_GOOSE(InputEntry* extRef)
{
  PTOC* inst = extRef->callBackParam;

  if(extRef->value != NULL)
  {
    char printBuf[1024];

    MmsValue_printToBuffer(extRef->value, printBuf, 1024);
    printf("Breaker position: %s\n", printBuf);
  }
}

//callback when SMV is received
void PTOC_callback_SMV(InputEntry* extRef)
{
  PTOC* inst = extRef->callBackParam;

  if(extRef->value != NULL)
  {
    MmsValue * stVal = MmsValue_getElement(extRef->value,0);
    uint8_t tempBuf[20];
    Conversions_msTimeToGeneralizedTime2(MmsValue_getUtcTimeInMs(MmsValue_getElement(extRef->value,2)), tempBuf);
    printf("val :%lld, q: %08X, time: %s\n", (long long) MmsValue_toInt64(stVal), MmsValue_toUint32(MmsValue_getElement(extRef->value,1)), tempBuf);
    //check if value is outside allowed band
    if(MmsValue_toInt64(stVal) > 10000){
      MmsValue* tripValue = MmsValue_newBoolean(true);
      IedServer_updateAttributeValue(inst->server,inst->Op_general,tripValue);
      MmsValue_delete(tripValue);
      //if so send to internal PTRC
    }
  }
}

void PTOC_init(IedServer server, Input* input)
{
  PTOC* inst = (PTOC *) malloc(sizeof(PTOC));//create new instance with MALLOC
  inst->server = server;
  inst->Op_general = (DataAttribute*) ModelNode_getChild((ModelNode*) input->parent, "Op.general");//the node to operate on
 
  //find extref for the last SMV, using the intaddr
  InputEntry* extRef = input->extRefs;

	while(extRef != NULL)
	{
		if(strcmp(extRef->intAddr,"Vol4") == 0)
		{
      extRef->callBack = (callBackFunction) PTOC_callback_SMV;
      extRef->callBackParam = inst;
		}
    if(strcmp(extRef->intAddr,"xcbr_stval") == 0)
		{
      extRef->callBack = (callBackFunction) PTOC_callback_GOOSE;
      extRef->callBackParam = inst;
		}
		extRef = extRef->sibling;
	}
}
