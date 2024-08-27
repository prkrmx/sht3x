/*
 * File:     i_i2c.cpp
 * Notes:
 *
 * Author:   Engr. Max Parker
 * Created:  Mon Jul 29 2024
 *
 */

#include "i_i2c.hpp"

i_i2c::i_i2c(/* args */)
{
}

i_i2c::~i_i2c()
{
}

int i_i2c::Open()
{
    printf("%s: Open device %s\n", alias.c_str(), device.c_str());
    fd = open(device.c_str(), O_RDWR);
    if (fd < 0)
    {
        printf("%s: Can't open %s: %s\n", alias.c_str(), device.c_str(), strerror(errno));
        return -1;
    }
    return 0;
}

int i_i2c::Close()
{
    if (fd > 0)
    {
        printf("%s: Close device %s\n", alias.c_str(), device.c_str());
        return close(fd);
    }
    return 0;
}



int i_i2c::Read(uint16_t reg, uint8_t *buf, uint16_t size)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    uint8_t reg_buf[sizeof(reg)] = {0};
    int ret = -1;

    reg_buf[0] = (uint8_t)(reg >> 8);
    reg_buf[1] = (uint8_t)(reg & 0xFF);
    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(2, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = sizeof(reg);
    msgs[0].buf = reg_buf;
    msgs[1].addr = address;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = size;
    msgs[1].buf = buf;
    data.msgs = msgs;
    data.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);

    // for (int i = 0; i < size; ++i)
    //     printf("%02x ", buf[i]);
    // printf("\n");
    return ret;
}

