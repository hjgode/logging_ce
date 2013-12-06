//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  

pkfuncs.h

Abstract:

Private version of kfuncs.h

Notes: 


--*/
    #ifndef _PRIV_KFUNCS_H__
    #define _PRIV_KFUNCS_H__

#include <windows.h>

#include <excpt.h>

#include <tlhelp32.h>
//#include <toolhelp.h>
#pragma comment (lib, "toolhelp.lib")

#include <winioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

// @doc EXTERNAL KERNEL
// @topic Kernel Entrypoints | Kernel Entrypoints

/* function signature generation macros */
#define ARG_DW          0
#define ARG_PTR         1
#define ARG_I64         2
#define ARG_TYPE_MASK   0x03
#define ARG_TYPE_BITS   2

#define _ARG(arg, inx)  (ARG_ ## arg << ARG_TYPE_BITS*inx)
#define FNSIG0()    0
#define FNSIG1(a0)  _ARG(a0,0)
#define FNSIG2(a0, a1)  (_ARG(a0,0)|_ARG(a1,1))
#define FNSIG3(a0, a1, a2)  (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2))
#define FNSIG4(a0, a1, a2, a3)  (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3))
#define FNSIG5(a0, a1, a2, a3, a4) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4))
#define FNSIG6(a0, a1, a2, a3, a4, a5) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5))
#define FNSIG7(a0, a1, a2, a3, a4, a5, a6) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6))
#define FNSIG8(a0, a1, a2, a3, a4, a5, a6, a7) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7))
#define FNSIG9(a0, a1, a2, a3, a4, a5, a6, a7, a8) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8))
#define FNSIG10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9))
#define FNSIG11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|_ARG(a10,10))
#define FNSIG12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|_ARG(a10,10)|_ARG(a11,11))
#define FNSIG13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
    (_ARG(a0,0)|_ARG(a1,1)|_ARG(a2,2)|_ARG(a3,3)|_ARG(a4,4)|_ARG(a5,5)|_ARG(a6,6) \
    |_ARG(a7,7)|_ARG(a8,8)|_ARG(a9,9)|_ARG(a10,10)|_ARG(a11,11)|_ARG(a12,12))

// KPage definitions
#define TH_KMODE 1
#define TH_UMODE 0

typedef struct ProcStartInfo {
    LPWSTR lpszImageName;
    LPWSTR lpszCmdLine;
    LPPROCESS_INFORMATION lppi;
    HANDLE he;
    BOOL fdwCreate;
} ProcStartInfo, *LPProcStartInfo;
    
#define CURTLSPTR_OFFSET 0x000
#define KINFO_OFFSET     0x300

#define UCurThread() (((HANDLE *)(PUserKData+SYSHANDLE_OFFSET))[SH_CURTHREAD])
#define UTlsPtr() (*(LPDWORD *)(PUserKData+CURTLSPTR_OFFSET))

#define TLSSLOT_MSGQUEUE    0
#define TLSSLOT_RUNTIME     1
#define TLSSLOT_KERNEL      2

#define TLSSLOT_ICECAP      3

#define TLSKERN_TRYINGTODIE     0x00000001
#define TLSKERN_NOFAULT         0x00000002  // Don't break into the debugger if set
#define TLSKERN_CECAP_PROFILING 0x00000004  // CECAP is currently profiling
#define TLSKERN_IN_LOADER       0x00000008  // Holding loader critical section
#define TLSKERN_NOFAULTMSG      0x00000010  // used with TLSKERN_NOFAULT, skip the display of exception message

#define TLSSLOT_NUMRES      4    // number of reserved slots (must be in order from 0)
#define TLSSLOT_RESERVE     0x0F // bitfield - reserve low 3 slots

// We move the stack base/bound as well as Current running Fiber directly 
// preceeding the TLS data so that coredll can access them and implement
// fast Fiber switching
#define PRETLS_STACKBASE        -1   // stack base now moved to PRE-TLS block
#define PRETLS_STACKBOUND       -2   // stack bound now moved to PRE-TLS block
#define PRETLS_CURFIBER         -3   // current running fiber in PRE-TLS block

// NOTE: we only allow threads with default stack size to be converted to fiber,
//       and all fibers have the stack size of the default process stack size
#define PRETLS_PROCSTKSIZE      -4   // process default stack size

#define PRETLS_THRDINFO         -5   // current thread's information (bit fields, only bit 0 used for now)

#define UTLS_INKMODE            0x00000001  // bit 1 set if in kmode
#define UTLS_NKCALLOUT          0x00000002  // bit 2 set if NK callout to FILESYS

// the follow 2 are only valid during PSL call
#define PRETLS_CALLERTRUST      -6  // caller's trust, only valid if it's in PSL call
#define PRETLS_CALLERVMBASE     -7  // caller's VM base, only valid if it's in PSL call

// how many field preceeding TLS is reserved 
#define PRETLS_RESERVED          8

// macros for accessing PRETLS data
#define UStkBase()               (UTlsPtr()[PRETLS_STACKBASE])
#define UStkBound()              (UTlsPtr()[PRETLS_STACKBOUND])
#define UCurFiber()              (UTlsPtr()[PRETLS_CURFIBER])
#define UProcStkSize()           (UTlsPtr()[PRETLS_PROCSTKSIZE])

#define PSLGetCallerTrust()      ((UTlsPtr()[PRETLS_THRDINFO] & UTLS_NKCALLOUT)? OEM_CERTIFY_TRUST : UTlsPtr()[PRETLS_CALLERTRUST])
#define PSLGetCallerVMBase()     (UTlsPtr()[PRETLS_CALLERVMBASE])

#define IsThrdInKMode()          (UTlsPtr()[PRETLS_THRDINFO] & UTLS_INKMODE)

#define TLS_PWRHDLR              ((LPDWORD) 0xff)    // an invalid number that is guarantee to fault
#define IsInPwrHdlr()            (UTlsPtr () == TLS_PWRHDLR)

#define SECTION_MASK        0x03F
#define VA_SECTION          25
#define UnMapPtr(P) ((((DWORD)(P) & 0x80000000) && !IsSecureVa(P))? ((DWORD)(P)) : ((DWORD)(P)) & ~(SECTION_MASK<<VA_SECTION))

/* indices for for UserKInfo array in kernel data page */
#define UserKInfo  ((long *)(PUserKData+KINFO_OFFSET))

