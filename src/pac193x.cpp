/*
 * File:     pac193x.cpp
 * Notes:    The PAC1931/2/3/4 power and energy monitoring devices connected to I2C
 *
 * Author:   Engr. Max Parker
 * Created:  Sun Aug 27 2024
 *
 */

#include "pac193x.hpp"

pac193x::pac193x()
{
}

pac193x::~pac193x()
{
}

int pac193x::init()
{
    uint8_t value;
    i2c->address = I2C_ADR;

    int ret = i2c->Write<uint8_t>(REFRESH);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Setup device for default operation\n");
        return 0;
    }

    ret = i2c->Read<uint8_t>(ID_REG, &value);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Get ID\n");
        return 0;
    }

    if (value != 0x5A) // for PAC1933
    {
        printf("PAC193X: ERROR - ID reg does not match expected\n");
        return 0;
    }

    ret = i2c->Write<uint8_t>(CTRL_REG, OVERFLOW);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Turn on ALERT on overflow\n");
        return 0;
    }
    return 1;
}

int pac193x::set_voltage_drct(uint8_t ch, bool direction)
{
    uint8_t value;
    int ret = i2c->Read<uint8_t>(NEG_PWR, &value);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Get Voltage Direction\n");
        return 0;
    }
    ret = direction? i2c->Write<uint8_t>(NEG_PWR, (value | (0x08 >> ch))): i2c->Write<uint8_t>(NEG_PWR, (value & ~(0x08 >> ch))); 
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Set Voltage Direction\n");
        return 0;
    }
    return 1;
}

int pac193x::set_current_drct(uint8_t ch, bool direction)
{
    uint8_t value;
    int ret = i2c->Read<uint8_t>(NEG_PWR, &value);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Get Current Direction\n");
        return 0;
    }
    ret = direction ? i2c->Write<uint8_t>(NEG_PWR, (value | (0x80 >> ch))) : i2c->Write<uint8_t>(NEG_PWR, (value & ~(0x80 >> ch)));
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Set Current Direction\n");
        return 0;
    }
    return 1;
}

bool pac193x::get_voltage_drct(uint8_t ch)
{
    uint8_t value;
    int ret = i2c->Read<uint8_t>(NEG_PWR, &value);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Get Voltage Direction\n");
        return 0;
    }
    return (value >> (3 - ch)) & 0x01;
}

bool pac193x::get_current_drct(uint8_t ch)
{
    uint8_t value;
    int ret = i2c->Read<uint8_t>(NEG_PWR, &value);
    if (ret < 0)
    {
        printf("PAC193X: ERROR - Get Current Direction\n");
        return 0;
    }
    return (value >> (7 - ch)) & 0x01;
}

float pac193x::get_bus_voltage(uint8_t ch, bool mean)
{
    uint16_t raw = get_voltage_raw(BUS1 + ch, mean);
    if (get_voltage_drct(ch))
        return (int16_t)raw * 32 / 32768.0f;
    else
        return raw * 32 / 65536.0f;
    // Return [V]
}

float pac193x::get_sense_voltage(uint8_t ch, bool mean)
{
    uint16_t raw = get_voltage_raw(SENSE1 + ch, mean);
    if (get_voltage_drct(ch))
        return (int16_t)raw * 1.525878906;
    else
        return raw * 1.525878906;
    // Return [uV]
}

uint16_t pac193x::get_voltage_raw(uint8_t reg, bool mean)
{
    uint16_t voltage = 0;
    uint8_t size = 2;
    uint8_t buffer[size] = {0};

    reg += mean ? 0x08 : 0x00;
    if (i2c->Read<uint8_t>(reg, buffer, size) < 0)
        printf("PAC193X: ERROR - Read voltage raw\n");
    else
        // memcpy(&voltage, buffer, size);
        voltage = buffer[0] << 8 | buffer[1]; // (big endian)
    return voltage;
}

float pac193x::get_current(uint8_t ch, bool mean)
{
    float FSC = 100.0f / R[ch]; // Calculate the full scale current, [Amps] using the defined resistor value
    uint16_t raw = get_voltage_raw(SENSE1 + ch, mean);
    if (get_current_drct(ch))
        return (int16_t)raw * FSC * 1000 / 32768.0f;
    else
        return raw * FSC * 1000 / 65536.0f;
    // return [mA]
}