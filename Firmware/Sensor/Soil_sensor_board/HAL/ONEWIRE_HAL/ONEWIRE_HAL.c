/*
 * ONEWIRE_HAL.c
 *
 *  Created on: Mar 27, 2024
 *      Author: thang
 */

#include "ONEWIRE_HAL.h"

#define ONEWIRE_SELECT_ROM 0x55
#define ONEWIRE_SKIP_ROM   0xcc
#define ONEWIRE_SEARCH     0xf0

onewire_config_t onewire_config;

unsigned char ROM_NO[8];
uint8_t LastDiscrepancy;
uint8_t LastDeviceFlag;
uint8_t LastFamilyDiscrepancy;

static inline bool _onewire_wait_for_bus(int max_wait)
{
    bool state;
    for (int i = 0; i < ((max_wait + 4) / 5); i++)
    {
        if (GPIO_ReadInputDataBit(onewire_config.port, onewire_config.pin))
            break;
        Delay_Us(5);
    }
    state = GPIO_ReadInputDataBit(onewire_config.port, onewire_config.pin);
    // Wait an extra 1us to make sure the devices have an adequate recovery
    // time before we drive things low again.
    Delay_Us(1);
    return state;
}

static void setup_pin(bool open_drain)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = onewire_config.pin;
    if(open_drain){
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    }
    else{
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    }
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(onewire_config.port, &GPIO_InitStructure);
}

