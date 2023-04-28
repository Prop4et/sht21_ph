#include "ph_driver.h"

void ph_init(struct ph_driver *dev, float* coeff, uint8_t mux1, uint8_t mux2){
    adc_init();
    adc_gpio_init(ADC_PIN); //cannot initialize gp 26 since it is used by the sx1262 module. (12 bits precision)
    adc_select_input(1);
    
    //motor = motor driver or something like that
    
    /*i cannot create an object, i can assign pins*/
    if(mux1 != -1 && mux2 != -1){
        dev->spin1 = mux1;
        dev->spin2 = mux2;
    }else{
        dev->spin1 = MUX_PIN1;//TODO
        dev->spin2 = MUX_PIN2;//TODO
    }

    if(coeff == NULL){
        for(int i=0; i<N_SENS; i++)
            dev->coeffAdj[i] = BASE_ADJ_COEFF;
    }else{
        for(int i=0; i<N_SENS; i++)
            dev->coeffAdj[i] = BASE_ADJ_COEFF;
    }
    /*
    sets the mux pin in output
    0 | 0 reads sensor 0
    0 | 1 reads sensor 1
    1 | 0 reads sensor 2
    */
    gpio_set_dir(dev->muxpin1, GPIO_OUT);
    gpio_set_dir(dev->muxpin2, GPIO_OUT);
}

/**
 * @brief changes the mux output by changing the input to the pins
 * 
 * @param spin1 spin1 of the mux
 * @param spin2 spin2 of the mux
 * @param n_sensor number of the sensor interested [0, 1, 2]
 */
static void switchSensor(uint8_t spin1, uint8_t spin2, uint8_t n_sensor){
    switch(n_sensor){
        case 0:
            gpio_put(spin1, 0);
            gpio_put(spin2, 0);
        break;
        case 1:
            gpio_put(spin1, 0);
            gpio_put(spin2, 1);
        break;
        case 2:
            gpio_put(spin1, 1);
            gpio_put(spin2, 0);
        break;
    }
}

/**
 * @brief returns a stable reading for a ph
 * 
 * @param analog_value analog value read from the sensor
 * @param adj_coeff adjustment coefficient
 * @param buf the array holding the readings
 * @param buf_index position in the array to fill
 * @return float 1 if reading success, 0 otherwise
 */
static uint8_t stablePh(uint16_t analog_value, float adj_coeff, float* buf, int buf_index){
    uint16_t prev_analog_value = analog_value;
    analog_value = adc_read();
    uint8_t cnt = 0;
    while((abs(prev_analog_value - analog_value) > 7) || (cnt < 12)){
        sleep_ms(10000);
        prev_analog_value = analog_value;
        analog_value = adc_read();
        cnt++;
    }
    buf[buf_index] = ((((float)analog_value/1000) - 4.52)/-0.216)-adj_coeff;
    return cnt >= 12 ? 0 : 1;
}

uint8_t ph_get(struct ph_driver dev, struct motor m, float* buf){
    //set the array to 0
    memset(buf, 0, 3);
    uint8_t meas_done[N_SENS] = {0, 0, 0};
    uint16_t analog_reads[N_SENS] = {0, 0, 0};
    uint32_t start = -480000001;
    uint8_t bitmask = 0; //bitmask signaling which readings are done (3 bits, s3s2s1 -> so if only s2 read => 010 = 2)
    while(time_us_32() - start < 480000000){
        for(int i=0; i<N_SENS; i++){
            switchSensor(dev.spin1, dev.spin2, i);
            analog_reads[i] = adc_read();
            sleep_ms(1000);
        }
        //1:1 what the python code does
        motor_d_cycle(m);

        for(uint8_t i = 0; i<N_SENS; i++){
            if(analog_reads[i] > 900 && meas_done[i] == 0){
                motor_off(m);
                switchSensor(dev.spin1, dev.spin2, i);
                meas_done[i] = stablePh(analog_reads[i], dev.coeffAdj[i], buf, i);
                if(meas_done[i])
                    bitmask |= (1 << i);
            }
        }
    }
    motor_off(m);
    return bitmask;
}
