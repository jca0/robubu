#include <WiFi.h>
#include "config.h"
#include "camera.h"
#include "servo.h"
#include "cam_stream.h"
#include "web.h"

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  delay(1000);

  initCamera();
  initWiFi();
  initServo();
  initStream();
  initServer();
}

void loop() {
  server.handleClient();
}
