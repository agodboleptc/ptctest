/* ************************************************************************* *\
  win32_sysparams.c -=> This file is used to house functions related to
     checking Windows32 Machine configuration for PTC products.

  Revision History:
  -----------------------------------------------------------
  16-Mar-1996 G-03-10 cfk    $$1 Created file chksysparams.c
  23-Apr-1996 G-03-13 cfk    $$2 Added Graphics Check code CheckGraphicsSetup()
                                 Added binding checks to CheckNetworkSetup();
                                 Added function WhichWindowsOperatingSystem();
  13-May-1996 G-03-13 cfk    $$3 Implemented function CheckForEtherNet();
  16-May-1996 G-03-13 cfk    $$4 Removed function CheckForEtherNet();
  24-May-1996 G-03-15 cfk    $$5 Added define CHKSYSPARMS_TCP_SERVICES for
                                 "Simple TCP/IP Services" and "TCP/IP NetBIOS"
                                 reimplemented IP checking.
  26-Jun-1996 G-03-18 cfk    $$6 + Fixed CheckNetworkSetup.checkhostname.
                                   Needed to make sure we strip the domain name
                                   from the IP hostname.
                                 + changed runtime platform check to compiletime
  24-Jan-97 H-01-25 EEB/MRC  $$7 Allow NT compiled code to run under WIN 95
  24-Aug-98 I-01-17 FBI      $$8 Removed check for SimpTCP;Needed for NTv3.5x.
  27-Sep-99 I-03-17 DB       $$9 Added socket connection check
  14-Jul-00 J-01-14 JPE     $$10 WIN64 Port
  24-AUG-01 J-03-07 JPE     $$11 Win64:  use INVALID_SOCKET
  04-May-04 K-03-01 PMORK/CHI $$12 Convert string handling calls to i18n_xxx()
  12-May-04 K-03-01+CHI     $$13 fix strlwr
  13-May-04 K-03-01 PMORK/CHI $$14 undoing #12,13
  24-Apr-06 L-01-07 TWH $$15 use windows unicode wrappers
  12-Jul-06 L-01-12 ksi $$16 Unicode compliant changes
  22-May-08 L-03-09 KSV $$17 Ground work for SandBox project
  25-Aug-08 L-03-16 KSV $$18 Sandbox for Windows support
  08-Dec-08 L-03-23 BI  $$19 IPv6 code changes
  25-Dec-09 L-05-14 CHI $$20 Unicode compliance update (SPR 1925549)
  12-Mar-12 P-20-01 AC  $$21 Updated for Project 13028358
  04-Mar-14 P-20-49 DEM $$22 Env variable to bypass Lanman service checks
  13-Jul-20 P-80-12 KSV $$23 Removed sandbox leftovers
  29-Nov-20 P-80-31 Ahma $$24 uninitialized variable error fixed
  25-Jul-21 P-90-19 DevOps $$25 Changed return type of some functions to void
  22-Nov-21 P-90-37 Ahmad  $$26 scrambled literal env vars
  20-Dec-23 Q-11-44 TT  $$27 Fixed AddressSanitizer: strcpy-param-overlap: memory ranges
  02-Apr-24 Q-12-06 Rah $$28 Removed code from CheckNetworkSetup to avoid unnecessary firewall popup 
\* ************************************************************************* */
#include <btkcsocket.h>
#include <btkcnet.h>
#include <ptc_win32.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include <btkcstring.h>
#include <hardware.h>
#include <btkscale31.h>

#if OPER_SYS == WINDOWS_32
#include <btkw32incs.h>
#include <w32chksysparms.h>
#include <const.h>
#include <ct_win_syscall_proto.h>
#include <sysstdio.h>
void WIN32_CreateIcon(char *iconName, char *iconPath, char *iconIconPath,
  int run_minimized) {

}

/*
 * Error Functions
 */
static unsigned int W32ChkSysParms_ErrorCode = 0;

BOOL W32ChkSysParm_SetError(int errorCode) {
  if ( errorCode == 0 ) {
    W32ChkSysParms_ErrorCode = 0;
  } else {
    W32ChkSysParms_ErrorCode = W32ChkSysParms_ErrorCode | errorCode;
  }
  return(TRUE);
}

