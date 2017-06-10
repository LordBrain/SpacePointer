from skyfield.api import Topos, load
import serial
import os
import time


# import config file
import sys
sys.path.append('../')
import config

# Write pid to file
pid = str(os.getpid())
file("pidfile", 'w').write(pid)

lon = config.siteLon
lat = config.siteLat

stations_url = "http://www.celestrak.com/NORAD/elements/science.txt"
satellites = load.tle(stations_url)
satellite = satellites['HST']

planets = load('de421.bsp')

earth = planets['Earth']

myLocation = Topos(lat, lon)

while True:
  ts = load.timescale()
  t = ts.now()

  difference = satellite - myLocation
  topocentric = difference.at(t)

  alt, az, distance = topocentric.altaz()
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
