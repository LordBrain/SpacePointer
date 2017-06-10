#!/bin/bash
# LiPoPi
# Shut down the Pi if the GPIO goes high indicating low battery

# GPIO Port
gpio_port="15"

# Enable GPIO
if [ ! -d "/sys/class/gpio/gpio$gpio_port" ]; then
  echo 15 > /sys/class/gpio/export || { echo -e "Can't access GPIO $gpio_port" 1>&2; exit 1; }
fi

# Set it to input
echo "in" > /sys/class/gpio/gpio$gpio_port/direction || { echo -e "Can't set GPIO $gpio_port to an input" 1>&2; exit 1; }

# Set it as active high
echo 0 > /sys/class/gpio/gpio$gpio_port/active_low || { echo -e "Can't set GPIO $gpio_port to active high" 1>&2; exit 1; }

# If its low (low battery light is on), shutdown
if [ "`cat /sys/class/gpio/gpio$gpio_port/value`" != 1 ]; then
  echo "Shutting down due to low power `date`"
  /sbin/shutdown -h now || { echo -e "Can't halt the system" 1>&2; exit 1; }
fi

exit 0
