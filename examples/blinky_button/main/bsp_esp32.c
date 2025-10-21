#include "bsp.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_RED GPIO_NUM_2
#define LED_BLUE GPIO_NUM_4
#define BTN_RED GPIO_NUM_13
#define DELAY_TIME 200

static void button_isr_handler(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio_get_level(BTN_RED) == 0)
    {
        static Event const buttonPressedEvt = {BUTTON_PRESSED_SIG};
        Active_postFromISR(AO_blinkyButton, &buttonPressedEvt, &xHigherPriorityTaskWoken);
    }
    else
    {
        static Event const buttonReleasedEvt = {BUTTON_RELEASED_SIG};
        Active_postFromISR(AO_blinkyButton, &buttonReleasedEvt, &xHigherPriorityTaskWoken);
    }
}

void BSP_init(void)
{
    // Configure button
    gpio_install_isr_service(0);
    gpio_reset_pin(BTN_RED);
    gpio_set_direction(BTN_RED, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN_RED);
    gpio_set_intr_type(BTN_RED, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(BTN_RED, button_isr_handler, NULL);

    // Configure LED
    gpio_reset_pin(LED_RED);
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(LED_BLUE);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);
}

void BSP_start(void)
{
    /* nothing to do */
}

void BSP_led0_on(void)
{
    gpio_set_level(LED_RED, 1);
}

void BSP_led0_off(void)
{
    gpio_set_level(LED_RED, 0);
}

void BSP_led1_on(void)
{
    gpio_set_level(LED_BLUE, 1);
}

void BSP_led1_off(void)
{
    gpio_set_level(LED_BLUE, 0);
}