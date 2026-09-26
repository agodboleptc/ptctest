/*******************************************************************************/
/*
 * FILE:     gdi32_system.c
 *
 * DESC:     Contains all the system related graphics dependent functions.
 *
 *
 * History:
 *
 * Date      Version    Author              Description of modification
 * -----------------------------------------------------------------------------
 *
 * 14-Sep-93 D-02-48   CJH   $$1 Created.
 * 28-Nov-93 E-01-28   CJH   $$2 Realize our palette after calling an
 *                               extenal program, in case NT doesnt
 *                               tell us to.
 * 17-Dec-93 E-01-31   bah   $$3 Removed call to realize_pal_of_cur_wind ()
 *                               until it is defined by someone
 * 14-Oct-94 E-06-15   CJL   $$4 Added use of TERMINAL_COMMAND config opt
 * 10-Oct-95 G-01-10   Amin  $$5 Fix gdi32_system_call for Win 95
 * 14-May-96 G-03-13   cfk   $$6 Added func CheckGraphicsSetup()
 * 07-Jun-96 H-01-04   PLS   $$7 Direct call to pro_exec_command in gdi32_system_call,
 *                               pass the "real" argument as the third argument
 * 20-Feb-97 H-03-02   hsu   $$8 Color manager clean up

  11-Jun-97 H-03-13+   DAJ   $$9 Replace API for messages with ID-base one
  01-Mar-00 I-03-26    prf   $$10 Added gdi32_restrictions_warning ()
  21-Sep-01 J-03-09    jas   $$11 Removed WINDOWS_95 macro
  04-Jan-02 J-03-16    Chris $$12 Renamed edit to edit_file
  12-Jul-06 L-01-12    ksi   $$13 Unicode compliant changes
  22-Mar-11 L-05-44 Shturm $$14 Prototyping.
  12-Mar-12 P-20-01 AC     $$15 Updated for Project 13028358
  25-Mar-21 P-90-05 jas    $$16 Removed unused code
  16-Oct-23 Q-11-34 AKA    $$17 Used interface for calling prostandalone funcs.
  19-Dec-24 Q-12-46 jas    $$18 Use WAIT_AND_PROCESS_EVENTS_MODE for editing
  25-Dec-25 Q-13-41 Ahmad  $$19 fixing hang issue
  09-Jan-26 Q-13-43 JAY    $$20 Block the notepad/excel editor launch in avd mode
  27-May-26 Q-27-12 jas    $$21 Revert ##19 and use WAIT_AND_PROCESS_EVENTS_MODE
********************************************************************************/
#include <btkcstdio.h>
#include <hardware.h>
#include <toolkit_msg.h>
#if OPER_SYS == WINDOWS_32
#include <win32gdi.h>
#include <w32chksysparms.h>
#include  <sysexecmd.h>
#include <state2_proto.h>
#include <cu_msgutil_proto.h>
#include <msgutil.h>
#include <windows_32_protos.h>
#include <ctsyscall_proto.h>
#include <const.h>
#include <ctstrutil_proto.h>
#include <dbg_crash.h>
#include <bindcall.h>
#include <ct_win_syscall_proto.h>
#include <extcall_interfaces.h>
#include <prostandalone_extcall_interface.h>

static void gdi32_restrictions_warning();
#include<ctmisc_cxx_proto.h>

/*******************************************************************************/
/*
 * Function:  gdi32_system_call
 *
 * Desc    :  Bring up a system window (command prompt).
 *
 * Args in :
 *
 * Args out:
 *
 * Returns :
 *
********************************************************************************/
int gdi32_system_call(struct window *window_ptr)
{
  char  termstr[256];
  wchar_t r_term_cmd[K_LINESIZE];
  /*
     if running WIN 95 - do start a shell and to make it wait till it
     terminates
  */
  strcpy(termstr, "cmd");

  if (get_config_term_cmd(r_term_cmd) == TRUE)
  {
       strcat(termstr, " /c ");
       strcat(termstr, str_ret(r_term_cmd));
  }

  msgID_put(msg_ID69);

  if (nt_policies_cmd_is_allowed ())
  {
    
    pro_exec_command(WAIT_AND_PROCESS_EVENTS_MODE, "%s", termstr); /* termstr can contain % sign, we
						 do not want to interpret this */
    /* realize_pal_of_cur_wind(NULL); */
    erprompt();
  }
  else
  {
    gdi32_restrictions_warning();
  }

  return E_NO_ERROR;
}

