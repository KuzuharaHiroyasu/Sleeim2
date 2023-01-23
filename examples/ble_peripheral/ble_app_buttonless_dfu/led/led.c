#include "led.h"

void led_on(void)
{
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,10), 1); // LED High
}

void led_off(void)
{
    nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,10), 0); // LED Low
}