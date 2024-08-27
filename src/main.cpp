#include <iostream>
#include "i_i2c.hpp"
#include "sht3x.hpp"
#include "ms5607.hpp"
#include "pac193x.hpp"

#define SHT3X 1
#define MS5607 1
#define CNTR 1

int main(/*int argc, char *argv[]*/)
{
    int cntr = CNTR;
    i_i2c i2c; // Main i2c interface
    printf("MAIN: Set IIC Parameters\n");
    i2c.alias = "IIC";
    i2c.device = "/dev/i2c-2";
    
    printf("MAIN: Open IIC Device\n");
    i2c.Open();

#if SHT3X
    i2c.address = 0x44; // For sht3x
    
    float temperature;
    float humidity;
    sht3x snsr; // Humidity and Temperature Sensor

    printf("MAIN: Init Sensor\n");
    snsr.i2c = &i2c;
    snsr.init();

    if (snsr.single(&temperature, &humidity) == 0)
        printf("SHT3x Sensor: %.2f °C, %.2f %%\n", temperature, humidity);

    // Start periodic measurements with 1 measurement per second.
    snsr.start(Frequency::PERIODIC_1, Repeatability::HIGH);
    snsr.sleep(Repeatability::HIGH);

    while (0 < cntr--)
    {
        if (snsr.get_results(&temperature, &humidity) == 0)
            printf("----- Sensor: %.2f °C, %.2f %%\n", temperature, humidity);
        else
            printf("SHT3x Error");
        sleep(1);
    }

    snsr.stop();
#endif

#if MS5607
    ms5607 s_ms5607;
    i2c.address = 0x76; // For ms5607
    s_ms5607.i2c = &i2c;
    float P_val,T_val,H_val;
    s_ms5607.init();

    cntr = CNTR;
    while (0 < cntr--)
    {
        if (s_ms5607.read())
        {
            T_val = s_ms5607.get_temperature();
            P_val = s_ms5607.get_pressure();
            H_val = s_ms5607.get_altitude();

            printf("----- Temperature: %.2f °C\n", T_val);
            printf("----- Pressure: %.2f mBar\n", P_val);
            printf("----- Altitude: %.2f meter\n", H_val);
        }
        else
            printf("MS5607 Error");
        sleep(1);
    }

#endif
    pac193x pac193x;
    i2c.address = 0x10; // pac193x
    pac193x.i2c = &i2c;
    // pac193x.init();

//     for (uint8_t i = 0; i < 3; i++)
//         printf("pac193x: CH %d direction Voltage: %d Current: %d\n", i + 1, pac193x.get_voltage_drct(i), pac193x.get_current_drct(i));

// sleep(5);

    for (uint8_t i = 0; i < 3; i++)
        printf("pac193x: CH %d\tmean voltage: %f[V]\tmean current: %f[mA]\n", i + 1, pac193x.get_bus_voltage(i, true), pac193x.get_current(i, true));

    i2c.Close();

    printf("MAIN: Done\n");
}
