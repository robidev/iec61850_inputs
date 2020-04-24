#!/usr/bin/env python
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

from PySpice.Probe.Plot import plot
import PySpice.Logging.Logging as Logging
from PySpice.Spice.Netlist import Circuit
from PySpice.Spice.NgSpice.Shared import NgSpiceShared

logger = Logging.setup_logging(logging_level=0)

class MyNgSpiceShared(NgSpiceShared):
    def __init__(self, ngspice_id=0, send_data=False):
        super(MyNgSpiceShared, self).__init__(ngspice_id, send_data)

    def get_vsrc_data(self, voltage, time, node, ngspice_id):
        #self._logger.debug('ngspice_id-{} get_vsrc_data @{} node {}'.format(ngspice_id, time, node))
        #TODO: provide voltage based on switch/cbr position
        if node == 'v.xcircuitbreaker.vsig':
          voltage[0] = 1 #circuitbreaker is closed
        if node == 'v.xdisconnector.vsig':
          voltage[0] = -1 #disconnector is open

        return 0


circuit = """
.title substation model
* substation components
#.options interp  ; strongly reduces memory requirements
.save none       ; ensure only last step is kept each iteration
.tran 10u 3600s ; run for an hour max, with 100 samples per cycle (201u stepsize does not distort, 200 does...)
* 
.subckt source Aout Bout Cout vss=100000 freq=50
vphaseA A 0 dc 0 ac 1 sin(0 {vss} {freq} 0 0 0)
vphaseB B 0 dc 0 ac 1 sin(0 {vss} {freq} 0 0 120)
vphaseC C 0 dc 0 ac 1 sin(0 {vss} {freq} 0 0 240)
* transmission line
rline1 A Aout 0.01
rline2 B Bout 0.01
rline3 C Cout 0.01
.ends source
*
.subckt load Ain Bin Cin rload=1000
* load TODO: add capacitor and inductor, maybe nonlineair load, or inverse (constant power)
r1 Ain 0 {rload}
r2 Bin 0 {rload}
r3 Cin 0 {rload}
.ends load
*
.subckt disconnector Ain Bin Cin Aout Bout Cout
sdisA Ain Aout sig 0
sdisB Bin Bout sig 0
sdisC Cin Cout sig 0
vsig sig 0 dc 0 external
rsig sig 0 10000
.ends disconnector
*
.subckt circuitbreaker Ain Bin Cin Aout Bout Cout
scbrA Ain Aout sig 0
scbrB Bin Bout sig 0
scbrC Cin Cout sig 0
vsig sig 0 dc 0 external
rsig sig 0 10000
.ends circuitbreaker
*
.subckt ctr Ain Bin Cin Aout Bout Cout
vctrA Ain Aout dc 0
vctrB Bin Bout dc 0
vctrC Cin Cout dc 0
.ends ctr
*
.subckt transformer Ain Bin Cin Aout Bout Cout inductor1=8 inductor2=0.5 coupling=1
* transformer 4:1 turns ratio specified by 8:0.5 inductance ratio (16:1), coupling is ideal (1)
l1pri Ain i1 {inductor1}
l2pri Bin i1 {inductor1}
l3pri Cin i1 {inductor1}
l1sec Aout i2 {inductor2}
l2sec Bout i2 {inductor2}
l3sec Cout i2 {inductor2}
k1 l1pri l1sec {coupling}
k2 l2pri l2sec {coupling}
k3 l3pri l3sec {coupling}
.ends transformer
*
*
* substation description
* TODO: generate this from SCL
xsource         v_220_4  v_220_5  v_220_6  source vss=220000
xctr1           v_220_4  v_220_5  v_220_6  v_220_7  v_220_8  v_220_9  ctr
xtransformer    v_220_7  v_220_8  v_220_9  v_132_1  v_132_2  v_132_3  transformer
xcircuitbreaker v_132_1  v_132_2  v_132_3  v_132_4  v_132_5  v_132_6  circuitbreaker
*vtr2           v_132_4, v_132_5, v_132_6 
xctr2           v_132_4  v_132_5  v_132_6  v_132_7  v_132_8  v_132_9  ctr
xdisconnector   v_132_7  v_132_8  v_132_9  v_132_10 v_132_11 v_132_12 disconnector
xload           v_132_10 v_132_11 v_132_12 load rload=5500
*
.end
"""

ngspice_shared = MyNgSpiceShared(send_data=False)
ngspice_shared.load_circuit(circuit)

ngspice_shared.step(2)
arr1 = numpy.array([])
arr2 = numpy.array([])
arr3 = numpy.array([])

for _ in range(1000):
  ngspice_shared.step(20)
  analysis = ngspice_shared.plot(plot_name='tran1', simulation=None).to_analysis()
  #TODO: send values back to the merging units
  arr1 = numpy.append(arr1, float(analysis['v_220_4'][0]))
  arr2 = numpy.append(arr2, float(analysis['v_132_1'][0]))
  arr3 = numpy.append(arr3, float(analysis['v_132_10'][0]))


#exit(0)

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
