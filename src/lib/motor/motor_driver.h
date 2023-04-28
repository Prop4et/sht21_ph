#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define DEFAULT_FREQ            10
#define PWM_PIN                 22
#define BASE_DC                 0.5
#define WRAP                    50000
#define CLOCK                   250
/*
With wrap = 50000 and clock = 250 -> freq = 10
freq = 125MHz/(WRAP*CLOCK) = 125000000/(50000*250) = 125000000/12500000 = 10
*/
struct motor{
    uint8_t pwm_pin;
    uint slice_num;
    uint chan_num;
    uint16_t freq;
    uint16_t clock;
    float duty_cycle;
};

/**
 * @brief initializes the parameters for the motor
 * 
 * @param m the struct holding the parameters
 * @param pwmpin pwm pin
 * @param freq frequency on the pwm
 * @param dutycycle dutycycle
 * @return uint8_t 0 if success, -1 otherwise
 */
uint8_t motor_init(struct motor* m, uint8_t pwmpin, uint16_t freq, float dutycycle);
/**
 * @brief set a new dutycycle
 * 
 * @param m the structure holding the motor parameters
 */
void motor_d_cycle(struct motor m);

/**
 * @brief puts the dutycycle to 0
 * 
 * @param m the structure holding the motor parameters
 */
void motor_off(struct motor m);