#define KINX_PROCARRAY  0   /* address of process array */
#define KINX_PAGESIZE   1   /* system page size */
#define KINX_PFN_SHIFT  2   /* shift for page # in PTE */
#define KINX_PFN_MASK   3   /* mask for page # in PTE */
#define KINX_PAGEFREE   4   /* # of free physical pages */
#define KINX_SYSPAGES   5   /* # of pages used by kernel */
#define KINX_KHEAP      6   /* ptr to kernel heap array */
#define KINX_SECTIONS   7   /* ptr to SectionTable array */
#define KINX_MEMINFO    8   /* ptr to system MemoryInfo struct */
#define KINX_MODULES    9   /* ptr to module list */
#define KINX_DLL_LOW   10   /* lower bound of DLL shared space */
#define KINX_NUMPAGES  11   /* total # of RAM pages */
#define KINX_PTOC      12   /* ptr to ROM table of contents */
#define KINX_KDATA_ADDR 13  /* kernel mode version of KData */
#define KINX_GWESHEAPINFO 14 /* Current amount of gwes heap in use */
#define KINX_TIMEZONEBIAS 15 /* Fast timezone bias info */
#define KINX_PENDEVENTS 16  /* bit mask for pending interrupt events */
#define KINX_KERNRESERVE 17 /* number of kernel reserved pages */
#define KINX_API_MASK 18    /* bit mask for registered api sets */
#define KINX_NLS_CP 19      /* hiword OEM code page, loword ANSI code page */
#define KINX_NLS_SYSLOC 20  /* Default System locale */
#define KINX_NLS_USERLOC 21 /* Default User locale */
#define KINX_HEAP_WASTE 22  /* Kernel heap wasted space */
#define KINX_DEBUGGER 23    /* For use by debugger for protocol communication */
#define KINX_APISETS 24     /* APIset pointers */
#define KINX_MINPAGEFREE 25 /* water mark of the minimum number of free pages */
#define KINX_CELOGSTATUS 26 /* CeLog status flags */
#define KINX_NKSECTION  27  /* Address of NKSection */
#define KINX_PWR_EVTS   28  /* Events to be set after power on */

#define KINX_NKSIG     31   /* last entry of KINFO -- signature when NK is ready */
#define NKSIG          0x4E4B5347       /* signature "NKSG" */

/* For RegisterApiSet */
#define REGISTER_APISET_TYPE 0x80000000

typedef struct _CALLBACKINFO {
    HANDLE  hProc;      /* destination process */
    FARPROC pfn;        /* function to call in dest. process */
    PVOID   pvArg0;     /* arg0 data */
} CALLBACKINFO;
typedef CALLBACKINFO *PCALLBACKINFO;

/* Fast path for file system mapping of multiple pointers quickly */
#define SECTION_SHIFT 25     // Must be in sync with VA_SECTION in kapi.h, mem_*.h
// secure section related defs
#define SECURE_SECTION      0x61    // VM at 0xC2XXXXXX
#define SECURE_VMBASE       (SECURE_SECTION << SECTION_SHIFT)
#define IsSecureVa(va)      (SECURE_SECTION == ((DWORD) (va) >> SECTION_SHIFT))

#define MAX_PROCESSES 32
#define RESERVED_SECTIONS 1     // reserve section 0 for current process

#define FIRST_MAPPER_ADDRESS ((MAX_PROCESSES+RESERVED_SECTIONS) << SECTION_SHIFT)
#define LAST_MAPPER_ADDRESS 0x7E000000

#define ZeroPtrABS(P) ((((DWORD)(P) & 0x80000000) && !IsSecureVa(P)) ? \
    (DWORD)(P) : ((DWORD)(P) & ((1<<SECTION_SHIFT)-1)))

#define ZeroPtr(P)  ((((DWORD)(P) < (2<<SECTION_SHIFT)) || ((int) (P) >= FIRST_MAPPER_ADDRESS)) ? \
    (DWORD) (P) : ZeroPtrABS(P))

#define MapPtrWithBits(Ptr, Bits) (!(Ptr) || ((DWORD)(Ptr)>>SECTION_SHIFT) ? \
    (LPVOID)(Ptr) : (LPVOID)((DWORD)(Ptr)|(Bits)))

#define CECOMPRESS_ALLZEROS 0
#define CECOMPRESS_FAILED   0xffffffffUL
#define CEDECOMPRESS_FAILED 0xffffffffUL

/* Flags for SetHardwareWatch */
 
#define HARDWARE_WATCH_WRITE 0x1
#define HARDWARE_WATCH_READ  0x2

/* Flags for CacheSync/CacheRangeFlush */
#define CACHE_SYNC_DISCARD      0x001   /* write back & discard all cached data */
#define CACHE_SYNC_INSTRUCTIONS 0x002   /* discard all cached instructions */
#define CACHE_SYNC_WRITEBACK    0x004   /* write back but don't discard data cache*/
#define CACHE_SYNC_FLUSH_I_TLB  0x008   /* flush I-TLB */
#define CACHE_SYNC_FLUSH_D_TLB  0x010   /* flush D-TLB */
#define CACHE_SYNC_FLUSH_TLB    (CACHE_SYNC_FLUSH_I_TLB|CACHE_SYNC_FLUSH_D_TLB)    /* flush all TLB */
#define CACHE_SYNC_L2_WRITEBACK 0x020   /* write-back L2 Cache */
#define CACHE_SYNC_L2_DISCARD   0x040   /* discard L2 Cache */

#define CACHE_SYNC_ALL          0x07F   /* sync and discard everything in Cache/TLB */

// Print flags for resource tracking
#define PRINT_ALL           (DWORD)0x00000100
#define PRINT_RECENT        (DWORD)0x00000200
#define PRINT_DETAILS       (DWORD)0x00000400
#define PRINT_TRACE         (DWORD)0x00000800
#define PRINT_FILTERTYPE    (DWORD)0x00001000
#define PRINT_FILTERPROCID  (DWORD)0x00002000
#define PRINT_FILTERHANDLE  (DWORD)0x00004000
#define PRINT_SETCHECKPOINT (DWORD)0x00008000

