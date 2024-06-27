/*
 * ONEWIRE_HAL.h
 *
 *  Created on: Mar 27, 2024
 *      Author: thang
 */

#ifndef ONEWIRE_HAL_ONEWIRE_HAL_H_
#define ONEWIRE_HAL_ONEWIRE_HAL_H_

#include <debug.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t onewire_addr_t;

typedef struct
{
    uint8_t rom_no[8];
    uint8_t last_discrepancy;
    bool last_device_found;
} onewire_search_t;

typedef struct{
    GPIO_TypeDef* port;
    uint16_t pin;
} onewire_config_t;

typedef enum{
    OK,
    INVALID_RESPONSE,
    INVALID_CRC,
    DEVICE_NOT_SUPPORTED,
} onewire_state;

#define ONEWIRE_NONE ((onewire_addr_t)(0xffffffffffffffffLL))

void onewire_init(GPIO_TypeDef* GPIOx, uint16_t pin);

bool onewire_reset();

bool onewire_select(const onewire_addr_t addr);

bool onewire_skip_rom();

bool onewire_write(uint8_t v);

bool onewire_write_bytes(const uint8_t *buf, size_t count);

int onewire_read();

bool onewire_read_bytes(uint8_t *buf, size_t count);

bool onewire_power();

void onewire_depower();

void onewire_search_start(onewire_search_t *search);

void onewire_search_prefix(onewire_search_t *search, uint8_t family_code);

onewire_addr_t onewire_search_internal(onewire_search_t *search, uint8_t command);

onewire_addr_t onewire_search(onewire_search_t *search);

onewire_addr_t search_alarms(onewire_search_t *search);

uint8_t onewire_crc8(const uint8_t *data, uint8_t len);

bool onewire_check_crc16(const uint8_t* input, size_t len, const uint8_t* inverted_crc, uint16_t crc_iv);

uint16_t onewire_crc16(const uint8_t* input, size_t len, uint16_t crc_iv);

#ifdef __cplusplus
}
#endif

#endif /* ONEWIRE_HAL_ONEWIRE_HAL_H_ */
