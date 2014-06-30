//

#ifndef _DEVICE_MONITOR_H_
#define _DEVICE_MONITOR_H_

#include "stdafx.h"


#include <pnp.h>
#include <ceconfig.h>
#include <pnp.h>
#include <storemgr.h>

//#include "ce600/cedrv_guid.h"

#define WM_PNPUPDATE WM_USER + 14

int startMonitor(HWND hWnd);
void stopMonitor();

#endif //_DEVICE_MONITOR_H_