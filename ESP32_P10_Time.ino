#include <WiFi.h>
#include "time.h"
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_black_16.h"

// Wi-Fi and NTP setup
const char* ssid = "Sujol";
const char* password = "19721979sujith";
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

// DMD32 setup
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Timer setup
hw_timer_t * timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Sync time using NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // Disconnect Wi-Fi after time sync
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Timer setup for refreshing the DMD display
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);

  // Initialize DMD display
  dmd.clearScreen(true);
}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Format time string to display on DMD
  char timeStr[10];
  int hour = timeinfo.tm_hour;
  int minute = timeinfo.tm_min;
  char ampm[] = "AM";
  if (hour >= 12) {
    ampm[0] = 'P';
    if (hour > 12) hour -= 12;
  }
  if (hour == 0) hour = 12;
  sprintf(timeStr, "%2d:%02d %s", hour, minute, ampm);

  // Display the formatted time on the DMD screen
  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(1, 1, timeStr, strlen(timeStr), GRAPHICS_NORMAL);

  delay(1000);  // Delay for 1 second to update the time
  readSerialCommand();
}

void readSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("set")) {
      Serial.println("Enter Date and Time in DD MM YYYY HH MM AM/PM format:");
      while (Serial.available() == 0);
      String input = Serial.readStringUntil('\n');
      int d, m, y, h, min;
      char ampm[3];
      sscanf(input.c_str(), "%d %d %d %d %d %s", &d, &m, &y, &h, &min, ampm);
      if (strcmp(ampm, "PM") == 0 && h != 12) h += 12;
      if (strcmp(ampm, "AM") == 0 && h == 12) h = 0;
      struct tm t = {0};
      t.tm_year = y - 1900;
      t.tm_mon = m - 1;
      t.tm_mday = d;
      t.tm_hour = h;
      t.tm_min = min;
      t.tm_sec = 0;
      time_t newTime = mktime(&t);
      struct timeval now = { .tv_sec = newTime, .tv_usec = 0 };
      settimeofday(&now, NULL);
      Serial.println("Time updated successfully!");
    }
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
