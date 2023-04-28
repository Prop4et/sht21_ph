#include "sht21.h"
/*i2c instance used with the two defined pins*/
static i2c_inst_t *i2c = i2c0;

/**
 * @brief utility function used to compute the crc after each readings
 * 
 * @param data the readings 
 * @param number_of_bytes numbero of bytes read
 * @return uint8_t the crc value
 */
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

/**
 * @brief utility function used to write to the sht21 registers
 * 
 * @param reg the address of the register to write 
 * @param buf data to write
 * @param nbytes number of bytes to write
 * @return uint8_t 0 if success, -1 otherwise
 */
static uint8_t sht21_write(uint8_t reg, const uint8_t *buf, uint32_t nbytes){
    int num_bytes_read = 0;
    uint8_t msg[nbytes +1];
    int8_t ret = 0;
    /*composes the message, puts the address as the first element to write to the bus*/
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

/**
 * @brief utility function used to read from the sht21 sensor
 * 
 * @param reg the value of the register to be read
 * @param buf buffer holding the read value
 * @param nbytes number of bytes to read
 * @param sleep_time amount of time waited after issuing the command before reading
 * @return uint8_t 0 if success, -1 otherwise
 */
static uint8_t sht21_read(uint8_t reg, uint8_t *buf, uint32_t nbytes, int8_t sleep_time){
    int8_t num_bytes_read = 0;
    int8_t ret = 0;

    if(nbytes < 0){
        return num_bytes_read;
    }
    //issue the command and wait the desired amount of time
    i2c_write_blocking(i2c, SHT21_ADDR, &reg, 1, false);
    sleep_ms(sleep_time);

    int i = 0;
    //try max 20 time if no reading occurs 
    while(i < 20 && num_bytes_read == 0){
        //read from the bus and check that the crc is right
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

/**
 * @brief converts the temperature signal output into temperature [°C] by following the formula in Chapter 6.2 of 
 * https://www.sensirion.com/media/documents/120BBE4C/63500094/Sensirion_Datasheet_Humidity_Sensor_SHT21.pdf
 * 
 * @param buf buffer holding the temperature signal output
 * @return float the value of the temperature in °C
 */
static float get_temperature(uint8_t* buf){
    float temp = 0;
    uint16_t unadjusted = 0;
    unadjusted = (buf[0] << 8) | buf[1];
    unadjusted &= SHT21_STATUS_BITS_MASK;
    temp = -46.85 + 175.72/65536 * (float)unadjusted;
    return temp;
}

/**
 * @brief converts the humidity signal output into humidity [%RH] by following the formula in Chapter 6.1 of 
 * https://www.sensirion.com/media/documents/120BBE4C/63500094/Sensirion_Datasheet_Humidity_Sensor_SHT21.pdf
 * 
 * @param buf buffer holding the humidity signal output
 * @return float the value of the humidity in %RH
 */
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
    if(sda_pin != -1 && scl_pin != -1){
        gpio_set_function(sda_pin, GPIO_FUNC_I2C);
        gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    }else{
        gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    }
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