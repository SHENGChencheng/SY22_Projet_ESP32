# PROJECT SY22 - ESP32

## Introduction:
In this project, we utilize ESP32 microcontrollers to establish a network of interconnected devices, facilitating communication via various protocols such as WiFi, XBee, etc.

### WiFi Integration:
This segment focuses on WiFi connectivity, involving three key ESP32 devices:

- **ESP32_M**: This ESP32 unit is mobile, actively seeking out WiFi signals with the strongest intensity.
- **ESP32_F1**: Functioning as a server, this ESP32 is linked to an XBee module for communication.
- **ESP32_F2**: Serving as an access point, this ESP32 connects to ESP32_F1 and manages network access.

Through this setup, we aim to establish a robust network environment capable of efficient communication between devices.
