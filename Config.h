
const char* ssid = "Iskander-AP";
const char* password = "######";
const char* mqtt_server = "#####";

const char* MQTT_username = "openhabian"; // login user to server MQTT
const char* MQTT_password = "####"; // password login to server MQTT
const char* MQTT_clientName = "mqtt_boiler"; // host name device ini

const char* MQTT_PUBLISH_TOPIC_TEMP1 = "mqtt_boiler/temp1";
const char* MQTT_PUBLISH_TOPIC_TEMP2 = "mqtt_boiler/temp2";
const char* MQTT_PUBLISH_TOPIC_ERR1 = "mqtt_boiler/err1";
const char* MQTT_PUBLISH_TOPIC_ERR2 = "mqtt_boiler/err2";
const char* MQTT_PUBLISH_TOPIC_COM = "mqtt_boiler/set";
const char* MQTT_PUBLISH_TOPIC_TIMER = "mqtt_boiler/time";
//countdown
const char* MQTT_PUBLISH_TOPIC_STATUS = "mqtt_boiler/status";
const char* MQTT_PUBLISH_TOPIC_stOTA = "mqtt_boiler/OTA";
const char* MQTT_PUBLISH_willTopic = "mqtt_boiler/avaliability";

const char* MQTT_payloadAvailable = "OK";
const char* MQTT_payloadNotAvailable = "Fail";
