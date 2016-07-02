#include "BAC_Lookup.h"
#include "nokia.h"
// #include "U8glib.h"

#define PIN_SCE   4
#define PIN_RESET 3
#define PIN_DC    5
#define PIN_SDIN  6
#define PIN_SCLK  7

#define backlight_pin 11

//U8GLIB_PCD8544 u8g(PIN_SCLK, PIN_SDIN, PIN_SCE, PIN_DC, PIN_RESET);

#define BAC_START   410     // Beginning ADC value of BAC chart
#define BAC_END     859     // Lsat ADC value in BAC chart
#define MQ3_PIN 2

void LcdInitialise(void)
{
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xB1 );  // Set LCD Vop (Contrast). 
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x20 );  // LCD Basic Commands
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.

  digitalWrite(backlight_pin, HIGH);
}

void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  analogWrite(backlight_pin, 200);
}

void LcdCharacter(char character)
{
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++)
  {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void LcdClear(void)
{
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    LcdWrite(LCD_D, 0x00);
  }
}

void LcdString(const char *characters)
{
  while (*characters)
  {
    LcdCharacter(*characters++);
  }
}

void setup(void)
{
  Serial.begin(9600);
  LcdInitialise();
  LcdClear();
  LcdString("CONST v0.2");
//  lcd.setCursor(0,1);
//  lcd.write("BAC: ");
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

//  lcd.setCursor(5,1);
//  lcd.write(temp_str);
  delay(2000);
}
