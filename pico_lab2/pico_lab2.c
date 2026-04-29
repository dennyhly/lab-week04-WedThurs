#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_SCK 18
#define PIN_TX 19
#define PIN_RX 16
#define PIN_CS 17

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

volatile struct {
    unsigned int pico : 4;
    unsigned int ices : 4;
} leds;

volatile struct {
    unsigned int num : 3;
} frame;

int main()
{
    stdio_init_all();
    adc_init();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_TX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_RX, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);

    gpio_init(6);
    gpio_init(7);
    gpio_init(8);
    gpio_init(9);
    gpio_init(10);
    gpio_set_dir(6, GPIO_OUT);
    gpio_set_dir(7, GPIO_OUT);
    gpio_set_dir(8, GPIO_OUT);
    gpio_set_dir(9, GPIO_OUT);
    gpio_set_dir(10, GPIO_IN);
    gpio_pull_up(10);
    adc_gpio_init(26);
    adc_select_input(0);

    frame.num = 0;
    leds.pico = 0;
    leds.ices = 0;

    while (true) {
        int adc_value = adc_read(), adc_mapped = map(adc_value, 0, 4095, 1, 10), animation_speed = (11 - adc_mapped) * 100;
        
        bool animation_type = gpio_get(10); //linear or alternate

        if (animation_type) {//linear
            if (frame.num < 4) {leds.pico = (1 << frame.num + 1) - 1; leds.ices = 0;} 
            else {leds.pico = 0xF; leds.ices = (1 << frame.num - 3) - 1;}
        } 
        else {//alternating
            if (frame.num % 2) {leds.pico = 0b1010; leds.ices = 0b1010;} 
            else {leds.pico = 0b0101; leds.ices = 0b0101;}
        }

        for (int i = 0; i < 4; i++) {
            gpio_put(6 + i, (leds.pico >> i) & 0x1);
        }

        uint8_t data = (uint8_t)leds.ices;
        gpio_put(PIN_CS, 0);
        spi_write_blocking(SPI_PORT, &data, 1);
        gpio_put(PIN_CS, 1);
        frame.num++;
        sleep_ms(animation_speed);
    }
}
