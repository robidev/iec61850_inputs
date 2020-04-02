/*
 *  server_example_goose.c
 *
 *  This example demonstrates how to use GOOSE publishing, Reporting and the
 *  control model.
 *
 */

#include "inputs_api.h"
#include "iec61850_model_extensions.h"
#include "iec61850_dynamic_model_extensions.h"

#include "iec61850_server.h"
#include "hal_thread.h" /* for Thread_sleep() */
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "static_model.h"

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;
extern IedModel_extensions iedExtendedModel;

IedModel_extensions*
ConfigFileParser_createModelFromConfigFileEx_inputs(const char* filename);

static int running = 0;
static IedServer iedServer = NULL;

void sigint_handler(int signalId)
{
	running = 0;
}

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

int main(int argc, char** argv) {

	iedServer = IedServer_create(&iedModel);

	IedModel_extensions* iedInputModel2 = ConfigFileParser_createModelFromConfigFileEx_inputs("config.cfg");

	GooseReceiver GSEreceiver = GooseReceiver_create();

    SVReceiver SMVreceiver = SVReceiver_create();

	char* ethernetIfcID = "eth0";

	if (argc > 1) {
		ethernetIfcID = argv[1];

		printf("Using GOOSE interface: %s\n", ethernetIfcID);

		/* set GOOSE interface for all GOOSE publishers (GCBs) */
		IedServer_setGooseInterfaceId(iedServer, ethernetIfcID);
		//goose subscriber
		GooseReceiver_setInterfaceId(GSEreceiver, ethernetIfcID);
		SVReceiver_setInterfaceId(SMVreceiver, ethernetIfcID);
	}

	//subscribe to datasets and local DA's based on iput/extRef, and generate one list with all inputValues
	LinkedList allInputValues = subscribeToGOOSEInputs(iedInputModel2, GSEreceiver);
	LinkedList temp = allInputValues;
	temp = LinkedList_getLastElement(temp);
	temp->next = subscribeToSMVInputs(iedInputModel2, SMVreceiver);
	temp = LinkedList_getLastElement(temp);
	temp->next = subscribeToLocalDAInputs(iedInputModel2, &iedModel,iedServer);

    GooseReceiver_start(GSEreceiver);
    SVReceiver_start(SMVreceiver);

    if (GooseReceiver_isRunning(GSEreceiver) || SVReceiver_isRunning(SMVreceiver)) 
	{
		printf("receivers working...\n");
	}
	/* MMS server will be instructed to start listening to client connections. */
	IedServer_start(iedServer, 8102);

	if (!IedServer_isRunning(iedServer)) {
		printf("Starting server failed! Exit.\n");
		IedServer_destroy(iedServer);
		exit(-1);
	}

	/* Start GOOSE publishing */
	IedServer_enableGoosePublishing(iedServer);

	running = 1;

	InputEntry* ee = iedInputModel2->inputs->extRefs;
	char aa[] = "adr_smv3";
	while(ee != NULL)
	{
		if(strcmp(ee->intAddr,aa) == 0)
		{
			break;
		}
		ee = ee->sibling;
	}
	

	signal(SIGINT, sigint_handler);

	while (running) {
		if(ee->value != NULL)
		{
			MmsValue * stVal = MmsValue_getElement(ee->value,0);
			uint8_t tempBuf[20];
			Conversions_msTimeToGeneralizedTime2(MmsValue_getUtcTimeInMs(MmsValue_getElement(ee->value,2)), tempBuf);
			printf("val :%lld, q: %08X, time: %s\n",                    
				(long long) MmsValue_toInt64(stVal) , 
				MmsValue_toUint32(MmsValue_getElement(ee->value,1)) ,
            	tempBuf);
		}
		else
		{
			printf("no value received yet\n");
		}
		
		Thread_sleep(590);
	}

	/* stop MMS server - close TCP server socket and all client sockets */
	IedServer_stop(iedServer);

	/* Cleanup - free all resources */
	IedServer_destroy(iedServer);

	GooseReceiver_stop(GSEreceiver);

    GooseReceiver_destroy(GSEreceiver);

	SVReceiver_stop(SMVreceiver);
     /* Cleanup and free resources */
    SVReceiver_destroy(SMVreceiver);
} /* main() */
