/*
 * RS485_HAL.c
 *
 *  Created on: Mar 31, 2024
 *      Author: thang
 */

#include "RS485_HAL/RS485_HAL.h"

static RS485_Config* conf;

void RS485_Init(RS485_Config* conf_t){
    conf = conf_t;
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    switch(conf->UART_Map){
    case NoRemap:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);
        /* USART1 TX-->D.5   RX-->D.6 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
    case Remap1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_PartialRemap1_USART1, ENABLE);
        /* USART1 TX-->D.0   RX-->D.1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
    case Remap2:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_PartialRemap2_USART1, ENABLE);
        /* USART1 TX-->D.6   RX-->D.5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
    case Remap3:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_FullRemap_USART1, ENABLE);
        /* USART1 TX-->C.0   RX-->C.1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;
    default:
        ;
    }
    USART_DeInit(USART1);
    USART_InitStructure.USART_BaudRate = conf->UART_Baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);;

    USART_Cmd(USART1, ENABLE);

    if(conf->RE_Port==GPIOA || conf->DE_Port==GPIOA){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }
    if(conf->RE_Port==GPIOC || conf->DE_Port==GPIOC){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    if(conf->RE_Port==GPIOD || conf->DE_Port==GPIOD){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    }

    GPIO_InitStructure.GPIO_Pin = conf->RE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(conf->RE_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = conf->DE_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(conf->DE_Port, &GPIO_InitStructure);

    GPIO_WriteBit(conf->RE_Port, conf->RE_Pin, 0);
    GPIO_WriteBit(conf->DE_Port, conf->DE_Pin, 0);
}

void RS485_Change_Baudrate(RS485_Config* conf_t){
    conf = conf_t;
    USART_InitTypeDef USART_InitStructure = {0};

//    USART_DeInit(USART1);
    USART_InitStructure.USART_BaudRate = conf->UART_Baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void RS485_Send_Mode(void){
    GPIO_WriteBit(conf->RE_Port, conf->RE_Pin, 1);
    GPIO_WriteBit(conf->DE_Port, conf->DE_Pin, 1);
}

void RS485_Receive_Mode(void){
    GPIO_WriteBit(conf->RE_Port, conf->RE_Pin, 0);
    GPIO_WriteBit(conf->DE_Port, conf->DE_Pin, 0);
}

void RS485_Sleep_Mode(void){
    GPIO_WriteBit(conf->RE_Port, conf->RE_Pin, 1);
    GPIO_WriteBit(conf->DE_Port, conf->DE_Pin, 0);
}
void Send_RS485_Data(uint8_t* data, uint8_t size){
    RS485_Send_Mode();
//    Delay_Ms(500);
    uint8_t i = 0;
    while(size){
        Delay_Ms(10);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); /* waiting for sending finish */
        USART_SendData(USART1, data[i++]);
        size--;
    }
//    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    USART_SendData(USART1, '\r');
//    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//    USART_SendData(USART1, '\n');
    Delay_Ms(100);
    RS485_Receive_Mode();
    return;
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        conf->Rx_buffer[*(conf->Rx_size)] = USART_ReceiveData(USART1);
        (*(conf->Rx_size))++;
        USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
//        if(conf->Rx_buffer[*(conf->Rx_size)] == '\r' || conf->Rx_buffer[*(conf->Rx_size)] == '\0')
//        {
//            *(conf->Rxfinish_FLag) = 1;
//            conf->Rx_buffer[*(conf->Rx_size)] = '\0';
//        }
//        else{
//            (*(conf->Rx_size))++;
//        }
//        USART_ClearFlag(USART1, USART_IT_RXNE);
    }
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){
        USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
        *(conf->Rxfinish_FLag) = 1;
//        USART_ClearFlag(USART1, USART_IT_IDLE);
    }
}
