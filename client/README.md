load svg, with single-line diagram in webclient from file

interactive id-elements should be related to IED/LN.Do.Da, if webclient should read/write value from server by socketio
desc/title can be used to register+display type/instance/ref

javascript to generate list of values that can be written/read, by type(switch/measure / setting/button)
writable= dialog box via javascript
readable= register for polling/reporting by python server backend

update read values by socketio event from server
update writes by socketio to server

options: 
         switch  - read: position, write: open/close, error:true/false
         measure - read: value, error:true/false

         setting - read: value, write:value, error:true/false
         button  - write: clickevent, error:true/false

- python server backend(flask) and pylib61850

server should list all ied's+ip's from SCL
when first request comes in, IEC61850 client connection is established, fail: error, succes: store, forward state to webclient
report/poll is determined by presence of dataset/rcb
any received data(poll or callback) is forwarded to webclient(s) by socketio

flask REST:
 - registerElement(ref, type) : wc -> s
 - unregisterElement(ref) : wc -> s
 - writeData(ref, value) : wc -> s
 - readData(ref) : wc -> s
 - updateValue() : s -> wc

process-simulation/fault-simulation
 - source/load values
 - fault location,type