//WRLSPWR.H
#pragma once

// Types of radio device
typedef enum _RADIODEVTYPE
{
	RADIODEVICES_MANAGED = 1,
	RADIODEVICES_PHONE,
	RADIODEVICES_BLUETOOTH,
} RADIODEVTYPE;

// whether to save before or after changing state
typedef enum _SAVEACTION
{
	RADIODEVICES_DONT_SAVE = 0,
	RADIODEVICES_PRE_SAVE,
	RADIODEVICES_POST_SAVE,
} SAVEACTION;

// Details of radio devices
struct RDD
{
	RDD() : pszDeviceName(NULL), pNext(NULL), pszDisplayName(NULL) {}
	~RDD()
	{
		LocalFree(pszDeviceName); 
		LocalFree(pszDisplayName); 
	}
	LPTSTR pszDeviceName; // Device name for registry setting.
	LPTSTR pszDisplayName; // Name to show the world
	DWORD dwState; // ON/off/[Discoverable for BT]
	DWORD dwDesired; // desired state - used for setting registry etc.
	RADIODEVTYPE DeviceType; // Managed, phone, BT etc.
	RDD * pNext; // Next device in list
}; //radio device details
