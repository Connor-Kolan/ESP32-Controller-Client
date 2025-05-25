#include "gpio.h"

#include <stdint.h>

#include "driver/gpio.h"

static gpio_mode_t pin_modes[21] = {0};
static uint8_t pin_map[] = {GPIO_NUM_15, GPIO_NUM_2,  GPIO_NUM_4,  GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_5,  GPIO_NUM_18,
                            GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_3,  GPIO_NUM_1,  GPIO_NUM_22, GPIO_NUM_23, GPIO_NUM_13,
                            GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32};

void init_gpio() {
    uint64_t bitmask = 0;
    bitmask |= (1ULL << GPIO_NUM_2);

    // Init gpio for debug
    gpio_config_t gpio = {
        .pin_bit_mask = bitmask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&gpio);
}

void set_pin_mode(uint32_t pins, gpio_mode_t mode) {
    uint64_t bitmap = 0;
    uint32_t bitmask = 0;
    for (size_t i = 0; i < sizeof(pin_map); i++) {
        bitmask = bitmask << i;
        if (bitmask && pins) {
            bitmap |= (1ULL << pin_map[i]);
            pin_modes[i] = mode;
        }
    }

    gpio_config_t gpio = {
        .pin_bit_mask = bitmap,
        .mode = mode,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&gpio);
}

void get_pin_mode(uint32_t pin) {}