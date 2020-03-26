/*
 *  inputs.c
 *
 *  Copyright 2013-2018 Michael Zillgith
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

#include "libiec61850_platform_includes.h"
#include "inputs_api.h"
#include "goose_subscriber.h"

typedef struct sInputValue InputValue;

struct sInputValue {
  int index;          //index of value in the dataset
  InputEntry * input; //pointer to related input
  InputValue* next;
};


int strcmp_p(const char* str1, const char* str2);
InputValue* create_InputValue(int index, InputEntry* input);
SubscriberEntry* findExtRefinDatasets(IedModel_inputs* self, char * extRef, int* index);
void subscriber_callback_inputs_GOOSE(GooseSubscriber subscriber, void* parameter);
void subscriber_callback_inputs_SMV(SVSubscriber subscriber, void* parameter, SVSubscriber_ASDU asdu);


int strcmp_p(const char* str1, const char* str2)
{
  int s1 = strlen(str1);
  int s2 = strlen(str2);
  if(s1 == s2 && s1 != 0 && s1 < 140)
  {
      return strcmp(str1, str2);
  }
  return -1;
}


InputValue* create_InputValue(int index, InputEntry* input)
{
  InputValue* self = (InputValue *) GLOBAL_MALLOC(sizeof(struct sInputValue));
  if(self == NULL)
    return NULL;
  
  self->index = index;
  self->input = input;
  self->next = NULL;
  return self;
}


SubscriberEntry* findExtRefinDatasets(IedModel_inputs* self, char * extRef, int* index)
{
  SubscriberEntry* sub = self->subRefs;

  char * prev_datset = NULL;
  *index = -1;

  while(sub != NULL)
  {
    if(strcmp_p(prev_datset, sub->Dataset) == 0)//figure out index in dataset based on ordering
      (*index)++;
    else
      (*index) = 0;
    prev_datset = sub->Dataset;
  
    if(strcmp_p(sub->variableName, extRef) == 0)//ref is equal to entry in a dataset
      return sub;
    
    sub = sub->sibling;
  }
  return NULL;
}


//order of extref elements for sampled values matter, they should appear grouped per dataset, and ordered per index
//for goose, a new subscriber-instance is made for every extref
void subscribeToInputs(IedModel_inputs* self, GooseReceiver GSEreceiver, SVReceiver SMVreceiver)
{
  //for each input
  Input* inputs = self->inputs;
  InputValue* prev_inp = NULL;
  SubscriberEntry* prev_sub = NULL;

  while(inputs != NULL)
  {
    InputEntry* extRef = inputs->extRefs; 
    while(extRef != NULL)
    {
      //search in datasets for matching ref
      int index = 0;
      SubscriberEntry* sub = findExtRefinDatasets(self, extRef->Ref, &index);
      if(sub != NULL)//matching dataset found, so subscribe
      {
        InputValue * inp = create_InputValue(index,extRef);

        if(strcmp_p(extRef->serviceType, "GOOSE") == 0 && GSEreceiver != NULL)
        {
          GooseSubscriber subscriber = GooseSubscriber_create(sub->cbRef, NULL);
          GooseSubscriber_setAppId(subscriber, sub->APPID);
          GooseSubscriber_setListener(subscriber, subscriber_callback_inputs_GOOSE, inp);
          GooseReceiver_addSubscriber(GSEreceiver, subscriber);
        }
        else if(strcmp_p(extRef->serviceType, "SMV") == 0 && SMVreceiver != NULL)
        {
          if(sub->APPID != prev_sub->APPID || sub->ethAddr != prev_sub->ethAddr)//check if a new subscriber should be made
          {
            prev_inp = NULL;
          }
          if(prev_inp == NULL)//create a new subscriber
          {
            SVSubscriber subscriber = SVSubscriber_create(sub->ethAddr, sub->APPID);
            SVSubscriber_setListener(subscriber, subscriber_callback_inputs_SMV, inp);
            SVReceiver_addSubscriber(SMVreceiver, subscriber);
            prev_inp = inp;
            prev_sub = sub;            
          }
          else//add entries to the new subscriber
          {
            prev_inp->next = inp;
          }
        }
        else
        {
          printf("WARNING: could not find extRef: '%s' serviceType: '%s', value will not be updated\n", extRef->Ref, extRef->serviceType);
        }
        // TODO(check if dataset is allready subscribed)
      }
      else
      {
        printf("WARNING: could not find extRef '%s' in subscribed datasets, value will not be updated\n", extRef->Ref);
      }
      extRef = extRef->sibling;
    }
    inputs = inputs->sibling;
  }
}

//called for subscribed GOOSE data
void subscriber_callback_inputs_GOOSE(GooseSubscriber subscriber, void* parameter)
{
  InputValue* inputVal = (InputValue*)parameter;
  if(inputVal != NULL && inputVal->input != NULL)  //iterate trough list of value-indexes that need to be copied, and
  {
    MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
    if(MmsValue_getType(values) == MMS_STRUCTURE || MmsValue_getType(values) == MMS_ARRAY)
    {
      int arraySize = MmsValue_getArraySize(values);
      if(inputVal->index < arraySize)
      {
        MmsValue* value = MmsValue_getElement(values, inputVal->index);
        if(value == NULL)
        {
          printf("ERROR: could not retrieve element from subscribed dataset, '%s' value not updated",inputVal->input->Ref);
          return;
        }
        printf("copying value %i to extRef:'%s'", inputVal->index, inputVal->input->Ref);
        if(inputVal->input->value == NULL)
          inputVal->input->value = MmsValue_clone(value);
        else
        {
          if(!MmsValue_update(inputVal->input->value, value))
            printf("ERROR: datatype does not match, '%s' value not updated",inputVal->input->Ref);
        }
      }
      else
      {
        printf("ERROR: requested index out of bounds, '%s' value not updated",inputVal->input->Ref);
        return;
      }
    }
    else
    {
      printf("ERROR: general datatype does not match, '%s' value not updated",inputVal->input->Ref);
    }
  }
  else
  {
    printf("ERROR: no valid inputval struct, no data processed");
  }
  //event trigger for value update
}

//called for subscribed SMV data
void subscriber_callback_inputs_SMV(SVSubscriber subscriber, void* parameter, SVSubscriber_ASDU asdu)
{
  InputValue* inputVal = (InputValue*)parameter;
  if(inputVal != NULL && inputVal->input != NULL)  //iterate trough list of value-indexes that need to be copied, and
  {
    int size = SVSubscriber_ASDU_getDataSize(asdu);
    if(size > 63)//set to fixed size of 9-2LE
    {
      int val[size/4];
      int i;
      for(i=0; i < (size/4); i += 2)
      {
        val[i] = SVSubscriber_ASDU_getINT32(asdu,i*4);
        val[i+1] = SVSubscriber_ASDU_getQuality(asdu,(i*4))+4;
      }
      for(i=0; i < (size/4); i++)
      {
        if(inputVal->index == i)
        {
          inputVal->input->value = MmsValue_newIntegerFromInt32(val[i]);
          inputVal = inputVal->next;
          if(inputVal == NULL)
            return;
        }
      }
    }
    else
    {
      printf("ERROR: no valid size");
    }
  }
  else
  {
    printf("ERROR: no valid inputval struct, no data processed");
  }
  //event trigger for value update
}

