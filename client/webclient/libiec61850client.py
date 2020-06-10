#!/usr/bin/env python3

import os,sys
import ctypes
import time
import lib61850
import logging

from urllib.parse import urlparse

logger = logging.getLogger(__name__)

class iec61850client():

	def __init__(self, readvaluecallback = None, loggerRef = None):
		global logger
		if loggerRef != None:
			logger = loggerRef

		self.polling = {}
		self.connections = {}
		self.readvaluecallback = readvaluecallback


	@staticmethod
	def printValue(value):
		_type = lib61850.MmsValue_getTypeString(value)
		_type = str(_type)
		if _type == "boolean":
			return ("%r" % lib61850.MmsValue_getBoolean(value)), _type
		if _type == "array":
			return ("arr"), _type
		if _type ==  "bcd":
			return ("bcd"), _type
		if _type ==  "binary-time":
			return ("%i" % lib61850.MmsValue_getBinaryTimeAsUtcMs(value)), _type
		if _type == "bit-string":
			return ("%i" % lib61850.MmsValue_getBitStringAsInteger(value)), _type
		if _type == "access-error":
			return ("ACCESS ERROR"), _type
		if _type == "float":
			return ("%f" % lib61850.MmsValue_toFloat(value)), _type
		if _type == "generalized-time":
			return ("%u" % lib61850.MmsValue_toUnixTimestamp(value)), _type
		if _type == "integer":
			return ("%i" % lib61850.MmsValue_toInt64(value)), _type
		if _type == "oid":
			return ("OID ERROR"), _type
		if _type == "mms-string":
			return ("%s" % lib61850.MmsValue_toString(value).decode("utf-8")), _type
		if _type == "structure":
			return ("STRUCTURE"), _type
		if _type == "octet-string":
			len = lib61850.MmsValue_getOctetStringSize(value)
			buf = lib61850.MmsValue_getOctetStringBuffer(value)
			#magic cast to convert a swig pointer into a ctypes pointer, the int(buf) works, but why?
			buff = ctypes.cast(buf, ctypes.POINTER(ctypes.c_char))
			#allocate a buffer for the result
			res = bytearray(len)
			#create a pointer to the result buffer
			rptr = (ctypes.c_char * len).from_buffer(res)
			#copy the memory from the swig buffer to the result buffer
			ctypes.memmove(rptr, buff, len)
			return ("%s" % ''.join(format(x, '02x') for x in res)), _type
		if _type == "unsigned":
			return ("%u" % lib61850.MmsValue_toUint32(value)), _type
		if _type == "utc-time":
			return ("%u" % lib61850.MmsValue_getUtcTimeInMs(value)), _type
		if _type == "visible-string":
			return ("%s" % lib61850.MmsValue_toString(value).decode("utf-8")), _type
		if _type == "unknown(error)":
			return ("UNKNOWN ERROR"), _type
		return ("CANNOT FIND TYPE"), _type


	@staticmethod
	def printDataDirectory(con, doRef):
		tmodel = {}
		if doRef.find("/") == -1:
			logger.error("invalid datadirecory")
			return {}

		error = lib61850.IedClientError()
		dataAttributes = lib61850.IedConnection_getDataDirectoryFC(con, ctypes.byref(error), doRef)

		if error.value != 0:
			logger.error("could not get logical device list, error:%i" % error.value)

		if dataAttributes:
			dataAttribute = lib61850.LinkedList_getNext(dataAttributes)

			while dataAttribute:
				daName = ctypes.cast(lib61850.LinkedList_getData(dataAttribute),ctypes.c_char_p).value.decode("utf-8")
				daRef = doRef+"."+daName[:-4]
				fcName = daName[-3:-1]

				submodel = iec61850client.printDataDirectory(con,daRef)
				if submodel:
					tmodel[daName[:-4]] = submodel
					
				else:
					tmodel[daName[:-4]] = {}
					tmodel[daName[:-4]]['reftype'] = "DA"
					tmodel[daName[:-4]]['FC'] = fcName
					tmodel[daName[:-4]]['value'] = "UNKNOWN"
					#read DA
					fc = lib61850.FunctionalConstraint_fromString(fcName) 
					value = lib61850.IedConnection_readObject(con, ctypes.byref(error), daRef, fc)

					if error.value == 0:
						tmodel[daName[:-4]]['value'], tmodel[daName[:-4]]['type'] = iec61850client.printValue(value)
						lib61850.MmsValue_delete(value)

				dataAttribute = lib61850.LinkedList_getNext(dataAttribute)

			lib61850.LinkedList_destroy(dataAttributes)
		return tmodel


	@staticmethod
	def discovery(con):
		tmodel = {}

		error = lib61850.IedClientError()
		deviceList = lib61850.IedConnection_getLogicalDeviceList(con, ctypes.byref(error))

		if error.value != 0:
			logger.error("could not get logical device list, error:%i" % error)

		if deviceList:
			device = lib61850.LinkedList_getNext(deviceList)
			while device:
				LD_name=ctypes.cast(lib61850.LinkedList_getData(device),ctypes.c_char_p).value.decode("utf-8")
				tmodel[LD_name] = {}

				logicalNodes = lib61850.IedConnection_getLogicalDeviceDirectory(con, ctypes.byref(error), LD_name)
				if error.value != 0:#ret becomes int if connection is lost
					lib61850.LinkedList_destroy(deviceList)
					return model
					
				logicalNode = lib61850.LinkedList_getNext(logicalNodes)
				while logicalNode:
					LN_name=ctypes.cast(lib61850.LinkedList_getData(logicalNode),ctypes.c_char_p).value.decode("utf-8")
					tmodel[LD_name][LN_name] = {}

					#[LNobjects, error] = iec61850.IedConnection_getLogicalNodeVariables(con, LD_name+"/"+LN_name)
					LNobjects = lib61850.IedConnection_getLogicalNodeDirectory(con, ctypes.byref(error), LD_name+"/"+LN_name,lib61850.ACSI_CLASS_DATA_OBJECT)
					if error.value != 0:#ret becomes int if connection is lost
						lib61850.LinkedList_destroy(logicalNodes)
						lib61850.LinkedList_destroy(deviceList)
						return model

					LNobject = lib61850.LinkedList_getNext(LNobjects)
					while LNobject:
						Do = ctypes.cast(lib61850.LinkedList_getData(LNobject),ctypes.c_char_p).value.decode("utf-8")
						tmodel[LD_name][LN_name][Do] = {}

						doRef = LD_name+"/"+LN_name+"."+Do

						tmodel[LD_name][LN_name][Do] = iec61850client.printDataDirectory(con, doRef)

						LNobject = lib61850.LinkedList_getNext(LNobject)
					lib61850.LinkedList_destroy(LNobjects)

					LNdss = lib61850.IedConnection_getLogicalNodeDirectory(con, ctypes.byref(error), LD_name+"/"+LN_name, lib61850.ACSI_CLASS_DATA_SET)
					if error.value != 0:#ret becomes int if connection is lost
						lib61850.LinkedList_destroy(logicalNodes)
						lib61850.LinkedList_destroy(deviceList)
						return tmodel

					LNds = lib61850.LinkedList_getNext(LNdss)
					while LNds:

						DSname = ctypes.cast(lib61850.LinkedList_getData(LNds),ctypes.c_char_p).value.decode("utf-8")
						tmodel[LD_name][LN_name][DSname] = {}

						#cannot pass the right type to isDeletable(last arg).. keeps complaining about 'bool *', and isDel = ctypes.pointer(ctypes.c_bool(False)) does not work
						isDel = ctypes.c_bool(False)
						dataSetMembers = lib61850.IedConnection_getDataSetDirectory(con, ctypes.byref(error), LD_name+"/"+LN_name+"."+DSname, ctypes.byref(isDel))  
						if error.value != 0:#ret becomes int if connection is lost
							lib61850.LinkedList_destroy(LNdss)
							lib61850.LinkedList_destroy(logicalNodes)
							lib61850.LinkedList_destroy(deviceList)
							return tmodel

						#all DS are assumed not deletable 
						if isDel == True:
							logger.error("  DS: %s, not Deletable" % DSname)
						else:
							logger.error("  DS: %s, is Deletable" % DSname)
						dataSetMemberRef = lib61850.LinkedList_getNext(dataSetMembers)

						i = 0
						while dataSetMemberRef:
							dsRef = ctypes.cast(lib61850.LinkedList_getData(dataSetMemberRef),ctypes.c_char_p).value.decode("utf-8")
							DX = dsRef[:-4]
							FC = dsRef[-3:-1]
							tmodel[LD_name][LN_name][DSname][str(i)] = {}
							tmodel[LD_name][LN_name][DSname][str(i)]['reftype'] = "DX"
							tmodel[LD_name][LN_name][DSname][str(i)]['type'] = "reference"
							tmodel[LD_name][LN_name][DSname][str(i)]['value'] = DX
							tmodel[LD_name][LN_name][DSname][str(i)]['FC'] = FC
							dataSetMemberRef = lib61850.LinkedList_getNext(dataSetMemberRef)
							i += 1
						lib61850.LinkedList_destroy(dataSetMembers)
						LNds = lib61850.LinkedList_getNext(LNds)

					lib61850.LinkedList_destroy(LNdss)
					logicalNode = lib61850.LinkedList_getNext(logicalNode)

				lib61850.LinkedList_destroy(logicalNodes)
				device = lib61850.LinkedList_getNext(device)

			lib61850.LinkedList_destroy(deviceList)
		return tmodel


	@staticmethod
	def getMMsValue(type, value, size=8, typeval = -1):
		#allocate mmsvalue based on type
		if type == "visible-string" or typeval == lib61850.MMS_VISIBLE_STRING:
			return lib61850.MmsValue_newVisibleString(str(value))
		if type == "boolean" or typeval == lib61850.MMS_BOOLEAN:
			if value == "True":
				return lib61850.MmsValue_newBoolean(True)
			else:
				return lib61850.MmsValue_newBoolean(False)
		if type == "integer" or typeval == lib61850.MMS_INTEGER:
			return lib61850.MmsValue_newInteger(int(value))
		#untested
		if type == "unsigned" or typeval == lib61850.MMS_UNSIGNED:
			return lib61850.MmsValue_newUnsignedFromUint32(int(value))
		if type == "mms-string" or typeval == lib61850.MMS_STRING:
			return lib61850.MmsValue_newMmsString(str(value))
		if type == "float" or typeval == lib61850.MMS_FLOAT:
			return lib61850.MmsValue_newFloat(float(value))
		if type ==  "binary-time" or typeval == lib61850.MMS_BINARY_TIME:
			return lib61850.MmsValue_newBinaryTime(int(value))
		if type == "bit-string" or typeval == lib61850.MMS_BIT_STRING:
			bs = lib61850.MmsValue_newBitString(size)
			return lib61850.MmsValue_setBitStringFromInteger(bs,int(value))
		if type == "generalized-time" or typeval == lib61850.MMS_GENERALIZED_TIME:
			return lib61850.MmsValue_newUtcTimeByMsTime(int(value))
		if type == "utc-time" or typeval == lib61850.MMS_UTC_TIME:
			return lib61850.MmsValue_newUtcTimeByMsTime(int(value))
		if type == "octet-string" or typeval == lib61850.MMS_OCTET_STRING:
			sl = len(value)
			sptr = (ctypes.c_char * sl).from_buffer(value)

			buf = lib61850.MmsValue_newOctetString(sl,127)
			buff = ctypes.cast(int(buf), ctypes.POINTER(ctypes.c_char))

			ctypes.memmove(buff, sptr, sl)
			return buf
		#unsupported types
		if type == "array" or typeval == lib61850.MMS_ARRAY:
			return None
		if type ==  "bcd" or typeval == lib61850.MMS_BCD:
			return None
		if type == "access-error" or typeval == lib61850.MMS_DATA_ACCESS_ERROR:
			return None
		if type == "oid" or typeval == lib61850.MMS_OBJ_ID:
			return None
		if type == "structure" or typeval == lib61850.MMS_STRUCTURE:
			return  None
		if type == "unknown(error)":
			return None
		logger.error("Mms value type %s not supported" % type)
		return None


	@staticmethod
	def writeValue(con, model, ref, value):
		submodel, path = iec61850client.parseRef(model,ref)

		if not submodel:
			logger.error("cannot find ref: %s in model" % ref)
			return {},-1
		if not 'FC' in submodel:
			logger.error("ref is not DA")
			return {},-1

		fc = lib61850.FunctionalConstraint_fromString(submodel['FC']) 
		mmsvalue = iec61850client.getMMsValue(submodel['type'],value)
		if not mmsvalue:
			return model,-1

		error = lib61850.IedClientError()
		lib61850.IedConnection_writeObject(con, ctypes.byref(error), ref, fc, mmsvalue)
		lib61850.MmsValue_delete(mmsvalue)
		if error.value == 0:
			model, err = iec61850client.updateValueInModel(con, model, ref)
			return model, err
		return model, error.value


	@staticmethod
	def updateValueInModel(con, model, ref):
		err = -1
		val, path = iec61850client.parseRef(model,ref)

		# recursive subfunction
		def update_recurse(con, submodel, path):
			err = -1
			if len(path) < 1:
				logger.error("recusion into model went wrong")
				err = -1
			elif len(path) == 1:
				if submodel[ path[0] ] and 'reftype' in submodel[ path[0] ] and submodel[ path[0] ]['reftype'] == 'DA':
					fcName = submodel[ path[0] ]['FC']
					#read DA
					fc = lib61850.FunctionalConstraint_fromString(fcName) 
					error = lib61850.IedClientError()
					value = lib61850.IedConnection_readObject(con, ctypes.byref(error), ref, fc)
					if error.value == 0:
						submodel[ path[0] ]['value'],  submodel[ path[0] ]['type'] = iec61850client.printValue(value)
						lib61850.MmsValue_delete(value)
						err = 0
					else:
						logger.error("could not read DA: %s from device" % ref)
						err = -1

				else:
					submodel[ path[0] ] = iec61850client.printDataDirectory(con, ref)
					if submodel[ path[0] ]:# check if value or empty returned
						err = 0
					else:
						err = -1
			else:
				submodel[ path[0] ], err = update_recurse(con, submodel[ path[0] ], path[1:])
			return submodel, err

		#recurse the model
		model, err = update_recurse(con, model, path)
		return model, err


	@staticmethod
	def parseRef(model,ref):
		path = []
		if ref == "" or ref == None:
			return model, path
		
		_ref = ref.split("/")
		if len(_ref) == 1:
			path.append(ref)
			if ref in model:
				return model[ref], path
			else:
				logger.error("cannot find LD in model")
				return {}, []
		
		if len(_ref) > 2:
			logger.error("cannot parse ref, more then 1 '/' encountered ")
			return {}, []
		#one / encountered
		LD = _ref[0]
		path.append(LD)
		
		if not LD in model:
			logger.error("cannot find LD in model")
			return {}, []
		mm = model[LD]

		_ref = _ref[1].split(".")
		for i in range( len(_ref) ):
			path.append(_ref[i])

			if not _ref[i] in mm:
				logger.error("cannot find node in model: %s" % _ref[i])
				return {},[]

			mm = mm[ _ref[i] ]

		return mm, path


	@staticmethod
	def getRef(model,path):
		ref = ""
		mm = model
		for i in range( len(path) ):
			if not path[i] in mm:
				logger.error("cannot find node in model: %s in %s" % (path[i],ref))
				return ref, mm

			if i == 1:
				ref += "/"
			elif i > 1:
				ref += "."
			ref += path[i]
			mm = mm[ path[i] ]
		return ref, mm


	@staticmethod
	def printrefs(model, ref="", depth=0):
		_ref = ""
		for element in model:
			if depth == 0:
				_ref = element
			elif depth == 1:
				_ref = ref + "/" + element
			elif depth > 1:
				_ref = ref + "." + element
			if 'value' in model[element]:
				print(_ref + ":\t" + str(model[element]['value']))
			else:
				iec61850client.printrefs(model[element],_ref, depth + 1)


	# retrieve an active connection to IED, and up to date datamodel, stored in 'connections'
	def getIED(self, host, port):
		if port == "" or port == None:
			port = 102

		if host == None:
			logger.error("missing hostname")
			return -1

		tupl = host + ":" + str(port)
		if tupl in self.connections and self.connections[tupl]["con"] != None:
			if not self.connections[tupl]["model"]:
				con = self.connections[tupl]["con"]
				model = iec61850client.discovery(con)
				if model: #if model is not empty
					# store the model
					self.connections[tupl]["model"] = model
					return 0
				else:
					#we could not perform a discovery, so remove connection
					lib61850.IedConnection_destroy(con)
					self.connections[tupl]["con"] = None
					return -1
			else:
				#we have a connection and a model
				return 0
		
		if not tupl in self.connections:
			self.connections[tupl] = {}
			self.connections[tupl]["con"] = None
			self.connections[tupl]["model"] = {}

		con = lib61850.IedConnection_create()
		error = lib61850.IedClientError()
		lib61850.IedConnection_connect(con,ctypes.byref(error), host, port)
		if error.value == lib61850.IED_ERROR_OK:
			# store the active connection
			self.connections[tupl]["con"] = con
			# read the model
			model = iec61850client.discovery(con)
			if model: #if model is not empty
				# store the model
				self.connections[tupl]["model"] = model
				return 0
			else:
				return -1
		else:
			lib61850.IedConnection_destroy(con)
			return -1


	# write a value to an active connection
	def registerWriteValue(self, ref, value):
		uri_ref = urlparse(ref)
		port = uri_ref.port
		if port == "" or port == None:
			port = 102

		if uri_ref.scheme != "iec61850":
			logger.error("incorrect scheme, only iec61860 is supported, not %s" % uri_ref.scheme)
			return -1

		if uri_ref.hostname == None:
			logger.error("missing hostname: %s" % ref)
			return -1

		tupl = uri_ref.hostname + ":" + str(port)

		#check if connection is active, or reconnect
		err = self.getIED(uri_ref.hostname, port)
		if err == 0:
			con = self.connections[tupl]['con']
			if not con:
				logger.error("no valid connection")
				return -1			

			model = self.connections[tupl]['model']
			if not model:
				logger.error("no valid model")
				return -1

			model, error = iec61850client.writeValue(con, model, uri_ref.path[1:], value)
			if error == 0:
				self.connections[tupl]['model'] = model
				submodel, path = iec61850client.parseRef(model,uri_ref.path[1:]) #get value from model via ref
				logger.debug("Value '%s' written to %s" % (str(submodel), ref) )

				if self.readvaluecallback != None:
					self.readvaluecallback(ref, submodel)

				return 0
			else:
				logger.error("could not write '%s' to %s with error: %i" % (str(value), ref, error))
				if error == 3: #we lost the connection
					lib61850.IedConnection_destroy(con)
					self.connections[tupl]['con'] = None
				return error
		else:
			logger.error("no connection to IED: %s:%s" % (uri_ref.hostname, port) )
		return -1


	# read a value from an active connection
	def ReadValue(self, ref):
		uri_ref = urlparse(ref)
		port = uri_ref.port
		if port == "" or port == None:
			port = 102

		if uri_ref.scheme != "iec61850":
			logger.error("incorrect scheme, only iec61860 is supported, not %s" % uri_ref.scheme)
			return {}, -1

		if uri_ref.hostname == None:
			logger.error("missing hostname: %s" % ref)
			return {}, -1

		tupl = uri_ref.hostname + ":" + str(port)


		#check if connection is active, or reconnect
		err = self.getIED(uri_ref.hostname, port)
		if err == 0:
			con = self.connections[tupl]['con']
			if not con:
				logger.error("no valid connection")
				return {}, -1			

			model = self.connections[tupl]['model']
			if not model:
				logger.error("no valid model")
				return {}, -1

			submodel, path = iec61850client.parseRef(model, uri_ref.path[1:])
			if submodel: #ref exists in model
				model, error = iec61850client.updateValueInModel(con, model, uri_ref.path[1:])
				if error == 0:
					self.connections[tupl]['model'] = model
					submodel, path = iec61850client.parseRef(model, uri_ref.path[1:])
					logger.debug("Value '%s' read from %s" % (str(submodel), ref) )

					if self.readvaluecallback != None:
						self.readvaluecallback(ref, submodel)

					return submodel, 0 
				else:
					logger.error("could not read '%s' with error: %i" % (ref, error))
					if error == 3: #we lost the connection
						lib61850.IedConnection_destroy(con)
						self.connections[tupl]['con'] = None
			else:
				logger.error("could not find %s in model" % uri_ref.path[1:])
		else:
			logger.error("no connection to IED: %s:%s" % (uri_ref.hostname, port) )
		return {}, -1


	# register value for reading
	def registerReadValue(self,ref):
		# check if present in dataset/report, and subscribe TODO
		
		# fallback to periodic poll when no report+dataset configured
		#if we allready have it in the list
		if ref in self.polling:
			logger.debug("reference: %s allready registered" % ref)
			return 0

		uri_ref = urlparse(ref)
		port = uri_ref.port
		if port == "" or port == None:
			port = 102


		if uri_ref.scheme != "iec61850":
			logger.error("incorrect scheme, only iec61860 is supported, not %s" % uri_ref.scheme)
			return -1

		if uri_ref.hostname == None:
			logger.error("missing hostname: %s" % ref)
			return -1

		tupl = uri_ref.hostname + ":" + str(port)

		#check if connection is active, or reconnect
		err = self.getIED(uri_ref.hostname, port)
		if err == 0:
			#only add an IED if it could be connected to
			model = self.connections[tupl]['model']
			#check if the ref exists in the model
			submodel, path = iec61850client.parseRef(model, uri_ref.path[1:])
			if submodel:
				self.polling[ref] = 1
				return 0
			else:
				logger.error("could not find %s in model" % uri_ref.path[1:])
		else:
			logger.error("no connection to IED: %s:%s, ref:%s not registered" % (uri_ref.hostname, port, ref) )
		return -1


	# retrieve all registered values by polling
	def poll(self):
		for key in self.polling:
			uri_ref = urlparse(key)

			port = uri_ref.port
			if port == "" or port == None:
				port = 102


			if uri_ref.scheme != "iec61850":
				logger.error("incorrect scheme, only iec61860 is supported, not %s" % uri_ref.scheme)
				continue

			if uri_ref.hostname == None:
				logger.error("missing hostname: %s" % key)
				continue

			tupl = uri_ref.hostname + ":" + str(port)

			#check if connection is active, or reconnect
			err = self.getIED(uri_ref.hostname, port)
			if err == 0:
				con = self.connections[tupl]['con']
				model = self.connections[tupl]['model']
				if con and model:
					model, err = iec61850client.updateValueInModel(con, model, uri_ref.path[1:])
					if err == 0:
						self.connections[tupl]['model'] = model
						submodel, path = iec61850client.parseRef(model, uri_ref.path[1:])
						logger.debug("value:%s read from key: %s" % (str(submodel), key))
						#call function with ref+value
						if self.readvaluecallback != None:
							self.readvaluecallback(key, submodel)

					else:
						logger.error("model not updated for %s with error: %i" % (key, err))
						if err == 3: #we lost the connection
							lib61850.IedConnection_destroy(con)
							self.connections[tupl]['con'] = None
			else:
				logger.error("model not updated for %s with error: %i" % (key, err))


	# retrieve datamodel from server
	def getDatamodel(self, ref=None, hostname="localhost", port=102):
		# if uri provided, it will have presedence over hostname and port
		if ref != None:
			uri_ref = urlparse(ref)
			hostname = uri_ref.hostname
			port = uri_ref.port

		# if port is explicitly defined as "" or None, assume 102
		if port == "" or port == None:
			port = 102

		err = self.getIED(hostname, port)
		if err == 0:
			tupl =  hostname + ":" + str(port)
			return self.connections[tupl]['model']
		else:
			logger.error("no connection to IED: %s:%s" % (hostname, port) )
			return {}
	

	def getRegisteredIEDs(self):
		return self.connections


	def commandTerminationHandler_cb(self, param, con):
		#buff = ctypes.cast(param, ctypes.POINTER(ctypes.c_char))
		buff = ctypes.cast(param,ctypes.c_char_p).value.decode("utf-8")
		logger.debug("commandTerminationHandler_cb called: %s", buff)
		lastApplError = lib61850.ControlObjectClient_getLastApplError(con)

		#/* if lastApplError.error != 0 this indicates a CommandTermination- */
		if lastApplError.error != 0:
			print("Received CommandTermination-")
			print(" LastApplError: %i"% lastApplError.error)
			print("      addCause: %i"% lastApplError.addCause)
		else:
			print("Received CommandTermination+")
		return


	def operate(self, ref, value, control = None):
		error = -1

		if ref != None:
			uri_ref = urlparse(ref)
			hostname = uri_ref.hostname
			port = uri_ref.port

		# if port is explicitly defined as "" or None, assume 102
		if port == "" or port == None:
			port = 102

		err = self.getIED(hostname, port)
		if err == 0:
			tupl =  hostname + ":" + str(port)
			con = self.connections[tupl]['con']

			if control == None:
				control = lib61850.ControlObjectClient_create(uri_ref.path[1:], con)
				ctlModel = lib61850.ControlObjectClient_getControlModel(control)
				if ctlModel == lib61850.CONTROL_MODEL_DIRECT_ENHANCED or ctlModel == lib61850.CONTROL_MODEL_SBO_ENHANCED:
					logger.debug("enhanced security")
					cb = lib61850.CommandTerminationHandler(self.commandTerminationHandler_cb)
					lib61850.ControlObjectClient_setCommandTerminationHandler(control, cb, "Operate")
				else:
					logger.debug("normal security")
				lib61850.ControlObjectClient_setOrigin(control, "mmi", 3)

			mmsType = lib61850.ControlObjectClient_getCtlValType(control)
			ctlVal = iec61850client.getMMsValue("",value,0,mmsType)
			
			error = lib61850.ControlObjectClient_operate(control, ctlVal, 0)

			time.sleep(2)
			lib61850.MmsValue_delete(ctlVal)
			lib61850.ControlObjectClient_destroy(control)
		
		return error

	def select(self, ref, value):
		error = -1
		control = None

		if ref != None:
			uri_ref = urlparse(ref)
			hostname = uri_ref.hostname
			port = uri_ref.port

		# if port is explicitly defined as "" or None, assume 102
		if port == "" or port == None:
			port = 102

		err = self.getIED(hostname, port)
		if err == 0:
			tupl =  hostname + ":" + str(port)
			con = self.connections[tupl]['con']
			control = lib61850.ControlObjectClient_create(uri_ref.path[1:], con)

			ctlModel = lib61850.ControlObjectClient_getControlModel(control)
			if ctlModel == lib61850.CONTROL_MODEL_SBO_NORMAL:
				logger.debug("SBO ctlmodel")
				error = lib61850.ControlObjectClient_select(control)

			elif ctlModel == lib61850.CONTROL_MODEL_SBO_ENHANCED:
				logger.debug("SBOw ctlmodel")
				cb = lib61850.CommandTerminationHandler(self.commandTerminationHandler_cb)
				lib61850.ControlObjectClient_setCommandTerminationHandler(control, cb, "Select")

				mmsType = lib61850.ControlObjectClient_getCtlValType(control)
				ctlVal = iec61850client.getMMsValue("",value,0,mmsType)
				lib61850.ControlObjectClient_setOrigin(control, "mmi", 3)
				error = lib61850.ControlObjectClient_selectWithValue(control, ctlVal)

				time.sleep(2)
				lib61850.MmsValue_delete(ctlVal)
				#lib61850.ControlObjectClient_destroy(control)
			else:
				logger.error("cannot select object with ctlmodel: %i" % ctlModel)
				error = -1
				lib61850.ControlObjectClient_destroy(control)
				control = None

		return error, control

	def cancel(self, ref, control = None):
		error = -1
		control = None

		if ref != None:
			uri_ref = urlparse(ref)
			hostname = uri_ref.hostname
			port = uri_ref.port

		# if port is explicitly defined as "" or None, assume 102
		if port == "" or port == None:
			port = 102

		err = self.getIED(hostname, port)
		if err == 0:
			tupl =  hostname + ":" + str(port)
			con = self.connections[tupl]['con']
			if control == None:
				control = lib61850.ControlObjectClient_create(uri_ref.path[1:], con)
			error = lib61850.ControlObjectClient_cancel(control)
			lib61850.ControlObjectClient_destroy(control)

		return error, control

