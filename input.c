#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gpiolib.h"

int main(int argc, char **argv)  {
    int gpio_pin = 26;

    gpio_export(gpio_pin);    
    gpio_direction(gpio_pin, 0);

    while(1) {
        printf(">> GPIO %d ON\n", gpio_read(26));

        sleep(1);
    }
    
    return 0;
}
