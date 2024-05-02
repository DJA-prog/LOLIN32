#include <WiFi.h>
const char *ssid = "Raptor95";
const char *password = "12345@admin!";
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

#include <PubSubClient.h>
const char *mqtt_server = "dino-pi5";
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

#include <TinyGPSPlus.h>
TinyGPSPlus gps;
void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print("Lat: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(",");
    Serial.print("Lng: ");
    Serial.print(gps.location.lng(), 6);
    Serial.println();
  }
  else
  {
    Serial.println("INVALID");
  }
}
void displayInfo1()
{
  if (!mqttClient.connected())
  {
    mqtt_reconnect();
  }
  // Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    mqttClient.publish("/LOLIN32/1/lat", String(gps.location.lat(), 6).c_str());
    mqttClient.publish("/LOLIN32/1/lng", String(gps.location.lng(), 6).c_str());
  }
  else
  {
    mqttClient.publish("/LOLIN32/1/gps", "INVALID");
  }
}
void updateSerial()
{
  delay(500);
  while (Serial.available())
    Serial2.write(Serial.read()); // Forward what Serial received to Software Serial Port

  while (Serial2.available())
    Serial.write(Serial2.read()); // Forward what Software Serial received to Serial Port
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);

  connectToWiFi();
  startOTA();
  startMQTT();
  
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
      displayInfo1();
    }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      delay(1);
  }

  mqttClient.loop();

  delay(2000);
}
