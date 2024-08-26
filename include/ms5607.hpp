/*
 * File:     ms5607.hpp
 * Notes:
 *
 * Author:   Engr. Max Parker
 * Created:  Mon Aug 26 2024
 *
 */

#ifndef MS5607_H_
#define MS5607_H_

#include "stdint.h"
#include "stdbool.h"
#include <math.h>

#include "i_i2c.hpp"

#define READ    0x00     // adc read command
#define PROM    0xA0 // prom read command
#define RESET   0x1E     // soft reset command

#define ACTION_OK 1
#define ACTION_FAIL 0

class ms5607
{
private:
    const float P0 = 1013.25;
    uint16_t OSR = 4096;             // default over sampling ratio
    uint16_t CONV_D1 = 0x48;         // corresponding temp conv. command for OSR
    uint16_t CONV_D2 = 0x58;         // corresponding pressure conv. command for OSR
    uint8_t CONV_DELAY = 10;            // corresponding conv. delay for OSR
    uint16_t C1, C2, C3, C4, C5, C6; // Calibration from device

public:
    unsigned long DP, DT;
    i_i2c *i2c;

    ms5607(/* args */);
    ~ms5607();

    /// @brief Init device
    /// @return Action status
    int init();

    /// @brief Reset Device
    /// @return Action status
    int reset();

    /// @brief Read the raw data and convert into unsigned int
    /// @param reg Register
    /// @param value Returned value
    /// @return Action status
    int read_uint16(uint8_t reg, uint16_t &value);

    /// @brief Read calibration data from PROM
    /// @return Action status
    int calibration();

    /// @brief Read raw data
    /// @return Action status
    int read();

    /// @brief Set OSR and select corresponding values for conversion commands & delay
    /// @param osr
    void setOSR(uint16_t osr);

    /// @brief Read data from device
    /// @param cmd
    /// @param value
    /// @return
    int do_job(uint8_t cmd, unsigned long &value);

    float get_temperature();
    float get_pressure();
    float get_altitude();
};

#endif /* MS5607_H_ */