// Filter flags for resource tracking
#define FILTER_TYPEDEFAULTON   0x00000001
#define FILTER_TYPEDEFAULTOFF  0x00000002
#define FILTER_TYPEON          0x00000004
#define FILTER_TYPEOFF         0x00000008
#define FILTER_PROCDEFAULTOFF  0x00000020   // not yet supported
#define FILTER_PROCIDON        0x00000040
#define FILTER_PROCIDOFF       0x00000080   // not yet supported

// special flag for CreateFileMapping
#define PAGE_INTERNALDBMAPPING 0x20000000

// Flags for physical memory types. Used by IOCTL_QUERY_PHYSICALMEM
#define PHYSICAL_ROM        (DWORD)0x00000001
#define PHYSICAL_RAM        (DWORD)0x00000002
#define PHYSICAL_IO         (DWORD)0x00000004
#define PHYSICAL_UNKNOWN    (DWORD)0x00000008

// Flags for processor info used by IOCTL_PROCESSOR_INFORMATION
#define PROCESSOR_FLOATINGPOINT     (DWORD)0x00000001
#define PROCESSOR_DSP               (DWORD)0x00000002
#define PROCESSOR_16BITINSTRUCTION  (DWORD)0x00000004

// Kernel IOCTLs based on FILE_DEVICE_HAL -- last ID used is 67
// Note that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers..
// Internal product teams are currently using values in the range of 1000-1100
#define IOCTL_HAL_GET_DEVICE_INFO   CTL_CODE(FILE_DEVICE_HAL, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_IP_ADDR       CTL_CODE(FILE_DEVICE_HAL, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_RADIO_CNTRL       CTL_CODE(FILE_DEVICE_HAL, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_DDK_CALL          CTL_CODE(FILE_DEVICE_HAL, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_EDBG_REGISTER_CLIENT  CTL_CODE(FILE_DEVICE_HAL, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_DEREGISTER_CLIENT CTL_CODE(FILE_DEVICE_HAL, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_REGISTER_DFLT_CLIENT CTL_CODE(FILE_DEVICE_HAL, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_SEND             CTL_CODE(FILE_DEVICE_HAL, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_RECV             CTL_CODE(FILE_DEVICE_HAL, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EDBG_SET_DEBUG        CTL_CODE(FILE_DEVICE_HAL,11, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_KERNEL_COMM_DEV   CTL_CODE(FILE_DEVICE_HAL, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_UUID          CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
    
#define IOCTL_HAL_INIT_RTC          CTL_CODE(FILE_DEVICE_HAL, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_REBOOT            CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SET_KERNEL_DEV_PORT   CTL_CODE(FILE_DEVICE_HAL, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_TRANSLATE_IRQ     CTL_CODE(FILE_DEVICE_HAL, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_RIO_INFO      CTL_CODE(FILE_DEVICE_HAL, 19, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_ILTIMING          CTL_CODE(FILE_DEVICE_HAL, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_GET_DEVICEID      CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_PROFILE           CTL_CODE(FILE_DEVICE_HAL, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_SET_DEVICE_INFO   CTL_CODE(FILE_DEVICE_HAL, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_QUERY_PHYSICALMEM     CTL_CODE(FILE_DEVICE_HAL, 24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROCESSOR_INFORMATION CTL_CODE(FILE_DEVICE_HAL, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_GETREGSECUREKEYS  CTL_CODE(FILE_DEVICE_HAL, 35, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_REQUEST_IRQ       CTL_CODE(FILE_DEVICE_HAL, 36, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_REQUEST_SYSINTR   CTL_CODE(FILE_DEVICE_HAL, 38, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_RELEASE_SYSINTR   CTL_CODE(FILE_DEVICE_HAL, 54, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to get KITL transport info
#define IOCTL_KITL_GET_INFO         CTL_CODE(FILE_DEVICE_HAL, 37, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HAL_ENABLE_WAKE       CTL_CODE(FILE_DEVICE_HAL, 40, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_DISABLE_WAKE      CTL_CODE(FILE_DEVICE_HAL, 41, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_GET_WAKE_SOURCE   CTL_CODE(FILE_DEVICE_HAL, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_AUTOWAKEUP        CTL_CODE(FILE_DEVICE_HAL, 43, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_PRESUSPEND        CTL_CODE(FILE_DEVICE_HAL, 61, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to tell OEM to initialize registry on boot
#define IOCTL_HAL_INITREGISTRY      CTL_CODE(FILE_DEVICE_HAL, 44, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to give OEM a more full-featured init when the kernel is ready to go
// and before any apps are started
#define IOCTL_HAL_POSTINIT          CTL_CODE(FILE_DEVICE_HAL, 45, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to ask OEM whether to force clean registry hive
#define IOCTL_HAL_GET_HIVE_CLEAN_FLAG CTL_CODE(FILE_DEVICE_HAL, 49, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to ask OEM the reason for power on
#define IOCTL_HAL_GET_POWERONREASON CTL_CODE(FILE_DEVICE_HAL, 50, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to get random hardware seed
#define IOCTL_HAL_GET_RANDOM_SEED   CTL_CODE(FILE_DEVICE_HAL, 52, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to ask OEM for a RAM region to put the registry hive into
#define IOCTL_HAL_GET_HIVE_RAM_REGION CTL_CODE(FILE_DEVICE_HAL, 53, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to tell OEM to save the RAM region where the registry hive is stored
#define IOCTL_HAL_SAVE_HIVE_RAM_REGION CTL_CODE(FILE_DEVICE_HAL, 55, METHOD_BUFFERED, FILE_ANY_ACCESS)


