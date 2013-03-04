/*************************************************** 
  This is a library for the TMP006 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit TMP006 Breakout 
  ----> https://www.adafruit.com/products/391

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#define TMP006_B0 -0.0000294
#define TMP006_B1 -0.00000057
#define TMP006_B2 0.00000000463
#define TMP006_C2 13.4
#define TMP006_TREF 298.15
#define TMP006_A2 -0.00001678
#define TMP006_A1 0.00175
#define TMP006_S0 6.4  // * 10^-14

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "Wire.h"

#define TMP006_DEBUG 1

#define TMP006_I2CADDR 0x40
#define TMP006_MANID 0xFE
#define TMP006_DEVID 0xFF

#define TMP006_VOBJ  0x0
#define TMP006_TAMB 0x01



class Adafruit_TMP006 {
 public:
  Adafruit_TMP006();
  boolean begin(void);  // by default go highres

  int16_t readRawDieTemperature(void);
  int16_t readRawVoltage(void);
  double readObjTempC(void);
 private:
  uint16_t read16(uint8_t addr);
  void write16(uint8_t addr, uint16_t data);
};

