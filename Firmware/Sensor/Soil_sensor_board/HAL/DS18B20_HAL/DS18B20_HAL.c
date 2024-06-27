/*
 * DS18B20_HAL.c
 *
 *  Created on: Mar 27, 2024
 *      Author: thang
 */

#include "DS18B20_HAL.h"

#define ds18x20_WRITE_SCRATCHPAD 0x4E
#define ds18x20_READ_SCRATCHPAD  0xBE
#define ds18x20_COPY_SCRATCHPAD  0x48
#define ds18x20_READ_EEPROM      0xB8
#define ds18x20_READ_PWRSUPPLY   0xB4
#define ds18x20_SEARCHROM        0xF0
#define ds18x20_SKIP_ROM         0xCC
#define ds18x20_READROM          0x33
#define ds18x20_MATCHROM         0x55
#define ds18x20_ALARMSEARCH      0xEC
#define ds18x20_CONVERT_T        0x44


void ds18x20_init(GPIO_TypeDef* port, uint16_t pin){
    onewire_init(port, pin);
}

onewire_state ds18x20_scan_devices(onewire_addr_t *addr_list, size_t addr_count, size_t *found){
    onewire_search_t search;
    onewire_addr_t addr;

    *found = 0;
    onewire_search_start(&search);
    while ((addr = onewire_search(&search)) != ONEWIRE_NONE)
    {
        uint8_t family_id = (uint8_t)addr;
        if (family_id == DS18X20_FAMILY_DS18S20
            || family_id == DS18X20_FAMILY_DS1822
            || family_id == DS18X20_FAMILY_DS18B20
            || family_id == DS18X20_FAMILY_MAX31850)
        {
            if (*found < addr_count)
                addr_list[*found] = addr;
            *found += 1;
        }
    }
    return OK;
}

onewire_state ds18x20_measure(onewire_addr_t addr, bool wait){
    if (!onewire_reset())
        return INVALID_RESPONSE;

    if (addr == DS18X20_ANY)
        onewire_skip_rom();
    else
        onewire_select(addr);

//    PORT_ENTER_CRITICAL;
    onewire_write(ds18x20_CONVERT_T);
    // For parasitic devices, power must be applied within 10us after issuing
    // the convert command.
    onewire_power();
//    PORT_EXIT_CRITICAL;

    if (wait)
    {
        Delay_Ms(750);
        onewire_depower();
    }
    return OK;
}

onewire_state ds18x20_read_temperature(onewire_addr_t addr, float *temperature){
    uint8_t family = (uint8_t)addr;
    switch (family)
    {
        case DS18X20_FAMILY_DS18S20:
        case DS18X20_FAMILY_DS1822:
            return ds18s20_read_temperature(addr, temperature);
        case DS18X20_FAMILY_DS18B20:
            return ds18b20_read_temperature(addr, temperature);
        case DS18X20_FAMILY_MAX31850:
            return max31850_read_temperature(addr, temperature);
        default:
            ;
    }
    return DEVICE_NOT_SUPPORTED;
}

onewire_state ds18b20_read_temperature(onewire_addr_t addr, float *temperature){


    uint8_t scratchpad[8];
    ds18x20_read_scratchpad(addr, scratchpad);

    uint16_t temp = scratchpad[1] << 8 | scratchpad[0];
    int sign = 1;
    if (temp > 2047)
    {
        temp = ~temp + 1;
        sign = -1;
    }
    *temperature = (float)temp * (float)sign * 0.0625f;

    return OK;
}

onewire_state ds18s20_read_temperature(onewire_addr_t addr, float *temperature){
    uint8_t scratchpad[8];
    ds18x20_read_scratchpad(addr, scratchpad);

    int16_t temp = (((scratchpad[1] << 8) | (scratchpad[0] & 0xfe)) << 3) | ((0x10 - scratchpad[6]) & 0x0f);
    *temperature = (float)temp * 0.0625f - 0.250f;

    return OK;
}

onewire_state max31850_read_temperature(onewire_addr_t addr, float *temperature){
    uint8_t scratchpad[8];
    ds18x20_read_scratchpad(addr, scratchpad);

    int16_t temp = scratchpad[1] << 8 | (scratchpad[0] & 0xfc);
    *temperature = (float)temp * 0.0625f;

    return OK;
}

onewire_state ds18x20_read_temp_multi(onewire_addr_t *addr_list, size_t addr_count, float *result_list){
    onewire_state res = OK;
    for (size_t i = 0; i < addr_count; i++)
    {
        onewire_state tmp = ds18x20_read_temperature(addr_list[i], &result_list[i]);
        if (tmp != OK)
            res = tmp;
    }
    return res;
}

onewire_state ds18x20_measure_and_read(onewire_addr_t addr, float *temperature){
    ds18x20_measure(addr, true);
    return ds18x20_read_temperature(addr, temperature);
}

onewire_state ds18s20_measure_and_read(onewire_addr_t addr, float *temperature){
    ds18x20_measure(addr, true);
    return ds18s20_read_temperature(addr, temperature);
}

onewire_state ds18b20_measure_and_read(onewire_addr_t addr, float *temperature){
    ds18x20_measure(addr, true);
    return ds18b20_read_temperature(addr, temperature);
}

onewire_state max31850_measure_and_read(onewire_addr_t addr, float *temperature){
    ds18x20_measure(addr, true);
    return max31850_read_temperature(addr, temperature);
}

onewire_state ds18x20_measure_and_read_multi(onewire_addr_t *addr_list, size_t addr_count, float *result_list){
    ds18x20_measure(DS18X20_ANY, true);
    return ds18x20_read_temp_multi(addr_list, addr_count, result_list);
}

onewire_state ds18x20_read_scratchpad(onewire_addr_t addr, uint8_t *buffer){
    uint8_t crc;
    uint8_t expected_crc;

    if (!onewire_reset())
        return INVALID_RESPONSE;

    if (addr == DS18X20_ANY)
        onewire_skip_rom();
    else
        onewire_select(addr);
    onewire_write(ds18x20_READ_SCRATCHPAD);

    for (int i = 0; i < 8; i++)
        buffer[i] = onewire_read();
    crc = onewire_read();

    expected_crc = onewire_crc8(buffer, 8);
    if (crc != expected_crc)
    {
//        ESP_LOGE(TAG, "CRC check failed reading scratchpad: %02x %02x %02x %02x %02x %02x %02x %02x : %02x (expected %02x)", buffer[0], buffer[1],
//                buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], crc, expected_crc);
        return INVALID_CRC;
    }

    return OK;
}

onewire_state ds18x20_write_scratchpad(onewire_addr_t addr, uint8_t *buffer){
    if (!onewire_reset())
       return INVALID_RESPONSE;

   if (addr == DS18X20_ANY)
       onewire_skip_rom();
   else
       onewire_select(addr);
   onewire_write(ds18x20_WRITE_SCRATCHPAD);

   for (int i = 0; i < 3; i++)
       onewire_write(buffer[i]);

   return OK;
}

onewire_state ds18x20_copy_scratchpad(onewire_addr_t addr){
    if (!onewire_reset())
        return INVALID_RESPONSE;

    if (addr == DS18X20_ANY)
        onewire_skip_rom();
    else
        onewire_select(addr);

//    PORT_ENTER_CRITICAL;
    onewire_write(ds18x20_COPY_SCRATCHPAD);
    // For parasitic devices, power must be applied within 10us after issuing
    // the convert command.
    onewire_power();
//    PORT_EXIT_CRITICAL;

    // And then it needs to keep that power up for 10ms.
    Delay_Ms(10);
    onewire_depower();
    return OK;
}

