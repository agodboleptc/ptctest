#include <ptc_win32.h>
#include <btkcio.h>
#include <btkcprocess.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include <ctsyscall_proto.h>
#include	"hardware.h"
#include	"sysstdio.h"
#include	"sysstdlib.h"
#include        <pro_memory.h>
#include	"systypes.h"
#include	"sysvarargs.h"
#include	"syssignal.h"
#include	"sysexecmd.h"
#include	"errors.h"
#include	"pro_string.h"
#include "debug.h"
#include <btkscale31.h>

#if OPER_SYS == UNIX
#include	<sys/wait.h>
#include	<errno.h>
#endif

#if OPER_SYS == WINDOWS_32
#include	<process.h>
#include	<io.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include <const.h>
#include <ctwcfun_proto.h>
#include <sysstdio.h>
#include <ct_win_syscall_proto.h>
#endif


#define PRO_COMMAND_LEN_MAX 2048
/* maximum length of command line constructed by pro_start_proc_new
   for any OS.

   This is only the length of internal character buffers, no claim
   is made that any shell can actually execute commands this long.

   Previous values (also no claim about validity) were 512 for
   pro_system_call and 753 for UNIX pro_exec_command_
*/
#define PRO_COMMAND_ARG_NO_MAX 30
/* maximum number of command line arguments - no reason to be exactly
   this, increase if necessary */
#define PRO_SHELL_PATH_LEN_MAX 256
#define PRO_UNIX_SYSTEM_CMD_STR_LEN (PRO_COMMAND_LEN_MAX + 12 + 1)





/*********************************************************************\
 01-JAN-97 H-01-21  mvb $$1 Created.
 29-JAN-97 H-01-25+ mvb $$2 Changed pro_start_proc_new to release hold
                            on SIGUSR1 for SGI and added declaration
                            for IBM and Apollo machines.
 01-MAY-97 H-01-31+ mvb $$3 Added two new functions pro_start_process
			    and pro_exec_cmd.
 09-OCT-97 H-03-26  JZ  $$4 Deleted obsolete SGI4D and _SYSTYPE_SYSV macros.
 23-MAR-98 H-03-42  prf $$5 Added pro_unix_system ()
22-APR-98 I-01-04  prf    $$6 Fixed compilation errors on RS 6000
27-JUN-98 I-01-16  prf    $$7 play_batch_file for WINDOWS 95
16-Sep-98 I-01-19  dmp    $$8 Make pro_unix_system use /bin/sh
30-Sep-98 I-01-21  ZRL    $$9 Added pro_check_space_in_command()
27-Oct-98 I-01-25  DB    $$10 Fixed call_script for Win9x
10-May-99 I-03-69  Rax   $$11 replace fork for vfork
10-May-99 I-03-69  Rax   $$12 minor compilation fixes
08-Jan-01 J-01-26  Prf   $$13 close fds [3:_SC_OPEN_MAX] before calling execl
25-Jan-01 J-01-26  Prf   $$14 commented out signal (SIGCHLD, SIG_IGN) on HP8K
21-Sep-01 J-03-09  jas   $$15 Removed WINDOWS_95 macro
24-Jun-02 J-03-28  TYA   $$16 Give out pro_start_process() to pro_start_
                              process() and pro_start_process_low().Added FALSE
                              arg (hide_dos_win) to pro_start_process_low().
15-Jul-02 J-03-29  TYA   $$17 Used return value in pro_start_process().
08-Nov-02 J-03-37  CHI   $$18 protect quotes in system cmd (SPR 976500)
09-Jun-03 K-01-09  Chris $$19 Used pro_memory.h
21-Oct-03 K-01-18  CHI   $$20 pro_start_process_lower (qv; SPR 1018662)
18-Dec-04 K-03-16  ksi   $$21 Used rlsmem
06-Jul-05 K-03-28  CHI   $$22 fix theoretical memory leaks found by PREFIX
			      (irrelevant in practice, since they only happen
			      in failures) (SPRs 11246{06,13,14})
07-Oct-05 K-03-36  CHI   $$23 undo #14 (SPR 1046022 and dbatchs issue)
 12-Feb-06  L-01-02 PROTO $$24  Automatic prototype creation
 24-Apr-06  L-01-07 TWH   $$25  use windows unicode wrappers
 12-Jul-06  L-01-12 ksi   $$26  Unicode compliant changes
 03-Jun-08  L-03-10 KSV   $$27  Got rid of duplicated functionality
 12-Mar-12  P-20-01 AC    $$28  Updated for Project 13028358
 12-Feb-14  P-20-48 JDP   $$29  Fix SPR 2211388
 17-Jun-20  P-80-10 ravjain $$30 Handle inheritance should be true
                                 if STARTF_USESTDHANDLES used - SPR 10087989
 25-Nov-21  P-90-37 Ahmad   $$31 scrambled literal env vars
 14-Feb-22  Q-10-01 jas     $$32 Fixed clang issues
 09-Jun-22  Q-10-16 MIF     $$33 Added pro_start_process_in 
 09-Mar-26  Q-27-00 PROTO   $$34 Automatic prototype creation
\*********************************************************************/

