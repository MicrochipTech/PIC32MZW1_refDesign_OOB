#include "app.h"
#include "system/fs/sys_fs.h"
#include "system/console/sys_console.h"
#include "http_net_print.h"
#include "OLEDB.h"
#include "led_driver.h"
#include "touch/touch_api_ptc.h"
#include "system/time/sys_time.h"
#include "peripheral/wdt/plib_wdt.h"

APP_DATA appData;

#define printf SYS_CONSOLE_PRINT

extern volatile uint8_t measurement_done_touch;
extern volatile uint8_t h_pos;
extern volatile uint8_t v_pos;

static bool refreshDisplay = false;

void display_callback(uintptr_t context) {
    refreshDisplay = true;
}

void APP_Initialize(void) {
    appData.state = APP_STATE_INIT;
    appData.xpos = 0;
    appData.ypos = 255;
    appData.clr =1;
    appData.touchActive=0;
    SYS_CONSOLE_PRINT("\r\nStarting Demo Application ("TERM_CYAN"%s %s"TERM_RESET"): " \
                    TERM_BOLD TERM_BG_RED APP_VERSION TERM_RESET" \r\n",__DATE__, __TIME__);
    SYS_TIME_CallbackRegisterMS(display_callback, (uintptr_t) 0, 200, SYS_TIME_PERIODIC);
}

static int isPowTwo(uint8_t n) {
    return n && (!(n & (n - 1)));
}

// Returns position of the set bit
static int decodeBit(uint8_t n) {
    if (!isPowTwo(n)) {
        return 0;
    }

    uint8_t i = 1, pos = 0;
    while (!(i & n)) {
        i = i << 1;
        ++pos;
    }
    return pos;
}

static void refresh_display() {
    static uint8_t xPos = 0;
    static uint8_t yPos = 0;

    uint8_t xPosNew = 0;
    uint8_t yPosNew = 0;

    if((0==h_pos)&&(0==v_pos)) return;
    
    xPosNew = decodeBit(h_pos);
    yPosNew = decodeBit(v_pos);

    if ((xPos!=xPosNew)||(yPos!=yPosNew)){
        xPos=xPosNew;
        yPos=yPosNew;
        oledb_displayXY(4-xPos,yPos);
    }
 }

void APP_Tasks(void) {
    WDT_Clear();
    switch (appData.state) {
        case APP_STATE_INIT:
        {
            OLEDB_Initialize();
            init_led_driver();
            HTTP_APP_Initialize();
            WDT_Enable();
            appData.state = APP_STATE_SERVICE_TASKS;
            break;
        }
        case APP_STATE_SERVICE_TASKS:
        {
            touch_process();
            if (measurement_done_touch == 1u) {
                measurement_done_touch = 0u;
                led_decode_position();
                break;
            }

            if (refreshDisplay) {
                refresh_display();
                refreshDisplay = false;
            }
        }
        default:
        {
            break;
        }

    }
}
/*******************************************************************************
 End of File
 */