// IOCTL to write XIP section.
#define IOCTL_HAL_WRITE_XIP         CTL_CODE(FILE_DEVICE_HAL, 46, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_XIPCHAIN          CTL_CODE(FILE_DEVICE_HAL, 47, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL for authenticate device
#define IOCTL_HAL_AUTHENTICATE_DEVICE   CTL_CODE(FILE_DEVICE_HAL, 60, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL for WHQL testing (used to check whether hardware is in a jumpered test mode)
#define IOCTL_HAL_WHQL_TEST_MODE           CTL_CODE(FILE_DEVICE_HAL, 56, METHOD_BUFFERED, FILE_ANY_ACCESS)
// IOCTL for retrieving location and size of bootloader in flash
#define IOCTL_HAL_GET_BOOTLOADER_PARAMS    CTL_CODE(FILE_DEVICE_HAL, 57, METHOD_BUFFERED, FILE_ANY_ACCESS)
// IOCTL for retrieving the signed state of an OS image
#define IOCTL_HAL_GET_IMAGE_SIGNED_STATE   CTL_CODE(FILE_DEVICE_HAL, 58, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to query for a default display resolution.
#define IOCTL_HAL_QUERY_DISPLAYSETTINGS    CTL_CODE(FILE_DEVICE_HAL, 63, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to query device for device-specific information
#define IOCTL_HAL_QUERY_DEVICE_SETTINGS  CTL_CODE(FILE_DEVICE_HAL, 64, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to allow the HAL to override the extension region
#define IOCTL_HAL_GET_BIN_CHAIN  CTL_CODE(FILE_DEVICE_HAL, 65, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to retrieve cache information
#define IOCTL_HAL_GET_CACHE_INFO  CTL_CODE(FILE_DEVICE_HAL, 66, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to control OEM-specified profiler 
#define IOCTL_HAL_OEM_PROFILER        CTL_CODE(FILE_DEVICE_HAL, 67, METHOD_BUFFERED, FILE_ANY_ACCESS)

////////////////////////////////////////////////////////////////////////////////
//  VBridge support in kernel, used by VMini (Null miniport driver).
//
#define VMINI_RX_EVENT                          TEXT("VMiniRxEvent")    //  VMini miniport waits for this.
#define EDBG_INTERRUPT_EVENT                    TEXT("EdbgIntEvent")    //  Named event EDGB wait for.
#define IOCTL_VBRIDGE_GET_TX_PACKET             CTL_CODE(FILE_DEVICE_HAL, 26, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_TX_PACKET_COMPLETE    CTL_CODE(FILE_DEVICE_HAL, 27, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_RX_PACKET             CTL_CODE(FILE_DEVICE_HAL, 28, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_RX_PACKET_COMPLETE    CTL_CODE(FILE_DEVICE_HAL, 29, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_GET_ETHERNET_MAC          CTL_CODE(FILE_DEVICE_HAL, 30, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_ADD_MAC                   CTL_CODE(FILE_DEVICE_HAL, 31, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_SHARED_ETHERNET           CTL_CODE(FILE_DEVICE_HAL, 32, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_CURRENT_PACKET_FILTER     CTL_CODE(FILE_DEVICE_HAL, 33, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VBRIDGE_802_3_MULTICAST_LIST      CTL_CODE(FILE_DEVICE_HAL, 34, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_VBRIDGE_WILD_CARD					CTL_CODE(FILE_DEVICE_HAL, 39, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
//	These are only handled in vbridge.lib
//

#define IOCTL_VBRIDGE_WILD_CARD_RESET_BUFFER	CTL_CODE(FILE_DEVICE_HAL, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_VBRIDGE_WILD_CARD_VB_INITIALIZED	CTL_CODE(FILE_DEVICE_HAL, 43, METHOD_BUFFERED, FILE_ANY_ACCESS)

//  Ioctl to retrieve OutputDebugString
#define IOCTL_EDBG_GET_OUTPUT_DEBUG_FN          CTL_CODE(FILE_DEVICE_HAL, 48, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_EDBG_IS_STARTED                   CTL_CODE(FILE_DEVICE_HAL, 51, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL to retrieve hardware-generated entropy
#define IOCTL_HAL_GET_HWENTROPY                 CTL_CODE(FILE_DEVICE_HAL, 62, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Used by IOCTL_HAL_GET_IMAGE_SIGNED_STATE
typedef enum
{
    IMAGE_NOT_SIGNED = 0,
    IMAGE_TEST_SIGNED,
    IMAGE_FINAL_SIGNED
} IMAGE_SIGNED_STATE;

// Used by IOCTL_HAL_GET_BOOTLOADER_PARAMS
typedef struct _EBOOT_PARAMS_
{
    DWORD dwEbootStartAddress;
    DWORD dwEbootSize;
    DWORD dwEbootROMOffset;
} EBOOT_PARAMS, *PEBOOT_PARAMS;

// Used by IOCTL_QUERY_PHYSICALMEM
typedef struct _PHYSICAL_BASIC_INFORMATION {
    PVOID   lpBaseAddress;
    DWORD   dwRegionSize;
    DWORD   dwType;
} PHYSICAL_BASIC_INFORMATION;

typedef PHYSICAL_BASIC_INFORMATION *PPHYSICAL_BASIC_INFORMATION;

// Used by IOCTL_PROCESSOR_INFORMATION
typedef struct _PROCESSOR_INFO {
    WORD    wVersion;
    WCHAR   szProcessCore[40];
    WORD    wCoreRevision;
    WCHAR   szProcessorName[40];
    WORD    wProcessorRevision;
    WCHAR   szCatalogNumber[100];
    WCHAR   szVendor[100];
    DWORD   dwInstructionSet;
    DWORD   dwClockSpeed;
} PROCESSOR_INFO;

typedef PROCESSOR_INFO *PPROCESSOR_INFO;

// Used by IOCT_HAL_GET_IRQ to assign an IRQ resource to a device
//
// This structure describes the hardware location of a device in terms of it's interface,
// bus number and logical location, and is typically used for describing PCI devices.  The 
// Pin member describes which pin (1-4) of a PCI device is being used for the interrupt.
// The PhysicalLoc member is reserved for future implementations.  The LogicalLoc parameter
// is used to identify the location of the device on the interface/bus.  For PCIbus, the
// bits of the DWORD are defined as: 31-24: 0, 23-16: bus number, 15-8: device number,
// 7-0: function number.
typedef struct _DEVICE_LOCATION {
    DWORD IfcType;                  // One of the INTERFACE_TYPE enum, typically PCIBus
    DWORD BusNumber;                // Bus number, typically the PCI bus number the device resides on
    DWORD LogicalLoc;               // Defines logical location of device.  See above for PCIbus example. 
    PVOID PhysicalLoc;              // Reserved for future use
    DWORD Pin;                      // PCIbus IRQ pin
} DEVICE_LOCATION, *PDEVICE_LOCATION;

typedef struct _DEVICE_PSL_NOTIFY {
    DWORD   dwSize;
    DWORD   dwFlags;
    HANDLE  hProc;
    HANDLE  hThread;
} DEVICE_PSL_NOTIFY, *PDEVICE_PSL_NOTIFY;

