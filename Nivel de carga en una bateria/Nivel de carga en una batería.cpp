#include "thingProperties.h"

int analogInPin = A0;  // Pin de entrada analógica
int sensorValue; 
float calibration = 0.3260; // Ajusta este valor basado en lecturas de multímetro

// Parámetros del divisor de voltaje
const float R1 = 100000.0; // Resistencia 1 en ohmios
const float R2 = 100000.0; // Resistencia 2 en ohmios

// Función para filtrar las lecturas de voltaje (media móvil simple)
#define NUM_SAMPLES 10
float voltageSamples[NUM_SAMPLES];
int sampleIndex = 0;

float readFilteredVoltage() {
  sensorValue = analogRead(analogInPin);
  float rawVoltage = (sensorValue * 3.3) / 4095; // Voltaje en el pin analógico
  float batteryVoltage = rawVoltage * ((R1 + R2) / R2) + calibration; // Voltaje de la batería
  voltageSamples[sampleIndex] = batteryVoltage;
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

  float total = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    total += voltageSamples[i];
  }
  return total / NUM_SAMPLES;
}

int voltageToPercentage(float voltage) {
  // Asegurar que el voltaje esté dentro de los límites
  if (voltage >= 4.2) {
    return 100;
  } else if (voltage <= 3.0) { // Ajuste del mínimo para reflejar mejor el porcentaje
    return 0;
  } else {
    return (voltage - 3.0) / (4.2 - 3.0) * 100;
  }
}

void setup() {
  Serial.begin(9600);
  delay(1500); 
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  
  // Inicializar el array de muestras de voltaje
  for (int i = 0; i < NUM_SAMPLES; i++) {
    voltageSamples[i] = 0;
  }
}

void loop() {
  ArduinoCloud.update();
  
  voltage = readFilteredVoltage();
  bat_percentage = voltageToPercentage(voltage);

  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);

  delay(1000);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
