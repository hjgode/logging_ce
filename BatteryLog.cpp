// BatteryLog.cpp : Defines the entry point for the application.
//
//use the follwoing to keep a BT discovery running in background
#define USE_BT_THREAD
#undef USE_BT_THREAD

#define USE_ANI_THREAD
//#undef USE_ANI_THREAD

#include "stdafx.h"
#include "wirelessdevices.h"

#include <itc50.h>
#pragma comment (lib, "itc50.lib")

#include "BatteryLog.h"
#include "Log2File.h"

#include "nclog.h"

//#include <winsock2.h>
#include <bt_api.h>

#define SEKUNDE	1000
#define MINUTE 60000

int BatCapacity = 4200; //battery capacity
bool bUseDLL=true; //false;
bool bUseBCAST=false;

//bT stuff
void PopulateBtDevList(void/* HWND hDlg */);
int btClientConnect(void);

//PM stuff
HANDLE    g_hPowerNotificationThread = NULL; 
DWORD PowerNotificationThread(LPVOID pVoid);
HANDLE    g_hEventShutDown = NULL;

/* List of known power notifications
#define PBT_TRANSITION          0x00000001  // broadcast specifying system power state transition
#define PBT_RESUME              0x00000002  // broadcast notifying a resume, specifies previous state
#define PBT_POWERSTATUSCHANGE   0x00000004  // power supply switched to/from AC/DC
#define PBT_POWERINFOCHANGE     0x00000008  // some system power status field has changed

#define PBT_SUSPENDKEYPRESSED   0x00000100  // Suspend Key has been pressed.

FLAGS:

#define POWER_STATE_ON           (DWORD)(0x00010000)        // on state
#define POWER_STATE_OFF          (DWORD)(0x00020000)        // no power, full off
#define POWER_STATE_CRITICAL     (DWORD)(0x00040000)        // critical off
#define POWER_STATE_BOOT         (DWORD)(0x00080000)        // boot state
#define POWER_STATE_IDLE         (DWORD)(0x00100000)        // idle state
#define POWER_STATE_SUSPEND      (DWORD)(0x00200000)        // suspend state
#define POWER_STATE_UNATTENDED   (DWORD)(0x00400000)        // Unattended state.
#define POWER_STATE_RESET        (DWORD)(0x00800000)        // reset state
#define POWER_STATE_USERIDLE     (DWORD)(0x01000000)        // user idle state
#define POWER_STATE_BACKLIGHTON  (DWORD)(0x02000000)        // device scree backlight on
#define POWER_STATE_PASSWORD     (DWORD)(0x10000000)        // This state is password protected.
*/

//global for arguments array
int          argc; 
TCHAR**       argv; 

TCHAR startPath[MAX_PATH];

TCHAR *szFlagFile = L"\\Windows\\_stop_battery_log.flag";

//thread stuff
HANDLE hThread=NULL;
BOOL bKeepRunning=false;
DWORD idThread=0;

// Returns number of elements
#define dim(x) (sizeof(x) / sizeof(x[0])) 

//===================================================================================
int getAppPath(TCHAR strPath[MAX_PATH])
{
	//get the app dir
	GetModuleFileName (NULL, strPath, MAX_PATH);
	//find the last backslash
	TCHAR* p;
	if ( p = wcsrchr ( strPath, '\\')) 
		*++p = _T('\0'); // zero-terminate at the last backslash
	wsprintf(startPath, strPath);
	return 0;
}

