#include <iostream>
#include "i_i2c.hpp"
#include "sht3x.hpp"

int main(/*int argc, char *argv[]*/)
{
    float temperature;
    float humidity;

    i_i2c i2c;  // Main i2c interface
    sht3x snsr; // Humidity and Temperature Sensor

    printf("MAIN: Set IIC Parameters\n");
    i2c.alias = "IIC";
    i2c.device = "/dev/i2c-2";
    i2c.address = 0x44;

    printf("MAIN: Open IIC Device\n");
    i2c.Open();

    printf("MAIN: Init Sensor\n");
    snsr.i2c = &i2c;
    snsr.init();

    if (snsr.single(&temperature, &humidity) == 0)
        printf("SHT3x Sensor: %.2f °C, %.2f %%\n", temperature, humidity);

    i2c.Close();

    printf("MAIN: Done\n");
}
