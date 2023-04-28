#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../../lib/ph_driver/ph_driver.h"
#include "../../lib/sht21/sht21.h"
#include "../../lib/motor/motor_driver.h"

#define DEV_ID               8
#define CHECKMASK            UINT8_C(1)

struct uplink {
    uint16_t id; 
    float temp;
    float hum;
    float ph[3];
};

int main(){
    stdio_init_all();
    struct ph_driver ph;
    struct motor m;
    struct uplink pkt = {
        .id = DEV_ID
    };

    motor_init(&m, -1, -1, -1);
    ph_init(&ph, NULL, -1, -1);
    sht21_init(-1, -1);
    float ph_buf[N_SENS] = {0, 0, 0};
    pkt.temp = sht21_temperature();
    pkt.hum = sht21_humidity();
    uint8_t bitmask = ph_get(ph, m, ph_buf);
    for(int i = 0; i<N_SENS; i++){
        if(bitmask & CHECKMASK)
            pkt.ph[i] = ph_buf[i];
        else
            pkt.ph[i] = -1000;
        bitmask = bitmask >> 1;
    }
    //TODO: LoRa protocol and check the pins used
}