#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "ph_driver.h"

int main(){
    stdio_init_all();
    struct ph_driver ph;
    ph_init(&ph);
}