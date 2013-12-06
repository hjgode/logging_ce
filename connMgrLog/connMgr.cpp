//connMgr.cpp helper functions

#include <windows.h>
#include "connMgr.h"

	#define INIT_GUID
	#include <InitGuid.h>
	#include <connmgr.h>
	#pragma comment(lib, "wininet.lib")

#include <ws2tcpip.h>
#pragma comment(lib, "ws2.lib")

HWND hWndMain;
HWND hEdit;

void logMsg (const wchar_t *fmt, ...);

connStats connectionStates[]={
	{L"CONNMGR_STATUS_UNKNOWN", 0x00},
	{L"CONNMGR_STATUS_CONNECTED", 0x10 },
	{L"CONNMGR_STATUS_SUSPENDED", 0x11 },
	{L"CONNMGR_STATUS_DISCONNECTED", 0x20 },
	{L"CONNMGR_STATUS_CONNECTIONFAILED", 0x21 },
	{L"CONNMGR_STATUS_CONNECTIONCANCELED", 0x22 },
	{L"CONNMGR_STATUS_CONNECTIONDISABLED", 0x23 },
	{L"CONNMGR_STATUS_NOPATHTODESTINATION", 0x24 },
	{L"CONNMGR_STATUS_WAITINGFORPATH", 0x25 },
	{L"CONNMGR_STATUS_WAITINGFORPHONE", 0x26 },
	{L"CONNMGR_STATUS_PHONEOFF", 0x27 },
	{L"CONNMGR_STATUS_EXCLUSIVECONFLICT", 0x28 },
	{L"CONNMGR_STATUS_NORESOURCES", 0x29 },
	{L"CONNMGR_STATUS_CONNECTIONLINKFAILED", 0x2A },
	{L"CONNMGR_STATUS_AUTHENTICATIONFAILED", 0x2B },
	{L"CONNMGR_STATUS_WAITINGCONNECTION", 0x40 },
	{L"CONNMGR_STATUS_WAITINGFORRESOURCE", 0x41 },
	{L"CONNMGR_STATUS_WAITINGFORNETWORK", 0x42 },
	{L"CONNMGR_STATUS_WAITINGDISCONNECTION", 0x80 },
	{L"CONNMGR_STATUS_WAITINGCONNECTIONABORT", 0x81 },
	{NULL, -1},
};

UINT WM_CM_STATUS_CHANGE = WM_USER+1234;

const TCHAR* getConnStatusText(int id){
	int i=0;
	do{
		if(connectionStates[i].isStatus==id){
			return connectionStates[i].szStatus;
		}
		i++;
	}while(connectionStates[i].szStatus!=NULL);
	return L"unknown";
}

void dump_ipaddr(CONNMGR_CONNECTION_IPADDR*ip)
{
	char buffer[INET6_ADDRSTRLEN];
	TCHAR bufferW[INET6_ADDRSTRLEN];
	memset(bufferW, 0, INET6_ADDRSTRLEN * sizeof(TCHAR));
    memset(buffer, 0, INET6_ADDRSTRLEN * sizeof(char));
	DWORD cIP = ip->cIPAddr;
    for (unsigned i=0 ; i < cIP ; i++)
    {
		socklen_t addr_len=sizeof(SOCKADDR_STORAGE);
		int err=getnameinfo((struct sockaddr*)&ip->IPAddr[i], addr_len, buffer, sizeof(buffer), 0, 0, NI_NUMERICHOST);
		mbstowcs(bufferW, buffer, strlen(buffer));
		//DEBUGMSG(1, (L"\tIP %i: '%s'\r\n", i, bufferW));
		logMsg(L"\tIP %i: '%s'\r\n", i, bufferW);
        //wsprintf(sIP, L"addr[%d]: %s\r\n", i, hexdump((BYTE*)(&ip->IPAddr[i]));
    }
}

void addText(HWND hEdit, TCHAR* text){
//	HWND hEdit = GetDlgItem(hWndMain, IDC_EDIT1);
	TCHAR textOld[32000];
	TCHAR textNew[32000];
	int iCount = GetWindowText(hEdit, textOld, 32000);
	if(iCount>0)
		wsprintf(textNew, L"%s\r\r\n%s\r\r\n", textOld, text);
	else
		wsprintf(textNew, L"%s\r\r\n", text);
	SetWindowText(hEdit, textNew);
}

