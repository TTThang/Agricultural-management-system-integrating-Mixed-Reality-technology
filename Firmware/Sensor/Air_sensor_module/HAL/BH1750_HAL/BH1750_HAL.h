/*
 * BH1750_HAL.h
 *
 *  Created on: Mar 22, 2024
 *      Author: thang
 */

#ifndef BH1750_HAL_BH1750_HAL_H_
#define BH1750_HAL_BH1750_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "I2C_HAL/I2C_HAL.h"
#include "stdbool.h"

#define BH1750_ADDR_LO 0x23 //!< I2C address when ADDR pin floating/low
#define BH1750_ADDR_HI 0x5c //!< I2C address when ADDR pin high

/**
 * Measurement mode
 */
typedef enum
{
    BH1750_MODE_ONE_TIME = 0, //!< One time measurement
    BH1750_MODE_CONTINUOUS    //!< Continuous measurement
} bh1750_mode_t;

/**
 * Measurement resolution
 */
typedef enum
{
    BH1750_RES_LOW = 0,  //!< 4 lx resolution, measurement time is usually 16 ms
    BH1750_RES_HIGH,     //!< 1 lx resolution, measurement time is usually 120 ms
    BH1750_RES_HIGH2     //!< 0.5 lx resolution, measurement time is usually 120 ms
} bh1750_resolution_t;

/**
 * @brief Initialize device descriptor
 *
 * @param[out] dev Device descriptorc
 * @param[in] addr I2C address, ::BH1750_ADDR_LO or ::BH1750_ADDR_HI
 * @param[in] port I2C port number
*/

void bh1750_init(bool addr);

/**
 * @brief Power down device
*/

void bh1750_power_down();

/**
 * @brief Power on device
*/

void bh1750_power_on();

/**
 * @brief Setup device parameters
 *
 * @param mode Measurement mode
 * @param resolution Measurement resolution
*/
void bh1750_setup(bh1750_mode_t mode, bh1750_resolution_t resolution);

/**
 * @brief Set measurement time
 *
 * @param time Measurement time (see datasheet)
*/
void bh1750_set_measurement_time(uint8_t time);

/**
 * @brief Read LUX value from the device.
 *
 * @param[out] level read value in lux units
*/
void bh1750_read(uint16_t *level);


#ifdef __cplusplus
}
#endif





#endif /* BH1750_HAL_BH1750_HAL_H_ */
