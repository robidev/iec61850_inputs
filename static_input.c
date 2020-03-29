/*
 * static_input.c
 *
 * automatically generated from simpleIO_inputs.cid
 */
#include <stdlib.h>
#include "iec61850_model.h"
#include "iec61850_model_input.h"
#include "static_model.h"


extern Input iedInputModel_GenericIO_LLN0_inputs;
extern Input iedInputModel_GenericIO_GGIO1_inputs;


extern InputEntry iedInputModel_GenericIO_LLN0_inputs_extRef0;
extern InputEntry iedInputModel_GenericIO_LLN0_inputs_extRef1;
extern InputEntry iedInputModel_GenericIO_LLN0_inputs_extRef2;

InputEntry iedInputModel_GenericIO_LLN0_inputs_extRef0 = {
  "sample",
  "SMV1MUnn/TVTR1.Vol",
  "adr_smv",
  "SMV",
  "SMV1MUnn/LLN0.MSVCB01",
  NULL,
  &iedInputModel_GenericIO_LLN0_inputs_extRef1
};

InputEntry iedInputModel_GenericIO_LLN0_inputs_extRef1 = {
  "sample",
  "SMV1MUnn/TVTR2.Vol",
  "adr_smv",
  "SMV",
  "SMV1MUnn/LLN0.MSVCB01",
  NULL,
  &iedInputModel_GenericIO_LLN0_inputs_extRef2
};

InputEntry iedInputModel_GenericIO_LLN0_inputs_extRef2 = {
  "sample",
  "SMV1MUnn/TCTR1.Amp",
  "adr_smv",
  "SMV",
  "SMV1MUnn/LLN0.MSVCB01",
  NULL,
  NULL
};

Input iedInputModel_GenericIO_LLN0_inputs = {
  &iedModel_GenericIO_LLN0,
  3,
  &iedInputModel_GenericIO_LLN0_inputs_extRef0,
  &iedInputModel_GenericIO_GGIO1_inputs
};

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
  "IED2GenericIO/LLN0.Mod.ctlModel",
  "adr2",
  "GOOSE",
  "IED2GenericIO/LLN1.GoCB",
  NULL,
  NULL
};

Input iedInputModel_GenericIO_GGIO1_inputs = {
  &iedModel_GenericIO_GGIO1,
  2,
  &iedInputModel_GenericIO_GGIO1_inputs_extRef0,
  NULL
};
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas10;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas11;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas12;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas13;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas14;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas15;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas16;
extern SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas17;
extern SubscriberEntry IED2_ds_GenericIO_LLN0_Events0;
extern SubscriberEntry IED2_ds_GenericIO_LLN0_Events1;
extern SubscriberEntry IED2_ds_GenericIO_LLN0_Events2;
extern SubscriberEntry IED2_ds_GenericIO_LLN0_Events3;

SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas10 = {
  "SMV1MUnn/TCTR1.Amp",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas11,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas11 = {
  "SMV1MUnn/TCTR2.Amp",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas12,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas12 = {
  "SMV1MUnn/TCTR3.Amp",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas13,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas13 = {
  "SMV1MUnn/TCTR4.Amp",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas14,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas14 = {
  "SMV1MUnn/TVTR1.Vol",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas15,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas15 = {
  "SMV1MUnn/TVTR2.Vol",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas16,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas16 = {
  "SMV1MUnn/TVTR3.Vol",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &SMV1_ds_MUnn_LLN0_PhsMeas17,
};
SubscriberEntry SMV1_ds_MUnn_LLN0_PhsMeas17 = {
  "SMV1MUnn/TVTR4.Vol",
  "PhsMeas1",
  4098,
  "SMV1MUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &IED2_ds_GenericIO_LLN0_Events0,
};
SubscriberEntry IED2_ds_GenericIO_LLN0_Events0 = {
  "IED2GenericIO/GGIO1.SPCSO1.stVal",
  "Events",
  4097,
  "IED2GenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x2},
  &IED2_ds_GenericIO_LLN0_Events1,
};
SubscriberEntry IED2_ds_GenericIO_LLN0_Events1 = {
  "IED2GenericIO/GGIO1.SPCSO2.stVal",
  "Events",
  4097,
  "IED2GenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x2},
  &IED2_ds_GenericIO_LLN0_Events2,
};
SubscriberEntry IED2_ds_GenericIO_LLN0_Events2 = {
  "IED2GenericIO/GGIO1.SPCSO3.stVal",
  "Events",
  4097,
  "IED2GenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x2},
  &IED2_ds_GenericIO_LLN0_Events3,
};
SubscriberEntry IED2_ds_GenericIO_LLN0_Events3 = {
  "IED2GenericIO/GGIO1.SPCSO4.stVal",
  "Events",
  4097,
  "IED2GenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x2},
  NULL,
};

extern LogicalNodeClass iedModel_GenericIO_LLN0_class;
extern LogicalNodeClass iedModel_GenericIO_LPHD1_class;
extern LogicalNodeClass iedModel_GenericIO_GGIO1_class;


LogicalNodeClass iedModel_GenericIO_LLN0_class = {
    &iedModel_GenericIO_LLN0,
    "LLN0",
    NULL,
    &iedModel_GenericIO_LPHD1_class,
};

LogicalNodeClass iedModel_GenericIO_LPHD1_class = {
    &iedModel_GenericIO_LPHD1,
    "LPHD",
    NULL,
    &iedModel_GenericIO_GGIO1_class,
};

LogicalNodeClass iedModel_GenericIO_GGIO1_class = {
    &iedModel_GenericIO_GGIO1,
    "GGIO",
    NULL,
    NULL
};


IedModel_inputs iedInputModel = {
    &iedInputModel_GenericIO_LLN0_inputs,
    &SMV1_ds_MUnn_LLN0_PhsMeas10,
    &iedModel_GenericIO_LLN0_class,
    };
