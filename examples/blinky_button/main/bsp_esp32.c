/**
 * @file bsp_esp32.c
 * @brief Board Support Package implementation for ESP32 BlinkyButton example
 *
 * @details
 * Provides hardware abstraction layer for:
 * - GPIO configuration for LEDs and button
 * - Button handling using ESP-IDF iot_button component
 * - LED control functions
 *
 * GPIO Assignments:
 * - LED_RED (GPIO18): LED0 for button feedback
 * - LED_BLUE (GPIO19): LED1 for blinking pattern
 * - BTN_RED (GPIO22): Button input with pull-up (active low)
 */

#include "bsp.h"
#include "button_gpio.h"
#include "driver/gpio.h"
#include "iot_button.h"

#define LED_RED CONFIG_EXAMPLE_BLINKY_BUTTON_GPIO_LED_RED    ///< Red LED (LED0) connected to GPIO18
#define LED_BLUE CONFIG_EXAMPLE_BLINKY_BUTTON_GPIO_LED_BLUE  ///< Blue LED (LED1) connected to GPIO19
#define BTN_RED CONFIG_EXAMPLE_BLINKY_BUTTON_GPIO_BUTTON     ///< Button connected to GPIO22 (active low)

/**
 * @brief Button event callback handler using ESP-IDF iot_button component
 *
 * @param arg Button handle (iot_button instance)
 * @param data User data (unused)
 *
 * @details
 * Handles button press and release events using ESP-IDF's iot_button component:
 * - Gets button event from iot_button API
 * - Posts appropriate event to BlinkyButton Active Object
 * - Uses task-safe posting function (not ISR context)
 *
 * Events generated:
 * - BUTTON_PRESSED_SIG: On BUTTON_PRESS_DOWN event
 * - BUTTON_RELEASED_SIG: On BUTTON_PRESS_UP event
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
 * Configures hardware peripherals using ESP-IDF iot_button component:
 * - Creates GPIO button device with active-low configuration
 * - Registers callbacks for button press/release events
 * - Configures LED GPIOs as outputs
 *
 * Button configuration:
 * - GPIO22 with active low level (pressed = 0V)
 * - Internal pull-up resistor enabled by iot_button component
 * - Debouncing handled by iot_button component
 *
 * @note Must be called before BSP_start() and before using any BSP functions
 */
void BSP_init(void)
{
    const button_config_t      btn_cfg      = {0};  ///< Default button config
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num     = BTN_RED,  ///< GPIO22 for button
        .active_level = 0,        ///< Active low (pressed = 0V)
    };

    static button_handle_t gpio_btn = NULL;  ///< Button handle

    iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);                    ///< Create button device
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, NULL, button_event_cb, NULL);  ///< Register press callback
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, NULL, button_event_cb, NULL);    ///< Register release callback

    // Configure LED
    gpio_reset_pin(LED_RED);                         ///< Reset red LED pin (GPIO18)
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);   ///< Set red LED as output
    gpio_reset_pin(LED_BLUE);                        ///< Reset blue LED pin (GPIO19)
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
 * Sets GPIO18 to high level to turn on the red LED.
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
 * Sets GPIO18 to low level to turn off the red LED.
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
 * Sets GPIO19 to high level to turn on the blue LED.
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
 * Sets GPIO19 to low level to turn off the blue LED.
 * Used for blinking pattern display.
 */
void BSP_led1_off(void)
{
    gpio_set_level(LED_BLUE, 0);
}