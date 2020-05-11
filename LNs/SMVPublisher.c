#include "iec61850_server.h"
#include "SMVPublisher.h"

#include "iec61850_model.h"
#include "iec61850_model_extensions.h"
#include "inputs_api.h"
#include "simulation_config.h"

#include "hal_thread.h"
#include <math.h>


typedef struct sSMVP {
    int svcbEnabled;
    SVPublisher_ASDU asdu;
    SVPublisher svPublisher;
    bool running;

    void * simulationHandler;
    LinkedList dataSetValues;
} SMVP;

void SMV_Thread(SMVP* inst);

void sVCBEventHandler (SVControlBlock* svcb, int event, void* parameter)
{
    SMVP* inst = (SMVP*)parameter;
    if (event == IEC61850_SVCB_EVENT_ENABLE)
        inst->svcbEnabled = 1;
    else if (event == IEC61850_SVCB_EVENT_DISABLE)
        inst->svcbEnabled = 0;
}

void* SMVP_init(SVPublisher SMVPublisher, SVControlBlock* svcb, IedServer server)
{
    SMVP* inst = (SMVP *) malloc(sizeof(SMVP));
    inst->running = false;

    inst->svPublisher = SMVPublisher;

    if (inst->svPublisher == NULL) {
        printf("ERROR: could not create sampled value publisher");
        return NULL;
    }
    

    //SVControlBlock* svcb = IedModel_getSVControlBlock(model, logicalNode, svcbName);//todo merge with calling func.

    //TODO check if dataSet-arg is at the right pos.
    inst->asdu = SVPublisher_addASDU(inst->svPublisher, svcb->dataSetName, NULL, 1);

    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);
    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);
    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);
    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);

    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);
    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);
    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);
    SVPublisher_ASDU_addINT32(inst->asdu);
    SVPublisher_ASDU_addQuality(inst->asdu);

    SVPublisher_ASDU_setSmpCntWrap(inst->asdu, 4000);
    SVPublisher_ASDU_setRefrTm(inst->asdu, 0);

    SVPublisher_setupComplete(inst->svPublisher);

    IedServer_setSVCBHandler(server, svcb, sVCBEventHandler, inst);  
    inst->svcbEnabled = 1;

    inst->dataSetValues = NULL;
    
    if(IEC61850_server_simulation_type() == SIMULATION_TYPE_REMOTE)
    {
        //get domain
        char objectReference[130];
        ModelNode_getObjectReference((ModelNode*) svcb->parent, objectReference);
        char* separator = strchr(objectReference, '/');
        *separator = 0;

        //form dataset name
        char* lnName = svcb->parent->name;
        char* dataSetReference = StringUtils_createString(5, objectReference, "/", lnName, "$", svcb->dataSetName);

        /* prepare data set values */
        inst->dataSetValues = LinkedList_create();
        IedModel* model = IedServer_getDataModel(server);
        DataSet* dataSet = IedModel_lookupDataSet(model, dataSetReference);
        DataSetEntry* dataSetEntry = dataSet->fcdas;
        while (dataSetEntry != NULL) {
            LinkedList_add(inst->dataSetValues, dataSetEntry->value);
            dataSetEntry = dataSetEntry->sibling;
        }
    }

    Thread thread = Thread_create((ThreadExecutionFunction)SMV_Thread, inst, true);
    Thread_start(thread);

    return inst;  
}

void SMVP_destroy(SMVP* inst)
{
    SVPublisher_destroy(inst->svPublisher);
}

