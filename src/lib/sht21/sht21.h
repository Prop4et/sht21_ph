/*Header file for the sht21 sensor library, specification come from the datasheet
(https://www.sensirion.com/media/documents/120BBE4C/63500094/Sensirion_Datasheet_Humidity_Sensor_SHT21.pdf)*/
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <string.h>

/*Commands to issue to the sensor (no user register)*/
#define SHT21_ADDR              UINT8_C(0x40)
#define SHT21_SOFT_RESET        UINT8_C(0xFE)
#define SHT21_T_HOLD            UINT8_C(0xE3)
#define SHT21_H_HOLD            UINT8_C(0xE5)
#define SHT21_T_NO_HOLD         UINT8_C(0xF3)
#define SHT21_H_NO_HOLD         UINT8_C(0xF5)

/*Mask used to zero the last to bits of each reading*/
#define SHT21_STATUS_BITS_MASK  UINT8_C(0xFFFC) 

/*Polynomial used to compute the crc*/
#define POLYNOMIAL              UINT16_C(0x131)

/*Wait time issued during the commands*/
#define SHT21_WAIT_T            UINT32_C(85)
#define SHT21_WAIT_H            UINT32_C(22)
#define SHT21_WAIT_SOFT_RESET   UINT32_C(50)

/*Default pins*/
#define SDA_PIN                 8
#define SCL_PIN                 9

/**
 * @brief Initialized the sht21 sensor
 * 
 * @param sda_pin gpio pin used for the IIC data line
 * @param scl_pin gpio pin used for the IIC clock line
 * @return uint8_t 0 if ok, -1 if error
 */
uint8_t sht21_init(uint8_t sda_pin, uint8_t scl_pin);

/**
 * @brief returns the temperature reading from the sensor
 * 
 * @return float the temperature value (14 bits precision)
 */
float sht21_temperature();

/**
 * @brief returns the humidity reading from the sensor
 * 
 * @return float the humidity value (12 bits precision)
 */
float sht21_humidity();
