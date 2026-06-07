#pragma once

// WiFi
const char* WIFI_SSID = "Frontier8032";
const char* WIFI_PASS = "4799518885";

// Servo
static const int SERVO_PIN = 2;

// I2S Audio (MAX98357A)
#define I2S_DOUT 4   // D3 - DIN
#define I2S_BCLK 5   // D4 - BCLK
#define I2S_LRC  6   // D5 - LRC

// XIAO ESP32S3 Sense camera pins
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  10
#define SIOD_GPIO_NUM  40
#define SIOC_GPIO_NUM  39
#define Y9_GPIO_NUM    48
#define Y8_GPIO_NUM    11
#define Y7_GPIO_NUM    12
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    16
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    17
#define Y2_GPIO_NUM    15
#define VSYNC_GPIO_NUM 38
#define HREF_GPIO_NUM  47
#define PCLK_GPIO_NUM  13