# if PRO_MACHINE == IBM_RIOS || PRO_MACHINE == APOLLO
#	define WAIT_STATUS union wait
#	define SET_WAIT_STATUS(to,from) to.w_status = from
# else
#	define WAIT_STATUS int
#	define SET_WAIT_STATUS(to,from) to = from
# endif

/*********************************************************************\
 * Function:    pro_unix_system
 *
 * Description: Starts a process in UNIX, always through a shell,
 *              even it's not required, and waits till it finishes
 *              if required.
 *
 * Input:      cmd_string
 *              The command line to be executed.
 *             wait_mode
 *               if TRUE, pass the command and wait else do not.
 *             use_shell
 *               if TRUE, pass the command line through a shell.
 *
 * Return:      -1, if unsuccess;
 *              process return value, if wait_mode == TRUE;
 *              0, if wait_mode == FALSE
 \*********************************************************************/

# if OPER_SYS == UNIX

/* there still no vforking on SGI */
# if PRO_MACHINE == SGI_R4K
# define vfork fork
# endif

int pro_unix_system (
        char *cmd_string, int wait_mode, int use_shell, int *ret_val)
{
  void (*save_SIGCHLD)(int);
  pid_t child_proc;
  int last_errno;
  char shell_cmd[PRO_UNIX_SYSTEM_CMD_STR_LEN];
  /* make a pipe for error messages */
  int checkpipe [2];

  if (use_shell)      /* it's an iffy legacy. 3/17/98:prf */
    {
      char* newcmd;

      /*
       * in case cmd_string contains, e.g.,
       *	foo "bar baz"
       * which would become unusable when sprintf'd below
       */
      MakeSafeQuotes( cmd_string, &newcmd );

      /* set the environment to use sh not csh */
      pro_putenv("SHELL=/bin/sh");
      btk_sprintf( shell_cmd, "%s \"%s\"", "csh -fc", newcmd );

      rlsmem( newcmd );

    }
  else
    {
      btk_sprintf(shell_cmd, "%s", cmd_string);
    }

  child_proc = btk_spawnl(wait_mode ? 0 : P_NOWAIT, "/bin/sh", "/bin/sh",
      "-c", shell_cmd, 0);

  if (child_proc == (pid_t) -1) {
    perror ("ERROR ");
    btk_fprintf (btk_get_stderr(), "ERROR : Cannot launch child process.");
    return (-1);
  }

  if (!wait_mode)
    {
      if (ret_val)
	{
	  *ret_val = 0;
	}
      return (0);
    }
  else
    {
      int status = child_proc;
      WAIT_STATUS wstatus;

      SET_WAIT_STATUS (wstatus, status);

      if (WIFEXITED (wstatus))
        {
	  if (ret_val)
	    {
	      *ret_val = WEXITSTATUS (wstatus);
	    }
	  return (status);
        }
      else
        {
	  return (-1);
        }
    }
}

# endif

/*********************************************************************\
 * Function:    pro_start_proc_new
 *
 * Description: Function is used currently by pro_exec_command_() if
 *              called in WAIT_MODE. Starts a process, through a
 *              shell if required, and waits till it finishes.
 *              Also can be invoked for all platforms for all modes.
 *
 * Input:      cmd_string
 *              The command line to be executed.
 *             wait_mode
 *               if TRUE, pass the command and wait else do not.
 *             use_shell
 *               if TRUE, pass the command line through a shell.
 *             use_temp_dir
 *               if TRUE, start the process in the system "temp" directory
 *               (NT only)
 *
 * Output:      ret_val
 *                if not NULL, contains value returned by spawning of process.
 *
 * Return:      TRUE = success
 *              FALSE = failure
 *
 * Notes:       The following notes apply on Windows platforms:
 *            - the process is run in "detached" mode, to ensure
 *              that it will not bring up an extraneous console window;
 *            - if use_temp_dir is set, the process is started (if
 *              possible) with its working directory set to the system
 *              "temp" directory, so that it can create files if
 *              necessary and so that the directory of the parent
 *              process can be deleted without waiting for the
 *              process to exit.
 \*********************************************************************/

int pro_start_proc_new(
#ifdef PRO_ANSI_VARARGS
                            char *cmd_string, int wait_mode,
                            int use_shell, int use_temp_dir, int *ret_val)
#else
                            cmd_string, wait_mode,
			    use_shell, use_temp_dir, ret_val)
    char *cmd_string;
    int   wait_mode;
    int   use_shell;
    int   use_temp_dir;
    int   *ret_val;
