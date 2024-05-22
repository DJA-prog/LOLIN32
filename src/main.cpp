#include <WiFi.h>
const char *ssid = "LabPi0";
const char *password = "12345@admin";

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
// #include <WebServer.h>

WiFiClient espClient;

#include <PubSubClient.h>
const char *mqtt_server = "192.168.2.2";
int mqtt_port = 1883;
const char *mqtt_username = "hass";
const char *mqtt_password = "12345@admin";
bool mqtt_bypass_diff = true;
PubSubClient mqttClient(espClient);
void mqtt_reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("LOLIN32_1", mqtt_username, mqtt_password))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
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
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}


void setup()
{
  Serial.begin(115200);

  connectToWiFi();
  startMQTT();

  pinMode(5, OUTPUT);
  analogWrite(5, 250);

}


void loop()
{
  if (!mqttClient.connected())
  {
    mqtt_reconnect();
  }
  else
  {
    dht.read(DHTPIN);
    if (dht.humidity != h_old || mqtt_bypass_diff)
    {
      mqttClient.publish("/LOLIN32/1/humidity", String(dht.humidity).c_str());
      h_old = dht.humidity;
    }
    if (dht.temperature != t_old || mqtt_bypass_diff)
    {
      mqttClient.publish("/LOLIN32/1/temperature", String(dht.temperature).c_str());
      t_old = dht.temperature;
    }
  }
  mqttClient.loop();

  delay(2000);
}
