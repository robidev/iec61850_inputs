#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "hal_thread.h"
#include <time.h>

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


//callback when SMV is received
void PTOC_callback(InputEntry* extRef)
{
  //InputEntry* extRef = (InputEntry*) param;
  if(extRef->value != NULL)
  {
    MmsValue * stVal = MmsValue_getElement(extRef->value,0);
    uint8_t tempBuf[20];
    Conversions_msTimeToGeneralizedTime2(MmsValue_getUtcTimeInMs(MmsValue_getElement(extRef->value,2)), tempBuf);
    printf("val :%lld, q: %08X, time: %s\n", (long long) MmsValue_toInt64(stVal), MmsValue_toUint32(MmsValue_getElement(extRef->value,1)), tempBuf);
  }
  //check if value is outside allowed band
  //if so send to internal PTRC
}

void PTOC_init(Input* input)
{
  //find extref for the last SMV, using the intaddr
  InputEntry* extRef = input->extRefs;
	char lastElement[] = "Vol4";
	while(extRef != NULL)
	{
		if(strcmp(extRef->intAddr,lastElement) == 0)
		{
			break;
		}
		extRef = extRef->sibling;
	}
  //register the callback
  if(extRef != NULL)
  {
    extRef->callBack = (callBackFunction) PTOC_callback;
    extRef->callBackParam = NULL;
  }
}