#define IOCTL_PSL_NOTIFY CTL_CODE(FILE_DEVICE_PSL, 255, METHOD_NEITHER, FILE_ANY_ACCESS)

typedef struct _DEVICE_ID {
    DWORD   dwSize;
    DWORD   dwPresetIDOffset;
    DWORD   dwPresetIDBytes;
    DWORD   dwPlatformIDOffset;
    DWORD   dwPlatformIDBytes;
} DEVICE_ID, *PDEVICE_ID;

typedef void (*TRACKER_CALLBACK)(DWORD dwFlags, DWORD dwType, HANDLE hItem,
    DWORD dwProc, BOOL fDeleted, DWORD dwSize, DWORD dw1, DWORD dw2);

#define PerformCallBack PRIV_WIN32_CALL(DWORD, PerformCallBack, (PCALLBACKINFO pcbi, ...))
#define PerformCallForward PRIV_WIN32_CALL(DWORD, PerformCallBack, (PCALLBACKINFO pcbi, ...))
#define RegisterGwesHandler PRIV_WIN32_CALL(void, RegisterGwesHandler, (LPVOID pfn))
DWORD PerformCallBack4(PCALLBACKINFO pcbi, ...);
#ifdef COREDLL
DWORD xxx_PerformCallBack4(PCALLBACKINFO pcbi, ...);
#define PerformCallBack4 xxx_PerformCallBack4
#endif

BOOL CeMapArgumentArray(HANDLE hProc, LPVOID *pArgList, DWORD dwSig);
BOOL CeSetExtendedPdata(LPVOID pData);

void ForcePageout(void);
HANDLE CreateAPISet(char acName[4], USHORT cFunctions, const PFNVOID *ppfnMethods, const DWORD *pdwSig);
BOOL VirtualCopy(LPVOID lpvDest, LPVOID lpvSrc, DWORD cbSize, DWORD fdwProtect);

/** Flags for LockPages: */
#define LOCKFLAG_WRITE      0x001   // write access required
#define LOCKFLAG_QUERY_ONLY 0x002   // query only, page in but don't lock
#define LOCKFLAG_READ       0x004   // read access required (as opposed to page present but PAGE_NOACCESS)

BOOL LockPages(LPVOID lpvAddress, DWORD cbSize, PDWORD pPFNs, int fOptions);
BOOL UnlockPages(LPVOID lpvAddress, DWORD cbSize);

LPVOID AllocPhysMem(DWORD cbSize, DWORD fdwProtect, DWORD dwAlignmentMask, DWORD dwFlags, PULONG pPhysicalAddress);
BOOL   FreePhysMem(LPVOID lpvAddress);

void SleepTillTick (void);

#ifdef SH4
#define VSPF_NONE       0x00000000
#define VSPF_VARIABLE   0x00000001
#define VSPF_8BIT       0x00000000  // default
#define VSPF_16BIT      0x00000002
#define VSPF_IOSPACE    0x00000004
#define VSPF_COMMON     0x00000008
#define VSPF_ATTRIBUTE  0x0000000c
#define VSPF_TC         0x00000010
BOOL VirtualSetPageFlags(LPVOID lpvAddress, DWORD cbSize, DWORD dwFlags, LPDWORD lpdwOldFlags);
BOOL SetRAMMode(BOOL bEnable, LPVOID *lplpvAddress, LPDWORD lpLength);
LPVOID SetStoreQueueBase(DWORD dwPhysPage);
#endif
BOOL VirtualSetAttributes (LPVOID lpvAddress, DWORD cbSize, DWORD dwNewFlags, DWORD dwMask, LPDWORD lpdwOldFlags);

DWORD CeModuleJit(LPCWSTR, LPWSTR, HANDLE*);
void ThreadAttachAllDLLs(void);
void ThreadDetachAllDLLs(void);
int U_ropen(const WCHAR *, UINT);
int U_rread(int, BYTE *, int);
int U_rwrite(int, BYTE *, int);
int U_rlseek(int, int, int);
int U_rclose(int);
VOID NKvDbgPrintfW(LPCWSTR lpszFmt, CONST VOID *lpParms);
VOID ProfileSyscall(LPDWORD);
BOOL GetRealTime(LPSYSTEMTIME);
BOOL SetRealTime(LPSYSTEMTIME);
void ProcessDetachAllDLLs(void);
LPVOID ExtractResource(LPCWSTR lpszFile, LPCWSTR lpszName, LPCWSTR lpszType);
UINT KernExtractIcons(LPCWSTR lpszFile, int nIconIndex, LPBYTE *pIconLarge, LPBYTE *pIconSmall, CALLBACKINFO *pcbi);
BOOL GetRomFileInfo(DWORD type, LPWIN32_FIND_DATA  lpfd, DWORD count);
DWORD GetRomFileBytes(DWORD type, DWORD count, DWORD pos, LPVOID buffer, DWORD nBytesToRead);
#ifdef _WIN32_WCE_EMULATION
    #define CacheSync(x)
    #define CacheRangeFlush(a, b, c)
#else
void CacheSync(int flags);
void CacheRangeFlush (LPVOID pAddr, DWORD dwLength, DWORD dwFlags);
#endif
BOOL AddTrackedItem(DWORD dwType, HANDLE handle, TRACKER_CALLBACK cb, DWORD dwProc, DWORD dwSize, DWORD dw1, DWORD dw2);
BOOL DeleteTrackedItem(DWORD dwType, HANDLE handle);
BOOL PrintTrackedItem(DWORD dwFlag, DWORD dwType, DWORD dwProcID, HANDLE handle);
BOOL GetKPhys(void *ptr, ULONG length);
BOOL GiveKPhys(void *ptr, ULONG length);
#ifndef _WIN32_WCE_EMULATION
void SetExceptionHandler(PEXCEPTION_ROUTINE per);
#endif
DWORD RegisterTrackedItem(LPWSTR szName);
VOID FilterTrackedItem(DWORD dwFlags, DWORD dwType, DWORD dwProcID);
void SetKernelAlarm(HANDLE hEvent, LPSYSTEMTIME lpst);
void RefreshKernelAlarm(void);
void CloseProcOE(DWORD);

