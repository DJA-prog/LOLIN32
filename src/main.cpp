#include <WiFi.h>
const char *ssid = "";
const char *password = "";
void connectToWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  // Print IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.setHostname("wemos_1");
  WiFi.setTxPower(WIFI_POWER_5dBm);
  Serial.print("TX power: ");
  Serial.println(WiFi.getTxPower());

  // Print MAC address
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  for (int i = 0; i < 6; ++i)
  {
    Serial.print(mac[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println();
}

#include <WiFiClient.h>
#include <WebServer.h>

#include <ElegantOTA.h>
WebServer server(80);
WiFiClient espClient;
void startOTA()
{
  /* OTA */
  server.on("/", []()
            { server.send(200, "text/plain", "Hi! This is ElegantOTA Demo."); });
  ElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
}

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
AsyncWebServer server1(81);
void recvMsg(uint8_t *data, size_t len)
{
  WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }

  if (d == "reboot")
  {
    WebSerial.println("Rebooting...");
    ESP.restart();
  }
  WebSerial.println(d);
}
void startWebSerial()
{
  /* WEBSERIAL */
  WebSerial.begin(&server1);
  WebSerial.msgCallback(recvMsg);
  server1.begin();
}

#include <SoftwareSerial.h>
SoftwareSerial mySerial(17, 16); // RX on pin 17, TX on pin 16
#include <PubSubClient.h>
const char *mqtt_server = "";
int mqtt_port = 1883;
const char *mqtt_username = "";
const char *mqtt_password = "";
bool mqtt_bypass_diff = true;
PubSubClient mqttClient(espClient);
void mqtt_reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    WebSerial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("LOLIN32_1", mqtt_username, mqtt_password))
    {
      WebSerial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
    }
    else
    {
      WebSerial.print("failed, rc=");
      WebSerial.print(mqttClient.state());
      WebSerial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void startMQTT()
{
  mqttClient.setServer(mqtt_server, mqtt_port);
}

#include "dht11.h"
#define DHTPIN 32
dht11 dht;
float t_old, h_old;
void dht11_readings()
{
  dht.read(DHTPIN);
  float h = dht.humidity;
  float t = dht.temperature;

  if (isnan(h) || isnan(t))
  {
    WebSerial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  WebSerial.print(F("Humidity: "));
  WebSerial.print(h);
  WebSerial.print(F("%  Temperature: "));
  WebSerial.print(t);
  WebSerial.println(F("°C "));
}


void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);

  connectToWiFi();
  startOTA();
  // startWebSerial();
  delay(3000);
  WebSerial.println("Connected");

  pinMode(5, OUTPUT);
  analogWrite(5, 250);
}


void loop()
{
  server.handleClient();
  ElegantOTA.loop();

  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
    {
      displayInfo();
      displayInfo2();
    }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      delay(1);
  }

  // WebSerial.println("Here!");

  // while (Serial2.available())
  // {
  //   char i = (char)Serial2.read();
  //   Serial.print(i);
  // }

  // delay(2000);
}
