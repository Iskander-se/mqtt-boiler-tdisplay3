#include <WiFi.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
#include "Config.h"
#include "Images.h"

#define TFT_BL 4
#define BUTTON_01_PIN  35
#define BUTTON_02_PIN  0
#define RELAY_01 27
#define MSG_BUFFER_SIZE  (50)

#define ONE_WIRE_BUS_1 26 // ESP32 pin connected to DS18B20 sensor's DQ pin
#define ONE_WIRE_BUS_2 25 // ESP32 pin connected to DS18B20 sensor's DQ pin

OneWire oneWire_1(ONE_WIRE_BUS_1);
OneWire oneWire_2(ONE_WIRE_BUS_2);

DallasTemperature sensor_1(&oneWire_1);
DallasTemperature sensor_2(&oneWire_2);
WiFiClient espClient;
PubSubClient client(espClient);
TFT_eSPI tft = TFT_eSPI(135, 240);

bool checkTemp = true;


struct sTimeer {
  unsigned long previous;
  long period;
};

struct {
  int countdown = 0;
  int curView = 0;
  int lastView = 0;
  int wait = 0;
  bool relay = false;
  bool up = false;
} Sheduler;


struct sTemp {
  float tempC = -127;
  char charBuf[6] = "--";
  int error = 5;
  int alert = 0;
};

sTemp xSensor1;
sTemp xSensor2;

int resetTimer = 0;
bool OTA = false;
bool Check = false;
String cStatus;
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawString("Connecting to ", 1, 41);
  tft.drawString(ssid, 15, 61);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    cStatus = "no wifi";
    tft.setTextSize(2);

    if (i > 10) return;
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString("*", 8 + i * 14, 81);
    i++;
  }

  randomSeed(micros());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == MQTT_PUBLISH_TOPIC_stOTA) {
    if ((char)payload[0] == '1') {
      StartArduinoOTA();
      Serial.print("OTA!!!!");
    }
  }
  if (String(topic) == MQTT_PUBLISH_TOPIC_COM) {
    if ((char)payload[0] == '1') {
      Sheduler.countdown = 30;
    } else if ((char)payload[0] == '0') {
      Sheduler.countdown = 0;
    }
    Sheduler.up = true;
  }
  if (String(topic) == MQTT_PUBLISH_TOPIC_TIMER) {
    int timer1 = Sheduler.countdown;
    if ((char)payload[0] == '0') {
      timer1 = 0;
    }
    else if (length == 1) {
      //(char)payload[0].toInt() == '1'
      timer1 = (char)payload[0] - '0';
    }
    else if (length == 2) {
      //(char)payload[0].toInt() == '1'
      timer1 = (char)payload[0] - '0';
      timer1 = timer1 * 10 + (char)payload[1] - '0';
    }
    if ( Sheduler.countdown != timer1) Sheduler.up = true;
    Sheduler.countdown = timer1;
    Serial.println(timer1);
  }

}

void reconnect() {
  // Loop until we're reconnected
  int i = 0;
  if (WiFi.status() != WL_CONNECTED) setup_wifi();
  tft.fillScreen(TFT_BLACK);
  ViewBG();
  tft.setTextSize(2);
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.drawString("MQTT connection...", 8, 41);

  // Attempt to connect
  if (client.connect(MQTT_clientName, MQTT_username, MQTT_password, MQTT_PUBLISH_willTopic, 0, true, MQTT_payloadNotAvailable, true)) {
    tft.drawString("MQTT connected !!!   ", 8, 41);
    cStatus = "conn. ok";
    client.publish(MQTT_PUBLISH_willTopic, MQTT_payloadAvailable);
    client.subscribe(MQTT_PUBLISH_TOPIC_COM);
    client.subscribe(MQTT_PUBLISH_TOPIC_stOTA);
    //client.subscribe(MQTT_PUBLISH_TOPIC_TIMER);
  } else {
    tft.drawString("failed", 12, 61);
    cStatus = "no mqtt";
    delay(500);
  }
}

