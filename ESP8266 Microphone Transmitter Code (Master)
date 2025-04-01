#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define MIC_PIN A0            // Microphone AGC input
#define SAMPLE_RATE 16000     // Sampling rate (Hz)
#define WIFI_SSID "YourWiFi"  // WiFi Network Name
#define WIFI_PASS "YourPassword" // WiFi Password
#define UDP_PORT 12345        // Port for streaming audio

WiFiUDP udp;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());

    udp.begin(UDP_PORT);
}

void loop() {
    int micInput = analogRead(MIC_PIN);  // Read Mic input
    byte audioData = map(micInput, 0, 1023, 0, 255); // Convert to 8-bit audio

    udp.beginPacket("255.255.255.255", UDP_PORT); // Broadcast to all receivers
    udp.write(audioData);
    udp.endPacket();

    delayMicroseconds(1000000 / SAMPLE_RATE);  // Control sample rate
}
