// showFullScreen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifndef TH32CS_SNAPNOHEAPS
#define TH32CS_SNAPNOHEAPS  0x40000000	// optimization to not snapshot heaps
#endif

//global vars
TCHAR szWindowTitle[MAX_PATH];
TCHAR szWindowClass[MAX_PATH];

BOOL bChangeSize=FALSE;
BOOL bShowFullScreen=TRUE;
BOOL bModifySize=FALSE;
BOOL bRDMfull=FALSE;

int iNewWidth=240;
int iNewHeight=320;

int iShowHide=0;	// 0=do not change, 1=Show, 2=hide

int iEnableDisable=0;	//enable=0-> no change, =1-> enable window, =2->disable window

BOOL bListWindows=FALSE;

//###################################################

struct cmdList{
	TCHAR value[MAX_PATH];
	struct cmdList *next;
};

void append(struct cmdList **lst, TCHAR value[MAX_PATH])
{
    struct cmdList *neuesElement;
    struct cmdList *lst_iter = *lst;
    neuesElement = (struct cmdList*) malloc(sizeof(*neuesElement)); // erzeuge ein neues Element
    wcscpy(neuesElement->value, value);
    neuesElement->next = NULL; // Wichtig für das Erkennen des Listenendes
    if ( lst_iter != NULL ) { // sind Elemente vorhanden
        while (lst_iter->next != NULL ) // suche das letzte Element
            lst_iter=lst_iter->next;
        lst_iter->next=neuesElement; // Hänge das Element hinten an
    }
    else // wenn die liste leer ist, bin ich das erste Element
        *lst=neuesElement;
}

int getString(CmdLineArgs a, int idx, TCHAR* str){
	int iRet=0;
	if(a.size()<idx+1)
		return 0;
	wcscpy(a[idx+1], str);
	return wcslen(str);
}

void getOptions(struct cmdList *l){
	struct cmdList *liste;
	liste=l;
	if(l==NULL)
		return;
	int iVal;
	do{
        DEBUGMSG(1, (L"%s\r\n", liste->value));
		if(wcsicmp(liste->value, L"-class")==0){		// window class
			if(liste->next != NULL){
				liste=liste->next;
				DEBUGMSG(1, (L"\t%s\r\n", liste->value));
				wsprintf(szWindowClass, L"%s", liste->value);
			}
		}
		else if(wcsicmp(liste->value, L"-title")==0){		// message text
			if(liste->next != NULL){
				liste=liste->next;
				DEBUGMSG(1, (L"\t%s\r\n", liste->value));
				wsprintf(szWindowTitle, L"%s", liste->value);
			}
		}
		else if(wcsicmp(liste->value, L"-resize")==0){		// -resize 240 320
			bModifySize=TRUE;
			bChangeSize=TRUE;
			//read width
			if(liste->next != NULL){
				liste=liste->next;
				DEBUGMSG(1, (L"\t%s\r\n", liste->value));
				iVal = _wtoi(liste->value);
				if(iVal!=0)
					iNewWidth=iVal;
			}
			//read height
			if(liste->next != NULL){
				liste=liste->next;
				DEBUGMSG(1, (L"\t%s\r\n", liste->value));
				iVal = _wtoi(liste->value);
				if(iVal!=0)
					iNewHeight=iVal;				
			}
		}
		else if(wcsicmp(liste->value, L"-rdmfull")==0){	// RDM fullscreen?
			bRDMfull=TRUE;
		}
		else if(wcsicmp(liste->value, L"-fullscreen")==0){	// fullscreen?
			bShowFullScreen=TRUE;
			bChangeSize=TRUE;
		}
		else if(wcsicmp(liste->value, L"-maximized")==0){	// maximized?
			bShowFullScreen=FALSE;
			bChangeSize=TRUE;
		}
		else if(wcsicmp(liste->value, L"-show")==0){	// use sw_show
			iShowHide=1;
		}
		else if(wcsicmp(liste->value, L"-hide")==0){	// use sw_hide
			iShowHide=2;
		}
		else if(wcsicmp(liste->value, L"-enable")==0){	// use enableWindow
			iEnableDisable=1;
		}
		else if(wcsicmp(liste->value, L"-disable")==0){	// use sw_hide
			iEnableDisable=2;
		}
		else if(wcsicmp(liste->value, L"-list")==0){	// use sw_hide
			bListWindows=TRUE;
		}
		//else if(wcsicmp(liste->value, L"-b")==0){	// rgb b value
		//	if(liste->next != NULL){
		//		liste=liste->next;
		//		iVal=_wtoi(liste->value);
		//		if(iVal!=0)
		//			backcolorBlue=iVal;
		//	}
		//}
		//else if(wcsicmp(liste->value, L"-rt")==0){	// text rgb r value
		//	if(liste->next != NULL){
		//		liste=liste->next;
		//		iVal=_wtoi(liste->value);
		//		if(iVal!=0)
		//			fontColorRed=iVal;
		//	}
		//}
		liste=liste->next;
	}while(liste != NULL);
}

