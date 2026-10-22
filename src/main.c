/*
* BKIT ARM4 - Lab 1: Blinky with Zephyr RTOS
* Blink LED3 (DEBUG LED, pin PE4): ON for 1 second, OFF for 1 second.
*/
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
/* Time between two LED toggles (ms) */
#define SLEEP_TIME_MS 3000
/* Get the LED node from the "led0" alias in the devicetree (app.overlay) */
#define LED0_NODE DT_ALIAS(led0)
/* gpio_dt_spec bundles everything: port (GPIOE), pin (4), flags (ACTIVE_HIGH) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
int main(void)
{
int ret;
bool led_state = true;
/* Check that the GPIO port driver is ready */
if (!gpio_is_ready_dt(&led)) {
printf("Error: LED GPIO device is not ready\n");
return 0;
}
/* Configure the pin as an output, initial state: LED on */
ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
if (ret < 0) {
printf("Error: failed to configure LED pin (%d)\n", ret);
return 0;
}
printf("BKIT ARM4 - Zephyr Blinky started!\n");
while (1) {
printf("LED3: %s\n", led_state ? "ON" : "OFF");
k_msleep(SLEEP_TIME_MS); /* Yield the CPU for 1000 ms */
ret = gpio_pin_toggle_dt(&led); /* Toggle the LED */
if (ret < 0) {
return 0;
}
led_state = !led_state;
}
return 0;
}