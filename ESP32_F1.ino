#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi parameters
const char *ssid = "ESP32_AP_F1"; // SSID for the access point
const char *password = "your_PASSWORD"; // Password for the access point

// IP and port of ESP32_F2
const char* hostIP_F2 = "192.168.4.3";
const int hostPort_F2 = 4211;

// IP and port of ESP32_M
const char* hostIP_M = "192.168.4.2";
const int hostPort_M = 4212;

WiFiUDP Udp;
unsigned int localUdpPort = 4210; // Local UDP port
char incomingPacket[255]; // Receive buffer
const char* replymessage = "ESP32_F1 got the message, thank you!";

void setup() {
    Serial.begin(115200);
    Serial.println("Setting up AP_F1...");

    // Configuring the access point
    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("Failed to configure AP_F1");
        return;
    }

    // Starting the access point
    if (!WiFi.softAP(ssid, password)) {
        Serial.println("Failed to start AP_F1");
        return;
    }

    // Obtaining the access point's IP address
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP_F1 IP address: ");
    Serial.println(IP);

    // Starting UDP listener
    if (Udp.begin(localUdpPort)) {
        Serial.printf("Now listening at IP %s, UDP port %d\n", IP.toString().c_str(), localUdpPort);
    } else {
        Serial.println("Failed to start UDP listener");
    }
}

void loop() {
    // Checking for incoming UDP packets
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        int len = Udp.read(incomingPacket, 255);
        if (len > 0) {
            incomingPacket[len] = 0;
        }
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        // Checking if the packet is from ESP32_M or ESP32_F2
        if (Udp.remoteIP().toString() == hostIP_M || Udp.remoteIP().toString() == hostIP_F2) {
            Serial.println("Forwarding message to Xbee...");
            // TODO: Add code to send to Xbee

            // Sending reply back to the sender
            if (Udp.beginPacket(Udp.remoteIP(), Udp.remotePort())) {
              Udp.write((const uint8_t*)replymessage, strlen(replymessage));
              if (Udp.endPacket()) {
                Serial.println("Reply message sent back");
              }
            }
        } else {
            // Forwarding the message to ESP32_M and ESP32_F2
            if (Udp.beginPacket(hostIP_M, hostPort_M)) {
                Serial.println("Forwarding message to ESP32_M...");
                Udp.write((const uint8_t*)incomingPacket, strlen(incomingPacket));
                if (Udp.endPacket()) {
                    Serial.println("Message forwarded to ESP32_M.");
                } else {
                    Serial.println("Failed to forward message to ESP32_M");
                }
            } 

            if (Udp.beginPacket(hostIP_F2, hostPort_F2)) {
                Serial.println("Forwarding message to ESP32_F2...");
                Udp.write((const uint8_t*)incomingPacket, strlen(incomingPacket));
                if (Udp.endPacket()) {
                    Serial.println("Message forwarded to ESP32_F2.");
                } else {
                    Serial.println("Failed to forward message to ESP32_F2");
                }
            }
        }
    } 
}
