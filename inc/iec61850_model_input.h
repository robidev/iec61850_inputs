/*
 *  model_input.h
 *
 *  Copyright 2013-2016 Michael Zillgith
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

#ifndef MODEL_INPUT_H_
#define MODEL_INPUT_H_

#include "iec61850_common.h"
#include "iec61850_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup server_api_group
 *  @{
 */

/**
 * @defgroup DATA_MODEL General data model definitions, access and iteration functions
 *
 * @{
 */

/**
 * \brief Root node of the IEC 61850 data model. This is usually created by the model generator tool (genmodel.jar)
 */
typedef struct sIedModel_inputs IedModel_inputs;
typedef struct sLogicalNodeClass LogicalNodeClass;
typedef struct sInput Input;
typedef struct sSubscriberEntry SubscriberEntry;

// struct that describes the iedmodel elements that are needed to implement the input-model
// the elements can be filled from the SCL using a static datamodel, as well as dynamic config files
struct sIedModel_inputs {
    Input* inputs;                      // describes the input elements in the datamodel
    SubscriberEntry* subRefs;           // describes the dataset-references that can be subscribed to from other IED's
    LogicalNodeClass* logicalNodes;     // describes the class of each LN, so that functions can be attached
};

// struct that describes extref elements from the SCL file, as defined in the standard
typedef struct sInputEntry {
    char* desc;
    char* Ref;
    char* intAddr;
    char* serviceType;
    char* srcRef;

    MmsValue* value;
    struct sInputEntry* sibling;
} InputEntry;

// struct that describes inputs elements from the SCL file, as defined in the standard
struct sInput {
    LogicalNode* parent;
    int elementCount;
    InputEntry* extRefs;
    Input* sibling;
};

// struct that describes dataset elements from the SCL file, as defined in the standard
struct sSubscriberEntry {
    char* variableName; //
    char* Dataset; //
    uint16_t APPID; //
    char* cbRef; //svCbRef/goCbRef
    char* ID; //svID/goID
    uint8_t ethAddr[6]; //smv ethaddr[6]
    struct sSubscriberEntry* sibling;
};

/////////////////////////////////////////////////////////////////////////////////////////
// Dynamic structures

typedef struct sInputValue InputValue;
typedef void (*callBackFunction) (InputValue* input);

// struct that describes input-extref elements, and additional data
struct sInputValue {
  InputEntry * extRef;          // pointer to related extref

  int index;                    // index of value in the dataset, if remote value
  DataAttribute* DA;            // data-attribute-reference if local value is referenced by extref
  callBackFunction callBack;    // callback to be called when value is updated

  InputValue* sibling;          // additional extref that are related (same DA or same dataset)
};

// struct that describes the class of each logical node in the model, and allows code to be attached
struct sLogicalNodeClass {
  LogicalNode* parent;
  char* lnClass;
  void* initFunction;
  LogicalNodeClass* sibling;
};

#ifdef __cplusplus
}
#endif


#endif /* MODEL_INPUT_H_ */
