#include "pico/stdlib.h"
#include "stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "../motor/motor_driver.h"
#include <string.h>

#define BASE_ADJ_COEFF          1.3
#define N_SENS                  3
#define ADC_PIN                 27
#define MUX_PIN1                10
#define MUX_PIN2                11
/**
 * @brief Initializes the adc pin to read
 */

struct ph_driver{
    uint8_t spin1;
    uint8_t spin2;
    float coeffAdj[N_SENS];
};

/**
 * @brief initializes the ph sensor by initializing the gpio27 pin for adc readings and the structure
 * 
 * @param dev structure holding the parameters using during the measurements 
 * @param coeff1 coefficient of adjustement 1, default value is 1.3 if -1 is passed
 * @param coeff2 coefficient of adjustement 2, default value is 1.3 if -1 is passed
 * @param coeff2 coefficient of adjustement 3, default value is 1.3 if -1 is passed
 * @param mux1 first pin for the multiplexer
 * @param mux2 second pin for the multiplexer
 * @param dutycycle dutycycle for the pump (i guess)
 */
void ph_init(struct ph_driver *dev, float *coeff, uint8_t mux1, uint8_t mux2);

/**
 * @brief computes the values for the ph for each sensor
 * 
 * @param dev structure holding the different values
 * @param m structure holding the motor parameters
 * @param buf buffer for the ph results
 * 
 * @return uint8_t is an 8-bit mask used to determine which sensor read the ph 
 * 111 -> all sensors
 * 100 -> 3rd sensor
 * 010 -> 2nd sensor
 * 001 -> 1st sensor
 * (or) combinations determine the rest 
 */
uint8_t ph_get(struct ph_driver dev, struct motor m, float* buf /*len is N_SENS*/);