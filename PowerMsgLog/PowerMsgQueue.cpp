//PowerMsgQueue.cpp

#include "stdafx.h"
#include "PowerMsgQueue.h"
#include "nclog.h"

HANDLE hExitPwrMsgThreadEvent;
TCHAR szExitPwrMsgThreadEventName[MAX_PATH] = L"szExitPwrMsgThreadEventName";

HANDLE hPwrMsgThreadHasStoppedEvent;
TCHAR szPwrMsgThreadHasStoppedEventName[MAX_PATH] = L"szPwrMsgThreadHasStoppedEventName";

HANDLE hPwrMsgQueue;
HANDLE hPwrMsgQueueThread;

HWND hwndMain;

char*  logDateTimeA(){
	static char cDateTimeStr[64];
	TCHAR str[64];
	wsprintf(str, L"%s", logDateTime(str));
	wcstombs(cDateTimeStr, str, wcslen(str));
	return cDateTimeStr;
}
//
//int InitTransitionsPowerNotification(void)
//{
//	int iRet=0;
//	nclog(L"\tInitTransitionsPowerNotification()...\n");
//	MSGQUEUEOPTIONS mqoQueueOptions;
//	mqoQueueOptions.dwSize = sizeof(mqoQueueOptions); 
//	mqoQueueOptions.dwFlags = MSGQUEUE_NOPRECOMMIT;
//	mqoQueueOptions.dwMaxMessages = 0;
//	mqoQueueOptions.cbMaxMessage = sizeof(POWER_BROADCAST) + MAX_NAMELEN;
//	mqoQueueOptions.bReadAccess = TRUE;
//
//	hTransitionsStatusQueue = CreateMsgQueue(NULL, &mqoQueueOptions);
//	if(hTransitionsStatusQueue==NULL){
//		nclog(L"\tCreateMsgQueue failed.\n");
//		iRet=-1;
//	}
//	else{
//		if(RequestPowerNotifications(hTransitionsStatusQueue, PBT_TRANSITION)==0){ //PBT_TRANSITION  PBT_RESUME
//			nclog(L"\tRequestPowerNotifications failed.\n");
//			iRet=-2;
//		}
//	}
//	nclog(L"\tInitTransitionsPowerNotification() done with ret=%i\n", iRet);
//	return iRet;
//}

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
#ifdef POWER_STATE_PASSWORD
	if(ppb->Flags & POWER_STATE_PASSWORD)
		wcscat(szPPB, L"PASSWORD|");
	if(ppb->Flags & POWER_STATE_UNATTENDED)
		wcscat(szPPB, L"UNATTENDED|");
	if(ppb->Flags & POWER_STATE_USERIDLE)
		wcscat(szPPB, L"USERIDLE|");
#endif
#if _WIN32_WCE > 0x501
	if(ppb->Flags & POWER_STATE_BACKLIGHTON)
		wcscat(szPPB, L"BACKLIGHTON|");
#endif
	if(ppb->Flags & POWER_STATE_SUSPEND)
		wcscat(szPPB, L"SUSPEND|");
	if(ppb->Flags & POWER_STATE_RESET)
		wcscat(szPPB, L"RESET|");
	wcscat(szPPB, L"\0");
	if(wcslen(szPPB)==0)
		wsprintf(szPPB, L"no flags");
}

TCHAR* szAClineStatus(TCHAR*szIn, DWORD dwStatus){
	switch(dwStatus){
		case 0:
			wsprintf(szIn, L"offline");
			break;
		case 1:
			wsprintf(szIn, L"online");
			break;
		default:
			wsprintf(szIn, L"unknown");
			break;
	}
	return szIn;
}

