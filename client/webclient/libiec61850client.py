#!/usr/bin/python
import os,sys
import iec61850
import ctypes
import inspect

def printValue(value):
	type = iec61850.MmsValue_getTypeString(value)
	#print(type)
	if type == "boolean":
		return ("%r" % iec61850.MmsValue_getBoolean(value))
	if type == "array":
		return ("arr")
	if type ==  "bcd":
		return ("bcd")
	if type ==  "binary-time":
		return ("%i" % iec61850.MmsValue_getBinaryTimeAsUtcMs(value))
	if type == "bit-string":
		return ("%i" % iec61850.MmsValue_getBitStringAsInteger(value))
	if type == "access-error":
		return ("ACCESS ERROR")
	if type == "float":
		return ("%f" % iec61850.MmsValue_toFloat(value))
	if type == "generalized-time":
		return ("%u" % iec61850.MmsValue_toUnixTimestamp(value))
	if type == "integer":
		return ("%i" % iec61850.MmsValue_toInt64(value))
	if type == "oid":
		return ("OID ERROR")
	if type == "mms-string":
		return ("%s" % iec61850.MmsValue_toString(value))
	if type == "structure":
		print("STRUCTURE")
	if type == "octet-string":
		#TODO: print as hex
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

		return ("%s" % ' '.join(format(x, '02x') for x in res)) # string copy
	if type == "unsigned":
		return ("%u" % iec61850.MmsValue_toUint32(value))
	if type == "utc-time":
		return ("%u" % iec61850.MmsValue_getUtcTimeInMs(value))
	if type == "visible-string":
		return ("%s" % iec61850.MmsValue_toString(value))
	if type == "unknown(error)":
		return ("UNKNOWN ERROR")
	return ("CANNOT FIND TYPE")



def printDataDirectory(con, doRef):
	[dataAttributes, error] = iec61850.IedConnection_getDataDirectoryFC(con, doRef)
	#print(error)
	model = {}

	if dataAttributes != None:
		dataAttribute = iec61850.LinkedList_getNext(dataAttributes)

		while dataAttribute != None:
			daName = iec61850.toCharP(dataAttribute.data)
			daRef = doRef+"."+daName[:-4]
			#print(daRef)

			submodel = printDataDirectory(con,daRef)
			if submodel:
				model[daRef] = "DA"
				model.update(submodel)
			else:
				#read DA
				fc = iec61850.FunctionalConstraint_fromString(daName[-3:-1]) 
				[value, error] = iec61850.IedConnection_readObject(con, daRef, fc)
				model[daRef] = printValue(value)
				iec61850.MmsValue_delete(value)

			dataAttribute = iec61850.LinkedList_getNext(dataAttribute)

	iec61850.LinkedList_destroy(dataAttributes)
	return model


def discovery(con):
	model = {}
	[deviceList, error] = iec61850.IedConnection_getLogicalDeviceList(con)
	device = iec61850.LinkedList_getNext(deviceList)
	while device:
		LD_name=iec61850.toCharP(device.data)
		#print("LD: %s" % LD_name)
		model[LD_name] = "LD"

		[logicalNodes, error] = iec61850.IedConnection_getLogicalDeviceDirectory(con, LD_name)
		logicalNode = iec61850.LinkedList_getNext(logicalNodes)
		while logicalNode:
			LN_name=iec61850.toCharP(logicalNode.data)
			#print(" LN: %s" % LN_name)
			model[LD_name+"/"+LN_name] = "LN"

			#[LNobjects, error] = iec61850.IedConnection_getLogicalNodeVariables(con, LD_name+"/"+LN_name)
			[LNobjects, error] = iec61850.IedConnection_getLogicalNodeDirectory(con, LD_name+"/"+LN_name,iec61850.ACSI_CLASS_DATA_OBJECT)
			LNobject = iec61850.LinkedList_getNext(LNobjects)
			while LNobject:
				Do = iec61850.toCharP(LNobject.data)
				#print("  DO: %s" % Do)
				model[LD_name+"/"+LN_name+"."+Do] = "DO"

				doRef = LD_name+"/"+LN_name+"."+Do

				submodel = printDataDirectory(con, doRef)
				model.update(submodel)

				LNobject = iec61850.LinkedList_getNext(LNobject)
			iec61850.LinkedList_destroy(LNobjects)

			[LNds, error] = iec61850.IedConnection_getLogicalNodeDirectory(con, LD_name+"/"+LN_name, iec61850.ACSI_CLASS_DATA_SET)
			LNds = iec61850.LinkedList_getNext(LNds)
			while LNds:
				isDel = None
				DSname = iec61850.toCharP(LNds.data)
				model[LD_name+"/"+LN_name+"$"+DSname] = "DS"

				#cannot pass the right type to isDeletable(last arg).. keeps complaining about 'bool *', and isDel = ctypes.pointer(ctypes.c_bool(False)) does not work
				[dataSetMembers, err] = iec61850.IedConnection_getDataSetDirectory(con, LD_name+"/"+LN_name+"."+DSname, isDel )  
				#all DS are assumed not deletable 
				#if isDel == None:
				#	print("  DS: %s, not Deletable" % DSname)
				#else:
				#	print("  DS: %s, is Deletable" % DSname)

				dataSetMemberRef = iec61850.LinkedList_getNext(dataSetMembers)
				while dataSetMemberRef:
					DX = iec61850.toCharP(dataSetMemberRef.data)
					#print("      %s" % DX)
					model[LD_name+"/"+LN_name+"$"+DSname+"^"+DX] = "DX"
					dataSetMemberRef = iec61850.LinkedList_getNext(dataSetMemberRef)
				iec61850.LinkedList_destroy(dataSetMembers)
				LNds = iec61850.LinkedList_getNext(LNds)

			logicalNode = iec61850.LinkedList_getNext(logicalNode)
		iec61850.LinkedList_destroy(logicalNodes)
		device = iec61850.LinkedList_getNext(device)
	iec61850.LinkedList_destroy(deviceList)
	return model


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
		model = discovery(con)
		iec61850.IedConnection_close(con)
		for key in model:
			print(key + "\t" + model[key])
	else:
		print("Failed to connect to %s:%i\n"%(hostname, tcpPort))
	iec61850.IedConnection_destroy(con)
