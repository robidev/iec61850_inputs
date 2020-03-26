#include "iec61850_model_input.h"
#include "static_model.h"

extern Input iedInputModel_GenericIO_GGIO1_inputs;


extern InputEntry iedInputModel_GenericIO_GGIO1_inputs_extRef0;
extern InputEntry iedInputModel_GenericIO_GGIO1_inputs_extRef1;

InputEntry iedInputModel_GenericIO_GGIO1_inputs_extRef0 = {
  "one",
  "IED2GenericIO/GGIO1.SPCSO1.stVal",
  "adr1",
  "GOOSE",
  "IED2GenericIO/GGIO1.GoCB",
  NULL,
  &iedInputModel_GenericIO_GGIO1_inputs_extRef1
};

InputEntry iedInputModel_GenericIO_GGIO1_inputs_extRef1 = {
  "two",
  "IED3GenericIO/LLN0.Mod.ctlModel",
  "adr2",
  "GOOSE",
  "IED3GenericIO/LLN1.GoCB",
  NULL,
  NULL
};

Input iedInputModel_GenericIO_GGIO1_inputs = {
  &iedModel_GenericIO_GGIO1,
  2,
  &iedInputModel_GenericIO_GGIO1_inputs_extRef0,
  NULL
};
extern SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events0;
extern SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events1;
extern SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events2;
extern SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events3;

SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events0 = {
  "simpleIOGenericIO/GGIO1$ST$SPCSO1$stVal",
  "Events",
  4096,
  "gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &simpleIO_ds_GenericIO_LLN0_Events1,
};
SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events1 = {
  "simpleIOGenericIO/GGIO1$ST$SPCSO2$stVal",
  "Events",
  4096,
  "gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &simpleIO_ds_GenericIO_LLN0_Events2,
};
SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events2 = {
  "simpleIOGenericIO/GGIO1$ST$SPCSO3$stVal",
  "Events",
  4096,
  "gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &simpleIO_ds_GenericIO_LLN0_Events3,
};
SubscriberEntry simpleIO_ds_GenericIO_LLN0_Events3 = {
  "simpleIOGenericIO/GGIO1$ST$SPCSO4$stVal",
  "Events",
  4096,
  "gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  NULL,
};


IedModel_inputs iedInputModel = {
    &iedInputModel_GenericIO_GGIO1_inputs,
    &simpleIO_ds_GenericIO_LLN0_Events0,
    };
