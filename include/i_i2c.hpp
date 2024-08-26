/*
 * File:     i_i2c.hpp
 * Notes:    i2c interface
 *
 * Author:   Engr. Max Parker
 * Created:  Mon Jul 29 2024
 *
 */

#ifndef I_I2C_H
#define I_I2C_H

#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <cstring>
#include <errno.h>
#include <sys/mman.h>
#include <iostream>


class i_i2c
{
private:
    int fd; // File descriptor

public:
    std::string alias;  // Device alias
    std::string device; // Device name
    uint8_t address;    // Slave address

    i_i2c(/* args */);
    ~i_i2c();

    /// @brief Open i2c device
    /// @return Action status
    int Open();

    /// @brief Close i2c device
    /// @return Action status
    int Close();

    /// @brief Read from register
    /// @param reg Register
    /// @param value Value
    /// @return Action status
    int Read(uint16_t reg, uint8_t *value);

    /// @brief Read data to array
    /// @param reg Register
    /// @param buf Destination Array
    /// @param size Array size
    /// @return Action status
    int Read(uint16_t reg, uint8_t *buf, uint16_t size);

    /// @brief Write to register
    /// @param reg Register
    /// @param value Value
    /// @return Action status
    int Write(uint16_t reg, uint8_t value);

    /// @brief Write array
    /// @param reg Start register
    /// @param buf Array to write
    /// @param size Array size
    /// @return Action status
    int Write(uint16_t reg, uint8_t *buf, uint16_t size);

    /// @brief Write Command
    /// @tparam T Command type uint8_t or uint16_t
    /// @param reg Command
    /// @return Action status
    template <typename T> 
    int Write(T reg);

    /// @brief Read array from register
    /// @tparam T Register type uint8_t or uint16_t
    /// @param reg Register
    /// @param buf Destination Array
    /// @param size Size to read
    /// @return Action status
    template <typename T> 
    int Read(T reg, uint8_t *buf, uint16_t size);
};


template <typename T>
int i_i2c::Write(T reg)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    int size = sizeof(T);
    uint8_t buffer[size] = {0};
    int ret = -1;

    if (size == 1)
        buffer[0] = reg;
    else // if (size == 2)
    {
        buffer[0] = (uint8_t)(reg >> 8);
        buffer[1] = (uint8_t)(reg & 0xFF);
    }

    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(1, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = size;
    msgs[0].buf = buffer;
    data.msgs = msgs;
    data.nmsgs = 1;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    return ret;
}

template <typename T>
int i_i2c::Read(T reg, uint8_t *buf, uint16_t size)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg *msgs = NULL;
    int reg_size = sizeof(T);
    uint8_t reg_buf[reg_size] = {0};
    int ret = -1;

    if (reg_size == 1)
        reg_buf[0] = reg;
    else // if (size == 2)
    {
        reg_buf[0] = (uint8_t)(reg >> 8);
        reg_buf[1] = (uint8_t)(reg & 0xFF);
    }

    memset(&data, 0, sizeof(data));
    msgs = (i2c_msg *)calloc(2, sizeof(struct i2c_msg));
    if (NULL == msgs)
        return ret;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = reg_size;
    msgs[0].buf = reg_buf;
    msgs[1].addr = address;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = size;
    msgs[1].buf = buf;
    data.msgs = msgs;
    data.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, &data);
    free(msgs);
    return ret;
}

#endif
