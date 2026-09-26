/*
06-Mar-96 G-03-06  jlc     $$1  created
06-Jun-96 G-03-17  Amin    $$2  added to determine if Pentium Pro
14-Aug-96 H-01-05  FBI     $$3  Added unix_machine_names table.
24-Jan-97 H-01-25  EEB/MRC $$4 Added pro_is_win95_running()
09-Jul-97 H-03-17  amin    $$5 Fix build version for Windows 95  (SPR#536350)
09-OCT-97 H-03-26  JZ      $$6 Deleted obsolete SGI4D and _SYSTYPE_SYSV macros.
27-JAN-00 J-01-01  MTP     $$7 Added Linux.
21-Sep-01 J-03-09  jas     $$8 Optimized pro_is_win95_running()
19-Mar-02 J-03-21  ACT     $$9 Use registry to print hardware info.
13-May-02 J-03-25  ACT    $$10 Changed #9 to print only one CPU information.
17-Oct-02 J-03-35  hplab  $$11 system info for ia64_nt
12-May-04 K-03-03  ASRS   $$12 X86E_WIN64 support
06-Dec-04 K-03-16  CHI    $$13 fix dimension.
12-Dec-04 K-03-18  KSV    $$14 Removed eol after CPU info
 12-Feb-06  L-01-02 PROTO $$15  Automatic prototype creation
 24-Apr-06  L-01-07 TWH   $$16  use windows unicode wrappers
 12-Jul-06  L-01-12 ksi   $$17  Unicode compliant changes
26-MAR-07   L-01-29 MTP   $$18  Added pro_is_vista_running()
11-Jan-08   L-01-42 dimiii $$19 Corrected using output buffer as input buffer.
16-May-08   L-03-09 BI    $$20  Used syswindows.h instead of windows.h
25-Aug-08   L-03-16 KSV   $$21  Sandbox for Windows support
06-Jun-10   L-05-24 lli   $$22  Added pro_is_win7_running()
01-Jul-10   L-05-26 AC    $$23  Added Linux support
07-Mar-12   P-20-01 jas   $$24  Added support for apple_osx
12-Mar-12   P-20-01 AC    $$25  Updated for Project 13028358
05-Nov-13   P-20-42 DEM   $$26  Added pro_get_win32_os_name()
02-Feb-14   P-20-47 DEM   $$27  Fixed Windows Server 2008 info
02-Jul-15   P-30-12 jas   $$28  Added Windows 10
16-Sep-15   P-30-17 jas   $$29  Updated Windows 10 post-release
19-Feb-18   P-50-49 jas   $$30  Added Windows Server 2016
01-May-18   P-60-02 jas   $$31  Added pro_is_win10_running()
13-Jul-20   P-80-12 KSV   $$32  Removed sandbox leftovers
07-Jun-21   P-90-12 jas   $$33  Use Windows 10 DisplayVersion
09-Jun-21   P-90-13 jas   $$34  Removed gdi32_windows.h
29-Jun-21   P-90-16 jas   $$35  Added pro_is_win11_running()
17-Oct-22   Q-10-32 jas   $$36  Added uIsWindows11OrGreater()
25-May-23   Q-11-14 jas   $$37  Added Windows 11 server
18-Sep-23   Q-11-30 jas   $$38  Return build from pro_is_win10_running
*/

#include <ptc_win32.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include "const.h"
#include "pro_sys_info.h"

#if OPER_SYS == UNIX_OS
#include <sys/utsname.h>
#endif

#if OPER_SYS == WINDOWS_32
#include <syswindows.h>
#include <errors.h>
#include <ct_win_syscall_proto.h>
#include <pro_string.h>
#include <nt_registry.h>
#endif


static ProSystemInfo system_info;
static Bool system_info_initialized = FALSE;

#if OPER_SYS == WINDOWS_32
typedef struct td_IdToString
{
  DWORD id;
  char *string;
} IdToString;

typedef struct td_Win32OSName
{
  DWORD dwPlatformId;
  DWORD dwMajorVersion;
  DWORD dwMinorVersion;
  int isWorkstation;
  char *name;
} Win32OSName;