#endif
{
    int status;
	WAIT_STATUS wstatus;

#if OPER_SYS == WINDOWS_32
    char  exec_string[PRO_COMMAND_LEN_MAX + PRO_COMMAND_ARG_NO_MAX];
    char  comspec[PRO_SHELL_PATH_LEN_MAX + 1];
    char *p_comspec;
    char *env_tmp_dir;
    char *working_dir = NULL;
    char  temp_dir_name[K_PATH_SIZE];
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA         siStartInfo;
    int                 background_flag;
    int                 inherit_flag;
    HANDLE              hStdInput = NULL;
    HANDLE              hStdOutput = NULL;
    HANDLE              hStdError = NULL;
    int          	use_console = TRUE;
    
#endif

#if OPER_SYS == UNIX

    dcl_com_start_func();
	status = (pro_unix_system (cmd_string, wait_mode, use_shell, 0) == 0);

    dcl_com_end_func();

	SET_WAIT_STATUS (wstatus, status);

	if (wait_mode != FALSE && ret_val != NULL && WIFEXITED(wstatus))
	{
		*ret_val = WEXITSTATUS(wstatus); /* it's erroneous, but I left it */
	}

#endif /* OPER_SYS == UNIX */


#if OPER_SYS == WINDOWS_32
    dcl_com_start_func();

    if (use_shell)
    {
        /* The command has to be interpreted by the shell. The env variable
           COMSPEC is supposed to give the shell executable path name */
        p_comspec = BTK_GETENV_31_S("COMSPEC");
        if (p_comspec != NULL)
        {
            strcpy(comspec, p_comspec); /* local copy to be marginally safer */
            btk_sprintf(exec_string, "%s /c %s", comspec, cmd_string);
        }
        else
        {
            return (FALSE);     /* abort */
        }
	if (wait_mode)
	{
           background_flag = 0;
	}
	else
	{
           background_flag = DETACHED_PROCESS;
	}
        inherit_flag = FALSE;
        if (!AllocConsole())
	{
            use_console = FALSE;
	}
        hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
        hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        hStdError  = GetStdHandle(STD_ERROR_HANDLE);
	if ((hStdInput == INVALID_HANDLE_VALUE)  ||
            (hStdOutput == INVALID_HANDLE_VALUE) ||
            (hStdError  == INVALID_HANDLE_VALUE))
	{
            return (FALSE);
	}
    }

    else
    {
        background_flag = DETACHED_PROCESS;
        inherit_flag = FALSE;
        strcpy(exec_string, cmd_string);
    }

    if (use_temp_dir)
    {
        /* set working directory for new process to be system temp dir */

        if ((env_tmp_dir = BTK_GETENV_31_S("TMP")) != NULL)
        {
            /* for safety, copy the dir name locally */
            strcpy(temp_dir_name, env_tmp_dir);
            working_dir = temp_dir_name;
        }
    }

    siStartInfo.cb          = sizeof(STARTUPINFOA);
    siStartInfo.lpReserved  = NULL;
    siStartInfo.lpReserved2 = NULL;
    siStartInfo.lpTitle     = NULL;
    siStartInfo.cbReserved2 = 0;
    siStartInfo.lpDesktop   = NULL;
    siStartInfo.dwFlags     = 0;
    if (use_shell)
    {
        siStartInfo.dwFlags    = STARTF_USESTDHANDLES;
		inherit_flag = TRUE; /* must be true if STARTF_USESTDHANDLES used */
        siStartInfo.hStdInput  = hStdInput;
        siStartInfo.hStdOutput = hStdOutput;
        siStartInfo.hStdError  = hStdError;
    }

    status = uCreateProcess (NULL, /* Module to execute */
                            exec_string, /* Command line */
                            NULL, /* Process security attrs */
                            NULL, /* Thread security attrs */
                            inherit_flag, /* Inherit handles flag */
                            background_flag, /* Creation flags */
                            NULL, /* Environment block */
                            working_dir, /* directory in which to start */
                            &siStartInfo, /* Address of STARTUPINFO */
                            &piProcInfo); /* Address of PROCESS_INFORMATION */

    if (ret_val != NULL)
    {
        *ret_val = status;
    }

    /* iconize console window so it doesn't annoy users */
    if ((use_shell) && (use_console) && (wait_mode))
    {
        EnumWindows((WNDENUMPROC) console_hide, 0);
    }

    if (wait_mode)
    {
        WaitForSingleObject(piProcInfo.hProcess,INFINITE);
        if (ret_val != NULL)
	{
            GetExitCodeProcess(piProcInfo.hProcess,ret_val);
	}
    }

    /* close process and thread handles to avoid resource leak */

    if (status)
    {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }

    if (use_shell)
    {
	if (use_console)
	{
	    if (wait_mode)
	    {
	    	SetStdHandle(STD_INPUT_HANDLE,(HANDLE)NULL);
	    	SetStdHandle(STD_OUTPUT_HANDLE,(HANDLE)NULL);
	    	SetStdHandle(STD_ERROR_HANDLE,(HANDLE)NULL);
	    }
            FreeConsole();
	}
    }

    dcl_com_end_func();
#endif /* WINDOWS_32 */

    return(status);
}

