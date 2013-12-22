#!/usr/bin/python

#USAGE: do sudo python Controller.py NAME_OF_FIREBASE
#Just use the name of your firebase without "https://" nor the  "firebaseio.com"
 
from firebase import firebase
import serial, time, sys
from subprocess import *



#find out which /dev/ttyACM it is, and return the first result
serial_port=check_output("find /dev/ -name ttyACM* | head -n 1", shell=True)
#ser = serial.Serial('/dev/ttyACM', 9600, timeout = 0.1)
ser = serial.Serial( serial_port.rstrip(), 9600, timeout = 0.1)

def send( theinput ):
  ser.write( theinput )
  time.sleep(0.01)
  

def send_and_receive( theinput ):
  ser.write( theinput )
  while True:
    try:
      time.sleep(0.01)
      state = ser.readline()
      print state
      break
    except:
      pass
  time.sleep(0.01)

  
firebase = firebase.FirebaseApplication("https://testbed-firebase.firebaseio.com", None)

while True:
  try:
    result = firebase.get('/robochef/temp', None)
    if result['meaasure']=="forward":
     send_and_receive('1')
    elif result['direction']=="backward":
      send('2')
    elif result['direction']=="left":
      send('3')
    elif result['direction']=="right":
      send('4')
    elif result['direction']=="stop":
      send('5')
    time.sleep(0.1)
  except:
    time.sleep(.1)
    pass

