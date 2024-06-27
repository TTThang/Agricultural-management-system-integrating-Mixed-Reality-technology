#include "Define.h"

extern int sleep_time;
extern int sleep_time_config[5];

typedef union {
  float float_num;
  uint8_t convert_num[4];
} float_to_u8;

float_to_u8 voltage;

uint8_t uplink_data[DATA_SIZE];

void get_data()
{   
    uint16_t solar_current = (uint16_t)random(0, 300);
    float solar_voltage = random(0,22000)/1000.0;
    int16_t battery_current = (int16_t)random(-100,100);
    float battery_voltage = random(3700,4150)/1000.0;
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
    long random_num = random(1700, 4300); 
    uplink_data[0] = (uint8_t)(random_num >> 8);
    uplink_data[1] = (uint8_t)(random_num);
    random_num = random(2400, 9600);
    uplink_data[2] = (uint8_t)(random_num >> 8);
    uplink_data[3] = (uint8_t)(random_num);
    random_num = random(1700, 5000); 
    uplink_data[4] = (uint8_t)(random_num >> 8);
    uplink_data[5] = (uint8_t)(random_num);
    random_num = random(2400, 9999);
    uplink_data[6] = (uint8_t)(random_num >> 8);
    uplink_data[7] = (uint8_t)(random_num);
    uplink_data[8] = (uint8_t)(random(0,255));
    uplink_data[9] = (uint8_t)(random(0,255));
    uplink_data[10] = (uint8_t)(random(5,10));
    uplink_data[11] = (uint8_t)(random(0,255));
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