/*********************************************************************/
int  pro_exec_command_new (
/*********************************************************************/
#ifdef PRO_ANSI_VARARGS
			int wait_mode, char *cmd_fmt, ... )
#else
			wait_mode, cmd_fmt, va_alist )
 int    wait_mode;
 char  *cmd_fmt;  /* printf() format */
 va_dcl		  /* optional printf() args */
#endif
{
 int      status = 0;
 char     cmd_string[512];
 va_list  ap;
 int	  spawn_status = 0;
 int	  use_shell = TRUE;


 if ( cmd_fmt != (char *)NULL )
   {
    pro_va_start(ap, cmd_fmt);

    if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
       {
	  dbg_crash_msg ("pro_exec_command_new", "bad format %s", cmd_fmt);
	  return (-1);
       }

    va_end( ap );
   }

 if (wait_mode)
 {
     use_shell = FALSE;
 }

    status = pro_start_proc_new(cmd_string, wait_mode, TRUE, FALSE, &spawn_status);
    return(status);
}

/*********************************************************************\
 * Function:    pro_start_process
 *
 * Description: Starts a process, through a
 *              shell if required, and waits till it finishes.
 *              Also can be invoked for all platforms for all modes.
 *
 * Input:      cmd_string
 *              The command line to be executed.
 *             wait_mode
 *               if TRUE, pass the command and wait else do not.
 *             use_shell
 *               if TRUE, pass the command line through a shell.
 *             use_temp_dir
 *               if TRUE, start the process in the system "temp" directory
 *               (NT only)
 *
 * Output:      ret_val
 *                if not NULL, contains value returned by spawning of process.
 *
 * Return:      TRUE = success
 *              FALSE = failure
 *
 * Notes:       The following notes apply on Windows platforms:
 *            - the process is run in "detached" mode, to ensure
 *              that it will not bring up an extraneous console window;
 *            - if use_temp_dir is set, the process is started (if
 *              possible) with its working directory set to the system
 *              "temp" directory, so that it can create files if
 *              necessary and so that the directory of the parent
 *              process can be deleted without waiting for the
 *              process to exit.
 \*********************************************************************/

int pro_start_process(  
                      char *cmd_string, int wait_mode, int use_shell,
                      int show_window, int use_temp_dir, int *ret_val)
{
    return(pro_start_process_low(cmd_string, wait_mode, use_shell,show_window,
                                 use_temp_dir, NULL , ret_val, FALSE)); 
}

int pro_start_process_in(
                         char* cmd_string, int wait_mode, int use_shell,
                         int show_window, const char* temp_dir, int* ret_val)
{
    return(pro_start_process_low(cmd_string, wait_mode, use_shell, show_window,
                                 TRUE, temp_dir, ret_val, FALSE));
}



