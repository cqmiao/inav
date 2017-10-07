/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#include "drivers/bus_i2c.h"
#include "drivers/bus_spi.h"

typedef enum {
    BUSTYPE_NONE = 0,
    BUSTYPE_I2C  = 1,
    BUSTYPE_SPI  = 2
} busType_e;

/* Ultimately all hardware descriptors will go to target definition files. 
 * Driver code will merely query for it's HW descriptor and initialize it */
typedef enum {
    DEVHW_NONE,
    DEVHW_MAX7456,
    DEVHW_BMP280
} devHardwareType_e;

typedef struct busDeviceDescriptor_s {
    busType_e           busType;
    devHardwareType_e   devHwType;
    union {
        struct {
            SPIDevice   spiBus;
            ioTag_t     csnPin;
        } spi;
        struct {
            I2CDevice   i2cBus;
            uint8_t     address;
        } i2c;
    } dev;
} busDeviceDescriptor_t;

/*
typedef struct busDevice_s {
    busType_e           busType;
    union {
        struct {
            SPIDevice   spiBus;
            IO_t        csnPin;
        } spi;
        struct {
            I2CDevice   i2cBus;
            uint8_t     address;
        } i2c;
    } busdev;
} busDevice_t;
*/

typedef union busDevice_u {
    struct deviceSpi_s {
        IO_t csnPin;
    } spi;
    struct deviceI2C_s {
        uint8_t address;
    } i2c;
} busDevice_t;

#ifdef __APPLE__
extern const busDeviceDescriptor_t __busdev_registry_start[] __asm("section$start$__DATA$__busdev_registry");
extern const busDeviceDescriptor_t __busdev_registry_end[] __asm("section$end$__DATA$__busdev_registry");
#define BUSDEV_REGISTER_ATTRIBUTES __attribute__ ((section("__DATA,__busdev_registry"), used, aligned(4)))
#else
extern const busDeviceDescriptor_t __busdev_registry_start[];
extern const busDeviceDescriptor_t __busdev_registry_end[];
#define BUSDEV_REGISTER_ATTRIBUTES __attribute__ ((section(".busdev_registry"), used, aligned(4)))
#endif

#define BUSDEV_REGISTER_SPI(_name, _devHw, _spiBus, _csnPin)            \
    extern const busDeviceDescriptor_t _name;                           \
    const busDeviceDescriptor_t _name BUSDEV_REGISTER_ATTRIBUTES = {    \
        .busType = BUSTYPE_SPI,                                         \
        .devHwType = _devHw,                                            \
        .dev.spi = {                                                    \
            .spiBus = _spiBus,                                          \
            .csnPin = IO_TAG(_csnPin)                                   \
        }                                                               \
    };                                                                  \
    /**/


/* Pre-initialize all known device descriptors to make sure hardware state is consistent and known 
 * Initialize bus hardware */
void busInit(void);

bool busWriteBuf(const busDevice_t * busdev, uint8_t reg, uint8_t * data, uint8_t length);
bool busWrite(const busDevice_t * busdev, uint8_t reg, uint8_t data);
bool busReadBuf(const busDevice_t * busdev, uint8_t reg, uint8_t * data, uint8_t length);
bool busRead(const busDevice_t * busdev, uint8_t reg, uint8_t * data);