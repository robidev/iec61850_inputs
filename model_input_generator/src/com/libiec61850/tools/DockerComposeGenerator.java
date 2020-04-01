package com.libiec61850.tools;

/*
 *  DynamicModelGenerator.java
 *
 *  Copyright 2014-2016 Michael Zillgith
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
import java.util.List;
import java.util.Collection;

import com.libiec61850.scl.DataAttributeDefinition;
import com.libiec61850.scl.SclParser;
import com.libiec61850.scl.SclParserException;
import com.libiec61850.scl.communication.ConnectedAP;
import com.libiec61850.scl.communication.PhyComAddress;
import com.libiec61850.scl.model.AccessPoint;
import com.libiec61850.scl.model.DataAttribute;
import com.libiec61850.scl.model.DataModelValue;
import com.libiec61850.scl.model.DataObject;
import com.libiec61850.scl.model.DataSet;
import com.libiec61850.scl.model.Inputs;
import com.libiec61850.scl.model.ExtRef;
import com.libiec61850.scl.model.FunctionalConstraintData;
import com.libiec61850.scl.model.GSEControl;
import com.libiec61850.scl.model.SampledValueControl;
import com.libiec61850.scl.model.IED;
import com.libiec61850.scl.model.Log;
import com.libiec61850.scl.model.LogControl;
import com.libiec61850.scl.model.LogicalDevice;
import com.libiec61850.scl.model.LogicalNode;
import com.libiec61850.scl.model.ReportControlBlock;
import com.libiec61850.scl.model.SettingControl;

public class DockerComposeGenerator {

    public DockerComposeGenerator(InputStream stream, String icdFile, PrintStream output) 
    		throws SclParserException {

        SclParser sclParser = new SclParser(stream);
        
        Collection<IED> ieds = sclParser.getIeds();

        if(ieds.size() == 0)
          throw new SclParserException("No data model present in SCL file! Exit.");

        for(IED ied : ieds) {    
          output.println("service: iedname");
         	List<AccessPoint> accessPoints = ied.getAccessPoints();

          if(accessPoints.size() == 0)
          	throw new SclParserException("No valid access point found!");
          
          for(AccessPoint accessPoint : accessPoints){
            output.println("interface: AP");
            ConnectedAP connectedAP = sclParser.getConnectedAP(ied, accessPoint.getName());
            //server definition
            //load server from AP and iedname, usind scd file
            //use defined ip/mac/hostname
            //define network interfaces
          }

          output.println("");

        }
    }




    public static void main(String[] args) throws FileNotFoundException {
        System.out.println("Docker compose generator");

        if (args.length < 1) {
            System.out.println("Usage: gencompose <ICD file> ");
            System.exit(1);
        }

        String icdFile = args[0];

        PrintStream outputStream = System.out;
        
        String accessPointName = null;
        String iedName = null;
        

        InputStream stream = new FileInputStream(icdFile);

        try {
			new DockerComposeGenerator(stream, icdFile, outputStream);
		} catch (SclParserException e) {
			System.err.println("ERROR: " + e.getMessage());
		}
    }


}
