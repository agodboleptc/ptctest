#include <ptc_win32.h>
#include <btkcprocess.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include	"hardware.h"
#include        <pro_memory.h>
#include	"sysstdio.h"
#include	"sysstdlib.h"
#include	"systypes.h"
#include	"sysvarargs.h"
#include	"syssignal.h"
#include	"sysexecmd.h"
#include	"errors.h"
#include	"pro_string.h"
#include        "access.h"
#include        "ent_type.h"
#include "debug.h"
#include <osapic.h>
#include <bindcall.h>
#include <ctfileutil_proto.h>
#include <btkscale31.h>


#if OPER_SYS == VMS_OS
#   ifndef DESCRIPH
#      define DESCRIPH
#      include 	<descrip.h>
#   endif
#   ifndef STSDEFH
#      define STSDEFH
#      include	<stsdef.h>
#   endif
#   ifndef RMSDEFH
#      define RMSDEFH
#      include	<rmsdef.h>
#   endif
#   ifndef SSDEFH
#      define SSDEFH
#      include	<ssdef.h>
#   endif
#endif

#if OPER_SYS == UNIX
#include	<sys/wait.h>
#include	<errno.h>
#endif

#if OPER_SYS == WINDOWS_32
#include	<process.h>
#include <stdio.h>
#include <io.h>
#endif

#ifndef NOT_USE_INTERNAL
#include <btkshellcmd.h>
#endif

#include    <pfa.h>

# if PRO_MACHINE == IBM_RIOS || PRO_MACHINE == APOLLO
#   define WAIT_STATUS union wait
#   define SET_WAIT_STATUS(to,from) to.w_status = from
# else
#   define WAIT_STATUS int
#   define SET_WAIT_STATUS(to,from) to = from
# endif

#define COMMAND_LEN_MAX 2048
/* maximum length of command line constructed by pro_system_call
   for any OS
   or passed to pro_exec_command_line for UNIX.

   This is only the length of internal character buffers, no claim
   is made that any shell can actually execute commands this long.

   Previous values (also no claim about validity) were 512 for pro_system_call
   and 753 for UNIX pro_exec_command_line
*/
#define COMMAND_ARG_NO_MAX 30
/* maximum number of command line arguments - no reason to be exactly this, increase if
   necessary */
#define SHELL_PATH_LEN_MAX 256
#define UNIX_SYSTEM_CMD_STR_LEN (COMMAND_LEN_MAX + 12 + 1)

#include <ctsyscall_proto.h>
#include <ctwcfun_proto.h>
#include <const.h>
#include <sysstdio.h>
#include <exitfuncs.h>
#include <sysexecmd.h>
#include <ct_win_syscall_proto.h>
#include <proprintf.h>

/*********************************************************************\
 16-NOV-93 E-01-27 Entin $$1 Created.
 03-MAY-94 E-03-23 Chris $$2 Added pro_system_call()
 06-MAY-94 E-03-24 Chris $$3 BSD for IBM and apollo
 16-May-94 E-03-27 amidon $$4	Fixed spelling of SS$_NORMAL
 02-JUN-94 E-03-28 Gene   $$5	Check return after formatting.
 07-JUN-94 E-03-28 Gene   $$6	Defined pro_vsprintf ()
 31-Jul-95  G-01-02 mxs	  $$7   increased buffer sizes in pro_system_call
                                and pro_exec_conmmand_, attempt to detect
				buffer overfill, added
				pro_spawn_for_version_check()
 28-Sep-95 G-01-08  PLS   $$8  Fixed detached mode for win32
                                which never worked.
 10-Oct-95  G-01-10 amin  $$9 Fix in pro_exec_command_ for Win 95
22-Jan-96 G-03-01 jmichaud $$10 Include header for dbg_xxx_msg() varargs funcs.
 23-May-96 G-03-17 rsc     $$11 Add pro_exec_background_shell() and
                                pro_start_background_process().
 10-SEP-96 H-01-08 DGMJ    $$12 Unstatic start_background_proc().
 10-DEC-96 H-01-20 mvb     $$13 CreateProcess flags changed in
				start_background_proc
 20-DEC-96 H-01-21 mvb     $$14 pro_start_background_proc changed to create
				console and shut it after launching app
				and also to use shell in Win 95 for batch
				file starting proe
 24-DEC-96 H-01-21 mvb     $$15 pro_exec_command_ changed to call
				pro_start_background_proc if called with
				DETACHED_MODE option for WINDOWS_32
 01-JAN-97 H-01-21 mvb     $$16 start_background_proc changed to set the
				inherit_flag to FALSE and remove some
				debug stuff which had been #if 0ed out.
 08-JAN-97 H-01-22 mvb     $$17 start_background_proc changed for WIN_32
			        so that AllocConsole and CloseConsole
				are used only if current process is non-
				console (Console has not been allocated)
 11-Jul-97 H-03-17 rsc     $$18 On Windows, make sure working dir (from TMP)
                                exists and is writeable.
 09-OCT-97 H-03-26 JZ      $$19 Deleted obsolete SGI4D and _SYSTYPE_SYSV macros.
 30-Oct-97 H-03-28+rsc     $$20 include access.h (for use of ACCESS_WRITE)
 23-Mar-98 H-03-42 prf     $$21 Used pro_unix_system () instead if system ()
22-Apr-98 I-01-04  prf      $$22 Fixed compilation errors on RS 6000
30-Sep-98 I-01-21  ZRL      $$23 Added pro_check_space_in_command()
12-Oct-98 I-01-22  ZRL      $$24 Handle space in pro_exec_command_()
02-Nov-98 I-01-25  ZRL      $$25 Added pro_check_file_is_executable()
08-Dec-98 I-01-27  ZRL/jlc  $$26 Added handle_nt_env_var(), use pfa_access
08-Feb-99 I-01-32  Rax      $$27 added pro_spawn_for_version_check_use_array
30-Nov-99 I-03-23  Rax      $$28 added return operator in pro_..._array
12-Apr-00 J-01-06  HMR      $$29 Use pfa in pro_check_file_is_executable().
20-Mar-01 J-01-31  KSV      $$30 Added pro_exec_background_shell_ex and
                                 start_background_proc_ex.
                   DB            WIN32: _spawnv() -> CreateProcess() in
                                 pro_spawn_for_version_check_use_array()
22-Jul-01 J-03-04  yamdur   $$31 Unstatic and rename pro_exec_command_().
21-Sep-01 J-03-09  jas      $$32 Removed WINDOWS_95 macro
20-May-02 J-03-26  CHI      $$33 Use batch file on NT if needed (SPR 939596)
08-Nov-02 J-03-37  CHI      $$34 protect quotes in system cmd (SPR 976500)
09-Jun-03 K-01-09  Chris    $$35 Used pro_memory.h
11-Aug-03 K-01-12  ACT      $$36 Use pfa_secure_open for temp files
09-Apr-03 K-01-13  CHI      $$37 add'l protection for >2 quotes (SPR 976500)
06-Dec-04 K-03-16  CHI      $$38 fix #23: malloc/free -> getmem/relmem
22-Jan-05 K-03-18  KSV      $$39 pro_exec_command adapt. for unix-style env.vars
06-Jul-05 K-03-28  CHI      $$40 Fix theoretical memory leaks found by PREFIX
				 (irrelevant in practice, since they only
				 happen in failures) (SPRs 112461{7,9})
				 Fix possible deref of NULL (SPR 1124597)
18-Jul-05 K-03-28+ CHI      $$41 Fix fix of SPR 1124597
02-Sep-05 K-03-31  KSV      $$42 Fix of SPR 1124597
12-Feb-06 L-01-02  PROTO    $$43 Automatic prototype creation
24-Apr-06 L-01-07  TWH      $$44 use windows unicode wrappers
12-Jul-06 L-01-12  ksi      $$45 Unicode compliant changes
27-Jul-06 L-01-13+ CHI      $$46 write batch scripts in EUC
27-Jul-06 L-01-15  CHI      $$47 Make sure scripts are unique.
21-Feb-07 L-01-27  ksi      $$48 Used COMMAND_LEN_MAX instead of 512 for
                                 cmd_string
25-Feb-08 L-03-03  KSV      $$49 #39 fixed for non-existing variables
12-Jun-08 L-03-10  KSV      $$50 Got rid of duplicated code
27-Jul-08 L-03-14  KSV      $$51 Sandbox fix
24-Nov-08 L-03-22  CHI      $$52 handle quoted args (for SPR 1591173)
02-Mar-09 L-03-27  AEY	    $$53 Included ctwcfun_proto.h
12-Mar-12 P-20-01  AC      $$54 Updated for Project 13028358
04-Jun-13 P-20-31  RDY      $$55 Modified function pro_exec_command_line()
								to avoid popup windows.
07-Jun-13 P-20-31  AC       $$56 Reverted ##55
25-Jun-13 P-20-33  RDY      $$57 Modified function pro_exec_command_line()
								 changed flag PS_F_CONSOLE_PROCESS to
								 PS_F_USE_SHELL.
12-Jul-13 P-20-33  RDY      $$58 Reverted ##57
07-Nov-13 P-20-42  jas      $$59 Added WAIT_AND_PROCESS_EVENTS_MODE
09-Feb-16 P-30-26  seer     $$60 Fixed flags for start_background_proc
26-Nov-18 P-60-26  KSV      $$61 Fixed missing std input (who needs it?)
17-Jun-20 P-80-10  ravjain  $$62 Handle inheritance should be true
                                 if STARTF_USESTDHANDLES used - SPR 10087989
13-Jul-20 P-80-12  KSV      $$63 Removed sandbox leftovers
25-Nov-21 P-90-37  Ahmad    $$64 scrambled literal env vars
25-Aug-24 Q-12-27  DevOps   $$65 Use standard wide string functions
\*********************************************************************/




