/*
 * BH1750_HAL.c
 *
 *  Created on: Mar 22, 2024
 *      Author: thang
 */

#include <BH1750_HAL/BH1750_HAL.h>

#define OPCODE_HIGH  0x0
#define OPCODE_HIGH2 0x1
#define OPCODE_LOW   0x3

#define OPCODE_CONT 0x10
#define OPCODE_OT   0x20

#define OPCODE_POWER_DOWN 0x00
#define OPCODE_POWER_ON   0x01
#define OPCODE_MT_HI      0x40
#define OPCODE_MT_LO      0x60

static uint8_t bh1750_addr;
//static uint8_t mode;
static uint8_t state;
void bh1750_init( bool addr){
    if(addr == 0){
        bh1750_addr = BH1750_ADDR_LO;
    }
    else{
        bh1750_addr = BH1750_ADDR_HI;
    }
}

void bh1750_power_down(){
    state = OPCODE_POWER_DOWN;
    I2C_HAL_master_write(bh1750_addr, &state, sizeof(state));
}

void bh1750_power_on(){
    state = OPCODE_POWER_ON;
    I2C_HAL_master_write(bh1750_addr, &state, sizeof(state));
}

void bh1750_setup(bh1750_mode_t mode, bh1750_resolution_t resolution){
    uint8_t opcode = mode == BH1750_MODE_CONTINUOUS ? OPCODE_CONT : OPCODE_OT;
    switch (resolution)
    {
        case BH1750_RES_LOW:  opcode |= OPCODE_LOW;   break;
        case BH1750_RES_HIGH: opcode |= OPCODE_HIGH;  break;
        default:              opcode |= OPCODE_HIGH2; break;
    }
    I2C_HAL_master_write(bh1750_addr, &opcode, sizeof(opcode));
    Delay_Ms(10);
}

void bh1750_set_measurement_time(uint8_t time){
    uint8_t buf = OPCODE_MT_HI | (time >> 5);
    I2C_HAL_master_write(bh1750_addr, &buf, sizeof(buf));
    buf = OPCODE_MT_LO | (time & 0x1f);
    I2C_HAL_master_write(bh1750_addr, &buf, sizeof(buf));
//    I2C_HAL_master_write(bh1750_addr, &mode, sizeof(mode));
}

void bh1750_read(uint16_t *level){
    uint8_t buf[2];
    I2C_HAL_master_read(bh1750_addr, NULL, 0, buf, 2);
    *level = buf[0] << 8 | buf[1];
    *level = (*level * 10) / 12;
}
