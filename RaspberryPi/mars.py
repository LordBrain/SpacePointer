from skyfield.api import Topos, load
import serial
import time
import os

# import config file
import sys
import config

# Write pid to file
pid = str(os.getpid())
file("pidfile", 'w').write(pid)


lon = config.siteLon
lat = config.siteLat

#load = Loader('~/skyfield-data',expire=false)
planets = load('de421.bsp')

body = planets['Mars']
earth = planets['Earth']

while True:
	ts = load.timescale()
	t = ts.now()

	myLocation = earth + Topos(lat, lon)
	apparent = myLocation.at(t).observe(body).apparent()

	alt, az, distance = apparent.altaz()
	print(alt.degrees)
	print(az.degrees)
	print(distance)
	smallAlt = "{0:.2f}".format(alt.degrees)
	smallAz = "{0:.2f}".format(az.degrees)
	# Send the position to the arduino.
	position = "<%s,%s>" % (smallAz, smallAlt)
	ser = serial.Serial(config.serialPort, config.serialSpeed)
	ser.write(position)
	
	time.sleep(10)
