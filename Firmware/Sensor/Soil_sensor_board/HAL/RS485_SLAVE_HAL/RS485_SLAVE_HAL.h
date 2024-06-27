/*
 * RS485_SLAVE_HAL.h
 *
 *  Created on: Apr 3, 2024
 *      Author: thang
 */

#ifndef RS485_SLAVE_HAL_RS485_SLAVE_HAL_H_
#define RS485_SLAVE_HAL_RS485_SLAVE_HAL_H_

#include "RS485_HAL/RS485_HAL.h"

typedef enum{
    rtu_Read_Coils = 0x01,
    rtu_Read_Discrete_Inputs= 0x02,
    rtu_Read_Holding_Registers = 0x03,
    rtu_Read_Input_Registers = 0x04,
    rtu_Write_Single_Coil = 0x05,
    rtu_Write_Single_Register = 0x06,
    rtu_Diagnostics = 0x08,                     //(Serial_Line_only)
    rtu_Get_Comm_Event_Counter = 0x0B,          //(Serial_Line_only)
    rtu_Write_Multiple_Coils = 0x0F,
    rtu_Write_Multiple_Registers = 0x10,
    rtu_Report_Server_ID = 0x11,                //(Serial_Line_only)
    rtu_Mask_Write_Register = 0x16,
    rtu_Read_Multiple_Registers = 0x17,
    rtu_Read_Device_Identification = 0x2B
} RTU_Function_Code;

//void RS485_Slave_Init(RS485_Config* conf, uint8_t address);

void Generate_CRC(uint8_t* data, uint8_t size, uint8_t* crc_buff);

bool CRC_Check(uint8_t* data, uint8_t size);

void RS485_Slave_Respone(uint8_t address, uint8_t functionCode, uint8_t* data, uint8_t data_size);


#endif /* RS485_SLAVE_HAL_RS485_SLAVE_HAL_H_ */