/*
 * converts '"' to ''' on UNIX, or ''' to '"' on Windows. Needed because
 * pro_check_space_in_command() protects spaces only in the command in a
 * system call, not in args to cmd; these args can now contain spaces (even
 * on UNIX?), so code calling (e.g.) pro_exec_command() needs a way to quote
 * these args. The problem is that the UNIX system calls often do
 *	sprintf( real_cmd, "csh -fc \"%s\"", in_cmd );
 * so a system call with the args in '"' will quote all the wrong sections
 * in UNIX, but a system call with the args in ''' will break on NT (which
 * still executes the call with DOS, which doesn't understand ''').
 *    WARNING: this code can safe-quote both platforms. However, it can NOT be
 * safely called on Windows until pro_check_space_in_command() (a horrible
 * hack in my opinion) has been abolished and all system calls required to
 * explicitly quote all parts of the call (cmd or args) that contain spaces.
 * Accordingly,
 *	QUOTEMARKS IN SYSTEM CALLS MUST BE '"' (double quote), NOT ''' (single)
 * (so Windows will work as is and UNIX will be protected)
 *    If this is ever used on Windows, remove the call to crash() at the
 * top of this function.
 *							       -- CHI, 8-Nov-02
 */
static enum ProParseDirErr MakeSafeQuotes( char* src_path, char** p_dest_path )
{
  char* c;

#if OPER_SYS == UNIX
#define SAFE_QUOTE '\''
#define UNSAFE_QUOTE '"'
#elif OPER_SYS == WINDOWS_32
#define SAFE_QUOTE '"'
#define UNSAFE_QUOTE '\''
  crash();
#endif

  if (p_dest_path == NULL)
    return( PRO_INTERNAL_ERROR );

  /* always return a copy -- caller responsible for freeing when done */
  *p_dest_path = getmem(  strlen( src_path )  + 1  );
  strcpy( *p_dest_path, src_path );

  c = *p_dest_path;
  while ( *c != '\0' )
    {
      if ( *c == UNSAFE_QUOTE )
	*c = SAFE_QUOTE;
      c++;
    }
  return( PRO_SUCCESS );

#undef SAFE_QUOTE
#undef UNSAFE_QUOTE

}

enum ProParseDirErr pro_quote_exe(char *src_path, char **p_dest_path)
{
  if ( p_dest_path == NULL )
    return (PRO_INTERNAL_ERROR);

  /* if the path contains quotes then return copy of the path */
  if (strchr(src_path,'\"'))
    {
      *p_dest_path = getmem( strlen(src_path) + 1 );
      if ( *p_dest_path == NULL )
	return( PRO_INTERNAL_ERROR );
      strcpy(*p_dest_path, src_path);
    }
  else
    {
      /* if the path contains spaces and no quotes then
	 return copy of the path with quotes around it */
      if (strchr(src_path,' '))
	{
	  *p_dest_path = getmem( strlen(src_path) + 3 );
	  if (*p_dest_path == NULL)
	    return (PRO_INTERNAL_ERROR);
	  btk_sprintf(*p_dest_path,"\"%s\"", src_path);
	}
      else
	{
	  /* if the path contains no spaces and no quotes then
	     return copy of the path */
	  *p_dest_path = getmem( strlen(src_path) + 1 );
	  if (*p_dest_path == NULL)
	    return (PRO_INTERNAL_ERROR);
	  strcpy(*p_dest_path, src_path);
	}
    }

  return (PRO_SUCCESS);

}


