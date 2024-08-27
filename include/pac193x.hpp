/*
 * File:     pac193x.hpp
 * Notes:    The PAC1931/2/3/4 power and energy monitoring devices connected to I2C
 *
 * Author:   Engr. Max Parker
 * Created:  Sun Aug 27 2024
 *
 */

#ifndef PAC193x_H_
#define PAC193x_H_

#include "stdint.h"
#include "stdbool.h"
#include "i_i2c.hpp"


class pac193x
{
    enum Channel
    {
        CH1 = 0,
        CH2 = 1,
        CH3 = 2,
        CH4 = 3,
    };
#define SENSE1 0x0B
#define SENSE2 0x0C
#define SENSE3 0x0D
#define SENSE4 0x0E
#define BUS1 0x07
#define BUS2 0x08
#define BUS3 0x09
#define BUS4 0x0A

#define I2C_ADR     0x10
    #define REFRESH 0x00
    #define CTRL_REG    0x01
    #define ID_REG      0xFD
    #define NEG_PWR     0x1D // Enabling bidirectional current and bipolar voltage measurements

    #define OVERFLOW    0x0A   // Turn on ALERT on overflow
    #define BIDIRECTIONAL 1
    #define UNIDIRECTIONAL 0

private:
public:
    i_i2c *i2c;
    float R[4] = {10, 10, 10, 10}; // Resistor values [mOhms]

    pac193x(/* args */);
    ~pac193x();

    /// @brief Init device
    /// @return Action status
    int init();

    /// @brief Set Voltage Direction
    /// @param ch Channel
    /// @param direction Direction
    /// @return Action status
    int set_voltage_drct(uint8_t ch, bool direction);

    /// @brief Set Current Direction
    /// @param ch Channel
    /// @param direction Direction
    /// @return Action status
    int set_current_drct(uint8_t ch, bool direction);

    /// @brief Get Voltage Direction
    /// @param ch Channel
    /// @return BIDIRECTIONAL 1, UNIDIRECTIONAL 0
    bool get_voltage_drct(uint8_t ch);

    /// @brief Get Current Direction
    /// @param ch Channel
    /// @return BIDIRECTIONAL 1, UNIDIRECTIONAL 0
    bool get_current_drct(uint8_t ch);

    float get_current(uint8_t ch, bool mean);
    float get_bus_voltage(uint8_t ch, bool mean);
    float get_sense_voltage(uint8_t ch, bool mean);
    uint16_t get_voltage_raw(uint8_t reg, bool mean);
};

#endif /* PAC193x_H_ */