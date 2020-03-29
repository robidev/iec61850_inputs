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

typedef struct sSubscribers Subscribers;
typedef struct sInputValue InputValue;
typedef void (*callBackFunction) (InputValue* input);

struct sSubscribers {
  InputValue* inputVal;
  SubscriberEntry* subscriberData;//initial subscriber-entry for APPID, cbRef and ID
  Subscribers* next;
};

struct sInputValue {
  int index;          //index of value in the dataset
  InputEntry * input; //pointer to related input
  InputValue* next;
  callBackFunction callBack;
};


int strcmp_p(const char* str1, const char* str2);

InputValue* create_InputValue(int index, InputEntry* input);

void subscriber_callback_inputs_GOOSE(GooseSubscriber subscriber, void* parameter);
void subscriber_callback_inputs_SMV(SVSubscriber subscriber, void* parameter, SVSubscriber_ASDU asdu);


int strcmp_p(const char* str1, const char* str2)
{
  if(str1 == NULL || str2 == NULL)
    return -1;

  int s1 = strlen(str1);
  int s2 = strlen(str2);
  if(s1 == s2 && s1 != 0 && s1 < 140)
  {
      return strcmp(str1, str2);
  }
  return -1;
}

Subscribers* create_Subscribers(InputValue* inputVal, SubscriberEntry* subscriberData)
{
  Subscribers* self = (Subscribers *) GLOBAL_MALLOC(sizeof(struct sSubscribers));
  self->inputVal = inputVal;
  self->subscriberData = subscriberData;
  self->next = NULL;
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


//order of elements for sampled values matter, they should appear grouped per dataset, and ordered per index
//for goose, a new subscriber-instance is made for every extref
void subscribeToInputs(IedModel_inputs* self, GooseReceiver GSEreceiver, SVReceiver SMVreceiver)
{
  SubscriberEntry* sub = self->subRefs;

  InputValue* prev_inp = NULL;
  SubscriberEntry* prev_sub = NULL;

  char * prev_datset = NULL;
  int index = 0;
  Subscribers* sub_list = NULL;
  
  while(sub != NULL)
  {
    //figure out index in dataset based on ordering and dataset-name
    //limitation: we assume that a dataset is not subscribed by 2 control-blocks
    if(strcmp_p(prev_datset, sub->Dataset) == 0)
      index++;
    else
    {
      index = 0;
      prev_datset = sub->Dataset;
    }

    //find a matching input, iterate over all of them (all inputs are grouped by logical node)
    Input* inputs = self->inputs;
    while(inputs != NULL)
    {
      InputEntry* extRef = inputs->extRefs; 
      while(extRef != NULL)//find all matching extref for this input
      {
        if(strcmp_p(sub->variableName, extRef->Ref) == 0)//if extref and datasetname match, we subscribe to it!
        {
          InputValue * inputValue = create_InputValue(index,extRef);//match a dataset index to an extref, so we know how to decode a subscribed dataset

          if(strcmp_p(extRef->serviceType, "GOOSE") == 0 && GSEreceiver != NULL)
          {//if the extref is GOOSE subscribed, then create a subscriber
            if(prev_sub == NULL)//always create an initial subscriber 
            {
              prev_sub = sub;
              prev_inp = NULL;
            }

            if(sub->APPID != prev_sub->APPID || strcmp_p(sub->cbRef,prev_sub->cbRef) != 0)//check if a new subscriber should be made, or added to an existing
            {//no match with the previous subscriber-entry, so search all existing inputs for a usable subscriber
              prev_inp = NULL;
              //search all existing subscribers for a ref. if previous ref does not match. 
              //TODO: is this needed when subscriber-entries are ordered by dataset?
                  Subscribers* lSub_list = sub_list;
                  while(lSub_list != NULL)
                  {
                    if(sub->APPID == lSub_list->subscriberData->APPID && strcmp_p(sub->cbRef,lSub_list->subscriberData->cbRef) == 0)
                    {
                      prev_inp = lSub_list->inputVal;//a match is found, so add this input entry to that subscriber
                      break;
                    }
                    lSub_list = lSub_list->next;
                  }
                  //ensure we are at the end of the list
                  while(prev_inp->next != NULL)
                  {
                    prev_inp = prev_inp->next;
                  }
            }

            //create a new subscriber, if its the first inputValue in the list
            if(prev_inp == NULL)
            {
              GooseSubscriber subscriber = GooseSubscriber_create(sub->cbRef, NULL);
              GooseSubscriber_setAppId(subscriber, sub->APPID);
              GooseSubscriber_setListener(subscriber, subscriber_callback_inputs_GOOSE, inputValue);
              GooseReceiver_addSubscriber(GSEreceiver, subscriber);

              if(sub_list == NULL)
                sub_list = create_Subscribers(inputValue, sub);
              else
                sub_list->next = create_Subscribers(inputValue, sub);

              prev_sub = sub; 
            }
            else//add entries to the current subscriber
            {
              prev_inp->next = inputValue;
            }
            prev_inp = inputValue;
          }
          else if(strcmp_p(extRef->serviceType, "SMV") == 0 && SMVreceiver != NULL)
          {//if the extref is SMV, then create/add a subscriber
            if(prev_sub == NULL)//always create an initial subscriber 
            {
              prev_sub = sub;
              prev_inp = NULL;
            }

            if(sub->APPID != prev_sub->APPID || memcmp(sub->ethAddr,prev_sub->ethAddr,6) != 0)//check if a new subscriber should be made, or added to an existing
            {
              prev_inp = NULL;
              //search all existing subscribers for a ref. if previous ref does not match
              //TODO: is this needed when subscriber-entries are ordered by dataset?
                  Subscribers* lSub_list = sub_list;
                  while(lSub_list != NULL)
                  {
                    if(sub->APPID == lSub_list->subscriberData->APPID && memcmp(sub->ethAddr,lSub_list->subscriberData->ethAddr,6) == 0)
                    {
                      prev_inp = lSub_list->inputVal;//a match is found, so add this input entry to that subscriber
                      break;
                    }
                    lSub_list = lSub_list->next;
                  }
                  //ensure we are at the end of the list
                  while(prev_inp->next != NULL)
                  {
                    prev_inp = prev_inp->next;
                  }
            }

            //create a new subscriber, if its the first inputValue in the list
            if(prev_inp == NULL)
            {
              char null_arr[6] = {0,0,0,0,0,0};
              SVSubscriber subscriber = SVSubscriber_create(sub->ethAddr, sub->APPID);
              SVSubscriber_setListener(subscriber, subscriber_callback_inputs_SMV, inputValue);
              SVReceiver_addSubscriber(SMVreceiver, subscriber);
              if(memcmp(sub->ethAddr,null_arr,6) != 0)
              {
                SVReceiver_enableDestAddrCheck(SMVreceiver);
              }

              if(sub_list == NULL)
                sub_list = create_Subscribers(inputValue, sub);
              else
                sub_list->next = create_Subscribers(inputValue, sub);

              prev_sub = sub;            
            }
            else//add entries to the current subscriber
            {
              prev_inp->next = inputValue;
            }
            prev_inp = inputValue;
          }
          else
          {
            printf("WARNING: could not find extRef: '%s' serviceType: '%s', value will not be updated\n", extRef->Ref, extRef->serviceType);
          }
        }
        extRef = extRef->sibling;
      }
      inputs = inputs->sibling;
    }
    sub = sub->sibling;
  }
}

//called for subscribed GOOSE data
void subscriber_callback_inputs_GOOSE(GooseSubscriber subscriber, void* parameter)
{
  printf("GOOSE received\n");
  InputValue* inputVal = (InputValue*)parameter;
  if(inputVal != NULL && inputVal->input != NULL)  //iterate trough list of value-indexes that need to be copied, and
  {
    MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
    if(MmsValue_getType(values) == MMS_STRUCTURE || MmsValue_getType(values) == MMS_ARRAY)
    {
      int arraySize = MmsValue_getArraySize(values);
      int i;
      for(i=0;i<arraySize;i++)
      {
        //find all extrefs for this index
        while(inputVal != NULL)//TODO: is this needed, when inputval entries are ordered in a dataset? could be combined into 'while(inputVal->index == i)'
        { 
          if(inputVal->index == i)
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

            if(inputVal->callBack != NULL){
              inputVal->callBack(inputVal);
            }
          }
          inputVal = inputVal->next;
        }
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
  uint64_t tm = SVSubscriber_ASDU_getRefrTmAsMs(asdu);//Hal_getTimeInMs();
  InputValue* inputVal = (InputValue*)parameter;
  if(inputVal != NULL && inputVal->input != NULL)  //iterate trough list of value-indexes that need to be copied, and
  {
    int size = SVSubscriber_ASDU_getDataSize(asdu);
    if(size > 63)//set to fixed size of 9-2LE
    {
      int32_t val[size/4];
      int i;
      for(i=0; i < (size/4); i += 2)//read the stval and q. time is for all the same.
      {
        val[i] = SVSubscriber_ASDU_getINT32(asdu,i*4);
        Quality q = SVSubscriber_ASDU_getQuality(asdu,(i*4)+4);
        val[i+1] = (int)q;
      }
      for(i=0; i < (size/8); i++)//a mmsval with stval, q and time is expected
      {
        //find all extrefs for this index
        while(inputVal != NULL)//TODO: is this needed, when inputval entries are ordered in a dataset? could be combined into 'while(inputVal->index == i)'
        {
          if(inputVal->index == i)
          {
            //performancewise this could be improved, by performing the copy directly
            MmsValue* value = MmsValue_createEmptyStructure(3);

            MmsValue* stVal = MmsValue_newIntegerFromInt32(val[(i*2)]);
            MmsValue_setElement(value,0,stVal);
            
            MmsValue* q = MmsValue_newUnsignedFromUint32(val[(i*2) + 1]);
            MmsValue_setElement(value,1,q);
            
            MmsValue* t = MmsValue_newUtcTimeByMsTime(tm);
            MmsValue_setElement(value,2,t);

            if(inputVal->input->value == NULL)
            {
              inputVal->input->value = value;
            }
            else
            {
              if(!MmsValue_update(inputVal->input->value,value))
                printf("update ERROR");
              MmsValue_delete(value);
            }
            
            if(inputVal->callBack != NULL){
              inputVal->callBack(inputVal);
            }
          }
          inputVal = inputVal->next;
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