Win32OSName win32_os_names[] =
{
  {VER_PLATFORM_WIN32_NT, 11, 0, TRUE,  "Windows 11"},
  {VER_PLATFORM_WIN32_NT, 11, 0, FALSE,  "Windows 11"},
  {VER_PLATFORM_WIN32_NT, 10, 0, TRUE,  "Windows 10"},
  {VER_PLATFORM_WIN32_NT, 10, 0, FALSE, "Windows Server 2016"},
  {VER_PLATFORM_WIN32_NT, 6, 3, TRUE,  "Windows 8.1"},
  {VER_PLATFORM_WIN32_NT, 6, 3, FALSE, "Windows Server 2012 R2"},
  {VER_PLATFORM_WIN32_NT, 6, 2, TRUE,  "Windows 8"},
  {VER_PLATFORM_WIN32_NT, 6, 2, FALSE, "Windows Server 2012"},
  {VER_PLATFORM_WIN32_NT, 6, 1, TRUE,  "Windows 7"},
  {VER_PLATFORM_WIN32_NT, 6, 1, FALSE, "Windows Server 2008 R2"},
  {VER_PLATFORM_WIN32_NT, 6, 0, TRUE,  "Windows Vista"},
  {VER_PLATFORM_WIN32_NT, 6, 0, FALSE, "Windows Server 2008"},
  {VER_PLATFORM_WIN32_NT, 5, 2, TRUE,  "Windows XP"},
  {VER_PLATFORM_WIN32_NT, 5, 2, FALSE, "Windows Server 2003"},
  {VER_PLATFORM_WIN32_NT, 5, 1, -1,    "Windows XP"},
  {VER_PLATFORM_WIN32_NT, 5, 0, -1,    "Windows 2000"},
  {1,                     0, 0, -1,    "Windows 95"}, /* Should be VER_PLATFORM_WIN32_WINDOWS, but it's */
                                                    /* not in the include files we're using for some */
                                                    /* reason. */
  {0,                     0, 0, -1,    "Unknown"}
};

IdToString win32_machine_names[] =
{
  {PROCESSOR_INTEL_386, "386"},
  {PROCESSOR_INTEL_486, "486"},
  {PROCESSOR_INTEL_PENTIUM, "Pentium"},
  {PROCESSOR_MIPS_R4000, "Mips R4000"},
  {PROCESSOR_ALPHA_21064, "Alpha 21064"},
  {0, "Unknown"}
};

#elif OPER_SYS == UNIX_OS

typedef struct td_ProMachineToName
{
 int	pro_machine_id;
 char   *pro_machine_name;
} ProMachineToName;

/* Define a table to map PRO_MACHINE_TYPE to a machine_name
 * in ProSystemInfo structure. (Ref: pro_hardware.h).
 */
ProMachineToName unix_machine_name[] =
{
#if   (PRO_MACHINE == NEC)
   {NEC, 	"NEC"}
#elif (PRO_MACHINE == SGI_R4K)
   {SGI_R4K, 	"SGI"}
#elif (PRO_MACHINE == HP300) || (PRO_MACHINE == HP700)
   {HP700, 	"HP"}
#elif (PRO_MACHINE == IBM_RT) || (PRO_MACHINE == IBM_RIOS)
   {SGI_R4K, 	"IBM"}
#elif (PRO_MACHINE == SUN3) || (PRO_MACHINE == SUN4)
   {SUN4, 	"SUN"}
#elif (PRO_MACHINE == ALPHA_UNIX)
   {ALPHA_UNIX,	"ALPHA"}
#elif (PRO_MACHINE == HITACHI)
   {HITACHI, 	"HITACHI"}
#elif (PRO_MACHINE == NEC_MIPS)
   {NEC_MIPS, 	"NEC_MIPS"}
#elif (PRO_MACHINE == LINUX)
   {LINUX, 	"LINUX"}
#elif (PRO_MACHINE == X86E_LINUX64)
   {X86E_LINUX64, 	"X86E_LINUX64"}
#elif (PRO_MACHINE == APPLE_OSX)
   {APPLE_OSX, 	"APPLE_OSX"}
#else
   {0, NULL_CHAR}
#endif
};

#endif

#if OPER_SYS == WINDOWS_32

static const char *pro_get_win32_os_name(const OSVERSIONINFOEXA *version_info)
{
	int i, n;

	n = sizeof(win32_os_names) / sizeof(win32_os_names[0]) - 1;
	for (i = 0; i < n; i++)
	{
		const Win32OSName *p = win32_os_names + i;

		if (p->dwPlatformId == 0)
			return p->name;
		if (p->dwPlatformId == version_info->dwPlatformId)
		{
			if (p->dwMajorVersion == 0)
				return p->name;
			if (p->dwMajorVersion == version_info->dwMajorVersion &&
			    p->dwMinorVersion == version_info->dwMinorVersion)
			{
				if (p->isWorkstation == -1)
					return p->name;
				if (toBOOL(p->isWorkstation) ==
						(version_info->wProductType == VER_NT_WORKSTATION))
					return p->name;
			}
		}
	}

	return "Unknown";
}

