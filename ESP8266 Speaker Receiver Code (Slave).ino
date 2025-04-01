#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <I2S.h>

#define WIFI_SSID "YourWiFi"
#define WIFI_PASS "YourPassword"
#define UDP_PORT 12345

WiFiUDP udp;
byte audioData;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi. IP: " + WiFi.localIP().toString());

    udp.begin(UDP_PORT);

    // Initialize I2S
    I2S.setOutputMode(OUTPUT_I2S);
    I2S.begin();
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        udp.read(&audioData, 1);  // Read the 8-bit audio data
        
        int16_t audioOutput = map(audioData, 0, 255, -32768, 32767); // Convert to 16-bit audio

        I2S.write((uint16_t)audioOutput);  // Play the audio through I2S DAC
    }
}
