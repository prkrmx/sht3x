/*
 * File:     ms5607.cpp
 * Notes:
 *
 * Author:   Engr. Max Parker
 * Created:  Mon Aug 26 2024
 *
 * Copyright (C) 2024 RP Optical Lab
 */

#include "ms5607.hpp"

ms5607::ms5607(/* args */)
{
}

ms5607::~ms5607()
{
}

// Initialise coefficient by reading calibration data
int ms5607::init()
{
    if (not reset() || calibration())
        return ACTION_FAIL;
    return ACTION_OK;
}

int ms5607::reset()
{
    printf("MS5607: Reset device\n");
    int ret = i2c->Write<uint8_t>(RESET);
    if (ret < 0)
    {
        printf("MS5607: ERROR - Reset device\n");
        return ACTION_FAIL;
    }
    usleep(3 * 1000); // wait for internal register reload
    return ACTION_OK;
}

int ms5607::calibration()
{
    printf("MS5607: Get calibration stuff\n");
    if (read_uint16(PROM + 2, C1) &&
        read_uint16(PROM + 4, C2) &&
        read_uint16(PROM + 6, C3) &&
        read_uint16(PROM + 8, C4) &&
        read_uint16(PROM + 10, C5) &&
        read_uint16(PROM + 12, C6))
        return ACTION_OK;
    else
    {
        printf("MS5607: ERROR - Get calibration stuff\n");
        return ACTION_FAIL;
    }
}

int ms5607::read_uint16(uint8_t reg, uint16_t &value)
{
    uint8_t buffer[2];
    int ret;

    ret = i2c->Read<uint8_t>(reg, buffer, 2);
    if (ret < 0)
        return ACTION_FAIL;

    value = (((unsigned int)buffer[0] * (1 << 8)) | (unsigned int)buffer[1]);
    return ACTION_OK;
}

int ms5607::do_job(uint8_t cmd, unsigned long &value)
{
    uint8_t length = 3;
    uint8_t data[length];

    // start conversion
    int ret = i2c->Write<uint8_t>(cmd);
    if (ret < 0)
    {
        printf("MS5607: ERROR - Conversion\n");
        return ACTION_FAIL;
    }
    usleep(CONV_DELAY * 1000);

    ret = i2c->Read<uint8_t>(READ, data, length);
    if (ret < 0)
        return ACTION_FAIL;
    value = (unsigned long)data[0] * 1 << 16 | (unsigned long)data[1] * 1 << 8 | (unsigned long)data[2];
    return ACTION_OK;
}

int ms5607::read()
{
    printf("MS5607: Read device raw\n");
    if (not do_job(CONV_D1, DP))
        return ACTION_FAIL;

    if (not do_job(CONV_D2, DT))
        return ACTION_FAIL;

    return ACTION_OK;
}

float ms5607::get_temperature(void)
{
    float dT = (float)DT - ((float)C5) * ((int)1 << 8);
    float TEMP = 2000.0 + dT * ((float)C6) / (float)((long)1 << 23);
    return TEMP / 100;
}

float ms5607::get_pressure(void)
{
    float dT = (float)DT - ((float)C5) * ((int)1 << 8);
    int64_t OFF = (((int64_t)C2) * ((long)1 << 17)) + dT * ((float)C4) / ((int)1 << 6);
    int64_t SENS = ((float)C1) * ((long)1 << 16) + dT * ((float)C3) / ((int)1 << 7);
    float pa = (float)((float)DP / ((long)1 << 15));
    float pb = (float)(SENS / ((float)((long)1 << 21)));
    float pc = pa * pb;
    float pd = (float)(OFF / ((float)((long)1 << 15)));
    float P = pc - pd;
    return P / 100;
}

float ms5607::get_altitude(void)
{
    float h, t, p;
    t = get_temperature();
    p = get_pressure();
    p = P0 / p;
    h = 153.84615 * (pow(p, 0.19) - 1) * (t + 273.15);
    return h;
}

void ms5607::setOSR(uint16_t osr)
{
    this->OSR = osr;
    switch (OSR)
    {
    case 256:
        CONV_D1 = 0x40;
        CONV_D2 = 0x50;
        CONV_DELAY = 1;
        break;
    case 512:
        CONV_D1 = 0x42;
        CONV_D2 = 0x52;
        CONV_DELAY = 2;
        break;
    case 1024:
        CONV_D1 = 0x44;
        CONV_D2 = 0x54;
        CONV_DELAY = 3;
        break;
    case 2048:
        CONV_D1 = 0x46;
        CONV_D2 = 0x56;
        CONV_DELAY = 5;
        break;
    case 4096:
        CONV_D1 = 0x48;
        CONV_D2 = 0x58;
        CONV_DELAY = 10;
        break;
    default:
        CONV_D1 = 0x40;
        CONV_D2 = 0x50;
        CONV_DELAY = 1;
        break;
    }
}
