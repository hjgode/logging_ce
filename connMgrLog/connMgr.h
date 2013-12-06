//connMgr.h

#ifndef _CONNMGR_H_
	#define _CONNMGR_H_



	#include <connmgr_conntypes.h>
	#include <connmgr_status.h>
	#pragma comment (lib, "CellCore.lib")

	struct connStats{
		TCHAR* szStatus;
		int isStatus;
	};
	const TCHAR* getConnStatusText(int id);

	extern HWND hWndMain;
//	extern HWND hEdit;
	extern UINT WM_CM_STATUS_CHANGE;

	void dump_details(CONNMGR_CONNECTION_DETAILED_STATUS *p);
	void startConnMgrWatch(HWND hWnd);
	void dumpConnections(HWND hWnd);

#endif