#endif /* OPER_SYS == WINDOWS_32 */

ProSystemInfo *pro_get_system_info()
{
  if (!system_info_initialized)
  {
    initialize_system_info();

    system_info_initialized = TRUE;
  }

  return &system_info;
}

static void initialize_system_info()
{



#if OPER_SYS == UNIX_OS


  struct utsname uts;

  /* Initialize the system info structure.
   */
  ZERO_OUT_STRUCT(system_info);

  uname(&uts);

  strncpy(system_info.os_name, uts.sysname, K_PATH_SIZE);
  system_info.os_name[K_PATH_SIZE - 1] = 0;
  strncpy(system_info.os_release, uts.release, K_PATH_SIZE);
  system_info.os_release[K_PATH_SIZE - 1] = 0;
  strncpy(system_info.os_version, uts.version, K_PATH_SIZE);
  system_info.os_version[K_PATH_SIZE - 1] = 0;
  strncpy(system_info.machine_type, uts.machine, K_PATH_SIZE);
  system_info.machine_type[K_PATH_SIZE - 1] = 0;

  strncpy(system_info.machine_name,
          unix_machine_name[0].pro_machine_name, K_PATH_SIZE);

#endif /* OPER_SYS == UNIX_OS */

#if OPER_SYS == WINDOWS_32

  OSVERSIONINFOEXA win32_version_info;
  SYSTEM_INFO win32_system_info;
  int ii, num;
  HKEY hkey ,hkey1;
  DWORD dwprocessor_count,dwSize = 1024,dwCpuIDSize = 1024;
  DWORD dwClockSpeedSize,type ;
  char     buffer[1024]="";
  char     cpu_identifier[1024];
  char     vendor[1024];
  DWORD    clock_speed,vendor_size=1024;
  FILETIME	ft;
  int i,done=0,found_one_processor=0;
  const char CPU_INFO_PATH[50] ="hardware\\description\\system\\centralprocessor";
  long reg_query_status = ERROR_INVALID_ACCESS;

  /* Initialize the system info structure.
   */
  ZERO_OUT_STRUCT(system_info);

  win32_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
  uGetVersionEx((OSVERSIONINFOA *)&win32_version_info);

  strcpy(system_info.os_name, pro_get_win32_os_name(&win32_version_info));

  btk_sprintf(system_info.os_release, "%1d.%1d",
                                  (int)win32_version_info.dwMajorVersion,
                                  (int)win32_version_info.dwMinorVersion);

  /* the LOWORD of dwBuildNumber has version #, on NT is set to zero, but
     not on Windows 95 - fixes bug 536350

     On Windows 10 use the CurrentBuild from the registry as the build number
  */
  if (!(num = pro_is_win10_running ()))
  {
    num = (int)LOWORD(win32_version_info.dwBuildNumber);
  }
  else if (nt_get_registry_string (
               HKEY_LOCAL_MACHINE,
               "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
               "DisplayVersion", sizeof (system_info.os_version),
               system_info.os_version))
  {
    num = 0;
  }
  if (num != 0)
  {
    btk_sprintf(system_info.os_version, "%1d", num);
  }

  GetSystemInfo(&win32_system_info);

  num = sizeof(win32_machine_names) / sizeof(win32_machine_names[0]) - 1;
  ii = 0;
  while ((ii < num) &&
         (win32_machine_names[ii].id != win32_system_info.dwProcessorType))
  {
    ii++;
  }

#if PRO_MACHINE == I486_NT || PRO_MACHINE == IA64_NT || PRO_MACHINE == X86E_WIN64


    strcpy(system_info.machine_type,"");
    reg_query_status = uRegOpenKeyEx(HKEY_LOCAL_MACHINE,CPU_INFO_PATH,0,
		                KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hkey);

    if (reg_query_status == ERROR_SUCCESS)
    {
       reg_query_status = uRegQueryInfoKey(hkey,NULL, 0, NULL, &dwprocessor_count,
					  NULL, NULL, NULL, NULL, NULL, NULL, NULL);

         /*
	  * The following code is capable of printing information on multiple
	  * processors. However currently information on only one processor is
	  * desired, so the variable found_one_processor was introduced.
	  * Remove the variable to enable multi-processor information printout.
	  */

       for (i=0;i < (int)dwprocessor_count && found_one_processor == 0; i++ )
       {
	  clock_speed  = 0;
	  dwSize       = 1024;vendor_size  = 1024;
	  dwCpuIDSize  = 1024;dwClockSpeedSize=1024;

	  reg_query_status = uRegEnumKeyEx(hkey, i, buffer, &dwSize, NULL,
					  NULL, NULL, &ft);

	  reg_query_status = uRegOpenKeyEx(hkey,buffer, 0,
			                   KEY_QUERY_VALUE , &hkey1);

	  if ( reg_query_status == ERROR_SUCCESS )
	  {

	      reg_query_status = uRegQueryValueEx(hkey1, "identifier", 0, NULL,
						 (LPBYTE)cpu_identifier, &dwCpuIDSize);

	      if ( reg_query_status == ERROR_SUCCESS )
		 btk_sprintf(system_info.machine_type + strlen(system_info.machine_type),
                             " %s ", cpu_identifier);

	      reg_query_status = uRegQueryValueEx(hkey1, "vendoridentifier", 0,NULL,
						 (LPBYTE)vendor, &vendor_size);

	      if ( reg_query_status == ERROR_SUCCESS )
		 btk_sprintf(system_info.machine_type + strlen(system_info.machine_type),
                             " %s ", vendor);

	      reg_query_status = uRegQueryValueEx(hkey1, "~MHz", 0,&type,
						 (LPBYTE)&clock_speed, &dwClockSpeedSize);

	      if ( reg_query_status == ERROR_SUCCESS )
		 btk_sprintf(system_info.machine_type + strlen(system_info.machine_type),
                            " %d MHz", clock_speed);

	      found_one_processor = 1;
	  }

	  RegCloseKey(hkey1);
       }
    }
    else
      strcpy(system_info.machine_type, win32_machine_names[ii].string);

	RegCloseKey(hkey);

#else
   strcpy(system_info.machine_type, win32_machine_names[ii].string);
#endif /* ifndef PRO_MACHINE == I486_NT */

#endif /* OPER_SYS == WINDOWS_32 */
}

