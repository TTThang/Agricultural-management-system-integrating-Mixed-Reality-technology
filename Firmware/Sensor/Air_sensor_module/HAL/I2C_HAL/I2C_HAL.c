/*
 * I2C_HAL.c
 *
 *  Created on: Mar 21, 2024
 *      Author: thang
 */

#include "I2C_HAL.h"

void I2C_HAL_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture );

    I2C_Cmd( I2C1, ENABLE );

    I2C_AcknowledgeConfig( I2C1, ENABLE );

}

i2c_state I2C_HAL_ping(uint8_t address){
    uint32_t time_out= I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY ) != RESET && (time_out--));
    if(time_out == -1) return I2C_BUSY;
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, address<<1, I2C_Direction_Transmitter);

    time_out= I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (time_out--));
    if(time_out == -1){
        I2C1->CTLR1 |= I2C_CTLR1_STOP;
        return I2C_NO_ACK;
    }
    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    return I2C_OK;
}

void I2C_HAL_scan(){
    printf("******I2C scanning******\r\n");
    int address;
    for(address = 0; address<128; address++){
        switch(I2C_HAL_ping(address)){
            case I2C_BUSY:
                printf("ERROR: I2C bus is busy!!!\r\n");
                return;
                break;
            case I2C_NO_ACK:
                printf(".");
                break;
            case I2C_OK:
                printf("\r\n0x%X\r\n", address);
                break;
            default:
                ;
        }
        Delay_Ms(100);
    }
    printf("\r\n**********Finish*********\r\n");
    return;
}

i2c_state I2C_HAL_master_write(uint8_t address, uint8_t *buffer, uint8_t size){
    uint32_t time_out= I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY ) != RESET && (time_out--));
    if(time_out == -1) return I2C_BUSY;

    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, address<<1, I2C_Direction_Transmitter);

    time_out= I2C_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (time_out--));
    if(time_out == -1){
        I2C1->CTLR1 |= I2C_CTLR1_STOP;
        return I2C_NO_ACK;
    }

    for(int i=0; i<size; i++){
        if(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE ) !=  RESET ){
            Delay_Ms(10);
            I2C_SendData(I2C1, *buffer++);
        }
    }

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    Delay_Ms(100);
    return I2C_OK;
}

i2c_state I2C_HAL_master_read(uint8_t address, uint8_t* out_buffer, uint8_t out_size, uint8_t* in_buffer, uint8_t in_size){
    uint32_t time_out = I2C_TIMEOUT;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY ) != RESET && (time_out--));
    if(time_out == -1) return I2C_BUSY;
    if(out_buffer && out_size){
        I2C_GenerateSTART(I2C1, ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
        I2C_Send7bitAddress(I2C1, address<<1, I2C_Direction_Transmitter);

        time_out= I2C_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (time_out--));
        if(time_out == -1){
            I2C1->CTLR1 |= I2C_CTLR1_STOP;
            return I2C_NO_ACK;
        }

        for(int i=0; i<out_size; i++){
            if(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE ) !=  RESET ){
                Delay_Ms(10);
                I2C_SendData(I2C1, *out_buffer++);
            }
        }

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    I2C1->CTLR1 |= I2C_CTLR1_ACK;
    time_out= I2C_TIMEOUT;
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, address<<1, I2C_Direction_Receiver);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && (time_out--));
    if(time_out == -1){
        I2C1->CTLR1 |= I2C_CTLR1_STOP;
        return I2C_NO_ACK;
    }
    Delay_Us(1000);

    while(in_size){
        if(in_size==1)  I2C1->CTLR1 &= ~I2C_CTLR1_ACK;
        if(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) != RESET){
            *in_buffer = I2C_ReceiveData(I2C1);
            in_buffer++;
            in_size--;
        }
    }

//    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    Delay_Ms(100);
    return I2C_OK;
}