int pro_check_file_is_executable(char *fname)
{
   int   status = FALSE;
   Pfa  *pfafile;

   pfa_alloc_pro_file(&pfafile);
   pfa_parse_to_pro_file(pfafile, fname);
   if (!pfa_is_dir(pfafile))
   {
      /* this is here to avoid pfa_access lowercasing the file name.  HMR */
      (void) pfa_put_obj_type(pfafile, T_FOREIGN);
      if (pfa_access(pfafile, ACCESS_EXECUTE))
         status = TRUE;
   }

   pfa_free_pro_file(&pfafile);

   return (status);
}


int pro_check_space_in_command(char *command, char **newcmd)
{
  char *tmpbuf, *rest, *cmdstring;
  char *ptr, *quotedcmd;
  int is_executable = FALSE;
  int first = TRUE;

  tmpbuf = getmem( strlen(command) + 1 );
  rest = getmem( strlen(command) + 1 );
  cmdstring = getmem( strlen(command) + 1 );

  strcpy(cmdstring, command);

  if ( (strstr(cmdstring, " ") == NULL) || (strstr(cmdstring, "\"") != NULL) )
    {
      *newcmd = getmem( strlen(command) + 1 );

      if (*newcmd != NULL)
          strcpy(*newcmd, command);

      is_executable = TRUE;
    }
  else
    {
      while ( (! is_executable) && ( (ptr = strchr(cmdstring, ' ')) != NULL) )
	{
	  strcpy(rest, ptr+1);
	  *ptr = '\0';

	  if (first)
	    {
	      strcpy(tmpbuf, cmdstring);
	      first = FALSE;
	    }
	  else
	    btk_sprintf(tmpbuf, "%s %s", tmpbuf, cmdstring);

	  strcpy(cmdstring, rest);

	  if (pro_check_file_is_executable(tmpbuf))
	    {
	      is_executable = TRUE;
	      pro_quote_exe(tmpbuf, &quotedcmd);
	      *newcmd = getmem( strlen(command) + 3 );
	      btk_sprintf(*newcmd, "%s %s", quotedcmd, cmdstring);
	      relmem( &quotedcmd );
	    }
	}

      if (! is_executable)
	{
	  btk_sprintf(tmpbuf, "%s %s", tmpbuf, cmdstring);
	  if (pro_check_file_is_executable(tmpbuf))
	    {
	      is_executable = TRUE;
	      pro_quote_exe(tmpbuf, &quotedcmd);
	      *newcmd = getmem( strlen(command) + 3 );
	      strcpy(*newcmd, quotedcmd);
	      relmem( &quotedcmd );
	    }
	  else
	    {
	      *newcmd = getmem( strlen(command) + 1 );
	      strcpy(*newcmd, command);
	    }
	}
    }

  relmem( &tmpbuf );
  relmem( &rest );
  relmem( &cmdstring );

  return (is_executable);
}


#if OPER_SYS == WINDOWS_32
void handle_nt_env_vars(char *command, char *newcmd)
{
   char rest[K_PATH_SIZE], cmdstring[K_PATH_SIZE], env[K_PATH_SIZE];
   char *p, *ptr;
   int  match = FALSE;

   newcmd[0] = '\0';
   strcpy(cmdstring, command);

   while ((ptr = strchr(cmdstring, '%')) != NULL)
   {
	if (match == FALSE)
	{
	   strcpy(rest, ptr+1);
	   *ptr = '\0';

           strcat(newcmd, cmdstring);

	   match = TRUE;
	}
	else
	{
	   strcpy(rest, ptr+1);
           *ptr = '\0';

           p = btk_getenv (cmdstring);
           if (p != NULL)
                strcpy(env, p);
           else
                pro_sprintf(env, "%%%s%%", cmdstring);

	   strcat(newcmd, env);

           match = FALSE;
        }

	strcpy(cmdstring, rest);
   }

   if (match)
   	pro_sprintf(newcmd, "%s%%%s", newcmd, cmdstring);
   else
   	strcat(newcmd, cmdstring);

}
#endif

static int pro_exec_command_line_with_events (char *cmd_string, int *retval)
{
    static int    (*function)(char *) = (int (*)(char *)) NULL;
    int             status = 0;

    if (function == (int (*)(char *)) NULL)
    {
        function =
            (pro_is_bound ("uit_system_call") ?
             (int (*)(char *)) pro_call ("uit_system_call") :
             (int (*)(char *)) pro_exec_command_line_with_events);
    }

    if (function != (int (*)(char *)) pro_exec_command_line_with_events &&
        get_input_mode () != FILE_INPUT)
    {
        status = (*function) (cmd_string);

        INIT_ARG (retval, status);

        status = 1;
    }

    return (status);
}

