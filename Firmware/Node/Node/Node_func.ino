#include <Wire.h>
#include <INA3221.h>
#include <ModbusMaster.h>
// #include <ArduinoJson.h>
#include "Define.h"

ModbusMaster air_sensor;
ModbusMaster soil_sensor;

extern int sleep_time;
extern int sleep_time_config[5];

INA3221 ina_0(INA3221_ADDR40_GND);

typedef union {
  float float_num;
  uint8_t convert_num[4];
} float_to_u8;

float_to_u8 voltage;

uint8_t uplink_data[DATA_SIZE];
// JsonDocument sensor_data;
// String uplink_data;

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void RS485_Power_Off(){
  digitalWrite(RS485_PW_EN, 0);
  digitalWrite(EN_12V, 0);
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 0);
}

void RS485_Power_On(){
  digitalWrite(EN_12V, 1);
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
  delay(100);
  digitalWrite(RS485_PW_EN, 1);
}

void hardware_setup()
{
    pinMode(EN_12V, OUTPUT);
    pinMode(RS485_PW_EN, OUTPUT);
    pinMode(MAX485_RE_NEG, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    
    RS485_Power_Off();
    postTransmission();

    ina_0.begin(&Wire);
    ina_0.reset();
    ina_0.setChannelDisable(INA3221_CH2);
    ina_0.setAveragingMode(INA3221_REG_CONF_AVG_1024);
    ina_0.setModePowerDown();
    ina_0.setShuntRes(100, 0, 50);
    
    Serial1.begin(9600, RAK_CUSTOM_MODE);
    Serial1.printf(".......................................");
    delay(2000);

    air_sensor.begin(18, Serial1);
    air_sensor.preTransmission(preTransmission);
    air_sensor.postTransmission(postTransmission);

    soil_sensor.begin(17, Serial1);
    soil_sensor.preTransmission(preTransmission);
    soil_sensor.postTransmission(postTransmission);
}

void get_data()
{   
    RS485_Power_On();
    delay(1000);
    ina_0.setBusMeasEnable();
    ina_0.setModeTriggered();
    delay(5);
    uint16_t solar_current = (uint16_t)(ina_0.getCurrent(INA3221_CH1) * 1000);
    float solar_voltage = ina_0.getVoltage(INA3221_CH1);
    int16_t battery_current = (int16_t)(ina_0.getCurrent(INA3221_CH3) * (-1000));
    float battery_voltage = ina_0.getVoltage(INA3221_CH3);
    ina_0.setModePowerDown();
    bool stable_charging = (solar_current > 100);
    if(battery_voltage > 4.0){
      if(stable_charging){
        sleep_time = sleep_time_config[0];
      }
      else{
        sleep_time = sleep_time_config[1];
      }
    }
    else if(battery_voltage > 3.7){
      if(stable_charging){
        sleep_time = sleep_time_config[2];
      }
      else{
        sleep_time = sleep_time_config[3];
      }
    }
    else{
      sleep_time = sleep_time_config[4];
    }
    Serial.printf(
      "Solar %dmA %1.3fV Battery %dmA %1.3fV\r\n",
      solar_current, solar_voltage,
      battery_current, battery_voltage);
    delay(3000);
    uint8_t result = soil_sensor.readInputRegisters(0x0000, 2);
    Serial.println("Soil Requested");

    if (result == soil_sensor.ku8MBSuccess) {
      // Get response data from sensor
      Serial.print("Soil temperature: ");
      Serial.print((int)(float(soil_sensor.getResponseBuffer(0))+0.5) / 100.0);
      Serial.print("   Soil humidity: ");
      Serial.println((int)(float(soil_sensor.getResponseBuffer(1))+0.5) / 100.0);
    }
    else{
      Serial.print("Error code: ");
      Serial.println(result); 
    }
    // sensor_data["soil"]["temp"] = (int)(float(soil_sensor.getResponseBuffer(0))+0.5) / 100.0;
    // sensor_data["soil"]["humi"] = (int)(float(soil_sensor.getResponseBuffer(1))+0.5) / 100.0;

    delay(1000);

    result = air_sensor.readInputRegisters(0x0000, 4);
    Serial.println("Air Requested");

    if (result == air_sensor.ku8MBSuccess) {
      // Get response data from sensor
      Serial.print("Air temperature: ");
      Serial.print((int)(float(air_sensor.getResponseBuffer(0))+0.5) / 100.0);
      Serial.print("   Air humidity: ");
      Serial.println((int)(float(air_sensor.getResponseBuffer(1))+0.5) / 100.0);
      Serial.print("Light: ");
      Serial.print(air_sensor.getResponseBuffer(2));
      Serial.print("   CO2: ");
      Serial.println(air_sensor.getResponseBuffer(3));
    }
    else{
      Serial.print("Error code: ");
      Serial.println(result); 
    }
    // sensor_data["air"]["temp"] = (int)(float(air_sensor.getResponseBuffer(0))+0.5) / 100.0;
    // sensor_data["air"]["humi"] = (int)(float(air_sensor.getResponseBuffer(1))+0.5) / 100.0;
    // sensor_data["air"]["light"] = air_sensor.getResponseBuffer(2);
    // sensor_data["air"]["CO2"] = air_sensor.getResponseBuffer(3);
    // serializeJson(sensor_data, Serial);
    // serializeJson(sensor_data, uplink_data);
    // Serial.println(uplink_data);
    RS485_Power_Off();
    uplink_data[0] = (uint8_t)(soil_sensor.getResponseBuffer(0)>>8);
    uplink_data[1] = (uint8_t)(soil_sensor.getResponseBuffer(0));
    uplink_data[2] = (uint8_t)(soil_sensor.getResponseBuffer(1)>>8);
    uplink_data[3] = (uint8_t)(soil_sensor.getResponseBuffer(1));
    uplink_data[4] = (uint8_t)(air_sensor.getResponseBuffer(0)>>8);
    uplink_data[5] = (uint8_t)(air_sensor.getResponseBuffer(0));
    uplink_data[6] = (uint8_t)(air_sensor.getResponseBuffer(1)>>8);
    uplink_data[7] = (uint8_t)(air_sensor.getResponseBuffer(1));
    uplink_data[8] = (uint8_t)(air_sensor.getResponseBuffer(2)>>8);
    uplink_data[9] = (uint8_t)(air_sensor.getResponseBuffer(2));
    uplink_data[10] = (uint8_t)(air_sensor.getResponseBuffer(3)>>8);
    uplink_data[11] = (uint8_t)(air_sensor.getResponseBuffer(0));
    uplink_data[12] = (uint8_t)(solar_current>>8);
    uplink_data[13] = (uint8_t)(solar_current);
    voltage.float_num = solar_voltage;
    uplink_data[14] = voltage.convert_num[3];
    uplink_data[15] = voltage.convert_num[2];
    uplink_data[16] = voltage.convert_num[1];
    uplink_data[17] = voltage.convert_num[0];
    uplink_data[18] = (uint8_t)(battery_current>>8);
    uplink_data[19] = (uint8_t)(battery_current);
    voltage.float_num = battery_voltage;
    uplink_data[20] = voltage.convert_num[3];
    uplink_data[21] = voltage.convert_num[2];
    uplink_data[22] = voltage.convert_num[1];
    uplink_data[23] = voltage.convert_num[0];
    uplink_data[24] = (uint8_t) sleep_time;
}
