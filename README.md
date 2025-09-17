# OTA Update - Local Server (ESP32 + Flask)

This project demonstrates how to perform **Over-The-Air (OTA) firmware updates** on an ESP32 using a **local HTTP server** built with Flask.  
It allows you to serve the firmware (`.bin`) file from your PC over Wi-Fi, and the ESP32 can fetch and flash it automatically.

---

## 🚀 Features
- Local **Flask HTTP server** to host the firmware file.  
- ESP32 OTA update using **ESP-IDF OTA APIs**.  
- Secure Wi-Fi connectivity (ensure ESP32 and server are on the same **2.4 GHz Wi-Fi**).  
- Simple REST API endpoint for firmware delivery.  
- Optional HTTPS support for secure transfer.  
- Example **OTA client code** included.

---
# ⚙️ Requirements

### On PC (Server)
- Python 3.x  
- Flask (`pip install flask`)  
- OpenSSL (optional, for HTTPS certificate generation)

### On ESP32
- ESP-IDF installed & configured  
- OTA client code (provided below)  
- ESP32 connected to the same Wi-Fi as the PC server  

---

## 🖥️ Setting Up the Local Server

1. Install Flask:
   ```bash
   pip install flask
Place your compiled ESP32 firmware in the project folder and rename it as:

python
Copy code
firmware.bin
Run the Flask server:

bash
Copy code
python server.py
The server will start on:
http://your-pc-ip:5000/firmware
Example:
http://192.168.1.100:5000/firmware
⚠️ Make sure your PC and ESP32 are connected to the same 2.4 GHz Wi-Fi network.

🌐 API Endpoints
GET /firmware
Fetches the latest firmware binary.

