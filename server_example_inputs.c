/*
 *  server_example_inputs.c
 *
 *  This example demonstrates how to use inputs, GOOSE and SMV subscribing.
 *
 */

#include "inputs_api.h"
#include "iec61850_model_extensions.h"
#include "iec61850_dynamic_model_extensions.h"
#include "LNParse.h"

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

int main(int argc, char** argv) {

	iedServer = IedServer_create(&iedModel);

	//IedModel* iedModel_dynamic = ConfigFileParser_createModelFromConfigFileEx("config.cfg");
	//IedModel_extensions* iedInputModel_dynamic = ConfigFileParser_createModelFromConfigFileEx_inputs("model.cfg");

	GooseReceiver GSEreceiver = GooseReceiver_create();
    SVReceiver SMVreceiver = SVReceiver_create();

	char* ethernetIfcID = "eth0";

	if (argc > 1) {
		ethernetIfcID = argv[1];

		printf("Using interface: %s\n", ethernetIfcID);

		/* set GOOSE interface for all GOOSE publishers (GCBs) */
		IedServer_setGooseInterfaceId(iedServer, ethernetIfcID);
		//goose subscriber
		GooseReceiver_setInterfaceId(GSEreceiver, ethernetIfcID);
		SVReceiver_setInterfaceId(SMVreceiver, ethernetIfcID);
	}

	//subscribe to datasets and local DA's based on iput/extRef, and generate one list with all inputValues
	LinkedList allInputValues = subscribeToGOOSEInputs(&iedExtendedModel, GSEreceiver);
	LinkedList temp = allInputValues;
	temp = LinkedList_getLastElement(temp);
	temp->next = subscribeToSMVInputs(&iedExtendedModel, SMVreceiver);
	temp = LinkedList_getLastElement(temp);
	temp->next = subscribeToLocalDAInputs(&iedExtendedModel, &iedModel,iedServer);

	//start subscribers
    GooseReceiver_start(GSEreceiver);
    SVReceiver_start(SMVreceiver);

    if (GooseReceiver_isRunning(GSEreceiver) || SVReceiver_isRunning(SMVreceiver)) 
	{
		printf("receivers working...\n");
	}
	else
	{
		printf("WARNING: no receivers are running\n");
	}
	

	/* MMS server will be instructed to start listening to client connections. */
	IedServer_start(iedServer, 102);

	if (!IedServer_isRunning(iedServer)) {
		printf("Starting server failed! Exit.\n");
		IedServer_destroy(iedServer);
		exit(-1);
	}

	//call all initializers for logical nodes in the model
	attachLogicalNodes(&iedExtendedModel);

	/* Start GOOSE publishing */
	IedServer_enableGoosePublishing(iedServer);

	running = 1;

	signal(SIGINT, sigint_handler);

	while (running) {
		Thread_sleep(1000);
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
