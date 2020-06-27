#include <math.h>

//Schematic:
// [Ground] ---- [10k-Resistor] -------|------- [Thermistor] ---- [+5v]
//                                     |
//                                     Analog Pin 0


const int TEMPERTURE_ANALOG_INPUT_PIN = 0;
const int LIGHT_ANALOG_INPUT_PIN = 1;

// enumerating 3 major temperature scales
enum {
  T_KELVIN = 0,
  T_CELSIUS,
  T_FAHRENHEIT
};

#define EPISCO_K164_10k 4300.0f,298.15f,50000.0f  // B,T0,R0  

float Temperature(int OutputUnit)
{
  const float B = 4300.0f;
  const float T0 = 298.15f;
  const float R0 = 50000.0f;
  const float R_Balance = 10000.0f;

  float R = R_Balance * (1024.0f / float(analogRead(TEMPERTURE_ANALOG_INPUT_PIN)) - 1);
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

//example of use #2
// using numbers instead of episco k164 definition
// this time reading from analog input 2
// getting result in fahrenheit

void setup() {
  Serial.begin(9600);
}

void loop() {

  Serial.println("********");
  Serial.println("Temp");
  Serial.println(Temperature(T_CELSIUS));
  Serial.println("********");
  Serial.println(" ");

  delay(500);
}
