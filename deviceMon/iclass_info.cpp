//iclass_info.cpp

#include "stdafx.h"
#include "iclass_info.h"

#define PMCLASS_GENERIC_DEVICE          TEXT("{A32942B7-920C-486b-B0E6-92A702A99B35}")
#define PMCLASS_NDIS_MINIPORT           TEXT("{98C5250D-C29A-4985-AE5F-AFE5367E5006}")
#define PMCLASS_BLOCK_DEVICE            TEXT("{8DD679CE-8AB4-43c8-A14A-EA4963FAA715}")
#define PMCLASS_DISPLAY                 TEXT("{EB91C7C9-8BF6-4a2d-9AB8-69724EED97D1}")

//Intermec TUSB GUIDs

#define DEVCLASS_TUSSERIAL_GUID			TEXT("{82ed1744-96d6-412d-ab63-deb107aad6d3}")
#define DEVCLASS_TUSMSR_GUID			TEXT("{8983e8e1-f8a4-46ee-8881-b2722bc65ae1}")
#define DEVCLASS_TUSMODEM_GUID			TEXT("{89178031-19c3-4c81-955d-4dd2ab5c1049}")
#define DEVCLASS_TUSRFID_GUID			TEXT("{803bc126-26ca-4572-a9fd-4a89eef2b800}")
#define DEVCLASS_TUSSCR_GUID			TEXT("{88163310-4D1D-42bd-9EBC-4F50A26A7C63}")
#define DEVCLASS_TUSPRINTER_GUID		TEXT("{825564FC-2662-41fb-B275-BCFE25805EB9}")

/*
        Unknown = -1,
        [DeviceClassAtrribute("{00000000-0000-0000-0000-000000000000}")]
        Any = 0,
        [DeviceClassAtrribute("{A4E7EDDA-E575-4252-9D6B-4195D48BB865}")]
        BlockDriver = 1,
        [DeviceClassAtrribute("{C1115848-46FD-4976-BDE9-D79448457004}")]
        FileStore = 2,
        [DeviceClassAtrribute("{169E1941-04CE-4690-97AC-776187EB67CC}")]
        FATFileSystem = 3,
        [DeviceClassAtrribute("{72D75746-D54A-4487-B7A1-940C9A3F259A}")]
        CDFileSystem = 4,
        [DeviceClassAtrribute("{462FEDA9-D478-4b00-86BB-51A8E3D10890}")]
        UDFileSystem = 5,
        [DeviceClassAtrribute("{BA6B1343-7980-4d0c-9290-762D527B33AB}")]
        CDDAFileSystem = 6,
        [DeviceClassAtrribute("{f8a6ba98-087a-43ac-a9d8-b7f13c5bae31}")]
        StreamDevice = 7,
        [DeviceClassAtrribute("{CBE6DDF2-F5D4-4e16-9F61-4CCC0B6695F3}")]
        Keyboard = 8,
        [DeviceClassAtrribute("{A32942B7-920C-486b-B0E6-92A702A99B35}")]
        PMGeneric = 9,
        [DeviceClassAtrribute("{98C5250D-C29A-4985-AE5F-AFE5367E5006}")]
        PMMiniport = 10,
        [DeviceClassAtrribute("{8DD679CE-8AB4-43c8-A14A-EA4963FAA715}")]
        PMBlock = 11,
        [DeviceClassAtrribute("{EB91C7C9-8BF6-4a2d-9AB8-69724EED97D1}")]
        PMDisplay = 12,
        [DeviceClassAtrribute("{8C77EDE8-47B9-45ae-8BC9-86E7B8D00EDD}")]
        FileSystem = 13,
        [DeviceClassAtrribute("{6F40791D-300E-44E4-BC38-E0E63CA8375C}")]
        ProtectedBus = 14,
        [DeviceClassAtrribute("{6BEAB08A-8914-42fd-B33F-61968B9AAB32}")]
        PCMCIACard = 15,
        [DeviceClassAtrribute("{CC5195AC-BA49-48a0-BE17-DF6D1B0173DD}")]
        Serial = 16
*/
struct iClassInfo {
	const TCHAR* szInfo;
	const TCHAR* szGUID;
};

