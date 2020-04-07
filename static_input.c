/*
 * static_input.c
 *
 * automatically generated from simpleIO_inputs.cid
 */
#include <stdlib.h>
#include "iec61850_model.h"
#include "iec61850_model_extensions.h"
#include "static_model.h"


extern Input iedExtendedModel_GenericIO_PTOC1_inputs;
extern Input iedExtendedModel_GenericIO_PTCR1_inputs;


extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef0;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef1;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef2;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef3;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef4;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef5;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef6;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef7;
extern InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef8;

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef0 = {
  "sample_a1",
  "IED3_SMVMUnn/TCTR1.Amp",
  "Amp1",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef1
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef1 = {
  "sample_a2",
  "IED3_SMVMUnn/TCTR2.Amp",
  "Amp2",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef2
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef2 = {
  "sample_a3",
  "IED3_SMVMUnn/TCTR3.Amp",
  "Amp3",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef3
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef3 = {
  "sample_a4",
  "IED3_SMVMUnn/TCTR4.Amp",
  "Amp4",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef4
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef4 = {
  "sample_v1",
  "IED3_SMVMUnn/TVTR1.Vol",
  "Vol1",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef5
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef5 = {
  "sample_v2",
  "IED3_SMVMUnn/TVTR2.Vol",
  "Vol2",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef6
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef6 = {
  "sample_v3",
  "IED3_SMVMUnn/TVTR3.Vol",
  "Vol3",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef7
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef7 = {
  "sample_v4",
  "IED3_SMVMUnn/TVTR4.Vol",
  "Vol4",
  "SMV",
  "IED3_SMVMUnn/LLN0.MSVCB01",
  NULL,
  NULL,
  NULL,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef8
};

InputEntry iedExtendedModel_GenericIO_PTOC1_inputs_extRef8 = {
  "xcbr_status",
  "IED1_XCBRGenericIO/XCBR1.Pos.stVal",
  "xcbr_stval",
  "GOOSE",
  "IED1_XCBRGenericIO/LLN0.GoCB",
  NULL,
  NULL,
  NULL,
  NULL
};

Input iedExtendedModel_GenericIO_PTOC1_inputs = {
  &iedModel_GenericIO_PTOC1,
  9,
  &iedExtendedModel_GenericIO_PTOC1_inputs_extRef0,
  &iedExtendedModel_GenericIO_PTCR1_inputs
};

extern InputEntry iedExtendedModel_GenericIO_PTCR1_inputs_extRef0;

InputEntry iedExtendedModel_GenericIO_PTCR1_inputs_extRef0 = {
  "PTOC_trip",
  "IED2_PTOCGenericIO/PTOC1.Op.general",
  "PTOC_Op",
  "Poll",
  "IED2_PTOC/",
  NULL,
  NULL,
  NULL,
  NULL
};

Input iedExtendedModel_GenericIO_PTCR1_inputs = {
  &iedModel_GenericIO_PTCR1,
  1,
  &iedExtendedModel_GenericIO_PTCR1_inputs_extRef0,
  NULL
};
extern SubscriberEntry IED1_XCBR_ds_GenericIO_LLN0_Events0;
extern SubscriberEntry IED2_PTOC_ds_GenericIO_LLN0_Events0;
extern SubscriberEntry IED2_PTOC_ds_GenericIO_LLN0_Events1;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas10;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas11;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas12;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas13;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas14;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas15;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas16;
extern SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas17;

SubscriberEntry IED1_XCBR_ds_GenericIO_LLN0_Events0 = {
  "IED1_XCBRGenericIO/XCBR1.Pos.stVal",
  "Events",
  4096,
  "IED1_XCBRGenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x4, 0x0, 0x1},
  &IED2_PTOC_ds_GenericIO_LLN0_Events0,
};
SubscriberEntry IED2_PTOC_ds_GenericIO_LLN0_Events0 = {
  "IED2_PTOCGenericIO/GGIO1.SPCSO1.stVal",
  "Events",
  4098,
  "IED2_PTOCGenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x2},
  &IED2_PTOC_ds_GenericIO_LLN0_Events1,
};
SubscriberEntry IED2_PTOC_ds_GenericIO_LLN0_Events1 = {
  "IED2_PTOCGenericIO/PTRC1.Tr.general",
  "Events",
  4098,
  "IED2_PTOCGenericIO/LLN0$GO$gcbEvents",
  "events",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x2},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas10,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas10 = {
  "IED3_SMVMUnn/TCTR1.Amp",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas11,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas11 = {
  "IED3_SMVMUnn/TCTR2.Amp",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas12,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas12 = {
  "IED3_SMVMUnn/TCTR3.Amp",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas13,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas13 = {
  "IED3_SMVMUnn/TCTR4.Amp",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas14,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas14 = {
  "IED3_SMVMUnn/TVTR1.Vol",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas15,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas15 = {
  "IED3_SMVMUnn/TVTR2.Vol",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas16,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas16 = {
  "IED3_SMVMUnn/TVTR3.Vol",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  &IED3_SMV_ds_MUnn_LLN0_PhsMeas17,
};
SubscriberEntry IED3_SMV_ds_MUnn_LLN0_PhsMeas17 = {
  "IED3_SMVMUnn/TVTR4.Vol",
  "PhsMeas1",
  16384,
  "IED3_SMVMUnn/LLN0$SV$MSVCB01",
  "xxxxMUnn01",
  {0x1, 0xc, 0xcd, 0x1, 0x0, 0x1},
  NULL,
};

extern LogicalNodeClass iedModel_GenericIO_LLN0_class;
extern LogicalNodeClass iedModel_GenericIO_LPHD1_class;
extern LogicalNodeClass iedModel_GenericIO_PTOC1_class;
extern LogicalNodeClass iedModel_GenericIO_PTCR1_class;
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
    &iedModel_GenericIO_PTOC1_class,
};

LogicalNodeClass iedModel_GenericIO_PTOC1_class = {
    &iedModel_GenericIO_PTOC1,
    "PTOC",
    NULL,
    &iedModel_GenericIO_PTCR1_class,
};

LogicalNodeClass iedModel_GenericIO_PTCR1_class = {
    &iedModel_GenericIO_PTCR1,
    "PTCR",
    NULL,
    &iedModel_GenericIO_GGIO1_class,
};

LogicalNodeClass iedModel_GenericIO_GGIO1_class = {
    &iedModel_GenericIO_GGIO1,
    "GGIO",
    NULL,
    NULL
};


IedModel_extensions iedExtendedModel = {
    &iedExtendedModel_GenericIO_PTOC1_inputs,
    &IED1_XCBR_ds_GenericIO_LLN0_Events0,
    &iedModel_GenericIO_LLN0_class,
    };
