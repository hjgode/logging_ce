//nclog.h

#ifndef NCLOG_INCLUDED
#define NCLOG_INCLUDED
extern void nclog (const wchar_t *fmt, ...);
//extern bool set_nclog_port(unsigned short x) { return wsa_bind(x); }
extern TCHAR* logDateTime();

extern LPCRITICAL_SECTION pCriticalAction;


/*
usage:
	nclog(L"The Window Handle is: %x\n",hwnd); 

On your desktop, you need netcat to capture the output from your
program, i.e. you open a console and type:

netcat -lup 9998

This will instruct netcat to show you everything that comes to your UDP
socket 9998 (which is the default for netcat). Since nclog opens the
socket in broadcast mode, you don't have to give an target adress. (Be
sure to select a port which does not interfer with the network you are
living in) 

*/

#endif //NCLOG_INCLUDED