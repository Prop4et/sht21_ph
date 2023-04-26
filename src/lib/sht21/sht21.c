#include "sht21.h"
static i2c_inst_t *i2c = i2c0;

static uint8_t calculate_checksum(uint8_t* data, uint32_t number_of_bytes){
    uint8_t crc = 0;
    //calculates 8-Bit checksum with given polynomial
    for (int byteCtr = 0; byteCtr < number_of_bytes; byteCtr++){
        crc ^= data[byteCtr];
        for (int bit = 8; bit > 0; bit--){
            if(crc & 0x80)
                crc = (crc << 1) ^ POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

static uint8_t sht21_write(uint8_t reg, const uint8_t *buf, uint32_t nbytes){
    int num_bytes_read = 0;
    uint8_t msg[nbytes +1];
    int8_t ret = 0;

    msg[0] = reg;
    for(int i = 0; i < nbytes; i++){
        msg[i+1] = buf[i];
    }

    ret = i2c_write_blocking(i2c, SHT21_ADDR, msg, (nbytes+1), false);
    if(ret > 0)
        return 0;
    else
        return -1;
}

static uint8_t sht21_read(uint8_t reg, uint8_t *buf, uint32_t nbytes, int8_t sleep_time){
    int8_t num_bytes_read = 0;
    int8_t ret = 0;

    if(nbytes < 0){
        return num_bytes_read;
    }
    i2c_write_blocking(i2c, SHT21_ADDR, &reg, 1, false);
    sleep_ms(sleep_time);
    int i = 0;
    while(i < 20 && num_bytes_read == 0){
        num_bytes_read = i2c_read_blocking(i2c, SHT21_ADDR, buf, nbytes, false);
        if(num_bytes_read != 0){
            if(calculate_checksum(buf, 2) != 0)
                num_bytes_read = -1;
        }else{
            sleep_ms(sleep_time);
        }
        i += 1;
    }
    
    if(num_bytes_read > 0)
        return num_bytes_read;
    else 
        return -1;
}

static float get_temperature(uint8_t* buf){
    float temp = 0;
    uint16_t unadjusted = 0;
    unadjusted = (buf[0] << 8) | buf[1];
    unadjusted &= SHT21_STATUS_BITS_MASK;
    temp = -46.85 + 175.72/65536 * (float)unadjusted;
    return temp;
}

static float get_humidity(uint8_t* buf){
    float hum = 0;
    uint32_t unadjusted = 0;
    unadjusted = (buf[0] << 8) | buf[1];
    unadjusted &= SHT21_STATUS_BITS_MASK;
    hum = -6.0 + 125.0/65536 * (float)unadjusted;
    return hum;
}

uint8_t sht21_init(uint8_t sda_pin, uint8_t scl_pin){
    i2c_init(i2c, 400*1000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    uint8_t ret = sht21_write(SHT21_SOFT_RESET, NULL, 0);
    sleep_ms(SHT21_WAIT_SOFT_RESET);
    return ret;
}

float temperature(){
    uint8_t ret = 0;
    uint8_t buf[3];
    ret = sht21_read(SHT21_T_NO_HOLD, buf, 3, SHT21_WAIT_T);
    if(ret == -1) return -1000;
    if(calculate_checksum(buf, 2) == buf[2]){
        return get_temperature(buf);
    }
    return -1000;
}

float humidity(){
    uint8_t ret = 0;
    uint8_t buf[3];
    ret = sht21_read(SHT21_H_NO_HOLD, buf, 3, SHT21_WAIT_H);
    if(ret == -1) return -1000;
    if(calculate_checksum(buf, 2) == buf[2]){
        return get_humidity(buf);
    }
    return -1000;
}