iClassInfo _iClassInfos[] = {
	{L"Any", L"{00000000-0000-0000-0000-000000000000}"},
	{L"BlockDriver", L"{A4E7EDDA-E575-4252-9D6B-4195D48BB865}"},
	{L"FileStore", L"{C1115848-46FD-4976-BDE9-D79448457004}"},
	{L"FATFileSystem", L"{169E1941-04CE-4690-97AC-776187EB67CC}"},
	{L"CDFileSystem", L"{72D75746-D54A-4487-B7A1-940C9A3F259A}"},
	{L"UDFileSystem", L"{462FEDA9-D478-4b00-86BB-51A8E3D10890}"},
	{L"CDDAFileSystem", L"{BA6B1343-7980-4d0c-9290-762D527B33AB}"},
	{L"StreamDevice", L"{f8a6ba98-087a-43ac-a9d8-b7f13c5bae31}"},
	{L"Keyboard", L"{CBE6DDF2-F5D4-4e16-9F61-4CCC0B6695F3}"},
	{L"PMGeneric", L"{A32942B7-920C-486b-B0E6-92A702A99B35}"},
	{L"PMMiniport", L"{98C5250D-C29A-4985-AE5F-AFE5367E5006}"},
	{L"PMBlock", L"{8DD679CE-8AB4-43c8-A14A-EA4963FAA715}"},
	{L"PMDisplay", L"{EB91C7C9-8BF6-4a2d-9AB8-69724EED97D1}"},
	{L"FileSystem", L"{8C77EDE8-47B9-45ae-8BC9-86E7B8D00EDD}"},
	{L"ProtectedBus", L"{6F40791D-300E-44E4-BC38-E0E63CA8375C}"},
	{L"PCMCIACard", L"{6BEAB08A-8914-42fd-B33F-61968B9AAB32}"},
	{L"Serial", L"{CC5195AC-BA49-48a0-BE17-DF6D1B0173DD}"},
	//{L"Serial", L"{4d36e978-e325-11ce-bfc1-08002be10318}"},
	{L"PMCLASS_GENERIC_DEVICE", L"{A32942B7-920C-486b-B0E6-92A702A99B35}"},
	{L"PMCLASS_NDIS_MINIPORT", L"{98C5250D-C29A-4985-AE5F-AFE5367E5006}"},
	{L"PMCLASS_BLOCK_DEVICE", L"{8DD679CE-8AB4-43c8-A14A-EA4963FAA715}"},
	{L"PMCLASS_DISPLAY", L"{EB91C7C9-8BF6-4a2d-9AB8-69724EED97D1}"},
	{L"DEVCLASS_TUSSERIAL_GUID", L"{82ed1744-96d6-412d-ab63-deb107aad6d3}"},
	{L"DEVCLASS_TUSMSR_GUID", L"{8983e8e1-f8a4-46ee-8881-b2722bc65ae1}"},
	{L"DEVCLASS_TUSMODEM_GUID", L"{89178031-19c3-4c81-955d-4dd2ab5c1049}"},
	{L"DEVCLASS_TUSRFID_GUID", L"{803bc126-26ca-4572-a9fd-4a89eef2b800}"},
	{L"DEVCLASS_TUSSCR_GUID", L"{88163310-4D1D-42bd-9EBC-4F50A26A7C63}"},
	{L"DEVCLASS_TUSPRINTER_GUID", L"{825564FC-2662-41fb-B275-BCFE25805EB9}"},
	{NULL,NULL}
};

TCHAR* getIClassInfo(TCHAR* szGUID, TCHAR* szInfo){
	TCHAR szTxt[100];
	int i=0;
	wsprintf(szTxt, L"unknown");
	do{
		if(wcsicmp(szGUID, _iClassInfos[i].szGUID)==0){
			wsprintf(szTxt, _iClassInfos[i].szInfo);
			break;
		}
		i++;
	}while(_iClassInfos[i].szGUID!=NULL);
	wcscpy(szInfo, szTxt);
	return szInfo;
}
