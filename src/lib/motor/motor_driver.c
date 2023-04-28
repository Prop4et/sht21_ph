#include "motor_driver.h"

uint8_t motor_init(struct motor* m, uint8_t pwmpin, uint16_t freq, float dutycycle){
    
    if(pwmpin != -1)
        m->pwm_pin = pwmpin;
    else
        m->pwm_pin = PWM_PIN;
    
    if(dutycycle != -1)
        m->duty_cycle = dutycycle;
    else   
        m->duty_cycle = BASE_DC;

    if(freq != -1){
        m->freq = freq;
        m->clock = 125000000/(WRAP*freq);
    }else{
        m->freq = DEFAULT_FREQ;
        m->clock = CLOCK;
    }
    //initialize pin for pwm
    gpio_set_function(m->pwm_pin, GPIO_FUNC_PWM);
    m->slice_num = pwm_gpio_to_slice_num(m->pwm_pin);
    m->chan_num = pwm_gpio_to_channel(m->pwm_pin);

    //set config
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, WRAP);
    pwm_init(m->slice_num, &config, true);

    //set the frequency to 10Hz
    pwm_set_clkdiv(m->slice_num, m->clock); //determines the frequency of the pwm signal -> 125MHz/(250*50000) 10Hz with these value
    pwm_set_wrap(m->slice_num, WRAP);
    pwm_set_enabled(m->slice_num, true);

    //create a pwm channel on the desired pin with a 50% duty cycle
    pwm_set_gpio_level(m->pwm_pin, 0);
    pwm_set_chan_level(m->slice_num, m->chan_num, WRAP*BASE_DC);
    return 0;
}

void motor_d_cycle(struct motor m){
    pwm_set_chan_level(m.slice_num, m.chan_num, WRAP*m.duty_cycle);
}

void motor_off(struct motor m){
    pwm_set_chan_level(m.slice_num, m.chan_num, 0);
}