/*---------------------------------------------------------*/
/* This function returns the return value of a system call.*/
/* If interested in retval, return the return value that   */
/* the process passed when it exited (if mode is DETATCHED */
/* then retval is always set to 0 since there is no exit   */
/* value from child process yet.                           */
/*---------------------------------------------------------*/
int pro_exec_command_line( char *cmd_string, int mode, int *retval )
{
int status;
WAIT_STATUS wstatus;

#if OPER_SYS == WINDOWS_32
char  string_buf[COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX];
char *pointer_buf[COMMAND_ARG_NO_MAX + 2]; /* shell /c command... */
char  comspec[SHELL_PATH_LEN_MAX + 1];
char *p_comspec;
char *newcmd = NULL;
#endif
if (retval)  *retval = 0;

#if OPER_SYS == UNIX
 {
   dcl_com_start_func();

   if (mode != WAIT_AND_PROCESS_EVENTS_MODE ||
       !(pro_exec_command_line_with_events (cmd_string, &status)))
   {
     status = pro_unix_system (cmd_string, (mode != DETACHED_MODE), 1, 0);
   }

   dcl_com_end_func();

   SET_WAIT_STATUS (wstatus, status);

   if (mode != DETACHED_MODE && retval != NULL && WIFEXITED(wstatus))
     *retval = WEXITSTATUS(wstatus); /* The 8 low order bits of exit value */
 }
#endif


#if OPER_SYS == WINDOWS_32
 {
 /* The code below replaces unix-style env. variables with
    windows-style ones, i.e. it does substitution $<name> -> %<name>% */
   wchar_t	*wcommand, *dsign;
   char *	 command;
   size_t    cmdlen=strlen(cmd_string)+1;

   wcommand = (wchar_t *)getmem(cmdlen*sizeof(wchar_t));
   btk_mbstowcs(wcommand, cmd_string, cmdlen);
   dsign=wstrchr(wcommand, L'$');

   if (dsign!=NULL)
   {
	   wchar_t *	geted=wcommand;
	   wchar_t		bf[COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX];
	   wchar_t		varname[COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX];
	   wchar_t *	insbf=bf;
	   size_t		n, wlen;

	   while (dsign!=NULL)
	   {
		   n=(dsign-geted);
		   insbf=wstrncpy(insbf, geted, n)+n;
		   geted=dsign+1;
		   n=wstrspn(geted, L"_0123456789"
			   L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			   L"abcdefghijklmnopqrstuvwxyz");
		   *(wstrncpy(varname, geted, n)+n)=L'\0';
           if (n == 0 || _wgetenv(varname) == NULL) /* no such variable */
           {
               *insbf=L'$', ++insbf;
               (void)wstrcpy(insbf, varname), insbf+=n;
           }
           else
           {
               *insbf=L'%', ++insbf;
               (void)wstrcpy(insbf, varname), insbf+=n;
               *insbf=L'%', ++insbf;
           }

		   geted+=n;
		   dsign=wstrchr(geted, L'$');
	   }

	   wstrcpy(insbf, geted);
	   wlen=(wstrlen(bf)+1)*4;
	   command=(char *)getmem(wlen);
	   btk_wcstombs(command, bf, wlen);

	   /* handle space in the directory */
	   pro_check_space_in_command(command, &newcmd);
	   relmem(&command);
   }
   else
   {
	   /* handle space in the directory */
	   pro_check_space_in_command(cmd_string, &newcmd);
   }

   relmem((char *)&wcommand);
   dcl_com_start_func();

     {
       if (mode == DETACHED_MODE)
	 {
	   status = start_background_proc(cmd_string, TRUE, FALSE);
	 }
       else
	 {
	   char* s = newcmd;
	   int quote_ct = 0;

	   /*
	    * support multi-line mapkey scripts; if cmd string contains a
	    * newline, dump it to a batch file and run the file.
	    * This does NOTHING about platform incompatibility; the caller
	    * is responsible for providing a string that will make a usable
	    * batch file, just as it must provide a string that makes sense
	    * to "sh" (rather than csh) when calling this code in UNIX.
	    * 						-- CHI, 20-May-02
	    *
	    * Added quote_ct to deal with 976500. Problem is that on Windows,
	    *	system( "cmd, \"<arg with spaces>\"" )
	    * and
	    *   system( "\"<path with spaces>/cmd\", arg" )
	    * work, but
	    *	system( "\"<path with spaces>/cmd\", \"<arg with spaces>\"" )
	    * gets
	    *   '<path>' is not recognized
	    * . (We've gone several rounds with Microsoft about this; they
	    * refuse to fix it, apparently because this is caused by some
	    * cleverness they can't disentangle without breaking some other
	    * functionality). But putting the last in a batch file works,
	    * probably because it gets around the cleverness.
	    *						-- CHI, 30-Jan-03
	    * We could do some elaborate parsing to find out whether all the
	    * '"' are necessary, but it probably wouldn't win enough to be
	    * worth the chance of getting it wrong; so just scan the cmd until
	    * we find >3 '"' or any '\n', and dump to a file if we do.
	    */
	   while ( *s != '\0'  &&  *s != '\n'  &&  quote_ct < 4 )
	     {
	       if ( *s == '"' )
		 quote_ct++;
	       s++;
	     }
	   if ( *s == '\n'  ||  quote_ct > 3 )
	     {
	       Pfa* pfa;
	       char name[PRO_PATH_SIZE];
	       int err;

	       err = pfa_alloc_pro_file( &pfa );
	       /*
		* initialize to correct type instead of putting extension
		* ".bat" Otherwise pfa will pick an unused name without "bat";
		* name+".bat" might exist, breaking pfa_secure_fopen() below.
		*/
	       if ( err == PFA_E_NO_ERROR )
	         err = pfa_make_temp_file( pfa, T_BATCH_FILE );
	       /* prevent BOM in script -- breaks processing */
	       if ( err == PFA_E_NO_ERROR )
	         err = pfa_put_encode( pfa, ENC_T_EUC );
	       if ( err == PFA_E_NO_ERROR )
	       /*
		* default output format for text is Unicode as of L-01-12.
		* We're almost certainly putting out only 7-bit text, but
		* QDOS barfs on the BOM at the beginning of the file; write in
		* in EUC so BOM doesn't happen.
		*/
		 err = pfa_put_encode( pfa, ENC_T_EUC );
	       if ( err == PFA_E_NO_ERROR )
	         err = pfa_secure_fopen( pfa,"w" );
	       if ( err == PFA_E_NO_ERROR )
	         err = pfa_fprintf( pfa, "%s\n", newcmd );
	       if ( err == PFA_E_NO_ERROR )
	         err = pfa_close( pfa );
	       if ( err == PFA_E_NO_ERROR )
	         err = pfa_get_full_name( pfa, name );
	       if ( err == PFA_E_NO_ERROR )
		 {
                   if (mode != WAIT_AND_PROCESS_EVENTS_MODE ||
                       !(pro_exec_command_line_with_events (name, &status)))
                     {
                       status = btk_system( name );
                     }

		   pfa_delete_file( pfa, THIS_VERSION );
		   pfa_free_pro_file( &pfa );
		 }
	       else
		 status = -1;
	     }
	   else if (mode != WAIT_AND_PROCESS_EVENTS_MODE ||
                    !(pro_exec_command_line_with_events (newcmd, &status)))
	     status = btk_system(newcmd);
	 }
     }

   dcl_com_end_func();

   relmem( &newcmd );

#if PRO_MACHINE != APOLLO
   if (mode != DETACHED_MODE && retval != NULL)
     *retval = (status >= 0) ? status : -1;
#else
#endif
 }
#endif

 return(status);
}

/*********************************************************************/
int  pro_exec_command (
/*********************************************************************/
#ifdef PRO_ANSI_VARARGS
			int mode, char *cmd_fmt, ... )
#else
			mode, cmd_fmt, va_alist )
 int    mode;
 char  *cmd_fmt;  /* printf() format */
 va_dcl		  /* optional printf() args */
#endif
{
 int      status = 0;
 char     cmd_string[COMMAND_LEN_MAX];
 va_list  ap;


 if ( cmd_fmt != (char *)NULL )
   {
    pro_va_start(ap, cmd_fmt);

    if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
       {
	  dbg_crash_msg ("pro_exec_command", "bad format %s", cmd_fmt);
	  return (-1);
       }

    va_end( ap );
   }

 status = pro_exec_command_line(cmd_string, mode, (int *)NULL);
 return(status);
}

/*********************************************************************/
/* Return value indicates whether system call was successful or not  */
/* (E_NO_ERROR/E_ERROR);  If present, the return argument ret_value  */
/* will be set to the exit value of the child process. (Non detatched*/
/* modes only; else 0).                                              */
/* It is important to realize that the application receiving the exit*/
/* status of the child process MUST be able to decipher this value   */
/* properly.  This means that it must know the possible exit error   */
/* codes of the child process.  Since some of these processes may be */
/* operating system dependent (ls (UNIX), dir(VMS) etc), ret_value   */
/* must be handled carefully and in an OS independent way.           */
/*********************************************************************/
int  pro_system_call(
#ifdef PRO_ANSI_VARARGS
                     int *ret_value, int mode, char *cmd_fmt, ... )
