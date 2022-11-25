#ifndef Greenhouse_h
#define Greenhouse_h
#include <Arduino.h>
#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BH1750FVI.h>

class Greenhouse {
public:
    int NUTRIENT_PUMP_PIN;    int FAN_PIN;
    int NUTRIENT_HEATER_PIN;  int AIR_PUMP_PIN;
    int LIGHT_BULB_PIN;       int VENT1_SERVO_PIN;
    int VENT2_SERVO_PIN;      int HUMIDIFIFER_SERVO_PIN;
    int DHT_PIN;              int WATER_TEMP_PIN;
    byte PH_PIN;              uint8_t LIGHT_SENSOR_I2C;
    
    Greenhouse();
    Greenhouse(
      int NUTRIENT_PUMP_PIN,    int FAN_PIN,
      int NUTRIENT_HEATER_PIN,  int AIR_PUMP_PIN,
      int LIGHT_BULB_PIN,       int VENT1_SERVO_PIN,
      int VENT2_SERVO_PIN,      int HUMIDIFIFER_SERVO_PIN,
      int DHT_PIN,              int WATER_TEMP_PIN,
      byte PH_PIN,              uint8_t LIGHT_SENSOR_I2C 
    );
    void updatePINS(
      int NUTRIENT_PUMP_PIN,    int FAN_PIN,
      int NUTRIENT_HEATER_PIN,  int AIR_PUMP_PIN,
      int LIGHT_BULB_PIN,       int VENT1_SERVO_PIN,
      int VENT2_SERVO_PIN,      int HUMIDIFIFER_SERVO_PIN,
      int DHT_PIN,              int WATER_TEMP_PIN,
      byte PH_PIN,              uint8_t LIGHT_SENSOR_I2C
    );

    void humidityGoal(int percent);
    void waterTemperatueGoal(int celcius);
    void airTemperatureGoal(int celcius);

    void ventControl(bool vent1, bool vent2);
    void humidifierControl(int pushCount);
    void fanControl(bool on);
    void airPumpControl(bool on);
    void waterPumpControl(bool on);
    void lightBulbControl(bool on);
    
    void refreshSensorData();
    float refreshPHData(float calibration_value = 0);
    void setup();

    int getHumidity();
    int getAirTemp();
    int getWaterTemp();
    float getPH();
    int getLightIntensity();

private:
    int _humidity;          int _airTemperature;
    int _waterTemperature;  
    float _lightIntensity;
    Servo _vent1Servo;      Servo _vent2Servo;
    Servo _humidifierServo; DFRobot_DHT11 _DHT;
    OneWire _oneWire;       DallasTemperature _waterTemp;
    bool _vent1Status;      bool _vent2Status;
    BH1750FVI _myLux;


    float _pH;              int _buffer_arr[10],temp;
    unsigned long int _avgvalmV; 


};
#endif