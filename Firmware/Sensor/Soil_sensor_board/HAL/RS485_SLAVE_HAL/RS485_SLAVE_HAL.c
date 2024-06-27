/*
 * RS485_SLAVE_HAL.c
 *
 *  Created on: Apr 3, 2024
 *      Author: thang
 */

#include "RS485_SLAVE_HAL.h"

uint8_t crc[2];

void RS485_Slave_Respone(uint8_t address, uint8_t functionCode, uint8_t* data, uint8_t data_size){
    uint8_t data_buff[data_size+4];
    data_buff[0] = address;
    data_buff[1] = functionCode;
    for(int8_t i = 0; i<data_size; i++){
        data_buff[2+i] = data[i];
    }
    Generate_CRC(data_buff, data_size+2, crc);
    data_buff[2+data_size] = crc[0];
    data_buff[2+data_size+1]= crc[1];
    Send_RS485_Data(data_buff, data_size+4);
//    RS485_Send_Mode();
//    printf("%s\r\n", data_buff);
//    RS485_Receive_Mode();
}

static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}

void Generate_CRC(uint8_t* data, uint8_t size, uint8_t* crc_buff){
    u16 u16CRC = 0xFFFF;
    for (uint8_t i = 0; i < size; i++){
        u16CRC = crc16_update(u16CRC, data[i]);
    }
    crc_buff[1] = (uint8_t)(u16CRC>>8);
    crc_buff[0] = (uint8_t)u16CRC;
}

bool CRC_Check(uint8_t* data, uint8_t size){
    uint8_t crc_temp[2];
    Generate_CRC(data, size, crc_temp);
    if((crc_temp[0] != data[size]) && crc_temp[1] != data[size+1]){
        return false;
    }
    return true;
}
