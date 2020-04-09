#include "iec61850_server.h"
#include "SMVPublisher.h"

#include "iec61850_model_extensions.h"
#include "inputs_api.h"

#include "hal_thread.h"
#include <math.h>


typedef struct sSMVP {
    int svcbEnabled;
    SVPublisher_ASDU asdu;
    SVPublisher svPublisher;
    bool running;

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

void* SMVP_init(SVPublisher SMVPublisher, IedServer server, IedModel* model,   LogicalNode* logicalNode, char* dataSet, char* svcbName)
{
    SMVP* inst = (SMVP *) malloc(sizeof(SMVP));
    inst->running = false;

    inst->svPublisher = SMVPublisher;

    if (inst->svPublisher == NULL) {
        printf("ERROR: could not create sampled value publisher");
        return NULL;
    }
    inst->asdu = SVPublisher_addASDU(inst->svPublisher, dataSet, NULL, 1);

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

    SVControlBlock* svcb = IedModel_getSVControlBlock(model, logicalNode, svcbName);

    if (svcb == NULL) {
        printf("ERROR: Lookup svcb failed!\n");
        SVPublisher_destroy(inst->svPublisher);
        return NULL;
    }
    IedServer_setSVCBHandler(server, svcb, sVCBEventHandler, inst);  
    inst->svcbEnabled = 1;
    
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

    Quality q = QUALITY_VALIDITY_GOOD;

    int vol = (int) (6350.f * sqrt(2));
    int amp = 0;
    float phaseAngle = 0.f;

    int voltageA;
    int voltageB;
    int voltageC;
    int voltageN;
    int currentA;
    int currentB;
    int currentC;
    int currentN;

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

        // TODO check datamodel for the values, instead of generating them here. (or copy them from here to those LN's)

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