#else
                     ret_value, mode, cmd_fmt, va_alist )
 int   *ret_value, mode;
 char  *cmd_fmt;  /* printf() format */
 va_dcl           /* optional printf() args */
#endif
{
 int      status, ret_status;
 char     cmd_string[COMMAND_LEN_MAX];
 va_list  ap;

 if ( cmd_fmt != (char *)NULL )
  {
   /* clumsy attempt to guard against overwrite */
   cmd_string[COMMAND_LEN_MAX - 1] = (char) 255;

   pro_va_start(ap, cmd_fmt);
   if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
      {
	 dbg_crash_msg ("pro_system_call", "bad format %s", cmd_fmt);
	 if (ret_value)
	    *ret_value = -1;
	 return (E_ERROR);
      }

   va_end( ap );

   if (cmd_string[COMMAND_LEN_MAX - 1] != (char) 255)
     {
       dbg_crash_msg ("pro_system_call", "resulting command string too long");
       if (ret_value)
	 *ret_value = -1;
       return (E_ERROR);
     }
  }

 status = pro_exec_command_line(cmd_string, mode, &ret_status);
 if (ret_value)   *ret_value = ret_status;

#if OPER_SYS == VMS_OS
  ret_status = (ret_status & SS$_NORMAL) ? E_NO_ERROR : E_ERROR;
#endif

#if OPER_SYS == UNIX
  ret_status = (status >= 0) ? E_NO_ERROR : E_ERROR;
#endif

#if OPER_SYS == WINDOWS_32
  ret_status = (status >= 0) ? E_NO_ERROR : E_ERROR;
#endif

 return(ret_status);
}

/* The following command supports multiple commands execution inside the */
/* single OS shell                                                       */
int pro_system_script_call(
#ifdef PRO_ANSI_VARARGS
                           int mode, char *cmd_fmt, ... )
#else
                           mode, cmd_fmt, va_alist )
int   mode;
char  *cmd_fmt;  /* printf() format */
va_dcl           /* optional printf() args */
#endif
{
#ifndef NOT_USE_INTERNAL
    int      ret_status;
    char     cmd_string [COMMAND_LEN_MAX] = "";
    va_list  ap;

    if (cmd_fmt != (char *)NULL)
    {
        /* clumsy attempt to guard against overwrite */
        cmd_string[COMMAND_LEN_MAX - 1] = (char) 255;

        pro_va_start(ap, cmd_fmt);

        if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
        {
            dbg_crash_msg ("pro_system_script_call", "bad format %s", cmd_fmt);
            return 0;
        }

        va_end( ap );

        if (cmd_string [COMMAND_LEN_MAX - 1] != (char) 255)
        {
            dbg_crash_msg ("pro_system_script_call", "resulting command string too long");
            return 0;
        }
    }

    dcl_com_start_func();
#if OPER_SYS == UNIX
    ret_status = pro_unix_system (cmd_string, mode != DETACHED_MODE, 1, 0);
#else /* OPER_SYS != UNIX */
    ret_status = btkExecuteShellCommands (cmd_string, mode != DETACHED_MODE);
#endif
    dcl_com_end_func();

    return (ret_status);
#else
    return 0;
#endif
}

/*********************************************************************\
 * Function: pro_exec_background_shell
 *
 * Description: Execute a command in the background, invoking it
 *              through a shell so as to allow the use of shell and
 *              environment variables. This function is similar to
 *              pro_exec_command; the main differences are: (1) the
 *              "mode" argument is not present -- the command always
 *              runs in the background; (2) on NT, the process is
 *              spawned with _P_DETACH rather than _P_NOWAIT to ensure
 *              that no extraneous console window appears.
 *
 * Input:      cmd_fmt
 *              printf format string for the command line being
 *              executed
 *
 *             va_alist
 *              optional additional printf arguments
 *
 * Return:      TRUE = success
 *              FALSE = failure
 *
 * Notes:       The code for this function is mostly excerpted from
 *              pro_exec_command. The latter is left intact because
 *              many places in Pro/E call it. Asynchronous Pro/Develop
 *              now calls this function instead, and this function or
 *              pro_start_background_process() should be used in
 *              preference to pro_exec_command() in all future calls.
\*********************************************************************/


/*********************************************************************/
int  pro_exec_background_shell (
/*********************************************************************/
#ifdef PRO_ANSI_VARARGS
			char *cmd_fmt, ... )
#else
			cmd_fmt, va_alist )
 char  *cmd_fmt;  /* printf() format */
 va_dcl		  /* optional printf() args */
#endif
{
    char     cmd_string[COMMAND_LEN_MAX];
    va_list  ap;

    if ( cmd_fmt != (char *)NULL )
    {
        pro_va_start(ap, cmd_fmt);

        if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
        {
            dbg_crash_msg ("pro_exec_background_shell", "bad format %s", cmd_fmt);
            return (FALSE);
        }

        va_end( ap );
    }

    return (start_background_proc(cmd_string, TRUE, FALSE));
}

#if OPER_SYS == WINDOWS_32
static HANDLE start_background_proc_ex(char *, int, int);
#else
static pid_t  start_background_proc_ex(char *, int, int);
#endif

#if OPER_SYS == WINDOWS_32
HANDLE pro_exec_background_shell_ex (
#else
pid_t  pro_exec_background_shell_ex (
#endif
#ifdef PRO_ANSI_VARARGS
			char *cmd_fmt, ... )
#else
			cmd_fmt, va_alist )
 char  *cmd_fmt;  /* printf() format */
 va_dcl		  /* optional printf() args */
#endif
{
    char     cmd_string[COMMAND_LEN_MAX];
    va_list  ap;

    if ( cmd_fmt != (char *)NULL )
    {
        pro_va_start(ap, cmd_fmt);

        if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
        {
            dbg_crash_msg ("pro_exec_background_shell", "bad format %s", cmd_fmt);
            return (FALSE);
        }

        va_end( ap );
    }
    return (start_background_proc_ex(cmd_string, TRUE, FALSE));
}

/*********************************************************************\
 * Function: pro_start_background_process
 *
 * Description: Start a process in the background, more or less as a
 *              daemon process. This function passes the command line
 *              directly to the OS, without passing it through a shell
 *              program.
 *
 * Input:      cmd_fmt
 *              printf format string for the command line being
 *              executed
 *
 *             va_alist
 *              optional additional printf arguments
 *
 * Return:      TRUE = success
 *              FALSE = failure
 \*********************************************************************/