int SplitArgs(LPTSTR lpCmdLine)
{
  unsigned int i; 
  int          j; 
  // parse a few of the command line arguments 
  // a space delimites an argument except when it is inside a quote 

  argc = 1; 
  int pos = 0; 
  for (i = 0; i < wcslen(lpCmdLine); i++) 
    { 
    while (lpCmdLine[i] == ' ' && i < wcslen(lpCmdLine)) 
      { 
      i++; 
      } 
    if (lpCmdLine[i] == '\"') 
      { 
      i++; 
      while (lpCmdLine[i] != '\"' && i < wcslen(lpCmdLine)) 
        { 
        i++; 
        pos++; 
        } 
      argc++; 
      pos = 0; 
      } 
    else 
      { 
      while (lpCmdLine[i] != ' ' && i < wcslen(lpCmdLine)) 
        { 
        i++; 
        pos++; 
        } 
      argc++; 
      pos = 0; 
      } 
    } 

  argv = (TCHAR**)malloc(sizeof(TCHAR*)* (argc+1)); 

  argv[0] = (TCHAR*)malloc(1024); 
  GetModuleFileName(0, argv[0],1024); 

  for(j=1; j<argc; j++) 
    { 
    argv[j] = (TCHAR*)malloc(wcslen(lpCmdLine)+10); 
    } 
  argv[argc] = 0; 

  argc = 1; 
  pos = 0; 
  for (i = 0; i < wcslen(lpCmdLine); i++) 
    { 
    while (lpCmdLine[i] == ' ' && i < wcslen(lpCmdLine)) 
      { 
      i++; 
      } 
    if (lpCmdLine[i] == '\"') 
      { 
      i++; 
      while (lpCmdLine[i] != '\"' && i < wcslen(lpCmdLine)) 
        { 
        argv[argc][pos] = lpCmdLine[i]; 
        i++; 
        pos++; 
        } 
      argv[argc][pos] = '\0'; 
      argc++; 
      pos = 0; 
      } 
    else 
      { 
      while (lpCmdLine[i] != ' ' && i < wcslen(lpCmdLine)) 
        { 
        argv[argc][pos] = lpCmdLine[i]; 
        i++; 
        pos++; 
        } 
      argv[argc][pos] = '\0'; 
      argc++; 
      pos = 0; 
      } 
    } 
  argv[argc] = 0; 

// Initialize the processes and start the application. 
//  retVal = MyMain(argc, argv); 

  // Delete arguments 
  TCHAR str[MAX_PATH+1];
  for(j=0; j<argc; j++) 
	{ 
		wsprintf(str, L"arg %i = %s\n", j, argv[j]);
		OutputDebugString(str);
		//free(argv[j]); 
	} 
  //free(argv); 
	return argc;
}

bool existFile(TCHAR f[MAX_PATH])
{
	FILE *stream;
	/* Open for read (will fail if file "data" does not exist) */
	if( (stream  = _wfopen( f, L"r" )) == NULL )
		return false;
	else
	{
		fclose(stream);
		return true;
	}
}

void dumpBattery(TCHAR* szPbb)
{
	SYSTEM_POWER_STATUS_EX2 pwrs;
	
	int iRes = GetSystemPowerStatusEx2(&pwrs, sizeof(SYSTEM_POWER_STATUS_EX2), true);
	TCHAR str[1024];
	TCHAR lpTimeStr[32];
	TCHAR lpDateStr[32];
	int res = GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
							TIME_FORCE24HOURFORMAT,
							NULL,
							L"hhmmss",
							lpTimeStr,
							sizeof (lpTimeStr ) * sizeof(TCHAR));
	if (res == 0)
	{
		wcscpy(lpTimeStr, L"000000");
	}

	//Read the system date
	res = GetDateFormat(  LOCALE_SYSTEM_DEFAULT,
						  NULL,
						  NULL,
						  L"yyyyMMdd",
						  lpDateStr,
						  sizeof (lpDateStr) * sizeof(TCHAR));
	if (res == 0)
	{
		wcscpy(lpDateStr, L"19610101");
	}

	//convert to hours and minutes: ie 13.50 => 13 hours and 30 Minutes
	double BatLife = (double)BatCapacity / (double)pwrs.BatteryCurrent;
	int hhBatLife = (int) BatLife;
	int mmBatLife = (int)(((BatLife - hhBatLife)*100) * 0.6);

	DWORD dwWifi=-1; 
	DWORD dwPhone=-1;
	DWORD dwBT=-1;
	if (bUseDLL){
		GetWDevState(&dwWifi, &dwPhone, &dwBT);
	}

	DWORD dwBright=0;
	ITCGetScreenBrightness(&dwBright);

	int iBKLon=-1;
	ITCGetScreenLight(&iBKLon);

	wsprintf(str,L"%s%s\t%04u\t%04u\t%04u\t%04u\t%04u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%02i:%02i\t%2i\t%2i\t%2i\t%2i\t%2i\t\"%s\"\n",
				   lpDateStr, lpTimeStr,
				   pwrs.ACLineStatus, pwrs.BatteryFlag, pwrs.BatteryLifePercent, 
				   pwrs.BackupBatteryFlag, pwrs.BackupBatteryLifePercent, 
				   pwrs.BatteryVoltage, pwrs.BatteryCurrent, pwrs.BatteryAverageCurrent, pwrs.BatteryAverageInterval, pwrs.BatterymAHourConsumed,
				   pwrs.BatteryTemperature, pwrs.BackupBatteryVoltage, hhBatLife, mmBatLife,
				   dwWifi, dwPhone, dwBT,
				   dwBright, iBKLon,
				   szPbb);
	Add2Log(str, false);

	if (bUseBCAST){
		const int buffSize=24;
		TCHAR number[buffSize-1];
		if (ITC_ISSUCCESS(ITCGetSerialNumber(number, buffSize)))
			nclog(L"%s:\t%s", number, str);
		else
			nclog(L"unknown:\t%s", str);
	}
	DEBUGMSG(true, (L"%s", str));
}

