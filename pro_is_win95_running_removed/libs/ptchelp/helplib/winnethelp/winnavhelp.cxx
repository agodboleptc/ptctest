/*
   11-AUG-96   H-01-04   MTP       $$1  Created.  Code obtained from netscape.
   04-SEP-96   H-01-07   DGMJ      $$2  Call new function from help_util.c
		                        include HelpStdAfx.h
   13-SEP-96   H-01-09   DGMJ      $$3  #include help_errors.h instead of  
                                        errors.h
   21-OCT-96   H-01-14   DGMJ      $$4  Added call to which_ptc_docs_path() to 
                                        support multiple doc areas.
   23-OCT-96   H-01-14+  DGMJ      $$5  Fix #include's
   17-DEC-96   H-01-21   CFK/DGMJ  $$6  DDEML implementation of WWW_OpenURL().
   20-DEC-96   H-01-21+  DGMJ      $$7  Fix dummy call.
   30-DEC-96   H-01-21+  DGMJ      $$8  Changed no_help.html to no_help.htm.
   02-JAN-97   H-01-21+  CFK       $$9  Fixed problem with 3.51.
   20-JAN-97   H-01-24   CFK       $$10 Rewrote to support future Netscape API 
                                        implementations.
                                        Implemented WWW_Activate();
   28-JAN-97   H-01-25   CFK       $$11 Added call to get_ptc_help_command() in
					initializer.
   11-FEB-97   H-03-01   DGMJ/CFK  $$12 Modified launching sequence, 
					make sure first URL
					is properly loaded.
   25-FEB-97   H-03-02   DGMJ      $$13 Use HELP_BUFF_LEN.
   09-MAR-97   H-03-03   guy       $$14 Added show_net_url().
   20-MAR-97   H-03-04   DGMJ      $$15 Added call to _nt_to_win95() 
					for Pro/MECHANICA.
					Added call to pro_is_win95_running();
   04-JUN-97   H-03-13   DGMJ      $$16 Added win_show_wizard_help() and
					win_wizard_init_net_help().
   08-AUG-97   H-03-19   DGMJ      $$17 Removed debug fprintf()'s.
   03-SEP-97   H-03-20   DGMJ      $$18 Updated for new API,
					added _get_netscape_from_registry().
   22-SEP-97   H-03-22   DGMJ      $$19 Added _check_for_win95_netscape() to fix
					Pro/MECHANICA Win95 problem.
   29-SEP-97   H-03-23   DGMJ      $$20 Added win_close_wizard_window().
   23-OCT-97   H-03-28   DGMJ      $$21 Minor modification.
   16-JUN-98   I-01-11   DGMJ      $$22 Fixed z-order bug, added
                                        WWW_Acitivate_low() and support.
   16-JUL-98   I-01-15   DGMJ      $$23 Added Internet Explorer Support.
   14-AUG-98   I-01-17   DGMJ      $$24 Minor modifications.
   05-OCT-98   I-01-21   DGMJ      $$25 Dynamically set CLASSPATH when running IE.
   08-OCT-98   I-01-21+  DGMJ      $$26 Compilation fix "false" --> "FALSE".
   21-OCT-98   I-01-24   DGMJ      $$27 Set CLASSPATH to TRUE on first try.
   10-DEC-99   I-02-14+  TWH       $$28 Use macro SUCCEEDED
   31-Jan-00   J-01-01   TWH       $$29 Call get_short_help_id
   03-Feb-00   J-01-01   TWH       $$30 Declare #29 extern C
   01-Nov-00   J-01-20+  TWH       $$31 fix local short help path setting 847931
   01-Nov-00   J-01-21   JPE       $$32 WIN64 port.
   09-Jan-01   J-01-26+  TWH       $$33 Set BROWER_TYPE when found from registry
   19-Mar-01   J-01-30   TWH       $$34 Fix error msg on help startup NT 807311
   11-Oct-01   J-03-09   TWH       $$35 remove extern decl spg_access
   12-Oct-01 J-03-10   jmichaud  $$36  Include pro_memory.h
   16-Jan-02   J-03-17   TWH       $$37 keep only wizard code
   20-Feb-03   K-01-01   snag      $$38 correct typo for StdAfx.h 
   06-Dec-04   K-03-15   ksi       $$39 Obsoleted win_show_wizard_help
   24-Apr-06   L-01-07   TWH       $$40 use windows unicode wrappers
   16-May-08   L-01-08+  TWH       $$41 short circuit wizard cleanup func
   12-Jul-06   L-01-12   ksi       $$42 Unicode compliant changes
*/

#include <ptc_win32.h>
#include "hardware.h"

# if OPER_SYS == WINDOWS_32

#include <syswindows.h>
#include <winreg.h>
#include <ole2.h>
#include <ddeml.h>
#include <memory.h>
#include <pro_memory.h>
#include <proprintf.h>
#include <sysunistd.h>
#include <access.h>
#include <sysunistd.h>
#include <const.h>
#include <runmode.h>
#include <ptc_win32.h>


/* For internal prototypes. */
# ifndef HELP_INTERNAL
# define HELP_INTERNAL
# endif
#include "help_errors.h"

static int OleComInitialized = FALSE;

//init NetHelp for wizard
/*================================================================*/
int win_wizard_init_net_help ()
/*================================================================*/
{
  if ( OleComInitialized == FALSE ) { 
    if ( SUCCEEDED(CoInitialize(NULL))) { 
      if ( SUCCEEDED(OleInitialize(NULL))) { 
        OleComInitialized = TRUE;
        return ( HELP_NO_ERROR );
      }
    } 
  } else {
    if ( OleComInitialized == TRUE ) { 
      return ( HELP_NO_ERROR );
    }
  }
  return ( HELP_ABORT );
}

//display help_id for wizard
/*================================================================*/
int win_show_wizard_help ( char *help_id )
/*================================================================*/
{
	// Function is obsolete. Do not use it!
    return(HELP_WIZARD_UNAVAILABLE);
}

/*================================================================*/
static BOOL CALLBACK identify_window ( HWND win_ptr , 
				       wizard_title_list_ch titles )
/*================================================================*/
{
    char current_window_title[HELP_BUFF_LEN];

    uGetWindowText ( win_ptr , current_window_title , HELP_BUFF_LEN - 1 );
    if ( wizard_title_match ( current_window_title , titles ) )
    {
	//send a WM_CLOSE message to the window
	SendMessage ( win_ptr , WM_CLOSE , NULL , NULL );
	//return FALSE to terminate EnumWindows()
        return ( FALSE );
    }
    return( TRUE );
}

/*================================================================*/
int win_close_wizard_window ( wizard_title_list_ch titles )
/*================================================================*/
{
    /* this will always happen because of K-03-15 change to 
       win_show_wizard_help - TWH */
    if (! OleComInitialized ) return (HELP_NO_ERROR);

    /* seek permission to remove rounds tutor wizard all together */
    if ( ! EnumWindows ( (WNDENUMPROC)&identify_window , (LPARAM)titles ) )
	return ( HELP_ABORT );
    else
        return ( HELP_NO_ERROR );
}

# endif
