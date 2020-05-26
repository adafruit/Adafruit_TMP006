/***************************************************
  This is a library for the TMP006 Temp Sensor

  Designed specifically to work with the Adafruit TMP006 Breakout
  ----> https://www.adafruit.com/products/1296

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_TMP006.h"

//#define TESTDIE 0x0C78
//#define TESTVOLT 0xFEED

/**************************************************************************/
/*!
    @brief Constructor for TMP006 sensor object
    @param i2caddr The I2C sensor address to use
*/
/**************************************************************************/
Adafruit_TMP006::Adafruit_TMP006(uint8_t i2caddr) { _addr = i2caddr; }

/**************************************************************************/
/*!
    @brief Initialize I2C and connect to the TMP006 sensor
    @param samplerate The value written to TMP006_CONFIG
    @returns True if sensor found
*/
/**************************************************************************/
boolean Adafruit_TMP006::begin(uint16_t samplerate) {
  Wire.begin();

  write16(TMP006_CONFIG, TMP006_CFG_MODEON | TMP006_CFG_DRDYEN | samplerate);

  uint16_t mid, did;
  mid = read16(TMP006_MANID);
  did = read16(TMP006_DEVID);
#ifdef TMP006_DEBUG
  Serial.print("mid = 0x");
  Serial.println(mid, HEX);
  Serial.print("did = 0x");
  Serial.println(did, HEX);
#endif
  if (mid != 0x5449)
    return false;
  if (did != 0x67)
    return false;
  return true;
}

/**************************************************************************/
/*!
    @brief Enter sleep mode
*/
/**************************************************************************/
void Adafruit_TMP006::sleep() {
  // Read the control register and update it so bits 12-14 are zero to enter
  // sleep mode.
  uint16_t control = read16(TMP006_CONFIG);
  control &= ~(TMP006_CFG_MODEON);
  write16(TMP006_CONFIG, control);
}

/**************************************************************************/
/*!
    @brief Wake from sleep mode
*/
/**************************************************************************/
void Adafruit_TMP006::wake() {
  // Read the control register and update it so bits 12-14 are one to enter full
  // operation.
  uint16_t control = read16(TMP006_CONFIG);
  control |= TMP006_CFG_MODEON;
  write16(TMP006_CONFIG, control);
}

/**************************************************************************/
/*!
    @brief  Read the calibrated die temperature
    @returns double The calculated temperature of the sensor itself
*/
/**************************************************************************/
double Adafruit_TMP006::readDieTempC(void) {
  double Tdie = readRawDieTemperature();
  Tdie *= 0.03125; // convert to celsius
#ifdef TMP006_DEBUG
  Serial.print("Tdie = ");
  Serial.print(Tdie);
  Serial.println(" C");
#endif
  return Tdie;
}

/**************************************************************************/
/*!
    @brief  Read the calibrated object temperature
    @returns double The calculated temperature of the object in front of sensor
*/
/**************************************************************************/
double Adafruit_TMP006::readObjTempC(void) {
  double Tdie = readRawDieTemperature();
  double Vobj = readRawVoltage();
  Vobj *= 156.25;  // 156.25 nV per LSB
  Vobj /= 1000;    // nV -> uV
  Vobj /= 1000;    // uV -> mV
  Vobj /= 1000;    // mV -> V
  Tdie *= 0.03125; // convert to celsius
  Tdie += 273.15;  // convert to kelvin

#ifdef TMP006_DEBUG
  Serial.print("Vobj = ");
  Serial.print(Vobj * 1000000);
  Serial.println("uV");
  Serial.print("Tdie = ");
  Serial.print(Tdie);
  Serial.println(" C");
#endif

  double tdie_tref = Tdie - TMP006_TREF;
  double S = (1 + TMP006_A1 * tdie_tref + TMP006_A2 * tdie_tref * tdie_tref);
  S *= TMP006_S0;
  S /= 10000000;
  S /= 10000000;

  double Vos =
      TMP006_B0 + TMP006_B1 * tdie_tref + TMP006_B2 * tdie_tref * tdie_tref;

  double fVobj = (Vobj - Vos) + TMP006_C2 * (Vobj - Vos) * (Vobj - Vos);

  double Tobj = sqrt(sqrt(Tdie * Tdie * Tdie * Tdie + fVobj / S));

  Tobj -= 273.15; // Kelvin -> *C
  return Tobj;
}

/**************************************************************************/
/*!
    @brief  Read the raw, uncalibrated die temperature
    @returns int16_t The raw data read from the TMP006_TAMB register
*/
/**************************************************************************/
int16_t Adafruit_TMP006::readRawDieTemperature(void) {
  int16_t raw = read16(TMP006_TAMB);

#if TMP006_DEBUG == 1

#ifdef TESTDIE
  raw = TESTDIE;
#endif

  Serial.print("Raw Tambient: 0x");
  Serial.print(raw, HEX);

  float v = raw / 4;
  v *= 0.03125;
  Serial.print(" (");
  Serial.print(v);
  Serial.println(" *C)");
#endif
  raw >>= 2;
  return raw;
}

/**************************************************************************/
/*!
    @brief  Read the raw, uncalibrated thermopile voltage
    @returns int16_t The raw data read from the TMP006_VOBJ register
*/
/**************************************************************************/
int16_t Adafruit_TMP006::readRawVoltage(void) {
  int16_t raw;

  raw = read16(TMP006_VOBJ);

#if TMP006_DEBUG == 1

#ifdef TESTVOLT
  raw = TESTVOLT;
#endif

  Serial.print("Raw voltage: 0x");
  Serial.print(raw, HEX);
  float v = raw;
  v *= 156.25;
  v /= 1000;
  Serial.print(" (");
  Serial.print(v);
  Serial.println(" uV)");
#endif
  return raw;
}

/**************************************************************************/
/*!
    @brief  Read uint16_t bits of data from a register
    @param    a The register address to write to
    @returns uint16_t The data read from the register
*/
/**************************************************************************/
uint16_t Adafruit_TMP006::read16(uint8_t a) {
  uint16_t ret;

  Wire.beginTransmission(_addr); // start transmission to device
#if (ARDUINO >= 100)
  Wire.write(a); // sends register address to read from
#else
  Wire.send(a);         // sends register address to read from
#endif
  Wire.endTransmission(); // end transmission

  Wire.requestFrom(_addr, (uint8_t)2); // send data n-bytes read
#if (ARDUINO >= 100)
  ret = Wire.read(); // receive DATA
  ret <<= 8;
  ret |= Wire.read(); // receive DATA
#else
  ret = Wire.receive(); // receive DATA
  ret <<= 8;
  ret |= Wire.receive(); // receive DATA
#endif

  return ret;
}

/**************************************************************************/
/*!
    @brief  Write uint16_t bits of data to a register
    @param    a The register address to write to
    @param    d The data to write to the register
*/
/**************************************************************************/
void Adafruit_TMP006::write16(uint8_t a, uint16_t d) {
  Wire.beginTransmission(_addr); // start transmission to device
#if (ARDUINO >= 100)
  Wire.write(a);      // sends register address to read from
  Wire.write(d >> 8); // write data
  Wire.write(d);      // write data
#else
  Wire.send(a);          // sends register address to read from
  Wire.send(d >> 8);     // write data
  Wire.send(d);          // write data
#endif
  Wire.endTransmission(); // end transmission
}