int pro_start_process_low(
    char* cmd_string, int wait_mode, int use_shell,
    int show_window, int use_temp_dir, const char* temp_dir, 
    int* ret_val, int hide_dos_win)
{
    int status;
    WAIT_STATUS wstatus;

#if OPER_SYS == WINDOWS_32
    char  *exec_string=NULL;
    char  comspec[PRO_SHELL_PATH_LEN_MAX + 1];
    char *p_comspec=NULL;
    char *env_tmp_dir;
    char *working_dir = NULL;
    char  temp_dir_name[K_PATH_SIZE];
    char *newcmd = NULL;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA         siStartInfo;
    int                 background_flag;
    int                 inherit_flag;
    HANDLE              hStdInput = NULL;
    HANDLE              hStdOutput = NULL;
    HANDLE              hStdError = NULL;
    int          	use_console = TRUE;
    
#endif

#if OPER_SYS == UNIX

    dcl_com_start_func();
    status = (pro_unix_system (cmd_string, wait_mode, use_shell, 0) == 0);
    dcl_com_end_func();

    SET_WAIT_STATUS (wstatus, status);

    if (wait_mode != FALSE && ret_val != NULL && WIFEXITED(wstatus))
    {
        *ret_val = WEXITSTATUS(wstatus); /* It's erroneous, but I left it */
    }

#endif /* OPER_SYS == UNIX */


#if OPER_SYS == WINDOWS_32
    /* handle space in the directory */
    pro_check_space_in_command(cmd_string, &newcmd);

    dcl_com_start_func();

    if (use_shell)
    {
        /* The command has to be interpreted by the shell. The env variable
        COMSPEC is supposed to give the shell executable path name */
        p_comspec = BTK_GETENV_31_S("COMSPEC");
        exec_string=(char *)getmem(sizeof(char)*(strlen(p_comspec)+strlen(newcmd)+30));
        if (p_comspec != NULL)
        {
            strcpy(comspec, p_comspec); /* local copy to be marginally safer */
            btk_sprintf(exec_string, "%s /c %s", comspec, newcmd);
        }
        else
        {
            rlsmem( newcmd );	/* silence PREFIX */
            rlsmem(exec_string);
            return (FALSE);     /* abort */
        }
        background_flag = 0;
        inherit_flag = FALSE;
        if (!AllocConsole())
        {
            use_console = FALSE;
        }
        hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
        hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        hStdError  = GetStdHandle(STD_ERROR_HANDLE);
        if ((hStdInput == INVALID_HANDLE_VALUE)  ||
            (hStdOutput == INVALID_HANDLE_VALUE) ||
            (hStdError  == INVALID_HANDLE_VALUE))
        {
            rlsmem( newcmd );	/* silence PREFIX */
            rlsmem(exec_string);
            return (FALSE);
        }
    }

    else
    {
        exec_string=(char *)getmem(sizeof(char)*(strlen(newcmd)+1));
        background_flag = 0;
        inherit_flag = FALSE;
        strcpy(exec_string, newcmd);
    }

    if (use_temp_dir)
    {
        /* set working directory for new process to be system temp dir */
        if (temp_dir)
        {
            strcpy(temp_dir_name, temp_dir);
            working_dir = temp_dir_name;
        }
        else
        if ((env_tmp_dir = BTK_GETENV_31_S("TMP")) != NULL)
        {
            /* for safety, copy the dir name locally */
            strcpy(temp_dir_name, env_tmp_dir);
            working_dir = temp_dir_name;
        }
    }

    siStartInfo.cb          = sizeof(STARTUPINFOA);
    siStartInfo.lpReserved  = NULL;
    siStartInfo.lpReserved2 = NULL;
    siStartInfo.lpTitle     = NULL;
    siStartInfo.cbReserved2 = 0;
    siStartInfo.lpDesktop   = NULL;
    siStartInfo.dwFlags     = 0;
    if (hide_dos_win)
    {
        siStartInfo.wShowWindow = SW_HIDE;
        siStartInfo.dwFlags     = STARTF_USESHOWWINDOW;
    }
    if (use_shell)
    {
        siStartInfo.dwFlags    = STARTF_USESTDHANDLES;
		inherit_flag = TRUE; /* must be true if STARTF_USESTDHANDLES used */
        siStartInfo.hStdInput  = hStdInput;
        siStartInfo.hStdOutput = hStdOutput;
        siStartInfo.hStdError  = hStdError;
    }

    status = uCreateProcess (NULL, /* Module to execute */
        exec_string, /* Command line */
        NULL, /* Process security attrs */
        NULL, /* Thread security attrs */
        inherit_flag, /* Inherit handles flag */
        background_flag, /* Creation flags */
        NULL, /* Environment block */
        working_dir, /* directory in which to start */
        &siStartInfo, /* Address of STARTUPINFO */
        &piProcInfo); /* Address of PROCESS_INFORMATION */

    if (ret_val != NULL)
    {
        *ret_val = status;
    }

    /* iconize console window so it doesn't annoy users */
    if ((use_shell) && (use_console) && (!show_window))
    {
        EnumWindows((WNDENUMPROC) console_hide, 0);
    }

    if (wait_mode)
    {
        WaitForSingleObject(piProcInfo.hProcess,INFINITE);
        if (ret_val != NULL)
        {
            GetExitCodeProcess(piProcInfo.hProcess,ret_val);
        }
    }

    /* close process and thread handles to avoid resource leak */

    if (status)
    {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }

    if (use_shell)
    {
        if (use_console)
        {
            if (wait_mode)
            {
                SetStdHandle(STD_INPUT_HANDLE,(HANDLE)NULL);
                SetStdHandle(STD_OUTPUT_HANDLE,(HANDLE)NULL);
                SetStdHandle(STD_ERROR_HANDLE,(HANDLE)NULL);
            }
            FreeConsole();
        }
    }

    dcl_com_end_func();
    rlsmem(newcmd);
    rlsmem(exec_string);
#endif /* WINDOWS_32 */

    return(status);
}

/*
 * pro_start_process_low() with added arg to allow separation of executable
 * from args. Needed for combination of mistakes in NT:
 *  - CreateProcess processes "" creatively (that's the polite word; see
 *    discussion of SPR 976500 in exec_cmd.c) BUT
 *  - solution to 976500 causes a DOS window to appear on top of all other
 *    windows, which is objectionable if this xtop is being run by dbatch and
 *    another xtop is running on the same system interactively (SPR 973148).
 * Note that pro_start_process and pro_exec_cmd really ought to be merged into
 * one function that separates exe from args, suppresses xs window, and maybe
 * fixes you a drink while you wait. \Not/ likely in the available time.
 * NOTES RE INPUTS:
 *  - traditional call through pro_start_process{,_low}() is with cmd=NULL,
 *    args=<whole cmd>
 *  - if called directly, cmd is just the executable and args is the modifiers
 *    (command-line arguments); in this case, use_shell must be FALSE -- the
 *    point of using this function directly is to specify the executable to be
 *    directly run by CreateProcess(), which would be overridden by use_shell.
 *  - cmd is NOT QUOTED! (Otherwise, CreateProcess() won't work.) This
 *    function will add quotes as might be necessary.
 * J-03 NOTE: this was designed to be called by pro_start_process_low(), and
 * still has traces of accommodating this (to avoid duplicating code); but a
 * fix for 1018662 that didn't break 973148 was needed quickly enough that
 * this was made a separate function.
 */
