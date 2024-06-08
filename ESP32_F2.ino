#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32_AP_F1"; // WiFi SSID for ESP32_F1
const char* password = "your_PASSWORD"; // WiFi password for ESP32_F1

const char* soft_ap_ssid = "ESP32_AP_F2"; // SoftAP SSID for ESP32_F2
const char* soft_ap_password = "your_PASSWORD"; // SoftAP password for ESP32_F2

const char* hostIP_F1 = "192.168.4.1"; // ESP32_F1's IP address
const int hostPort_F1 = 4210; // UDP port ESP32_F1 is listening on
const char* hostIP_M = "192.168.5.2"; // ESP32_M's IP address
const int hostPort_M = 4212; // UDP port ESP32_M is listening on

WiFiUDP Udp;
unsigned int localUdpPort = 4211; // Local UDP port
char incomingPacket[255]; // Receive buffer

// Declare these globally so they can be accessed in both setup() and loop()
IPAddress ap_local_IP(192, 168, 5, 1); // AP mode IP address
IPAddress ap_gateway(192, 168, 5, 1);  // AP mode gateway
IPAddress ap_subnet(255, 255, 255, 0); // AP mode subnet mask

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up AP_F2...");

  WiFi.mode(WIFI_AP_STA); // Set to Station-AP mode

  // Configure AP mode static IP address
  if (!WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet)) {
    Serial.println("Failed to configure AP_F2");
    return;
  }

  // Start AP
  if (!WiFi.softAP(soft_ap_ssid, soft_ap_password)) {
    Serial.println("Failed to start AP_F2");
    return;
  }

  IPAddress AP_IP = WiFi.softAPIP();
  Serial.print("AP_F2 IP address: ");
  Serial.println(AP_IP);

  // Configure Station mode static IP address
  IPAddress sta_local_IP(192, 168, 4, 3); // Station mode IP address
  IPAddress sta_gateway(192, 168, 4, 1);  // Station mode gateway
  IPAddress sta_subnet(255, 255, 255, 0); // Station mode subnet mask

  if (!WiFi.config(sta_local_IP, sta_gateway, sta_subnet)) {
    Serial.println("Failed to configure Station IP");
    return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());

  if (Udp.begin(localUdpPort)) {
    Serial.printf("Listening on UDP port %d\n", localUdpPort);
  } else {
    Serial.println("Failed to start UDP listener");
  }
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        // Receive UDP packet
        int len = Udp.read(incomingPacket, 255);
        if (len > 0) {
            incomingPacket[len] = 0;
        }
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        // Forward message based on its source
        if (Udp.remoteIP().toString() == hostIP_M) {
            // Message from ESP32_M, forward to ESP32_F1
            Serial.println("Message from ESP32_M, forwarding to ESP32_F1...");
            if (Udp.beginPacket(hostIP_F1, hostPort_F1)) {
                Udp.write((const uint8_t*)incomingPacket, strlen(incomingPacket));
                if (Udp.endPacket()) {
                    Serial.println("Message forwarded to ESP32_F1.");
                } else {
                    Serial.println("Failed to forward message to ESP32_F1");
                }
            } else {
                Serial.println("Failed to begin packet for ESP32_F1");
            }
        } else {
            // Message from ESP32_F1, forward to ESP32_M
            Serial.println("Message from ESP32_F1, forwarding to ESP32_M...");
            if (Udp.beginPacket(hostIP_M, hostPort_M)) {
                Udp.write((const uint8_t*)incomingPacket, strlen(incomingPacket));
                if (Udp.endPacket()) {
                    Serial.println("Message forwarded to ESP32_M.");
                } else {
                    Serial.println("Failed to forward message to ESP32_M");
                }
            } else {
                Serial.println("Failed to begin packet for ESP32_M");
            }
        }
    } 
}
