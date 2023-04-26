#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <string.h>
#define SHT21_ADDR              UINT8_C(0x40)
#define SHT21_SOFT_RESET        UINT8_C(0xFE)
#define SHT21_T_HOLD            UINT8_C(0xE3)
#define SHT21_H_HOLD            UINT8_C(0xE5)
#define SHT21_T_NO_HOLD         UINT8_C(0xF3)
#define SHT21_H_NO_HOLD         UINT8_C(0xF5)
#define SHT21_STATUS_BITS_MASK  UINT8_C(0xFFFC) 

#define POLYNOMIAL              UINT16_C(0x131)

#define SHT21_WAIT_T            UINT32_C(85)
#define SHT21_WAIT_H            UINT32_C(22)
#define SHT21_WAIT_SOFT_RESET   UINT32_C(50)

typedef union {
  float f;
  uint8_t b[4];
} unionfloat_t;

uint8_t sht21_init(uint8_t sda_pin, uint8_t scl_pin);
float temperature();
float humidity();
