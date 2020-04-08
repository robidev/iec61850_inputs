/*
 * static_input.c
 *
 * automatically generated from simpleIO_inputs.cid
 */
#include <stdlib.h>
#include "iec61850_model.h"
#include "iec61850_model_extensions.h"
#include "static_model.h"



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

extern LogicalNodeClass iedModel_MUnn_LLN0_class;
extern LogicalNodeClass iedModel_MUnn_TCTR1_class;
extern LogicalNodeClass iedModel_MUnn_TCTR2_class;
extern LogicalNodeClass iedModel_MUnn_TCTR3_class;
extern LogicalNodeClass iedModel_MUnn_TCTR4_class;
extern LogicalNodeClass iedModel_MUnn_TVTR1_class;
extern LogicalNodeClass iedModel_MUnn_TVTR2_class;
extern LogicalNodeClass iedModel_MUnn_TVTR3_class;
extern LogicalNodeClass iedModel_MUnn_TVTR4_class;


LogicalNodeClass iedModel_MUnn_LLN0_class = {
    &iedModel_MUnn_LLN0,
    "LLN0",
    NULL,
    &iedModel_MUnn_TCTR1_class,
};

LogicalNodeClass iedModel_MUnn_TCTR1_class = {
    &iedModel_MUnn_TCTR1,
    "TCTR",
    NULL,
    &iedModel_MUnn_TCTR2_class,
};

LogicalNodeClass iedModel_MUnn_TCTR2_class = {
    &iedModel_MUnn_TCTR2,
    "TCTR",
    NULL,
    &iedModel_MUnn_TCTR3_class,
};

LogicalNodeClass iedModel_MUnn_TCTR3_class = {
    &iedModel_MUnn_TCTR3,
    "TCTR",
    NULL,
    &iedModel_MUnn_TCTR4_class,
};

LogicalNodeClass iedModel_MUnn_TCTR4_class = {
    &iedModel_MUnn_TCTR4,
    "TCTR",
    NULL,
    &iedModel_MUnn_TVTR1_class,
};

LogicalNodeClass iedModel_MUnn_TVTR1_class = {
    &iedModel_MUnn_TVTR1,
    "TVTR",
    NULL,
    &iedModel_MUnn_TVTR2_class,
};

LogicalNodeClass iedModel_MUnn_TVTR2_class = {
    &iedModel_MUnn_TVTR2,
    "TVTR",
    NULL,
    &iedModel_MUnn_TVTR3_class,
};

LogicalNodeClass iedModel_MUnn_TVTR3_class = {
    &iedModel_MUnn_TVTR3,
    "TVTR",
    NULL,
    &iedModel_MUnn_TVTR4_class,
};

LogicalNodeClass iedModel_MUnn_TVTR4_class = {
    &iedModel_MUnn_TVTR4,
    "TVTR",
    NULL,
    NULL
};


IedModel_extensions iedExtendedModel = {
    NULL,
    &IED1_XCBR_ds_GenericIO_LLN0_Events0,
    &iedModel_MUnn_LLN0_class,
    };
