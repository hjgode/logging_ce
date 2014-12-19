//
/*
StringFromCLSID: {C375C787-B721-4B8E-B67F-A112D5C0A404}:unknown detached
StringFromCLSID: {CC5195AC-BA49-48A0-BE17-DF6D1B0173DD}:unknown detached
StringFromCLSID: {A32942B7-920C-486B-B0E6-92A702A99B35}:PMCLASS_GENERIC_DEVICE detached
StringFromCLSID: {89178031-19C3-4C81-955D-4DD2AB5C1049}:DEVCLASS_TUSMODEM_GUID detached
StringFromCLSID: {82ED1744-96D6-412D-AB63-DEB107AAD6D3}:DEVCLASS_TUSSERIAL_GUID detached
StringFromCLSID: {F8A6BA98-087A-43AC-A9D8-B7F13C5BAE31}:unknown detached

StringFromCLSID: {F8A6BA98-087A-43AC-A9D8-B7F13C5BAE31}:unknown attached
StringFromCLSID: {82ED1744-96D6-412D-AB63-DEB107AAD6D3}:DEVCLASS_TUSSERIAL_GUID attached
StringFromCLSID: {89178031-19C3-4C81-955D-4DD2AB5C1049}:DEVCLASS_TUSMODEM_GUID attached
StringFromCLSID: {A32942B7-920C-486B-B0E6-92A702A99B35}:PMCLASS_GENERIC_DEVICE attached
StringFromCLSID: {CC5195AC-BA49-48A0-BE17-DF6D1B0173DD}:unknown attached
StringFromCLSID: {C375C787-B721-4B8E-B67F-A112D5C0A404}:unknown attached


*/

#include "device_monitor.h"
#include "iclass_info.h"

#include <ole2.h>
#pragma comment (lib, "ole32.lib")

#include "nclog.h"

BOOL bStopMonitor=FALSE;
DWORD dwThreadID;
HANDLE dwThreadHandle;
const TCHAR* szStopThreadEvent=L"STOP_MYPNP_MONITOR";
HANDLE hStopEventHandle;

TCHAR* dumpGUID(GUID* pGuidIn, TCHAR* szGUID){
	TCHAR strOut[80];
	memset(strOut,0,80 * sizeof(TCHAR));
     _stprintf( strOut, TEXT("{%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X}"),
                pGuidIn->Data1, pGuidIn->Data2, pGuidIn->Data3,
                pGuidIn->Data4[0], pGuidIn->Data4[1],
                pGuidIn->Data4[2], pGuidIn->Data4[3],
                pGuidIn->Data4[4], pGuidIn->Data4[5],
                pGuidIn->Data4[6], pGuidIn->Data4[7] );
	 //DEBUGMSG(1, (strOut));
	 wcscpy(szGUID, strOut);
	 return szGUID;
}

TCHAR* dumpGUID(GUID pGuidIn, TCHAR* szGUID){
	TCHAR strOut[80];
	memset(strOut,0,80 * sizeof(TCHAR));
     _stprintf( strOut, TEXT("{%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X}"),
                pGuidIn.Data1, pGuidIn.Data2, pGuidIn.Data3,
                pGuidIn.Data4[0], pGuidIn.Data4[1],
                pGuidIn.Data4[2], pGuidIn.Data4[3],
                pGuidIn.Data4[4], pGuidIn.Data4[5],
                pGuidIn.Data4[6], pGuidIn.Data4[7] );
	 //DEBUGMSG(1, (strOut));
	 wcscpy(szGUID, strOut);
	 return szGUID;
}

