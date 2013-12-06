// btmsgq.cpp
#include <windows.h>
#include <bt_api.h>
#include "btmsgq.h"
#include "nclog.h"

#ifndef BTE_CONNECTION_AUTH_FAILURE
#define BTE_CONNECTION_AUTH_FAILURE 105
#endif
#define MAX_TEXT_SIZE 10000

HANDLE hBTNotif;
HANDLE hMsgQ ;
DWORD threadID=0;
BOOL bStop;
void dumpBTevent(BTEVENT btEvent, HWND hwnd);
static WCHAR* btAddr2Mac(unsigned __int64 btAddr, WCHAR *macW);

HWND hwndMsg=NULL;

TCHAR* szBTerror[]={
	L"0x00 undefined",
	L"0x01 Unknown HCI Command.",
	L"0x02 No Connection.",
	L"0x03 Hardware Failure.",
	L"0x04 Page Timeout.",
	L"0x05 Authentication Failure.",
	L"0x06 Key Missing.",
	L"0x07 Memory Full.",
	L"0x08 Connection Timeout.",
	L"0x09 Max Number Of Connections.",
	L"0x0A Max Number Of SCO Connections To A Device.",
	L"0x0B ACL connection already exists.",
	L"0x0C Command Disallowed.",
	L"0x0D Host Rejected due to limited resources.",
	L"0x0E Host Rejected due to security reasons.",
	L"0x0F Host Rejected due to remote device is only a personal device.",
	L"0x10 Host Timeout.",
	L"0x11 Unsupported Feature or Parameter Value.",
	L"0x12 Invalid HCI Command Parameters.",
	L"0x13 Other End Terminated Connection: User Ended Connection.",
	L"0x14 Other End Terminated Connection: Low Resources.",
	L"0x15 Other End Terminated Connection: About to Power Off.",
	L"0x16 Connection Terminated by Local Host.",
	L"0x17 Repeated Attempts.",
	L"0x18 Pairing Not Allowed.",
	L"0x19 Unknown LMP PDU.",
	L"0x1A Unsupported Remote Feature.",
	L"0x1B SCO Offset Rejected.",
	L"0x1C SCO Interval Rejected.",
	L"0x1D SCO Air Mode Rejected.",
	L"0x1E Invalid LMP Parameters.",
	L"0x1F Unspecified Error.",
	L"0x20 Unsupported LMP Parameter Value.",
	L"0x21 Role Change Not Allowed",
	L"0x22 LMP Response Timeout",
	L"0x23 LMP Error Transaction Collision",
	L"0x24 LMP PDU Not Allowed",
	L"0x25 Encryption Mode Not Acceptable",
	L"0x26 Unit Key Used",
	L"0x27 QoS is Not Supported",
	L"0x28 Instant Passed",
	L"x29 Pairing with Unit Key Not Supported",
};

const int lastError=0x29;

void printMsg(TCHAR* msg, HWND hwnd){
	static TCHAR _msg[MAX_PATH];
	wsprintf(_msg, L"%s", msg);

	if(hwnd!=NULL){
		TCHAR szTemp[MAX_TEXT_SIZE];
		int iCnt = GetWindowTextLength(hwnd);
		if(iCnt+wcslen(_msg)>MAX_TEXT_SIZE){
			//clear text
			DEBUGMSG(1, (L"printMsg: hwndMsg=%i, '%s'\n", hwnd, _msg));
			SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM) _msg);		
		}
		else{
			GetWindowText(hwnd, szTemp, MAX_TEXT_SIZE);
			//wsprintf(szTemp, L"%s\r\n%s", szTemp, _msg);
			wcscat(szTemp, _msg);
			SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM) szTemp);
			SendMessage(hwnd, EM_LINESCROLL, 0, 255);
		}
	}
	else
		DEBUGMSG(1, (L"hwndMsg=NULL\n"));
}