/*********************************************************************/
int  pro_start_background_process (
/*********************************************************************/
#ifdef PRO_ANSI_VARARGS
                                   char *cmd_fmt, ... )
#else
    cmd_fmt, va_alist )
    char  *cmd_fmt;  /* printf() format */
    va_dcl		  /* optional printf() args */
#endif
{
    char     cmd_string[COMMAND_LEN_MAX];
    va_list  ap;

#if OPER_SYS == WINDOWS_32
    char *env_tmp_dir;
    char *working_dir;
    char  temp_dir_name[K_PATH_SIZE];
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA         siStartInfo;
#endif

    if ( cmd_fmt != (char *)NULL )
    {
        pro_va_start(ap, cmd_fmt);

        if (pro_vsprintf(cmd_string, cmd_fmt, ap) == NULL)
        {
            dbg_crash_msg ("pro_start_background_process", "bad format %s", cmd_fmt);
            return (FALSE);
        }

        va_end( ap );
    }

    return (start_background_proc(cmd_string, FALSE, TRUE));
}

/*********************************************************************\
 * Function:    start_background_proc
 *
 * Description: Common function used by pro_exec_background_shell()
 *              and pro_start_background_process(). Starts a
 *              background process, through a shell if required.
 *
 * Input:      cmd_string
 *              The command line to be executed.
 *             use_shell
 *               if TRUE, pass the command line through a shell.
 *             use_temp_dir
 *               if TRUE, start the process in the system "temp" directory
 *               (NT only)
 *
 * Return:      TRUE = success
 *              FALSE = failure
 *
 * Notes:       The following notes apply on Windows platforms:
 *            - the process is run in "detached" mode, to ensure
 *              that it will not bring up an extraneous console window;
 *            - handles are not inherited, so the process is
 *              responsible for managing its own stdout and stderr;
 *            - if use_temp_dir is set, the process is started (if
 *              possible) with its working directory set to the system
 *              "temp" directory, so that it can create files if
 *              necessary and so that the directory of the parent
 *              process can be deleted without waiting for the
 *              background process to exit.
 \*********************************************************************/

