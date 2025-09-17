# OTA Update with Local Flask Server for ESP32

This project demonstrates how to perform **Over-The-Air (OTA) updates** on an ESP32 using a **local HTTP Flask server**.  
The setup allows you to update ESP32 firmware by simply hosting a `.bin` file on your PC and letting the ESP32 download and flash it over Wi-Fi.

---

## 🚀 How It Works
1. A **Flask HTTP server** is run on your PC.  
   - It serves the firmware file (`firmware.bin`) from the server folder.  
2. The ESP32 and PC are connected to the **same 2.4 GHz Wi-Fi network**.  
3. The ESP32 firmware is flashed once with OTA-capable code (included in this repo).  
4. On boot:
   - The ESP32 connects to Wi-Fi.  
   - It requests the **firmware binary** from the Flask server via HTTP.  
   - The new firmware is written into the **OTA partition**.  
   - ESP32 automatically reboots and runs the new firmware.  
5. Example firmware (`firmware.bin`) in this repo simply **blinks an LED** connected to **GPIO 22**.

---

## 📂 Repository Structure
OTA_UPDATE-LOCAL-SERVER/
│
├── server/ # Flask HTTP server
│ ├── firmware.bin # Example firmware (LED blink on GPIO 22)
│ └── server.py # Flask server code
│
├── esp32_code/ # ESP-IDF OTA update code
│
└── README.md # Project documentation

The server will start on:

http://<your-pc-ip>:5000/firmware




