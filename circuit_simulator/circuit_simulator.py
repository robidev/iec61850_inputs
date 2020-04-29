#!/usr/B1/env python
# export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
#
# this python program will simulate the primary process using PySpice with the ngspice library interactively
# it takes input from the SCL, and generates the circuit based on the substation-section using assumed values and an ideal model 
# allthough this ideal model does not behave (yet) like a real substation, it is good enough for simple protection use-cases
# the simulator can be influenced with values from the breakers and switches, and can feed the calculated voltage and current
# back to the merging-units, creating a closed loop process simulation
#

import os
import numpy
import pprint
import matplotlib.pyplot as plt
import xmlschema


from PySpice.Probe.Plot import plot
import PySpice.Logging.Logging as Logging
from PySpice.Spice.Netlist import Circuit
from PySpice.Spice.NgSpice.Shared import NgSpiceShared



circuit = """
.title substation model
* substation components
#.options interp  ; strongly reduces memory requirements
.save none       ; ensure only last step is kept each iteration
.tran 19us 3600s uic; run for an hour max, with 100 samples per cycle (201u stepsize does not distort, 200 does...)
* 
.subckt IFL A1 B1 C1 vss=100000 freq=50
vphaseA A 0 dc 0 ac 1 sin(0 {vss} {freq} 0 0 0)
vphaseB B 0 dc 0 ac 1 sin(0 {vss} {freq} 0 0 120)
vphaseC C 0 dc 0 ac 1 sin(0 {vss} {freq} 0 0 240)
* transmission line
rline1 A A1 0.01
rline2 B B1 0.01
rline3 C C1 0.01
.ends IFL
*
.subckt load A1 B1 C1 rload=1000
* load TODO: add capacitor and inductor, maybe nonlineair load, or inverse (constant power)
r1 A1 0 {rload}
r2 B1 0 {rload}
r3 C1 0 {rload}
.ends load
*
.subckt DIS A1 B1 C1 A2 B2 C2
sdisA A1 A2 sig 0
sdisB B1 B2 sig 0
sdisC C1 C2 sig 0
vsig sig 0 dc 0 external
rsig sig 0 10000
.ends DIS
*
.subckt CBR A1 B1 C1 A2 B2 C2
scbrA A1 A2 sig 0
scbrB B1 B2 sig 0
scbrC C1 C2 sig 0
vsig sig 0 dc 0 external
rsig sig 0 10000
.ends CBR
*
.subckt CTR A1 B1 C1 A2 B2 C2
vctrA A1 A2 dc 0
vctrB B1 B2 dc 0
vctrC C1 C2 dc 0
.ends CTR
*
.subckt VTR A1 B1 C1
*VTR does not do anything, but is needed for substation netlist consistency
rvtrA A1 0 1000000
rvtrB B1 0 1000000
rvtrC C1 0 1000000
.ends VTR
*
.subckt PTR A1 B1 C1 A2 B2 C2 inductor1=8 inductor2=0.5 coupling=1
* transformer 4:1 turns ratio specified by 8:0.5 inductance ratio (16:1), coupling is ideal (1)
l1pri A1 i1 {inductor1}
l2pri B1 i1 {inductor1}
l3pri C1 i1 {inductor1}
l1sec A2 i2 {inductor2}
l2sec B2 i2 {inductor2}
l3sec C2 i2 {inductor2}
k1 l1pri l1sec {coupling}
k2 l2pri l2sec {coupling}
k3 l3pri l3sec {coupling}
.ends PTR
*
*
* example substation description
*xIFL            v_220_4/3  v_220_5/1/2  v_220_6  IFL vss=220000
*xCTR1           v_220_4/3  v_220_5/1/2  v_220_6  v_220_7  v_220_8  v_220_9  CTR
*xPTR            v_220_7  v_220_8  v_220_9  v_132_1  v_132_2  v_132_3  PTR
*xCBR            v_132_1  v_132_2  v_132_3  v_132_4  v_132_5  v_132_6  CBR
*xVTR2           v_132_4, v_132_5, v_132_6                             VTR
*xCTR2           v_132_4  v_132_5  v_132_6  v_132_7  v_132_8  v_132_9  CTR
*xDIS            v_132_7  v_132_8  v_132_9  v_132_10 v_132_11 v_132_12 DIS
*xload           v_132_10 v_132_11 v_132_12 load rload=5500
*
xload           S12/E1/W1/BB1_a S12/E1/W1/BB1_b S12/E1/W1/BB1_c load rload=5500
"""

