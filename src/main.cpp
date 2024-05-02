

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

  pinMode(5, OUTPUT);
  analogWrite(5, 250);
}


void loop()
{
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
    {
      displayInfo();
    }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      delay(1);
  }

  delay(2000);
}