TCHAR* szBatteryFlag(TCHAR* szIn, DWORD dwFlag){
	switch(dwFlag){
		case BATTERY_FLAG_HIGH:
			wsprintf(szIn, L"BATTERY_FLAG_HIGH");
			break;
		case BATTERY_FLAG_CRITICAL:
			wsprintf(szIn, L"BATTERY_FLAG_CRITICAL");
			break;
		case BATTERY_FLAG_CHARGING:
			wsprintf(szIn, L"BATTERY_FLAG_CHARGING");
			break;
		case BATTERY_FLAG_NO_BATTERY:
			wsprintf(szIn, L"BATTERY_FLAG_NO_BATTERY");
			break;
		case BATTERY_FLAG_LOW:
			wsprintf(szIn, L"BATTERY_FLAG_LOW");
			break;
		case BATTERY_FLAG_UNKNOWN:
			wsprintf(szIn, L"BATTERY_FLAG_UNKNOWN");
			break;
		default:
			wsprintf(szIn, L"UNKNOWN");
			break;
	}
	return szIn;
}

//***************************************************************************
// Function Name: PowerNotificationThread
//
// Purpose: listens for power change notifications
//
DWORD PowerMsgsThread(LPVOID pVoid)
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
                                                           PBT_TRANSITION 
														   | PBT_RESUME 
                                                           | PBT_POWERINFOCHANGE 
														   | PBT_POWERSTATUSCHANGE);
    if (NULL == hPowerNotifications) 
    {
        nclog(L"RequestPowerNotifications failed: %x\n", GetLastError());
        goto Error;
    }

	//create exit thread handle event
	hExitPwrMsgThreadEvent = CreateEvent(NULL, FALSE, FALSE, szExitPwrMsgThreadEventName);
	hPwrMsgThreadHasStoppedEvent = CreateEvent(NULL, FALSE, FALSE, szPwrMsgThreadHasStoppedEventName);

    HANDLE rgHandles[2] = {0};
    rgHandles[0] = hPowerMsgQ;	//will be signaled on power changes
    rgHandles[1] = hExitPwrMsgThreadEvent;	//used to signal the thread to exit
	int iPost=0;

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
		TCHAR szTemp[MAX_PATH];
        // loop through in case there is more than 1 msg 
        while(ReadMsgQueue(hPowerMsgQ, ppb, cbPowerMsgSize, &cbRead, 
                           0, &dwFlags))
        {
			wsprintf(szPPB, L"");
			wsprintf(szPBtype, L"");
			if(ppb->Message!=PBT_POWERINFOCHANGE)
				nclog(L"ReadMsgQueue: %s\n", ppb->SystemPowerState);
			else
				nclog(L"ReadMsgQueue: %s\n", L"PBT_POWERINFOCHANGE");
            switch (ppb->Message)
            {
				case PBT_POWERSTATUSCHANGE:
                    nclog(L"Power Notification Message: PBT_POWERSTATUSCHANGE\n");
					wsprintf(szPBtype, L"change: ");
					//Add2Log(L"Power Notification Message: PBT_POWERSTATUSCHANGE\n",TRUE);
					break;
#ifdef PBT_SUSPENDKEYPRESSED
				case PBT_SUSPENDKEYPRESSED:
                    nclog(L"Power Notification Message: PBT_SUSPENDKEYPRESSED\n");
					wsprintf(szPBtype, L"keypress: ");
					//Add2Log(L"Power Notification Message: PBT_SUSPENDKEYPRESSED\n",TRUE);
					break;
#endif
                case PBT_TRANSITION:
                    nclog(L"Power Notification Message: PBT_TRANSITION\n");
					//Add2Log(L"Power Notification Message: PBT_TRANSITION\n",TRUE);
                    nclog(L"Flags: %lx\n", ppb->Flags);
                    nclog(L"Length: %d\n", ppb->Length);
					wsprintf(szPBtype, L"trans.: ");
/*
Flags: 12010000
Length: 6
trans.: ON|PASSWORD|BACKLIGHTON|
*/
					//				 0x10000000				0x00010000		 0x02000000
#ifdef POWER_STATE_PASSWORD
					if( ((ppb->Flags & POWER_STATE_PASSWORD) == POWER_STATE_PASSWORD) ||
						((ppb->Flags & POWER_STATE_ON) == POWER_STATE_ON) 
	#if _WIN32_WCE > 0x501
						||
						((ppb->Flags & POWER_STATE_BACKLIGHTON) == POWER_STATE_BACKLIGHTON) 
	#endif
						)
#else
					if  ( (ppb->Flags & POWER_STATE_ON) == POWER_STATE_ON  )
#endif
					{
						nclog(L"PwrMsgQueue: got 'ON|PASSWORD|BACKLIGHTON'...\n");
						//send a message to main window
						//iPost = PostMessage(hwndMain, WM_USER_RESUMECOMM, 0, 0);
						//nclog(L"PostMessage WM_USER_RESUMECOMM returned %i\n", iPost);
					}
                    break;

                case PBT_RESUME:
                    nclog(L"Power Notification Message: PBT_RESUME\n");
					//Add2Log(L"Power Notification Message: PBT_RESUME\n",TRUE);
					wsprintf(szPBtype, L"resume: ");
					//send a message to main window
					//iPost = PostMessage(hwndMain, WM_USER_RESUMECOMM, 0, 0);
					//nclog(L"PostMessage WM_USER_RESUMECOMM returned %i\n", iPost);
					nclog(L"Power: PBT_RESUME\n");

                    break;

                case PBT_POWERINFOCHANGE:
                {
                    nclog(L"Power Notification Message: PBT_POWERINFOCHANGE\n");
                    // PBT_POWERINFOCHANGE message embedds a 
                    // POWER_BROADCAST_POWER_INFO structure into the 
                    // SystemPowerState field
					wsprintf(szPBtype, L"info: ");

                    PPOWER_BROADCAST_POWER_INFO ppbpi =
                        (PPOWER_BROADCAST_POWER_INFO) ppb->SystemPowerState;
					/*
					dwBatteryLifeTime
						Number of seconds of battery life remaining, or 0xFFFFFFFF if remaining seconds are unknown.
					dwBatteryFullLifeTime
						Number of seconds of battery life when at full charge, or 0xFFFFFFFF if full battery lifetime is unknown.
					dwBackupBatteryLifeTime
						Number of seconds of backup battery life remaining, or BATTERY_LIFE_UNKNOWN if remaining seconds are unknown.
					dwBackupBatteryFullLifeTime
						Number of seconds of backup battery life when at full charge, or BATTERY_LIFE_UNKNOWN if full battery lifetime is unknown.
					bACLineStatus
						AC power status. This member can be one of the values in the following table. Values not in the table are reserved.
							0	Offline
							1	Online
							255	Unknown status
					bBatteryFlag
						Battery charge status. This member can be a combination of the values in the following table. Values not in the table are reserved. 
							1	High
							2	Low
							4	Critical
							8	Charging
							128	No system battery
							255	Unknown status
					bBatteryLifePercent
						Percentage of full battery charge remaining. This member can be a value in the range 0 (zero) to 100, or 255 if the status is unknown. All other values are reserved.
					bBackupBatteryFlag
						Backup battery charge status. This member can be one of the following values:
							BATTERY_FLAG_HIGH
							BATTERY_FLAG_CRITICAL
							BATTERY_FLAG_CHARGING
							BATTERY_FLAG_NO_BATTERY
							BATTERY_FLAG_UNKNOWN
							BATTERY_FLAG_LOW
					bBackupBatteryLifePercent
						Percentage of full backup battery charge remaining. This value must be in the range of 0 to 100, or BATTERY_PERCENTAGE_UNKNOWN.
					*/
					if (ppbpi) 
                    {
                        nclog(L"\tLength: %d\n", ppb->Length);
						nclog(L"\tBatteryLifeTime = %s\n", ppbpi->dwBatteryLifeTime==0xFFFFFFFF ? L"unknown" : _ltow(ppbpi->dwBatteryLifeTime, szTemp, MAX_PATH));
                        nclog(L"\tBatterFullLifeTime = %s\n",
                                     ppbpi->dwBatteryFullLifeTime==0xFFFFFFFF ? L"unknown" : _ltow(ppbpi->dwBatteryFullLifeTime, szTemp, MAX_PATH));
                        nclog(L"\tBackupBatteryLifeTime = %s\n",
                                     ppbpi->dwBackupBatteryLifeTime==0xFFFFFFFF ? L"unknown" : _ltow(ppbpi->dwBackupBatteryLifeTime, szTemp, MAX_PATH));
                        nclog(L"\tBackupBatteryFullLifeTime = %s\n",
                                     ppbpi->dwBackupBatteryFullLifeTime==0xFFFFFFFF ? L"unknown" : _ltow(ppbpi->dwBackupBatteryFullLifeTime, szTemp, MAX_PATH));
                        nclog(L"\tACLineStatus = %s\n", szAClineStatus(szTemp, ppbpi->bACLineStatus));
                        nclog(L"\tBatteryFlag = %s\n", szBatteryFlag(szTemp, ppbpi->bBatteryFlag));
                        nclog(L"\tBatteryLifePercent = %d\n", ppbpi->bBatteryLifePercent);
                        nclog(L"\tBackupBatteryFlag = %s\n", szBatteryFlag(szTemp, ppbpi->bBackupBatteryFlag));
                        nclog(L"\tBackupBatteryLifePercent = %d\n", ppbpi->bBackupBatteryLifePercent);
                    }
                    break;
                }

                default:
					nclog(L"Unknown PwrMsg: 0x%08x", ppb->Message);
                    break;
            }
			dumpPowerFlags(ppb, szPPB);
			wsprintf(szOut, L"%s%s\n", szPBtype, szPPB);
			nclog(szOut);
        }
        delete[] ppb;
		delete(szPPB);
		delete(szPBtype);
    }
	SetEvent(hPwrMsgThreadHasStoppedEvent);
