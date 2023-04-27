#include "ph_driver.h"

void ph_init(struct ph_driver *dev, float* coeff, uint8_t mux1, uint8_t mux2, float dutycycle){
    adc_init();
    adc_gpio_init(27); //cannot initialize gp 26 since it is used by the sx1262 module. (12 bits precision)
    adc_select_input(1);
    
    //motor = motor driver or something like that
    
    /*i cannot create an object, i can assign pins*/
    if(mux1 != -1 && mux2 != -1){
        dev->spin1 = mux1;
        dev->spin2 = mux2;
    }else{
        dev->spin1 = 19;
        dev->spin2 = 20;
    }

    for(int i=0; i<N_SENS; i++){
        if(coeff[i] != -1)
            dev->coeffAdj[i] = coeff[i];
        else
            dev->coeffAdj[i] = BASE_ADJ_COEFF;

    }

    if(dutycycle != -1)
        dev->duty_cycle = dutycycle;
    else   
        dev->duty_cycle = BASE_DC;

    /*
    sets the mux pin in output, i need to understand how the multiplexer works and to check if it is possible to use it like that
    gpio_set_dir(dev->muxpin1, GPIO_OUT);
    gpio_set_dir(dev->muxpin2, GPIO_OUT);
    */
}

static void switchSensor(uint8_t spin1, uint8_t spin2, n_sensor){
    if n_sensor == 0:
        gpio_put(spin1, 0);
        gpio_put(spin2, 0);
    if n_sensor == 1:
        gpio_put(spin1, 0);
        gpio_put(spin2, 1);
    if n_sensor == 2:
        gpio_put(spin1, 1);
        gpio_put(spin2, 0);
}

static float stablePh(uint16_t analog_value, float adj_coeff){
    uint16_t prev_analog_value = analog_value;
    analog_value = adc_read();
    uint8_t cnt = 0;
    while(abs(prev_analog_value - analog_value) > 7 or cnt < 12){
        sleep_ms(10000);
        prev_analog_value = analog_value;
        analog_value = adc_read();
        cnt++;
    }
    return ((((float)analog_value/1000) - 4.52)/-0.216)-adj_coeff;
}

uint8_t ph_get(struct ph_driver dev, float* buf){
    //set the array to 0
    memset(buf, 0, 3);
    uint8_t meas_done[N_SENS] = {0, 0, 0};
    uint16_t analog_reads[N_SENS] = {0, 0, 0}
    uint32_t start = -480000001;
    uint8_t bitmask = 0; //bitmask signaling which readings are done (3 bits, s3s2s1 -> so if only s2 read => 010 = 2)
    while(time_us_32() - start < 480000000){
        for(int i=0; i<N_SENS; i++){
            switchSensor(dev.spin1, dev.spin2, i)
            analog_reads[i] = adc_read();
            sleep_ms(1000);
        }
        //TODO: create the structure holding all the motor shit
        //dev.motor->dutycycle(dev.duty_cycle);
        //compute sensor 1
        for(uint8_t i = 0; i<N_SENS; i++){
            if(analog_reads[i] > 900 && meas_done[i] == 0){
                switchSensor(i);
                buf[i] = stablePh(analog_reads[i], dev.coeffAdj[i]);
                meas_done[i] = 1;
                bitmask |= (1 << i);
            }
        }
    }
    return bitmask;

}