void dumpGUID(GUID* pGuidIn){
	TCHAR strOut[80];
	memset(strOut,0,80 * sizeof(TCHAR));
     _stprintf( strOut, TEXT("{%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X}"),
                pGuidIn->Data1, pGuidIn->Data2, pGuidIn->Data3,
                pGuidIn->Data4[0], pGuidIn->Data4[1],
                pGuidIn->Data4[2], pGuidIn->Data4[3],
                pGuidIn->Data4[4], pGuidIn->Data4[5],
                pGuidIn->Data4[6], pGuidIn->Data4[7] );
	 //DEBUGMSG(1, (strOut));
	 logMsg(strOut);
}

TCHAR* szConnType[] = {
	L"CM_CONNTYPE_UNKNOWN",
	L"CM_CONNTYPE_CELLULAR",
	L"CM_CONNTYPE_NIC",
	L"CM_CONNTYPE_BLUETOOTH",
	L"CM_CONNTYPE_UNIMODEM",
	L"CM_CONNTYPE_VPN",
	L"CM_CONNTYPE_PROXY",
	L"CM_CONNTYPE_PC",
};

TCHAR* szConnSubtype[] = {
	L"CM_CONNSUBTYPE_CELLULAR_UNKNOWN",
	L"CM_CONNSUBTYPE_CELLULAR_CSD",
	L"CM_CONNSUBTYPE_CELLULAR_GPRS",
	L"CM_CONNSUBTYPE_CELLULAR_1XRTT",
	L"CM_CONNSUBTYPE_CELLULAR_1XEVDO",
	L"CM_CONNSUBTYPE_CELLULAR_1XEVDV",
	L"CM_CONNSUBTYPE_CELLULAR_EDGE",
	L"CM_CONNSUBTYPE_CELLULAR_UMTS",
	L"CM_CONNSUBTYPE_CELLULAR_VOICE",
	L"CM_CONNSUBTYPE_CELLULAR_PTT",
};
void dump_details_params(CONNMGR_CONNECTION_DETAILED_STATUS *p){
	//DEBUGMSG(1, (L"\r\n############################################\r\n"));
	logMsg(L"\r\n############################################\r\n");
	//check which flags are valid and dump them
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_TYPE){			//The dwType member of CONNMGR_CONNECTION_DETAILED_STATUS is valid
		//DEBUGMSG(1,(L"\tConnType= '%s'\r\n", szConnType[p->dwType]));
		logMsg(L"\tConnType= '%s'\r\n", szConnType[p->dwType]);
	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_SUBTYPE){		//dwSubtype member of CONNMGR_CONNECTION_DETAILED_STATUS is valid
		//DEBUGMSG(1,(L"\tConnSubtype= '%s'\r\n", szConnSubtype[p->dwSubtype]));
		logMsg(L"\tConnSubtype= '%s'\r\n", szConnSubtype[p->dwSubtype]);
		switch(p->dwSubtype){
			case CM_CONNSUBTYPE_UNKNOWN:
				break;
		}
	}
	if(p->dwFlags & CONNMGRDETAILEDSTATUS_PARAM_FLAGS){
		if(p->dwFlags && CM_DSF_BILLBYTIME){
			//DEBUGMSG(1, (L"\t cm_flags+=CM_DSF_BILLBYTIME\r\n"));
			logMsg(L"\t cm_flags+=CM_DSF_BILLBYTIME\r\n");
		}
		if(p->dwFlags && CM_DSF_ALWAYSON){
			//DEBUGMSG(1, (L"\t cm_flags+=CM_DSF_ALWAYSON\r\n"));
			logMsg(L"\t cm_flags+=CM_DSF_ALWAYSON\r\n");
		}
		if(p->dwFlags && CM_DSF_SUSPENDRESUME){
			//DEBUGMSG(1, (L"\t cm_flags+=CM_DSF_SUSPENDRESUME\r\n"));
			logMsg(L"\t cm_flags+=CM_DSF_SUSPENDRESUME\r\n");
		}

	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_SECURE){
		//DEBUGMSG(1, (L"\tsecurity level: %i\r\n", p->dwSecure));
		logMsg(L"\tsecurity level: %i\r\n", p->dwSecure);
	}
	if(p->dwParams && CONNMGRDETAILEDSTATUS_PARAM_DESTNET){
		//DEBUGMSG(1, (L"\tdest guid: "));
		logMsg(L"\tdest guid: ");
		dumpGUID( &p->guidDestNet );
		//DEBUGMSG(1, (L"\r\n"));
		logMsg(L"\r\n");
	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_SOURCENET){
		//DEBUGMSG(1, (L"\tsource guid: "));
		logMsg(L"\tsource guid: ");
		dumpGUID( &p->guidSourceNet );
		//DEBUGMSG(1, (L"\r\n"));
		logMsg(L"\r\n");
	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_DESCRIPTION){
		//DEBUGMSG(1, (L"\tdescription: '%s'\r\n", p->szDescription));
		logMsg(L"\tdescription: '%s'\r\n", p->szDescription);
	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_ADAPTERNAME){
		//DEBUGMSG(1, (L"\tadapter: '%s'\r\n", p->szAdapterName));
		logMsg(L"\tadapter: '%s'\r\n", p->szAdapterName);
	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_CONNSTATUS){
		//DEBUGMSG(1, (L"\tconn state: %i='%s'\r\n", p->dwConnectionStatus, connectionStates[p->dwConnectionStatus]));
		logMsg(L"\tconn state: %i='%s'\r\n", p->dwConnectionStatus, connectionStates[p->dwConnectionStatus]);
	}
	if(p->dwParams && CONNMGRDETAILEDSTATUS_PARAM_LASTCONNECT){
		//DEBUGMSG(1, (L"\tlast connect: %04d-%02d-%02d(%d) %02d:%02d:%02d.%03d\r\n",
				//p->LastConnectTime.wYear, p->LastConnectTime.wMonth, p->LastConnectTime.wDay,
				//p->LastConnectTime.wDayOfWeek,
				//p->LastConnectTime.wHour, p->LastConnectTime.wMinute, p->LastConnectTime.wSecond,
				//p->LastConnectTime.wMilliseconds));
		logMsg(L"\tlast connect: %04d-%02d-%02d(%d) %02d:%02d:%02d.%03d\r\n",
				p->LastConnectTime.wYear, p->LastConnectTime.wMonth, p->LastConnectTime.wDay,
				p->LastConnectTime.wDayOfWeek,
				p->LastConnectTime.wHour, p->LastConnectTime.wMinute, p->LastConnectTime.wSecond,
				p->LastConnectTime.wMilliseconds);

	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_SIGNALQUALITY){
		//DEBUGMSG(1, (L"\tsignal: %i\r\n", p->dwSignalQuality));
		logMsg(L"\tsignal: %i\r\n", p->dwSignalQuality);
	}
	if(p->dwParams & CONNMGRDETAILEDSTATUS_PARAM_IPADDR ){
		dump_ipaddr(p->pIPAddr);
	}
	else{
		//DEBUGMSG(1, (L"\tno IP\r\n"));
		logMsg(L"\tno IP\r\n");
	}
}

