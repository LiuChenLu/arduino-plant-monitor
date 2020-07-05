#include <math.h>

//Schematic:
// [Ground] --|-- [10k-Resistor] -------|------- [Thermistor 503] ---- [+5v]
//            |                         |                               |
//            |                    Analog Pin 0                         |
//            |                                                         |
//            |                                                         |
//            |-- [10k-Resistor] -------|------- [LDR]---------------- [+5v]
//                                      |
//                                 Analog Pin 1

const int TEMPERTURE_ANALOG_INPUT_PIN = 0;
const int LIGHT_ANALOG_INPUT_PIN = 1;
const int LED_PIN = 13;

// enumerating 3 major temperature scales
enum {
  T_KELVIN = 0,
  T_CELSIUS,
  T_FAHRENHEIT
};

#define EPISCO_K164_10k 4300.0f,298.15f,50000.0f  // B,T0,R0  


float LightLevel()
{
  const int MAX_ADC_READING = 1023;
  const int ADC_REF_VOLTAGE = 5.0;
  // REF_RESISTANCE is 5 kohm
  const int REF_RESISTANCE = 5030;

  // copied from https://www.allaboutcircuits.com/projects/design-a-luxmeter-using-a-light-dependent-resistor/
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
  double ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  // LDR_LUX
  // Change the code below to the proper conversion from ldrResistance to
  // ldrLux
  float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
  return ldrLux;
}

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
  Serial.println("Light");
  Serial.println(LightLevel());
  Serial.println("********");
  Serial.println(" ");

  delay(500);
}
