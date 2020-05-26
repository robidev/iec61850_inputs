#!/usr/bin/env python3

import os,sys
import ctypes
import time
import iec61850

from urllib.parse import urlparse

class iec61850client():

	def __init__(self, readvaluecallback = None):
		self.polling = {}
		self.connections = {}
		self.readvaluecallback = readvaluecallback


	@staticmethod
	def printValue(value):
		type = iec61850.MmsValue_getTypeString(value)
		if type == "boolean":
			return ("%r" % iec61850.MmsValue_getBoolean(value)), type
		if type == "array":
			return ("arr"), type
		if type ==  "bcd":
			return ("bcd"), type
		if type ==  "binary-time":
			return ("%i" % iec61850.MmsValue_getBinaryTimeAsUtcMs(value)), type
		if type == "bit-string":
			return ("%i" % iec61850.MmsValue_getBitStringAsInteger(value)), type
		if type == "access-error":
			return ("ACCESS ERROR"), type
		if type == "float":
			return ("%f" % iec61850.MmsValue_toFloat(value)), type
		if type == "generalized-time":
			return ("%u" % iec61850.MmsValue_toUnixTimestamp(value)), type
		if type == "integer":
			return ("%i" % iec61850.MmsValue_toInt64(value)), type
		if type == "oid":
			return ("OID ERROR"), type
		if type == "mms-string":
			return ("%s" % iec61850.MmsValue_toString(value)), type
		if type == "structure":
			return ("STRUCTURE"), type
		if type == "octet-string":
			len = iec61850.MmsValue_getOctetStringSize(value)
			buf = iec61850.MmsValue_getOctetStringBuffer(value)
			#magic cast to convert a swig pointer into a ctypes pointer, the int(buf) works, but why?
			buff = ctypes.cast(int(buf), ctypes.POINTER(ctypes.c_char))
			#allocate a buffer for the result
			res = bytearray(len)
			#create a pointer to the result buffer
			rptr = (ctypes.c_char * len).from_buffer(res)
			#copy the memory from the swig buffer to the result buffer
			ctypes.memmove(rptr, buff, len)
			return ("%s" % ''.join(format(x, '02x') for x in res)), type
		if type == "unsigned":
			return ("%u" % iec61850.MmsValue_toUint32(value)), type
		if type == "utc-time":
			return ("%u" % iec61850.MmsValue_getUtcTimeInMs(value)), type
		if type == "visible-string":
			return ("%s" % iec61850.MmsValue_toString(value)), type
		if type == "unknown(error)":
			return ("UNKNOWN ERROR"), type
		return ("CANNOT FIND TYPE"), type


	@staticmethod
	def printDataDirectory(con, doRef):
		[dataAttributes, error] = iec61850.IedConnection_getDataDirectoryFC(con, doRef)
		#print(error)
		model = {}

		if dataAttributes != None:
			dataAttribute = iec61850.LinkedList_getNext(dataAttributes)

			while dataAttribute != None:
				daName = iec61850.toCharP(dataAttribute.data)
				daRef = doRef+"."+daName[:-4]
				fcName = daName[-3:-1]
				#print(daRef)
				model[daRef] = {}
				model[daRef]['reftype'] = "DA"
				model[daRef]['FC'] = fcName

				submodel = iec61850client.printDataDirectory(con,daRef)
				if submodel:
					model[daRef]['type'] = "structure"
					model[daRef]['value'] = "{}"
					model.update(submodel)
					
				else:
					#read DA
					fc = iec61850.FunctionalConstraint_fromString(fcName) 
					[value, error] = iec61850.IedConnection_readObject(con, daRef, fc)
					if error == 0:
						model[daRef]['value'], model[daRef]['type'] = iec61850client.printValue(value)
						iec61850.MmsValue_delete(value)

				dataAttribute = iec61850.LinkedList_getNext(dataAttribute)

		iec61850.LinkedList_destroy(dataAttributes)
		return model


	@staticmethod
	def discovery(con):
		model = {}
		[deviceList, error] = iec61850.IedConnection_getLogicalDeviceList(con)
		device = iec61850.LinkedList_getNext(deviceList)
		while device:
			LD_name=iec61850.toCharP(device.data)
			#print("LD: %s" % LD_name)
			model[LD_name] = {}
			model[LD_name]['reftype'] = "LD"
			model[LD_name]['type'] = "structure"
			model[LD_name]['FC'] = "**"
			model[LD_name]['value'] = "{}"

			[logicalNodes, error] = iec61850.IedConnection_getLogicalDeviceDirectory(con, LD_name)
			logicalNode = iec61850.LinkedList_getNext(logicalNodes)
			while logicalNode:
				LN_name=iec61850.toCharP(logicalNode.data)
				#print(" LN: %s" % LN_name)
				model[LD_name+"/"+LN_name] = {}
				model[LD_name+"/"+LN_name]['reftype'] = "LN"
				model[LD_name+"/"+LN_name]['type'] = "structure"
				model[LD_name+"/"+LN_name]['FC'] = "**"
				model[LD_name+"/"+LN_name]['value'] = "{}"

				#[LNobjects, error] = iec61850.IedConnection_getLogicalNodeVariables(con, LD_name+"/"+LN_name)
				[LNobjects, error] = iec61850.IedConnection_getLogicalNodeDirectory(con, LD_name+"/"+LN_name,iec61850.ACSI_CLASS_DATA_OBJECT)
				LNobject = iec61850.LinkedList_getNext(LNobjects)
				while LNobject:
					Do = iec61850.toCharP(LNobject.data)
					#print("  DO: %s" % Do)
					model[LD_name+"/"+LN_name+"."+Do] = {}
					model[LD_name+"/"+LN_name+"."+Do]['reftype'] = "DO"
					model[LD_name+"/"+LN_name+"."+Do]['type'] = "structure"
					model[LD_name+"/"+LN_name+"."+Do]['FC'] = "**"
					model[LD_name+"/"+LN_name+"."+Do]['value'] = "{}"

					doRef = LD_name+"/"+LN_name+"."+Do

					submodel = iec61850client.printDataDirectory(con, doRef)
					model.update(submodel)

					LNobject = iec61850.LinkedList_getNext(LNobject)
				iec61850.LinkedList_destroy(LNobjects)

				[LNds, error] = iec61850.IedConnection_getLogicalNodeDirectory(con, LD_name+"/"+LN_name, iec61850.ACSI_CLASS_DATA_SET)
				LNds = iec61850.LinkedList_getNext(LNds)
				while LNds:
					isDel = None
					DSname = iec61850.toCharP(LNds.data)
					model[LD_name+"/"+LN_name+"$"+DSname] = {}
					model[LD_name+"/"+LN_name+"$"+DSname]['reftype'] = "DS"
					model[LD_name+"/"+LN_name+"$"+DSname]['type'] = "structure"
					model[LD_name+"/"+LN_name+"$"+DSname]['FC'] = "**"
					model[LD_name+"/"+LN_name+"$"+DSname]['value'] = "{}"

					#cannot pass the right type to isDeletable(last arg).. keeps complaining about 'bool *', and isDel = ctypes.pointer(ctypes.c_bool(False)) does not work
					[dataSetMembers, err] = iec61850.IedConnection_getDataSetDirectory(con, LD_name+"/"+LN_name+"."+DSname, isDel )  
					#all DS are assumed not deletable 
					#if isDel == None:
					#	print("  DS: %s, not Deletable" % DSname)
					#else:
					#	print("  DS: %s, is Deletable" % DSname)

					dataSetMemberRef = iec61850.LinkedList_getNext(dataSetMembers)

					i = 0
					while dataSetMemberRef:
						dsRef = iec61850.toCharP(dataSetMemberRef.data)
						DX = dsRef[:-4]
						FC = dsRef[-3:-1]
						#print("      %s" % DX)
						model[LD_name+"/"+LN_name+"$"+DSname+"^"+DX] = {}
						model[LD_name+"/"+LN_name+"$"+DSname+"^"+DX]['reftype'] = "DX"
						model[LD_name+"/"+LN_name+"$"+DSname+"^"+DX]['FC'] = FC
						model[LD_name+"/"+LN_name+"$"+DSname+"^"+DX]['type'] = "reference"
						model[LD_name+"/"+LN_name+"$"+DSname+"^"+DX]['value'] = str(i)
						dataSetMemberRef = iec61850.LinkedList_getNext(dataSetMemberRef)
						i += 1
					iec61850.LinkedList_destroy(dataSetMembers)
					LNds = iec61850.LinkedList_getNext(LNds)

				logicalNode = iec61850.LinkedList_getNext(logicalNode)
			iec61850.LinkedList_destroy(logicalNodes)
			device = iec61850.LinkedList_getNext(device)
		iec61850.LinkedList_destroy(deviceList)
		return model


	@staticmethod
	def getMMsValue(type, value):
		#allocate mmsvalue based on type
		if type == "visible-string":
			return iec61850.MmsValue_newVisibleString(value)
		if type == "boolean":
			return iec61850.MmsValue_newBoolean(value)
		if type == "integer":
			return iec61850.MmsValue_newInteger(int(value))
		print("ERROR: Mms value type not supported")
		return None


	@staticmethod
	def writeValue(con, model, ref, value):
		fc = iec61850.FunctionalConstraint_fromString(model[ref]['FC']) 
		mmsvalue = iec61850client.getMMsValue(model[ref]['type'],value)
		if not mmsvalue:
			return model,-1

		error = iec61850.IedConnection_writeObject(con, ref, fc, mmsvalue)
		iec61850.MmsValue_delete(mmsvalue)
		if error == 0:
			[RetValue, error] = iec61850.IedConnection_readObject(con, ref, fc)
			if error == 0:
				model[ref]['value'], model[ref]['type'] = iec61850client.printValue(RetValue)
				iec61850.MmsValue_delete(RetValue)
		return model, error
	

	@staticmethod
	def updateValueInModel(con, model, ref):
		if model[ref]['reftype'] != 'DA': 
			error = -1
			submodel = iec61850client.printDataDirectory(con,ref)
			if submodel:
				model.update(submodel)
				error = 0
		else:
			fc = iec61850.FunctionalConstraint_fromString(model[ref]['FC']) 
			[RetValue, error] = iec61850.IedConnection_readObject(con, ref, fc)
			if error == 0:
				model[ref]['value'], model[ref]['type'] = iec61850client.printValue(RetValue)
		return model, error


	# retrieve an active connection to IED, and up to date datamodel, stored in 'connections'
	def getIED(self, host, port):
		connections = self.connections
		if port == "" or port == None:
			port = 102

		tupl = host + ":" + str(port)
		if tupl in connections and connections[tupl]["con"] != None:
			if not connections[tupl]["model"]:
				con = connections[tupl]["con"]
				model = discovery(con)
				if model: #if model is not empty
					# store the model
					connections[tupl]["model"] = model
					return 0
				else:
					return -1
		
		if not tupl in connections:
			connections[tupl] = {}
			connections[tupl]["con"] = None
			connections[tupl]["model"] = {}

		con = iec61850.IedConnection_create()
		error = iec61850.IedConnection_connect(con, host, port)
		if (error == iec61850.IED_ERROR_OK):
			# store the active connection
			connections[tupl]["con"] = con
			# read the model
			model = iec61850client.discovery(con)
			if model: #if model is not empty
				# store the model
				connections[tupl]["model"] = model
				return 0
			else:
				return -1
		else:
			return -1


	# write a value to an active connection
	def registerWriteValue(self, ref, value):
		polling = self.polling
		connections = self.connections

		uri_ref = urlparse(ref)
		port = uri_ref.port
		if port == "" or port == None:
			port = 102

		tupl = uri_ref.hostname + ":" + str(port)

		if uri_ref.scheme != "iec61850":
			print("ERROR: incorrect scheme, only iec61860 is supported, not %s" % uri_ref.scheme)
			return -1

		err = self.getIED(uri_ref.hostname, port)
		if err == 0:
			con = connections[tupl]['con']
			if not con:
				print("ERROR: no valid connection")
				return -1			

			model = connections[tupl]['model']
			if not model:
				print("ERROR: no valid model")
				return -1

			if uri_ref.path[1:] in model:
				model, error = iec61850client.writeValue(con, model, uri_ref.path[1:], value)
				if error == 0:
					connections[tupl]['model'] = model
					print("Value '%s' written to %s" % (model[uri_ref.path[1:]]['value'], ref) )

					if self.readvaluecallback != None:
						self.readvaluecallback(key, model[uri_ref.path[1:]])

					return 0
				else:
					print("ERROR: could not write '%s' to %s" % (model[uri_ref.path[1:]], ref))
			else:
				print("ERROR: could not find %s in model" % uri_ref.path[1:])
		else:
			print("ERROR: no connection to IED: %s:%s" % (uri_ref.hostname, port) )
		return -1


	# register value for reading
	def registerReadValue(self,ref):
		# check if present in dataset/report, and subscribe is impossible for now due to lacking reportcallback implementation in SWIG
		# i.e. there is no valid way to pass a function-pointer for the callback in IedConnection_installReportHandler
		# so periodic poll
		polling = self.polling
		connections = self.connections

		uri_ref = urlparse(ref)
		port = uri_ref.port
		if port == "" or port == None:
			port = 102

		tupl = uri_ref.hostname + ":" + str(port)

		if uri_ref.scheme != "iec61850":
			print("ERROR: incorrect scheme, only iec61860 is supported, not %s" % uri_ref.scheme)
			return -1

		err = self.getIED(uri_ref.hostname, port)
		if err == 0:
			model = connections[tupl]['model']
			if uri_ref.path[1:] in model:
				polling[ref] = 1
				return 0
			else:
				print("ERROR: could not find %s in model" % uri_ref.path[1:])
		else:
			print("ERROR: no connection to IED: %s:%s" % (uri_ref.hostname, port) )
		return -1


	# retrieve all registered values by polling
	def poll(self):
		polling = self.polling
		connections = self.connections
		for key in polling:
			uri_ref = urlparse(key)
			tupl = uri_ref.hostname + ":" + str(uri_ref.port)
			con = connections[tupl]['con']
			model = connections[tupl]['model']
			if con and model:
				model, err = iec61850client.updateValueInModel(con, model, uri_ref.path[1:])
				if err == 0:
					connections[tupl]['model'] = model
					print("value:%s read from key: %s" % (model[uri_ref.path[1:]]['value'], key))
					#call function with ref+value
					if self.readvaluecallback != None:
						self.readvaluecallback(key, model[uri_ref.path[1:]])

				else:
					print("ERROR: model not updated for %s" % key)


