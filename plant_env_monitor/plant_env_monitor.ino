#include <math.h>

//Schematic (excluding sd card module):
// [Ground] --|-- [10k-Resistor] -------|------- [Thermistor 503] ---- [+5v]
//            |                         |                               |
//            |                    Analog Pin 0                         |
//            |                                                         |
//            |                                                         |
//            |-- [10k-Resistor] -------|------- [LDR]---------------- [+5v]
//                                      |
//                                 Analog Pin 1
//
// SD card Module pin wiring:
// MISO 12
// CLK 13
// MOSI 11
// CS Digital 4
// VCC 5v
// GND GND

const int TEMPERTURE_ANALOG_INPUT_PIN = 0;
const int LIGHT_ANALOG_INPUT_PIN = 1;
const int SD_CHIP_SELECT = 4;

///////////////////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////////////////

#include <AceTime.h>

using namespace ace_time;
using namespace ace_time::clock;

// ZoneProcessor instance should be created statically at initialization time.
static BasicZoneProcessor easternProcessor;

static SystemClockLoop systemClock(nullptr /*reference*/, nullptr /*backup*/);

void ClockSetup()
{
#if ! defined(UNIX_HOST_DUINO)
  delay(1000);
#endif

  systemClock.setup();

  auto easternTz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_New_York,
                                         &easternProcessor);

  // Set the SystemClock using these components.
  auto easternTime = ZonedDateTime::forComponents(
                       2020, 7, 5, 19, 39, 0, easternTz);
  systemClock.setNow(easternTime.toEpochSeconds());
}

String GetCurrentTime()
{
  acetime_t now = systemClock.getNow();

  // Creating timezones is cheap, so we can create them on the fly as needed.
  auto easternTz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_New_York,
                                         &easternProcessor);
  auto easternTime = ZonedDateTime::forEpochSeconds(now, easternTz);

  String timeString = String(easternTime.year()) + ' ' + String(easternTime.month()) + ' '
                      + String(easternTime.day()) + ' ' + String(easternTime.hour()) + ':'
                      + String(easternTime.minute());
  return timeString;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Light level sensor
// based on https://www.allaboutcircuits.com/projects/design-a-luxmeter-using-a-light-dependent-resistor/
///////////////////////////////////////////////////////////////////////////////////////////

float LightLevel()
{
  const int MAX_ADC_READING = 1023;
  const int ADC_REF_VOLTAGE = 5.0;
  // REF_RESISTANCE is 5 kohm
  const int REF_RESISTANCE = 5030;

  // calculated from Lux_Approximation_from_LDR.xlsx
  const double LUX_CALC_SCALAR = 3570405606;
  const double LUX_CALC_EXPONENT = -2.141;

  // Perform the analog to digital conversion
  int ldrRawData = analogRead(LIGHT_ANALOG_INPUT_PIN);

  // RESISTOR VOLTAGE_CONVERSION
  // Convert the raw digital data back to the voltage that was measured on the analog pin
  float resistorVoltage = (float)ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;


  // voltage across the LDR is the 5V supply minus the 5k resistor voltage
  float ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;

  // LDR_RESISTANCE_CONVERSION
  // resistance that the LDR would have for that voltage
  double ldrResistance = ldrVoltage / resistorVoltage * REF_RESISTANCE;
  // LDR_LUX
  // Change the code below to the proper conversion from ldrResistance to
  // ldrLux
  float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
  return ldrLux;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Temperture sensor
///////////////////////////////////////////////////////////////////////////////////////////

// enumerating 3 major temperature scales
enum {
  T_KELVIN = 0,
  T_CELSIUS,
  T_FAHRENHEIT
};

float Temperature(int OutputUnit)
{
  const float B = 4300.0f;
  const float T0 = 298.15f;
  const float R0 = 50000.0f;
  const float rBalance = 10000.0f;

  float R = rBalance * (1024.0f / float(analogRead(TEMPERTURE_ANALOG_INPUT_PIN)) - 1);
  float T = 1.0f / (1.0f / T0 + (1.0f / B) * log(R / R0));

  switch (OutputUnit) {
    case T_CELSIUS :
      T -= 273.15f;
      break;
    case T_FAHRENHEIT :
      T = 9.0f * (T - 273.15f) / 5.0f + 32.0f;
      break;
    default:
      break;
  };

  return T;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Write to SD card
///////////////////////////////////////////////////////////////////////////////////////////

#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card

void SdSetup()
{
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("SD initialization failed!");
    return;
  }

  //open file
  File myFile = SD.open("DATA.txt", FILE_WRITE);

  // if the file opened ok, write to it:
  if (myFile) {
    Serial.println("File opened ok");
    // print the headings for our data
    myFile.println("Time Stamp,Temperature ºC,Lux");
  }
  myFile.close();
}

void LogToSd() {
  File myFile = SD.open("DATA.txt", FILE_WRITE);

  if (myFile) {
    myFile.println(GetCurrentTime() + ',' + Temperature(T_CELSIUS) + ',' + LightLevel());
    Serial.println("wrote to sd: " + GetCurrentTime() + ',' + Temperature(T_CELSIUS)
                   + ',' + LightLevel());
  }
  myFile.close();
}


void LogToSdAtInterval(int seconds) {
  static acetime_t prevNow = systemClock.getNow();
  systemClock.loop();
  acetime_t now = systemClock.getNow();
  if (now - prevNow >= seconds) {
    LogToSd();
    prevNow = now;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
// Setup and loop
///////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);

  ClockSetup();
  SdSetup();
}

void loop() {
  LogToSdAtInterval(2);
}
