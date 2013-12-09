//PowerMsgQueue.h
#pragma once

#if !defined(PowerMsgQueue__INCLUDED_)
#define PowerMsgQueue__INCLUDED_
#pragma message ("##### PowerMsgQueue included #####")


//#include "stdafx.h"
#include <windows.h>
#include <pm.h>
#include <Msgqueue.h>
#include <notify.h>

extern HWND hwndMain;

#define WM_USER_RESUMECOMM	(WM_USER + 101)

#define MAX_NAMELEN 200


int startMsgThread();
int stopMsgThread();

#endif //PowerMsgQueue__INCLUDED_