void dump_details(CONNMGR_CONNECTION_DETAILED_STATUS *p)
{
	dump_details_params(p);
	return;
	TCHAR txt[MAX_PATH];
	if (p->szAdapterName && wcslen(p->szAdapterName)>0){
        //DEBUGMSG(1,(L"adap: %s\r\n", p->szAdapterName));
		logMsg(L"adap: %s\r\n", p->szAdapterName);
		//wsprintf(txt, L"adap: %s\r\n", p->szAdapterName);
		//addText(hWndMain, txt);
	}
	if (p->szDescription){
        //DEBUGMSG(1, (L"desc: %s\r\n", p->szDescription));
		logMsg(L"desc: %s\r\n", p->szDescription);
		//wsprintf(txt, L"desc: %s\r\n", p->szDescription);
		//addText(hWndMain, txt);
	}

    //DEBUGMSG(1, (L"\version=%08lx params=%08lx t=%08lx s=%08lx f=%08lx sec=%08lx stat=%08lx q=%08lx\r\n",
        //p->dwVer, p->dwParams, p->dwType, p->dwSubtype, p->dwFlags, p->dwSecure, p->dwConnectionStatus, p->dwSignalQuality));
	logMsg(L"\version=%08lx params=%08lx t=%08lx s=%08lx f=%08lx sec=%08lx stat=%08lx q=%08lx\r\n",
        p->dwVer, p->dwParams, p->dwType, p->dwSubtype, p->dwFlags, p->dwSecure, p->dwConnectionStatus, p->dwSignalQuality);
    //debug(L"dst=%s\r\n", GuidToString(&p->guidDestNet).c_str());
    //debug(L"src=%s\r\n", GuidToString(&p->guidSourceNet).c_str());
    //DEBUGMSG(1, (L"\tlast: %04d-%02d-%02d(%d) %02d:%02d:%02d.%03d\r\n",
            //p->LastConnectTime.wYear, p->LastConnectTime.wMonth, p->LastConnectTime.wDay,
            //p->LastConnectTime.wDayOfWeek,
            //p->LastConnectTime.wHour, p->LastConnectTime.wMinute, p->LastConnectTime.wSecond,
            //p->LastConnectTime.wMilliseconds));
	logMsg(L"\tlast: %04d-%02d-%02d(%d) %02d:%02d:%02d.%03d\r\n",
            p->LastConnectTime.wYear, p->LastConnectTime.wMonth, p->LastConnectTime.wDay,
            p->LastConnectTime.wDayOfWeek,
            p->LastConnectTime.wHour, p->LastConnectTime.wMinute, p->LastConnectTime.wSecond,
            p->LastConnectTime.wMilliseconds);

    if (p->pIPAddr)
        dump_ipaddr(p->pIPAddr);
}

