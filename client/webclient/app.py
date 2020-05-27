#!/usr/bin/env python3
from flask import Flask, render_template, session, request, redirect, url_for, jsonify
from flask_socketio import SocketIO, emit
from werkzeug.utils import secure_filename

import socket
import json
import subprocess
import time
import logging
import yaml
import os
import logging

import libiec61850client


thread = None
tick = 0.001
focus = ''
hosts_info = {}
reset_log = False
async_mode = None

#webserver
app = Flask(__name__, template_folder='templates', static_folder='static')
socketio = SocketIO(app, async_mode=async_mode)

#logging handler, for sending logs to the client
class socketHandler(logging.StreamHandler):
  def __init__(self, socket):
    logging.StreamHandler.__init__(self)
    self.socket = socket

  def emit(self, record):
    msg = self.format(record)
    self.socket.emit('log_event', {'host':'localhost','data':msg,'clear':0})


#http calls
@app.route('/', methods = ['GET'])
def index():
  global reset_log
  reset_log = True
  return render_template('index.html', async_mode=socketio.async_mode)


# Socket events
@socketio.on('get_page_data', namespace='')
def get_page_data(data):
  emit('page_reload', {'data': ""})


@socketio.on('register_datapoint', namespace='')
def register_datapoint(data):
  global client
  logger.debug("register datapoint:" + str(data) )
  client.registerReadValue(str(data['id']))


@socketio.on('write_value', namespace='')
def write_value(data):
  global client
  logger.debug("write value:" + str(data['value']) + ", element:" + str(data['id']) )
  client.registerWriteValue(str(data['id']),str(data['value']))

@socketio.on('write_position', namespace='')
def write_position(data):
  logger.debug("write position:" + str(data['id'])  )
  client.registerWriteValue(str(data['id']),data['value'])

  
@socketio.on('set_focus', namespace='')
def set_focus(data):
  global focus
  global hosts_info
  focus = data
  #print("focus:" + str(focus))
  if focus in hosts_info:
    socketio.emit('info_event', hosts_info[focus]['data'] )
  emit('select_tab_event', {'host_name': focus})

@socketio.on('connect', namespace='')
def test_connect():
  global thread
  if thread is None:
    thread = socketio.start_background_task(target=worker)

@socketio.on('register_datapoint_finished', namespace='')
def register_datapoint_finished(data):
  return #there is a bug here, so disable for now
  global client
  ieds = client.getRegisteredIEDs()
  for key in ieds:
    tupl = key.split(':')
    hostname = tupl[0]

    emit('log_event', {'host':hostname,'data':'adding IED info','clear':1})

    port = None
    if len(tupl) > 1 and tupl[1] != "":
      port = int(tupl[1])
    model = client.getDatamodel(hostname=hostname, port=port)

    loaded_json = {}
    loaded_json['host'] = hostname
    loaded_json['data'] = str(model)
    process_info_event(loaded_json)



#worker subroutines
def process_info_event(loaded_json): #add info to the ied datamodel tab
  global focus
  global hosts_info
  ihost = loaded_json['host']
  idata = loaded_json['data']
  # store data
  if not ihost in hosts_info:
    hosts_info[ihost] = {}

  hosts_info[ihost]['last'] = time.time()
  hosts_info[ihost]['data'] = idata
  # send data also to webclient
  if ihost==focus:
    socketio.emit('info_event', idata)


#background thread
def worker():
  global focus
  global hosts_info
  global reset_log
  global client
  socketio.sleep(tick)

  sh = socketHandler(socketio)
  sh.setLevel(logging.DEBUG)
  fm = logging.Formatter('%(asctime)s %(name)-12s %(levelname)-8s %(message)s')
  sh.setFormatter(fm)
  logger.addHandler(sh)

  logger.info("worker treat started")

  while True:
    socketio.sleep(tick)
    #reset the client
    if reset_log == True:
      socketio.sleep(0.5)
      focus = ''
      reset_log = False
      socketio.sleep(0.5)

    socketio.sleep(1)
    client.poll()
    logger.info("values polled")



# callback from libiec61850client
# called by client.poll
def readvaluecallback(key,data):
  logger.debug("cb: %s - %s" % (key,data))
  socketio.emit("svg_value_update_event",{ 'element' : key, 'data' : data })


if __name__ == '__main__':
  logger = logging.getLogger('webserver')
  logging.basicConfig(format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
    level=logging.INFO)
	# note the `logger` from above is now properly configured
  logger.debug("started")
  client = libiec61850client.iec61850client(readvaluecallback, logger)
  socketio.run(app)

"""

simulation... not sure.. special handling?

"""