Error:
    if (hPowerNotifications)
        StopPowerNotifications(hPowerNotifications);

    if (hPowerMsgQ)
        CloseMsgQueue(hPowerMsgQ);
	CloseHandle(hExitPwrMsgThreadEvent); //no more needed
    return NULL;
}

int startMsgThread(){
	DWORD rc=0;
	hPwrMsgQueueThread = CreateThread(NULL, 0, &PowerMsgsThread, NULL, CREATE_SUSPENDED, &rc);
	if(hPwrMsgQueueThread!=NULL){
		SetThreadPriority(hPwrMsgQueueThread, THREAD_PRIORITY_ABOVE_NORMAL);
		nclog(L"Starting MessageThread with id=0x%x...\n", rc);
		ResumeThread(hPwrMsgQueueThread);
		return 0;
	}
	else 
		return -1;
}

int stopMsgThread(){
	int iResult=0;
	hPwrMsgThreadHasStoppedEvent = CreateEvent(NULL, FALSE, FALSE, szPwrMsgThreadHasStoppedEventName); //hopefully an exisiting event as thread is started
	if(GetLastError()==ERROR_ALREADY_EXISTS){
		nclog(L"stopMsgThread: using existing hPwrMsgThreadHasStoppedEvent handle to wait for PwrMsgThread End...\n");
		if(hExitPwrMsgThreadEvent!=NULL){
			nclog(L"stopMsgThread: using existing hExitPwrMsgThreadEvent handle to signal Exit to PwrMsgThread...\n");
			PulseEvent(hExitPwrMsgThreadEvent);

			nclog(L"stopMsgThread: Waiting for thread end...\n");
			
			DWORD dwStat = WaitForSingleObject(hPwrMsgThreadHasStoppedEvent, INFINITE);
			switch(dwStat){
				case WAIT_OBJECT_0:
					nclog(L"stopMsgThread: PwrMsgThread signaled it has ended\n");
					iResult = 1;
					break;
				case WAIT_ABANDONED:
					nclog(L"stopMsgThread: WaitForSingleObject abondoned!\n");
					iResult = -1;
					break;
				case WAIT_FAILED:
					nclog(L"stopMsgThread: WaitForSingleObject failed!\n");
					iResult = -2;
					break;
			}
		}
		else
			RETAILMSG(1, (L"stopMsgThread: No hExitPwrMsgThreadEvent handle found! PwrMsgThread running?\n"));
	}
	else
		RETAILMSG(1, (L"stopMsgThread: hPwrMsgThreadHasStoppedEvent not found! PwrMsgThread running?\n"));
	CloseHandle(hExitPwrMsgThreadEvent);
	CloseHandle(hPwrMsgQueueThread);
	CloseHandle(hPwrMsgThreadHasStoppedEvent);
	return iResult;
}