void dumpBattery()
{
	SYSTEM_POWER_STATUS_EX2 pwrs;
	
	int iRes = GetSystemPowerStatusEx2(&pwrs, sizeof(SYSTEM_POWER_STATUS_EX2), true);
/*
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
    BYTE Reserved2;
    BYTE BackupBatteryFlag;
    BYTE BackupBatteryLifePercent;
    BYTE Reserved3;
    DWORD BackupBatteryLifeTime;
    DWORD BackupBatteryFullLifeTime;
    // Above here is old struct, below are new fields
    DWORD BatteryVoltage; 				// Reports Reading of battery voltage in millivolts (0..65535 mV)
    DWORD BatteryCurrent;				// Reports Instantaneous current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
    DWORD BatteryAverageCurrent; 		// Reports short term average of device current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
    DWORD BatteryAverageInterval;		// Reports time constant (mS) of integration used in reporting BatteryAverageCurrent	
    DWORD BatterymAHourConsumed; 		// Reports long-term cumulative average DISCHARGE (mAH). Reset by charging or changing the batteries. 0 to 32767 mAH  
    DWORD BatteryTemperature;			// Reports Battery temp in 0.1 degree C (-3276.8 to 3276.7 degrees C)
    DWORD BackupBatteryVoltage;			// Reports Reading of backup battery voltage
    BYTE  BatteryChemistry; 		    // See Chemistry defines above
*/
	TCHAR str[1024];
	TCHAR lpTimeStr[32];
	TCHAR lpDateStr[32];
	int res = GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
							TIME_FORCE24HOURFORMAT,
							NULL,
							L"hhmmss",
							lpTimeStr,
							sizeof (lpTimeStr ) * sizeof(TCHAR));
	if (res == 0)
	{
		wcscpy(lpTimeStr, L"00:00:00");
	}

	//Read the system date
	res = GetDateFormat(  LOCALE_SYSTEM_DEFAULT,
						  NULL,
						  NULL,
						  L"yyyyMMdd",
						  lpDateStr,
						  sizeof (lpDateStr) * sizeof(TCHAR));
	if (res == 0)
	{
		wcscpy(lpDateStr, L"01.01.1961");
	}

	//convert to hours and minutes: ie 13.50 => 13 hours and 30 Minutes
	double BatLife = (double)BatCapacity / (double)pwrs.BatteryCurrent;
	int hhBatLife = (int) BatLife;
	int mmBatLife = (int)(((BatLife - hhBatLife)*100) * 0.6);

	DWORD dwWifi=-1; 
	DWORD dwPhone=-1;
	DWORD dwBT=-1;
	if (bUseDLL){
		GetWDevState(&dwWifi, &dwPhone, &dwBT);
		//if(dwWifi == 1)
		//	str=TEXT("ON");
		//DeinitDLL();
	}

	DWORD dwBright=0;
	ITCGetScreenBrightness(&dwBright);

	int iBKLon=-1;
	ITCGetScreenLight(&iBKLon);

	wsprintf(str, L"%s%s\t%04u\t%04u\t%04u\t%04u\t%04u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%02i:%02i\t%2i\t%2i\t%2i\t%2i\t%2i\t\"-\"\n",
				   lpDateStr, lpTimeStr,
				   pwrs.ACLineStatus, pwrs.BatteryFlag, pwrs.BatteryLifePercent, 
				   pwrs.BackupBatteryFlag, pwrs.BackupBatteryLifePercent, 
				   pwrs.BatteryVoltage, pwrs.BatteryCurrent, pwrs.BatteryAverageCurrent, pwrs.BatteryAverageInterval, pwrs.BatterymAHourConsumed,
				   pwrs.BatteryTemperature, pwrs.BackupBatteryVoltage, 
				   hhBatLife, mmBatLife,
				   dwWifi, dwPhone, dwBT,
				   dwBright, iBKLon);
	Add2Log(str, false);

	if (bUseBCAST){
		const int buffSize=24;
		TCHAR number[buffSize-1];
		if (ITC_ISSUCCESS(ITCGetSerialNumber(number, buffSize)))
			nclog(L"%s:\t%s", number, str);
		else
			nclog(L"unknown:\t%s", str);
	}
	DEBUGMSG(true, (L"%s", str));
}

DWORD ThreadProc(LPVOID lpParameter)
{
	do{
		dumpBattery();
		Sleep((ULONG) lpParameter);
	}while (bKeepRunning);
	return 0;
}


