#include <Greenhouse.h>


// Outputs ###########################################

// Relay Module 1 DC
#define NUTRIENT_PUMP_PIN 13
#define FAN_PIN 12

// Relay Module 2 AC
#define NUTRIENT_HEATER_PIN 8
#define AIR_PUMP_PIN 9
#define LIGHT_BULB_PIN 10

#define VENT1_SERVO_PIN 3
#define VENT2_SERVO_PIN 4
#define HUMIDIFIFER_SERVO_PIN 5


// Inputs ###########################################
#define DHT_PIN 7
#define WATER_TEMP_PIN 6
#define PH_PIN A0
#define LIGHT_SENSOR_I2C 0x23

Greenhouse house;

void setup() {
    Serial.begin(9600);
    house.updatePINS(
             NUTRIENT_PUMP_PIN,     FAN_PIN,
             NUTRIENT_HEATER_PIN,   AIR_PUMP_PIN,
             LIGHT_BULB_PIN,        VENT1_SERVO_PIN,
             VENT2_SERVO_PIN,       HUMIDIFIFER_SERVO_PIN,
             DHT_PIN,               WATER_TEMP_PIN,
             PH_PIN,                LIGHT_SENSOR_I2C);
    house.setup();
}
void loop() {
    house.refreshSensorData();
    Serial.println("pH :");
    Serial.println(house.getPH());
    Serial.println("Air Temp :");
    Serial.println(house.getAirTemp());
    Serial.println("Humidity :");
    Serial.println(house.getHumidity());
    Serial.println("Water Temp :");
    Serial.println(house.getWaterTemp());
    Serial.println("Light Intensity (lux) :");
    Serial.println(house.getLightIntensity());
    house.humidifierControl(5);
    house.ventControl(0, 0);
    delay(1000);
    house.ventControl(0, 1);
    delay(1000);
    house.ventControl(1, 0);
    delay(1000);
    house.ventControl(1, 1);
    delay(2000);
    house.waterPumpControl(1);
    house.fanControl(0);
    delay(2000);
    house.waterPumpControl(0);
    house.fanControl(1);
    delay(2000);
}