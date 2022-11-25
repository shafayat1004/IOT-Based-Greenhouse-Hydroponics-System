#include <Arduino.h>
#include <Greenhouse.h>
#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <math.h> 
#include <BH1750FVI.h>

Greenhouse::Greenhouse(){}
Greenhouse::Greenhouse(
            int NUTRIENT_PUMP,    int FAN,
            int NUTRIENT_HEATER,  int AIR_PUMP,
            int LIGHT_BULB,       int VENT1_SERVO,
            int VENT2_SERVO,      int HUMIDIFIFER_SERVO,
            int DHT,              int WATER_TEMP,
            byte PH,               uint8_t LIGHT_SENSOR ){

    NUTRIENT_PUMP_PIN     = NUTRIENT_PUMP;
    FAN_PIN               = FAN;
    NUTRIENT_HEATER_PIN   = NUTRIENT_HEATER;
    AIR_PUMP_PIN          = AIR_PUMP;
    LIGHT_BULB_PIN        = LIGHT_BULB;
    VENT1_SERVO_PIN       = VENT1_SERVO;
    VENT2_SERVO_PIN       = VENT2_SERVO;
    HUMIDIFIFER_SERVO_PIN = HUMIDIFIFER_SERVO;
    DHT_PIN               = DHT;
    WATER_TEMP_PIN        = WATER_TEMP;
    PH_PIN                = PH;
    LIGHT_SENSOR_I2C      = LIGHT_SENSOR; 
}
void Greenhouse::updatePINS(
            int NUTRIENT_PUMP,    int FAN,
            int NUTRIENT_HEATER,  int AIR_PUMP,
            int LIGHT_BULB,       int VENT1_SERVO,
            int VENT2_SERVO,      int HUMIDIFIFER_SERVO,
            int DHT,              int WATER_TEMP,
            byte PH,              uint8_t LIGHT_SENSOR ){

    NUTRIENT_PUMP_PIN     = NUTRIENT_PUMP;
    FAN_PIN               = FAN;
    NUTRIENT_HEATER_PIN   = NUTRIENT_HEATER;
    AIR_PUMP_PIN          = AIR_PUMP;
    LIGHT_BULB_PIN        = LIGHT_BULB;
    VENT1_SERVO_PIN       = VENT1_SERVO;
    VENT2_SERVO_PIN       = VENT2_SERVO;
    HUMIDIFIFER_SERVO_PIN = HUMIDIFIFER_SERVO;
    DHT_PIN               = DHT;
    WATER_TEMP_PIN        = WATER_TEMP;
    PH_PIN                = PH;
    LIGHT_SENSOR_I2C      = LIGHT_SENSOR; 
}


void Greenhouse::setup() {
    // Relay 12V DC
    pinMode(NUTRIENT_PUMP_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    waterPumpControl(0);
    fanControl(0);

    // Relay OUTLET AC
    pinMode(NUTRIENT_HEATER_PIN, OUTPUT);
    pinMode(AIR_PUMP_PIN, OUTPUT);
    pinMode(LIGHT_BULB_PIN, OUTPUT);

    // Servos
    _vent1Servo.attach(VENT1_SERVO_PIN);
    _vent2Servo.attach(VENT2_SERVO_PIN);
    _humidifierServo.attach(HUMIDIFIFER_SERVO_PIN);

    // DS18B20 Liquid Temp Sensor
    // 4.7kOHM resistor needed between 5V and Signal
    _oneWire = OneWire(WATER_TEMP_PIN);
    _waterTemp = DallasTemperature(&_oneWire);
    _waterTemp.begin();

    // BH1750FVI
    // Sets sensor to high resolution mode.
    // Connect module to 3.3V or 5V and Gnd
    // Connect SDA pin to SDA on MCU and SCL pin to SCL on MCU
    // Leave ADDR floating.  It is pulled LOW to give I2C address of 0x23

    _myLux = BH1750FVI(LIGHT_SENSOR_I2C);
    Wire.begin();
    _myLux.powerOn();
    _myLux.setContHighRes();



}

void Greenhouse::refreshSensorData() {

    _DHT.read(DHT_PIN);
    _humidity       = _DHT.humidity;
    _airTemperature = _DHT.temperature;
    _waterTemp.requestTemperatures();
    _waterTemperature = _waterTemp.getTempCByIndex(0);
    _pH               = refreshPHData();
    _lightIntensity   = _myLux.getLux();
    
}

float Greenhouse::refreshPHData(float calibration_value){
    for (int i = 0; i < 10; i++)
    {
        _buffer_arr[i] = analogRead(A0);
        delay(30);
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = i + 1; j < 10; j++)
        {
            if (_buffer_arr[i] > _buffer_arr[j])
            {
                temp = _buffer_arr[i];
                _buffer_arr[i] = _buffer_arr[j];
                _buffer_arr[j] = temp;
            }
        }
    }
    _avgvalmV = 0;
    for (int i = 2; i < 8; i++)
        _avgvalmV += _buffer_arr[i];
    // float volt=(float)_avgvalmV*5.0/1024/6; 
    // ph_act = -5.70 * volt + calibration_value;
    float millivolt = (float)_avgvalmV / 6;
    return ((1024-millivolt)/1024) * 14 + calibration_value;
}

void Greenhouse::ventControl(bool vent1, bool vent2) {
    _vent1Servo.write(vent1 * 180);
    _vent2Servo.write(vent2 * 180);
}

void Greenhouse::airPumpControl(bool on) {
    
}

void Greenhouse::humidityGoal(int percent) {
    
}

void Greenhouse::waterTemperatueGoal(int celcius) {
    
}

void Greenhouse::airTemperatureGoal(int celcius) {
    
}



void Greenhouse::humidifierControl(int pushCount) {
    for (int i = 0; i < pushCount; i++)
    {
        _humidifierServo.write(45);
        delay(100);
        _humidifierServo.write(0);
        delay(100);
    }
    
}

void Greenhouse::fanControl(bool on) {
    digitalWrite(FAN_PIN, on);
    
}

void Greenhouse::waterPumpControl(bool on) {
    digitalWrite(NUTRIENT_PUMP_PIN, on);
}

void Greenhouse::lightBulbControl(bool on) {
    
}



int Greenhouse::getHumidity() {
    return _humidity;
}

int Greenhouse::getAirTemp() {
    return _airTemperature;
}

int Greenhouse::getWaterTemp() {
    return _waterTemperature;
}

float Greenhouse::getPH() {
    return _pH;
}

int Greenhouse::getLightIntensity() {
    return _lightIntensity;
}

