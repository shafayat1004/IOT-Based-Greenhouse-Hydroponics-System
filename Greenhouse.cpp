#include <Arduino.h>
#include <Greenhouse.h>
#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <math.h> 
#include <BH1750FVI.h>
#include <ArduinoJson.h>

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
    _nutrientTemp = DallasTemperature(&_oneWire);
    _nutrientTemp.begin();

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

void Greenhouse::testAllSystems(HardwareSerial &espSerial = Serial3){
    
    Serial.println("Water Pump On");
    waterPumpControl(1);
    delay(3000);
    Serial.println("Water Pump Off");
    waterPumpControl(0);
    
    Serial.println();
    delay(2000);

    Serial.println("Fan On");
    fanControl(1);
    delay(3000);
    Serial.println("Fan Off");
    fanControl(0);
    
    Serial.println();
    delay(2000);

    Serial.println("Water Heater On");
    waterHeaterControl(1);
    delay(3000);
    Serial.println("Water Heater Off");
    waterHeaterControl(0);
    
    Serial.println();
    delay(2000);

    // Serial.println("Light Bulb On");
    // lightBulbControl(1);
    // delay(3000);
    // Serial.println("Light Bulb Off");
    // lightBulbControl(0);
    //    
    // Serial.println();
    // delay(2000);

    Serial.println("Exhaust Vent Off");
    Serial.println("Intake Vent Off");
    ventControl(0,0);
    delay(2000);
    Serial.println("Exhaust Vent Off");
    Serial.println("Intake Vent On");
    ventControl(0,1);
    delay(2000);
    Serial.println("Exhaust Vent On");
    Serial.println("Intake Vent Off");
    ventControl(1,0);
    delay(2000);
    Serial.println("Exhaust Vent On");
    Serial.println("Intake Vent On");
    ventControl(1,1);
    
    Serial.println();
    delay(2000);

    for (int i = 0; i < 10; i++){
        Serial.println("----------------------------------------");
        Serial.print(  "------------ Sensor Test: ");
        Serial.print(i);
        Serial.println(" ------------");
        refreshSensorData();
        printSensorData();
        Serial.println("----------------------------------------");
        delay(2000);
    }

    for (int i = 0; i < 8; i++){
        Serial.println("----------------------------------------");
        Serial.print(  "----- Fetching Config Data Test: ");
        Serial.print(i);
        Serial.println(" -----");
        setCurrentConfig(espSerial);
        printCurrentConfig();
        Serial.println("----------------------------------------");
        delay(2000);
    }

    Serial.println("Uploading Sensor Data");
    uploadSensorData(espSerial);
        

}

void Greenhouse::setCurrentConfig(HardwareSerial &espSerial = Serial3){
    espSerial.print('V');
    while(espSerial.read() != 'R') delay(100);
    deserializeJson(_currentConfig, espSerial);
    _airTemperatureGoal      = _currentConfig["airTemp"];
    _humidityGoal            = _currentConfig["humidity"];
    _lightDurationGoal       = _currentConfig["lightDuration"];
    _lightIntensityGoal      = _currentConfig["lux"];
    _nutrientPumpFrequency   = _currentConfig["nutrientPumpFrequency"];
    _nutrientReplacementTime = _currentConfig["nutrientReplacementTime"];
    _nutrientTemperatureGoal = _currentConfig["nutrientTemp"];
    _initialNutrientVolume   = _currentConfig["nutrientVol"];
    _pHGoal                  = _currentConfig["pH"];
    espSerial.flush();
}

void Greenhouse::updateTime(HardwareSerial &espSerial = Serial3){
    espSerial.print('T');
    while(espSerial.read() != 'R') delay(100);
    deserializeJson(_currentTime, espSerial);
    int _Day = _currentTime["D"]; // 0
    int _Hour = _currentTime["H"]; // 12
    int _Minute = _currentTime["M"]; // 12
    int _Second = _currentTime["S"];
    
}

void Greenhouse::getSunsetSunrise(HardwareSerial &espSerial = Serial3){
    espSerial.print('S');
    while(espSerial.read() != 'R') delay(100);
    deserializeJson(_sunsetSunrise, espSerial);
    int _sunsetTime = _sunsetSunrise["sunrise"];  // 0
    int _sunriseTime = _sunsetSunrise["sunrise"]; // 12 
}
void Greenhouse::printSunsetSunrise(){
    Serial.print("Sunrise");
    Serial.println(_sunriseTime);
    Serial.print("Sunset");
    Serial.println(_sunsetTime);
}

