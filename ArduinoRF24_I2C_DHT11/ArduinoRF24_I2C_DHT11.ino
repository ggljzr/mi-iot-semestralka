/*
LCD i2c library: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
LCD i2c module: for example http://www.aliexpress.com/item/LCD1602-I2C-LCD-1602-module-Blue-screen-IIC-I2C-for-arduino-LCD1602-Adapter-plate/32651314933.html
DHT11 library: https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib
*/

#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

dht DHT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHT11_PIN 5
#define DHT11_POWER_PIN 6

#define TEMP_STR "Temp: "
#define HUM_STR "Hum: "

void setup()
{
  Serial.begin(115200);
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");

  pinMode(DHT11_POWER_PIN, OUTPUT);
  digitalWrite(DHT11_POWER_PIN, HIGH);

  lcd.begin();
  lcd.backlight();
}

void loop()
{
  // READ DATA
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
		Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.print("Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.print("Time out error,\t"); 
		break;
    default: 
		Serial.print("Unknown error,\t"); 
		break;
  }
  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(TEMP_STR);
  lcd.setCursor(6,0);
  lcd.print(DHT.temperature);
  lcd.setCursor(0,1);
  lcd.print(HUM_STR);
  lcd.setCursor(6,1);
  lcd.print(DHT.humidity);

  delay(2000);
}
//
// END OF FILE
//