void SMV_Thread(SMVP* inst)
{
    inst->running = true;
    int voltageA;
    int voltageB;
    int voltageC;
    int voltageN;
    int currentA;
    int currentB;
    int currentC;
    int currentN;

    if(IEC61850_server_simulation_type() == SIMULATION_TYPE_LOCAL)
    {
        Quality q = QUALITY_VALIDITY_GOOD;

        int vol = (int) (6350.f * sqrt(2));
        int amp = 0;
        float phaseAngle = 0.f;

        int sampleCount = 0;

        uint64_t nextCycleStart = Hal_getTimeInMs() + 1000;
        while (inst->running) 
        {
                        /* update measurement values */
            int samplePoint = sampleCount % 80;

            double angleA = (2 * M_PI / 80) * samplePoint;
            double angleB = (2 * M_PI / 80) * samplePoint - ( 2 * M_PI / 3);
            double angleC = (2 * M_PI / 80) * samplePoint - ( 4 * M_PI / 3);

            voltageA = (vol * sin(angleA)) * 100;
            voltageB = (vol * sin(angleB)) * 100;
            voltageC = (vol * sin(angleC)) * 100;
            voltageN = voltageA + voltageB + voltageC;

            currentA = (amp * sin(angleA - phaseAngle)) * 1000;
            currentB = (amp * sin(angleB - phaseAngle)) * 1000;
            currentC = (amp * sin(angleC - phaseAngle)) * 1000;
            currentN = currentA + currentB + currentC;

            if (inst->svcbEnabled) {
                
                SVPublisher_ASDU_setINT32(inst->asdu, 0, currentA);
                SVPublisher_ASDU_setQuality(inst->asdu, 4, q);
                SVPublisher_ASDU_setINT32(inst->asdu, 8, currentB);
                SVPublisher_ASDU_setQuality(inst->asdu, 12, q);
                SVPublisher_ASDU_setINT32(inst->asdu, 16, currentC);
                SVPublisher_ASDU_setQuality(inst->asdu, 20, q);
                SVPublisher_ASDU_setINT32(inst->asdu, 24, currentN);
                SVPublisher_ASDU_setQuality(inst->asdu, 28, q);

                SVPublisher_ASDU_setINT32(inst->asdu, 32, voltageA);
                SVPublisher_ASDU_setQuality(inst->asdu, 36, q);
                SVPublisher_ASDU_setINT32(inst->asdu, 40, voltageB);
                SVPublisher_ASDU_setQuality(inst->asdu, 44, q);
                SVPublisher_ASDU_setINT32(inst->asdu, 48, voltageC);
                SVPublisher_ASDU_setQuality(inst->asdu, 52, q);
                SVPublisher_ASDU_setINT32(inst->asdu, 56, voltageN);
                SVPublisher_ASDU_setQuality(inst->asdu, 60, q);

                SVPublisher_ASDU_setRefrTm(inst->asdu, Hal_getTimeInMs());

                SVPublisher_ASDU_setSmpCnt(inst->asdu, (uint16_t) sampleCount);

                SVPublisher_publish(inst->svPublisher);
            }

            sampleCount = ((sampleCount + 1) % 4000);

            if ((sampleCount % 400) == 0) {
                uint64_t timeval = Hal_getTimeInMs();

                while (timeval < nextCycleStart + 100) {
                    Thread_sleep(1);

                    timeval = Hal_getTimeInMs();
                }

                nextCycleStart = nextCycleStart + 100;
            }
        }
    }
    else // if(IEC61850_server_simulation_type() == SIMULATION_TYPE_NONE || IEC61850_server_simulation_type() == SIMULATION_TYPE_REMOTE)
    {
        int sampleCount = 0;
        //Quality q = QUALITY_VALIDITY_GOOD;
        uint64_t nextCycleStart = Hal_getTimeInMs() + 1000;

        int step = 0;

        while (inst->running) 
        {
            int samplePoint = sampleCount % 80;
            if (inst->svcbEnabled) {
                
                LinkedList ds = inst->dataSetValues;
                int index = 0;
                while(ds != NULL)//for each LN with an inputs/extref defined;
                {
                    if(ds->data != NULL)
                    {
                        MmsValue* datasetValue = ds->data;
                        char buf[255];
                        MmsValue_printToBuffer(datasetValue,buf,255);
                        printf("data: %s\n",buf);

                        SVPublisher_ASDU_setINT32(inst->asdu, index, MmsValue_toInt32( MmsValue_getElement( MmsValue_getElement(datasetValue,0), 0) ) );
                        SVPublisher_ASDU_setQuality(inst->asdu, index + 4, MmsValue_toUint32( MmsValue_getElement(datasetValue,1) ));
                        index += 8;
                    }
                    ds = LinkedList_getNext(ds);
                }

                SVPublisher_ASDU_setRefrTm(inst->asdu, Hal_getTimeInMs());

                SVPublisher_ASDU_setSmpCnt(inst->asdu, (uint16_t) sampleCount);

                SVPublisher_publish(inst->svPublisher);
            }

            sampleCount = ((sampleCount + 1) % 4000);

            if(IEC61850_server_simulation_type() == SIMULATION_TYPE_REMOTE)
            {
                IEC61850_server_simulation_sync(step++);
            }
            else
            {
                if ((sampleCount % 400) == 0) {
                    uint64_t timeval = Hal_getTimeInMs();

                    while (timeval < nextCycleStart + 100) {
                        Thread_sleep(1);

                        timeval = Hal_getTimeInMs();
                    }

                    nextCycleStart = nextCycleStart + 100;
                }
            }
        }
    }
}