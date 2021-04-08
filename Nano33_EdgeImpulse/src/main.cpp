#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Arduino_LSM6DS3.h>

#define CONVERT_G_TO_MS2 9.80665F
#define FREQUENCY_HZ 104
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))

struct Acc_senseData
{
  float acc_x = 0.0F;
  float acc_y = 0.0F;
  float acc_z = 0.0F;
};

static Acc_senseData acc_data;

void setup() {

  Serial.println(9600);
  Serial.println("Started");

  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while(1);
  }

  
}

void loop() {

  static unsigned long last_interval_ms = 0;

  float a_x, a_y, a_z;

  if (millis() > last_interval_ms + INTERVAL_MS)
  {
    IMU.readAcceleration(a_x, a_y, a_z);
    acc_data.acc_x = a_x;
    acc_data.acc_y = a_y;
    acc_data.acc_z = a_z;

    Serial.print(acc_data.acc_x * CONVERT_G_TO_MS2);
    Serial.print('\t');
    Serial.print(acc_data.acc_y * CONVERT_G_TO_MS2);
    Serial.print('\t');
    Serial.println(acc_data.acc_z * CONVERT_G_TO_MS2);
  }
  
}