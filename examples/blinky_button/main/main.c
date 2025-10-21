
/**
 * @file main.c
 * @brief BlinkyButton Active Object Example
 *
 * @details
 * This example demonstrates the FreeAct Active Object pattern with:
 * - LED blinking with different on/off periods using TimeEvents
 * - Button handling with immediate LED response
 * - Event-driven state management
 *
 * The example creates a BlinkyButton Active Object that:
 * - Blinks LED1 with 200ms ON / 800ms OFF pattern
 * - Turns LED0 ON when button is pressed
 * - Turns LED0 OFF when button is released
 */

#include "FreeAct.h"
#include "bsp.h"
#include "esp_log.h"

/** @brief Log tag for this module */
#define TAG "app"

/**
 * @brief The BlinkyButton Active Object
 *
 * @details
 * Active Object that handles LED blinking and button events.
 * Inherits from Active base class and adds:
 * - Time event for LED blinking
 * - LED state tracking
 */
typedef struct
{
    Active    super;    ///< Inherit Active base class
    TimeEvent te;       ///< Time event for LED blinking
    bool      isLedOn;  ///< Current LED state (true = ON, false = OFF)
} BlinkyButton;

/**
 * @brief Constructor for BlinkyButton Active Object
 *
 * @param me Pointer to the BlinkyButton instance to initialize
 *
 * @details
 * Initializes the BlinkyButton Active Object by:
 * - Calling Active_ctor() with dispatch handler
 * - Setting up TimeEvent as one-shot type
 * - Constructing TimeEvent with TIMEOUT_SIG
 * - Initializing LED state to OFF
 */
void BlinkyButton_ctor(BlinkyButton* const me);

/**
 * @brief Event dispatch handler for BlinkyButton Active Object
 *
 * @param me Pointer to the BlinkyButton instance
 * @param e Pointer to the event to be processed
 *
 * @details
 * Processes events for the BlinkyButton Active Object:
 * - INIT_SIG: Initial event, starts LED blinking
 * - TIMEOUT_SIG: Timer expired, toggles LED and re-arms timer
 * - BUTTON_PRESSED_SIG: Button pressed, turns LED0 ON
 * - BUTTON_RELEASED_SIG: Button released, turns LED0 OFF
 *
 * LED blinking pattern:
 * - ON period: 200ms
 * - OFF period: 800ms
 */
static void BlinkyButton_dispatch(BlinkyButton* const me, Event const* const e)
{
    switch (e->sig)
    {
        case INIT_SIG:     // Initial event - start LED blinking
        case TIMEOUT_SIG:  // Timer timeout - toggle LED
        {
            if (!me->isLedOn)
            {
                BSP_led1_on();  // LED currently OFF - turn ON and set short timeout
                me->isLedOn = true;
                TimeEvent_arm(&me->te, 200);  // ON period: 200ms
            }
            else
            {
                BSP_led1_off();  // LED currently ON - turn OFF and set long timeout
                me->isLedOn = false;
                TimeEvent_arm(&me->te, 800);  // OFF period: 800ms
            }
            break;
        }
        case BUTTON_PRESSED_SIG:  // Button pressed - turn LED0 ON
        {
            BSP_led0_on();
            break;
        }
        case BUTTON_RELEASED_SIG:  // Button released - turn LED0 OFF
        {
            BSP_led0_off();
            break;
        }
        default:
        {
            break;
        }
    }
}

void BlinkyButton_ctor(BlinkyButton* const me)
{
    Active_ctor(&me->super, (DispatchHandler)&BlinkyButton_dispatch);
    me->te.type = TYPE_ONE_SHOT;
    TimeEvent_ctor(&me->te, TIMEOUT_SIG, &me->super);
    me->isLedOn = false;
}

/** @brief Task stack for BlinkyButton Active Object (2x minimal stack size) */
static StackType_t blinkyButton_stack[configMINIMAL_STACK_SIZE * 2];

/** @brief Event queue storage for BlinkyButton Active Object (10 events) */
static Event* blinkyButton_queue[10];

/** @brief BlinkyButton Active Object instance */
static BlinkyButton blinkyButton;

/** @brief Public pointer to BlinkyButton Active Object */
Active* AO_blinkyButton = &blinkyButton.super;

/**
 * @brief Main application entry point
 *
 * @details
 * Initializes and starts the BlinkyButton example:
 * 1. Initializes the Board Support Package (BSP)
 * 2. Constructs the BlinkyButton Active Object
 * 3. Starts the Active Object with:
 *    - Priority 1 (lowest user priority)
 *    - Event queue of 10 events
 *    - Stack size of 2x configMINIMAL_STACK_SIZE
 * 4. Starts BSP interrupts for button handling
 */
void app_main()
{
    ESP_LOGI(TAG, "BlinkyButton example start");
    BSP_init();  // Initialize the Board Support Package

    BlinkyButton_ctor(&blinkyButton);  // Create and start the BlinkyButton Active Object

    Active_start(AO_blinkyButton,     // Active object to start
                 1U,                  // Task priority (1-based, lowest user priority)
                 blinkyButton_queue,  // Event queue storage array
                 sizeof(blinkyButton_queue) / sizeof(blinkyButton_queue[0]),  // Queue length (10 events)
                 blinkyButton_stack,                                          // Task stack storage array
                 sizeof(blinkyButton_stack),                                  // Stack size in bytes
                 0U);                                                         // Options (unused)

    BSP_start();  // Configure and start BSP interrupts for button handling
}
