/*
 *  dynamic_model_input.c
 *
 *  Copyright 2014-2016 Michael Zillgith
 *
 *  This file is part of libIEC61850.
 *
 *  libIEC61850 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libIEC61850 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */

#include "iec61850_model_input.h"
#include "iec61850_dynamic_model_input.h"

#include "iec61850_server.h"
#include "libiec61850_platform_includes.h"
#include "stack_config.h"


IedModel_inputs*
IedModel_input_create()
{
    IedModel_inputs* self = (IedModel_inputs*) GLOBAL_CALLOC(1, sizeof(IedModel_inputs));

    self->inputs = NULL;

    self->subRefs = NULL;

    self->logicalNodes = NULL;

    return self;
}


static void
IedModel_addInput(IedModel_inputs* self, Input* input)
{
    if (self->inputs == NULL)
        self->inputs = input;
    else {
        Input* lastinput = self->inputs;

        while (lastinput != NULL) {
            if (lastinput->sibling == NULL) {
                lastinput->sibling = input;
                break;
            }

            lastinput = lastinput->sibling;
        }
    }
}

static void
IedModel_addLogicalNodeClass(IedModel_inputs* self, LogicalNodeClass* lnClass)
{
    if (self->logicalNodes == NULL)
        self->logicalNodes = lnClass;
    else {
        LogicalNodeClass* lastlnClass = self->logicalNodes;

        while (lastlnClass != NULL) {
            if (lastlnClass->sibling == NULL) {
                lastlnClass->sibling = lnClass;
                break;
            }

            lastlnClass = lastlnClass->sibling;
        }
    }
}



LogicalNodeClass*
LogicalNodeClass_create(LogicalNode* parent, IedModel_inputs* inputs, char * lnClass )
{
    LogicalNodeClass* self = (LogicalNodeClass*) GLOBAL_MALLOC(sizeof(LogicalNodeClass));

    self->parent = parent;

    if(lnClass != NULL)
      self->lnClass = StringUtils_copyString(lnClass);

    self->initFunction = NULL;
    self->sibling = NULL;

    IedModel_addLogicalNodeClass(inputs, self);

    return self;
}

Input*
Input_create(LogicalNode* parent, IedModel_inputs* inputs )
{
    Input* self = (Input*) GLOBAL_MALLOC(sizeof(Input));

    self->parent = parent;
    self->elementCount = 0;
    self->sibling = NULL;
    self->extRefs = NULL;

    IedModel_addInput(inputs, self);

    return self;
}

int
Input_getSize(Input* self)
{
    return self->elementCount;
}

InputEntry*
Input_getFirstEntry(Input* self)
{
    return self->extRefs;
}

InputEntry*
InputEntry_getNext(InputEntry* self)
{
    return self->sibling;
}

static void
Input_addEntry(Input* self, InputEntry* newEntry)
{
    self->elementCount++;

    if (self->extRefs == NULL)
        self->extRefs = newEntry;
    else {
        InputEntry* lastEntry = self->extRefs;

        while (lastEntry != NULL) {

            if (lastEntry->sibling == NULL) {
                lastEntry->sibling = newEntry;
                break;
            }

            lastEntry = lastEntry->sibling;
        }
    }
}

InputEntry*
InputEntry_create(Input* input, const char* desc, const char* Ref, const char* intAddr, const char* serviceType, const char* srcRef)
{
    InputEntry* self = (InputEntry*) GLOBAL_MALLOC(sizeof(InputEntry));

    if (desc != NULL) {
        self->desc = StringUtils_copyString(desc);
    }

    if (Ref != NULL) {
        self->Ref = StringUtils_copyString(Ref);
    }

    if (intAddr != NULL) {
        self->intAddr = StringUtils_copyString(intAddr);
    }    

    if (serviceType != NULL) {
        self->serviceType = StringUtils_copyString(serviceType);
    }

    if (srcRef != NULL) {
        self->srcRef = StringUtils_copyString(srcRef);
    }

    self->value = NULL;

    self->sibling = NULL;

    Input_addEntry(input, self);

    return self;
}

static void
Subscriber_addEntry(IedModel_inputs* self, SubscriberEntry* newEntry)
{
    if (self->subRefs == NULL)
        self->subRefs = newEntry;
    else {
        SubscriberEntry* lastEntry = self->subRefs;

        while (lastEntry != NULL) {

            if (lastEntry->sibling == NULL) {
                lastEntry->sibling = newEntry;
                break;
            }

            lastEntry = lastEntry->sibling;
        }
    }
}

SubscriberEntry*
SubscriberEntry_create(IedModel_inputs* model, const char* variableName, const char* Dataset, uint16_t APPID, const char* cbRef, const char* ID, uint8_t* ethAddr)
{
    SubscriberEntry* self = (SubscriberEntry*) GLOBAL_MALLOC(sizeof(SubscriberEntry));
    if(self == NULL)
        return NULL;

    if(variableName != NULL)
        self->variableName = StringUtils_copyString(variableName);

    if(Dataset != NULL)
        self->Dataset = StringUtils_copyString(Dataset);

    self->APPID = APPID;

    if(cbRef != NULL)
        self->cbRef = StringUtils_copyString(cbRef);
    
    if(ID != NULL)
        self->ID = StringUtils_copyString(ID);

    if(ethAddr != NULL)
        memcpy(self->ethAddr,ethAddr,6);
        
    self->sibling = NULL;

    Subscriber_addEntry(model, self);

    return self;
}


void
IedModel_destroy_inputs(IedModel_inputs* model)
{
    /* delete all model nodes and dynamically created strings */

    /*  delete all inputs */

    Input* input = model->inputs;

    while (input != NULL) {
        Input* nextinput = input->sibling;

        InputEntry* dse = input->extRefs;

        while (dse != NULL) {
            InputEntry* nextDse = dse->sibling;

            //TODO clean extref mem

            GLOBAL_FREEMEM(dse);

            dse = nextDse;
        }

        GLOBAL_FREEMEM(input);

        input = nextinput;
    }

    /*  delete all SubscriberEntry */

    SubscriberEntry* subref = model->subRefs;

    while (subref != NULL) {
        SubscriberEntry* nextsubref = subref->sibling;
        //TODO clean subRefs mem
        GLOBAL_FREEMEM(subref);
        subref = nextsubref;
    }

    GLOBAL_FREEMEM(model);

}