// New API is SetOOMEvent(), SetGwesOOMEvent will go away.
VOID SetGwesOOMEvent(HANDLE hEvt, DWORD cpLow, DWORD cpCritical, DWORD cpLowBlockSize, DWORD cpCriticalBlockSize);
VOID SetOOMEvent(HANDLE hEvt, DWORD cpLow, DWORD cpCritical, DWORD cpLowBlockSize, DWORD cpCriticalBlockSize);
DWORD StringCompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
DWORD StringDecompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
DWORD BinaryCompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
DWORD BinaryDecompress(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout, DWORD skip);
DWORD DecompressBinaryBlock(LPBYTE bufin, DWORD lenin, LPBYTE bufout, DWORD lenout);
int InputDebugCharW(VOID);
void TakeCritSec(LPCRITICAL_SECTION lpcs);
void LeaveCritSec(LPCRITICAL_SECTION lpcs);
LPVOID MapPtrToProcess(LPVOID lpv, HANDLE hProc);
LPVOID MapPtrUnsecure(LPVOID lpv, HANDLE hProc);
HANDLE GetProcFromPtr(LPVOID lpv);
BOOL IsBadPtr(DWORD flags, LPBYTE ptr, UINT length);
DWORD GetProcAddrBits(HANDLE hProc);
DWORD GetFSHeapInfo(void);
BOOL OtherThreadsRunning(void);
VOID KillAllOtherThreads(void);
HANDLE GetOwnerProcess(void);
HANDLE GetCallerProcess(void);
DWORD GetCallerProcessIndex(void);
DWORD GetProcessIndexFromID(HANDLE hProc);
HANDLE GetProcessIDFromIndex(DWORD dwIdx);
#define GetCurrentProcessIndex() GetProcessIndexFromID(GetCurrentProcess())
DWORD GetIdleTime(void);
DWORD SetLowestScheduledPriority(DWORD maxprio);
BOOL IsPrimaryThread(void);
LPBYTE CreateLocaleView(BOOL bFirst);
DWORD SetProcPermissions(DWORD);
DWORD GetCurrentPermissions(void);
void SetTimeZoneBias(DWORD dwBias, DWORD dwDaylightBias);
void SetCleanRebootFlag(void);
HANDLE CreateCrit(LPCRITICAL_SECTION lpcs);
void PowerOffSystem(void);
BOOL SetDbgZone(DWORD dwProcid, LPVOID lpvMod, LPVOID baseptr, DWORD zone, LPDBGPARAM lpdbgTgt);
VOID TurnOnProfiling(HANDLE hThread);
VOID WINAPI TurnOffProfiling(HANDLE hThread);
void NKTerminateThread(DWORD dwExitCode);
LPCWSTR GetProcName(void);
void CloseAllHandles(void);
BOOL SetHandleOwner(HANDLE h, HANDLE hProc);
HINSTANCE LoadDriver(LPCTSTR lpszFile);
HANDLE LoadIntChainHandler(LPCTSTR lpszFileName, LPCTSTR lpszFunctionName, BYTE bIRQ);
BOOL FreeIntChainHandler(HANDLE hInstance);
HANDLE LoadKernelLibrary(LPCWSTR lpszFileName);
BOOL InterruptInitialize(DWORD idInt, HANDLE hEvent, LPVOID pvData, DWORD cbData);
void InterruptMask (DWORD idInt, BOOL fDisable);
void InterruptDone(DWORD idInt);
void InterruptDisable(DWORD idInt);
DWORD SetKMode(DWORD);
BOOL SetPowerOffHandler(FARPROC pfn);
BOOL SetGwesPowerHandler(FARPROC pfn);
BOOL SetWDevicePowerHandler(FARPROC pfn);
BOOL ConnectDebugger(LPVOID pInit);
__int64 CeGetRandomSeed(void);
BOOL SetHardwareWatch(LPVOID vAddr, DWORD flags);
BOOL RegisterAPISet(HANDLE hASet, DWORD dwSetID);
HANDLE CreateAPIHandle(HANDLE hASet, LPVOID pvData);
LPVOID VerifyAPIHandle(HANDLE hASet, HANDLE h);
void PPSHRestart(void);
BOOL AddEventAccess(HANDLE hEvent);

// valid flags for PageOutModule
#define PAGE_OUT_PROCESS_ONLY                   0   // page out process/module only
#define PAGE_OUT_DLL_USED_ONLY_BY_THISPROC      1   // (ignored if hModule is a DLL) page out process 
                                                    // together with dependent modules used only by the process
#define PAGE_OUT_ALL_DEPENDENT_DLL              2   // (ignored if hModule is a DLL) page out process 
                                                    // together with all dependent modules. USE WITH CARE.
BOOL PageOutModule (HANDLE hModule, DWORD dwFlags);

// set/get event data
DWORD GetEventData(HANDLE hEvent);
BOOL SetEventData(HANDLE hEvent, DWORD dwData);

void DebugNotify(DWORD dwFlags, DWORD data);

//probes with ppc2003 sdk
//THSNAP *THCreateSnapshot(DWORD dwFlags, DWORD dwProcID);
//LPBYTE THGrow(THSNAP *pSnap, DWORD dwSize);
BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

void CompactAllHeaps (void);

//////////////////////////////////////////////////////////////////////////////////
// cache information structure, used with IOCTL_KLIB_GETCACHE_INFO
typedef struct _CacheInfo {
    DWORD   dwL1Flags;              // flags for L1 cache, see below
    DWORD   dwL1ICacheSize;         // total size in bytes
    DWORD   dwL1ICacheLineSize;     // line size in bytes
    DWORD   dwL1ICacheNumWays;      // number of ways, 1 for direct mapped
    DWORD   dwL1DCacheSize;         // total size in bytes
    DWORD   dwL1DCacheLineSize;     // line size in bytes
    DWORD   dwL1DCacheNumWays;      // number of ways, 1 for direct mapped
    DWORD   dwL2Flags;              // flags for L2 cache, see below
    DWORD   dwL2ICacheSize;         // total size in bytes, 0 means no L2 ICache
    DWORD   dwL2ICacheLineSize;     // line size in bytes
    DWORD   dwL2ICacheNumWays;      // number of ways, 1 for direct mapped
    DWORD   dwL2DCacheSize;         // total size in bytes, 0 means no L2 DCache
    DWORD   dwL2DCacheLineSize;     // line size in bytes
    DWORD   dwL2DCacheNumWays;      // number of ways, 1 for direct mapped

} CacheInfo, *PCacheInfo;
// cache info flags (applied to both L1 and L2
#define  CF_UNIFIED             0x01     // I and D caches unified, use ICache info fields
#define  CF_WRITETHROUGH        0x02     // writeback if not set
#define  CF_COHERENT            0x04     // no cache flush operations necessary

