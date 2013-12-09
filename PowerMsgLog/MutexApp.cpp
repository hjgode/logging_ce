//MutexApp.cpp

#include "stdafx.h"
//#include "MutexApp.h"

HANDLE hMutex;

BOOL isRunning(){
	hMutex=CreateMutex(NULL, TRUE, MY_MUTEX);
	if(hMutex==NULL){
		//this should never happen
		nclog(L"Error in CreateMutex! GetLastError()=%i\n", GetLastError());
		nclog(L"-------- END -------\n");
		return FALSE;
	}
	DWORD dwLast = GetLastError();
	if(dwLast== ERROR_ALREADY_EXISTS){//mutex already exists, wait for mutex release
		nclog(L"\tAttached to existing mutex\n");
		return TRUE;
	}
	else{
		nclog(L"\tCreated new mutex\n");
		return FALSE;
	}
}

int waitForExit(){
	return waitForExit(INFINITE);
}

int waitForExit(DWORD dwTimeout){
	hMutex=CreateMutex(NULL, TRUE, MY_MUTEX);
	if(hMutex==NULL){
		//this should never happen
		nclog(L"Error in CreateMutex! GetLastError()=%i\n", GetLastError());
		nclog(L"-------- END -------\n");
		return -99;
	}
	DWORD dwLast = GetLastError();
	if(dwLast== ERROR_ALREADY_EXISTS){//mutex already exists, wait for mutex release
		nclog(L"\tAttached to existing mutex\n");
		nclog(L"................ Waiting for mutex release......\n");
		DWORD dwWait = WaitForSingleObject( hMutex, dwTimeout );// 30000);//
		switch(dwWait){
			case WAIT_OBJECT_0:
				//normal signaled end
				nclog(L"++++++++++++++++ Mutex wait released. +++++++++++++++\n");
				return 1;	//wait OK and second instance did exit
				break;
			case WAIT_TIMEOUT:
				nclog(L"++++++++++++++++ Mutex wait timed out. +++++++++++++++\n");
				goto exit_timedout;
				break;
			default:
				nclog(L"++++++++++++++++ Mutex wait unknown return code: %i +++++++++++++++\n", dwWait);
				goto exit_error;
				break;
		}
	}
	else{
		nclog(L"\tCreated new mutex\n");
	}
	nclog(L"\twait for mutex ended\n");
	return 0;
exit_timedout:
	//mutex timed out
	nclog(L"\twait for mutex timed out\n");
	return 0;
exit_error:
	nclog(L"\twait for mutex error\n");
	return -1;
}