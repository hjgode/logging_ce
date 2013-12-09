// PowerMsgLog.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

HANDLE stopApp=NULL;

int _tmain(int argc, _TCHAR* argv[])
{
	nclog(L"\n### PowerMsg logging started\n");

	BOOL bIsRunning=isRunning();

	stopApp=CreateEvent(NULL, FALSE, FALSE, L"stopPowerMsgLog");
	if(stopApp==INVALID_HANDLE_VALUE){
		nclog(L"Create stopEvent failed! EXIT.\n");
		return -2;
	}
	if(argc==2){
		if(wcsicmp(argv[1],L"stop")==0){
			if(bIsRunning){
				nclog(L"PowerMsg stop received\n");
				SetEvent(stopApp);
				return 1;
			}
			else{
				//stop requested but no instance is running
				nclog(L"PowerMsg stop received, but no instance running. Start abandoned.\n");
				return -1;
			}
		}
	}
	else if (argc==1){	//no args
		if(bIsRunning){
			nclog(L"Already running. No new instance will be started.\n");
			return -2;
		}
	}

	startMsgThread();

	DWORD dwWait=WaitForSingleObject(stopApp, INFINITE);
	switch(dwWait){
		case WAIT_OBJECT_0:
			nclog(L"app will be stopped\n");
			stopMsgThread();
			return 2;
			break;
		default:
			nclog(L"unknown wait result\n");
			break;
	}
	stopMsgThread();
	return 0;
}

