#include <Greenhouse.h>
#include <Wire.h>
#include <ArduinoJson.h>


// Outputs ###########################################

// Relay Module 1 DC
#define NUTRIENT_PUMP_PIN 11
#define FAN_PIN 10

// Relay Module 2 AC
#define NUTRIENT_HEATER_PIN 12
#define LIGHT_BULB_PIN 13
#define AIR_PUMP_PIN 22

#define VENT1_SERVO_PIN 3  //90 is on
#define VENT2_SERVO_PIN 2  // 0 is on
#define HUMIDIFIFER_SERVO_PIN 4


// Inputs ###########################################
#define DHT_PIN 7
#define WATER_TEMP_PIN 6
#define PH_PIN A0
#define LIGHT_SENSOR_I2C 0x23

// ESP is in Serial3 [RX3, TX3]




Greenhouse house;

void setup() {
    Serial.begin(115200);
    Serial3.begin(4800);

    house.updatePINS(
             NUTRIENT_PUMP_PIN,     FAN_PIN,
             NUTRIENT_HEATER_PIN,   AIR_PUMP_PIN,
             LIGHT_BULB_PIN,        VENT1_SERVO_PIN,
             VENT2_SERVO_PIN,       HUMIDIFIFER_SERVO_PIN,
             DHT_PIN,               WATER_TEMP_PIN,
             PH_PIN,                LIGHT_SENSOR_I2C);
    house.setup();
    // house.testAllSystems();
}
void loop() {
    // house.printTime(Serial3);
    // house.getSunsetSunrise();
    // house.printSunsetSunrise();
    
    // house.setCurrentConfig(Serial3);
    // house.printCurrentConfig();
    // house.refreshSensorData();
    // house.printSensorData();
    // house.uploadSensorData(Serial3);
    // house.waterPumpControl(1);
    // delay(5000);
    // house.waterPumpControl(0);
    // house.maintainConfig(10000,0,0.3);
    house.ventControl(0, 0);
    
    delay(5000);
    
}