//======================================================================
struct ProcEntry{
	DWORD dwID;
	TCHAR szName[MAX_PATH];
}myProcEntry;

ProcEntry procLIST[32];

int iLISToffset=0;
//======================================================================
TCHAR* getProcessName(DWORD dwID, TCHAR* name){
	wsprintf(name, L"");
	for (int i=0; i<32;i++){
		if(procLIST[i].dwID==dwID){
			wsprintf(name, L"%s", procLIST[i].szName);
			break;
		}
	}
	return name;
}

void getProcList(){
	for (int i=0; i<32; i++)
		wsprintf(procLIST[i].szName, L"");
	PROCESSENTRY32 pe32;
	DWORD dwPID=0;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPNOHEAPS | TH32CS_SNAPPROCESS, dwPID);
	if(hProcessSnap ==INVALID_HANDLE_VALUE)
		return;
	pe32.dwSize=sizeof(PROCESSENTRY32);
	if( !Process32First( hProcessSnap, &pe32 ) ){
		CloseToolhelp32Snapshot(hProcessSnap);
		return;
	}
	do{
		procLIST[iLISToffset].dwID=pe32.th32ProcessID;
		wsprintf(procLIST[iLISToffset++].szName, L"%s", pe32.szExeFile);
	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseToolhelp32Snapshot(hProcessSnap);

	//for (int i=0; i<32; i++){
	//	DEBUGMSG(1, (L"%i\t0x%08x\t'%s'\n", i, procLIST[i].dwID, procLIST[i].szName));
	//}
}

static int iLevel=0;
#ifndef WS_MINIMIZE
	#define WS_MINIMIZE 0x20000000L 
#endif
//======================================================================
BOOL CALLBACK procEnumWindows(HWND hwnd, LPARAM lParam) //find window for PID
{
//	LONG_PTR iLevel = lParam;	//change
	iLevel++;

	TCHAR caption[MAX_PATH];
	TCHAR classname[MAX_PATH];
	TCHAR procname[MAX_PATH];
	TCHAR* szName; szName = (TCHAR*)malloc (MAX_PATH);
	GetClassName(hwnd, classname, MAX_PATH);
	//hack as it hangs for "Button"
	if(wcscmp(classname,L"Button")==0){
		wsprintf(caption, L"");
	}
	else{
		if ( GetWindowTextLength(hwnd)>0 )
			GetWindowText(hwnd, caption, MAX_PATH);
		else
			wsprintf(caption, L"");
	}
	DWORD dwProcID=0;
	GetWindowThreadProcessId(hwnd, &dwProcID);
	
	//dimensions and position
	RECT rect;
	GetWindowRect(hwnd, &rect);
	TCHAR szRect[64];
	wsprintf(szRect, L"%i;%i/%i;%i (%ix%i)", rect.left, rect.top, rect.right, rect.bottom, rect.right-rect.left, rect.bottom-rect.top);

	////visible?, MINIMIZED not supported
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	TCHAR szStyle[64];
	wsprintf(szStyle, L"[%s]", dwStyle&WS_VISIBLE ? L"visible":L"hidden" );

	wsprintf(procname, L"%s", getProcessName(dwProcID, szName));
	DEBUGMSG(1, (L"%i\t0x%08x\t0x%08x\t('%s')\t'%s'\t'%s'\t%s\t%s\n", iLevel, hwnd, dwProcID, procname, classname, caption, szRect, szStyle));
	nclog(L"%i\t0x%08x\t0x%08x\t('%s')\t'%s'\t'%s'\t%s\t%s\n", iLevel, hwnd, dwProcID, procname, classname, caption, szRect, szStyle);

	free(szName);
	//return FALSE; // to stop iteration before end of window list
	return true;
}
//======================================================================
// Look for windows starting with "Installing... or "Setup ..." and let them come to front
int ListWindows()
{
	getProcList();
	DEBUGMSG(1, (L"Window List\nnr\thwnd\tprocID\tprocName\tclass\ttitle\tpos/size\tstate\n"));

	nclog(L"Window List\nthis\tnr\thwnd\tprocID\tprocName\tclass\ttitle\tpos/size\tstate\n");

	int iLevel=0;
	EnumWindows(procEnumWindows, iLevel);

	return 0;
}

