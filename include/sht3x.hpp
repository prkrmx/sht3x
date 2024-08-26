/*
 * File:     sht3x.hpp
 * Notes:    SHT3x temperature and humidity sensor connected to I2C
 *
 * Author:   Engr. Max Parker
 * Created:  Sun Aug 25 2024
 *
 */

 
#ifndef SHT3x_H_
#define SHT3x_H_

// Uncomment to enable debug output
// #define SHT3x_DEBUG_LEVEL_1     // only error messages
// #define SHT3x_DEBUG_LEVEL_2     // error and debug messages

#include "stdint.h"
#include "stdbool.h"
#include "i_i2c.hpp"

// definition of possible I2C slave addresses
#define SHT3x_ADDR_1 0x44        // ADDR pin connected to GND/VSS (default)
#define SHT3x_ADDR_2 0x45        // ADDR pin connected to VDD

// definition of error codes
#define SHT3x_OK                     0
#define SHT3x_NOK                    -1

#define SHT3x_I2C_ERROR_MASK         0x000f
#define SHT3x_DRV_ERROR_MASK         0xfff0

// error codes for I2C interface ORed with SHT3x error codes
#define SHT3x_I2C_READ_FAILED        1
#define SHT3x_I2C_SEND_CMD_FAILED    2
#define SHT3x_I2C_BUSY               3

// SHT3x driver error codes OR ed with error codes for I2C interface
#define SHT3x_MEAS_NOT_STARTED       (1  << 8)
#define SHT3x_MEAS_ALREADY_RUNNING   (2  << 8)
#define SHT3x_MEAS_STILL_RUNNING     (3  << 8)
#define SHT3x_READ_RAW_DATA_FAILED   (4  << 8)

#define SHT3x_SEND_MEAS_CMD_FAILED   (5  << 8)
#define SHT3x_SEND_RESET_CMD_FAILED  (6  << 8)
#define SHT3x_SEND_STATUS_CMD_FAILED (7  << 8)
#define SHT3x_SEND_FETCH_CMD_FAILED  (8  << 8)

#define SHT3x_WRONG_CRC_TEMPERATURE  (9  << 8)
#define SHT3x_WRONG_CRC_HUMIDITY     (10 << 8)

#define SHT3x_RAW_DATA_SIZE 6

#define SHT3x_STATUS_CMD               0xF32D
#define SHT3x_CLEAR_STATUS_CMD         0x3041
#define SHT3x_RESET_CMD                0x30A2
#define SHT3x_FETCH_DATA_CMD           0xE000
#define SHT3x_HEATER_OFF_CMD           0x3066

const uint16_t SHT3x_MEASURE_CMD[6][3] = { 
        {0x2400,0x240b,0x2416},    // [SINGLE_SHOT][H,M,L] without clock stretching
        {0x2032,0x2024,0x202f},    // [PERIODIC_05][H,M,L]
        {0x2130,0x2126,0x212d},    // [PERIODIC_1 ][H,M,L]
        {0x2236,0x2220,0x222b},    // [PERIODIC_2 ][H,M,L]
        {0x2234,0x2322,0x2329},    // [PERIODIC_4 ][H,M,L]
        {0x2737,0x2721,0x272a} };  // [PERIODIC_10][H,M,L]


class sht3x
{
private:
    const uint8_t g_polynom = 0x31;
public:
    i_i2c *i2c;

    sht3x(/* args */);
    ~sht3x();

    void init();
    void reset();
    void get_status();
    void clear_status();
    uint8_t crc8 (uint8_t *arr, int size);
};


#endif /* SHT3x_H_ */