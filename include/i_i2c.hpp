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

    int Read(uint8_t *buf, uint16_t size);

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

    int Write(uint16_t reg);

    int GetStatus(uint32_t b_addr);
};

#endif

/// Read value from 0x0012 register, 0x18 slave address, device /dev/i2c-2
/// sudo i2cset -y 2 0x18 0x00 0x12 && sudo i2cget -y 2 0x18

/// Dump chunk from 0x0200 register, 0x18 slave address, device /dev/i2c-2
/// sudo i2cset -y 2 0x18 0x02 0x00 && sudo i2cdump -y 2 0x18