void setup() {
  pinMode(RELAY_01, OUTPUT);
  digitalWrite(RELAY_01, HIGH);
  pinMode(BUTTON_01_PIN, INPUT_PULLUP);

  sensor_1.begin();
  sensor_2.begin();

  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  analogWrite(TFT_BL, 100);
  sensor_1.requestTemperatures();
  xSensor1.tempC = sensor_1.getTempCByIndex(0);
  if (xSensor1.tempC < 0)  xSensor1.error = 5;
  sensor_2.requestTemperatures();
  xSensor2.tempC = sensor_2.getTempCByIndex(0);
  if (xSensor2.tempC < 0)  xSensor2.error = 5;

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

sTimeer mainTime = {0, 200};
sTimeer mqttTime = {0, 2000};
sTimeer ShedulerTime = {0, 60000};  //60000

int sysblink = 0;
void loop() {
  unsigned long currentMillis = millis();
  if (!client.connected()) {
    if (Sheduler.wait == 0) {
      readDSValue();
      reconnect();
      Serial.println("reconnect");
      Sheduler.wait = 2;
    }
  }
  client.loop();


  if (currentMillis - ShedulerTime.previous >= ShedulerTime.period) {
    ShedulerTime.previous = currentMillis;

    if (Sheduler.countdown > 0)   {
      Sheduler.countdown--;
    }
    else if ( Sheduler.wait > 0)  Sheduler.wait--;
    char charBuf[8];
    dtostrf(xSensor1.alert, 3, 2, charBuf);
    client.publish(MQTT_PUBLISH_TOPIC_ERR1, charBuf);
    dtostrf(xSensor2.alert, 3, 2, charBuf);
    client.publish(MQTT_PUBLISH_TOPIC_ERR2, charBuf);
    xSensor1.alert = 0;
    xSensor2.alert = 0;
  }

  if (currentMillis - mainTime.previous >= mainTime.period) {

    mainTime.previous = currentMillis;
    digitalWrite(RELAY_01, !Sheduler.relay);
    if ( Sheduler.wait > 0) analogWrite(TFT_BL, 210);
    else analogWrite(TFT_BL, 5);

    if (Sheduler.countdown > 0)   {
      Sheduler.wait = 1;
      Sheduler.relay = true;
    } else Sheduler.relay = false;


    if (Sheduler.countdown > 0) {
      Sheduler.curView = 2;
    } else Sheduler.curView = 1;


    if (OTA) {
      if (sysblink < 10) sysblink = 57;
      Sheduler.wait = 1;
      Sheduler.curView = 9;
      ArduinoOTA.handle();
    }

    if (Sheduler.lastView != Sheduler.curView) {
      Sheduler.lastView = Sheduler.curView;
      tft.fillScreen(TFT_BLACK);
    }



    switch (Sheduler.curView)
    {
      case 1:
        View2Temp();
        break;
      case 2:
        ViewCase2(Sheduler.countdown);
        break;
      case 9:
        ViewCaseOTA() ;
        break;
      default:
        ViewBG();
        ViewSmallTemp();
    }





  }

  if (currentMillis - mqttTime.previous >= mqttTime.period) {
    mqttTime.previous = currentMillis;
    readDSValue();
    char charBuf[8];
    if (Sheduler.wait < 2)   {
      dtostrf(Sheduler.countdown, 3, 2, charBuf);
      client.publish(MQTT_PUBLISH_TOPIC_TIMER, charBuf);
    }
    if (Sheduler.relay) client.publish(MQTT_PUBLISH_TOPIC_STATUS, "1");
    else client.publish(MQTT_PUBLISH_TOPIC_STATUS, "0");

    dtostrf(xSensor1.tempC, 3, 2, charBuf);
    if (xSensor1.tempC > 0) client.publish(MQTT_PUBLISH_TOPIC_TEMP1, charBuf);
    dtostrf(xSensor2.tempC, 3, 2, charBuf);
    if (xSensor2.tempC > 0) client.publish(MQTT_PUBLISH_TOPIC_TEMP2, charBuf);
  }

  button();
  delay(50);
}

bool readDSValue() {
  bool check = false;
  float temp = 0;
  sensor_1.setWaitForConversion(false);  // makes it async
  sensor_1.requestTemperatures();
  sensor_1.setWaitForConversion(true);
  temp = sensor_1.getTempCByIndex(0);

  if (temp > 0)  {
    if(xSensor1.error>3)  xSensor1.tempC = temp;
    xSensor1.tempC = (xSensor1.tempC * 4 + temp) / 5;
    xSensor1.error = 0;
  }
  else {
    if (xSensor1.error < 200)xSensor1.error++;
  }
  sensor_2.setWaitForConversion(false);  // makes it async
  sensor_2.requestTemperatures();
  sensor_2.setWaitForConversion(true);
  temp = sensor_2.getTempCByIndex(0);

  if (temp > 0)  {
    if (xSensor2.error > 3)  xSensor2.tempC = temp;
    xSensor2.tempC = (xSensor2.tempC * 9 + temp) / 10;
    xSensor2.error = 0;
  }
  else {
    if (xSensor2.error < 200)xSensor2.error++;
  }

  String nan = "NAN ";
  if (xSensor1.error < 2) {
    dtostrf(xSensor1.tempC, 2, 1, xSensor1.charBuf);
  }
  else  {
    nan.toCharArray(xSensor1.charBuf, 5);
    if (xSensor1.alert < 200)xSensor1.alert++;
  }
  if (xSensor2.error < 2) {
    dtostrf(xSensor2.tempC, 2, 1, xSensor2.charBuf);
  }
  else  {
    nan.toCharArray(xSensor2.charBuf, 5);
    if (xSensor2.alert < 200)xSensor2.alert++;
  }
  return check;
}


static uint32_t btn1_delay, btn2_delay;
void button() {
  int timer1 = Sheduler.countdown;
  if (!digitalRead(BUTTON_01_PIN))
  {
    if (millis() - btn1_delay > 500)
    {
      timer1 += 10;
      if (timer1 > 90) timer1 = 90;
      btn1_delay = millis();
    }
  }

  if (!digitalRead(BUTTON_02_PIN))
  {
    if (millis() - btn2_delay > 500)
    {
      timer1 = 0;
      Sheduler.wait = 1;
      Sheduler.curView = 1;
      btn1_delay = millis();
    }
  }

  if ( Sheduler.countdown != timer1) Sheduler.up = true;
  Sheduler.countdown = timer1;
}
