#include "BAC_Lookup.h"
#include <LiquidCrystal.h>

#define BAC_START   410     // Beginning ADC value of BAC chart
#define BAC_END     859     // Lsat ADC value in BAC chart
#define MQ3_PIN 2

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(void)
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("const v0.1");
  lcd.setCursor(0,1);
  lcd.write("BAC: ");
}

int sensor_read = 0;
uint8_t bac;

void loop()
{
  sensor_read = analogRead(MQ3_PIN);       // read analog input pin 0
  Serial.println("Sensor read:");
  Serial.println(sensor_read, DEC);  // prints the value read

  static char temp_str[20];
  
  if ( sensor_read < BAC_START ) {
      bac = 0;
      sprintf(temp_str, "0000");
  } else if ( sensor_read > BAC_END ) {
      sprintf(temp_str, "EEEE");
  } else {
      sensor_read = sensor_read - BAC_START;
      bac = bac_chart[sensor_read];
      if ( bac < 10 ) {
        sprintf(temp_str, "000%1d", bac);
      } else if ( bac < 100 ) {
        sprintf(temp_str, "00%2d", bac);
      } else {
        sprintf(temp_str, "0%3d", bac);
      }
  }  

  lcd.setCursor(5,1);
  lcd.print(sensor_read);
  delay(2000);
}
