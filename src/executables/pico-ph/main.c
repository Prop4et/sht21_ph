#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/lorawan.h"
#include "hardware/watchdog.h"

#include "../../lib/ph_driver/ph_driver.h"
#include "../../lib/sht21/sht21.h"
#include "../../lib/motor/motor_driver.h"

#include "lora-config.h"

#define CHECKMASK            UINT8_C(1)
/**
 * @brief watchdog that goes on a loop to force a reset of the pico
 * 
 */
void software_reset()
{
    watchdog_enable(1, 1);
    while(1);
}
const struct lorawan_sx12xx_settings sx12xx_settings = {
    .spi = {
        .inst = spi1,
        .mosi = 11,
        .miso = 12,
        .sck  = 10,
        .nss = 3
    },
    .reset = 15,
    .busy = 2,
    .dio1 = 20
};

/*
    ABP settings
*/ 
const struct lorawan_abp_settings abp_settings = {
    .device_address = LORAWAN_DEV_ADDR,
    .network_session_key = LORAWAN_NETWORK_SESSION_KEY,
    .app_session_key = LORAWAN_APP_SESSION_KEY,
    .channel_mask = LORAWAN_CHANNEL_MASK
};

/*
    variables for receiving lora downlinks (if any)
*/
int receive_length = 0;
uint8_t receive_buffer[242];
uint8_t receive_port = 0;

/*
    Structure of the uplink, total of 12 Bytes
*/
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
    
    lorawan_debug(true);
    if (lorawan_erase_nvm() < 0) {
        printf("Cannot erase NVM\n");
    } else {
        printf("Success!\n");
    }

    if (lorawan_init_abp(&sx12xx_settings, LORAWAN_REGION, &abp_settings) < 0) {
        printf("Fail, restarting\n");
        software_reset();
    }
    
    lorawan_join();
    while (!lorawan_is_joined()) {
        lorawan_process();
    }
    lorawan_process_timeout_ms(1000);

    sht21_init(-1, -1);
    motor_init(&m, -1, -1, -1);
    ph_init(&ph, NULL, -1, -1);
    float ph_buf[N_SENS] = {0, 0, 0};
    while(1){
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

        if (lorawan_send_unconfirmed(&pkt, sizeof(struct uplink), 2) < 0) {
            printf("Failed sending\n");
        }else{ 
            printf("Sending ok\n");
        }
        lorawan_process_timeout_ms(3000);
        receive_length = lorawan_receive(receive_buffer, sizeof(receive_buffer), &receive_port);
        if (receive_length > -1) {
            printf("received a %d byte message on port %d: ", receive_length, receive_port);
            for (int i = 0; i < receive_length; i++) {
                printf("%02x", receive_buffer[i]);
            }
            printf("\n");
        }
        sleep_ms(3600000);//1hr
    }
}