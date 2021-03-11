/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif


#define SUPPORT_VT100 
#ifdef SUPPORT_VT100 
#define TERM_GREEN "\x1B[32m"
#define TERM_RED   "\x1B[31m"
#define TERM_YELLOW "\x1B[33m"
#define TERM_CYAN "\x1B[36m"
#define TERM_WHITE "\x1B[47m"
#define TERM_RESET "\x1B[0m"
#define TERM_BG_RED "\x1B[41m" 
#define TERM_BOLD "\x1B[1m" 
#define TERM_UL "\x1B[4m"

#define TERM_CTRL_RST "\x1B\x63"
#define TERM_CTRL_CLRSCR "\x1B[2J"
#else
#define TERM_GREEN 
#define TERM_RED   
#define TERM_YELLOW 
#define TERM_CYAN 
#define TERM_WHITE 
#define TERM_RESET 
#define TERM_BG_RED  
#define TERM_BOLD  
#define TERM_UL 

#define TERM_CTRL_RST 
#define TERM_CTRL_CLRSCR 

#endif
    
#define APP_VERSION "v1.0.0"
    
typedef enum
{
    APP_STATE_INIT=0,
    APP_STATE_SERVICE_TASKS,
} APP_STATES;

typedef struct
{
    APP_STATES state;
    uint32_t xpos;
    uint32_t ypos;
    uint32_t clr;
    uint32_t touchActive;
} APP_DATA;

void APP_Initialize ( void );

void APP_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_H */

/*******************************************************************************
 End of File
 */