//
// CeGetCacheInfo: Get cache information
// Parameters:
//      cbSize: must be sizeof(CacheInfo)
//      pCacheInfo: pointer to CachInfo struct to receive the cache infomation
// Return Value:
//      TRUE if success, FALSE if failed (if OEM didn't implement the function
//
_inline BOOL CeGetCacheInfo (DWORD cbSize, PCacheInfo pCacheInfo)
{
    return KernelIoControl (IOCTL_HAL_GET_CACHE_INFO, 0, 0, pCacheInfo, cbSize, 0);
}


// CeLog status flags
#define CELOGSTATUS_DISABLED        0x0
#define CELOGSTATUS_ENABLED_GENERAL 0x1
#define CELOGSTATUS_ENABLED_PROFILE 0x2

// Is CeLog loaded and ready for use.
#define IsCeLogLoaded()           ((UserKInfo[KINX_CELOGSTATUS] & (CELOGSTATUS_ENABLED_GENERAL | CELOGSTATUS_ENABLED_PROFILE)) != 0)
// Is CeLog enabled for general logging.  This macro returns FALSE when CeLog 
// is in profile-only mode.
#define IsCeLogEnabled()          ((UserKInfo[KINX_CELOGSTATUS] & CELOGSTATUS_ENABLED_GENERAL) != 0)
// Is CeLog enabled for profiling logging.  Currently profiling is always
// enabled if CeLog is loaded.
#define IsCeLogProfilingEnabled() IsCeLogLoaded()
// Is CeLog enabled for kernel logging.  Identifies kernel cases that currently
// are always enabled whenever CeLog is loaded but may not be in the future.
#define IsCeLogKernelEnabled()    IsCeLogLoaded()


// Function pointers provided by CeLog for the kernel to use.  Registered with
// IOCTL_CELOG_REGISTER.
typedef struct _CeLogExportTable {
    DWORD   dwVersion;                // Version of this structure, set to 2
    PFNVOID pfnCeLogData;
    PFNVOID pfnCeLogInterrupt;
    PFNVOID pfnCeLogSetZones;
    FARPROC pfnCeLogQueryZones;       // Receives CeLogGetZones calls, minus last param
    DWORD   dwCeLogTimerFrequency;    // Less than or equal to QueryPerformanceFrequency()
} CeLogExportTable;


// Used with IOCTL_KLIB_CHANGEMAPFLUSHING
typedef struct _MapFlushInfo {
    LPCVOID lpBaseAddress;
    DWORD   dwFlags;
} MapFlushInfo;
#define FILEMAP_NORMAL             0    // Default
#define FILEMAP_NOBACKGROUNDFLUSH  1    // Turn off background flushing


//
// KernelLibIoControl definitions
//

// Predefined Kernel Module numbers
#define KMOD_INVALID    0
#define KMOD_CORE       1
#define KMOD_DBG        2               // Debugger specific extensions
#define KMOD_CELOG      3
#define KMOD_VERIFIER   4
#define KMOD_OAL        5               // Pass through to OEMIoControl
#define KMOD_MAX        5

// Kernel reserves the control code from 0-255
#define IOCTL_KLIB_ALLOCSHAREMEM      0
#define IOCTL_KLIB_FREESHAREMEM       1
#define IOCTL_KLIB_GETROMCHAIN        2
#define IOCTL_KLIB_GETCOMPRESSIONINFO 3
#define IOCTL_KLIB_CHANGEMAPFLUSHING  4         // Change flush settings for mapped view of a file
#define IOCTL_KLIB_GETALARMRESOLUTION 5         // Retrieves NKAlarmResolutionMSec from the kernel
#define IOCTL_KLIB_ISKDPRESENT        6         // Returns TRUE if KD is pressent FALSE otherwise    
#define IOCTL_KLIB_SETROMDLLBASE      7         // called from filesys only, for loading externel MODULE to slot1
#define IOCTL_KLIB_GETPROCMEMINFO     8         // get per-process id and r/w memory usage
#define IOCTL_KLIB_GETCALLSTACK       9         // get callstack of of current thread with context information
#define IOCTL_KLIB_JITGETCALLSTACK    10        // get the last call stack of the thread that have an un-handled exception
#define IOCTL_KLIB_SETIGNORENOFAULT   11        // enable/disable no-fault so that one can choose to break into debugger on threads with nofault set
#define IOCTL_KLIB_USER               256       // where OEM defined IOCTL starts.

// Debugger IOCTLs are separate from the rest of the kernel
#define IOCTL_DBG_INIT                0

// CeLog IOCTLs are separate from the rest of the kernel
#define IOCTL_CELOG_IMPORT            0
#define IOCTL_CELOG_REGISTER          1
#define IOCTL_CELOG_GETDESKTOPZONE  2

// Verifier IOCTLs are separate from the rest of the kernel
#define IOCTL_VERIFIER_IMPORT            0
#define IOCTL_VERIFIER_REGISTER          1


BOOL KernelLibIoControl(HANDLE hLib, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);


// Interface from OAL (inside kernel)
LPVOID NKCreateStaticMapping(DWORD dwPhysBase, DWORD dwSize);
// Interface from user-mode applications (kernel trap)
LPVOID CreateStaticMapping(DWORD dwPhysBase, DWORD dwSize);

#ifdef ARM
PFNVOID NKSetDataAbortHandler(PFNVOID pfnDataAbortHandler);
#endif


LPVOID MapCallerPtr (LPVOID ptr, DWORD dwLen);
LPVOID MapPtrToProcWithSize (LPVOID ptr, DWORD dwLen, HANDLE hProc);

// allocating shared memory for ISR/IST memory sharing
_inline BOOL KLibAllocShareMem (DWORD nPages, BOOL fNoCache, LPVOID *pVa, LPVOID *pPa)
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_ALLOCSHAREMEM,
        pVa, nPages, pPa, fNoCache, NULL);
}
_inline BOOL KLibFreeShareMem (DWORD nPages, LPVOID pVa, LPVOID pPa)
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_FREESHAREMEM,
        pVa, nPages, pPa, 0, NULL);
}

