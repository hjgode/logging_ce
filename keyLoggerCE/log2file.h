//log2file.h

//use #define UseLogging to enable logging
#if DEBUG
	#define UseLogging		//to enable logging to file
#endif

//global
char	logFileName[MAX_PATH];
TCHAR	logFileNameW[MAX_PATH];
//bool	FirstStart = FALSE;

void Add2Log(TCHAR);
void Add2Log(LPSTR);
void Add2Log(TCHAR *txt, BOOL bLogTime);
void Add2Log(char *txt, BOOL bLogTime);
void Add2Log (TCHAR *lpszFormat, ...);
void Add2LogWtime (TCHAR *lpszFormat, ...);

int writefile(LPTSTR);
int newfile(LPSTR);
int newfile(TCHAR);
int appendfile(LPSTR);

//=================================================================================
//implementation
//=================================================================================
//-----------------------------------------------------------------------
// Add2Log - Add string to the the file
//
void Add2Log (TCHAR *lpszFormat, ...) {
    int nBuf;//, i;
    TCHAR szBuffer[512];

    va_list args;
    va_start(args, lpszFormat);

    nBuf = vswprintf(szBuffer, lpszFormat, args);
	Add2Log(szBuffer, false);
    va_end(args);
}
//-----------------------------------------------------------------------
// Add2Log - Add string to the the file
//
void Add2LogWtime (TCHAR *lpszFormat, ...) {
    int nBuf;//, i;
    TCHAR szBuffer[512];

    va_list args;
    va_start(args, lpszFormat);

    nBuf = vswprintf(szBuffer, lpszFormat, args);
	Add2Log(szBuffer, true);
    va_end(args);
}
//-----------------------------------------------------------------------
// appendfile - set the global filename to append text
//
int appendfile(char *filename)
{
	//store the filename to use in char and tchar
	sprintf(logFileName, filename);
	mbstowcs(logFileNameW, logFileName, sizeof(logFileName)*sizeof(logFileName[0]));

	FILE	*fp;
	fp = fopen(logFileName, "a+");
	fclose(fp);
	return 0;
}

int newfile(char *filename)
{
	//store the filename to use in char and tchar
	sprintf(logFileName, filename);
	mbstowcs(logFileNameW, logFileName, sizeof(logFileName)*sizeof(logFileName[0]));

	FILE	*fp;
	fp = fopen(logFileName, "w+");
	fclose(fp);
	return 0;
}

int newfile(TCHAR *filename)
{
	//store the filename to use in char and tchar
	TCHAR logFileNameW[MAX_PATH];
	wsprintf(logFileNameW, filename);
	wcstombs(logFileName, logFileNameW, sizeof(logFileNameW)*sizeof(logFileNameW[0]));

	FILE	*fp;
	fp = fopen(logFileName, "w+");
	fclose(fp);
	return 0;
}

int writefile(TCHAR *filetext)
{
#ifdef UseLogging
	/* File Write Function, written by professor chemicalX */
	FILE	*fp;						/* Declare FILE structure */
	TCHAR  szTemp[255];
	char  szTempA[255];

	wsprintf(szTemp, L"%s", filetext);
	wcstombs(szTempA, szTemp, sizeof(szTemp)/sizeof(TCHAR));

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
	fclose(fp); /* Remember to close the file stream after calling file functions, */

	/* otherwise the file wont be created. */
#endif
	return 0;
}

void Add2Log(TCHAR *txt, BOOL bLogTime = TRUE)
{
	TCHAR str[512];
	TCHAR lpTimeStr[128];
	TCHAR lpDateStr[128];
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

	if (bLogTime == TRUE)
		wsprintf(str, L"\t%s %s:\n%s", lpDateStr, lpTimeStr , txt);
	else
		wsprintf(str, L"%s", txt);
	writefile(str);

}

void Add2Log(char *txt, BOOL bLogTime = TRUE)
{
	TCHAR  szTemp[512];
	char  szTempA[512];
	wsprintf(szTemp, L"");
	sprintf(szTempA, "");
	sprintf(szTempA, "%s", txt);
	mbstowcs(szTemp, szTempA, 512);//strlen(szTempA)*sizeof(szTempA[0]));

	Add2Log(szTemp, bLogTime);
}