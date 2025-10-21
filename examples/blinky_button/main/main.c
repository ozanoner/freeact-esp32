/*
 * Lab Project: BlinkyButton/Button with RTOS (FreeRTOS) and blocking
 * Board: ESP32*
 *
 * Copyright (C) 2005-2020 Quantum Leaps, LLC. All rights reserved.
 *
 * Contact information:
 * <www.state-machine.com>
 * <info@state-machine.com>
 */

#include "FreeAct.h"
#include "bsp.h"
#include "esp_log.h"

#define TAG "app"

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
            }
            else
            { /* LED is on */
                BSP_led1_off();
                me->isLedOn = false;
            }
            TimeEvent_arm(&me->te, 1000);
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

static StackType_t  blinkyButton_stack[configMINIMAL_STACK_SIZE * 2]; /* task stack */
static Event*       blinkyButton_queue[10];
static BlinkyButton blinkyButton;
Active*             AO_blinkyButton = &blinkyButton.super;

void app_main()
{
    ESP_LOGI(TAG, "BlinkyButton example start");
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
