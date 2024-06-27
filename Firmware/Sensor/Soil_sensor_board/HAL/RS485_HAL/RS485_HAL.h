/*
 * RS485_HAL.h
 *
 *  Created on: Mar 31, 2024
 *      Author: thang
 */

#ifndef RS485_HAL_RS485_HAL_H_
#define RS485_HAL_RS485_HAL_H_

#include "debug.h"
#include "stdbool.h"

typedef enum{
    NoRemap=DEBUG_UART1_NoRemap,
    Remap1=DEBUG_UART1_Remap1,
    Remap2=DEBUG_UART1_Remap2,
    Remap3=DEBUG_UART1_Remap3
} UART_Mapping;

typedef struct{
    UART_Mapping UART_Map;
    GPIO_TypeDef* RE_Port;
    uint16_t RE_Pin;
    GPIO_TypeDef* DE_Port;
    uint16_t DE_Pin;
    uint32_t UART_Baudrate;
    volatile bool* Rxfinish_FLag;
    volatile u8* Rx_buffer;
    volatile u8* Rx_size;
} RS485_Config;

void RS485_Init(RS485_Config* conf_t);
void RS485_Change_Baudrate(RS485_Config* conf_t);

void RS485_Send_Mode(void);
void RS485_Receive_Mode(void);
void RS485_Sleep_Mode(void);

void Send_RS485_Data(uint8_t* data, uint8_t size);

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


#endif /* RS485_HAL_RS485_HAL_H_ */