int start_background_proc(cmd_string, use_shell, use_temp_dir)
    char *cmd_string;
    int   use_shell;
    int   use_temp_dir;
{
  int status;
#if OPER_SYS == WINDOWS_32
  /*
   * this looks wrong -- the two constants measure different types -- but the
   * result has worked for some time and is big enough for the fixes to SPR
   * 1591173, so I won't change it now.
   */
  char  exec_string[COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX];
  char  comspec[SHELL_PATH_LEN_MAX + 1];
  char *p_comspec;
  char *env_tmp_dir;
  char *working_dir = NULL;
  char  temp_dir_name[K_PATH_SIZE];
  char *newcmd = NULL;
  Pfa  *wd_pfa;
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFOA         siStartInfo;
  int                 background_flag;
  int                 inherit_flag;
  HANDLE              hStdInput = NULL;
  HANDLE              hStdOutput = NULL;
  HANDLE              hStdError = NULL;
  int                 use_console = TRUE;
  BOOL                rc = FALSE;
  HANDLE              p0, p1;
#endif

#if OPER_SYS == UNIX

  dcl_com_start_func();
  status = (pro_unix_system (cmd_string, 0 /* no wait */, use_shell, 0) == 0);
  dcl_com_end_func();

#endif /* OPER_SYS == UNIX */


#if OPER_SYS == WINDOWS_32

  /* handle space in the directory */
  pro_check_space_in_command(cmd_string, &newcmd);

  if (use_shell)
    {
      /* The command has to be interpreted by the shell. The env variable COMSPEC
	 is supposed to give the shell executable path name */
      p_comspec = BTK_GETENV_31_S("COMSPEC");
      if (p_comspec != NULL)
        {
	  char* cp = newcmd;
	  int qct = 0;

	  strcpy(comspec, p_comspec); /* local copy to be marginally safer */

	  /*
	   * XYZZY_1591173:
	   * By observation:
	   *  - The previous code failed when newcmd contained an executable
	   *    path and an arg, and both contained spaces -- even if they were
	   *    both protected with '"'. (Old talks with Microsoft suggested
	   *    that this related to their handling an exe path with spaces
	   *    even if it isn't quoted -- but if they were handling such
	   *    paths, why did someone write pro_check_space_in_command()?)
	   *  - Wrapping \all/ of newcmd in quotes fixes the above failure,
	   *    provided both strings were protected with '"'
	   * So if newcmd contains >2 '"', wrap the entire string in '"'.
	   * (Calling code \should/ be responsible for putting '"' around
	   * individual args as necessary; pro_check_space_in_command() does
	   * this for completely unquoted strings which need only one set of
	   * '"' added.) Tests suggest that wrapping the entire string in all
	   * cases should be safe -- but I'm looking for a minimal fix to go in
	   * a shipping version.
	   * NOTE: dumping a string with >2 quotes to a .bat worked for
	   * synchronous processes (see "976500") but did not work here, for
	   * async processes. Possibly sync processes should also have been
	   * wrapped in '"' rather than dumped to a .bat, but that's not a
	   * change to make in a shipping version (and may not work since the
	   * sync fix also handles multiline cmds).
	   *						-- CHI, 24-Nov-08
	   */
	  while ( *cp != '\0' )
            {
	      if ( *cp++ == '"' )
		qct++;
            }
	  btk_sprintf( exec_string,
		       qct > 2  ?  "%s /c \"%s\"" : "%s /c %s",
		       comspec, newcmd );
            }
      else
        {
	  relmem( &newcmd );	/* silence PREFIX */
	  return (FALSE);     /* abort */
        }
      background_flag = DETACHED_PROCESS;
      inherit_flag = FALSE;
      if (!AllocConsole())
	{
	  use_console = FALSE;
	}
      hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
      hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      hStdError  = GetStdHandle(STD_ERROR_HANDLE);
      if (hStdInput == INVALID_HANDLE_VALUE)
	  {
          if (CreatePipe(&p0, &p1, NULL, 0))
            rc = DuplicateHandle(GetCurrentProcess(), p0, GetCurrentProcess(), &p0,
              0, TRUE, DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE);
          if (rc)
            hStdInput = p0;
      }
      if ((hStdInput == INVALID_HANDLE_VALUE)  ||
	  (hStdOutput == INVALID_HANDLE_VALUE) ||
	  (hStdError  == INVALID_HANDLE_VALUE))
	{
	  relmem( &newcmd );	/* silence PREFIX */
	  return (FALSE);
	}
    }
  else
    {
      background_flag = DETACHED_PROCESS;
      inherit_flag = FALSE;
      strcpy(exec_string, newcmd);
    }

  if (use_temp_dir)
    {
      /* set working directory for new process to be system temp dir */

      if ((env_tmp_dir = BTK_GETENV_31_S("TMP")) != NULL)
        {
	  /* for safety, copy the dir name locally */
	  strcpy(temp_dir_name, env_tmp_dir);

	  /*
	   * make sure directory in question exists and is writeable.
	   * We have to make it into a PFA in order to check. If anything
	   * fails, or the check answers FALSE, leave working_dir as NULL.
	   */
	  if (pfa_alloc_pro_file(&wd_pfa) == PFA_E_NO_ERROR)
            {
	      if (pfa_parse_to_pro_file(wd_pfa, temp_dir_name) == PFA_E_NO_ERROR &&
		  pfa_access(wd_pfa, ACCESS_WRITE))
		{
		  working_dir = temp_dir_name;
		}
	      /* if we allocated a PFA, free it */
	      (void) pfa_free_pro_file(&wd_pfa);
            }
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
      inherit_flag           = TRUE; /* According to msdn this must be true if STARTF_USESTDHANDLES used */
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
  if (use_shell && rc)
  {
      CloseHandle(p1);
	  CloseHandle(p0);
  }

  /* close process and thread handles to avoid resource leak */

  relmem( &newcmd );

  if (status)
    {
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
    }

  if (use_shell)
    {
      if (use_console)
        {
	  FreeConsole();
        }
    }
#endif /* WINDOWS_32 */

  return(status);
}

#if OPER_SYS == WINDOWS_32
static HANDLE start_background_proc_ex(cmd_string, use_shell, use_temp_dir)
#else
static pid_t  start_background_proc_ex(cmd_string, use_shell, use_temp_dir)
#endif
    char *cmd_string;
    int   use_shell;
    int   use_temp_dir;
{
#if OPER_SYS == WINDOWS_32
  int status;
  char  exec_string[COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX];
  char  comspec[SHELL_PATH_LEN_MAX + 1];
  char *p_comspec;
  char *env_tmp_dir;
  char *working_dir = NULL;
  char  temp_dir_name[K_PATH_SIZE];
  char *newcmd = NULL;
  Pfa  *wd_pfa;
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFOA         siStartInfo;
  int                 background_flag;
  int                 inherit_flag;
  HANDLE              hStdInput = NULL;
  HANDLE              hStdOutput = NULL;
  HANDLE              hStdError = NULL;
  int                 use_console = TRUE;
  HANDLE              pid = 0;
#endif

#if OPER_SYS == UNIX
  pid_t pid;
  void (*save_SIGCHLD)(int);
  pid_t child_proc;
  char shell_cmd [UNIX_SYSTEM_CMD_STR_LEN];

  dcl_com_start_func();

  if (use_shell) /* it's an iffy legacy. 3/17/98:prf */
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
      btk_sprintf(shell_cmd, "%s \"%s\"", "csh -fc", newcmd);

      relmem( &newcmd );
    }
  else
    {
      btk_sprintf(shell_cmd, "%s", cmd_string);
    }

  child_proc = btk_spawnl(P_NOWAIT, "/bin/sh", "/bin/sh", "-c", shell_cmd, 0);
  if (child_proc == (pid_t) -1)
    {
      perror ("ERROR ");
      btk_fprintf (btk_get_stderr(), "ERROR : Cannot launch child process.");
      return 0;
    }

  pid = child_proc;

  dcl_com_end_func();

#endif /* OPER_SYS == UNIX */

#if OPER_SYS == WINDOWS_32

  /* handle space in the directory */
  pro_check_space_in_command(cmd_string, &newcmd);

  if (use_shell)
    {
      /* The command has to be interpreted by the shell. The env variable COMSPEC
	 is supposed to give the shell executable path name */
      p_comspec = BTK_GETENV_31_S("COMSPEC");
      if (p_comspec != NULL)
        {
	  char* cp = newcmd-1;
	  int qct = 0;

	  strcpy(comspec, p_comspec); /* local copy to be marginally safer */

	  /* see XYZZY_1591173 */
	  btk_sprintf( exec_string,
		       qct > 2  ?  "%s /c \"%s\"" : "%s /c %s",
		       comspec, newcmd );
            }
	  else
            {
	  relmem( &newcmd );	/* silence PREFIX */
	  return 0;     /* abort */
        }
      background_flag = DETACHED_PROCESS;
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
	  relmem( &newcmd );	/* silence PREFIX */
	  return 0;
        }
    }

  else
    {
      background_flag = DETACHED_PROCESS;
      inherit_flag = FALSE;
      strcpy(exec_string, newcmd);
    }

  if (use_temp_dir)
    {
      /* set working directory for new process to be system temp dir */

      if ((env_tmp_dir = BTK_GETENV_31_S("TMP")) != NULL)
        {
	  /* for safety, copy the dir name locally */
	  strcpy(temp_dir_name, env_tmp_dir);

	  /*
	   * make sure directory in question exists and is writeable.
	   * We have to make it into a PFA in order to check. If anything
	   * fails, or the check answers FALSE, leave working_dir as NULL.
	   */
	  if (pfa_alloc_pro_file(&wd_pfa) == PFA_E_NO_ERROR)
            {
	      if (pfa_parse_to_pro_file(wd_pfa, temp_dir_name) == PFA_E_NO_ERROR &&
		  pfa_access(wd_pfa, ACCESS_WRITE))
                {
		  working_dir = temp_dir_name;
                }
	      /* if we allocated a PFA, free it */
	      (void) pfa_free_pro_file(&wd_pfa);
            }
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
	  inherit_flag           = TRUE; /* must be true if STARTF_USESTDHANDLES used */
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

  /* close process and thread handles to avoid resource leak */

  relmem( &newcmd );

  if (status)
    {
      pid = piProcInfo.hProcess;
      CloseHandle(piProcInfo.hThread);
    }

  if (use_shell)
    {
      if (use_console)
        {
	  FreeConsole();
        }
    }
#endif /* WINDOWS_32 */

  return (pid);
}

/****************************************************************************/
/* in ms Windows it is better to send parameter to function separatly		*/
/* ask all to migrate to it from pro_spawn_for_version_check				*/
/****************************************************************************/
#if OPER_SYS == WINDOWS_32
int pro_spawn_for_version_check_use_array (exec_name, exec_param)
char *exec_name;	/* the full path to executable, may contain spaces and be in quotes */
char **exec_param; /* array containing exe parameters, starting from exec_params[0], last parametes should be 0 */
{
#define		SPAWN_MAX_PARAMS 20

	int ret_status = 0;
	char *chrp;

    char exec_str[COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX + 1];
    char comm_str[_MAX_PATH + 1];

    int status;
    int i = 0;
    size_t str_size;
    char *ch;
	int inherit_flag = FALSE;

    STARTUPINFOA startinfo;
    PROCESS_INFORMATION procinfo;
    BOOL success;

	/* if exec_name surrounded with "", then remove quotes
	*/
	if (*exec_name == '\"') {
		strcpy( comm_str, exec_name+1);
		for (chrp = comm_str ; *chrp != 0 ; chrp++) {
            /* removing second quote, check for multibute case
             */
			if ((*chrp == '\"') && (*(chrp+1) == 0) )
				*chrp = 0;
		}
	}
	else {
		strcpy( comm_str, exec_name);
	}

    strncpy (exec_str, comm_str, COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX);
    str_size = strlen (exec_str);

    /* append parameters to exec_str */
    while ((ch = exec_param[i++]) != NULL) {
        exec_str [str_size++] = ' ';
        while ((*ch  != 0) &&
               (str_size <= COMMAND_LEN_MAX + COMMAND_ARG_NO_MAX)
              ) {
            exec_str [str_size++] = *ch;
            ch++;
        }
        exec_str [str_size] = 0;
    }

    /* start new process */
    dcl_com_start_func();

    startinfo.cb = sizeof (STARTUPINFOA);
    startinfo.lpReserved = NULL;
    startinfo.lpReserved2 = NULL;
    startinfo.lpTitle = NULL;
    startinfo.cbReserved2 = 0;
    startinfo.lpDesktop = NULL;
    startinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	inherit_flag = TRUE; /* must be true if STARTF_USESTDHANDLES used */
    startinfo.wShowWindow = SW_HIDE;
    startinfo.hStdInput = GetStdHandle (STD_INPUT_HANDLE);
    startinfo.hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);
    startinfo.hStdError = GetStdHandle (STD_ERROR_HANDLE);

    /*fprintf(stderr, "pro_spawn_for_version_check() launching %s\n", exec_str);*/

    if (uCreateProcess (NULL, /* Module to execute */
                       exec_str, /* Command line */
                       NULL, /* Process security attrs */
                       NULL, /* Thread security attrs */
					   inherit_flag, /* Inherit handles flag */
                       DETACHED_PROCESS, /* Creation flags */
                       NULL, /* Environment block */
                       NULL, /* directory in which to start */
                       &startinfo,/* Address of STARTUPINFO */
                       &procinfo) /* Address of PROCESS_INFORMATION */
        ) {
        WaitForSingleObject (procinfo.hProcess, INFINITE);
        GetExitCodeProcess (procinfo.hProcess, &status);
        CloseHandle (procinfo.hProcess);
        CloseHandle (procinfo.hThread);

        if ( status != 0 ) {
            perror("pro_spawn_for_version_check() : ");
            btk_fprintf(btk_get_stderr(), "SPAWNED PROCESS DID NOT EXIT NORMALLY\n");
        }
    }
    else {
        ret_status = -1;
        perror("pro_spawn_for_version_check() : SPAWN FAILED");
    }

    dcl_com_end_func();

    return ret_status;
}
#endif
/*****************************************************************************/
int pro_spawn_for_version_check(exec_name)
    char * exec_name;
{
  int ret_status = 0;

#if OPER_SYS == VMS_OS || OPER_SYS == UNIX
  ret_status = pro_exec_command (WAIT_MODE, exec_name);
#endif

#if OPER_SYS == WINDOWS_32
  char *arg, **args;
  int cnt = 0;
  int status = 0;

  /* We have to break the command line into words */
  /* first count the words */
  for (arg = exec_name; *arg != NULL_CHAR; arg++)
    if (isspace (*arg))
      cnt++;
  cnt += 1;

  /* allocate array of words */
  args = (char **)getmem( sizeof (char *) * (cnt + 1) );
  args[cnt] = NULL;

  cnt = 0;
  for (arg = exec_name; *arg != NULL_CHAR; arg++)
    if (isspace (*arg))
      {
	*arg = NULL_CHAR;
	args[cnt++] = arg+1;
      }

  /* code is extracted for direct call from gm_get_bactch_version */

  ret_status = pro_spawn_for_version_check_use_array (exec_name, args);

  relmem( &args );

#endif
  return (ret_status);
}

/*
   Purpose:
   Prepare the list of pointers to arguments for the
   variety of system calls (spawnv etc). Treats any sequence
   of space separated chars as a separate argument.
   Returns a sequence of pointers to arguments in pointer_buf
   (last pointers being NULL), uses string_buf to copy
   the arguments from pieces of cmd_string.

   All memory allocation/management done by the caller.

   Return value:
   0      SUCCESS
   -1     FAILURE - buffer too small
*/
/***************************************************************************/
static int make_arg_list(cmd_string, string_buf, string_buf_len,
			     pointer_buf, pointer_buf_len)
  char  *cmd_string;      /* INPUT */
  char  *string_buf;      /* OUTPUT */
  int    string_buf_len;  /* INPUT */
  char **pointer_buf;     /* OUTPUT */
  int    pointer_buf_len; /* INPUT */
{
    int   arg_no;         /* arg_no is the first free entry in
			     pointer_buf array */
    int   string_buf_pos; /* index of first free place in string_buf */
    char *cmd_string_pos;
    int   status;


    status            = 0;
    string_buf_pos    = 0;
    cmd_string_pos    = cmd_string;
    arg_no            = 0;



    /* Leaves the loop on error or when all cmd_string was processed.
       Pointer_buf ends with a pointer to argument after every
       loop iteration */
    do
    {
	pointer_buf[arg_no] = NULL;

	while (*cmd_string_pos == ' ' && *cmd_string_pos != '\0')
	    cmd_string_pos++;
	if (*cmd_string_pos == '\0')
	{
	    break; /* proper return, pointer_buf is correct  */
	}
	else
	    /* more arguments coming */
	{
	    if (arg_no >= pointer_buf_len - 1)
	    {
		status = -1;
		break;
	    }
	    pointer_buf[arg_no] = &(string_buf[string_buf_pos]);
	    arg_no++;
	}


	while (*cmd_string_pos != ' ' && *cmd_string_pos != '\0' &&
	       string_buf_pos < string_buf_len)
	{
	    string_buf[string_buf_pos] = *cmd_string_pos;
	    string_buf_pos++;
	    cmd_string_pos++;
	}


	if (string_buf_pos < string_buf_len)
	{
	    string_buf[string_buf_pos] = '\0';
	    string_buf_pos++;
	}
	else
	{
	    status = -1;
	    break;
	}
    }
    /*CONSTCOND*/
    while (1);


    return (status);
} /* end of make_arg_list */
/***************************************************************************/

#undef COMMAND_ARG_NO_MAX
#undef SHELL_PATH_LEN_MAX
#undef UNIX_SYSTEM_CMD_STR_LEN
