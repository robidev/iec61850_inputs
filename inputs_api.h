/*
 *  inputs_api.h
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

#ifndef INPUT_API_H_
#define INPUT_API_H_


#include "libiec61850_platform_includes.h"
#include "iec61850_common.h"
#include "iec61850_model.h"

#include "iec61850_model_input.h"

#include "goose_receiver.h"
#include "sv_subscriber.h"

#ifdef __cplusplus
extern "C" {
#endif


void subscribeToInputs(IedModel_inputs* self, GooseReceiver GSEreceiver, SVReceiver SMVreceiver);


#ifdef __cplusplus
}
#endif


#endif /* INPUT_API_H_ */