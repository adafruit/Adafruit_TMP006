#!/usr/bin/python
from firebase import firebase
import serial, time
ser = serial.Serial('/dev/tty.usbmodem1411', 9600, timeout = 0.1)



def send( theinput ):
  ser.write( theinput )
  while True:
    try:
      time.sleep(0.01)
      #state = ser.readline()
      #print state
      break
    except:
      pass
  time.sleep(0.1)

def send_and_receive( theinput ):
  ser.write( theinput )
  while True:
    try:
      time.sleep(0.01)
      state = ser.readline()
      print state
      return state
    except:
      pass
  time.sleep(0.1)
firebase = firebase.FirebaseApplication('https://testbed-firebase.firebaseio.com', None)

while True:
  result = firebase.get('/RoboChef/Cooking', None)
  if result['watching']=="temperature":
    tempC = send_and_receive('1')
    firebase.put('/','RoboChef/Food',{'CurrentTemp': str( tempC )})
  else:
    time.sleep(2)

