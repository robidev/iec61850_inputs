package com.libiec61850.tools;

/*
 *  StaticModelGenerator.java
 *
 *  Copyright 2013-2016 Michael Zillgith
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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintStream;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.LinkedList;
import java.util.List;
import java.util.Collection;

import com.libiec61850.scl.SclParser;
import com.libiec61850.scl.SclParserException;
import com.libiec61850.scl.communication.Address;
import com.libiec61850.scl.communication.Communication;
import com.libiec61850.scl.communication.ConnectedAP;
import com.libiec61850.scl.communication.GSE;
import com.libiec61850.scl.communication.P;
import com.libiec61850.scl.communication.PhyComAddress;
import com.libiec61850.scl.communication.SubNetwork;
import com.libiec61850.scl.model.AccessPoint;
import com.libiec61850.scl.model.ClientLN;
import com.libiec61850.scl.model.DataAttribute;
import com.libiec61850.scl.model.DataModelValue;
import com.libiec61850.scl.model.DataObject;
import com.libiec61850.scl.model.DataSet;
import com.libiec61850.scl.model.FunctionalConstraint;
import com.libiec61850.scl.model.FunctionalConstraintData;
import com.libiec61850.scl.model.GSEControl;
import com.libiec61850.scl.model.Inputs;
import com.libiec61850.scl.model.ExtRef;
import com.libiec61850.scl.model.IED;
import com.libiec61850.scl.model.Log;
import com.libiec61850.scl.model.LogControl;
import com.libiec61850.scl.model.LogicalDevice;
import com.libiec61850.scl.model.LogicalNode;
import com.libiec61850.scl.model.ReportControlBlock;
import com.libiec61850.scl.model.RptEnabled;
import com.libiec61850.scl.model.SampledValueControl;
import com.libiec61850.scl.model.Server;
import com.libiec61850.scl.model.SettingControl;
import com.libiec61850.scl.model.TriggerOptions;

public class StaticModelGenerator_input {

    private List<String> variablesList;
    private PrintStream cOut;
    private PrintStream hOut;

    private List<String> inputsNames;
    private String subRefs_name = null;

    private Communication communication;

    private ConnectedAP connectedAP;
    
    private IED ied;
    private AccessPoint accessPoint;

	private String outputFileName;
	private String hDefineName;
	private String modelPrefix;
	private boolean initializeOnce;
	
	private SclParser sclParser;

    public StaticModelGenerator_input(InputStream stream, String icdFile, PrintStream cOut, PrintStream hOut,
    		String outputFileName, String iedName, String accessPointName, String modelPrefix,
			boolean initializeOnce) throws SclParserException 
    {
        this.cOut = cOut;
        this.hOut = hOut;
        
        
        sclParser = new SclParser(stream);

		this.outputFileName = outputFileName;
		this.hDefineName = outputFileName.toUpperCase().replace( '.', '_' ).replace( '-', '_' ) + "_H_";
		this.modelPrefix = modelPrefix;
		this.initializeOnce = initializeOnce;

		if( hDefineName.lastIndexOf( '/' ) >= 0 )
		{
			hDefineName = hDefineName.substring( hDefineName.lastIndexOf( '/' ) + 1 );
		}

        ied = null; 
        
        if (iedName == null)
        	ied = sclParser.getFirstIed();
        else
        	ied = sclParser.getIedByName(iedName);

        if (ied == null)
            System.out.println("IED model not found in SCL file! Exit.");
       
        accessPoint = null;

        if (accessPointName == null)
        	accessPoint = ied.getFirstAccessPoint();
        else
        	accessPoint = ied.getAccessPointByName(accessPointName);

        connectedAP = sclParser.getConnectedAP(ied, accessPoint.getName());

        printDeviceModelDefinitions();

    }

    private static String toMmsString(String iecString) {
        return iecString.replace('.', '$');
    }

    public static void main(String[] args) throws FileNotFoundException  {
        if (args.length < 1) {
            System.out.println("Usage: genmodel <ICD file>  [-ied  <ied-name>] [-ap <access-point-name>] [-out <output-name>] [-modelprefix <model-prefix>]");
            System.exit(1);
        }

        String icdFile = args[0];

		String outputFileName = "static_input";
        
        String accessPointName = null;
        String iedName = null;
		String modelPrefix = "iedInputModel";
		boolean initializeOnce = false;
        
        if (args.length > 1) {
        	for (int i = 1; i < args.length; i++) {
        		if (args[i].equals("-ap")) {
        			accessPointName = args[i+1];
        			
        			System.out.println("Select access point " + accessPointName);
        			
        			i++;
        		}
        		else if (args[i].equals("-ied")) {
        			iedName = args[i+1];
        			
        			System.out.println("Select IED " + iedName);
        			
        			i++;
        			
        		}
        		else if (args[i].equals("-out")) {
					outputFileName = args[i+1];

        			System.out.println("Select Output File " + outputFileName);

        			i++;
        			
        		}
        		else if (args[i].equals("-modelprefix")) {
					modelPrefix = args[i+1];

        			System.out.println("Select Model Prefix " + modelPrefix);

        			i++;
        			
        		}
        		else if (args[i].equals("-initializeonce")) {
					initializeOnce = true;

        			System.out.println("Select Initialize Once");

        		}
        		else {
        			 System.out.println("Unknown option: \"" + args[i] + "\"");
        		}
        	}
        	      
        }

        PrintStream cOutStream = new PrintStream(new FileOutputStream(new File(outputFileName + ".c")));
        PrintStream hOutStream = new PrintStream(new FileOutputStream(new File(outputFileName + ".h")));

		System.out.println("Select ICD File " + icdFile);
        InputStream stream = new FileInputStream(icdFile);

        try {
			new StaticModelGenerator_input(stream, icdFile, cOutStream, hOutStream, outputFileName, iedName, accessPointName, modelPrefix, initializeOnce);
        } catch (SclParserException e) {
			System.err.println("ERROR: " + e.getMessage());
		}
    }

    private void printVariablePointerDefines() {
        hOut.println("\n\n");

        for (String variableName : variablesList) {
			String name = modelPrefix.toUpperCase() + variableName.substring( modelPrefix.length() );
            hOut.println("#define " + name + " (&" + variableName + ")");
        }
    }

    private String getLogicalDeviceInst(LogicalDevice logicalDevice) {
    	return logicalDevice.getInst();
    }
    
    private void printDeviceModelDefinitions() {

        List<LogicalDevice> logicalDevices = accessPoint.getServer().getLogicalDevices();


        printInputs();
        printSubscribeDataSets(null);
        
        String firstLogicalDeviceName = logicalDevices.get(0).getInst();
        cOut.println("\nIedModel_inputs " + modelPrefix + " = {");

        if (inputsNames.size() > 0)
            cOut.println("    &" + inputsNames.get(0) + ",");
        else
            cOut.println("    NULL,");
        
        if (subRefs_name != null)
            cOut.println("    &" + subRefs_name + ",");
        else
            cOut.println("    NULL,");

        
        cOut.println("    };");
    }


    private void printInputs() {
        inputsNames = new LinkedList<String>();
        List<LogicalDevice> logicalDevices = accessPoint.getServer().getLogicalDevices();
        for (LogicalDevice logicalDevice : logicalDevices) {

            for (LogicalNode logicalNode : logicalDevice.getLogicalNodes()) {

                List<Inputs> inputs = logicalNode.getInputs();

                for (Inputs input : inputs) {
                    String _inputsName = modelPrefix + "_" + logicalDevice.getInst() + "_" + logicalNode.getName() + "_inputs";
                    inputsNames.add(_inputsName);
                }
            }
        }

        /* print inputs declarations */
        cOut.println();
        for (String inputName : inputsNames) {
        	cOut.println("extern Input " + inputName + ";");
        }
        cOut.println();


        /* print extrefs */
        int inputsNameListIndex = 0;
        
        for (LogicalDevice logicalDevice : logicalDevices) {

            for (LogicalNode logicalNode : logicalDevice.getLogicalNodes()) {

                List<Inputs> inputs = logicalNode.getInputs();

                for (Inputs input : inputs) {

                    String inputVarName = inputsNames.get(inputsNameListIndex++);

                    int extRefCount = 0;

                    int numberOfExtRef = input.getExtRef().size();

                    cOut.println();
                    for (ExtRef extref : input.getExtRef()) {
                        String inputEntryName = inputVarName + "_extRef" + extRefCount;

                        cOut.println("extern InputEntry " + inputEntryName + ";");

                        extRefCount++;
                    }
                    cOut.println();

                    extRefCount = 0;

                    for (ExtRef extref : input.getExtRef()) {
                        String inputEntryName = inputVarName + "_extRef" + extRefCount;

                        cOut.println("InputEntry " + inputEntryName + " = {");
                        cOut.println("  \"" + extref.getDesc() + "\",");
                        cOut.println("  \"" + extref.ExtRefToString() + "\",");

                        cOut.println("  \"" + extref.getIntAddr() + "\",");
                        cOut.println("  \"" + extref.getServiceType() + "\",");
                        cOut.println("  \"" + extref.srcToString() + "\",");
                        cOut.println("  NULL,"); // MmsValue* value;

                        if (extRefCount + 1 < numberOfExtRef)
                            cOut.println("  &" + inputVarName + "_extRef" + (extRefCount + 1));
                        else
                            cOut.println("  NULL");

                        cOut.println("};\n");

                        extRefCount++;
                    }

                    cOut.println("Input " + inputVarName + " = {");

                    String lnVariableName = modelPrefix + "_" + logicalDevice.getInst() + "_" + logicalNode.getName();
                    cOut.println("  &" + lnVariableName + ",");
                    cOut.println("  " + numberOfExtRef + ",");
                    cOut.println("  &" + inputVarName + "_extRef0,");
                                        
                    if (inputsNameListIndex < inputsNames.size()) {
                    	 String nextInputVariableName = inputsNames.get(inputsNameListIndex);
                    	cOut.println("  &" + nextInputVariableName);
                    }
                    else
                    	cOut.println("  NULL");
                    
                    cOut.println("};");

                }
            }
        }
    }




    private void printSubscribeDataSets(String accessPointName_local) {

        ConnectedAP connectedAP_local;
        List<String> dataSetElementNames_local = new LinkedList<String>();

        List<String> dataSetNames_ = new LinkedList<String>();
        List<String> mmsVariableNames = new LinkedList<String>();

        List<String> AppIDs = new LinkedList<String>();
        List<String> ethAddrs = new LinkedList<String>();
        List<String> IDs = new LinkedList<String>();
        List<String> cbRefs = new LinkedList<String>();

        AccessPoint accessPoint_local;
        Collection<IED> ieds;

        ieds = sclParser.getIeds();
        System.out.println("subscribe");

        for (IED ied_local : ieds){
            System.out.println("IED:" + ied_local.getName());
            accessPoint_local = null;

            if (accessPointName_local == null)
                accessPoint_local = ied_local.getFirstAccessPoint();
            else
                accessPoint_local = ied_local.getAccessPointByName(accessPointName_local);

            connectedAP_local = sclParser.getConnectedAP(ied_local, accessPoint_local.getName());
            List<LogicalDevice> logicalDevices = accessPoint_local.getServer().getLogicalDevices();

            /* create list of data set names */
            for (LogicalDevice logicalDevice : logicalDevices) {

                for (LogicalNode logicalNode : logicalDevice.getLogicalNodes()) {

                    List<DataSet> dataSets = logicalNode.getDataSets();

                    for (DataSet dataSet : dataSets) {

                        String dataSetVariableName = ied_local.getName() + "_ds_" + logicalDevice.getInst() + "_" + logicalNode.getName() + "_" + dataSet.getName();
                        String datasetName_ = dataSet.getName();

                        String AppID = "0";
                        String ethAddr =  "0,0,0,0,0,0";
                        String cbRef = "NULL";
                        String ID = "NULL";
                        for (LogicalNode logicalNode2 : logicalDevice.getLogicalNodes()) {
                            List<GSEControl> gseControlBlocks = logicalNode2.getGSEControlBlocks();           
                            for (GSEControl gseControlBlock : gseControlBlocks) {
                                if(datasetName_.equals(gseControlBlock.getDataSet()))
                                {
                                    PhyComAddress gseAddress = connectedAP_local.lookupGSEAddress(logicalDevice.getInst(), gseControlBlock.getName());
                                    if (gseAddress != null) {
                                        AppID = "" + gseAddress.getAppId();

                                        ethAddr = "";
                                        for (int i = 0; i < 6; i++) {
                                            ethAddr += "0x" + Integer.toHexString(gseAddress.getMacAddress()[i]);
                                            if (i < 5)
                                                ethAddr += ", ";
                                        }
                                    }
                                    
                                    if (gseControlBlock.getAppID() != null)
                                        ID = "\"" + gseControlBlock.getAppID() + "\"";
                                    
                                    cbRef = "\"" + gseControlBlock.getName() + "\"";
                                }
                            }

                            List<SampledValueControl> svControlBlocks = logicalNode2.getSampledValueControlBlocks();                      
                            for (SampledValueControl svCB : svControlBlocks) {
                                if(datasetName_.equals(svCB.getDatSet()))
                                {
                                    PhyComAddress svAddress = connectedAP.lookupSMVAddress( logicalDevice.getInst(), svCB.getName());                            
                                    if (svAddress != null) {
                                        AppID = "" + svAddress.getAppId();
                                        
                                        ethAddr = "";
                                        for (int i = 0; i < 6; i++) {
                                            ethAddr += "0x" + Integer.toHexString(svAddress.getMacAddress()[i]);
                                            if (i < 5)
                                                ethAddr += ", ";
                                        }
                                    }
                                    
                                    if (svCB.getSmvID() != null)
                                        ID = "\"" + svCB.getSmvID() + "\"";

                                    cbRef = "\"" + svCB.getName() + "\"";
                                }
                            }
                        }

                        int index = 0;
                        for (FunctionalConstraintData fcda : dataSet.getFcda()) {
                            dataSetElementNames_local.add(dataSetVariableName + (index++));

                            String mmsVariableName = ied_local.getName() + fcda.getLdInstance() + "/";
                            if (fcda.getPrefix() != null)
                                mmsVariableName += fcda.getPrefix();
                            mmsVariableName += fcda.getLnClass();
                            if (fcda.getLnInstance() != null)
                                mmsVariableName += fcda.getLnInstance();
                            mmsVariableName += "$" + fcda.getFc().toString();
                            mmsVariableName += "$" + toMmsString(fcda.getDoName());
                            if (fcda.getDaName() != null)
                                mmsVariableName += "$" + toMmsString(fcda.getDaName());
                            mmsVariableNames.add(mmsVariableName);

                            dataSetNames_.add(datasetName_);

                            AppIDs.add(AppID);
                            ethAddrs.add(ethAddr);
                            IDs.add(ID);
                            cbRefs.add(cbRef);

                        }
                    }
                }
            }
        }
        int dataSetElementNameListIndex = 0;
        for(String dataSetName : dataSetElementNames_local)
        {
            cOut.println("extern SubscriberEntry " + dataSetName + ";");
        }
        cOut.println("");

        dataSetElementNameListIndex = 0;
        for(String dataSetName : dataSetElementNames_local)
        {
            cOut.println("SubscriberEntry " + dataSetElementNames_local.get(dataSetElementNameListIndex) + " = {");

            cOut.println("  \"" + mmsVariableNames.get(dataSetElementNameListIndex) + "\",");
            cOut.println("  \"" + dataSetNames_.get(dataSetElementNameListIndex) + "\",");
            cOut.println("  " + AppIDs.get(dataSetElementNameListIndex) + ",");
            cOut.println("  " + cbRefs.get(dataSetElementNameListIndex) + ",");
            cOut.println("  "+ IDs.get(dataSetElementNameListIndex) + ",");
            cOut.println("  {" + ethAddrs.get(dataSetElementNameListIndex) + "},");

            dataSetElementNameListIndex++;
            if(dataSetElementNameListIndex < dataSetElementNames_local.size()) {
                String nextDataSetElementVariableName = dataSetElementNames_local.get(dataSetElementNameListIndex);
                cOut.println("  &" + nextDataSetElementVariableName + ",");
            }
            else
                cOut.println("  NULL,");

            cOut.println("};");
        }
        cOut.println("");

        if(subRefs_name == null)//get first entry in the list
            subRefs_name = dataSetElementNames_local.get(0);
    }

}