void Greenhouse::printTime(HardwareSerial &espSerial = Serial3){
    updateTime(espSerial);
    Serial.println("Current Time");
    Serial.print(_Day);
    Serial.print(":");
    Serial.print(_Minute);
    Serial.print(":");
    Serial.println(_Second);
    espSerial.flush();
}

void Greenhouse::printCurrentConfig(){
    Serial.println("#########################################");
    Serial.println("Current Configuration");
    Serial.println("#########################################");

    Serial.print("airTemp : ");
    Serial.println(_airTemperatureGoal);
    Serial.print("humidity : ");
    Serial.println(_humidityGoal);
    Serial.print("lightDuration : ");
    Serial.println(_lightDurationGoal);
    Serial.print("lux : ");
    Serial.println(_lightIntensityGoal);
    Serial.print("nutrientPumpFrequency : ");
    Serial.println(_nutrientPumpFrequency);
    Serial.print("nutrientReplacementTime : ");
    Serial.println(_nutrientReplacementTime);
    Serial.print("nutrientTemp : ");
    Serial.println(_nutrientTemperatureGoal);
    Serial.print("nutrientVol : ");
    Serial.println(_initialNutrientVolume);
    Serial.print("pH : ");
    Serial.println(_pHGoal);
}

void Greenhouse::printSensorData(String select = "All"){
    Serial.println("#########################################");
    Serial.println("Sensor Data");
    Serial.println("#########################################");

    if (select == "All" || select == "airTemp"){
        Serial.print("airTemp : ");
        Serial.println(_airTemperature);
    }
    if (select == "All" || select == "humidity"){        
        Serial.print("humidity : ");
        Serial.println(_humidity);
    }
    if (select == "All" || select == "lux"){
        Serial.print("lux : ");
        Serial.println(_lightIntensity);
    }
    if (select == "All" || select == "nutrientTemp"){
        Serial.print("nutrientTemp : ");
        Serial.println(_nutrientTemperature);
    }
    if (select == "All" || select == "pH"){
        Serial.print("pH : ");
        Serial.println(_pH);
    }
}

void Greenhouse::refreshSensorData() {
 
    _DHT.read(DHT_PIN);
    _humidity       = _DHT.humidity;
    _airTemperature = _DHT.temperature;
    while (_humidity == 0 || _humidity == 255 ||_airTemperature == 0 || _airTemperature == 255 ){
        _DHT.read(DHT_PIN);
        _humidity       = _DHT.humidity;
        _airTemperature = _DHT.temperature;
        delay(500);
    }


    _nutrientTemp.requestTemperatures();
    _nutrientTemperature = _nutrientTemp.getTempCByIndex(0);
    _pH               = refreshPHData();
    _lightIntensity   = _myLux.getLux();

    _sensorData["airTemp"] = _airTemperature;
    _sensorData["humidity"] = _humidity;
    _sensorData["lux"] = _lightIntensity;
    _sensorData["nutrientTemp"] = _nutrientTemperature;
    _sensorData["pH"] = _pH;   
}

void Greenhouse::uploadSensorData(HardwareSerial &espSerial = Serial3){
    espSerial.print('U');
    serializeJson(_sensorData, espSerial);
    espSerial.flush();
}

float Greenhouse::refreshPHData(float calibration_value){
    for (int i = 0; i < 10; i++){
        _buffer_arr[i] = analogRead(A0);
        delay(40);
    }
    for (int i = 0; i < 9; i++){
        for (int j = i + 1; j < 10; j++){
            if (_buffer_arr[i] > _buffer_arr[j]){
                temp = _buffer_arr[i];
                _buffer_arr[i] = _buffer_arr[j];
                _buffer_arr[j] = temp;
            }
        }
    }
    _avgvalmV = 0;
    for (int i = 2; i < 8; i++) _avgvalmV += _buffer_arr[i];

    // float volt=(float)_avgvalmV*5.0/1024/6; 
    // ph_act = -5.70 * volt + calibration_value;
    float millivolt = (float)_avgvalmV / 6;
    return ((1024-millivolt)/1024) * 14 + calibration_value;
}

void Greenhouse::setNutrientPumpFrequency(){

}

