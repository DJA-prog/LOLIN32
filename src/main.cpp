#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void startOLED()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  // display.setCursor(0, 5);
  // Display static text
  // display.println("Hello, world!");
  // display.display();
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
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    display.clearDisplay();
    display.setCursor(0, 2);
    display.println("Location: ");
    display.print("LAT: ");
    display.println(gps.location.lat(), 6);
    display.print("LNG: ");
    display.println(gps.location.lng(), 6);
    display.display();
  }
  else
  {
    display.clearDisplay();
    display.setCursor(40, 10);
    display.println("INVALID");
    display.display();
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

  startOLED();

  pinMode(5, OUTPUT);
  analogWrite(5, 250);
}


void loop()
{
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

  delay(2000);
}
