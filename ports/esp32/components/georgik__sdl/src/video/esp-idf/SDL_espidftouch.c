#include "events/SDL_touch_c.h"
#include "video/SDL_sysvideo.h"
#include "SDL_espidftouch.h"
#include <stdbool.h>

#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#if BSP_CAPS_TOUCH == 1
#include "bsp/touch.h"
esp_lcd_touch_handle_t touch_handle;   // LCD touch handle
#endif
#include "esp_log.h"

#define ESPIDF_TOUCH_ID         1
#define ESPIDF_TOUCH_FINGER     1


void ESPIDF_InitTouch(void)
{
#if BSP_CAPS_TOUCH == 1
#endif
}

void ESPIDF_PumpTouchEvent(void)
{
#if BSP_CAPS_TOUCH == 1
#endif
}

int ESPIDF_CalibrateTouch(float screenX[], float screenY[], float touchX[], float touchY[])
{
    return 0;
}

void ESPIDF_ChangeTouchMode(int raw)
{
    return;
}

void ESPIDF_ReadTouchRawPosition(float* x, float* y)
{
    return;
}

void ESPIDF_QuitTouch(void)
{
    // ts_close(ts);
}
