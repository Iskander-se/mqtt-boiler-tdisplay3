#pragma once
#include <stdint.h>

// ==== Hardware ====
constexpr uint8_t RELAY_PIN = 5;

// ==== Timing ====
constexpr uint32_t LOGIC_PERIOD_MS = 200;
constexpr uint32_t UI_PERIOD_MS    = 500;

// ==== WiFi ====
constexpr char WIFI_SSID[] = "####";  //Iskander-AP    //"xxx"
constexpr char WIFI_PASS[] = "####"; //"xxx";

// ==== MQTT ====
constexpr char MQTT_HOST[] = "192.168.5.#";
constexpr uint16_t MQTT_PORT = 1883;

const char* MQTT_username = "####"; // login user to server MQTT
const char* MQTT_password = "###"; // password login to server MQTT
const char* MQTT_clientName = "mqtt_boiler"; // host name device ini

//countdown
const char* MQTT_PUBLISH_TOPIC_STATUS = "mqtt_boiler/status";
const char* MQTT_PUBLISH_willTopic = "mqtt_boiler/avaliability";

const char* MQTT_payloadAvailable = "OK";
const char* MQTT_payloadNotAvailable = "Fail";