def cb(a,b):
	print("cb called!")


if __name__=="__main__":
	logging.basicConfig(format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
		level=logging.DEBUG)
	# note the `logger` from above is now properly configured
	logger.debug("started")

	hostname = "localhost"
	tcpPort = 9102
	if len(sys.argv)>1:
		hostname = sys.argv[1]
	if len(sys.argv)>2:
		port = sys.argv[2]


	#error = lib61850.IedClientError()
	#con = lib61850.IedConnection_create()
	#lib61850.IedConnection_connect(con,ctypes.byref(error), hostname, tcpPort)
	#if (error.value == lib61850.IED_ERROR_OK):

	#	model = iec61850client.discovery(con)
		
	#	model, err = iec61850client.updateValueInModel(con, model, "IED3_SMVMUnn")
	#	print(err)
		#iec61850client.printrefs(val,"",len(path))


		#for key in model:
		#	logger.info("[" + model[key]['FC'] + "] " + model[key]['type'] + "\t" + key + "\t" + model[key]['value'])

		#for key in model:
		#	logger.error("[" + model[key]['FC'] + "] " + model[key]['type'] + "\t" + key + "\t" + model[key]['value'])
		#        /* Read RCB values */
		
		# rcb = lib61850.IedConnection_getRCBValues(con,ctypes.byref(error), "simpleIOGenericIO/LLN0.RP.EventsRCB01", None)

		# if error.value != lib61850.IED_ERROR_OK:
		# 	print("getRCBValues service error!")
		# 	exit()

		# #/* prepare the parameters of the RCP */
		# lib61850.ClientReportControlBlock_setResv(rcb, True)
		# lib61850.ClientReportControlBlock_setTrgOps(rcb, lib61850.TRG_OPT_DATA_CHANGED | lib61850.TRG_OPT_QUALITY_CHANGED | lib61850.TRG_OPT_GI)
		# lib61850.ClientReportControlBlock_setDataSetReference(rcb, "simpleIOGenericIO/LLN0$Events")
		# lib61850.ClientReportControlBlock_setRptEna(rcb, True)
		# lib61850.ClientReportControlBlock_setGI(rcb, True)
		# rptid = lib61850.ClientReportControlBlock_getRptId(rcb)

		# cbRef = lib61850.ReportCallbackFunction(cb)
		# lib61850.IedConnection_installReportHandler(con,"simpleIOGenericIO/LLN0.RP.EventsRCB",rptid,cbRef,None)
		# print("cb installed")
		# lib61850.IedConnection_setRCBValues(con,ctypes.byref(error), rcb, lib61850.RCB_ELEMENT_RESV | lib61850.RCB_ELEMENT_DATSET | lib61850.RCB_ELEMENT_TRG_OPS | lib61850.RCB_ELEMENT_RPT_ENA | lib61850.RCB_ELEMENT_GI, True)
		# print("setRCB called")
		# if error.value != lib61850.IED_ERROR_OK:
		# 	print("setRCBValues service error!")

		# time.sleep(1)

		# #/* Trigger GI Report */
		# lib61850.ClientReportControlBlock_setGI(rcb, True)
		# lib61850.IedConnection_setRCBValues(con,ctypes.byref(error), rcb, lib61850.RCB_ELEMENT_GI, True)
		# print("gi send, waiting 5 sec.")
		# if error.value != lib61850.IED_ERROR_OK:
		# 	print("Error triggering a GI report (code: %i)"% err)
		
		# time.sleep(5)

	#	lib61850.IedConnection_close(con)
	#else:
	#	logger.error("Failed to connect to %s:%i\n"%(hostname, tcpPort))
	#lib61850.IedConnection_destroy(con)

	cl = iec61850client()
	model = cl.getDatamodel(None,'localhost',102)
	cl.printrefs(model)

	#err = cl.registerReadValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/MMXU2.AvAPhs")
	#err = cl.registerReadValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/MMXU2.AvPhVPhs.mag.f")
	#err = cl.registerReadValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/TCTR1.Amp")
	#err = cl.registerReadValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/TCTR2.Amp.instMag.i")

	#err = cl.registerWriteValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/LLN0.MSVCB01.SvEna",True)
	#while True:
	#cl.poll()
	#cl.registerWriteValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/LLN0.MSVCB01.SvEna",False)
	#time.sleep(0.719)
	#cl.poll()
	#cl.registerWriteValue("iec61850://127.0.0.1:9102/IED3_SMVMUnn/LLN0.MSVCB01.SvEna",True)
	error, control = cl.select("iec61850://127.0.0.1:102/IED1_XCBRGenericIO/CSWI1.Pos", True)
	if error == 1:
		print("selected successfully")
	else:
		print("failed to select")	
	#control = None
	if cl.operate("iec61850://127.0.0.1:102/IED1_XCBRGenericIO/CSWI1.Pos", True, control) == 1:
		print("operated successfully")
	else:
		print("failed to operate")

