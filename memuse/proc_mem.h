//proc_mem.h

#include "stdafx.h"

struct PROCESSNAMES{
	DWORD _dwID;
	TCHAR _name[MAX_PATH];
	DWORD _memuse;
};

extern PROCESSNAMES ProcessNames[40];
extern DWORD old_permissions_;

DWORD getVMuse();