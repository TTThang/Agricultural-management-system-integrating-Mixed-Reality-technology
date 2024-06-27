/*
 * I2C_HAL.h
 *
 *  Created on: Mar 21, 2024
 *      Author: thang
 */

#ifndef I2C_HAL_I2C_HAL_H_
#define I2C_HAL_I2C_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "debug.h"
#include "stdint.h"

#define I2C_TIMEOUT 2000

typedef enum {
    I2C_OK = 0,
    I2C_BUSY,
    I2C_NO_ACK
} i2c_state;

void I2C_HAL_Init(u32 bound, u16 address);

i2c_state I2C_HAL_ping(u8 address);
void I2C_HAL_scan();

i2c_state I2C_HAL_master_write(uint8_t address, uint8_t *buffer, uint8_t size);
i2c_state I2C_HAL_master_read(uint8_t address, uint8_t* out_buffer, uint8_t out_size, uint8_t* in_buffer, uint8_t in_size);



#ifdef __cplusplus
}
#endif

#endif /* I2C_HAL_I2C_HAL_H_ */
