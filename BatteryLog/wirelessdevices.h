//wirelessdevices.h
#pragma once

#include "wrlspwr.h"
#include "Wininet.h"
#include <service.h>
#pragma comment(lib, "Wininet.lib")

// WLAN Switch
#define _WLAN_SWITCH_OFF 0
#define _WLAN_SWITCH_ON 1
// Phone Switch
#define _PHONE_SWITCH_OFF 0
#define _PHONE_SWITCH_ON 1
// BT Switch
#define _BT_SWITCH_OFF 0
#define _BT_SWITCH_ON 1
//
#define GetWirelessDevice_ORDINAL 276
#define ChangeRadioState_ORDINAL 273
#define FreeDeviceList_ORDINAL 280


//imports from ossvcs.dll
typedef LRESULT (CALLBACK* _GetWirelessDevices)(RDD **pDevices, DWORD dwFlags);
typedef LRESULT (CALLBACK* _ChangeRadioState)(RDD* pDev, DWORD dwState, SAVEACTION sa);
typedef LRESULT (CALLBACK* _FreeDeviceList)(RDD *pRoot);

//Header of further functions
DWORD GetWDevState(DWORD* bWifi, DWORD* bPhone, DWORD* bBT);
DWORD SetWDevState(DWORD dwDevice, DWORD dwState);
BOOL DeinitDLL();
BOOL InitDLL();
//