DWORD ThreadProcBT(LPVOID lpParameter)
{
	WSADATA wsaData;
	// Initialize Winsock
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	TCHAR msg[MAX_PATH];
	
	if (int iErr=WSAStartup( wVersionRequested, &wsaData ) != 0)
	{		
		wsprintf(msg, L"Winsock Init error: %i", iErr);
		MessageBox(NULL, msg, _T("Fatal Error"), MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		return -1;
	}
	else{
		DEBUGMSG(1, (L"Winsock started\n"));
	}
	while(true){
		PopulateBtDevList();
		btClientConnect();
#ifdef DEBUG
		Sleep(10000);
#else
		Sleep(60000);
#endif
	}
}

/**************************************************************/
void PopulateBtDevList(void/* HWND hDlg */)
{
	INT				iResult = 0;
	LPWSAQUERYSET	pwsaResults;
	DWORD			dwSize = 0;
	WSAQUERYSET		wsaq;
//	HCURSOR			hCurs;
	HANDLE			hLookup = 0;
	
	// empty the list box
	//SendMessage(GetDlgItem(hDlg, IDC_DEVLIST),LB_RESETCONTENT,0,0);
	
	//SendMessage(GetDlgItem(hDlg, IDC_DEVLIST),LB_ADDSTRING,0,(LPARAM) _T("Results...")); 
	
	memset (&wsaq, 0, sizeof(wsaq));
	wsaq.dwSize      = sizeof(wsaq);
	wsaq.dwNameSpace = NS_BTH;
	wsaq.lpcsaBuffer = NULL;
	
	// initialize searching procedure
	iResult = WSALookupServiceBegin(&wsaq, 
		LUP_CONTAINERS, 
		&hLookup);
	
	if (iResult != 0)
	{
		TCHAR tszErr[32];
		iResult = WSAGetLastError();		
		StringCchPrintf(tszErr, 32, _T("Socket Error: %d"), iResult);
		DEBUGMSG(1, (L"%s\n", tszErr));
		//MessageBox(hDlg, tszErr, _T("Error"), MB_OK);		
	}
	
	union {
		CHAR buf[5000];				// returned struct can be quite large 
		SOCKADDR_BTH	__unused;	// properly align buffer to BT_ADDR requirements
	};
	
	// save the current cursor
	//hCurs = GetCursor();
	
	for (; ;)
	{
		// set the wait cursor while searching
		//SetCursor(LoadCursor(NULL, IDC_WAIT));
		pwsaResults = (LPWSAQUERYSET) buf;
		
		dwSize  = sizeof(buf);
		
		memset(pwsaResults,0,sizeof(WSAQUERYSET));
		pwsaResults->dwSize      = sizeof(WSAQUERYSET);
		// namespace MUST be NS_BTH for bluetooth queries
		pwsaResults->dwNameSpace = NS_BTH;
		pwsaResults->lpBlob      = NULL;
		
		// iterate through all found devices, returning name and address
		// (this sample only uses the name, but address could be used for
		// further queries)
		iResult = WSALookupServiceNext (hLookup, 
			LUP_RETURN_NAME | LUP_RETURN_ADDR, 
			&dwSize, 
			pwsaResults);
				
		if (iResult != 0)
		{
			iResult = WSAGetLastError();
			if (iResult != WSA_E_NO_MORE)
			{
				TCHAR tszErr[32];
				iResult = WSAGetLastError();
				StringCchPrintf(tszErr, 32, _T("Socket Error: %d\n"), iResult);
				//MessageBox(hDlg, tszErr, _T("Error"), MB_OK);
				DEBUGMSG(1, (L"Error %s\n", tszErr));
			}
			// we're finished
			break;
		}
		
		// add the name to the listbox
		if (pwsaResults->lpszServiceInstanceName)
		{
//			TCHAR wTxt[MAX_PATH];
			DEBUGMSG(1, (L"Found BT device: '%s'\n", (LPARAM)pwsaResults->lpszServiceInstanceName));
			nclog( L"Found BT device: '%s'\n", (LPARAM)pwsaResults->lpszServiceInstanceName);
			//GUID* theGUID = pwsaResults->lpServiceClassId;
			//DEBUGMSG(1, (L"\tServiceClassID=%u\n", theGUID));
			bt_addr sa = ((SOCKADDR_BTH *)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;
			DEBUGMSG(1, (L"\tBT_ADDress=%x\n", sa));
			//SendMessage(GetDlgItem(hDlg, IDC_DEVLIST), LB_ADDSTRING, 0, (LPARAM)pwsaResults->lpszServiceInstanceName);
		}
	}
	WSALookupServiceEnd(hLookup);
}

int btClientConnect(){
	WSADATA wsd;
	BT_ADDR b = 0x06660309E8;
	WSAStartup (MAKEWORD(1,0), &wsd);
	SOCKET client_socket = socket (AF_BT, SOCK_STREAM, BTHPROTO_RFCOMM);
	SOCKADDR_BTH sa;
	memset (&sa, 0, sizeof(sa));
	sa.btAddr = b; //b is a BT_ADDR variable
	//sa.serviceClassId=FaxServiceClass_UUID;
	ULONG channel = 1124;
	sa.port = channel & 0xff;
	if (connect (client_socket, (SOCKADDR *)&sa, sizeof(sa))==SOCKET_ERROR) 
	{
		//Perform error handling.
		DEBUGMSG(1, (L"Socket Connect Error: %l\n", WSAGetLastError()));
		closesocket (client_socket);
		return -1;
	}
	else
		DEBUGMSG(1, (L"Socket Connect OK.\n"));

	char* pBuf = new char[MAX_PATH];
	memset((void*)pBuf, 0, MAX_PATH);
	int len = MAX_PATH;

	sprintf(pBuf, "NEW\nPF\n");
	len=strlen(pBuf);

	int flgs = 0;
	int iRes = send(client_socket, pBuf, len, flgs);
	if(iRes==SOCKET_ERROR)
		DEBUGMSG(1, (L"Socket Send Error: %l\n", GetLastError()));
	else
		DEBUGMSG(1, (L"Socket Send OK.\n"));

	closesocket(client_socket);
	CloseHandle ((LPVOID)client_socket);
	return 0;
}

void dumpPowerFlags(POWER_BROADCAST* ppb, TCHAR* szPPB){
	wsprintf(szPPB, L"");
	if(ppb->Flags & POWER_STATE_ON)
		wcscat(szPPB, L"ON|");
	if(ppb->Flags & POWER_STATE_OFF)
		wcscat(szPPB, L"OFF|");
	if(ppb->Flags & POWER_STATE_BOOT)
		wcscat(szPPB, L"ON|");
	if(ppb->Flags & POWER_STATE_CRITICAL)
		wcscat(szPPB, L"CRITICAL|");
	if(ppb->Flags & POWER_STATE_IDLE)
		wcscat(szPPB, L"IDLE|");
	if(ppb->Flags & POWER_STATE_PASSWORD)
		wcscat(szPPB, L"PASSWORD|");
	if(ppb->Flags & POWER_STATE_BACKLIGHTON)
		wcscat(szPPB, L"BACKLIGHTON|");
	if(ppb->Flags & POWER_STATE_SUSPEND)
		wcscat(szPPB, L"SUSPEND|");
	if(ppb->Flags & POWER_STATE_UNATTENDED)
		wcscat(szPPB, L"UNATTENDED|");
	if(ppb->Flags & POWER_STATE_RESET)
		wcscat(szPPB, L"RESET|");
	if(ppb->Flags & POWER_STATE_USERIDLE)
		wcscat(szPPB, L"USERIDLE|");
	wcscat(szPPB, L"\0");
}
//***************************************************************************
// Function Name: PowerNotificationThread
//
// Purpose: listens for power change notifications
//
DWORD PowerNotificationThread(LPVOID pVoid)
{
    // size of a POWER_BROADCAST message
    DWORD cbPowerMsgSize = sizeof POWER_BROADCAST + (MAX_PATH * sizeof TCHAR);

    // Initialize our MSGQUEUEOPTIONS structure
    MSGQUEUEOPTIONS mqo;
    mqo.dwSize = sizeof(MSGQUEUEOPTIONS); 
    mqo.dwFlags = MSGQUEUE_NOPRECOMMIT;
    mqo.dwMaxMessages = 4;
    mqo.cbMaxMessage = cbPowerMsgSize;
    mqo.bReadAccess = TRUE;              
                                         
    // Create a message queue to receive power notifications
    HANDLE hPowerMsgQ = CreateMsgQueue(NULL, &mqo);
    if (NULL == hPowerMsgQ) 
    {
        RETAILMSG(1, (L"CreateMsgQueue failed: %x\n", GetLastError()));
        goto Error;
    }

    // Request power notifications 
    HANDLE hPowerNotifications = RequestPowerNotifications(hPowerMsgQ,
                                                           PBT_TRANSITION | 
                                                           PBT_RESUME | 
                                                           PBT_POWERINFOCHANGE);
    if (NULL == hPowerNotifications) 
    {
        RETAILMSG(1, (L"RequestPowerNotifications failed: %x\n", GetLastError()));
        goto Error;
    }

    HANDLE rgHandles[2] = {0};
    rgHandles[0] = hPowerMsgQ;
    rgHandles[1] = g_hEventShutDown;

    // Wait for a power notification or for the app to exit
    while(WaitForMultipleObjects(2, rgHandles, FALSE, INFINITE) == WAIT_OBJECT_0)
    {
        DWORD cbRead;
        DWORD dwFlags;
        POWER_BROADCAST *ppb = (POWER_BROADCAST*) new BYTE[cbPowerMsgSize];
        TCHAR* szPPB = new TCHAR[MAX_PATH];  
		memset(szPPB, 0, MAX_PATH * 2);
		TCHAR* szPBtype=new TCHAR[MAX_PATH];
		memset(szPBtype, 0, MAX_PATH * 2);
		TCHAR szOut[MAX_PATH];
        // loop through in case there is more than 1 msg 
        while(ReadMsgQueue(hPowerMsgQ, ppb, cbPowerMsgSize, &cbRead, 
                           0, &dwFlags))
        {
			wsprintf(szPPB, L"");
			wsprintf(szPBtype, L"");
            switch (ppb->Message)
            {
				case PBT_POWERSTATUSCHANGE:
                    RETAILMSG(1,(L"Power Notification Message: PBT_POWERSTATUSCHANGE\n"));
					wsprintf(szPBtype, L"change: ");
					//Add2Log(L"Power Notification Message: PBT_POWERSTATUSCHANGE\n",TRUE);
					break;
				case PBT_SUSPENDKEYPRESSED:
                    RETAILMSG(1,(L"Power Notification Message: PBT_SUSPENDKEYPRESSED\n"));
					wsprintf(szPBtype, L"keypress: ");
					//Add2Log(L"Power Notification Message: PBT_SUSPENDKEYPRESSED\n",TRUE);
					break;
                case PBT_TRANSITION:
                    RETAILMSG(1,(L"Power Notification Message: PBT_TRANSITION\n"));
					//Add2Log(L"Power Notification Message: PBT_TRANSITION\n",TRUE);
                    RETAILMSG(1,(L"Flags: %lx\n", ppb->Flags));
                    RETAILMSG(1,(L"Length: %d\n", ppb->Length));
					wsprintf(szPBtype, L"trans.: ");
                    break;

                case PBT_RESUME:
                    RETAILMSG(1,(L"Power Notification Message: PBT_RESUME\n"));
					//Add2Log(L"Power Notification Message: PBT_RESUME\n",TRUE);
					wsprintf(szPBtype, L"resume: ");
                    break;

                case PBT_POWERINFOCHANGE:
                {
                    RETAILMSG(1,(L"Power Notification Message: PBT_POWERINFOCHANGE\n"));
					//Add2Log(L"Power Notification Message: PBT_POWERINFOCHANGE\n", TRUE);
                    // PBT_POWERINFOCHANGE message embeds a 
                    // POWER_BROADCAST_POWER_INFO structure into the 
                    // SystemPowerState field
					wsprintf(szPBtype, L"info: ");

                    //PPOWER_BROADCAST_POWER_INFO ppbpi =
                    //    (PPOWER_BROADCAST_POWER_INFO) ppb->SystemPowerState;
                    //if (ppbpi) 
                    //{
                    //    RETAILMSG(1,(L"Length: %d\n", ppb->Length));
                    //    RETAILMSG(1,(L"BatteryLifeTime = %d\n",ppbpi->dwBatteryLifeTime));
                    //    RETAILMSG(1,(L"BatterFullLifeTime = %d\n",
                    //                 ppbpi->dwBatteryFullLifeTime));
                    //    RETAILMSG(1,(L"BackupBatteryLifeTime = %d\n",
                    //                 ppbpi->dwBackupBatteryLifeTime));
                    //    RETAILMSG(1,(L"BackupBatteryFullLifeTime = %d\n",
                    //                 ppbpi->dwBackupBatteryFullLifeTime));
                    //    RETAILMSG(1,(L"ACLineStatus = %d\n",ppbpi->bACLineStatus));
                    //    RETAILMSG(1,(L"BatteryFlag = %d\n",ppbpi->bBatteryFlag));
                    //    RETAILMSG(1,(L"BatteryLifePercent = %d\n",
                    //                 ppbpi->bBatteryLifePercent));
                    //    RETAILMSG(1,(L"BackupBatteryFlag = %d\n",
                    //                 ppbpi->bBackupBatteryFlag));
                    //    RETAILMSG(1,(L"BackupBatteryLifePercent = %d\n",
                    //                 ppbpi->bBackupBatteryLifePercent));
                    //}
                    break;
                }

                default:
                    break;
            }
            //dumpBattery();
			dumpPowerFlags(ppb, szPPB);
			wsprintf(szOut, L"%s%s", szPBtype, szPPB);
			dumpBattery(szOut);
        }
        delete[] ppb;
		delete(szPPB);
		delete(szPBtype);
    }

Error:
    if (hPowerNotifications)
        StopPowerNotifications(hPowerNotifications);

    if (hPowerMsgQ)
        CloseMsgQueue(hPowerMsgQ);

    return NULL;
}

//####################### Activity thread ############################
#ifdef USE_ANI_THREAD
	//the animation line showing the status
	int		lineHeight=4;		//progress bar height
	int		drawPointX=80;		//start to draw a line (horizontal pos)
	int		drawPointY=4;		//start to draw a line (vertical pos)
	int		lineLength = 4; //GetSystemMetrics(SM_CXSCREEN);
	HDC		hdc = NULL;	//get DC of desktop window
	HPEN	po;	// Pen Progress bar
	HPEN	lineBack, lineRed, lineYellow, lineGreen;
	//thread vars
	DWORD idThreadAnimation=0;
	HANDLE hThreadAnimation=NULL;
	BOOL	runThread=TRUE; //var to stop animation thread

int DrawLine(int pos){
	static BOOL bToggle=false;
	
	//hdc = GetDC(GetForegroundWindow());
	HWND hwndDraw;
	hwndDraw=FindWindow(L"HHTASKBAR", NULL);
	HDC hdc = GetDC(hwndDraw);

	//move start point to x,y (ie 0,319
	MoveToEx(hdc,drawPointX,drawPointY,NULL);

	//draw a line to pos/drawpointY
	//Sleep(300);
	HPEN po;
	if(bToggle){
		po=(HPEN)SelectObject(hdc,lineGreen);
		//ie to 30/2,319
		LineTo(hdc, lineLength + drawPointX, drawPointY); //draw from current point to new point
	}
	else{
		po = (HPEN)SelectObject(hdc,lineBack); 
		//ie to 30,319
		LineTo(hdc, lineLength + drawPointX, drawPointY); 
	}
	bToggle=!bToggle;

	MoveToEx(hdc,drawPointX,drawPointY,NULL); //move back
	ReleaseDC(hwndDraw, hdc);
	UpdateWindow(hwndDraw);
	return 0;
}

//background thread to draw an animation
DWORD myThreadAnimation(LPVOID lpParam){
	static BOOL bToggle=false;

	//line color pens
	lineBack = CreatePen(PS_SOLID, lineHeight, RGB(127,127,127));
	lineYellow = CreatePen(PS_SOLID, lineHeight, RGB(255,255,0));
	lineRed = CreatePen(PS_SOLID, lineHeight, RGB(255,0,0));
	lineGreen = CreatePen(PS_SOLID, lineHeight, RGB(0,255,0));

//	HWND hDesk;
	do{
		DrawLine(lineLength);
		bToggle=!bToggle;
		Sleep(1000);
	}while(runThread);
	ShowWindow(FindWindow(L"HHTaskbar",NULL), SW_HIDE);
	Sleep(10);
	ShowWindow(FindWindow(L"HHTaskbar",NULL), SW_SHOWNORMAL);
	ExitThread(0);

	return 0;
}
#endif

/************************************************************************/
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
 	// TODO: Place code here.
	TCHAR strP[MAX_PATH];
	int argc;
	uint iTime=10;
	TCHAR str[1024];
	//use the ossvc.dll?
	if (InitDLL())
		bUseDLL=true;
	else
		bUseDLL=false;
	argc = SplitArgs(lpCmdLine); //argc is count of arguments, argv[i] are the arguments
	GetModuleFileName(NULL, strP, MAX_PATH);
	getAppPath(strP);

	TCHAR logFileName[MAX_PATH];
	wsprintf(logFileName, L"%sbatterylog.txt", startPath);
	char logFileNameA[MAX_PATH];
	wcstombs(logFileNameA,logFileName, MAX_PATH);

	if (wcslen(lpCmdLine)==0){
		TCHAR szMsg[2048];
		wsprintf(szMsg, L"BatteryLog: invalid command line, missing argument\n \
			usage1: \"batterylog 10 4200\"\nrun BatteryLog and log battery status every 10 minutes, estimate for a 4200mAh capacity\n \
			usage2: \"batterylog kill\"\nkill all running BatteryLog processes");
		MessageBox(GetForegroundWindow(), szMsg, L"BatteryLog 2.3", MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
		//Add2Log(L"BatteryLog: invalid command line, missing argument\n", true);
		//Add2Log(L"usage: batterylog 10 \n\trun BatteryLog and log battery status every 10 minutes, estimation is done for 4000mAh capacity\n", false);
		//Add2Log(L"usage: batterylog 10 4200\n\trun BatteryLog and log battery status every 10 minutes, estimate for a 4200mAh capacity\n", false);
		//Add2Log(L"usage: batterylog kill\n\tkill all running BatteryLog processes\n", false);
		//MessageBeep(MB_ICONEXCLAMATION);
		return -1;
	}
	DEBUGMSG(true, (L"argc=%i\n", argc));
	if (argc>=2)
	{
		if (wcsicmp(L"kill", argv[1])==0)
		{
			FILE *stream;
			stream = _wfopen ( szFlagFile, L"w" );
			if (stream != NULL)
			{
				fwprintf(stream, L"file to stop battery logging");
				fclose(stream);
				return 1;
			}
			else
				return -2;
		}
		if (argc==3){
			DWORD d=_wtoi(argv[2]);
			if (d!=0)
				BatCapacity = d;
		}

		if (argc==4){
			if (wcsicmp(L"NOBCAST", argv[3]) == 0)
				bUseBCAST = false;
		}
		else
			bUseBCAST = true;

		int i=_wtoi(argv[1]);
		if ( (i>0) && (i<60) )
		{
			appendfile(logFileNameA);
			//newfile(logFileName);
			//print header
			wsprintf(str, L"DateTime \tACLineStatus \tBatteryFlag \tBatteryLifePercent \tBackupBatteryFlag \tBackupBatteryLifePercent \tBatteryVoltage \tBatteryCurrent \tBatteryAverageCurrent \tBatteryAverageInterval \tBatterymAHourConsumed \tBatteryTemperature \tBackupBatteryVoltage \testimated BattLife(for %i mAh) \twifi \tphone \tBT \tbklLevel \tBKL stat\tPowerMsg\n", BatCapacity);
			DEBUGMSG(true, (L"%s", str));
			Add2Log(str, false);
			iTime=i * MINUTE;// SEKUNDE; //use as minutes 60 * 1000mSeconds
#ifdef USE_BT_THREAD
			//create a BT thread
			HANDLE hThreadBT = CreateThread(NULL, 0, ThreadProcBT, &iTime, 0, &idThread);
#endif
#ifdef USE_ANI_THREAD
			//thread to animate line bar at bottom
			hThreadAnimation = CreateThread(NULL, 0, myThreadAnimation, NULL, 0, &idThreadAnimation);
			DEBUGMSG(1, (L"Animation CreateThread handle=%u\n", hThread));
#endif
			//create PowerNotificationThread
            // Start up the thread to get the power notifications
            g_hEventShutDown = CreateEvent(NULL, FALSE, FALSE, NULL);
			g_hPowerNotificationThread = CreateThread(0, 0, PowerNotificationThread, NULL, 0, 0);
			//SetThreadPriority(g_hPowerNotificationThread, THREAD_PRIORITY_HIGHEST);
			SetThreadPriority(g_hPowerNotificationThread, THREAD_PRIORITY_TIME_CRITICAL);

			//the main LOOP, iTime is 60000 to ?
			//check all 1000ms (1 sec) for exit
			//dumplog all iTime
			DWORD dwCount=0;
			do
			{
				if (dwCount >= iTime){
					dumpBattery();
					dwCount=0;
				}
				dwCount += 1000; //add one second
				Sleep(1000);
			}
			while (!existFile(szFlagFile));

			// or use a thread
			//hTread = CreateThread(NULL, 0, ThreadProc, &iTime, 0, &idThread);

			if(bUseDLL)
				DeinitDLL();
            
			// Kill the power notification thread
            if (NULL != g_hEventShutDown)
            {
                SetEvent(g_hEventShutDown);

                if (NULL != g_hPowerNotificationThread)
                {
                    WaitForSingleObject(g_hPowerNotificationThread , INFINITE);
                    CloseHandle(g_hPowerNotificationThread);
                }

                CloseHandle(g_hEventShutDown);
            }
			//
#ifdef USE_ANI_THREAD
			runThread=FALSE;
			Sleep(1000);
#endif
#ifdef USE_BT_THREAD
			if (hThreadBT!=NULL)
			{
				TerminateThread(hThreadBT, -1);
			}
			WSACleanup();
#endif
			DeleteFile(szFlagFile);
			MessageBeep(MB_OK);
			return 0;
		}
	}

	return 0;
}