DWORD msgThread(LPVOID lpParam){
	HWND hWnd = (HWND)lpParam;
	hwndMsg=hWnd;

	DEBUGMSG(1,(L"Waiting for Bluetooth notifications with hwnd=%i...\n", hWnd));
	nclog(L"%s: Waiting for Bluetooth notifications...\n", logDateTime());
	
	TCHAR szMsg[MAX_PATH];
	wsprintf(szMsg, L"\r\n%s: Waiting for Bluetooth notifications...", logDateTime());
	printMsg(szMsg, hWnd);

	BTEVENT btEvent;
	DWORD dwBytesRead;
	DWORD dwFlags;
	BOOL fRet;

	while (FALSE == bStop) {
		DWORD dwWait = WaitForSingleObject (hMsgQ, 5000);//wait up to 5 seconds INFINITE);
		switch (dwWait){
			case WAIT_OBJECT_0:
				// We have got a Bluetooth event!
				dwFlags = 0;
				dwBytesRead = 0;

				fRet = ReadMsgQueue (hMsgQ, &btEvent, sizeof(BTEVENT), &dwBytesRead, 10, &dwFlags);
				if (! fRet) {
					DEBUGMSG(1,(L"Error - Failed to read message from queue!\n"));
					//bStop=TRUE;
				} 
				else {
					dumpBTevent(btEvent, hWnd);
				}
				break;
			case WAIT_TIMEOUT:
				break;
			case WAIT_ABANDONED:
				DEBUGMSG(1,(L"Error - Unexpected return value from WaitForSingleObject!\n"));
				//bStop=TRUE;
				break;
			case WAIT_FAILED:
				DEBUGMSG(1,(L"Error - Unexpected return value from WaitForSingleObject!\n"));
				//bStop=TRUE;
				break;
		}//switch
	}//while
	return 0;
}

void startMsgQueue(HWND hWnd){
	DEBUGMSG(1, (L"Entering msgQueue with hwnd=%i\n", hWnd));

	MSGQUEUEOPTIONS mqOptions;
	memset (&mqOptions, 0, sizeof(mqOptions));

	mqOptions.dwFlags = 0;
	mqOptions.dwSize = sizeof(mqOptions);
	mqOptions.dwMaxMessages = 10;
	mqOptions.cbMaxMessage = sizeof(BTEVENT);
	mqOptions.bReadAccess = TRUE;

	hMsgQ = CreateMsgQueue(NULL, &mqOptions);
	if (! hMsgQ) {
		nclog(L"Error creating message queue.\n");
		goto exit;
	}
	hBTNotif = RequestBluetoothNotifications(
		BTE_CLASS_CONNECTIONS	|
			BTE_CONNECTION			|
			BTE_DISCONNECTION		|
			BTE_ROLE_SWITCH			|
			BTE_MODE_CHANGE			|
			BTE_PAGE_TIMEOUT		|
			BTE_CONNECTION_AUTH_FAILURE	|
		BTE_CLASS_PAIRING	|
			BTE_KEY_NOTIFY	|
			BTE_KEY_REVOKED	|
		BTE_CLASS_DEVICE	|
			BTE_LOCAL_NAME	|
			BTE_COD			|
		BTE_CLASS_STACK		|
			BTE_STACK_UP	|
			BTE_STACK_DOWN	
		, hMsgQ);

	bStop=FALSE;
	CreateThread(NULL, 0, msgThread, hWnd, 0, &threadID);

exit:
	;
}

void stopMsgQueue(){
	StopBluetoothNotifications(hBTNotif);
	nclog(L"BT notification ended: %s\n", logDateTime());
}



