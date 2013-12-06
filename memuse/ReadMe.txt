========================================================================
    WIN32 APPLICATION : memuse Project Overview
========================================================================

## memuse writes a log with the memory usage of running processes

### sample:

08.05.2013 14:56:15		0x1e9ff002	'NK.EXE'	0	0x5e9eb29a	'filesys.exe'	0	0xbe9b342a	'device.exe'	10473472	0x173a3642	'PPCC.exe'	4550656	0x39e5e632	'repllog.exe'	651264	0x5cbb76e6	'gwes.exe'	7634944	0x9aa18c16	'shell32.exe'	2031616	0xba8a5426	'services.exe'	1069056	0xb9b07f62	'AAM.exe'	135168	0x59866c9e	'connmgr.exe'	372736	0x1958be66	'cprog.exe'	1122304	0x97eba4e2	'ConManClient2.exe'	221184	0x398958d2	'poutlook.exe'	86016	0x16347482	'Fahrverkauf.exe'	7073792	0x18074ea2	'edm2.exe'	479232	0x590e6eca	'RBLogger.exe'	69632	0xf4ca1c62	'memuse.exe'	106496	0x790b4566	'RBTimeStamp.exe'	20480	0x590e6f12	'itcWanInfoCollector.exe'	147456	0x791459fe	'rapiclnt'	339968	0x775352f6	'mstli.exe'	110592	0xb8c53b42	'tmail.exe'	303104	0x98c53f9e	'RFIDDce.Exe'	2793472	0x784d6efa	'DataServer.exe'	1740800	0xd815db2a	'IQueue.exe'	671744	0x57bd76ce	'SSClient.exe'	2195456		(44400640)
08.05.2013 14:56:17		0x1e9ff002	'NK.EXE'	0	0x5e9eb29a	'filesys.exe'	0	0xbe9b342a	'device.exe'	10473472	0x173a3642	'PPCC.exe'	4550656	0x39e5e632	'repllog.exe'	651264	0x5cbb76e6	'gwes.exe'	7634944	0x9aa18c16	'shell32.exe'	2031616	0xba8a5426	'services.exe'	1069056	0xb9b07f62	'AAM.exe'	135168	0x59866c9e	'connmgr.exe'	372736	0x1958be66	'cprog.exe'	1122304	0x97eba4e2	'ConManClient2.exe'	221184	0x398958d2	'poutlook.exe'	86016	0x16347482	'Fahrverkauf.exe'	7073792	0x18074ea2	'edm2.exe'	479232	0x590e6eca	'RBLogger.exe'	69632	0xf4ca1c62	'memuse.exe'	106496	0x790b4566	'RBTimeStamp.exe'	20480	0x590e6f12	'itcWanInfoCollector.exe'	147456	0x791459fe	'rapiclnt'	339968	0x775352f6	'mstli.exe'	110592	0xb8c53b42	'tmail.exe'	303104	0x98c53f9e	'RFIDDce.Exe'	2793472	0x784d6efa	'DataServer.exe'	1740800	0xd815db2a	'IQueue.exe'	671744	0x57bd76ce	'SSClient.exe'	2195456		(44400640)
08.05.2013 14:56:19		0x1e9ff002	'NK.EXE'	0	0x5e9eb29a	'filesys.exe'	0	0xbe9b342a	'device.exe'	10481664	0x173a3642	'PPCC.exe'	4550656	0x39e5e632	'repllog.exe'	651264	0x5cbb76e6	'gwes.exe'	7634944	0x9aa18c16	'shell32.exe'	2031616	0xba8a5426	'services.exe'	1069056	0xb9b07f62	'AAM.exe'	135168	0x59866c9e	'connmgr.exe'	372736	0x1958be66	'cprog.exe'	1122304	0x97eba4e2	'ConManClient2.exe'	221184	0x398958d2	'poutlook.exe'	86016	0x16347482	'Fahrverkauf.exe'	7073792	0x18074ea2	'edm2.exe'	491520	0x590e6eca	'RBLogger.exe'	69632	0xf4ca1c62	'memuse.exe'	106496	0x790b4566	'RBTimeStamp.exe'	20480	0x590e6f12	'itcWanInfoCollector.exe'	147456	0x17ee4fce	'memuse.exe'	77824	0x791459fe	'rapiclnt'	339968	0x775352f6	'mstli.exe'	110592	0xb8c53b42	'tmail.exe'	303104	0x98c53f9e	'RFIDDce.Exe'	2793472	0x784d6efa	'DataServer.exe'	1740800	0xd815db2a	'IQueue.exe'	671744	0x57bd76ce	'SSClient.exe'	2195456		(44498944)

You will get a column with date/time and then the Process ID, the Process Name, the memory usage for every running process. At the end of each line is the total memory listed within ()

## command line arguments

option	value		meaning						default
-i		interval	interval in seconds			10 seconds
-f		"name.log"	path and name of log file	"\memusage.log.txt"
-q					quit running instance

### examples:

Start memuse with an interval of 2 seconds and using the log file \My Documents\test.log:
	memuse -i 2 -f "\My Documents\test.log"

Stop the running memuse instance:
	memuse -q


/////////////////////////////////////////////////////////////////////////////