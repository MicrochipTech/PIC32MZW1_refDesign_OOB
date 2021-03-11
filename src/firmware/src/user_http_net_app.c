/*******************************************************************************
  Application to Demo HTTP NET Server

  Summary:
    Support for HTTP NET module in Microchip TCP/IP Stack

  Description:
    -Implements the application
 *******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/


#include "system_config.h"
#include "system_definitions.h"
#include "http_net_print.h"
#if defined(TCPIP_STACK_USE_HTTP_NET_SERVER)

#include "crypto/crypto.h"
#include "net_pres/pres/net_pres_socketapi.h"
#include "system/sys_random_h2_adapter.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip.h"
#include "tcpip/src/common/helpers.h"


// Sticky status message variable.
// This is used to indicated whether or not the previous POST operation was
// successful.  The application uses these to store status messages when a
// POST operation redirects.  This lets the application provide status messages
// after a redirect, when connection instance data has already been lost.
static bool lastSuccess = false;

// Sticky status message variable.  See lastSuccess for details.
static bool lastFailure = false;

/****************************************************************************
  Section:
    Customized HTTP NET Functions
 ****************************************************************************/

// processing the HTTP buffer acknowledgment
void TCPIP_HTTP_NET_DynAcknowledge(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const void *buffer, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = (HTTP_APP_DYNVAR_BUFFER*)((const uint8_t *)buffer - offsetof(HTTP_APP_DYNVAR_BUFFER, data));

    pDynBuffer->busy = 0;
}

// processing the HTTP reported events
void TCPIP_HTTP_NET_EventReport(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_NET_EVENT_TYPE evType, const void *evInfo, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    const char *evMsg = (const char *)evInfo;

    if(evType < 0)
    {   // display errors only
        if(evMsg == 0)
        {
            evMsg = "none";
        }
        SYS_CONSOLE_PRINT("HTTP event: %d, info: %s\r\n", evType, evMsg);
    }
}

bool TCPIP_HTTP_NET_SSINotification(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_SSI_NOTIFY_DCPT *pSSINotifyDcpt, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    return false;
}


TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionGetExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    return TCPIP_HTTP_NET_IO_RES_ERROR;
}

/****************************************************************************
  Section:
    Dynamic Variable Callback Functions
 ****************************************************************************/

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_builddate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, __DATE__" "__TIME__, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

extern APP_DATA appData;
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_xpos(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    size_t nChars;

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    nChars = sprintf(pDynBuffer->data, "%d", appData.xpos);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_ypos(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    size_t nChars;

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    nChars = sprintf(pDynBuffer->data, "%d", appData.ypos);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_clr(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    size_t nChars;

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    nChars = sprintf(pDynBuffer->data, "%d", appData.clr);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_active(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    size_t nChars;

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    nChars = sprintf(pDynBuffer->data, "%d", appData.touchActive);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_status_ok(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *statMsg = lastSuccess ? "block" : "none";
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, statMsg, false);
    lastSuccess = false;
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_status_fail(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *statMsg = lastFailure ? "block" : "none";
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, statMsg, false);
    lastFailure = false;
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

#endif // #if defined(TCPIP_STACK_USE_HTTP_SERVER)