void printHelp(){
	nclog(L"========================================================================");
	nclog(L"    CONSOLE APPLICATION : showFullScreen Project Overview");
	nclog(L"========================================================================");
	nclog(L"");
	nclog(L"Tool to alter foreign windows");
	nclog(L"");
	nclog(L"/////////////////////////////////////////////////////////////////////////////");
	nclog(L"Arguments:				default			meaning");
	nclog(L"-class					\"\"				class name of window to change, optional");
	nclog(L"-title					\"\"				title of window to change, optional");
	nclog(L"			at least -class or -title must be used to change a window");
	nclog(L"										");
	nclog(L"-fullscreen				false			make window fullscreen");
	nclog(L"-maximized				false			make window normal size");
	nclog(L"");
	nclog(L"-resize					240 320			change window size to x y, needs two int args");
	nclog(L"");
	nclog(L"-rdmfull				false			change RDM to full screen");
	nclog(L"");
	nclog(L"-show					no change		make window visible");
	nclog(L"-hide					no change		make window invisible");
	nclog(L"");
	nclog(L"-enable					no change		enable window");
	nclog(L"-disable				no change		disable window");
	nclog(L"");
	nclog(L"-list					-				list windows into file");
	nclog(L"");
	nclog(L"/////////////////////////////////////////////////////////////////////////////");
	nclog(L"examples:");
	nclog(L"");
	nclog(L"Enable IE6 soft menu button:	-class \"IE6on6SoftKeyBar\" -enable");
	nclog(L"Disable IE6 soft menu button:	-class \"IE6on6SoftKeyBar\" -disable");
	nclog(L"");
	nclog(L"Enable taskbar:		-class \"HHTASKBAR\" -enable");
	nclog(L"Disable taskbar:	-class \"HHTASKBAR\" -disable");
	nclog(L"");
	nclog(L"make win fullscreen: -class \"WFIcaClient\" -fullscreen");
	nclog(L"");
	nclog(L"");
	nclog(L"");
	nclog(L"/////////////////////////////////////////////////////////////////////////////");
}

