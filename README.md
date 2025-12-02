1) D1-Mini-esp8266-clock
Esp8266 accurate digital clock using ds3231sn, a 0.96 inch Oled display,with wifi NTP time setting,ds18b20 temperature sensor.
 ESP8266 Digital Clock
# ESP8266 Smart Clock

A clean and accurate smart clock built using an ESP8266 (Wemos D1 Mini), DS3231 RTC, DS18B20 temperature sensor, and a 128×64 OLED display. It keeps time reliably using:

* DS3231 hardware RTC
* On-demand NTP sync
* Software-based second ticking
* A smooth boot animation
* Compact and optimized OLED rendering

This project is the result of multiple iterations focused on timing accuracy, display stability, and clean code structure.

## Features

* Accurate timekeeping with DS3231 SN version
* WiFi to NTP time sync on demand
* Smooth software second ticking
* Two display modes: Time and Temperature
* DS18B20 high precision temperature reading
* Four-dot smooth boot animation
* Optimized redraws to avoid flicker
* All strings moved to PROGMEM
* WiFi disabled most of the time for low power

# Parts Used

* Wemos D1 Mini (ESP8266)
* DS3231 RTC module (SN version recommended)
* DS18B20 temperature sensor
* 128×64 OLED Display (I2C, SSD1306)
* Two momentary push buttons
* 4.7kΩ resistor (DS18B20 pull-up)
* Jumper wires

# Wiring Overview

## I2C Devices (OLED and RTC)

Both share the same bus.

* SCL → D1
* SDA → D2
* VCC → 3.3V
* GND → GND

# Full Connection Table

## OLED (SSD1306 I2C)

* VCC → 3.3V
* GND → GND
* SCL → D1
* SDA → D2

## DS3231 RTC

* VCC → 3.3V
* GND → GND
* SCL → D1
* SDA → D2

## DS18B20 Temperature Sensor

* VDD → 3.3V
* GND → GND
* DQ (Data) → D5
* 4.7kΩ resistor between DQ → 3.3V

## Buttons

### Mode Switch

* One side → D3
* Other side → GND

### WiFi Sync Switch

* One side → D6
* Other side → GND

# How It Works

* On boot, a four-dot animation plays while the DS3231 is read.
* The ESP8266 starts counting from the exact second the RTC reports.
* Every 30 seconds the RTC is read again to correct small drift.
* Pressing the WiFi button enables WiFi, fetches NTP time, updates the RTC, and disables WiFi, this is useful to set time to a new
* RTC and then in future correct RTC drifts using a single button press.
* The OLED only updates when necessary using a dirty flag system.


# License

This project is licensed under the MIT License.
