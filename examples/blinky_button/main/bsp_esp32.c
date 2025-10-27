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
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_RED GPIO_NUM_2   ///< Red LED (LED0) connected to GPIO2
#define LED_BLUE GPIO_NUM_4  ///< Blue LED (LED1) connected to GPIO4
#define BTN_RED GPIO_NUM_13  ///< Button connected to GPIO13
#define DELAY_TIME 200       ///< General delay time constant (unused)

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
static void button_isr_handler(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio_get_level(BTN_RED) == 0)
    {
        static Event const buttonPressedEvt = {BUTTON_PRESSED_SIG};  ///< Button pressed event
        Active_postFromISR(AO_blinkyButton, &buttonPressedEvt, &xHigherPriorityTaskWoken);
    }
    else
    {
        static Event const buttonReleasedEvt = {BUTTON_RELEASED_SIG};  ///< Button released event
        Active_postFromISR(AO_blinkyButton, &buttonReleasedEvt, &xHigherPriorityTaskWoken);
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
    // Configure button
    gpio_install_isr_service(0);                              ///< Install GPIO ISR service
    gpio_reset_pin(BTN_RED);                                  ///< Reset button pin to default state
    gpio_set_direction(BTN_RED, GPIO_MODE_INPUT);             ///< Set button as input
    gpio_pullup_en(BTN_RED);                                  ///< Enable internal pull-up resistor
    gpio_set_intr_type(BTN_RED, GPIO_INTR_ANYEDGE);           ///< Trigger on both edges
    gpio_isr_handler_add(BTN_RED, button_isr_handler, NULL);  ///< Add ISR handler

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