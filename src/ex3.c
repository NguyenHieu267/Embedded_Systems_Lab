
/*
 * Exercise 3: Independent Timing Cycles
 * Board: BKIT ARM4 - STM32F407
 *
 * Thread 1:
 * LED3 ON 1.5s, OFF 0.5s
 *
 * Thread 2:
 * Y0 ON, Y1 OFF: 1s
 * Y0 OFF, Y1 ON: 2s
 * Y0 ON, Y1 ON:  2s
 *
 * Both threads run independently.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Devicetree aliases */
#define LED3_NODE DT_ALIAS(led0)
#define Y0_NODE   DT_ALIAS(y0)
#define Y1_NODE   DT_ALIAS(y1)

/* GPIO specifications */
static const struct gpio_dt_spec led3 =
    GPIO_DT_SPEC_GET(LED3_NODE, gpios);

static const struct gpio_dt_spec y0 =
    GPIO_DT_SPEC_GET(Y0_NODE, gpios);

static const struct gpio_dt_spec y1 =
    GPIO_DT_SPEC_GET(Y1_NODE, gpios);

/* Thread configuration */
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* Thread 1: LED3 blinking */
void thread_led_blinky(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    if (!gpio_is_ready_dt(&led3)) {
        printk("LED3 GPIO not ready!\n");
        return;
    }

    if (gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE) < 0) {
        printk("LED3 configuration failed!\n");
        return;
    }

    while (1) {
        /* LED3 ON for 1.5 seconds */
        gpio_pin_set_dt(&led3, 1);
        printk("LED3: ON (1.5s)\n");
        k_msleep(1500);

        /* LED3 OFF for 0.5 seconds */
        gpio_pin_set_dt(&led3, 0);
        printk("LED3: OFF (0.5s)\n");
        k_msleep(500);
    }
}

/* Thread 2: Y0 and Y1 output sequence */
void thread_output_sequence(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    if (!gpio_is_ready_dt(&y0) ||
        !gpio_is_ready_dt(&y1)) {
        printk("Y0/Y1 GPIO not ready!\n");
        return;
    }

    if (gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE) < 0 ||
        gpio_pin_configure_dt(&y1, GPIO_OUTPUT_INACTIVE) < 0) {
        printk("Y0/Y1 configuration failed!\n");
        return;
    }

    while (1) {
        /* State 1: Only Y0 ON for 1 second */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 0);
        printk("Y0: ON | Y1: OFF (1s)\n");
        k_msleep(1000);

        /* State 2: Only Y1 ON for 2 seconds */
        gpio_pin_set_dt(&y0, 0);
        gpio_pin_set_dt(&y1, 1);
        printk("Y0: OFF | Y1: ON (2s)\n");
        k_msleep(2000);

        /* State 3: Both Y0 and Y1 ON for 2 seconds */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 1);
        printk("Y0: ON | Y1: ON (2s)\n");
        k_msleep(2000);
    }
}

/* Create LED3 thread */
K_THREAD_DEFINE(
    led_thread_id,
    STACK_SIZE,
    thread_led_blinky,
    NULL, NULL, NULL,
    THREAD_PRIORITY,
    0,
    0
);

/* Create Y0/Y1 thread */
K_THREAD_DEFINE(
    output_thread_id,
    STACK_SIZE,
    thread_output_sequence,
    NULL, NULL, NULL,
    THREAD_PRIORITY,
    0,
    0
);

int main(void)
{
    /* Both threads run independently */
    return 0;
}
