#!/usr/bin/python
import serial, time
from time import strftime
import sys
import subprocess


ser = serial.Serial(sys.argv[1], 9600, timeout = 0.1) #takes in the serial port as a command line argument
#ser = serial.Serial('/dev/tty.usbmodem1411', 9600, timeout = 0.1)



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

while True:
  tempC = send_and_receive('1')
  f = open("temp.txt", "a")
  f.write( str(tempC) + strftime(" %s %Y-%m-%d %H:%M:%S") + "\n" )
  f.close()
  subprocess.call( 'say the food is currently ' + tempC + 'degrees', shell=True)
  time.sleep(2)

