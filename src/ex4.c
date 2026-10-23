
/*
 * Exercise 4: Parameterized LED Blinking
 * Board: BKIT ARM4 - STM32F407
 *
 * LED3: ON 2000 ms, OFF 2000 ms
 * Y0:   ON  500 ms, OFF  500 ms
 * Y1:   ON 3000 ms, OFF 1000 ms
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Devicetree aliases */
#define LED3_NODE DT_ALIAS(led0)
#define Y0_NODE   DT_ALIAS(y0)
#define Y1_NODE   DT_ALIAS(y1)

/* Configuration structure */
struct blink_cfg {
    struct gpio_dt_spec led;
    int on_ms;
    int off_ms;
};

/* Configuration for each LED */
static struct blink_cfg led3_cfg = {
    .led = GPIO_DT_SPEC_GET(LED3_NODE, gpios),
    .on_ms = 2000,
    .off_ms = 2000
};

static struct blink_cfg y0_cfg = {
    .led = GPIO_DT_SPEC_GET(Y0_NODE, gpios),
    .on_ms = 500,
    .off_ms = 500
};

static struct blink_cfg y1_cfg = {
    .led = GPIO_DT_SPEC_GET(Y1_NODE, gpios),
    .on_ms = 3000,
    .off_ms = 1000
};

/*
 * One thread function is enough because all three LEDs
 * perform the same blinking logic, with only their GPIO
 * pins and ON/OFF durations differing.
 */
void thread_blink(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    /* Receive configuration through p1 */
    struct blink_cfg *cfg = (struct blink_cfg *)p1;

    /* Check GPIO device */
    if (!gpio_is_ready_dt(&cfg->led)) {
        printk("GPIO device not ready!\n");
        return;
    }

    /* Configure GPIO as output, initially OFF */
    if (gpio_pin_configure_dt(
            &cfg->led, GPIO_OUTPUT_INACTIVE) < 0) {
        printk("GPIO configuration failed!\n");
        return;
    }

    while (1) {
        /* Turn LED ON */
        if (gpio_pin_set_dt(&cfg->led, 1) < 0) {
            printk("Failed to turn LED ON!\n");
            return;
        }

        k_msleep(cfg->on_ms);

        /* Turn LED OFF */
        if (gpio_pin_set_dt(&cfg->led, 0) < 0) {
            printk("Failed to turn LED OFF!\n");
            return;
        }

        k_msleep(cfg->off_ms);
    }
}

/* Thread 1: LED3 */
K_THREAD_DEFINE(
    led3_thread_id,
    1024,
    thread_blink,
    &led3_cfg, NULL, NULL,
    5, 0, 0
);

/* Thread 2: Y0 */
K_THREAD_DEFINE(
    y0_thread_id,
    1024,
    thread_blink,
    &y0_cfg, NULL, NULL,
    5, 0, 0
);

/* Thread 3: Y1 */
K_THREAD_DEFINE(
    y1_thread_id,
    1024,
    thread_blink,
    &y1_cfg, NULL, NULL,
    5, 0, 0
);

int main(void)
{
    /* LED control is handled by three threads */
    return 0;
}
