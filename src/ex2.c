
/*
 * Exercise 2: Two Threads with Shared Variable
 * Board: BKIT ARM4 - STM32F407
 *
 * LED3: ON 2 seconds, OFF 2 seconds
 * Y0: Follows LED3 through shared atomic variable
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/printk.h>

/* Devicetree aliases */
#define LED3_NODE DT_ALIAS(led0)
#define Y0_NODE   DT_ALIAS(y0)

/* GPIO devices */
static const struct gpio_dt_spec led3 =
    GPIO_DT_SPEC_GET(LED3_NODE, gpios);

static const struct gpio_dt_spec y0 =
    GPIO_DT_SPEC_GET(Y0_NODE, gpios);

/* Thread configuration */
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* Shared variable: 0 = OFF, 1 = ON */
static atomic_t led_state = ATOMIC_INIT(0);

/* Thread 1: Controls LED3 */
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
        /* LED3 ON */
        if (gpio_pin_set_dt(&led3, 1) < 0) {
            printk("LED3 ON failed!\n");
            return;
        }

        atomic_set(&led_state, 1);
        printk("LED3 ON\n");
        k_msleep(2000);

        /* LED3 OFF */
        if (gpio_pin_set_dt(&led3, 0) < 0) {
            printk("LED3 OFF failed!\n");
            return;
        }

        atomic_set(&led_state, 0);
        printk("LED3 OFF\n");
        k_msleep(2000);
    }
}

/* Thread 2: Y0 follows LED3 */
void thread_y0_follow(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    if (!gpio_is_ready_dt(&y0)) {
        printk("Y0 GPIO not ready!\n");
        return;
    }

    if (gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE) < 0) {
        printk("Y0 configuration failed!\n");
        return;
    }

    int previous_state = -1;

    while (1) {
        int current_state = atomic_get(&led_state);

        /* Update Y0 only when LED3 state changes */
        if (current_state != previous_state) {
            if (gpio_pin_set_dt(&y0, current_state) < 0) {
                printk("Y0 update failed!\n");
                return;
            }

            previous_state = current_state;
            printk("Y0: %s\n", current_state ? "ON" : "OFF");
        }

        /* Poll shared variable every 10 ms */
        k_msleep(10);
    }
}

/* Create two threads */
K_THREAD_DEFINE(
    led_thread_id,
    STACK_SIZE,
    thread_led_blinky,
    NULL, NULL, NULL,
    THREAD_PRIORITY,
    0,
    0
);

K_THREAD_DEFINE(
    y0_thread_id,
    STACK_SIZE,
    thread_y0_follow,
    NULL, NULL, NULL,
    THREAD_PRIORITY,
    0,
    0
);

int main(void)
{
    /* LED control is handled by the two threads */
    return 0;
}
