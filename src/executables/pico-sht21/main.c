#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../../lib/sht21/sht21.h"

int main(){
    stdio_init_all();
    uint8_t ret = sht21_init(8, 9);
    printf("Init: %u\n", ret);
    printf("Temperature: %.2f\n", temperature());
    printf("Humidity: %.2f\n", humidity());


}