void onewire_init(GPIO_TypeDef* GPIOx, uint16_t pin){
    onewire_config.port = GPIOx;
    onewire_config.pin = pin;
    if(onewire_config.port == GPIOA){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }
    else if(onewire_config.port == GPIOC){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    else{
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    }
    return;
}

bool onewire_reset()
{
    setup_pin(true);

    GPIO_WriteBit(onewire_config.port, onewire_config.pin, 1);
    // wait until the wire is high... just in case
    if (!_onewire_wait_for_bus(250))
        return false;

    GPIO_WriteBit(onewire_config.port, onewire_config.pin, 0);
    Delay_Us(480);

    GPIO_WriteBit(onewire_config.port, onewire_config.pin, 1); // allow it to float
    Delay_Us(70);
    bool r = !GPIO_ReadInputDataBit(onewire_config.port, onewire_config.pin);

    // Wait for all devices to finish pulling the bus low before returning
    if (!_onewire_wait_for_bus(410))
        return false;

    return r;
}

static bool _onewire_write_bit(bool v)
{
    if (!_onewire_wait_for_bus(10))
        return false;
//    PORT_ENTER_CRITICAL;
    if (v)
    {
        GPIO_WriteBit(onewire_config.port, onewire_config.pin, 0);  // drive output low
        Delay_Us(10);
        GPIO_WriteBit(onewire_config.port, onewire_config.pin, 1);  // allow output high
        Delay_Us(55);
    }
    else
    {
        GPIO_WriteBit(onewire_config.port, onewire_config.pin, 0);  // drive output low
        Delay_Us(65);
        GPIO_WriteBit(onewire_config.port, onewire_config.pin, 1); // allow output high
        Delay_Us(5);
    }
//    PORT_EXIT_CRITICAL;

    return true;
}

static int _onewire_read_bit()
{
    if (!_onewire_wait_for_bus(10))
        return -1;

//    PORT_ENTER_CRITICAL;
    GPIO_WriteBit(onewire_config.port, onewire_config.pin, 0);
    Delay_Us(3);
    GPIO_WriteBit(onewire_config.port, onewire_config.pin, 1);  // let pin float, pull up will raise
    Delay_Us(10);
    int r = GPIO_ReadInputDataBit(onewire_config.port, onewire_config.pin);  // Must sample within 15us of start
    Delay_Us(53)    ;
//    PORT_EXIT_CRITICAL;

    return r;
}

bool onewire_write(uint8_t v)
{
    for (uint8_t bitMask = 0x01; bitMask; bitMask <<= 1)
        if (!_onewire_write_bit((bitMask & v)?1:0))
            return false;

    return true;
}

bool onewire_write_bytes(const uint8_t *buf, size_t count)
{
    for (size_t i = 0; i < count; i++)
        if (!_onewire_write_bit(buf[i]))
            return false;

    return true;
}

int onewire_read()
{
    int r = 0;

    for (uint8_t bitMask = 0x01; bitMask; bitMask <<= 1)
    {
        int bit = _onewire_read_bit();
        if (bit < 0)
            return -1;
        else if (bit)
            r |= bitMask;
    }
    return r;
}

bool onewire_read_bytes(uint8_t *buf, size_t count)
{
    size_t i;
    int b;

    for (i = 0; i < count; i++)
    {
        b = onewire_read();
        if (b < 0)
            return false;
        buf[i] = b;
    }
    return true;
}

bool onewire_select(onewire_addr_t addr)
{
    uint8_t i;

    if (!onewire_write(ONEWIRE_SELECT_ROM))
        return false;

    for (i = 0; i < 8; i++)
    {
        if (!onewire_write(addr & 0xff))
            return false;
        addr >>= 8;
    }

    return true;
}

bool onewire_skip_rom()
{
    return onewire_write(ONEWIRE_SKIP_ROM);
}

bool onewire_power()
{
    // Make sure the bus is not being held low before driving it high, or we
    // may end up shorting ourselves out.
    if (!_onewire_wait_for_bus(10))
        return false;

    setup_pin(false);
    GPIO_WriteBit(onewire_config.port, onewire_config.pin, 1);

    return true;
}

void onewire_depower()
{
    setup_pin(true);
}

void onewire_search_start(onewire_search_t *search)
{
    // reset the search state
    memset(search, 0, sizeof(*search));
}

void onewire_search_prefix(onewire_search_t *search, uint8_t family_code)
{
    uint8_t i;

    search->rom_no[0] = family_code;
    for (i = 1; i < 8; i++)
    {
        search->rom_no[i] = 0;
    }
    search->last_discrepancy = 64;
    search->last_device_found = false;
}

// Perform a search. If the next device has been successfully enumerated, its
// ROM address will be returned.  If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then ONEWIRE_NONE is returned.  Use OneWire::reset_search() to
// start over.
//
// --- Replaced by the one from the Dallas Semiconductor web site ---
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return 1 : device found, ROM number in ROM_NO buffer
//        0 : device not found, end of search
//
onewire_addr_t onewire_search_internal(onewire_search_t *search, uint8_t command)
{
    //TODO: add more checking for read/write errors
    uint8_t id_bit_number;
    uint8_t last_zero, search_result;
    int rom_byte_number;
    int8_t id_bit, cmp_id_bit;
    onewire_addr_t addr;
    unsigned char rom_byte_mask;
    bool search_direction;

    // initialize for search
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = 0;

    // if the last call was not the last one
    if (!search->last_device_found)
    {
        // 1-Wire reset
        if (!onewire_reset())
        {
            // reset the search
            search->last_discrepancy = 0;
            search->last_device_found = false;
            return ONEWIRE_NONE;
        }

        // issue the search command
        onewire_write(command);

        // loop to do the search
        do
        {
            // read a bit and its complement
            id_bit = _onewire_read_bit();
            cmp_id_bit = _onewire_read_bit();

            if ((id_bit == 1) && (cmp_id_bit == 1))
                break;
            else
            {
                // all devices coupled have 0 or 1
                if (id_bit != cmp_id_bit)
                    search_direction = id_bit;  // bit write value for search
                else
                {
                    // if this discrepancy if before the Last Discrepancy
                    // on a previous next then pick the same as last time
                    if (id_bit_number < search->last_discrepancy)
                        search_direction = ((search->rom_no[rom_byte_number] & rom_byte_mask) > 0);
                    else
                        // if equal to last pick 1, if not then pick 0
                        search_direction = (id_bit_number == search->last_discrepancy);

                    // if 0 was picked then record its position in LastZero
                    if (!search_direction)
                    {
                        last_zero = id_bit_number;
                        if (last_zero < 9)
                            LastFamilyDiscrepancy = last_zero;
                    }
                }

                // set or clear the bit in the ROM byte rom_byte_number
                // with mask rom_byte_mask
                if (search_direction)
                    search->rom_no[rom_byte_number] |= rom_byte_mask;
                else
                    search->rom_no[rom_byte_number] &= ~rom_byte_mask;

                // serial number search direction write bit
                _onewire_write_bit(search_direction);

                // increment the byte counter id_bit_number
                // and shift the mask rom_byte_mask
                id_bit_number++;
                rom_byte_mask <<= 1;

                // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if (rom_byte_mask == 0)
                {
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        } while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

        // if the search was successful then
        if (!(id_bit_number < 65))
        {
            // search successful so set last_discrepancy,last_device_found,search_result
            search->last_discrepancy = last_zero;

            // check for last device
            if (search->last_discrepancy == 0)
                search->last_device_found = true;

            search_result = 1;
        }
    }

    // if no device found then reset counters so next 'search' will be like a first
    if (!search_result || !search->rom_no[0])
    {
        search->last_discrepancy = 0;
        search->last_device_found = false;
        LastFamilyDiscrepancy = 0;
        return ONEWIRE_NONE;
    }
    else
    {
        addr = 0;
        for (rom_byte_number = 7; rom_byte_number >= 0; rom_byte_number--)
        {
            addr = (addr << 8) | search->rom_no[rom_byte_number];
        }
//        printf("Ok I found something at %08x%08x...\n", (uint32_t)(addr >> 32), (uint32_t)addr);
    }
    return addr;
}

onewire_addr_t onewire_search(onewire_search_t *search)
{
    return onewire_search_internal(search,0xF0);
}


//
// Like search. Return devices that are in alarm state only.
//

onewire_addr_t search_alarms(onewire_search_t *search)
{
    return onewire_search_internal(search,0xEC);
}
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

#ifdef CONFIG_ONEWIRE_CRC8_TABLE
// This table comes from Dallas sample code where it is freely reusable,
// though Copyright (c) 2000 Dallas Semiconductor Corporation
static const uint8_t dscrc_table[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

//
// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (note: this might better be done without to
// table, it would probably be smaller and certainly fast enough
// compared to all those delayMicrosecond() calls.  But I got
// confused, so I use this table from the examples.)
//
uint8_t onewire_crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;

    while (len--)
        crc = dscrc_table[crc ^ *data++];

    return crc;
}
#else
//
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but much smaller, than the lookup table.
//
uint8_t onewire_crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;

    while (len--)
    {
        uint8_t inbyte = *data++;
        for (int i = 8; i; i--)
        {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix)
                crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}
#endif

// Compute the 1-Wire CRC16 and compare it against the received CRC.
// Example usage (reading a DS2408):
//    // Put everything in a buffer so we can compute the CRC easily.
//    uint8_t buf[13];
//    buf[0] = 0xF0;    // Read PIO Registers
//    buf[1] = 0x88;    // LSB address
//    buf[2] = 0x00;    // MSB address
//    WriteBytes(net, buf, 3);    // Write 3 cmd bytes
//    ReadBytes(net, buf+3, 10);  // Read 6 data bytes, 2 0xFF, 2 CRC16
//    if (!CheckCRC16(buf, 11, &buf[11])) {
//        // Handle error.
//    }
//
// @param input - Array of bytes to checksum.
// @param len - How many bytes to use.
// @param inverted_crc - The two CRC16 bytes in the received data.
//                       This should just point into the received data,
//                       *not* at a 16-bit integer.
// @param crc - The crc starting value (optional)
// @return 1, iff the CRC matches.
bool onewire_check_crc16(const uint8_t* input, size_t len, const uint8_t* inverted_crc, uint16_t crc_iv)
{
    uint16_t crc = ~onewire_crc16(input, len, crc_iv);
    return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}

// Compute a Dallas Semiconductor 16 bit CRC.  This is required to check
// the integrity of data received from many 1-Wire devices.  Note that the
// CRC computed here is *not* what you'll get from the 1-Wire network,
// for two reasons:
//   1) The CRC is transmitted bitwise inverted.
//   2) Depending on the endian-ness of your processor, the binary
//      representation of the two-byte return value may have a different
//      byte order than the two bytes you get from 1-Wire.
// @param input - Array of bytes to checksum.
// @param len - How many bytes to use.
// @param crc - The crc starting value (optional)
// @return The CRC16, as defined by Dallas Semiconductor.
uint16_t onewire_crc16(const uint8_t* input, size_t len, uint16_t crc_iv)
{
    uint16_t crc = crc_iv;
    static const uint8_t oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

    uint16_t i;
    for (i = 0; i < len; i++)
    {
        // Even though we're just copying a byte from the input,
        // we'll be doing 16-bit computation with it.
        uint16_t cdata = input[i];
        cdata = (cdata ^ crc) & 0xff;
        crc >>= 8;

        if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
            crc ^= 0xC001;

        cdata <<= 6;
        crc ^= cdata;
        cdata <<= 1;
        crc ^= cdata;
    }
    return crc;
}
