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

int i_i2c::Read(uint16_t reg, uint8_t *value)
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
    msgs[1].len = 1;
    msgs[1].buf = value;
    data.msgs = msgs;
    data.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    return ret;
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

int i_i2c::Read(uint8_t *buf, uint16_t size)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    int ret = -1;

    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(1, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = I2C_M_RD;
    msgs[0].len = size;
    msgs[0].buf = buf;
    data.msgs = msgs;
    data.nmsgs = 1;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    return ret;
}

int i_i2c::Write(uint16_t reg, uint8_t value)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    uint8_t buffer[1 + sizeof(reg)] = {0};
    int ret = -1;

    buffer[0] = (uint8_t)(reg >> 8);
    buffer[1] = (uint8_t)(reg & 0xFF);
    buffer[2] = value;
    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(1, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = 1 + sizeof(reg);
    msgs[0].buf = buffer;
    data.msgs = msgs;
    data.nmsgs = 1;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    return ret;
}

int i_i2c::Write(uint16_t reg, uint8_t *buf, uint16_t size)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    uint8_t buffer[size + sizeof(reg)] = {0};
    uint8_t buffer_check[size] = {0};
    int ret = -1;

    // ret = Read(reg, buffer_check, size);
    // if (ret < 0)
    //     return -2;

    buffer[0] = (uint8_t)(reg >> 8);
    buffer[1] = (uint8_t)(reg & 0xFF);
    memcpy(buffer + sizeof(reg), buf, size);
    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(1, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = size + sizeof(reg);
    msgs[0].buf = buffer;
    data.msgs = msgs;
    data.nmsgs = 1;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    if (ret < 0)
        return -1;

    ret = Read(reg, buffer_check, size);
    if (ret < 0)
        return -2;

    ret = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        if (buf[i] != buffer_check[i])
        {
            // printf("\tbyte %d set %02x get %02x\n",i, buf[i], buffer_check[i]);
            ret = -3;
        }
    }

    return ret;
}

int i_i2c::Write(uint16_t reg)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    uint8_t buffer[2] = {0};
    int ret = -1;

    buffer[0] = (uint8_t)(reg >> 8);
    buffer[1] = (uint8_t)(reg & 0xFF);
    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(1, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = buffer;
    data.msgs = msgs;
    data.nmsgs = 1;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    return ret;
}