int _tmain(int argc, _TCHAR* argv[])
{
	wsprintf(szWindowClass, L"");
	wsprintf(szWindowTitle, L"");
	// -class "IE6on6SoftKeyBar" -enable -list
	if(argc==1){	// no args
		nclog(L"showFullScreen utility v 1.0 * %s\n", logDateTime());
		printHelp();
		MessageBox(GetDesktopWindow(), L"showFullScreen needs some args. Please see 'showFullScreen.exe.log.txt'", L"showFullScreen", MB_SETFOREGROUND | MB_TOPMOST | MB_OK);
		return -3;
	}

	DEBUGMSG(1, (L"CmdLine parsing #1: \r\n"));
	//command parsing
	struct cmdList *Liste;
	Liste=NULL;
	CmdLineArgs args;
	for (UINT i = 0; i < args.size(); i++){
		DEBUGMSG(1, (L"%20i: '%s'\r\n", i, args[i]));
		append(&Liste, args[i]);
	}
	getOptions(Liste);
	args.~CmdLineArgs();
	//end of parsing

	if(bListWindows){
		nclog(L"showFullScreen utility v 1.1 * %s\n", logDateTime());
		
		ListWindows();
	}

	//look for window
	HWND hWnd=NULL;

	if(bRDMfull){
		nclog(L"modifying RDM window\n", logDateTime());
		//resize RDM window
		hWnd=FindWindow(L"TSSHELLWND", NULL);
		long style = GetWindowLong(hWnd, GWL_STYLE);
		style|=WS_DLGFRAME;
		style&=~WS_BORDER;
		SetWindowLong(hWnd, GWL_STYLE, style);
		int iX = GetSystemMetrics(SM_CXSCREEN);	// 480
		int iY = GetSystemMetrics(SM_CYSCREEN);	// 640
		DEBUGMSG(1, (L"fullscreen width/height %i/%i\n", iX, iY));
		SetWindowPos(hWnd, GetDesktopWindow(), 0, 0, iX, iY, SWP_SHOWWINDOW);
//		ShowWindow(hWnd, SW_MAXIMIZE);
		UpdateWindow(hWnd);
		//modify client win
		hWnd=getChildWindowByTitle(L"TSSHELLWND", L"WinShell");
		SetWindowPos(hWnd, GetDesktopWindow(), 0, 0, iX, iY, SWP_SHOWWINDOW);
		UpdateWindow(hWnd);
		//move menu bar
		hWnd=getChildWindowByTitle(L"TSSHELLWND", L"WinShell");
		if(hWnd!=NULL){
			HWND hWndMenu = getWindowMenu(hWnd);
			if(hWndMenu!=NULL){
				RECT rectMenu;
				GetWindowRect(hWndMenu, &rectMenu);
				DWORD dwType=REG_DWORD;
				DWORD dwBuf;
				DWORD dwSize=sizeof(DWORD);
				HKEY hReg;
				if(RegOpenKeyEx(HKEY_CURRENT_USER, L"ControlPanel\\Sip", 0, KEY_READ, &hReg)==ERROR_SUCCESS){
					if(RegQueryValueEx(hReg, L"MenuBarHeight", NULL, &dwType, (LPBYTE)&dwBuf, &dwSize)==ERROR_SUCCESS){
						SetWindowPos(hWndMenu, NULL, rectMenu.top+dwBuf, rectMenu.left, rectMenu.bottom, rectMenu.right, SWP_NOZORDER|SWP_NOSIZE);
					}
					RegCloseKey(hReg);
				}else
					ShowWindow(hWndMenu, SW_HIDE);
			}
		}
		//modify SIP button
		hWnd=FindWindow(L"MS_SIPBUTTON",NULL);
		SetWindowPos(hWnd, GetDesktopWindow(), iX, iY, 0,0,SWP_SHOWWINDOW|SWP_NOSIZE);
		return 0;
	}

	if(wcslen(szWindowClass)>0 && wcslen(szWindowTitle)>0){
		//window title and class
		hWnd = FindWindow(szWindowClass, szWindowTitle);
		nclog(L"FindWindow using class and title %s/%s\n", szWindowClass, szWindowTitle);
	}
	else if(wcslen(szWindowClass)>0 && wcslen(szWindowTitle)==0){
		//only window class, ie "WFIcaClient"
		hWnd = FindWindow(szWindowClass, NULL);
		nclog(L"FindWindow using class %s\n", szWindowClass);
	}
	else if(wcslen(szWindowClass)==0 && wcslen(szWindowTitle)>0){
		//only window title
		hWnd = FindWindow(NULL, szWindowTitle);
		nclog(L"FindWindow using title %s\n", szWindowTitle);
	}


	HWND hWndTaskbar = FindWindow(L"HHTASKBAR", NULL);
	//sanity restore of HHTASKBAR?
	//ShowWindow(hWndTaskbar, SW_SHOWNORMAL);

	if(hWnd==NULL){
		nclog(L"No window found!\n");
		return -1;	//no window found
	}

	if(bChangeSize){	//change size?
		nclog(L"Changing Size...\n");
		RECT rectDesktop;
		RECT rectTarget;
		HRESULT hRes = GetWindowRect(GetDesktopWindow(), &rectDesktop);	//desktopwindow is only the taskbar window!
		// SM_CXFULLSCREEN , SM_CXMAXIMIZED , SM_CXSCREEN 
		int iCX;
		int iCY;
		BOOL bRes;
		if(bShowFullScreen && !bModifySize){
			nclog(L"..make FullScreen...\n");
			iCX = GetSystemMetrics(SM_CXSCREEN);	// 480
			iCY = GetSystemMetrics(SM_CYSCREEN);	// 640
			DEBUGMSG(1, (L"fullscreen width/height %i/%i\n", iCX, iCY));
			bRes = SetWindowPos(hWnd, GetDesktopWindow(), 0, 0, iCX, iCY, SWP_SHOWWINDOW);
			ShowWindow(hWndTaskbar, SW_HIDE);
			UpdateWindow(hWnd);
		}
		else if(bModifySize){
			nclog(L"..modify size to w/h %i/%i...\n", iNewWidth, iNewHeight);
			GetWindowRect(hWnd, &rectTarget);
			bRes = SetWindowPos(hWnd, GetDesktopWindow(), rectTarget.left, rectTarget.top, iNewWidth, iNewHeight, SWP_SHOWWINDOW);
			if(!bRes)
				nclog(L"... Changing Size failed: %i\n", GetLastError());
			ShowWindow(hWnd, SW_SHOWNORMAL);
		}
		else{
			RECT rcMaxScreen;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rcMaxScreen, SPIF_SENDCHANGE);	// {top=36 bottom=640 left=0 right=480}
			bRes = SetWindowPos(hWnd, GetDesktopWindow(), rcMaxScreen.left, rcMaxScreen.top, rcMaxScreen.right, rcMaxScreen.bottom-rcMaxScreen.top, SWP_SHOWWINDOW);
			ShowWindow(hWndTaskbar, SW_SHOWNORMAL);
		}
	}
	if(iShowHide!=0){	//change show/hide
		switch(iShowHide){
			case 1:
				ShowWindow(hWnd, SW_SHOWNORMAL);
				break;
			case 2:
				ShowWindow(hWnd, SW_HIDE);
				break;
		}
	}
	if(iEnableDisable!=0){	//enable/disbale window
		switch(iEnableDisable){
			case 1:
				EnableWindow(hWnd, TRUE);
				break;
			case 2:
				EnableWindow(hWnd, FALSE);
				break;
		}
	}

	return 0;
}

