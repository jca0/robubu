#include <WiFi.h>
#include "config.h"
#include "servo.h"
#include "audio.h"
#include "web.h"

bool manualMode = false;

static int servoAngle = 0;
static int servoDir = 1;
static unsigned long lastServoMove = 0;
static const int SERVO_STEP_MS = 30;

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
  delay(1000);

  initWiFi();
  initServo();
  initAudio();
  initServer();
  Serial.println("Ready");
}

void loop() {
  server.handleClient();
  handleAudioLoop();

  if (manualMode) return;

  if (!isAudioPlaying()) {
    playRandomClip();
  }

  unsigned long now = millis();
  if (now - lastServoMove >= SERVO_STEP_MS) {
    lastServoMove = now;
    servoAngle += servoDir;
    if (servoAngle >= 180) {
      servoAngle = 180;
      servoDir = -1;
    } else if (servoAngle <= 0) {
      servoAngle = 0;
      servoDir = 1;
    }
    setServoAngle(servoAngle);
  }
}
