#include <iostream>
#include "i_i2c.hpp"
#include "sht3x.hpp"

int main(/*int argc, char *argv[]*/)
{
    i_i2c i2c;  // Main i2c interface
    sht3x snsr; // Humidity and Temperature Sensor

    printf("MAIN: Set IIC Parameters\n");
    i2c.alias ="IIC";
    i2c.device = "/dev/i2c-2";
    i2c.address = 0x44;



    printf("MAIN: Open IIC Device\n");
    i2c.Open();

    printf("MAIN: Init Sensor\n");
    snsr.i2c = &i2c;
    snsr.init();



    printf("MAIN: Done\n");
}
