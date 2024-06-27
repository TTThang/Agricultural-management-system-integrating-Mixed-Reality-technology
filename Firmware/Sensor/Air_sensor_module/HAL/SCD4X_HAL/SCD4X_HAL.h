/*
 * SCD4X_HAL.h
 *
 *  Created on: Mar 23, 2024
 *      Author: thang
 */

#ifndef SCD4X_HAL_SCD4X_HAL_H_
#define SCD4X_HAL_SCD4X_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "I2C_HAL/I2C_HAL.h"
#include <stdbool.h>
#include <debug.h>
#include <math.h>

#define SCD4X_ADDR     0x62

#define FAHRENHEIT(celcius)         (((celcius * 9.0) / 5.0) + 32.0)
#define KELVIN(celcius)             (celcius + 273.15)
#define SCALE_CELCIUS               ('C')
#define SCALE_FAHRENHEIT            ('F')
#define SCALE_KELVIN                ('K')

#define SCD41_SENSOR_ADDR           0x62
#define SCD41_READ_ERROR            (0xFFFF)
#define SCD41_HEX_CODE_SIZE         (0x02)

#define CRC8_POLYNOMIAL             (0x31)
#define CRC8_INIT                   (0xFF)

typedef struct scd4x_msb_lsb {
    uint8_t msb;
    uint8_t lsb;
} scd4x_msb_lsb_t;

typedef struct scd4x_sensor_value {
    scd4x_msb_lsb_t value;
    uint8_t crc;
} scd4x_sensor_value_t;

typedef struct scd4x_sensors_values {
    uint16_t co2;
    float temperature;
    float humidity;
} scd4x_sensors_values_t;

void scd4x_init();

void scd4x_send_command(uint16_t command, uint16_t* data, uint8_t size);

void scd4x_read_resp(uint16_t *data, uint8_t size);

void execute_command(uint16_t command, uint32_t delay_time, uint16_t* out_data, uint8_t out_size, uint16_t* in_data, uint8_t in_size);

void scd4x_start_periodic_measurement();
void scd4x_read_measurement_ticks(uint16_t* co2, uint16_t* temperature, uint16_t* humidity);

void scd4x_read_measurement(uint16_t* co2, float* temperature, float* humidity);

void scd4x_stop_periodic_measurement();

void scd4x_get_temperature_offset_ticks(uint16_t* t_offset);
void scd4x_set_temperature_offset(float temperature);
void scd4x_set_temperature_offset_ticks(uint16_t t_offset);
void scd4x_get_temperature_offset(float* t_offset);

void scd4x_get_sensor_altitude(uint16_t* altitude);
void scd4x_set_sensor_altitude(uint16_t altitude);

void scd4x_set_ambient_ressure(uint16_t pressure);


void scd4x_perform_forced_recalibration(uint16_t target_co2_concentration, uint16_t *frc_correction);

void scd4x_get_automatic_self_calibration(bool* enable);

void scd4x_set_automatic_self_calibration(bool enable);

void scd4x_start_low_power_periodic_measurement();

void scd4x_get_data_ready_status(bool *data_ready);

void scd4x_persist_settings();

void scd4x_get_serial_number(uint16_t *serial0, uint16_t *serial1, uint16_t *serial2);

void scd4x_perform_self_test(bool *malfunction);

void scd4x_perfom_factory_reset();

void scd4x_reinit();

void scd4x_measure_single_shot();

void scd4x_measure_single_shot_rht_only();

void scd4x_power_down();

void scd4x_wake_up();


#ifdef __cplusplus
}
#endif

#endif /* SCD4X_HAL_SCD4X_HAL_H_ */
