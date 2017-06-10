import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

import Image
import ImageDraw
import ImageFont

# Raspberry Pi pin configuration:
RST = 24
# Note the following are only used with SPI:
DC = 23
SPI_PORT = 0
SPI_DEVICE = 0

disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST, dc=DC, spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE, max_speed_hz=8000000))

# Initialize library.
disp.begin()

# Clear display.
#disp.clear()
#disp.display()

#image = Image.open('iss.jpg').convert('1')

#disp.image(image)
#disp.display()

def clearDisplay():
  disp.clear()
  disp.display()

def writeDisplay(line1,line2,line3):
  disp.clear()
  disp.display()

  width = disp.width
  height = disp.height
  image = Image.new('1', (width, height))

  draw = ImageDraw.Draw(image)

  # Draw a black filled box to clear the image.
  draw.rectangle((0,0,width,height), outline=0, fill=0)

  # Draw some shapes.
  # First define some constants to allow easy resizing of shapes.
  padding = -2
  top = padding
  bottom = height-padding
  # Move left to right keeping track of the current x position for drawing shapes.
  x = 0

  # Load default font.
  #font = ImageFont.load_default()
  fontLine1 = ImageFont.truetype('/usr/share/fonts/truetype/roboto/Roboto-Thin.ttf', 24)
  font = ImageFont.truetype('/usr/share/fonts/truetype/roboto/Roboto-Thin.ttf', 16)


  draw.text((x, top),       line1,  font=fontLine1, fill=255)
  draw.text((x, top+24),     line2, font=font, fill=255)
  draw.text((x, top+40),    line3,  font=font, fill=255)


  # Display image.
  disp.image(image)
  disp.display()
  time.sleep(.1)