if __name__=="__main__":
	hostname = "localhost"
	tcpPort = 102
	if len(sys.argv)>1:
		hostname = sys.argv[1]
	if len(sys.argv)>2:
		port = sys.argv[2]

	con = iec61850.IedConnection_create()
	error = iec61850.IedConnection_connect(con, hostname, tcpPort)
	if (error == iec61850.IED_ERROR_OK):
		model = iec61850client.discovery(con)
		for key in model:
			print("[" + model[key]['FC'] + "] " + model[key]['type'] + "\t" + key + "\t" + model[key]['value'])
		iec61850.IedConnection_close(con)
		#for key in model:
		#	print("[" + model[key]['FC'] + "] " + model[key]['type'] + "\t" + key + "\t" + model[key]['value'])
	else:
		print("Failed to connect to %s:%i\n"%(hostname, tcpPort))
	iec61850.IedConnection_destroy(con)

	cl = iec61850client()

	err = cl.registerReadValue("iec61850://127.0.0.1:102/TEMPLATEMUnn/TVTR1.Vol.instMag.i")
	err = cl.registerReadValue("iec61850://127.0.0.1:102/TEMPLATEMUnn/TVTR2.Vol.instMag.i")
	err = cl.registerReadValue("iec61850://127.0.0.1:102/TEMPLATEMUnn/TVTR3.Vol.instMag.i")
	err = cl.registerReadValue("iec61850://127.0.0.1:102/TEMPLATEMUnn/TVTR4.Vol.instMag.i")

	err = cl.registerWriteValue("iec61850://127.0.0.1/TEMPLATEMUnn/LLN0.MSVCB01.SvEna",True)
	#while True:
	cl.poll()
	cl.registerWriteValue("iec61850://127.0.0.1:102/TEMPLATEMUnn/LLN0.MSVCB01.SvEna",False)
	time.sleep(0.719)
	cl.poll()
	cl.registerWriteValue("iec61850://127.0.0.1:102/TEMPLATEMUnn/LLN0.MSVCB01.SvEna",True)