int pro_start_process_lower( char* cmd, char *args, int wait_mode,
			     int use_shell, int show_window, int use_temp_dir,
			     int hide_dos_win, int *ret_val )
{
  int status;
  WAIT_STATUS wstatus;

#if OPER_SYS == WINDOWS_32
  char  exec_string[PRO_COMMAND_LEN_MAX + PRO_COMMAND_ARG_NO_MAX];
  char  exec_string_args[PRO_COMMAND_LEN_MAX + PRO_COMMAND_ARG_NO_MAX];
  char  comspec[PRO_SHELL_PATH_LEN_MAX + 1];
  char *p_comspec;
  char *env_tmp_dir;
  char *working_dir = NULL;
  char  temp_dir_name[K_PATH_SIZE];
  char *newcmd = NULL;
  char* module = NULL;
  char* cmdline = NULL;
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFOA         siStartInfo;
  int                 background_flag;
  int                 inherit_flag;
  HANDLE              hStdInput = NULL;
  HANDLE              hStdOutput = NULL;
  HANDLE              hStdError = NULL;
  int          	use_console = TRUE;
  
#endif

#if OPER_SYS == UNIX
  char  cmd_string[PRO_COMMAND_LEN_MAX + PRO_COMMAND_ARG_NO_MAX];

  if ( cmd != NULL  &&  args != NULL )
    /*
     * quote "cmd" in case it contains spaces. ('"' are PTC standard;
     * pro_unix_system() will emend if necessary.) I suppose we could use
     * pro_check_spaces_in_command(), but why bother when we already know what
     * part is the executable.
     */
    btk_sprintf( cmd_string, "\"%s\" %s", cmd, args );
  else	/* for J-03, all other cases are illegal (see "J-03 NOTE" above) */
    {
      if ( ret_val != NULL )
	*ret_val = PRO_TK_BAD_INPUTS;
      return( FALSE );
    }

  dcl_com_start_func();
  status = (  pro_unix_system( cmd_string, wait_mode, use_shell, 0 ) == 0  );
  dcl_com_end_func();

  SET_WAIT_STATUS (wstatus, status);

  if (wait_mode != FALSE && ret_val != NULL && WIFEXITED(wstatus))
    {
      *ret_val = WEXITSTATUS(wstatus); /* It's erroneous, but I left it */
    }
#endif /* OPER_SYS == UNIX */

#if OPER_SYS == WINDOWS_32
  if ( use_shell == TRUE  &&  cmd != NULL )
    {
      /* see function description */
      if ( ret_val != NULL )
	*ret_val = PRO_TK_BAD_INPUTS;
      return( FALSE );
    }

  /* handle space in the directory */
  /*
   * actually, in the path to the executable.
   * handling space \should/ be obsolete -- caller should be responsible for
   * quoting path-to-executable containing spaces -- but if call to
   * pro_check_space_in_cmd is removed, still point "module" or "cmdline"
   * to "exec_string" and point "newcmd" appropriately; otherwise code below
   * (which processes into exec_string) won't work. Or cleanup "if (use_shell)"
   * (and its "else") as well.
   * 
   */
  if ( cmd != NULL )
    {
      /*
       * C programs expect argv[0] to be their name, and argv[1,...] to be
       * the modifiers; CreateProcess() doesn't do this so we have to.
       *    HOWEVER, cmd fed to CreateProcess() must \not/ be in quotes -- but
       * it may contain spaces, which must be protected in cmdline args. (As
       * in UNIX case above, could use pro_check_space_in_command() but why.)
       */
      btk_sprintf( exec_string_args, "\"%s\" %s", cmd, args );
      cmdline = exec_string_args;
      /*
       * this simplifies "else" of "if (use_shell)" and allows one
       * CreateProcess() call to handle all variations of {cmd,arg}{==,!=}NULL
       */
      module = exec_string;
      newcmd = cmd;
    }
  else
    {
      cmdline = exec_string;
      pro_check_space_in_command( args, &newcmd );
    }

  dcl_com_start_func();

  if (use_shell)
    {
      /* The command has to be interpreted by the shell. The env variable
           COMSPEC is supposed to give the shell executable path name */
      p_comspec = BTK_GETENV_31_S("COMSPEC");
      if (p_comspec != NULL)
	{
	  strcpy(comspec, p_comspec); /* local copy to be marginally safer */
	      btk_sprintf(exec_string, "%s /c %s", comspec, newcmd);
	}
      else
	{
	  if ( newcmd != cmd )	/* newcmd was allocated */
	    rlsmem( newcmd );	/* silence PREFIX */
	  return (FALSE);     /* abort */
	}
      background_flag = 0;
      inherit_flag = FALSE;
      if ( !AllocConsole() )
        {
	  use_console = FALSE;
        }
      hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
      hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      hStdError  = GetStdHandle(STD_ERROR_HANDLE);
      if ((hStdInput == INVALID_HANDLE_VALUE)  ||
	  (hStdOutput == INVALID_HANDLE_VALUE) ||
	  (hStdError  == INVALID_HANDLE_VALUE))
	{
	  if ( newcmd != cmd )	/* newcmd was allocated */
	    rlsmem( newcmd );	/* silence PREFIX */
	  return (FALSE);
	}
    }

  else
    {
      /*
       * The MSDN Library lies like a rug; it claims CreateProcess assumes
       * .exe if there is no extension, but regtest jlinktest10 fails because
       * pcltool_command() calls this with "...\pcltool" instead of
       * "...\pcltool.exe". So we'll add .exe if it's not already there --
       * unless there's some other .xxx suffix, in which case we'll hope the
       * caller knows what he's doing.
       */

      int len = strlen( newcmd );
      char* sp = newcmd + len - 4; /* length of ".xxx" suffix, if it exists */

      if (  strcmp( sp, ".exe" )  &&  sp[0] != '.'  )
	btk_sprintf( exec_string, "%s.exe", newcmd );
      else
        strcpy( exec_string, newcmd );
      background_flag = 0;
      inherit_flag = FALSE;
    }

  if (use_temp_dir)
    {
      /* set working directory for new process to be system temp dir */

      if ((env_tmp_dir = BTK_GETENV_31_S("TMP")) != NULL)
	{
	  /* for safety, copy the dir name locally */
	  strcpy(temp_dir_name, env_tmp_dir);
	  working_dir = temp_dir_name;
	}
    }

  siStartInfo.cb          = sizeof(STARTUPINFOA);
  siStartInfo.lpReserved  = NULL;
  siStartInfo.lpReserved2 = NULL;
  siStartInfo.lpTitle     = NULL;
  siStartInfo.cbReserved2 = 0;
  siStartInfo.lpDesktop   = NULL;
  siStartInfo.dwFlags     = 0;
  if (hide_dos_win)
    {
      siStartInfo.wShowWindow = SW_HIDE;
      siStartInfo.dwFlags     = STARTF_USESHOWWINDOW;
    }
  if (use_shell)
    {
      siStartInfo.dwFlags    = STARTF_USESTDHANDLES;
	  inherit_flag = TRUE; /* must be true if STARTF_USESTDHANDLES used */
      siStartInfo.hStdInput  = hStdInput;
      siStartInfo.hStdOutput = hStdOutput;
      siStartInfo.hStdError  = hStdError;
    }

  status = uCreateProcess( module,	   /* Module to execute */
			  cmdline,	   /* Command line */
			  NULL,		   /* Process security attrs */
			  NULL,		   /* Thread security attrs */
			  inherit_flag,	   /* Inherit handles flag */
			  background_flag, /* Creation flags */
			  NULL,		   /* Environment block */
			  working_dir,	   /* directory in which to start */
			  &siStartInfo,	   /* Address of STARTUPINFO */
			  &piProcInfo );   /* Address of PROCESS_INFORMATION */
  if (ret_val != NULL)
    {
      if ( status == TRUE )
	*ret_val = status;
      else
	*ret_val = GetLastError();
    }
  /* iconize console window so it doesn't annoy users */
  /* currently hide; doesn't work, and iconize breaks -- CHI 13-Oct-03 */
  if ( (use_shell) && (use_console) && (!show_window) )
    {
      EnumWindows((WNDENUMPROC) console_hide, 0);
    }

  if ( wait_mode )
    {
      WaitForSingleObject(piProcInfo.hProcess,INFINITE);
      if (ret_val != NULL)
	{
	  GetExitCodeProcess(piProcInfo.hProcess,ret_val);
	}
    }

  /* close process and thread handles to avoid resource leak */

  if (status)
    {
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
    }

  if (use_shell)
    {
      if (use_console)
	{
	  if (wait_mode)
	    {
	      SetStdHandle(STD_INPUT_HANDLE,(HANDLE)NULL);
	      SetStdHandle(STD_OUTPUT_HANDLE,(HANDLE)NULL);
	      SetStdHandle(STD_ERROR_HANDLE,(HANDLE)NULL);
	    }
	  FreeConsole();
	}
    }

  dcl_com_end_func();
  if ( newcmd != cmd )
    /* if so, newcmd was created by malloc() in pro_check_space_in_command */
    rlsmem( newcmd );
#endif /* WINDOWS_32 */

  return(status);
}

