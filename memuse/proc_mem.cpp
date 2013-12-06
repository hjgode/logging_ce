//proc_mem.cpp
#include "stdafx.h"

//BOOL KernelLibIoControl(HANDLE hLib, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned); 
//const int KMOD_CORE = 1; 
//const int IOCTL_KLIB_GETPROCMEMINFO = 8; 
//
//typedef struct _PROCVMINFO {
//        HANDLE hProc;          // process id
//        DWORD  cbRwMemUsed;    // RW RAM used
//    } PROCVMINFO, *PPROCVMINFO;

DWORD old_permissions_;
struct CosmicPowers
{
    CosmicPowers()
    {
        old_permissions_ = SetProcPermissions( 0xFFFFFFFF );
    }

    ~CosmicPowers()
    {
        SetProcPermissions( old_permissions_ );
    }
};

PROCESSNAMES ProcessNames[40];

int GetProcessNameList(PROCESSNAMES*);

#ifndef TH32CS_SNAPNOHEAPS
 const int TH32CS_SNAPNOHEAPS = 0x40000000;
#endif

DWORD getVMuse(){
	//PROCVMINFO info;
	////int idxProc; //process id
	//if(KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_GETPROCMEMINFO, NULL, idxProc, &info, sizeof(info), NULL))
	//	return info.cbRwMemUsed;
	//else
	//	return 0;

	//build the process list
	int iMax = GetProcessNameList((PROCESSNAMES*) &ProcessNames);

	DWORD total = 0;
	for( int idx = 2; idx < 33; ++idx )
	{
		PROCVMINFO vmi = { 0 };
		if( CeGetProcVMInfo( idx, sizeof( PROCVMINFO ), &vmi ) )
		{
			//DEBUGMSG(1, ( L"%d: 0x08x %d bytes\r\n", idx, vmi.hProc, vmi.cbRwMemUsed ));
			total += vmi.cbRwMemUsed;
			//GetProcessIDFromIndex() and match it to the th32ProcessID member of the PROCESSENTRY32 structure
			HANDLE dwThreaID = GetProcessIDFromIndex(idx);
			for(int i=0; i<iMax; i++){
				if(ProcessNames[i]._dwID == (DWORD)dwThreaID)
					ProcessNames[i]._memuse = vmi.cbRwMemUsed;
			}
		}
	}
	//TCHAR* szTxt = (TCHAR*)malloc(1024);
	Add2LogWithTime(L"\t");
	for(int i=0; i<iMax; i++){
		//DEBUGMSG(1, (L"Proc: 0x%x '%s' %u\r\n", ProcessNames[i]._dwID, ProcessNames[i]._name, ProcessNames[i]._memuse));
		//wsprintf(szTxt, L"Proc: 0x%x '%s' %u\r\n", ProcessNames[i]._dwID, ProcessNames[i]._name, ProcessNames[i]._memuse);
		//Add2Log(L"Proc: 0x%x '%s' %u\n", ProcessNames[i]._dwID, ProcessNames[i]._name, ProcessNames[i]._memuse);
		Add2Log(L"0x%x\t'%s'\t%u\t", ProcessNames[i]._dwID, ProcessNames[i]._name, ProcessNames[i]._memuse);
	}
	Add2Log(L"\t(%d)\n", total);
	//DEBUGMSG(1, (L"-----------------\r\nTotal: %d bytes\r\n====================\r\n", total ));
	//wsprintf(szTxt, L"-----------------\r\nTotal: %d bytes\r\n====================\r\n", total );
	//Add2Log(L"-----------------\nTotal: %d bytes\n====================\n", total );
	return 0;
}




/// Get a list associating currently running process IDs with names
int GetProcessNameList(PROCESSNAMES* name_list)
{
    //PROCESSNAMES name_list[40];
	int iCurrent = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS | 
                                                  TH32CS_SNAPNOHEAPS, 
                                                  0 );
    if( INVALID_HANDLE_VALUE != snapshot )
    {
        PROCESSENTRY32 pe = { 0 };
        pe.dwSize = sizeof( PROCESSENTRY32 );

        if( Process32First( snapshot, &pe ) )
        {
            do 
            {
                //name_list[ pe.th32ProcessID ] = pe.szExeFile;
				name_list[iCurrent]._dwID=pe.th32ProcessID;
				wsprintf(name_list[iCurrent]._name, pe.szExeFile);
				//DEBUGMSG(1, (L"ProcInfo: 0x%08x, '%s'\r\n", name_list[iCurrent]._dwID, name_list[iCurrent]._name ));
				iCurrent++;
            } while( Process32Next( snapshot, &pe ) );
        }
        CloseToolhelp32Snapshot( snapshot );
    }

    return iCurrent;
}

void getMemUsage(){
	
	int iMax = GetProcessNameList((PROCESSNAMES*) &ProcessNames);
	for(int i=0; i<iMax; i++){
		;
	}
}
