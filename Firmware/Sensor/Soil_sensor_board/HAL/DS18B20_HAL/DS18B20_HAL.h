/*
 * DS18B20_HAL.h
 *
 *  Created on: Mar 27, 2024
 *      Author: thang
 */

#ifndef DS18B20_HAL_DS18B20_HAL_H_
#define DS18B20_HAL_DS18B20_HAL_H_

#include "ONEWIRE_HAL/ONEWIRE_HAL.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef onewire_addr_t ds18x20_addr_t;

/** An address value which can be used to indicate "any device on the bus" */
#define DS18X20_ANY ONEWIRE_NONE

/** Family ID (lower address byte) of sensors */
typedef enum {
    DS18X20_FAMILY_DS18S20  = 0x10, //!< DS1820/DS18S20  9-bit  +/-0.5¡ãC
    DS18X20_FAMILY_DS1822   = 0x22, //!< DS1822          12-bit +/-2¡ãC
    DS18X20_FAMILY_DS18B20  = 0x28, //!< DS18B20         12-bit +/-0.5¡ãC
    DS18X20_FAMILY_MAX31850 = 0x3b, //!< MAX31850        14-bit +/-0.25¡ãC
} ds18x20_family_id_t;

void ds18x20_init(GPIO_TypeDef* port, uint16_t pin);

onewire_state ds18x20_scan_devices(onewire_addr_t *addr_list, size_t addr_count, size_t *found);

onewire_state ds18x20_measure(onewire_addr_t addr, bool wait);

onewire_state ds18x20_read_temperature(onewire_addr_t addr, float *temperature);

onewire_state ds18b20_read_temperature(onewire_addr_t addr, float *temperature);

onewire_state ds18s20_read_temperature(onewire_addr_t addr, float *temperature);

onewire_state max31850_read_temperature(onewire_addr_t addr, float *temperature);

onewire_state ds18x20_read_temp_multi(onewire_addr_t *addr_list, size_t addr_count, float *result_list);

onewire_state ds18x20_measure_and_read(onewire_addr_t addr, float *temperature);

onewire_state ds18s20_measure_and_read(onewire_addr_t addr, float *temperature);

onewire_state ds18b20_measure_and_read(onewire_addr_t addr, float *temperature);

onewire_state max31850_measure_and_read(onewire_addr_t addr, float *temperature);

onewire_state ds18x20_measure_and_read_multi(onewire_addr_t *addr_list, size_t addr_count, float *result_list);

onewire_state ds18x20_read_scratchpad(onewire_addr_t addr, uint8_t *buffer);

onewire_state ds18x20_write_scratchpad(onewire_addr_t addr, uint8_t *buffer);

onewire_state ds18x20_copy_scratchpad(onewire_addr_t addr);

#ifdef __cplusplus
}
#endif



#endif /* DS18B20_HAL_DS18B20_HAL_H_ */
