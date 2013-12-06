// --- filelog.cpp ---
#pragma once

#include "stdafx.h"
#include "filelog.h"

#include <stdarg.h>
#include <stdio.h>

//file stuff
//global
static char		logFileName[MAX_PATH];
static TCHAR	logFileNameW[MAX_PATH];
static BOOL		bFirstFileCall = true;

//========================== start of file stuff =============================
static int initFileNames()
{
	 // Get name of executable
	TCHAR lpFileName1[MAX_PATH+1];
	GetModuleFileName(NULL, lpFileName1, MAX_PATH); //lpFileName will contain the exe name of this running app!

	//add txt extension
	TCHAR txtFileName[MAX_PATH+1];
	wsprintf(txtFileName, L"%s.log.txt", lpFileName1);
	//store the filename to use in char and tchar
	TCHAR logFileNameW[MAX_PATH];

	//#### we maintain two log files, an actual and a bak one
	//get file size
	HANDLE hFile = CreateFile(txtFileName, 
       GENERIC_READ,          // open for writing
       FILE_SHARE_READ,       // share
       NULL,                   // default security
       OPEN_EXISTING,          // open existing
       FILE_ATTRIBUTE_NORMAL,  // normal file
       NULL);                  // no attr. template
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	CloseHandle(hFile);

	if(dwFileSize != 0xFFFFFFFF){ //no error
		if(dwFileSize>0x100000){ //more than 1MB?
			//make a backup
			//delete previous bak
			TCHAR txtFileNameBAK[MAX_PATH];
			wsprintf(txtFileNameBAK, L"%s.bak", txtFileName);
			DeleteFile(txtFileNameBAK);
			//rename old file to .BAK
			MoveFile(txtFileName, txtFileNameBAK);
		}
	}

	//copy filename to global char and tchar var
	wsprintf(logFileNameW, txtFileName);
	wcstombs(logFileName, logFileNameW, sizeof(logFileNameW)*sizeof(logFileNameW[0]));

	FILE	*fp;
	fp = fopen(logFileName, "a+");
	fclose(fp);
	bFirstFileCall=false;
	return 0;
}

TCHAR* logDateTime(){
	static TCHAR str[64];
	TCHAR lpTimeStr[32];
	TCHAR lpDateStr[32];
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

	wsprintf(str, L"Date and Time: %s %s", lpDateStr, lpTimeStr);
	return str;
}

static int writefile(TCHAR *filetext){
	/* File Write Function, written by professor chemicalX */
	FILE	*fp;						/* Declare FILE structure */
	TCHAR  szTemp[255];
	char  szTempA[255];

	wsprintf(szTemp, L"%s", filetext);
	wcstombs(szTempA, szTemp, sizeof(szTemp)/sizeof(TCHAR));

	if (bFirstFileCall){
		// Get name of executable
		initFileNames();
	}

	fp = fopen(logFileName, "a+");

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* First of we open the file supplied by the filename paremeter */

	/*
	 * in the "a+" mode for appending, so if it doesnt exist its created. £
	 * fp = fopen(filename,"w"); // Open using the "w" mode for writing.
	 */
	long	fsize = strlen(szTempA);	/* Declare the long fsize with the length of the filetext */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* paremeter witch stores the text or data to right to the file. */
	fwrite(szTempA, 1, fsize, fp);		/* Write File */
	fflush(fp);
	fclose(fp); /* Remember to close the file stream after calling file functions, */

	/* otherwise the file wont be created. */
	return 0;
}
//========================== end of file stuff =============================

// format input, convert to 8-bit and send.
void filelog (const wchar_t *fmt, ...)
{
        va_list vl;
        va_start(vl,fmt);
        wchar_t bufW[1024]; // to bad CE hasn't got wvnsprintf
        wvsprintf(bufW,fmt,vl);
        char bufOutA[512];

		////add instance number
		//HMODULE hMod = GetModuleHandle(NULL);
		//WCHAR bufTmpW[512];
		//wsprintf(bufTmpW, L"0x%08x: %s", hMod, bufW);
		//wsprintf(bufW, L"%s", bufTmpW);

		//convert to char
        WideCharToMultiByte(CP_ACP,0,bufW,-1,bufOutA,400, NULL, NULL);

		writefile(bufW);
#ifdef DEBUG
		DEBUGMSG(1, (bufW));
#else
		RETAILMSG(1, (bufW));
#endif
}

// finalize the socket on program termination.
struct _nclog_module
{
        ~_nclog_module()
        {
                //if (wsa_socket!=INVALID_SOCKET)
                //{
                //        filelog(L"filelog goes down\n");
                //        shutdown(wsa_socket,2);
                //        closesocket(wsa_socket);
                //}
                filelog(L"filelog goes down\n");
        }

};

static _nclog_module module; 
