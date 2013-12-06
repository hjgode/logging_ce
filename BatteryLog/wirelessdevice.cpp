//wirelessdevice.cpp
#include "stdafx.h"
#include "WirelessDevices.h"


_GetWirelessDevices pGetWirelessDevices = NULL;
_ChangeRadioState pChangeRadioState = NULL;
_FreeDeviceList pFreeDeviceList = NULL;

HINSTANCE g_DllWrlspwr;

BOOL InitDLL()
{
	g_DllWrlspwr = LoadLibrary(TEXT("ossvcs.dll"));
	if (g_DllWrlspwr == NULL)
		return FALSE;
	pGetWirelessDevices = (_GetWirelessDevices)GetProcAddress(g_DllWrlspwr,MAKEINTRESOURCE(GetWirelessDevice_ORDINAL));
	if (pGetWirelessDevices == NULL)
		return FALSE;

	pChangeRadioState = (_ChangeRadioState)GetProcAddress(g_DllWrlspwr,MAKEINTRESOURCE(ChangeRadioState_ORDINAL));
	if (pChangeRadioState == NULL)
		return FALSE;

	pFreeDeviceList = (_FreeDeviceList)GetProcAddress(g_DllWrlspwr,MAKEINTRESOURCE(FreeDeviceList_ORDINAL));
	if (pFreeDeviceList == NULL)
		return FALSE;
	return TRUE;
}

BOOL DeinitDLL()
{
	return FreeLibrary(g_DllWrlspwr);
}

//set the status of the desired wireless device
DWORD SetWDevState(DWORD dwDevice, DWORD dwState)
{
	RDD * pDevice = NULL;
	RDD * pTD;
	HRESULT hr;
	DWORD retval = 0;

	// InitDLL();
	hr = pGetWirelessDevices(&pDevice, 0);
	if(hr != S_OK) return -1;

	if (pDevice)
	{
		pTD = pDevice;

		// loop through the linked list of devices
		while (pTD)
		{
			if (pTD->DeviceType == dwDevice)
			{
				hr = pChangeRadioState(pTD, dwState, RADIODEVICES_PRE_SAVE);
				retval = 0;
			}

		pTD = pTD->pNext;

		}
		// Free the list of devices retrieved with
		// GetWirelessDevices()
		pFreeDeviceList(pDevice);
	}

	if(hr == S_OK)return retval;

	return -2;
}

//*********pls see this is getWdevstate***********

//get status of all wireless devices at once
DWORD GetWDevState(DWORD* bWifi, DWORD* bPhone, DWORD* bBT)
{
	RDD * pDevice = NULL;
	RDD * pTD;

	HRESULT hr;
	DWORD retval = 0;

	hr = pGetWirelessDevices(&pDevice, 0);

	if(hr != S_OK) return -1;

	if (pDevice)
	{
		pTD = pDevice;

		// loop through the linked list of devices
		while (pTD)
		{
			switch (pTD->DeviceType)
			{
				case RADIODEVICES_MANAGED:
					*bWifi = pTD->dwState;
					break;
				case RADIODEVICES_PHONE:
					*bPhone = pTD->dwState;
					break;
				case RADIODEVICES_BLUETOOTH:
					*bBT = pTD->dwState;
					break;
				default:
					break;
			}
			pTD = pTD->pNext;
		}
		// Free the list of devices retrieved with
		// GetWirelessDevices()
		pFreeDeviceList(pDevice);
	}

	if(hr == S_OK)
		return retval;

	return -2;
}

/*
//on initdialog add following code

	DeinitDLL();
	DWORD dwWifi;
	UpdateData(TRUE);
	GetWDevState(&dwWifi);
	if(dwWifi == 1)
	str=TEXT("ON");
	//str is textbox varriable
	else
	str=TEXT("OFF");
	UpdateData(FALSE);

//on clicking ON button following code will be executed

	SetWDevState( RADIODEVICES_MANAGED, 1);
	DWORD dwWifi;
	GetWDevState(&dwWifi);
	UpdateData(TRUE);
	if(dwWifi == 1)
	str=TEXT("ON");
	else
	str=TEXT("OFF");
	UpdateData(FALSE);

//and on off button

	SetWDevState( RADIODEVICES_MANAGED, 0);
	DWORD dwWifi;
	UpdateData(TRUE);
	GetWDevState(&dwWifi);
	if(dwWifi == 1)
	str=TEXT("ON");
	else
	str=TEXT("OFF");
	UpdateData(FALSE);
*/