void Greenhouse::maintainConfig(int delayTime, int tempHumiditySlack, float pHSlack){
    humidityAdjust(tempHumiditySlack);
    delay(10000);
    Serial.println("Adjusting Nutrient Temperature .....");
    waterTemperatureAdjust(tempHumiditySlack);
    delay(10000);
    Serial.println("Adjusting Air Temperature .....");
    airTemperatureAdjust(tempHumiditySlack);
    delay(10000);
    // pHAdjust(pHSlack);
    // delay(10000);

}





void Greenhouse::ventControl(int vent1, int vent2) {
    if (vent1 == 1 || vent1 == 0) _vent1Servo.write(90*vent1);
    if (vent2 == 1 || vent2 == 0) _vent2Servo.write(90-90*vent2);
}

void Greenhouse::humidifierControl(int pushCount) {
    for (int i = 0; i < pushCount; i++){
        _humidifierServo.write(45);
        delay(100);
        _humidifierServo.write(0);
        delay(100);
    }
}

void Greenhouse::waterHeaterControl(bool on){
    digitalWrite(NUTRIENT_HEATER_PIN, on);
}

void Greenhouse::fanControl(bool on) {
    digitalWrite(FAN_PIN, on);   
}

void Greenhouse::waterPumpControl(bool on) {
    digitalWrite(NUTRIENT_PUMP_PIN, on);
}

void Greenhouse::lightBulbControl(bool on) {
    digitalWrite(LIGHT_BULB_PIN, on);
}

int Greenhouse::getHumidity() {
    return _humidity;
}

int Greenhouse::getAirTemp() {
    return _airTemperature;
}

int Greenhouse::getWaterTemp() {
    return _nutrientTemperature;
}

float Greenhouse::getPH() {
    return _pH;
}

int Greenhouse::getLightIntensity() {
    return _lightIntensity;
}

// void Greenhouse::airPumpControl(bool on) {
    
// }

void Greenhouse::increaseHumidity(){
    humidifierControl(20);
    waterHeaterControl(1);
    fanControl(0);
    ventControl(0,0);
    lightBulbControl(0);
}

void Greenhouse::decreaseHumidity(){
    waterHeaterControl(1);
    fanControl(1);
    ventControl(1,0);
    lightBulbControl(1);
}


int Greenhouse::humidityAdjust(int slack = 2) {
    if(_humidity<_humidityGoal-slack){
        Serial.println("Increasing Humidity");
        increaseHumidity();
        return 1;
    }
    else if(_humidity>_humidityGoal+slack){
        Serial.println("Decreasing Humidity");
        decreaseHumidity();  
        return -1;
    }
    return 0;
}


void Greenhouse::increaseWaterTemperature(){
    waterHeaterControl(1);
}
void Greenhouse::decreaseWaterTemperature(){
    waterHeaterControl(0);
}
int Greenhouse::waterTemperatureAdjust(int slack = 2) {
    if(_nutrientTemperature < _nutrientTemperatureGoal-slack){
        Serial.println("Increasing Nutrient Temperature");
        increaseWaterTemperature();
        return 1;
    }
    else if(_nutrientTemperature > _nutrientTemperatureGoal+slack){
        Serial.println("Decreasing Nutrient Temperature");
        decreaseWaterTemperature();  
        return -1;
    }
    return 0;
}


void Greenhouse::increaseAirTemperature(){
    lightBulbControl(1);
    fanControl(0);
    ventControl(0,0);
}
void Greenhouse::decreaseAirTemperature(){
    lightBulbControl(0);
    fanControl(1);
    ventControl(1,1);
}
int Greenhouse::airTemperatureAdjust(int slack = 2) {
    if(_airTemperature < _airTemperatureGoal-slack){
        Serial.println("Increasing Air Temperature");
        increaseAirTemperature();
        return 1;
    }
    else if(_airTemperature > _airTemperatureGoal+slack){
        Serial.println("Decreasing Air Temperature");
        decreaseAirTemperature();  
        return -1;
    }
    return 0;
}

void Greenhouse::increasePH(){
    
}
void Greenhouse::decreasePH(){
    
}
int Greenhouse::pHAdjust(int slack = 0.3){
    if(_pH < _pHGoal-slack){
        increasePH();
        return 1;
    }
    else if(_pH > _pHGoal+slack){
        decreasePH();  
        return -1;
    }
    return 0;
}


