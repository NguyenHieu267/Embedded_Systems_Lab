
/*
 * Exercise 1: Asymmetric LED Blink
 * Board: BKIT ARM4 - STM32F407
 * LED3 (PE4): ON 1 second, OFF 2 seconds
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Get LED3 from app.overlay */
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Thread configuration */
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* LED blinking thread */
void thread_led_blinky(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    /* Check GPIO device */
    if (!gpio_is_ready_dt(&led)) {
        printk("LED GPIO is not ready!\n");
        return;
    }

    /* Configure LED3 as output, initially OFF */
    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
        printk("Failed to configure LED3!\n");
        return;
    }

    printk("Exercise 1: Asymmetric LED Blink\n");

    while (1) {
        /* Turn LED3 ON */
        gpio_pin_set_dt(&led, 1);
        printk("LED3 ON - 1 second\n");
        k_msleep(1000);

        /* Turn LED3 OFF */
        gpio_pin_set_dt(&led, 0);
        printk("LED3 OFF - 2 seconds\n");
        k_msleep(2000);
    }
}

/* Create and start the LED thread automatically */
K_THREAD_DEFINE(
    led_thread_id,
    STACK_SIZE,
    thread_led_blinky,
    NULL, NULL, NULL,
    THREAD_PRIORITY,
    0,
    0
);

int main(void)
{
    /* main() does not control the LED */
    return 0;
}
