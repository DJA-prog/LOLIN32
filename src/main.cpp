// #include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <dht11.h>

dht11 dht;

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

  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 5);
  // Display static text
  display.println("Hello");
  display.display();
  delay(2000);
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);

  startOLED();

  pinMode(5, OUTPUT);
  analogWrite(5, 250);
}

int  count = 0;

void loop()
{

  display.setCursor(0, 5);
  display.clearDisplay();
  dht.read(15);

  display.print("T: ");
  display.println(dht.temperature);
  display.print("H: ");
  display.println(dht.humidity);
  display.display();

  delay(1000);
  count++;
}
