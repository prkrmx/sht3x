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
    i2c->address = SHT3x_ADDR_1;
    reset();
    get_status();
}

void sht3x::reset()
{
    printf("SHT3X: Reset device\n");
    int ret = i2c->Write(SHT3x_RESET_CMD);
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
    ret = i2c->Write(SHT3x_STATUS_CMD);
    if (ret < 0)
        printf("SHT3X: ERROR - failed to write SHT3x_STATUS_CMD\n");
    ret = i2c->Read(buffer, size);
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
    ret = i2c->Write(SHT3x_CLEAR_STATUS_CMD);
    if (ret < 0)
        printf("SHT3X: ERROR - failed to write SHT3x_CLEAR_STATUS_CMD\n");
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