BOOL W32ChkSysParm_IsErrorSet(int errorCode) {
  unsigned int tmp = W32ChkSysParms_ErrorCode;

  tmp = tmp & errorCode;

  if ( tmp == errorCode ) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}


/*
 * Check Functions
 */

BOOL CheckNetworkSetup(void) {
    int ret = TRUE;
    char hostname[32];
    char inet_addr[32];
    char def_gate[32];
    char lana_id[12];
    char* devname = NULL;
    char reg_path_name[128];
    char computername[32];
    char* ppnt = NULL;
    int  tmp_return = 0, i = 0;
    int  compname_len = sizeof computername;
    char* OLD_NETWORK_CHECKS = BTK_GETENV_31_S("OLD_NETWORK_CHECKS");
    char* NO_LANMAN_CHECKS = BTK_GETENV_31_S("NO_LANMAN_CHECKS");
   
    W32ChkSysParm_SetError(0);
    
    /* Check here for hostname */
    if (((tmp_return = gethostname(hostname, sizeof hostname)) != 0) ||
        (uGetComputerName(computername, &compname_len) != TRUE)) {
        W32ChkSysParm_SetError(CHKSYSPARM_HOSTNAME_ERROR);
        ret = FALSE;
    }
    else {
        ppnt = strstr(hostname, ".");
        if (ppnt) *ppnt = '\0';

        if (strcmp(btk_strlwr(hostname), btk_strlwr(computername)) != 0) {
            W32ChkSysParm_SetError(CHKSYSPARM_HOSTNAME_NOMATCH);
            ret = FALSE;
        }
        else {
            for (i = 0; i < strlen(hostname); i++) {
                if ((isalnum(hostname[i]) == 0)
                    && (hostname[i] != '-')
                    && (hostname[i] != '_')
                    ) {
                    W32ChkSysParm_SetError(CHKSYSPARM_HOSTNAME_INVALIDCHAR);
                    ret = FALSE;
                }
            }
        }
    }

    /* Check OS Specific Stuff  WINDOWS95/WINDOWSNT */
    if (!NO_LANMAN_CHECKS && ginst_is_service_running("LanmanWorkstation") != TRUE) {
        W32ChkSysParm_SetError(CHKSYSPARM_WIN32_SERVICES);
        ret = FALSE;
    }

    if (OLD_NETWORK_CHECKS && !NO_LANMAN_CHECKS && (ginst_is_service_running("LanmanServer") != TRUE)) {
        W32ChkSysParm_SetError(CHKSYSPARM_WIN32_SERVICES);
        ret = FALSE;
    }

    if (OLD_NETWORK_CHECKS && (ginst_is_service_running("LmHosts") != TRUE)) {
        W32ChkSysParm_SetError(CHKSYSPARM_TCP_SERVICES);
        ret = FALSE;
    }

    if (OLD_NETWORK_CHECKS && (nt_get_registry_string_value(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Linkage", "Bind",
        inet_addr) == TRUE)) {
        devname = strchr(inet_addr + 1, '\\');
        devname = devname + 1;
        btk_sprintf(reg_path_name, "SYSTEM\\CurrentControlSet\\Services\\"
            "%s\\Parameters\\Tcpip", devname);
        memset(inet_addr, 0, sizeof inet_addr);
        if (nt_get_registry_string_value(HKEY_LOCAL_MACHINE,
            reg_path_name, "DefaultGateway", def_gate) == TRUE) {
            if (def_gate[0] == 0) {
                W32ChkSysParm_SetError(CHKSYSPARM_INVALID_DEFGATE);
                ret = FALSE;
            }
        }
        if (nt_get_registry_string_value(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Services\\NetBIOS\\Linkage",
            "LanaMap", lana_id) == TRUE) {
            if (lana_id[0] != '\x01') {
                W32ChkSysParm_SetError(CHKSYSPARM_INVALID_LANAID);
                ret = FALSE;
            }
        }
    }

 //  removing this code to avoid unnecessary firewall popup.
 //  The code is only for sanity check; not doing any communication.
 //  It is further unnecessary now as all machines are likely to have network stack available.
     return (ret);
}

#endif




