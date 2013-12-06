//log2file.h

#include <windows.h>

#pragma once

#ifndef _LOG2_FILE_
#define _LOG2_FILE_

//use #define UseLogging to enable logging
#define UseLogging	1		//to enable logging to file

//global
static BOOL	bFirstFileCall=true;

extern char logFileName[MAX_PATH];
extern TCHAR logFileNameW[MAX_PATH];
//bool	FirstStart = FALSE;

int initFileNames(TCHAR* _logFileName);

void Add2Log(TCHAR);
void Add2Log(LPSTR);

//void Add2Log(TCHAR *txt, BOOL bLogTime);
void Add2LogWithTime(TCHAR *txt);

void Add2Log(char *txt, BOOL bLogTime);
void Add2Log (TCHAR *lpszFormat, ...);

void Add2LogWtime (TCHAR *lpszFormat, ...);

int writefile(LPTSTR);
int newfile(LPSTR);
int newfile(TCHAR);
int appendfile(LPSTR);



#endif //_LOG2_FILE_