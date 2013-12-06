//

#include "device_monitor.h"

BOOL StopMonitoringForNewDisks=FALSE;

void MonitorForNewDisks()
{
	HANDLE hNotify;
	HANDLE qStore ;
	DWORD flags;
	DWORD size;
	BYTE DevDetail[sizeof(DEVDETAIL) + (MAX_PATH * sizeof( TCHAR ))];
	DEVDETAIL * pDetail = (DEVDETAIL *)DevDetail;
	MSGQUEUEOPTIONS msgopts;

	msgopts.dwSize = sizeof(MSGQUEUEOPTIONS);
	msgopts.dwFlags = 0;
	msgopts.dwMaxMessages = 0;
	msgopts.cbMaxMessage = sizeof(DevDetail);
	msgopts.bReadAccess = TRUE;

	qStore = CreateMsgQueue(NULL, &msgopts);
/*
"IClass"="{f8a6ba98-087a-43ac-a9d8-b7f13c5bae31}"; This is DEVCLASS_STREAM_GUID
In case you follow my suggestion going for CE_DRIVER_SERIAL_PORT_GUID you will need:
"IClass"="{CC5195AC-BA49-48a0-BE17-DF6D1B0173DD}" ; this is CE_DRIVER_SERIAL_PORT_GUID
*/
	hNotify = RequestDeviceNotifications(&BLOCK_DRIVER_GUID, qStore, TRUE);

	do
	{
		if(WaitForSingleObject(qStore, 5000) == WAIT_OBJECT_0)
		{
			while(ReadMsgQueue(qStore, pDetail, sizeof(DevDetail), &size, 1, &flags))
			{
				if(pDetail->fAttached)
					;//do something HandleNewDisk(pDetail->szName);
			}
		}
	} while( !StopMonitoringForNewDisks );

	StopDeviceNotifications(hNotify);
}
