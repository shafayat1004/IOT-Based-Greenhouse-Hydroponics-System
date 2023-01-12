#ifndef Greenhouse_h
#define Greenhouse_h
#include <Arduino.h>
#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BH1750FVI.h>
#include <ArduinoJson.h>

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

    void setCurrentConfig(HardwareSerial &espSerial = Serial3);
    void uploadSensorData(HardwareSerial &espSerial = Serial3);
    void testAllSystems(HardwareSerial &espSerial = Serial3);

    void updateTime(HardwareSerial &espSerial = Serial3);
    void printTime(HardwareSerial &espSerial = Serial3);
    void getSunsetSunrise(HardwareSerial &espSerial = Serial3);
    void printSunsetSunrise();

    void printCurrentConfig();
    void printSensorData(String select = "All");

    void maintainConfig(int delayTime, int tempHumiditySlack, float pHSlack);

    int getHumidity();
    void increaseHumidity();
    void decreaseHumidity();
    int humidityAdjust(int slack = 2);

    int getWaterTemp();
    void increaseWaterTemperature();
    void decreaseWaterTemperature();
    int waterTemperatureAdjust(int slack = 2);

    int getAirTemp();
    void increaseAirTemperature();
    void decreaseAirTemperature();
    int airTemperatureAdjust(int slack = 2);

    float getPH();
    void increasePH();
    void decreasePH();
    int pHAdjust(int slack = 0.3);

    void setNutrientPumpFrequency();

    void ventControl(int vent1, int vent2);
    void humidifierControl(int pushCount);
    void fanControl(bool on);
    void waterHeaterControl(bool on);
    // void airPumpControl(bool on);
    void waterPumpControl(bool on);
    void lightBulbControl(bool on);
    
    void refreshSensorData();
    float refreshPHData(float calibration_value = 0);
    void setup();

    int getLightIntensity();

private:
    int _humidity;          int _airTemperature;
    int _nutrientTemperature;  
    int _lightIntensity;
    Servo _vent1Servo;      Servo _vent2Servo;
    Servo _humidifierServo; DFRobot_DHT11 _DHT;
    OneWire _oneWire;       DallasTemperature _nutrientTemp;
    bool _vent1Status;      bool _vent2Status;
    BH1750FVI _myLux;


    float _pH;                int _buffer_arr[10],temp;
    unsigned long int _avgvalmV; 

    StaticJsonDocument<256> _sensorData;
    StaticJsonDocument<256> _currentConfig;
    StaticJsonDocument<128> _sunsetSunrise;
    int _airTemperatureGoal;  int _nutrientTemperatureGoal;
    int _lightIntensityGoal;  float _pHGoal;
    int _humidityGoal;        int _lightDurationGoal;
    int _nutrientPumpFrequency;
    int _nutrientReplacementTime;
    int _initialNutrientVolume;

    int _Day;  /*Day*/        int _Hour;  /*Hour*/
    int _Minute;  /*Minute*/  int _Second;  /*Second*/
    char* _sunsetTime;        char* _sunriseTime;
    StaticJsonDocument<128> _currentTime;

};
#endif