//see also http://msdn.microsoft.com/en-us/library/aa447466.aspx
DWORD MonitorDevices(LPVOID param)
{
	HWND g_hWndEdit = (HWND)param;
	DWORD dwRes = 0;
	GUID guid = {0};    // or any known and relevant device interface GUID
	HANDLE hNotify;
	HANDLE qStore ;
	DWORD flags;
	DWORD size;
	BYTE DevDetail[sizeof(DEVDETAIL) + (MAX_PATH * sizeof( TCHAR ))];
	DEVDETAIL * pDetail = (DEVDETAIL *)DevDetail;
	MSGQUEUEOPTIONS msgopts;

	msgopts.dwSize = sizeof(MSGQUEUEOPTIONS);
	msgopts.dwFlags = 0; //MSGQUEUE_VARIABLESIZE | MSGQUEUE_MSGSIZE;
	msgopts.dwMaxMessages = 0;
	msgopts.cbMaxMessage = sizeof(DevDetail);
	msgopts.bReadAccess = TRUE;
//	msgopts.dwDesiredAccess = GENERIC_READ;
//	msgopts.dwShareMode = 0;
//	msgopts.dwCreationDisposition = CREATE_NEW;

	qStore = CreateMsgQueue(NULL, &msgopts);
	hStopEventHandle = CreateEvent(NULL, FALSE, FALSE, szStopThreadEvent);

/*
"IClass"="{f8a6ba98-087a-43ac-a9d8-b7f13c5bae31}"; This is DEVCLASS_STREAM_GUID
In case you follow my suggestion going for CE_DRIVER_SERIAL_PORT_GUID you will need:
"IClass"="{CC5195AC-BA49-48a0-BE17-DF6D1B0173DD}" ; this is CE_DRIVER_SERIAL_PORT_GUID
*/
	//hNotify = RequestDeviceNotifications(NULL /* for ALL */, qStore, FALSE /*FALSE report new only */);
	hNotify = RequestDeviceNotifications(NULL /* for ALL */, qStore, TRUE  /*TRUE report all */);

	HANDLE hWaitHandles[2];
	hWaitHandles[0]=qStore;
	hWaitHandles[1]=hStopEventHandle;
	DWORD dwWaitRes=0;

	//EditBox update
	int tLen=0;

	do
	{
		dwWaitRes=WaitForMultipleObjects(2, hWaitHandles, FALSE, 5000);
		switch (dwWaitRes){
			case WAIT_OBJECT_0:
				DEBUGMSG(1, (L"got WAIT_OBJECT_0 \n"));
				while(ReadMsgQueue(qStore, pDetail, sizeof(DevDetail), &size, 1, &flags))
				{
					TCHAR szText[100];
					TCHAR szGUID[80];
					TCHAR szInfo[40];
					TCHAR szText2[MAX_PATH];

					dumpGUID(pDetail->guidDevClass, szGUID);
					wcsncpy(szText, pDetail->szName, pDetail->cbName );
					wsprintf(szText2, L"%s %s %s %s", szText, getIClassInfo(szGUID, szInfo), szGUID, pDetail->fAttached?L"attached":L"detached");
					
					nclog(L"%s: %s\n", logDateTime(), szText2);

					PostMessage(g_hWndEdit, WM_PNPUPDATE, (WPARAM)szText2, 0);

					//LPOLESTR lplpsz = (LPOLESTR) malloc(80);
					//HRESULT hRes = StringFromCLSID(pDetail->guidDevClass, &lplpsz);
					//getIClassInfo(lplpsz, szText);
					//DEBUGMSG(1, (L"StringFromCLSID: %s:%s %s\n", lplpsz, szText, pDetail->fAttached?L"attached":L"detached"));
				}
				break;
			case WAIT_OBJECT_0+1:
				DEBUGMSG(1, (L"got WAIT_OBJECT_0+1 \n"));
				bStopMonitor=TRUE;
				break;
			case WAIT_TIMEOUT:
				DEBUGMSG(1, (L"got WAIT_TIMEOUT \n"));
				break;
			default:
				DEBUGMSG(1, (L"got default! \n"));
				break;
		}
		/*
		if(WaitForSingleObject(qStore, 5000) == WAIT_OBJECT_0)
		{
			while(ReadMsgQueue(qStore, pDetail, sizeof(DevDetail), &size, 1, &flags))
			{
				if(pDetail->fAttached){
					TCHAR szText[100];
					wsprintf(szText, L"%s", (TCHAR*)(pDetail->szName));
					PostMessage(hWnd, WM_PNPUPDATE, (WPARAM)szText, 0); ;//do something HandleNewDisk(pDetail->szName);
				}				
			}
		}
		*/
	} while( !bStopMonitor );

	StopDeviceNotifications(hNotify);
	CloseMsgQueue(qStore);

	DEBUGMSG(1, (L"monitor ended!\n"));
	nclog(L"monitor ended!\n");
	return dwRes;
}

int startMonitor(HWND hWnd){
	nclog_LogginEnabled=TRUE;
	nclog(L"");
	int iRet=0;
	bStopMonitor=FALSE;
	dwThreadHandle = CreateThread(NULL, 0, MonitorDevices, (LPVOID) hWnd, 0, &dwThreadID);
	if(dwThreadHandle==NULL){
		iRet=GetLastError();
		DEBUGMSG(1, (L"startMonitor failed with error=%i\n", iRet));
		nclog(L"startMonitor failed with error=%i\n", iRet);
	}
	else{
		DEBUGMSG(1, (L"startMonitor OK\n"));
		nclog(L"startMonitor OK\n");
	}
	return iRet;
}

void stopMonitor(){
	if(hStopEventHandle!=NULL){
		SetEvent(hStopEventHandle);
		Sleep(500);
	}
}