_inline LPVOID KLibGetROMChain (void)
{
    return (LPVOID) KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_GETROMCHAIN, 0, 0, 0, 0, 0);
}

_inline void KLibSetIgnoreNoFault (BOOL fIgnore)
{
    KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_SETIGNORENOFAULT, 0, fIgnore, 0, 0, 0);
}

typedef struct _PROCVMINFO {
    HANDLE hProc;           // process id
    DWORD  cbRwMemUsed;     // RW RAM used
} PROCVMINFO, *PPROCVMINFO;

_inline BOOL CeGetProcVMInfo (int idxProc, DWORD cbSize, PROCVMINFO *pInfo)
{
    return KernelLibIoControl((HANDLE) KMOD_CORE, IOCTL_KLIB_GETPROCMEMINFO, NULL, idxProc, pInfo, cbSize, NULL);
}

void NotifyForceCleanboot(void);
void DumpKCallProfile(DWORD bReset);
int QueryAPISetID(char *pName);
FARPROC GetAPIAddress(int setId, int iMethod);
// @CESYSGEN IF !REMOVE_DRAGON
BOOL SetStdioPathW(DWORD id, LPCWSTR pwszPath);
BOOL GetStdioPathW(DWORD id, PWSTR pwszBuf, LPDWORD lpdwLen);
// @CESYSGEN ENDIF
DWORD ReadRegistryFromOEM(DWORD dwFlags, LPBYTE pBuf, DWORD len);
BOOL WriteRegistryToOEM(DWORD dwFlags, LPBYTE pBuf, DWORD len);

BOOL FlushViewOfFileMaybe(LPCVOID lpBaseAddress, DWORD dwNumberOfBytesToFlush);

BOOL IsProcessDying(void);
void PSLNotify(DWORD flags, DWORD proc, DWORD thread);


HANDLE CreateKernelThread(LPTHREAD_START_ROUTINE lpStartAddr,
                          LPVOID lpvThreadParm, WORD prio, DWORD dwFlags);
// OEM entry point
#define NKCreateKernelThread  CreateKernelThread

// To force a clean boot (OEM entry point)
void NKForceCleanBoot (void);

//------------------------------------------------------------------------------
// Limited registry access from OAL during IOCTL_HAL_INITREGISTRY
//------------------------------------------------------------------------------

// These definitions are equivalent to the Win32 APIs

#ifndef REGSAM
typedef DWORD REGSAM;
#endif

LONG NKRegCreateKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved,
                       LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                       LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                       PHKEY phkResult, LPDWORD lpdwDisposition);
LONG NKRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                     REGSAM samDesired, PHKEY phkResult);
LONG NKRegCloseKey(HKEY hKey);
LONG NKRegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
                        LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
LONG NKRegSetValueExW(HKEY hKey, LPCWSTR lpValueName, DWORD dwReserved,
                      DWORD dwType, LPBYTE  lpData, DWORD cbData);

// Unicode only
#define NKRegCreateKeyEx    NKRegCreateKeyExW
#define NKRegOpenKeyEx      NKRegOpenKeyExW
#define NKRegQueryValueEx   NKRegQueryValueExW
#define NKRegSetValueEx     NKRegSetValueExW

//------------------------------------------------------------------------------

#pragma warning(disable:4200) // nonstandard extensions warning

// Definitions used with GetCallstackSnapshot

#define STACKSNAP_FAIL_IF_INCOMPLETE  1

typedef struct _CallSnapshot {
    DWORD dwReturnAddr;
} CallSnapshot;

typedef struct _StackSnapshot {
    WORD  wVersion;              // Version of this structure: Set to 1
    WORD  wNumCalls;             // Number of calls in this stack snapshot
    CallSnapshot rgCalls[0];     // Followed by an array of function calls
} StackSnapshot;

#pragma warning(default:4200) // nonstandard extensions warning

ULONG GetCallStackSnapshot (ULONG dwMaxFrames, CallSnapshot lpFrames[], DWORD dwFlags, DWORD dwSkip);

_inline DWORD KLibGetLastExcpCallStack (DWORD dwMaxFrame, CallSnapshot lpFrames [])
{
    return KernelLibIoControl ((HANDLE) KMOD_CORE, IOCTL_KLIB_JITGETCALLSTACK, 0, 0, lpFrames, dwMaxFrame, 0);
}


// The following are used to support KDHAL
//++++++++++++++++++++++++++++++++++
BOOL OEMKDIoControl( DWORD dwIoControlCode, LPVOID lpBuf, DWORD nBufSize);
typedef BOOL (*PFN_OEMKDIoControl)( DWORD dwIoControlCode, LPVOID lpBuf, DWORD nBufSize);
extern PFN_OEMKDIoControl pKDIoControl;

#define KD_IOCTL_INIT           0x00000000
#define KD_IOCTL_SET_CBP        0x00000001
#define KD_IOCTL_CLEAR_CBP      0x00000002
#define KD_IOCTL_QUERY_CBP      0x00000003
#define KD_IOCTL_ENUM_CBP       0x00000004
#define KD_IOCTL_SET_DBP        0x00000010
#define KD_IOCTL_CLEAR_DBP      0x00000011
#define KD_IOCTL_QUERY_DBP      0x00000012
#define KD_IOCTL_ENUM_DBP       0x00000013
#define KD_IOCTL_MAP_EXCEPTION  0x00000020
#define KD_IOCTL_RESET          0x00000030

//max size for save/restore CoProc registers
#define MAX_COPROCREGSIZE       512

typedef struct _KD_BPINFO{
    int     nVersion;
    ULONG   ulFlags;
    ULONG   ulAddress;
    ULONG   ulHandle;
    ULONG   ulCount;
} KD_BPINFO, *PKD_BPINFO;

typedef struct _KD_EXCEPTION_INFO {
    int     nVersion;
    ULONG   ulFlags;
    ULONG   ulAddress;
    ULONG   ulExceptionCode;
} KD_EXCEPTION_INFO, *PKD_EXCEPTION_INFO;
//++++++++++++++++++++++++++++++++++


#ifdef ARM
#define PAGE_ARM_MINICACHE 0x800
#endif

#ifdef x86
#define PAGE_x86_WRITETHRU 0x40000000
#endif

#ifdef __cplusplus
}
#endif

#endif