void dumpBTevent(BTEVENT btEvent, HWND hwnd){
	BT_CONNECT_EVENT* cntEvt = NULL;
	BT_DISCONNECT_EVENT* discntEvt=NULL;
	BT_ROLE_SWITCH_EVENT* rolSwitchEvt=NULL;
	BT_MODE_CHANGE_EVENT* btModeChgEvt=NULL;
	BT_LINK_KEY_EVENT* btLnkKeyEvt=NULL;

	TCHAR btAddress[18];
	TCHAR hConn[12];
	TCHAR encMode[5];
	TCHAR linkType[5];
	TCHAR szTemp[MAX_PATH];
	TCHAR szMsg[MAX_PATH];

	wsprintf(szMsg, L"\r\n");

	switch (btEvent.dwEventId){
		case BTE_KEY_NOTIFY:
			wsprintf(szMsg, L"\r\n%s: BTE_KEY_NOTIFY:", logDateTime());
			break;
		case BTE_KEY_REVOKED:
			btLnkKeyEvt = (BT_LINK_KEY_EVENT*)btEvent.baEventData;
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			wsprintf(btAddress, L"%s", btAddr2Mac( btLnkKeyEvt->bta, btAddress));
			wsprintf(szMsg, L"\r\n%s: BTE_KEY_REVOKED: mac=%s", logDateTime(), btAddress);
			break;
		case BTE_LOCAL_NAME:
			wsprintf(szMsg, L"\r\n%s: BTE_LOCAL_NAME:", logDateTime());
			break;
		case BTE_COD:			
			wsprintf(szMsg, L"\r\n%s: BTE_COD:", logDateTime());
			break;
		case BTE_STACK_UP:			
			wsprintf(szMsg, L"\r\n%s: BTE_STACK_UP:", logDateTime());
			break;
		case BTE_STACK_DOWN:
			wsprintf(szMsg,L"\r\n%s: BTE_STACK_DOWN:", logDateTime());
			break;
		case BTE_CONNECTION:
			cntEvt = (BT_CONNECT_EVENT*)btEvent.baEventData;
			btAddr2Mac(cntEvt->bta, btAddress);
			wsprintf(btAddress, L"%s", btAddress);// L"0x00 n/a");
			wsprintf(hConn, L"0x%08x", cntEvt->hConnection);
			wsprintf(encMode, L"0x%02x", cntEvt->ucEncryptMode);
			wsprintf(linkType, L"0x%02x", cntEvt->ucLinkType);
			wsprintf(szMsg, L"\r\n%s: BTE_CONNECTION: hnd=%s, mac=%s, enc=%s, lnk=%s",
				logDateTime(),
				hConn,
				btAddress,
				encMode,
				linkType);
			break;
		case BTE_PAGE_TIMEOUT:
			wsprintf(szMsg,L"\r\n%s: BTE_PAGE_TIMEOUT:", logDateTime());
			break;
		case BTE_MODE_CHANGE:
			btModeChgEvt = (BT_MODE_CHANGE_EVENT*)btEvent.baEventData;
			wsprintf(btAddress, L"%s", btAddr2Mac(btModeChgEvt->bta, btAddress));
			wsprintf(hConn, L"0x%08x", btModeChgEvt->hConnection);
			wsprintf(encMode, L"0x%02x", btModeChgEvt->bMode);
			wsprintf(szTemp, L"%i", btModeChgEvt->usInterval);
			wsprintf(szMsg, L"\r\n%s: BTE_MODE_CHANGE: cnt=%s, mac=%s, mod=%s, int=%s",
				logDateTime(),
				hConn,
				btAddress,
				encMode,
				szTemp);
			break;
		case BTE_ROLE_SWITCH:
			rolSwitchEvt = (BT_ROLE_SWITCH_EVENT*)btEvent.baEventData;
			wsprintf(btAddress, L"%s", btAddr2Mac(rolSwitchEvt->bta, btAddress));
			wsprintf(szTemp, L"0x%02x", rolSwitchEvt->fRole);
			wsprintf(szMsg, L"\r\n%s: BTE_ROLE_SWITCH: mac=%s, new role=%s",
				logDateTime(),
				btAddress, 
				szTemp);
			break;
		case BTE_DISCONNECTION:
			discntEvt = (BT_DISCONNECT_EVENT*)btEvent.baEventData;
			wsprintf(hConn, L"0x%08x", discntEvt->hConnection);
			wsprintf(szTemp, L"%i", discntEvt->ucReason);
			if( discntEvt->ucReason <= lastError)
				wsprintf(szTemp, L"'%s'", szBTerror[discntEvt->ucReason]);
			wsprintf(szMsg, L"\r\n%s: BTE_DISCONNECTION: hnd=%s, why=%s",
				logDateTime(),
				hConn, 
				szTemp);
			break;
		case BTE_CONNECTION_AUTH_FAILURE:
			wsprintf(szMsg, L"\r\n%s BTE_CONNECTION_AUTH_FAILURE", logDateTime());
			break;
		default:
			wsprintf(szMsg, L"\r\n%s: unknown BT event: %i", logDateTime(), btEvent.dwEventId);
			break;
	}//switch
	DEBUGMSG(1, (szMsg));
	nclog(szMsg);
	printMsg(szMsg, hwnd);
}

static WCHAR* btAddr2Mac(unsigned __int64 btAddr, WCHAR *macW)
{
	// 0x00000023686e70bc
	wsprintf(macW, L"0x%0.2I64X", btAddr);

	//char *mac = "              ";
	//int BT_ALEN=8;
	//for (int i = (BT_ALEN - 1); i >= 0; i--) {
	//	mac[i] = (UINT8) ((unsigned __int64) 0xff & btAddr);
	//	btAddr = btAddr >> 8;
	//}
	//mbstowcs(macW,mac,strlen(mac));
	return macW;
}