#if OPER_SYS == WINDOWS_32
/******************************************************************************

    Function:   pro_is_vista_running()

    Purpose:    To provide a function that returns true if Windows Vista is the
                operating system currently running.

    Input:      none

    Output:	TRUE (1) or FALSE (0)
******************************************************************************/
int pro_is_vista_running (void)
{
    static int  ret_val = -1;

    if (ret_val == -1)
    {
        ret_val = (int) uIsWindowsVistaOrGreater();
    }

    return (ret_val);
}

/******************************************************************************

    Function:   pro_is_win7_running()

    Purpose:    To provide a function that returns true if Windows 7 or higher
                is the operating system currently running.

    Input:      none

    Output:	TRUE (1) or FALSE (0)
******************************************************************************/
int pro_is_win7_running (void)
{
    static int  ret_val = -1;

    if (ret_val == -1)
    {
        ret_val = (int) uIsWindows7OrGreater();
    }

    return (ret_val);
}

/******************************************************************************

    Function:   pro_is_win10_running()

    Purpose:    To provide a function that returns the build if Windows 10
                or higher is the operating system currently running.

    Input:      none

    Output:	The Windows 10 build or FALSE (0)
******************************************************************************/
int pro_is_win10_running (void)
{
    static int  ret_val = -1;
    static char build [16];

    if (ret_val == -1)
    {
        if (!(uIsWindows10OrGreater()))
        {
            ret_val = FALSE;
        }
        else if (!(nt_get_registry_string (
                       HKEY_LOCAL_MACHINE,
                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                       "CurrentBuild", sizeof (build), build)) ||
                 (ret_val = atoi (build)) < WIN10_1507)
        {
            /*
            ** Use the RTM version 1507 as the earliest build of Windows 10
            */

            ret_val = WIN10_1507;
        }
    }

    return (ret_val);
}

/******************************************************************************

    Function:   pro_is_win11_running()

    Purpose:    To provide a function that returns the release if Windows 11
                or higher is the operating system currently running.

    Input:      none

    Output:	The Windows 11 release or FALSE (0)
******************************************************************************/
int pro_is_win11_running (void)
{
    static int  ret_val = -1;

    if (ret_val == -1)
    {
        if (!(uIsWindows11OrGreater()))
        {
            ret_val = FALSE;
        }
        else if ((ret_val = pro_is_win10_running()) < WIN11_21H2)
        {
            /*
            ** Use the RTM version 21H2 as the earliest build of Windows 11
            */

            ret_val = WIN11_21H2;
        }
    }

    return (ret_val);
}

#endif  /* OPER_SYS == WINDOWS_32 */