/*******************************************************************************/
/*
 * Function:  gdi32_edit
 *
 * Desc    :  Bring up the system editor (Notepad) to edit the file.
 *
 * Args in :
 *
 * Args out:
 *
 * Returns :
 *
********************************************************************************/
int gdi32_edit (struct window *window_ptr, wchar_t *fname ,char editor_com[K_LINESIZE])
{
    if (is_ext_editor_allowed())
    {
        pro_exec_command(
            WAIT_AND_PROCESS_EVENTS_MODE,
            "%s %s",
            ((editor_com != NULL && editor_com[0] != NULL_CHAR) ? editor_com : "notepad"),
            str_ret(fname));
    }
    else
    {
        msgID_put(BLOCK_EDITOR_IN_BROWSER);
    }
   /* realize_pal_of_cur_wind(NULL); */
   return E_NO_ERROR;
}

/******************************************************************************\
 * Function: CheckGraphicsSetup
 * Descript: This function interrogates the system to find out if the graphics
 *           capabilities of the machine are up to snuff for Pro/ENGINEER.
 * Args In : NONE
 * Args Out: NONE
 * Return:   TRUE if successful, FALSE if not.  Call W32ChkSysParm_IsErrorSet()
 *           to discover if <Certain> error code is set.  See prototypes in
 *           w32chksysparms.h
\******************************************************************************/
BOOL CheckGraphicsSetup(void) {
  int x_size = 0;
  int y_size = 0;
  int color_depth = 0;
  HWND deskWnd;
  HDC  deskDC;
  int ret;

  W32ChkSysParm_SetError(0);

  ret = FALSE;
  deskWnd = GetDesktopWindow();
  if ( (deskDC = GetDC(deskWnd)) != NULL ) {
    x_size =      GetDeviceCaps(deskDC, HORZRES);
    y_size =      GetDeviceCaps(deskDC, VERTRES);
    color_depth = GetDeviceCaps(deskDC, BITSPIXEL);
    if ( x_size < CHKSYS_MIN_HORZ_SIZE ) {
      W32ChkSysParm_SetError(CHKSYSPARM_LOW_HOR_RES);
      ret = FALSE;
    } else if (y_size < CHKSYS_MIN_VERT_SIZE) {
      W32ChkSysParm_SetError(CHKSYSPARM_LOW_VER_RES);
      ret = FALSE;
    } else if ( color_depth < CHKSYS_MIN_COLOR_BITSPIXEL) {
      W32ChkSysParm_SetError(CHKSYSPARM_LOW_COLORDEPTH);
      ret = FALSE;
    } else {
      ret = TRUE;
    }
  }

  return(ret);
}

static void gdi32_restrictions_warning()
{
  const ProstandaloneExtcallInterface* p_sa_intf = get_prostandalone_extcall_intf();
  if (!EXT_FUNC_IN_INTF(p_sa_intf, ut_info_msg_id_dialog))

  {
    dbg_err_crash("gdi32_restrictions_warning",
                  "ut_info_msg_id_dialog is not bound.\n");
  }

  p_sa_intf->ut_info_msg_id_dialog (WIN32_RESTRICTIONS_TITLE,
                                    WIN32_RESTRICTIONS_TEXT,
                                    NULL, NULL, NULL, NULL, NULL,
                                    NULL, NULL, NULL, NULL, NULL);
}

#endif
