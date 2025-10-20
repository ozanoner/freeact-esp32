
#include "FreeAct.h"
#include "bsp.h"
#include "esp_err.h"
#include "esp_log.h"

Active* AO_blinkyButton;

void app_main(void)
{
    BSP_init();
    BSP_start();
}
