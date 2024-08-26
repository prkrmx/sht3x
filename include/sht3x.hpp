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

#include "stdint.h"
#include "stdbool.h"
#include "i_i2c.hpp"

/// @brief Data acquisition frequency (0.5, 1, 2, 4 & 10 measurements per second, mps)
enum class Frequency : uint8_t
{
    SINGLE_SHOT, // one single measurement
    PERIODIC_05, // periodic with 0.5 measurements per second (mps)
    PERIODIC_1,  // periodic with   1 measurements per second (mps)
    PERIODIC_2,  // periodic with   2 measurements per second (mps)
    PERIODIC_4,  // periodic with   4 measurements per second (mps)
    PERIODIC_10  // periodic with  10 measurements per second (mps)
};

/// @brief Repeatability Options
enum class Repeatability : uint8_t
{
    HIGH,
    MEDIUM,
    LOW
};

class sht3x
{
    // definition of possible I2C slave addresses
    #define ADDR_1 0x44 // ADDR pin connected to GND/VSS (default)
    #define ADDR_2 0x45 // ADDR pin connected to VDD

    // SHT3x driver error codes
    #define MEAS_NOT_STARTED        (1 << 8)
    #define MEAS_ALREADY_RUNNING    (2 << 8)
    #define MEAS_STILL_RUNNING      (3 << 8)
    #define READ_RAW_DATA_FAILED    (4 << 8)

    #define SEND_MEAS_CMD_FAILED    (5 << 8)
    #define SEND_RESET_CMD_FAILED   (6 << 8)
    #define SEND_STATUS_CMD_FAILED  (7 << 8)
    #define SEND_FETCH_CMD_FAILED   (8 << 8)

    #define WRONG_CRC_TEMPERATURE   (9 << 8)
    #define WRONG_CRC_HUMIDITY      (10 << 8)

    #define STATUS_CMD          0xF32D
    #define CLEAR_STATUS_CMD    0x3041
    #define RESET_CMD           0x30A2
    #define FETCH_DATA_CMD      0xE000
    #define HEATER_OFF_CMD      0x3066
    #define BREAK_CMD           0x3093 // Break command - Stop Periodic Data Acquisition Mode

    #define MEAS_DURATION_HIGH  15
    #define MEAS_DURATION_MED   6
    #define MEAS_DURATION_LOW   4

    #define RAW_DATA_SIZE       6
    typedef uint8_t raw_data_t[RAW_DATA_SIZE];

    const uint16_t MEASURE_CMD[6][3] = {
        {0x2400, 0x240b, 0x2416},  // [SINGLE_SHOT][H,M,L] without clock stretching
        {0x2032, 0x2024, 0x202f},  // [PERIODIC_05][H,M,L]
        {0x2130, 0x2126, 0x212d},  // [PERIODIC_1 ][H,M,L]
        {0x2236, 0x2220, 0x222b},  // [PERIODIC_2 ][H,M,L]
        {0x2234, 0x2322, 0x2329},  // [PERIODIC_4 ][H,M,L]
        {0x2737, 0x2721, 0x272a}}; // [PERIODIC_10][H,M,L]

    // measurement durations in us
    const uint16_t MEAS_DURATION_US[3] = {MEAS_DURATION_HIGH * 1000,
                                          MEAS_DURATION_MED  * 1000,
                                          MEAS_DURATION_LOW  * 1000};

private:
    const uint8_t g_polynom = 0x31;
    Frequency mode;
    bool started;

public:
    i_i2c *i2c;

    sht3x(/* args */);
    ~sht3x();

    void init();
    void reset();
    void get_status();
    void clear_status();
    int start(Frequency frq, Repeatability rept);
    int single(float *temperature, float *humidity);
    uint8_t crc8(uint8_t *arr, int size);
    void parse_data(raw_data_t raw_data, float *temperature, float *humidity);
    int get_results (float* temperature, float* humidity);
    int get_data(raw_data_t raw_data);
    void sleep (Repeatability rept);
    int stop();
};

#endif /* SHT3x_H_ */