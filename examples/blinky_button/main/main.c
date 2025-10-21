/* Interrupt Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>

#include "FreeAct.h"
#include "bsp.h"

/* The BlinkyButton AO */
typedef struct
{
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    bool      isLedOn;
} BlinkyButton;

void BlinkyButton_ctor(BlinkyButton* const me);

static void BlinkyButton_dispatch(BlinkyButton* const me, Event const* const e)
{
    switch (e->sig)
    {
        case INIT_SIG: /* intentionally fall through... */
        case TIMEOUT_SIG:
        {
            if (!me->isLedOn)
            { /* LED not on */
                BSP_led1_on();
                me->isLedOn = true;
                TimeEvent_arm(&me->te, pdMS_TO_TICKS(200));
            }
            else
            { /* LED is on */
                BSP_led1_off();
                me->isLedOn = false;
                TimeEvent_arm(&me->te, pdMS_TO_TICKS(800));
            }
            break;
        }
        case BUTTON_PRESSED_SIG:
        {
            BSP_led0_on();
            break;
        }
        case BUTTON_RELEASED_SIG:
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

static StackType_t  blinkyButton_stack[configMINIMAL_STACK_SIZE]; /* task stack */
static Event*       blinkyButton_queue[10];
static BlinkyButton blinkyButton;
Active*             AO_blinkyButton = &blinkyButton.super;

void app_main()
{
    BSP_init(); /* initialize the BSP */

    /* create and start the BlinkyButton AO */
    BlinkyButton_ctor(&blinkyButton);
    Active_start(AO_blinkyButton,                                             ///< Active object to start
                 1U,                                                          ///< Task priority (1-based)
                 blinkyButton_queue,                                          ///< event queue storage
                 sizeof(blinkyButton_queue) / sizeof(blinkyButton_queue[0]),  ///< queue length
                 blinkyButton_stack,                                          ///< stack storage
                 sizeof(blinkyButton_stack),                                  ///< stack size in bytes
                 0U);

    BSP_start(); /* configure and start interrupts */
}

/*

void button_config()
{
    gpio_install_isr_service(0);
    printf("configuring button\n");
    gpio_reset_pin(BTN_RED);
    gpio_set_direction(BTN_RED, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN_RED);
    gpio_set_intr_type(BTN_RED, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(BTN_RED, gpio_isr_handler, NULL);
    printf("config complete\n");
}

void led_config()
{
    gpio_reset_pin(LED_RED);
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
}

void app_main()
{
    uint8_t led_value = 0;

    button_config();
    led_config();

    while (1)
    {
        if (button_pressed)
        {
            printf("*\n");
            button_pressed = false;
            led_value      = !led_value;
            gpio_set_level(LED_RED, led_value);
        }
        vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
    }
}

*/