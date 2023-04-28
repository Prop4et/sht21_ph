#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

int main() {
    stdio_init_all();
    const uint PWM_PIN = 15;
    const uint PWM_FREQ = 10;  // in Hz
    const float DUTY_CYCLE = 0.5;

    gpio_set_function(15, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(15);
    uint chan_num = pwm_gpio_to_channel(15);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 40000);
    //initialize the pwm but don't start it (call pwm_set_enabled to do so)
    pwm_init(slice_num, &config, true);

    /*pwm_set_clkdiv(slice_num, 16.0f);
    pwm_set_wrap(slice_num, 65025);
    pwm_set_enabled(slice_num, true);*/

    pwm_set_clkdiv(slice_num, 250); //determines the frequency of the pwm signal -> 125MHz/(250*40000)
    pwm_set_wrap(slice_num, 40000);
    pwm_set_enabled(slice_num, true);
    
    //create a pwm channel on the desired pin with a 50% duty cycle
    pwm_set_gpio_level(15, 0);
    uint16_t duty_cycle = 32767;
    pwm_set_chan_level(slice_num, chan_num, 20000);
    sleep_ms(10000);
    pwm_set_chan_level(slice_num, chan_num, 0);
    
    return 0;
}