logger = Logging.setup_logging(logging_level=0)

measurants = {}
actuators = {}

#lnode can be attached at each level of the substation
def LNode(item, levelRef):
  if 'LNode' in item:
    for LNode in item['LNode']:
      print("    LNode:" + levelRef + " > " + LNode['@iedName'] + " class:" + LNode["@lnClass"]) 
      if LNode["@lnClass"] == "TCTR":
        #register ref for TCTR-IED
        measurants[levelRef] = LNode['@iedName']
      if LNode["@lnClass"] == "TVTR":
        #register ref for TCTR-IED
        measurants[levelRef] = LNode['@iedName']
      if LNode["@lnClass"] == "XCBR":
        #register ref for XCBR-IED
        actuators["v.x" + levelRef.lower() + "_cbr.vsig"] = LNode['@iedName']
        print("v.x" + levelRef.lower() + "_cbr.vsig")
      if LNode["@lnClass"] == "XSWI":
        #register ref for XSWI-IED
        actuators["v.x" + levelRef.lower() + "_dis.vsig"] = LNode['@iedName']
        print("v.x" + levelRef.lower() + "_dis.vsig")


def PowerTransformer(item, levelRef):
  spice_model = ""
  if 'PowerTransformer' in item:
      for Powertransformer in item['PowerTransformer']:
        print(" Powertransformer:" + levelRef + "_" + Powertransformer["@name"])
        LNode(Powertransformer, levelRef + "_" + Powertransformer["@name"])

        spice_model += "x" + levelRef + "_" + Powertransformer["@name"] + " "

        if 'TransformerWinding' in Powertransformer:
          for TransformerWinding in Powertransformer['TransformerWinding']:
            if "Terminal" in TransformerWinding:
              for Terminal in TransformerWinding["Terminal"]:
                t = Terminal["@connectivityNode"]
                spice_model += t + "_a " 
                spice_model += t + "_b " 
                spice_model += t + "_c " 

        spice_model += "PTR\n"

  return spice_model


def ConductingEquipment(item, levelRef, Voltagelevel):
  spice_model = ""
  if "ConductingEquipment" in item:
    for ConductingEquipment in item["ConductingEquipment"]:
      Cond_fullRef = levelRef + "_" + ConductingEquipment["@name"]
      type = ConductingEquipment["@type"]

      print("  ConductingEquipment:" + Cond_fullRef + " type:" + type)
      LNode(ConductingEquipment, Cond_fullRef)

      if "SubEquipment" in ConductingEquipment:
        for SubEquipment in ConductingEquipment["SubEquipment"]:
          Sub_fullRef = Cond_fullRef + "_" + SubEquipment["@name"]
          print("   SubEquipment:" + Sub_fullRef + " phase: " + SubEquipment["@phase"] )
          LNode(SubEquipment, Sub_fullRef)

      if "Terminal" in ConductingEquipment:
        for Terminal in ConductingEquipment["Terminal"]:
          print("   Terminal:" + Terminal["@connectivityNode"])

      #generate the spice model element
      spice_model += "x" + Cond_fullRef + "_" + type + " "

      if "Terminal" in ConductingEquipment:
        for Terminal in ConductingEquipment["Terminal"]:
          t = Terminal["@connectivityNode"]
          spice_model += t + "_a " 
          spice_model += t + "_b " 
          spice_model += t + "_c " 

      spice_model += type + " "
      spice_model += checkoptions(type, Voltagelevel)
      spice_model += "\n"
  return spice_model

