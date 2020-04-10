# iec61850_inputs
An implementation of an IED using the libiec61850 library

This project aims to simulate some options according to the IEC61850 standard, using only the SCL file. 
The main goal is feeding the SCL file into the system, and the system instantiating all IEDS, LD's, 
LN's and services accordingly.

The current implementation contains 3 IED's, a SMV publisher(merging unit), a protection IED containting 
a PTOC that will trigger on an overcurrent, and trigger the PTRC in the same LD. The PTOC is fed by 
sampled values from the merging unit. The PTRC publishes a trip command by GOOSE.
a XCBR IED subscribes to GOOSE messages from the PTRC, and publishes back its stVal.

Process values are static simulations, and not connected to a real process-simulation.

The same executable should be usable to describe all IED's, only the config-file should change, allowing
a flexible setup that can be used in a docker container for a more complete simulation setup of a substation

# Getting started:

create a working folder, e.g. substation;  
  
`# mkdir ~/substation`  
`# cd ~/substation`  
  
get the necesarry libraries;  
`git clone git clone https://github.com/mz-automation/libiec61850.git`  
`git clone https://github.com/robidev/iec61850_inputs.git`  
`# cd iec61850_inputs`  
  
generate the config files;  
`# make model`  
  
generate the compose file;  
`# make compose`  
  
run the compose file;  
`# sudo docker-compose -f substation.yml up`  
