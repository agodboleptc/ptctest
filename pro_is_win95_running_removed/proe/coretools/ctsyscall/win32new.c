/*
  06/30/96 G-03-19 swv $$1 Created
  10/16/96 H-01-14 swv $$2 deleted extra ";"
  03/06/97 H-03-04 swv/mvb $$3 Changed run_win95_program to
				call pro_exec_command_new
  05/14/97 H-03-10 mvb $$4 Changed run_win95_program to
				call pro_exec_cmd.
  21/09/01 J-03-09 jas $$5 Removed WINDOWS_95 macro
  13-Aug-04 K-03-08 MRC $$6 compiler warnings promoted to error
  12-Jul-06 L-01-12 ksi $$7 Unicode compliant changes
  08-Dec-08 L-03-23 BI  $$8 Added syswindows.h
  12-Mar-12 P-20-01 AC  $$9 Updated for Project 13028358
  12-Jan-21 P-80-36 DSS $$10 Fixed include file case 
*/

#include <btkcstdio.h>
#include "sysstdio.h"
#include "string.h"
#include "sysstdlib.h"

#ifdef WIN32
#include <ctype.h>
#include <syswindows.h>
#include <winbase.h>
#include <commctrl.h>
#endif

#include "sysvarargs.h"
#include "sysexecmd.h"
#include "pro_string.h"
#include "hardware.h"
#include "errors.h"
#include <const.h>
#include <ctsyscall_proto.h>
#include <ctwcfun_proto.h>
#include <ct_win_syscall_proto.h>
#include <debug.h>

#define COMMAND_LEN_MAX 2048
#define SHELL_PATH_LEN_MAX 256


/*****************************************************************************

    Function:   run_win95_program

    Purpose:    Start a program, and wait for it to finish. Should be used
                as substitute for 'system' in case of spawning a console
                application from window application.

    Input:      char* cmdline : Full command line of process to start.

    Output:     return status of spawned child

    Return:     -1 : if could not start the process or failure
                status  : success

*****************************************************************************/

int run_win95_program (char* cmdline)
{
    int status = 0;

    return (pro_exec_cmd(TRUE, TRUE, FALSE, FALSE, &status, cmdline));
}



/************************************************************/

/*
 For platforms other than WINDOWS 95, pro_system_call_new
 is identical to pro_system_call.

 On WINDOWS_95, pro_system_call_new will call run_win95_program
*/


#if OPER_SYS != WINDOWS_32
int  pro_system_call_new(ret_value, mode, varstr)
int *ret_value;
int mode;
char *varstr;
{
  return(pro_system_call(ret_value, mode, varstr));
}
#else
/* WINDOWS_32 code */
int  pro_system_call_new(
#ifdef PRO_ANSI_VARARGS
                     int *ret_value, int mode, char *cmd_fmt, ... )
#else
                     ret_value, mode, cmd_fmt, ap )
 int   *ret_value, mode;
 char  *cmd_fmt;  /* printf() format */
 va_list  ap;          /* optional printf() args */
#endif
{
 return(pro_system_call(ret_value, mode, cmd_fmt));
}
#endif