def checkoptions(type, Voltagelevel):
  option = ""
  if type == "IFL":
    if "Voltage" in Voltagelevel:
      option = "vss=" + str(Voltagelevel["Voltage"]['$']) + Voltagelevel["Voltage"]['@multiplier']
  return option

class MyNgSpiceShared(NgSpiceShared):
    def __init__(self, ngspice_id=0, send_data=False):
        super(MyNgSpiceShared, self).__init__(ngspice_id, send_data)
        #self._logger = logger

    def get_vsrc_data(self, voltage, time, node, ngspice_id):
        #self._logger.debug('ngspice_id-{} get_vsrc_data @{} node {}'.format(ngspice_id, time, node))
        #TODO: provide voltage based on switch/cbr position
        #print(node)
        if node in actuators:
          # get position data from actuators[node], by retrieving the status over tcp(or buffered)
          if True:
            voltage[0] = 10 #circuitbreaker is closed
          else:
            voltage[0] = -10 #circuitbreaker is open
        return 0


scd_schema = xmlschema.XMLSchema("../schema/SCL.xsd")
scl = scd_schema.to_dict("../simpleIO_inputs.cid")
#pprint.pprint(scl)
#exit(0)
#generalequipment is ignored, as they are not part of the spice simulation
#function elements are ignored,as they are not part of the primary process
spice = ""


if "Substation" in scl:
  for substation in scl["Substation"]:
    sub_name = substation["@name"]
    print("--- Substation:" + sub_name + " ---")
    LNode(substation, sub_name)
    spice += PowerTransformer(substation, sub_name)

    if 'VoltageLevel' in substation:
      for Voltagelevel in substation['VoltageLevel']:
        vlvl_name = Voltagelevel["@name"]
        vlvl_fullRef = sub_name + "_" + vlvl_name
        print("Voltagelevel:" + vlvl_fullRef)

        LNode(Voltagelevel, vlvl_fullRef)
        spice += PowerTransformer(Voltagelevel, vlvl_fullRef)

        if 'Bay' in Voltagelevel:
          for Bay in Voltagelevel['Bay']:
            Bay_name = Bay["@name"]
            Bay_fullRef = vlvl_fullRef + "_" + Bay_name
            print(" Bay:" + Bay_fullRef)

            LNode(Bay, Bay_fullRef)
            spice += ConductingEquipment(Bay, Bay_fullRef, Voltagelevel)


            if 'ConnectivityNode' in Bay:
              for ConnectivityNode in Bay['ConnectivityNode']:
                print("  ConnectivityNode:" + Bay_fullRef + "_" + ConnectivityNode["@name"])
                if "ext:type" in ConnectivityNode:
                  print("xload")

circuit += spice + ".end\n"

print("--- model ---")
print(spice)
print("---")



ngspice_shared = MyNgSpiceShared(send_data=False)
ngspice_shared.load_circuit(circuit)

ngspice_shared.step(2)
arr1 = numpy.array([])
arr2 = numpy.array([])
arr3 = numpy.array([])


for _ in range(2000):
  ngspice_shared.step(10)
  analysis = ngspice_shared.plot(plot_name='tran1', simulation=None).to_analysis()
  #TODO: send values back to the merging units
  arr1 = numpy.append(arr1, float(analysis['S12/D1/Q1/L0_a'][0]))
  arr2 = numpy.append(arr2, float(analysis['S12/E1/Q1/L2_b'][0]))
  arr3 = numpy.append(arr3, float(analysis['S12/E1/W1/BB1_a'][0]))
  

exit(0)
print(ngspice_shared.plot_names)

figure = plt.figure(1, (20, 10))
axe = plt.subplot(111)
plt.title('')
plt.xlabel('Time [s]')
plt.ylabel('Voltage [V]')
plt.grid()
plt.plot(arr1)
plt.plot(arr2)
plt.plot(arr3)
plt.legend(('V1', 'V2', 'V3'), loc=(.05,.1))

plt.tight_layout()
plt.show()
