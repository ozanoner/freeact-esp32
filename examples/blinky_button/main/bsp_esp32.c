/**
 * @file bsp_esp32.c
 * @brief Board Support Package implementation for ESP32 BlinkyButton example
 *
 * @details
 * Provides hardware abstraction layer for:
 * - GPIO configuration for LEDs and button
 * - Interrupt handling for button events
 * - LED control functions
 *
 * GPIO Assignments:
 * - LED_RED (GPIO2): LED0 for button feedback
 * - LED_BLUE (GPIO4): LED1 for blinking pattern
 * - BTN_RED (GPIO13): Button input with pull-up
 */

#include "bsp.h"
#include "button_gpio.h"
#include "driver/gpio.h"
#include "iot_button.h"

#define LED_RED GPIO_NUM_18   ///< Red LED (LED0) connected to GPIO2
#define LED_BLUE GPIO_NUM_19  ///< Blue LED (LED1) connected to GPIO4
#define BTN_RED GPIO_NUM_22   ///< Button connected to GPIO13

/**
 * @brief GPIO interrupt handler for button events
 *
 * @param arg Unused parameter (required by ESP-IDF ISR signature)
 *
 * @details
 * Handles button press and release events by:
 * - Reading current GPIO level
 * - Posting appropriate event to BlinkyButton Active Object
 * - Using ISR-safe posting function
 *
 * Events generated:
 * - BUTTON_PRESSED_SIG: When button is pressed (GPIO low)
 * - BUTTON_RELEASED_SIG: When button is released (GPIO high)
 */
static void button_event_cb(void* arg, void* data)
{
    button_event_t event = iot_button_get_event(arg);

    switch (event)
    {
        case BUTTON_PRESS_DOWN:
            static Event const buttonPressedEvt = {BUTTON_PRESSED_SIG};  ///< Button pressed event
            Active_post(AO_blinkyButton, &buttonPressedEvt);
            break;

        case BUTTON_PRESS_UP:
            static Event const buttonReleasedEvt = {BUTTON_RELEASED_SIG};  ///< Button released event
            Active_post(AO_blinkyButton, &buttonReleasedEvt);
            break;

        default:
            break;
    }
}

/**
 * @brief Initialize Board Support Package
 *
 * @details
 * Configures hardware peripherals:
 * - Installs GPIO ISR service
 * - Configures button GPIO as input with pull-up and interrupt
 * - Configures LED GPIOs as outputs
 * - Sets up interrupt handler for button events
 *
 * @note Must be called before BSP_start() and before using any BSP functions
 */
void BSP_init(void)
{
    const button_config_t      btn_cfg      = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num     = BTN_RED,
        .active_level = 0,
    };

    static button_handle_t gpio_btn = NULL;

    iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, NULL, button_event_cb, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, NULL, button_event_cb, NULL);

    // Configure LED
    gpio_reset_pin(LED_RED);                         ///< Reset red LED pin
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);   ///< Set red LED as output
    gpio_reset_pin(LED_BLUE);                        ///< Reset blue LED pin
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);  ///< Set blue LED as output
}

/**
 * @brief Start Board Support Package operations
 *
 * @details
 * Currently no additional operations needed after initialization.
 * Reserved for future BSP startup operations if needed.
 */
void BSP_start(void)
{
    /* nothing to do */
}

/**
 * @brief Turn LED0 (red LED) ON
 *
 * @details
 * Sets GPIO2 to high level to turn on the red LED.
 * Used for button press feedback.
 */
void BSP_led0_on(void)
{
    gpio_set_level(LED_RED, 1);
}

/**
 * @brief Turn LED0 (red LED) OFF
 *
 * @details
 * Sets GPIO2 to low level to turn off the red LED.
 * Used for button release feedback.
 */
void BSP_led0_off(void)
{
    gpio_set_level(LED_RED, 0);
}

/**
 * @brief Turn LED1 (blue LED) ON
 *
 * @details
 * Sets GPIO4 to high level to turn on the blue LED.
 * Used for blinking pattern display.
 */
void BSP_led1_on(void)
{
    gpio_set_level(LED_BLUE, 1);
}

/**
 * @brief Turn LED1 (blue LED) OFF
 *
 * @details
 * Sets GPIO4 to low level to turn off the blue LED.
 * Used for blinking pattern display.
 */
void BSP_led1_off(void)
{
    gpio_set_level(LED_BLUE, 0);
}