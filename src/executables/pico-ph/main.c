#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../../lib/ph_driver/ph_driver.h"
#include "../../lib/sht21/sht21.h"
#include "../../lib/motor/motor_driver.h"

int main(){
    stdio_init_all();
    struct ph_driver ph;
    struct motor m;
    motor_init(&m, -1, -1, -1);
    ph_init(&ph, NULL, -1, -1);
    sht21_init(-1, -1);
    
}