void dumpConnections(HWND hWnd){
	hEdit=hWnd;

	HANDLE hConnMgrReady = ConnMgrApiReadyEvent();
	DWORD dwWait = WaitForSingleObject(hConnMgrReady, 3000);
	switch (dwWait){
		case WAIT_OBJECT_0:
			break;
		case WAIT_FAILED:
			break;
		case WAIT_ABANDONED:
			break;
		default:
			break;
	}
	CloseHandle(hConnMgrReady);

	CONNMGR_CONNECTION_DETAILED_STATUS* pStatusBuffer=NULL;
	DWORD dwBufferSize=0;
	HRESULT hResult=0;
	//query the needed buffer size
	hResult = ConnMgrQueryDetailedStatus(NULL, &dwBufferSize);
	if(hResult==(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)))
	{
		BYTE* bv = (BYTE*) malloc(dwBufferSize);
		CONNMGR_CONNECTION_DETAILED_STATUS *s= (CONNMGR_CONNECTION_DETAILED_STATUS*)&bv[0];
		hResult= ConnMgrQueryDetailedStatus(s, &dwBufferSize);

		for (CONNMGR_CONNECTION_DETAILED_STATUS *p= s ; p ; p= p->pNext)
		{
		   dump_details(p);
		}
		free(pStatusBuffer);
		free(bv);
	}
}


void startConnMgrWatch(HWND hWnd){
	HRESULT hr;
	hEdit=hWnd;
	if(WM_CM_STATUS_CHANGE==WM_USER+1234){	// do only once, if 1234 then not registered
		WM_CM_STATUS_CHANGE = RegisterWindowMessage( CONNMGR_STATUS_CHANGE_NOTIFICATION_MSG );
		//DEBUGMSG(1, (L"RegisterWindowMessage =0x%x.\r\n", WM_CM_STATUS_CHANGE));
		logMsg(L"RegisterWindowMessage =0x%x.\r\n", WM_CM_STATUS_CHANGE);
	}
	// after you registered for the CONNMGR_STATUS_CHANGE_NOTIFICATION_MSG and got a constant you can watch for changes
	hr = ConnMgrRegisterForStatusChangeNotification(TRUE, hWnd);
	DEBUGMSG(1, (L"ConnMgrRegisterForStatusChangeNotification =0x%x.\r\n", hr));
	return;
}

void logMsg (const wchar_t *fmt, ...)
{
        va_list vl;
        va_start(vl,fmt);
        wchar_t bufW[1024]; // to bad CE hasn't got wvnsprintf
        wvsprintf(bufW,fmt,vl);
        char bufOutA[512];

		//convert to char
        WideCharToMultiByte(CP_ACP,0,bufW,-1,bufOutA,400, NULL, NULL);

		if(hEdit){
			int iLen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
			iLen+=wcslen(bufW);
			if(iLen>32000)
			{
				//clear text
				SendMessage(hEdit,WM_SETTEXT,0,0);
				iLen=wcslen(bufW);
				SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)bufW);
			}
			else{
				TCHAR* buffer=(TCHAR*)calloc((2+iLen), sizeof(TCHAR));
				SendMessage(hEdit,WM_GETTEXT, iLen * sizeof(TCHAR), reinterpret_cast<LPARAM>(buffer));
				wcscat(buffer, bufW);
				//DEBUGMSG(1, (buffer));
				//addText(hEdit, bufW);
				SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)buffer);
				free (buffer);
			}
			SendMessage(hEdit, EM_SETSEL, 0, MAKELONG(0xffff,0xffff));
			SendMessage(hEdit, EM_SCROLLCARET, 0,0);
			UpdateWindow(hEdit);
		}

#ifdef DEBUG
		DEBUGMSG(1, (bufW));
#else
		RETAILMSG(1, (bufW));
#endif
}
