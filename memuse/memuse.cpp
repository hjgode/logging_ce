// memuse.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "memuse.h"


#define MAX_LOADSTRING 100

// Global Variables:
int	iTimeInterval=10000;
TCHAR szLogFileName[MAX_PATH];
BOOL doQuit = FALSE;

HINSTANCE			g_hInst;			// current instance
HWND				g_hWndMenuBar;		// menu bar handle

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int SplitArgs(LPTSTR lpCmdLine);

DWORD dwTimerID = 10001;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	//process cmd line
	SplitArgs(lpCmdLine);
	DEBUGMSG(1, (L"using log file: '%s', interval=%ims\n", szLogFileName, iTimeInterval));


	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEMUSE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//global for arguments array
int          argc; 
TCHAR**       argv;
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
  j=0;
  do{ 
		wsprintf(str, L"arg %i = %s\n", j, argv[j]);
		OutputDebugString(str);
		if(wcsicmp(argv[j], L"-i")==0)	//-i 2000	?
		{
			if(j+1 < argc){
				int iVal = _wtoi(argv[++j]);
				if(iVal>0)
					iTimeInterval=iVal*1000;
			}
		}
		else if(wcsicmp(argv[j], L"-f")==0){
			if(j+1 < argc){
				TCHAR szVal[MAX_PATH];
				wsprintf(szVal, argv[++j]);
				if(wcslen(szVal)>0){
					wsprintf(szLogFileName, szVal);
				}
			}
		}
		else if(wcsicmp(argv[j], L"-q")==0){
			doQuit=TRUE;
		}
		//free(argv[j]); 
		j++;
	}while (j<argc);
  /*
	arg 0 = \Program Files\memuse\memuse.exe
	arg 1 = -i
	arg 2 = 2
	arg 3 = -f
	arg 4 = \My Documents\test.log
  */
  // free(argv); 
  return argc;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEMUSE));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

    g_hInst = hInstance; // Store instance handle in our global variable

    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_MEMUSE, szWindowClass, MAX_LOADSTRING);

    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
		if(doQuit){
			PostMessage(hWnd, WM_QUIT, 0, 0);
		}
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }

    ShowWindow(hWnd, SW_MINIMIZE);// nCmdShow);
    UpdateWindow(hWnd);


    return TRUE;
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
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    static SHACTIVATEINFO s_sai;
	
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_HELP_ABOUT:
                    DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, About);
                    break;
                case IDM_OK:
                    SendMessage (hWnd, WM_CLOSE, 0, 0);				
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
            SHMENUBARINFO mbi;

            memset(&mbi, 0, sizeof(SHMENUBARINFO));
            mbi.cbSize     = sizeof(SHMENUBARINFO);
            mbi.hwndParent = hWnd;
            mbi.nToolBarId = IDR_MENU;
            mbi.hInstRes   = g_hInst;

            if (!SHCreateMenuBar(&mbi)) 
            {
                g_hWndMenuBar = NULL;
            }
            else
            {
                g_hWndMenuBar = mbi.hwndMB;
            }

            // Initialize the shell activate info structure
            memset(&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);

			//
			if(wcslen(szLogFileName)<4)
				wsprintf(szLogFileName, L"\\memusage.log.txt");
			initFileNames(szLogFileName);
			//start timer
			old_permissions_ = SetProcPermissions( 0xFFFFFFFF );
			SetTimer(hWnd, dwTimerID, iTimeInterval, NULL);
            break;
		case WM_TIMER:
			if(wParam==dwTimerID){
				getVMuse();
			}
			break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            
            // TODO: Add any drawing code here...
            
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            CommandBar_Destroy(g_hWndMenuBar);
			old_permissions_ = SetProcPermissions( old_permissions_ );
            PostQuitMessage(0);
            break;

        case WM_ACTIVATE:
            // Notify shell of our activate message
            SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
            break;
        case WM_SETTINGCHANGE:
            SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            {
                // Create a Done button and size it.  
                SHINITDLGINFO shidi;
                shidi.dwMask = SHIDIM_FLAGS;
                shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
                shidi.hDlg = hDlg;
                SHInitDialog(&shidi);
            }
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;

        case WM_CLOSE:
            EndDialog(hDlg, message);
            return TRUE;

    }
    return (INT_PTR)FALSE;
}