/*********************************************************************/
int  pro_exec_cmd (
/*********************************************************************/
#ifdef PRO_ANSI_VARARGS
			int wait_mode, int use_shell,
			int show_window, int use_temp_dir, int *ret_val,
			char *cmd_fmt, ... )
#else
			wait_mode, use_shell, show_window, use_temp_dir,
			ret_val, cmd_fmt, va_alist )
 int    wait_mode;
 int    use_shell;
 int    show_window;
 int    use_temp_dir;
 int   *ret_val;
 char  *cmd_fmt;  /* printf() format */
 va_dcl		  /* optional printf() args */
#endif
{
 int      status = 0;
 char     cmd_string[512];
 va_list  ap;


 if ( cmd_fmt != (char *)NULL )
   {
    pro_va_start(ap, cmd_fmt);

    if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
       {
	  dbg_crash_msg ("pro_exec_command_new", "bad format %s", cmd_fmt);
	  return (-1);
       }

    va_end( ap );
   }

 if (wait_mode)
 {
     use_shell = FALSE;
 }
 status = pro_start_process(cmd_string, wait_mode, use_shell, show_window,
			   use_temp_dir, ret_val);
 return(status);
}

#if OPER_SYS == WINDOWS_32


/* attempts to hide the black window */
BOOL CALLBACK console_hide(mywindow, ignore)
HWND mywindow;
LPARAM ignore;
{
    DWORD pid;

    if (GetWindowThreadProcessId(mywindow, &pid) == GetCurrentThreadId())
    {
        (void) ShowWindow(mywindow, SW_HIDE);
	return (FALSE);
    }
    return (TRUE);
}

