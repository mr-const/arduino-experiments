#include "U8glib.h"
 
#define backlight_pin 11
 
 
U8GLIB_PCD8544 u8g(8, 4, 7, 5, 6);  // CLK=8, DIN=4, CE=7, DC=5, RST=6

int sensorValue;

void draw(const char* msg) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_profont11);  // select font
  u8g.drawStr( 4, 15, "hello world");
}

void setup()
{
  Serial.begin(9600);
  analogWrite(backlight_pin, 20);  /* Set the Backlight intensity */
  u8g.setColorIndex(0); // pixel on
}

void loop()
{
  sensorValue = analogRead(0);       // read analog input pin 0
  Serial.println("Sensor read:");
  Serial.println(sensorValue, DEC);  // prints the value read

  static char tempStr[20];
  sprintf(tempStr, "BAC: %d", sensorValue);
  // picture loop
  u8g.firstPage();
  do {
    draw(tempStr);
  } while( u8g.nextPage() );

  delay(2000);
}
