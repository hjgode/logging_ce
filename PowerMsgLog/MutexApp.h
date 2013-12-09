//mutexApp.h

	/*
	Avoid running multiple instances of same app

	return TRUE if an app is already runnung
	return FALSE if app is not yet running

	*/

#ifndef MUTEXAPP_H
#define MUTEXAPP_H

#include "stdafx.h"
//#include "nclog.h"

extern HANDLE hMutex;
BOOL isRunning();
int waitForExit();					//waits infinite
int waitForExit(DWORD dwTimeout);	//waits dwTimeout milli seconds

// ALWAYS use an App unique mutex name!
#define MY_MUTEX L"PowerMsgLogMutex"

#endif //MUTEXAPP_H