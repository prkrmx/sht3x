/*
 * File:     sht3x.cpp
 * Notes:    
 *
 * Author:   Engr. Max Parker
 * Created:  Sun Aug 25 2024
 *
 */

#include "sht3x.hpp"

sht3x::sht3x(/* args */)
{
}

sht3x::~sht3x()
{
}

void sht3x::init()
{
    i2c->address = ADDR_1;
    reset();
    get_status();
}

void sht3x::reset()
{
    printf("SHT3X: Reset device\n");
    int ret = i2c->Write<uint16_t>(RESET_CMD);
    if(ret<0)
        printf("SHT3X: ERROR - Reset device\n");
    // 0.5 ms - time between ACK of soft reset command and sensor entering idle state
    usleep(500 * 1000);
}

void sht3x::get_status()
{
    printf("SHT3X: Get status\n");
    int ret;
    int size = 3;
    uint8_t buffer[size] = {0};
    ret = i2c->Read<uint16_t>(STATUS_CMD, buffer, size);
    if (ret < 0)
        printf("SHT3X: ERROR - failed read status\n");

    if (crc8(buffer, 2) != buffer[2])
        printf("SHT3X: ERROR - checksum failed\n");
    uint16_t status = buffer[0] << 8 | buffer[1];
    printf("SHT3X: Status: 0x%hx\n", status);
}

void sht3x::clear_status()
{
    printf("SHT3X: Clear status\n");
    int ret;
    ret = i2c->Write<uint16_t>(CLEAR_STATUS_CMD);
    if (ret < 0)
        printf("SHT3X: ERROR - failed to write CLEAR_STATUS_CMD\n");
}

int sht3x::start(Frequency frq, Repeatability rept)
{
    printf("SHT3X: Start measurements\n");
    int ret;
    // start measurement according to selected mode and return an duration estimate
    ret = i2c->Write<uint16_t>(MEASURE_CMD[(uint8_t)frq][(uint8_t)rept]);
    if (ret < 0)
    {
        printf("SHT3X: ERROR - failed to write MEASURE_CMD\n");
        return ret;
    }
    mode = frq;
    started = true;
    return ret;
}

int sht3x::stop()
{
    printf("SHT3X: Stop measurements\n");
    int ret = i2c->Write<uint16_t>(BREAK_CMD);
    if (ret < 0)
    {
        printf("SHT3X: ERROR - failed to write BREAK_CMD\n");
        return ret;
    }
    started = false;
    return ret;
}

int sht3x::single(float* temperature, float* humidity)
{
    printf("SHT3X: Get Single measurement\n");
    if (start(Frequency::SINGLE_SHOT, Repeatability::HIGH) < 0)
        return -1;

    usleep(MEAS_DURATION_US[(uint8_t)Repeatability::HIGH]);

    return get_results (temperature, humidity);
}

void sht3x::sleep (Repeatability rept)
{
    usleep(MEAS_DURATION_US[(uint8_t)rept]);
}

int sht3x::get_results (float* temperature, float* humidity)
{
    raw_data_t raw_data;
    if (get_data(raw_data) < 0)
        return -1;

    parse_data (raw_data, temperature, humidity);
    return 0;
}

int sht3x::get_data(raw_data_t raw_data)
{
    printf("SHT3X: Get measurement\n");
    int ret = i2c->Read<uint16_t>(FETCH_DATA_CMD, raw_data, RAW_DATA_SIZE);
    if (ret < 0)
    {
        printf("SHT3X: ERROR - failed to read raw data\n");
        return ret;
    }

    if (mode == Frequency::SINGLE_SHOT)
        started = false;

    // check temperature crc
    if (crc8(raw_data, 2) != raw_data[2])
    {
        printf("SHT3X: ERROR - CRC check for temperature data failed\n");
        return -1;
    }

    // check humidity crc
    if (crc8(raw_data + 3, 2) != raw_data[5])
    {
        printf("SHT3X: ERROR - CRC check for humidity data failed\n");
        return -1;
    }

    return ret;
}

void sht3x::parse_data(raw_data_t raw_data, float *temperature, float *humidity)
{
    printf("SHT3X: Parsing raw data\n");
    *temperature = ((((raw_data[0] * 256.0) + raw_data[1]) * 175) / 65535.0) - 45;
    *humidity = ((((raw_data[3] * 256.0) + raw_data[4]) * 100) / 65535.0);
}

uint8_t sht3x::crc8(uint8_t *arr, int size)
{
    uint8_t crc = 0xff;
    for (int i = 0; i < size; i++)
    {
        crc ^= arr[i];
        for (int i = 0; i < 8; i++)
        {
            bool xor_val = crc & 0x80;
            crc = crc << 1;
            crc = xor_val ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}
