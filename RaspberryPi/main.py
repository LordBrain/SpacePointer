#!/usr/bin/python

import time
import sys
import os
import math
import subprocess
import RPi.GPIO as GPIO
import urllib

from display import writeDisplay

# load project files.
import objectList

# Clean up old pid file if its there
if os.path.isfile("pidfile"):
    os.remove("pidfile")


#start tracking on button push
def trackThing( trackID ):
  #First kill any existing tracking script
  #Start new tracking script
  subprocess.call(['python', 'track.py',str(trackID)])
  return;

# Set display
writeDisplay("Welcome","Pick a object","to track")

# Switch config
LEFT_BUTTON_PIN = 20 #Button
RIGHT_BUTTON_PIN = 21 #Button
SELECT_BUTTON_PIN = 26 #Button

GPIO.setmode(GPIO.BCM)
GPIO.setup(LEFT_BUTTON_PIN, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(RIGHT_BUTTON_PIN, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(SELECT_BUTTON_PIN, GPIO.IN,pull_up_down=GPIO.PUD_UP)

# Menu stuff
trackingSelected = 0
tracking = 0
isTracking = ""

# Start tracking the ISS on startup
trackThing(trackingSelected)

while True:
    if(GPIO.input(LEFT_BUTTON_PIN) == False):
        print("left button pushed")
        tracking -= 1
        if( tracking < 0 ):
            tracking = len(objectList.spaceObjects) - 1
	print("Menu: " + objectList.spaceObjects[tracking])
	if( tracking == trackingSelected ):
		isTracking = "Tracking"
	else:
		isTracking = ""
	writeDisplay(objectList.spaceObjects[tracking],isTracking)
        time.sleep( .5 )
    elif(GPIO.input(SELECT_BUTTON_PIN) == False):
        print("select button pushed")
        trackingSelected = tracking
        trackThing(trackingSelected)
	writeDisplay(objectList.spaceObjects[tracking],"","Tracking")
        time.sleep(.5)
    elif(GPIO.input(RIGHT_BUTTON_PIN) == False):
        print("right button pushed")
        tracking += 1
        if( len(objectList.spaceObjects) - 1 < tracking ):
            tracking = 0
	print("Menu: " + objectList.spaceObjects[tracking])
        if( tracking == trackingSelected ):
                isTracking = "Tracking"
        else:
                isTracking = ""
	writeDisplay(objectList.spaceObjects[tracking],"",isTracking)
        time.sleep(.5)
        #