/***************************************************************************/
int play_batch_file( char *cmd_string )
{
  int RC = -1;
  call_script (cmd_string, &RC);
  return (RC);
}

int call_script( char *cmd_string, int *rc )
{
  char *comspec;
  char cmdline [2048];
  STARTUPINFOA startinfo;
  PROCESS_INFORMATION procinfo;
  BOOL success;
  int ftemp = -1;
  char tempbatch [1024];
  char tempbatchcmd [1024];
  char *newcmd = NULL;
  char *tempdir = NULL;
  int tempidx;
  int inherit_flag = FALSE;

  char envstr [2048];
  char *envblock = NULL;

  /* handle space in the directory */
  pro_check_space_in_command(cmd_string, &newcmd);

  comspec = BTK_GETENV_31_S("COMSPEC");
  if (comspec != NULL)
    btk_sprintf(cmdline, "%s /e:4096 /c %s\n", comspec, newcmd);
  else
    {
      rlsmem( newcmd );	/* silence PREFIX */
      return (FALSE);
    }

  if ((tempdir = BTK_GETENV_31_S ("TMP")) == NULL)
    {
      rlsmem( newcmd );	/* silence PREFIX */
      return (FALSE);
    }
  for (tempidx = 0;;tempidx++)
    {
      btk_sprintf (tempbatch, "%s\\~tempbat%i.bat", tempdir, tempidx);
      if ((ftemp = btk_open (tempbatch, _O_CREAT | _O_EXCL | _O_RDWR | _O_TEXT, _S_IREAD | _S_IWRITE)) != -1)
	{
	  break;
	}
    }

  envblock = uGetEnvironmentStrings ();
  if (envblock != NULL)
    {
      char *envblocktofree = envblock;
      while (envblock[0])
        {
	  if (envblock [0] != '=')
            {
	      btk_sprintf (envstr, "set %s\n", envblock);
	      _write (ftemp, envstr, strlen (envstr));
            }
	  envblock += strlen (envblock) + 1;
        }
      uFreeEnvironmentStrings (envblocktofree);
    }

  _write (ftemp, cmdline, strlen (cmdline));
  _close (ftemp);

  startinfo.cb          = sizeof(STARTUPINFOA);
  startinfo.lpReserved  = NULL;
  startinfo.lpReserved2 = NULL;
  startinfo.lpTitle     = NULL;
  startinfo.cbReserved2 = 0;
  startinfo.lpDesktop   = NULL;
  startinfo.dwFlags    = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  inherit_flag = TRUE; /* must be true if STARTF_USESTDHANDLES used */
  startinfo.wShowWindow = SW_HIDE;
  startinfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
  startinfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  startinfo.hStdError  = GetStdHandle(STD_ERROR_HANDLE);

  btk_sprintf(tempbatchcmd, "%s /e:4096 /c %s", comspec, tempbatch);

  if (uCreateProcess (NULL, tempbatchcmd, NULL, NULL, inherit_flag, DETACHED_PROCESS, NULL, NULL, &startinfo, &procinfo))
    {
      success = TRUE;
      btk_printf ("Process \"%s is called via CreateProcess \"%s\n", cmdline, tempbatchcmd);
      WaitForSingleObject(procinfo.hProcess,INFINITE);
      if (rc != NULL)
        {
	  GetExitCodeProcess(procinfo.hProcess,rc);
        }
      CloseHandle(procinfo.hProcess);
      CloseHandle(procinfo.hThread);
    }
  else
    {
      success = FALSE;
      btk_printf ("Process \"%s\" failed\n", tempbatch);
    }

  if (ftemp != -1)
    {
      btk_remove (tempbatch);
    }

  rlsmem(newcmd);

  return (success);
}

#endif
