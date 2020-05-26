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

import libiec61850client

def readvaluecallback(key,data):
  print("cb: %s - %s" % (key,data))
  socketio.emit("svg_value_update_event",{ 'element' : key, 'value' : data['value'] })

client = libiec61850client.iec61850client(readvaluecallback)

thread = None
tick = 0.001
focus = ''
hosts_info = {}
reset_log = False
async_mode = None

hosts_info['localhost2'] = {}
hosts_info['host3'] = {}

#webserver
app = Flask(__name__, template_folder='templates', static_folder='static')
socketio = SocketIO(app, async_mode=async_mode)
#logging
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)


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

@socketio.on('start_simulation', namespace='')
def start_level(data):
  print("starting level")


@socketio.on('stop_simulation', namespace='')
def stop_level(data):
  print("stopping level")


@socketio.on('register_datapoint', namespace='')
def register_datapoint(data):
  global client
  print("register datapoint:" + str(data) )
  client.registerReadValue(str(data['id']))

@socketio.on('register_datapoint_finished', namespace='')
def register_datapoint_finished(data):
  print("register datapoint finished" )


@socketio.on('write_value', namespace='')
def write_value(data):
  global client
  print("write value:" + str(data['value']) + ", element:" + str(data['id']) )
  client.registerWriteValue(str(data['id']),str(data['value']))

@socketio.on('write_position', namespace='')
def write_position(data):
  print("write position:" + str(data['id'])  )
  client.registerWriteValue(str(data['id']),data['value'])

  
@socketio.on('set_focus', namespace='')
def set_focus(data):
  global focus
  global hosts_info
  focus = data
  #print("focus:" + str(focus))
  if focus in hosts_info:
    if '0' in hosts_info[focus]:
      socketio.emit('info_event', {'type': '0', 'data': hosts_info[focus]['0']})
  emit('select_tab_event', {'host_name': focus})

@socketio.on('connect', namespace='')
def test_connect():
  global thread
  if thread is None:
    thread = socketio.start_background_task(target=worker)

#worker subroutines
def process_info_event(loaded_json): #add info to the ied datamodel tab
  global focus
  global hosts_info
  ihost = loaded_json
  itype = 0#loaded_json['data']['type']
  idata = "models:"+ loaded_json #loaded_json['data']['data']

  hosts_info[ihost]['last'] = time.time()
  hosts_info[ihost][itype] = idata
  if ihost==focus:
    socketio.emit('info_event', {'type': itype, 'data': idata})


#background thread
def worker():
  global focus
  global hosts_info
  global reset_log
  global client
  socketio.sleep(tick)

  last_time = time.time()

  logline = "logline data"
  if logline != "":
    logline_utf = logline #.decode('utf-8')
    socketio.emit('log_event', {'host':'localhost','data':logline_utf,'clear':1})
    socketio.emit('log_event', {'host':'localhost2','data':logline_utf,'clear':1})
    socketio.emit('log_event', {'host':'host3','data':logline_utf,'clear':1})

  i = 0
  toggle = False
  print("treat started")
  while True:
    socketio.sleep(tick)
    #reset the client
    if reset_log == True:
      socketio.sleep(0.5)
      focus = ''
      hosts_info = {}
      hosts_info['localhost2'] = {}
      hosts_info['host3'] = {}
      reset_log = False
      socketio.sleep(0.5)

    socketio.sleep(1.5)

    client.poll()

    socketio.emit('log_event', {'host':'localhost','data':str(i),'clear':0})
    #socketio.emit('log_event', {'host':'localhost2','data':"two"+str(i),'clear':0})
    #socketio.emit('log_event', {'host':'host3','data':"three"+str(i),'clear':0})

    if toggle == True:
      #socketio.emit("svg_value_update_event",{ 'element' : 'ied://10.0.0.2:102/IED1_XCBRGenericIO/XCBR1.Pos.stVal', 'value' : 'open', 'type' : 'switch' })
      toggle = False
    else:
      #socketio.emit("svg_value_update_event",{ 'element' : 'ied://10.0.0.2:102/IED1_XCBRGenericIO/XCBR1.Pos.stVal', 'value' : 'close', 'type' : 'switch' })
      toggle = True

    #socketio.emit("svg_value_update_event",{ 'element' : 'ied://10.0.0.2:102/IED1_XCBRGenericIO/LOAD.Pos.stVal', 'value' : 'test2', 'type' : 'text' })
    #i += 1
    #parse info events    
    #process_info_event("localhost2")
    #process_info_event("host3")



if __name__ == '__main__':
  socketio.run(app)

"""

simulation... not sure.. special handling?

"""
