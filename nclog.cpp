// --- nclog.cpp ---
#include "stdafx.h"

////#include "winsock.h"
//#include "winsock2.h"
//#pragma comment (lib, "ws2.lib")
//
//#include <stdarg.h>
//#include <stdio.h>
//
//static SOCKET wsa_socket=INVALID_SOCKET;
//#pragma comment(lib , "winsock")

static unsigned short theLogPort;

// bind the log socket to a specific port.
static bool wsa_bind(unsigned short port)
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int r=bind(wsa_socket,(sockaddr*)&addr,sizeof(addr));
    if (r==0) 
		theLogPort=port;
    return (r==0);
}

// initialize everything, if the socket isn't open.
static bool wsa_init()
{
        if (wsa_socket != INVALID_SOCKET) return true;
        int r;
        WSADATA wd;
        BOOL bc=true;

        if (0 != WSAStartup(0x101, &wd)) 
			goto error;
        wsa_socket=socket(PF_INET, SOCK_DGRAM, 0);
        if (wsa_socket == INVALID_SOCKET) 
			goto error;
        r=setsockopt(wsa_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bc, sizeof(bc));
        if (r!=0) 
			goto error;
        if (wsa_bind(9998)) 
			return true; // bind to default port. 
	error:
        if (wsa_socket != INVALID_SOCKET) 
			closesocket(wsa_socket);
#ifdef DEBUG 
			OutputDebugString(TEXT("nclog: TCP/IP Problem"));
#endif
        return false;

}

// can be called externally to select a different port for operations
bool set_nclog_port(unsigned short x) { return wsa_bind(x); }

static void wsa_send(const char *x)
{
        SOCKADDR_IN sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(theLogPort);
        sa.sin_addr.s_addr = htonl(INADDR_BROADCAST);

        if (SOCKET_ERROR == sendto(wsa_socket,x,strlen(x), 0, (sockaddr*) &sa, sizeof(sa)))
        {
#ifdef DEBUG
                //if (debug_mode) 
					OutputDebugString(TEXT("nclog: Send Error"));
#endif
        }

}

// format input, convert to 8-bit and send.
void nclog (const wchar_t *fmt, ...)
{
        va_list vl;
        va_start(vl,fmt);
        wchar_t buf[1024]; // to bad CE hasn't got wvnsprintf
        wvsprintf(buf,fmt,vl);
        wsa_init();
        char bufOut[512];
        WideCharToMultiByte(CP_ACP,0,buf,-1,bufOut,400, NULL, NULL);
        wsa_send(bufOut);
}

// finalize the socket on program termination.
struct _nclog_module
{
        ~_nclog_module()
        {
                if (wsa_socket!=INVALID_SOCKET)
                {
                        nclog(L"nclog goes down\n");
                        shutdown(wsa_socket,2);
                        closesocket(wsa_socket);
                }
        }

};

static _nclog_module module; 
