from machine import Pin, I2C
import ssd1306
import time

# I2C connection
i2c = I2C(0, scl=Pin(1), sda=Pin(0), freq=400000)

# OLED display width and height
width = 128
height = 64

# Initialize OLED display
oled = ssd1306.SSD1306_I2C(width, height, i2c)

# Clear the display
oled.fill(0)

# Display some text
oled.text("Hello, World!", 0, 0)
oled.text("Raspberry Pi Pico W", 0, 10)
oled.text("OLED Display", 0, 20)

# Update the display
oled.show()

# Optional: Create a simple animation or message loop
while True:
    oled.fill(0)
    oled.text("Counting:", 0, 0)
    for i in range(10):
        oled.text(str(i), 70, 10)
        oled.show()
        time.sleep(1)
        oled.fill_rect(70, 10, 20, 10, 0)  # Clear the previous number
