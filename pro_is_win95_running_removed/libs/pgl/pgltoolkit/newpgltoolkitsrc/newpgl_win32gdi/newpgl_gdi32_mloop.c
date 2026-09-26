/*--------------------------------------------------------------------------*\
|
|  Name:  pgl_gdi32_mloop.c
|
|  Module Details:
|
|  Purpose:
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  06-May-98 I-01-   dac    ##1  Extracted from toolkit, removed obsolete code
|  25-Jan-98         dac         button state in keystate
|  17-Apr-98         dac         Alt Key Support
|  25-Apr-98         dac         Pass messages to DefWindProc
|  11-Sep-98         dac         Keypresses handled immediately in EventDrivenP
|  08-Oct-98         dac         Check modifier states
|  28-Oct-98         dac         ipglGdiReleaseCapture
|  15-Dec-98 I-01-28 dac    ##2  Rename funcs
|  05-Jan-99 I-01-28 dac    ##3  Rename DestroyNotify to WMCloseNotify
|                                 Handle DestroyNotify (WM_DESTROY) Events
|  27-Apr-99 I-03-08 CJL    ##4  Made magellan global vars into static vars.
|  14-May-99 I-03-09 dac    ##5  Add Double click
|  23-Jul-99 I-03-12 dac    ##6  Fix for event polling
|  11-Oct-99 I-03-18 dac    ##7  Spaceball
|  18-Oct-99 I-03-18+dac    ##8  Properly ifdef SPACEBALL code
|  11-Dec-99 I-03-24 dac    ##9  PGL_LITE
|  31-Jan-00 J-01-01 dac   ##10  external event queue
|  13-Feb-01 J-01-27 jas   ##11  Use IsDBCSLeadByteEx on Windows
|  27-Jan-01 J-01-26 jmichaud ##12 endif warning
|  06-Jun-01 J-03-02 dac   ##13  Power management
|  03-Jul-01 J-03-02+arm   ##14  NT Link fix: changed to pgli_Gdi32GetInst().
|  20-Aug-01 J-03-06 dac   ##15  new event api
|  21-Aug-01 J-03-06 dac   ##16  pgl toolkit closure
|  05-Oct-01 J-03-08 Chris ##17  Used newpgll_spware.h
|  21-Sep-01 J-03-09 jas   ##18  Removed WINDOWS_95 macro
|  08-Oct-01 J-03-09 dac   ##19  fix double click
|  11-Oct-01 J-03-09 rds   ##20  changed include file spware.h to newpgll_spware.h
|  26-Oct-01 J-03-11 arm   ##21  Implemented immediate_expose logic for performance.
|                    dac          pass events to external func if avail
|  31-Oct-01 J-03-12 dac   ##22  Pass events to external func if flag set
|                    dac         check mod in pgli_Gdi32GetAllMouseMotionEvents
|                    dac         fix spaceball
|  28-Nov-01 J-03-14 wgs   ##23  Removed call to pgli_gfn_adjust_y_coord
|  17-Dec-01 J-03-15 JPE   ##24  WPARAM wParam and LPARAM lParam
|  28-Feb-02 J-03-20 aamm  ##25  Add Mouse Wheel support
|  01-Mar-02         cm          fix mouse wheel focus problems
|  16-May-02 J-03-25 cm    ##26  don't forward wheel events to DefWindProc
|  25-Jun-02 J-03-28 dac   ##27  check window in Gdi32GetAllMouseMotionEvents
|  31-Jan-03 J-03-41 rmi   ##28  Init/properly set power management result arg
|                                Cleanup unneeded code. Add documentation.
|  12-Feb-03 K-01-01 rmi   ##29  Power management logic deleted
|  06-Mar-03 K-01-02 mgs   ##30  Fix resource leak
|  14-May-03 K-01-07 rds   ##31  w_ptr code cleanup
|  19-May-03 K-01-07 jas   ##32  Do not process Pro/E mouse events as legacy
|  04-May-04 K-03-01 PMORK/CHI ##33 Convert string handling calls to i18n_xxx()
|  13-May-04 K-03-01 PMORK/CHI ##34 undoing #33
|  28-Jun-04 K-03-05 rds       ##35 removed dep_struct
|  30-Apr-05 K-03-21 rds       ##36 fixed compilation error
|  07-Jun-05 K-03-29 PPB   ##37   Added 
|                                 pgli_set_external_copy_and_release_queue_func.
|                                 Call pgli_copy_and_rel_this_queue_elt only
|                                 if running PGL without proe.
|  05-Dec-05 K-03-37 TWH   ##38 Remove galaxy hooks
!  20-DEC-05 K-03-38 MTP   ##39 Spaceball support for Windows 64.
!  24-Apr-06 L-01-07 TWH   ##40 use windows unicode wrappers
!  12-Jul-06 L-01-12 ksi   ##41 Unicode compliant changes
|  31-Jan-07 L-01-26 rds   ##42 pgli_GdiWMLButtonUpFunc and pgli_GdiWMLButtonDnFunc
|                               should return TRUE or FALSE. pgli_Gdi32EventDrivenProc 
|                               shouldn't process key event if above functions return FALSE.
|  20-Aug-07 L-01-37 rds   ##43 fixed crash in spacemouse event handling.
|  12-Sep-07 L-01-38 rds   ##44  struct window renamed to _PglGfxWindow
|  04-Jan-10 L-05-14 aamm  ##45 Add pgli_Gdi32EventMMouseProc()
|  10-Dec-10 L-05-37 aamm  ##46 Initialize event.flags variable
|  18-Jan-11 L-05-40 aamm  ##47 Initialize another event.flags variable
|  22-Mar-11 L-05-44 Shturm ##48 Prototyping
|  10-Mar-11 L-05-44 PPB   ##49 Used GetProNotifyMessage and IsProNotifyMessage
|  29-Mar-11 L-05-44 Shturm ##50 Fixed #48
|  22-Nov-11 P-10-16 aamm   ##51 Add RAW_SPACEBALL
|  27-Jan-12 P-10-16 aamm   ##52 Fix link errors for PGL_LITE .dll's
|  08-Feb-12 P-20-01 aamm   ##53 Remove pgl_gdi32_rawstdafx.h
|  12-Mar-12 P-20-01 AC     ##54 Updated for Project 13028358
|  27-Mar-12 P-20-02 LOK    ##55 Move headers at appropriate place
|  14-Jan-14 P-20-46 lli    ##56 Added missing return statements
|  10-Jan-17 P-50-02 rds    ##57 Use DefWindowProc for WM_ERASEBKGND
|  05-May-17 P-50-08 rds    ##58 Respond to WM_PRINTCLIENT message
|  12-Jul-17 P-50-18 rds    ##59 Undo ##57. It caused customer SPR 6792676
|  21-Jul-20 P-80-13 rds    $$1  Copied from pgltoolkitsrc
|  23-Dec-20 P-80-35 rds    $$2  Lint error fixed.
|  09-Jun-21 P-90-13 jas    $$3  Removed ptcime.h
|  15-Jan-24 Q-11-47 jas    $$4  Fixed strings vs wide-string errors
|  24-Sep-25 Q-13-28 DevOps $$5  Fixed incorrect printf arguments
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/
#include <ptc_win32.h>
#include <btkcstdio.h>
#include <hardware.h>
#if OPER_SYS == WINDOWS_32
#include "newpgll_gdi32_windows.h"
#include "newpgll_keys.h"

#include "newpgll_wm_functions.h"
#include "pro_string.h"
#include "dbg_crash.h"
#include "const.h"
#include "newpgll_win32gdi.h"
#include "newpgll_comm_event.h"
#include "languages.h"
#include "newpgll_windmac.h"
#include "bindcall.h"
#include "newpgll_driver_intf.h"
#include <newpgli_mgr.h>
#include <ct_win_syscall_proto.h>
#include <windows_32_protos.h>
#include <ctwcfun_proto.h>

#ifdef SPACEBALL
#include "newpgll_spware.h"

  #ifdef WIN32
   #define OS_WIN32
  #endif
  #if ( PRO_MACHINE == X86E_WIN64 )
    #include "si64.h"
    #include "siapp64.h"
    #define SIBUTTON SI_APP_FIT_BUTTON
  #else
    #include "si.h"
    #define SIBUTTON SI_PICK_BUTTON
  #endif

SiHdl         si_handle = NULL;
SiSpwHandlers si_dispatch_handlers = {0};

void pgli_SbMotionEvent(SiOpenData *oData, SiGetEventData *eData,
                        SiSpwEvent *event, void *uData);
void pgli_SbButtonEvent(SiOpenData *oData, SiGetEventData *eData,
                   SiSpwEvent *event, void *uData);
#endif

#ifdef RAW_SPACEBALL
  #include <pgl_gdi32_rawvirtualkeys.h>
  #ifdef WIN32
   #define OS_WIN32
  #endif
#endif

#ifdef PGL_LITE
#undef RAW_SPACEBALL
#undef SPACEBALL
#undef MAGELLAN
#endif

#ifdef SPACEBALL
  #include <pgl_spaceball.h>
#endif

#ifdef RAW_SPACEBALL
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>

#include "pgl_gdi32_rawdata.h"
#include <pgl_spaceball.h>
#endif

#ifdef MAGELLAN
  #include "win32maglib.h"
#endif /* MAGELLAN */


extern double get3DInputSensitivity();

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif


/* Masks which are not defined, but were dicovered by experiment. */
#define ICONIFY_MASK    (SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_NOCOPYBITS)
#define UNICONIFY_MASK  (SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOCOPYBITS)
#define PGL_KEY_STATE_CHECK 0xFFFF0000

static void pgli_GdiCheckwParamForButtons(WPARAM wParam, int *key_state);

extern void pgli_gdi32_realize_pal_of_cur_wind();
extern _PglGfxWindow *ipgl_create_dummy_window();

static int pgli_grow_free_list();

typedef struct queue_elt_ {
  struct queue_elt_ *next;
  Gdi32Event event;
  } QueueElt;

static QueueElt first_event,*last_event;
static QueueElt free_q_elts;
static int queueSize;
static int maxQueueSize;
#ifdef TWO_BUTTON_MOUSE_HACK
static int lbutton_is_down = FALSE, mbutton_is_down = FALSE;
#endif
static int immediate_expose = TRUE;

/* If we're running PGL with Proe, we will need to add any unprocessed events
 *  to Proe's queue, so that proe callbacks can be processed for the PGL
 *  window and/or picks can passed up through the polling mechanism.  If
 *  we're running PGL with Proe, add_to_external_queue will be non-NULL. (We
 *  could check PGL's queue when checking proe's queue, but then we'd have to
 *  worry about synchronization - i.e. which event came first, one in the
 *  PGL queue or one in the Proe queue.) */
static Function *add_to_external_queue = NULLFN;
static Function *copy_and_rel_external_queue_elt = NULLFN;

Function *pgli_set_external_queue_function(Function *queue_func)
{
  Function *old_func = add_to_external_queue;
  add_to_external_queue = queue_func;
  return(old_func);
}

/* Same logic as above. If we are running PGL with proe copy and release queue
   should be done from Proe's queue */
Function *pgli_set_external_copy_and_release_queue_func(Function *queue_func)
{
  Function *old_func = copy_and_rel_external_queue_elt;
  copy_and_rel_external_queue_elt = queue_func;
  return(old_func);
}

#define MINIMUM_Q_ALLOC 20

static int dont_process_pos_changes = 0;
static int need_to_realize_palette=0;

/* Function to get next event if DUI activated */
typedef int (*WinMsgProc)();
static WinMsgProc GetNextEvent = NULLFN;


/*********************************************************************/
void pgli_Gdi32FlagAllowPosChanges()
/*********************************************************************/
{
  dont_process_pos_changes--;
  if (dont_process_pos_changes < 0)
    dont_process_pos_changes = 0;
}


/*********************************************************************/
void pgli_Gdi32FlagPreventPosChanges()
/*********************************************************************/
{
  dont_process_pos_changes++;
}


/*********************************************************************/
int pgli_Gdi32QueryAllowPosChanges()
/*********************************************************************/
{
  return dont_process_pos_changes;
}

/*
 * Use these functions to avoid realizing our palette more than
 * is necessary.
 */


/*********************************************************************/
void pgli_Gdi32SetNotRealizingPalette()
/*********************************************************************/
{
  need_to_realize_palette=FALSE;
}


/*********************************************************************/
/*
/* Function:  pgli_Gdi32InitEventQueue
/*
/* Desc    :  Create the window class used by gdi32 windows.
/*
/*********************************************************************/
PRO_STATIC int pgli_Gdi32InitEventQueue()
{
  free_q_elts.next = NULL;
  pgli_grow_free_list();
  first_event.next = NULL;
  last_event = NULL;
  queueSize = 0;
  maxQueueSize = 0;
  /*pgli_dump_free_queue();*/
  return TRUE;
}


/*********************************************************************/
void pgli_dump_queue()
/*********************************************************************/
{
  QueueElt *tmp;
  int count = 1;
  tmp = first_event.next;
  while(tmp)
  {
    btk_printf("%d\t Event %p is %d - hWindow %d\n",count,tmp,tmp->event.type,
                                                 (int)tmp->event.hWindow);
    count++;
    tmp = tmp->next;
  }
  btk_printf("\n^^^^^^^^^^^^^^^^^^^^^^^^\n");
}


/*********************************************************************/
void pgli_dump_free_queue()
/*********************************************************************/
{
  QueueElt *tmp;
  int count = 1;
  tmp = free_q_elts.next;
  btk_printf("\n-----FREE--------------\n");
  while(tmp)
  {
    btk_printf("%d\t Free %p\n",count,tmp);
    count++;
    tmp = tmp->next;
  }
  btk_printf("\n^^^^^^^^^^^^^^^^^^^^^^^^\n");
}


/*********************************************************************/
int pgli_add_to_queue(newEvent)
Gdi32Event *newEvent;
/*********************************************************************/
{
  QueueElt *tmp;

  if (!free_q_elts.next)
    pgli_grow_free_list();
  tmp = free_q_elts.next;
  free_q_elts.next = free_q_elts.next->next;
  BYTCPY(&(tmp->event),newEvent,sizeof(Gdi32Event));
  tmp->next = first_event.next;
  if (!last_event)
  {
    last_event = tmp;
  }
  first_event.next = tmp;
  queueSize++;
  if (queueSize > maxQueueSize)
  {
    maxQueueSize = queueSize;
    if (get_run_mode() == 207043)  /* C_Q_SIZE */
      btk_printf("Max Q Size is                           %d\n",maxQueueSize);
  }
  /*pgli_dump_queue();*/
  return TRUE;
}

/*********************************************************************/
int pgli_copy_and_rel_queue_elt(into)
Gdi32Event *into;
/*********************************************************************/
{
  QueueElt *tmp;
  /*pgli_dump_queue();*/

  if (!last_event)
    return FALSE;
  tmp = &first_event;
  while(tmp && tmp->next != last_event)
  {
    tmp = tmp->next;
  }
  if (!tmp)
    return FALSE;
  BYTCPY(into,&(last_event->event),sizeof(Gdi32Event));
  last_event->next = free_q_elts.next;
  free_q_elts.next = last_event;
  if (tmp == &first_event)
  {
    first_event.next = NULL;
    last_event = NULL;
  }
  else
  {
    tmp->next = NULL;
    last_event = tmp;
  }
  queueSize--;
  return TRUE;
}

/* This function removes the last event from the queue if it occurred
 *  in the given window.  TRUE is returned if the event was removed,
 *  else false is returned.
 */
int pgli_remove_window_event_from_queue(HWND hwindow)
{
  QueueElt *elt, *tmp = NULL, *prev = NULL;
  Gdi32Event event;

/*  pgli_dump_queue(); */
  if (!last_event)
    return FALSE;

  elt = first_event.next;
  prev = &first_event;
  while(elt)
    {
    BYTCPY(&event, &(elt->event),sizeof(Gdi32Event));
    if(event.hWindow == hwindow)
      {
      tmp = elt->next;
      elt->next = free_q_elts.next;
      free_q_elts.next = elt;
      if(prev != NULL)
        {
        prev->next = tmp;
        }
      elt = tmp;
      }
    else
      {
      prev = elt;
      elt = elt->next;
      }
    }

/*  pgli_dump_queue(); */
  return(FALSE);
}


/*********************************************************************/
PRO_STATIC int pgli_copy_and_rel_this_queue_elt(Gdi32Event *into,HWND hCont, HWND hCanv, int type)
/*********************************************************************/
{
  QueueElt *finder,*follower,*found,*found_follower;

  /*pgli_dump_queue();*/
  follower = &first_event;
  finder = first_event.next;
  found = NULL;
  found_follower = NULL;
  if(!last_event)
    return FALSE;

  while (finder)
  {
    if (finder->event.type == type && (hCont == (HWND)-1 || hCont==finder->event.hWindow ||
           hCanv==finder->event.hWindow))
    {
      found_follower = follower;
      found = finder;
    }
    follower = finder;
    finder = finder->next;
  }
  if (found)
  {
    BYTCPY(into,&(found->event),sizeof(Gdi32Event));

    if (found == last_event)
    {
      last_event->next = free_q_elts.next;
      free_q_elts.next = last_event;
      if (found_follower == &first_event)
      {
        last_event = NULL;
        first_event.next = NULL;
      }
      else
      {
        found_follower->next = NULL;
        last_event = found_follower;
      }
    }
    else
    {
      found_follower->next = found->next;
      found->next = free_q_elts.next;
      free_q_elts.next = found;
    }
    queueSize--;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*********************************************************************/
int pgli_copy_this_queue_elt(Gdi32Event *into,HWND hCont, HWND hCanv, int type)
/*********************************************************************/
{
  QueueElt *finder,*follower,*found;

  follower = &first_event;
  finder = first_event.next;
  found = NULL;

  while (finder)
  {
    if (finder->event.type == type && (hCont== (HWND)-1 || hCont==finder->event.hWindow
                                       || hCanv == finder->event.hWindow))
      found = finder;
    follower = finder;
    finder = finder->next;
  }
  if (found)
  {
    BYTCPY(into,&(found->event),sizeof(Gdi32Event));
    return TRUE;
  }
  else
    return FALSE;
}


/*********************************************************************/
static int pgli_grow_free_list()
/*********************************************************************/
{
  QueueElt *newElts;
  int i;

  if (!(newElts = (QueueElt *)malloc(MINIMUM_Q_ALLOC*sizeof(QueueElt))))
  {
    btk_printf("Failed to grow event queue: out of memory.\n");
    return FALSE;
  }

  for (i = 0 ; i < (MINIMUM_Q_ALLOC - 1) ; i++)
  {
    newElts[i].next = &(newElts[i+1]);
    /*newElts[i].used = FALSE;*/
  }

  newElts[MINIMUM_Q_ALLOC -1].next = free_q_elts.next;
  free_q_elts.next = newElts;

  return TRUE;
}


/*********************************************************************/
BOOL CALLBACK pgli_gdi32FitChildToParent(HWND hWnd,RECT *clntRectPtr)
/*********************************************************************/
{
  char name[512];
  uGetClassName(hWnd, name, 512);
  /* Resize only the graphics (PGL_xxx) windows.   */
  /* TODO: let the _PglGfxWindow resize itself thus   */
  /* avoiding any unwanted or unnecessary resizes. */
  if (!strncmp(name, "PGL_", 4))
  SetWindowPos(hWnd,HWND_TOP,clntRectPtr->left,clntRectPtr->top,
                       clntRectPtr->right-clntRectPtr->left,clntRectPtr->bottom - clntRectPtr->top,SWP_NOZORDER);
  return TRUE;
}


/*
 * Here, we must convert from the Shift-JIS returned to us by the
 * system, into UNICODE, which we are using everywhere.
 */
/*********************************************************************/
void pgli_Gdi32InputChar(HWND hWnd,WPARAM wParam)
/*********************************************************************/
{
  Gdi32Event  l_event;
  wchar_t unicode_new_char;
  BYTE        szstr[2];

  if ((get_language() == USASCII) || (wParam >= 0x0 && wParam <= 0x7e))
  {
     unicode_new_char = (wchar_t)wParam;
  }
  else if (wParam >= 0xa0 && wParam <= 0xdf)
  {
    szstr[0] = (unsigned char)wParam;
    MultiByteToWideChar(0,MB_PRECOMPOSED,&szstr,1,&unicode_new_char,1);
  }
  else
  {
    szstr[0] = (unsigned char)HIBYTE(wParam);
    szstr[1] = (unsigned char)LOBYTE(wParam);
    MultiByteToWideChar(0,MB_PRECOMPOSED,&szstr,2,&unicode_new_char,1);
  }
  l_event.type=KeyPress;
  l_event.key_state=0;
  l_event.char_in=unicode_new_char;
  l_event.hWindow=hWnd;
  pgli_GdiCheckwParamForButtons(wParam, &l_event.key_state);
  pgli_add_to_queue(&l_event);
}

static void pgli_GdiWMCreateFunc(HWND hWnd)
{
#ifdef SPACEBALL
  SpaceballInit(hWnd, pgli_SbMotionEvent, pgli_SbMotionEvent,
                   pgli_SbButtonEvent);
#endif /* SPACEBALL */
#ifdef MAGELLAN
   PglMagellanInit(hWnd);
#endif /* MAGELLAN */
  return;
}

static int pgli_GdiWMPaintFunc(HWND hWnd, Gdi32Event *l_event)
{
  int ret_val = FALSE;
  PAINTSTRUCT paintStruct;
  HDC         hDC;

  hDC=BeginPaint(hWnd,&paintStruct);
//  if ((windInfo = pgli_Gdi32GetDepStructOf(hWnd)))
//  {
//        if (windInfo->hDC != INVALID_HANDLE_VALUE)
//          FillRect(windInfo->hDC,&(paintStruct.rcPaint),
//               windInfo->dyn_tool_box->hBgBrush);
//  }
  l_event->type=Expose;
  l_event->x=paintStruct.rcPaint.left;
  l_event->y=paintStruct.rcPaint.top;
  l_event->width=paintStruct.rcPaint.right-paintStruct.rcPaint.left;
  l_event->height=paintStruct.rcPaint.bottom-paintStruct.rcPaint.top;
  l_event->hWindow=hWnd;
  if( l_event->width != 0 || l_event->height != 0)
    ret_val = TRUE;
  ReleaseDC(hWnd, hDC);
  EndPaint(hWnd,&paintStruct);

  return(ret_val);
}

static int pgli_GdiWMPrintClientFunc(HWND hWnd, Gdi32Event *l_event) {
  RECT rc;
  int ret_val = FALSE;
  GetClientRect(hWnd, &rc);

  l_event->type=Expose;
  l_event->x=rc.left;
  l_event->y=rc.top;
  l_event->width=rc.right-rc.left;
  l_event->height=rc.bottom-rc.top;
  l_event->hWindow=hWnd;
  if(l_event->width != 0 || l_event->height != 0)
    ret_val = TRUE;

  return(ret_val);
}


/**************************************************************************\
* Function: pgli_GdiCheckwParamForButtons
* Purpose:  Check the wParam field (that came with a message) to see the
*            state of the modifier keys and mouse buttons
* Input:    UINT wParam    - the wParam from the message
*           int *key_state - The state to be stored in the Gdi32Event struct
* Return:   PRO_CHAR_EVENT
\**************************************************************************/
static void pgli_GdiCheckwParamForButtons(WPARAM wParam, int *key_state)
{
  if ( wParam & MK_CONTROL )
      *key_state |= ControlMask;
  if ( wParam & MK_SHIFT )
      *key_state |= ShiftMask;
  if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
      *key_state |= Mod1Mask /* Alt */;
  if ( wParam & MK_LBUTTON )
      *key_state |= Button1Mask;
  if ( wParam & MK_MBUTTON )
      *key_state |= Button2Mask;
  if ( wParam & MK_RBUTTON )
      *key_state |= Button3Mask;
  return;
}

static void pgli_GdiWMLButtonDownFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                Gdi32Event *l_event)
{
  l_event->type = ButtonPress;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  SetFocus(hWnd);
#ifdef TWO_BUTTON_MOUSE_HACK
  if ( wParam & MK_SHIFT )
    {
    l_event->button=Button2;
    mbutton_is_down = TRUE;
    }
  else
    {
    l_event->button=Button1;
    lbutton_is_down = TRUE;
    }
#else
  l_event->button=Button1;
#endif /* TWO_BUTTON_MOUSE_HACK */
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  l_event->hWindow = hWnd;
  SetCapture(hWnd);

  return;
}


static void pgli_GdiWMMouseWheelFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                Gdi32Event *l_event)
{
  int x,y,w;
  POINT pt;

  l_event->type = MouseWheel;
  x = (signed short)LOWORD (lParam);
  y = (signed short)HIWORD (lParam);
  w = (signed short)HIWORD (wParam);
  pt.x = (LONG)x;
  pt.y = (LONG)y;
  ScreenToClient(hWnd, &pt);
  l_event->x = (int)pt.x;
  l_event->y = (int)pt.y;
  l_event->wheel = (int)w;
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  l_event->hWindow = hWnd;

  return;
}


static void pgli_GdiWMLDblClkFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                Gdi32Event *l_event)
{
  l_event->type = ButtonPress;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  SetFocus(hWnd);
#ifdef TWO_BUTTON_MOUSE_HACK
  if ( wParam & MK_SHIFT )
    {
    l_event->button=Button2DblClk;
    mbutton_is_down = TRUE;
    }
  else
    {
    l_event->button=Button1DblClk;
    lbutton_is_down = TRUE;
    }
#else
  l_event->button=Button1DblClk;
#endif  /* TWO_BUTTON_MOUSE_HACK */
  if ( wParam & MK_CONTROL )
      l_event->key_state |= ControlMask;
  if ( wParam & MK_SHIFT )
      l_event->key_state |= ShiftMask;
  if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
      l_event->key_state |= Mod1Mask /* Alt */;
  l_event->hWindow = hWnd;
  SetCapture(hWnd);

  return;
}


static void pgli_GdiWMLButtonUpFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                               Gdi32Event *l_event)
{
  l_event->type = ButtonRelease;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);

#ifdef TWO_BUTTON_MOUSE_HACK
  if ( ((wParam & MK_SHIFT) && (lbutton_is_down == FALSE)) ||
       ((lbutton_is_down == FALSE) && (mbutton_is_down == TRUE)) )
      {
      l_event->button=Button2;
      mbutton_is_down = FALSE;
      }
  else
      {
      l_event->button=Button1;
      lbutton_is_down = FALSE;
      }
#else
  l_event->button=Button1;
#endif /* TWO_BUTTON_MOUSE_HACK */
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  l_event->hWindow = hWnd;
  ReleaseCapture();

  return;
}


static void pgli_GdiWMMButtonDownFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                Gdi32Event *l_event)
{
  l_event->type = ButtonPress;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  l_event->button=Button2;
  SetFocus(hWnd);
#ifdef TWO_BUTTON_MOUSE_HACK
  mbutton_is_down = TRUE;
#endif
  l_event->hWindow = hWnd;
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  SetCapture(hWnd);

  return;
}

static void pgli_GdiWMMDblClkFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                   Gdi32Event *l_event)
{
  l_event->type = ButtonPress;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  l_event->button=Button2DblClk;
  SetFocus(hWnd);
#ifdef TWO_BUTTON_MOUSE_HACK
  mbutton_is_down = TRUE;
#endif
  l_event->hWindow = hWnd;
  if ( wParam & MK_CONTROL )
      l_event->key_state |= ControlMask;
  if ( wParam & MK_SHIFT )
      l_event->key_state |= ShiftMask;
  if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
      l_event->key_state |= Mod1Mask /* Alt */;
  SetCapture(hWnd);

  return;
}

static void pgli_GdiWMMButtonUpFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                               Gdi32Event *l_event)
{
  l_event->type = ButtonRelease;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  l_event->button = Button2;
  l_event->hWindow = hWnd;
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  ReleaseCapture();

  return;
}

static void pgli_GdiWMRButtonDownFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                Gdi32Event *l_event)
{
  l_event->type = ButtonPress;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  l_event->button=Button3;
  l_event->hWindow = hWnd;
  SetFocus(hWnd);
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  SetCapture(hWnd);

  return;
}

static void pgli_GdiWMRDblClkFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                Gdi32Event *l_event)
{
  l_event->type = ButtonPress;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  l_event->button=Button3DblClk;
  l_event->hWindow = hWnd;
  SetFocus(hWnd);
  if ( wParam & MK_CONTROL )
      l_event->key_state |= ControlMask;
  if ( wParam & MK_SHIFT )
      l_event->key_state |= ShiftMask;
  if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
      l_event->key_state |= Mod1Mask /* Alt */;
  SetCapture(hWnd);

  return;
}

static void pgli_GdiWMRButtonUpFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                               Gdi32Event *l_event)
{
  l_event->type = ButtonRelease;
  l_event->x = (signed short)LOWORD (lParam);
  l_event->y = (signed short)HIWORD (lParam);
  l_event->button=Button3;
  l_event->hWindow = hWnd;
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);
  ReleaseCapture();

  return;
}

static int pgli_GdiWMKeyDnFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
                          Gdi32Event *l_event)
{
  l_event->key_state = 0;

  if (GetKeyState(VK_SHIFT) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=ShiftMask;
  if (GetKeyState(VK_CONTROL) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=ControlMask;
  if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Mod1Mask /* Alt */;
  if (GetKeyState(VK_LBUTTON) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Button1Mask;
  if (GetKeyState(VK_MBUTTON) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Button2Mask;
  if (GetKeyState(VK_RBUTTON) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Button3Mask;

  switch(wParam)
  {
    case VK_CONTROL:
      l_event->char_in = LOG_CONTROL_KEY;
      break;
    case VK_SHIFT:
      l_event->char_in = LOG_SHIFT_KEY;
      break;
    case VK_RIGHT:
      l_event->char_in = LOG_ARROW_KEY_RIGHT;
      break;
    case VK_LEFT:
      l_event->char_in = LOG_ARROW_KEY_LEFT;
      break;
    case VK_UP:
      l_event->char_in = LOG_ARROW_KEY_UP;
      break;
    case VK_DOWN:
      l_event->char_in = LOG_ARROW_KEY_DOWN;
      break;
    case VK_HOME:
      DefWindowProc(hWnd,message,wParam,lParam);
      return FALSE;
      break;
    case VK_END:
      DefWindowProc(hWnd,message,wParam,lParam);
      return FALSE;
      break;
    case VK_INSERT:
      DefWindowProc(hWnd,message,wParam,lParam);
      return FALSE;
      break;
    case VK_DELETE:
      l_event->char_in = LOG_DELETE_KEY;
      break;
    case VK_F1:
      l_event->char_in = PRO_F1;
      break;
    case VK_F2:
      l_event->char_in = PRO_F2;
      break;
    case VK_F3:
      l_event->char_in = PRO_F3;
      break;
    case VK_F4:
      l_event->char_in = PRO_F4;
      break;
    case VK_F5:
      l_event->char_in = PRO_F5;
      break;
    case VK_F6:
      l_event->char_in = PRO_F6;
      break;
    case VK_F7:
      l_event->char_in = PRO_F7;
      break;
    case VK_F8:
      l_event->char_in = PRO_F8;
      break;
    case VK_F9:
      l_event->char_in = PRO_F9;
      break;
    case VK_F10:
      l_event->char_in = PRO_F10;
      break;
    case VK_F11:
      l_event->char_in = PRO_F11;
      break;
    case VK_F12:
      l_event->char_in = PRO_F12;
      break;
    case VK_PRIOR:
      l_event->char_in = LOG_PREVIOUS_SCREEN_KEY;
      break;
    case VK_NEXT:
      l_event->char_in = LOG_NEXT_SCREEN_KEY;
      break;
    case VK_ESCAPE:
      /* Pass the VK_ESCAPE keydown to the DefWindowProc - it will generate
       *  a WM_CHAR event - we can assign LOG_ESC_KEY in pgli_Gdi32WMCharFunc */
    default:
      DefWindowProc(hWnd,message,wParam,lParam);
      return(FALSE);
      break;
    }

  l_event->hWindow=hWnd;
  l_event->type=KeyPress;

  return(TRUE);
}

static int pgli_GdiWMKeyUpFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
                         Gdi32Event *l_event)
{
  l_event->key_state = 0;

  if (GetKeyState(VK_SHIFT) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=ShiftMask;
  if (GetKeyState(VK_CONTROL) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=ControlMask;
  if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Mod1Mask /* Alt */;
  if (GetKeyState(VK_LBUTTON) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Button1Mask;
  if (GetKeyState(VK_MBUTTON) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Button2Mask;
  if (GetKeyState(VK_RBUTTON) & PGL_KEY_STATE_CHECK)
    l_event->key_state|=Button3Mask;
  switch(wParam)
  {
    case VK_CONTROL:
      l_event->char_in = LOG_CONTROL_KEY;
      break;
    case VK_SHIFT:
      l_event->char_in = LOG_SHIFT_KEY;
      break;
    case VK_RIGHT:
      l_event->char_in = LOG_ARROW_KEY_RIGHT;
      break;
    case VK_LEFT:
      l_event->char_in = LOG_ARROW_KEY_LEFT;
      break;
    case VK_UP:
      l_event->char_in = LOG_ARROW_KEY_UP;
      break;
    case VK_DOWN:
      l_event->char_in = LOG_ARROW_KEY_DOWN;
      break;
    case VK_HOME:
      DefWindowProc(hWnd,message,wParam,lParam);
      return FALSE;
      break;
    case VK_END:
      DefWindowProc(hWnd,message,wParam,lParam);
      return FALSE;
      break;
    case VK_INSERT:
      DefWindowProc(hWnd,message,wParam,lParam);
      return FALSE;
      break;
    case VK_DELETE:
      l_event->char_in = LOG_DELETE_KEY;
      break;
    case VK_F1:
      l_event->char_in = PRO_F1;
      break;
    case VK_F2:
      l_event->char_in = PRO_F2;
      break;
    case VK_F3:
      l_event->char_in = PRO_F3;
      break;
    case VK_F4:
      l_event->char_in = PRO_F4;
      break;
    case VK_F5:
      l_event->char_in = PRO_F5;
      break;
    case VK_F6:
      l_event->char_in = PRO_F6;
      break;
    case VK_F7:
      l_event->char_in = PRO_F7;
      break;
    case VK_F8:
      l_event->char_in = PRO_F8;
      break;
    case VK_F9:
      l_event->char_in = PRO_F9;
      break;
    case VK_F10:
      l_event->char_in = PRO_F10;
      break;
    case VK_F11:
      l_event->char_in = PRO_F11;
      break;
    case VK_F12:
      l_event->char_in = PRO_F12;
      break;
    case VK_PRIOR:
      l_event->char_in = LOG_PREVIOUS_SCREEN_KEY;
      break;
    case VK_NEXT:
      l_event->char_in = LOG_NEXT_SCREEN_KEY;
      break;
    case VK_ESCAPE:
      l_event->char_in = LOG_ESC_KEY;
      break;
    default:
      DefWindowProc(hWnd,message,wParam,lParam);
      return(FALSE);
      break;
    }

  l_event->hWindow=hWnd;
  l_event->type=KeyRelease;

  return(TRUE);
}

static void pgli_Gdi32WMMouseMoveFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                 Gdi32Event *l_event)
{
  static HWND lasthWnd = -1;
  l_event->type = MotionNotify;
  l_event->x = (signed short)LOWORD(lParam);
  l_event->y = (signed short)HIWORD(lParam);
  l_event->hWindow = hWnd;
  pgli_GdiCheckwParamForButtons(wParam, &l_event->key_state);

  if(hWnd != lasthWnd)
    {
    int window;
    PglWindow pgl_window;
    lasthWnd = hWnd;
    if(pgli_Gdi32FindWindow(hWnd, &window, NULL, &pgl_window))
      PglFocusChanged(pgl_window);
    }

  return;
}

static void pgli_Gdi32WMCharFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                 Gdi32Event *l_event)
{
   l_event->type=KeyPress;
   l_event->key_state=0;

   if (GetKeyState(VK_SHIFT) & PGL_KEY_STATE_CHECK)
     l_event->key_state|=ShiftMask;
   if (GetKeyState(VK_CONTROL) & PGL_KEY_STATE_CHECK)
     l_event->key_state|=ControlMask;
   if (GetKeyState(VK_MENU) & PGL_KEY_STATE_CHECK)
     l_event->key_state|=Mod1Mask /* Alt */;

   if (GetKeyState(VK_LBUTTON) & PGL_KEY_STATE_CHECK)
     l_event->key_state|=Button1Mask;
   if (GetKeyState(VK_MBUTTON) & PGL_KEY_STATE_CHECK)
     l_event->key_state|=Button2Mask;
   if (GetKeyState(VK_RBUTTON) & PGL_KEY_STATE_CHECK)
     l_event->key_state|=Button3Mask;

   l_event->char_in=(wchar_t)wParam;
   if(l_event->char_in == ESCAPE)
      l_event->char_in = LOG_ESC_KEY;
   l_event->hWindow=hWnd;

   return;
}

static LONG pgli_Gdi32WMNchittestFunc(HWND hWnd, UINT message, WPARAM wParam,
                                 LPARAM lParam)
{
  LONG nchittest_area;

  nchittest_area = DefWindowProc(hWnd,message,wParam,lParam);
  pgli_Gdi32SetAllowZOrderChange(nchittest_area != HTCLIENT?TRUE:FALSE);
  return nchittest_area;
}

static void pgli_Gdi32WMPosChangingFunc(HWND hWnd, LPARAM lParam)
{
  WINDOWPOS *posStruct;

  if (!pgli_Gdi32GetAllowZOrderChange())
    {
    posStruct = (WINDOWPOS *)lParam;
    posStruct->flags |=SWP_NOZORDER;
    return;
    }
  pgli_Gdi32SetAllowZOrderChange(FALSE);

  return;
}

static int pgli_Gdi32WMDestroyFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                   Gdi32Event *l_event)
{
  HWND hWindow;
  Event event;
  l_event->type = DestroyNotify;
  l_event->key_state = 0;
  l_event->hWindow = hWnd;
  /*pgli_add_to_queue(l_event);*/

  pgli_Gdi32DestroyNotify(l_event, &hWindow, &event);

  return(E_NO_ERROR);
}

static int pgli_Gdi32WMPosChangeFunc(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                 Gdi32Event *l_event)
{
  WINDOWPLACEMENT w_info;
  WINDOWPOS *posStruct;
  int window, ret_val = FALSE;
  _PglGfxWindow *w_ptr;

  if (!dont_process_pos_changes)
    {
    w_info.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hWnd, &w_info);

    posStruct = (WINDOWPOS *)lParam;
    /* Windows NT iconify check */
    if (((posStruct->flags & ICONIFY_MASK) == ICONIFY_MASK) &&
        ((w_info.showCmd == SW_MINIMIZE) ||
         (w_info.showCmd == SW_SHOWMINIMIZED)))
      {
      l_event->type = UnmapNotify;
      l_event->hWindow = hWnd;
      ret_val = TRUE;
      }
    /* Windows NT uniconify check */
    else if ((posStruct->flags & UNICONIFY_MASK) == UNICONIFY_MASK)
      {
      l_event->type = MapNotify;
      l_event->hWindow = hWnd;
      ret_val = TRUE;
      }
    else if ((posStruct->flags & (SWP_NOSIZE|SWP_NOMOVE)) != (SWP_NOSIZE|SWP_NOMOVE))
      {
      RECT clntRect;

      GetWindowRect(hWnd,&clntRect);
      l_event->type = ConfigureNotify;
      l_event->hWindow = hWnd;
      l_event->x = clntRect.left;
      l_event->y = clntRect.top;
      l_event->width = clntRect.right - clntRect.left;
      l_event->height = clntRect.bottom - clntRect.top;
      if (pgli_Gdi32FindWindow(hWnd,&window,&w_ptr, NULL))
        {
        if(w_ptr->parent_ptr != NULL)
          {
          /* If this window has a parent, get the position relative to
           *  That parent.  If there's no parent, the position is relative
           *  to the screen.
           */
          GetWindowRect(w_ptr->parent_ptr, &clntRect);
          l_event->x = l_event->x - clntRect.left;
          l_event->y = l_event->y - clntRect.top;
          }
        }
/*
 * If we are a container window, then we must resize our child(ren).
 */
      if (!(posStruct->flags & SWP_NOSIZE))
      {
        GetClientRect(hWnd,&clntRect);
        pgli_Gdi32FlagPreventPosChanges();
        EnumChildWindows(hWnd,(WNDENUMPROC)pgli_gdi32FitChildToParent,(LPARAM)&clntRect);
        pgli_Gdi32FlagAllowPosChanges();
        ret_val = TRUE;
      }
      else
        {
        if (pgli_Gdi32FindWindow(hWnd,&window,&w_ptr, NULL))
          {
#if 0
          GetWindowRect(hWnd, &((WINDOW_DEP_STRUCTURE(w_ptr))->CanvRect));
#endif
          }
        }
      }
    }
  return(ret_val);
}

HWND hWndFromPglWindow(PglWindow pgl_window)
{
  _PglWindow *p_pgl_window = (_PglWindow *)pgl_window;
  _Pgl_DE_Window_GDI *p_de_window_gdi = (_Pgl_DE_Window_GDI *)p_pgl_window->p_de_window;

  return(p_de_window_gdi->hCanvas);
}

LONG APIENTRY pgli_Gdi32EventDrivenProc(
  HWND hWnd,      /* window handle         */
  UINT message,      /* type of message         */
  WPARAM wParam,      /* additional information       */
  LPARAM lParam)      /* additional information       */
{
#ifdef PGL_LITE
      return(DefWindowProc(hWnd, message, wParam, lParam));
#else
  Gdi32Event l_event;
  Event event;
  int status =0, window, tmp_button, tmp_pos[2], process = FALSE;
  int use_for_poll = TRUE;
  double d_pos[2];
  HWND tmp_hwnd;
  _PglGfxWindow *w_ptr;
  MINMAXINFO *lpmmi;
static int wm_paint_cnt = 0;
static int output = 0;
  int pass_to_wind_proc = TRUE;
  char string[80];
  int repeat_count, scan_code, ext_key, context_code, prev_state, transition_state;
  MSG           si_message;
  static HWND last_hwnd = NULL;
  PglBool process_immed = PGL_FALSE;


  l_event.key_state = 0;
  event.type = NULL_DEVICE;
  event.data.mouse.wheel = 0;
  event.keystate = 0;
  event.flags = 0;

  btk_sprintf(string,"message: %x (%d)\n", (int)message, (int)message);
  if(output)
    uOutputDebugString(string);

  switch(message)
    { 	
    case WM_INPUT:
#ifdef RAW_SPACEBALL
      if(get_use_raw_spaceball())
        {
        // WM_INPUT message contains 3D mouse device data
        OnRawInput ((UINT)(GET_RAWINPUT_CODE_WPARAM(wParam))
 	        , (HRAWINPUT)(lParam));
        }
#endif /* RAW_SPACEBALL */
      break;

    case WM_TIMER:
#ifdef RAW_SPACEBALL
      // Add timer handler if 3D mouse polling is enabled
      if (get_use_raw_spaceball() && gRI_bPoll3dmouse)
      {
 	      if (wParam == gRI_3dmouseTimer)
 		      On3dmouseInput();
      }
#endif /* RAW_SPACEBALL */
      break;

    case WM_CREATE:
       pgli_GdiWMCreateFunc(hWnd);
    break;

    case WM_SYSKEYDOWN:
      repeat_count = lParam & 0xFFFF;
      scan_code = (lParam & 0xff0000) >> 16;
      ext_key = (lParam & (1<<24)) >> 24;
      context_code = (lParam & (1<<29)) >> 29;
      prev_state = (lParam & (1<<30)) >> 30;
      transition_state = (lParam & (1<<31)) >> 31;
      return(DefWindowProc(hWnd, message, wParam, lParam));
    break;

    case WM_SYSKEYUP:
      repeat_count = lParam & 0xFFFF;
      scan_code = (lParam & 0xff0000) >> 16;
      ext_key = (lParam & (1<<24)) >> 24;
      context_code = (lParam & (1<<29)) >> 29;
      prev_state = (lParam & (1<<30)) >> 30;
      transition_state = (lParam & (1<<31)) >> 31;
      return(DefWindowProc(hWnd, message, wParam, lParam));
    break;

    case WM_ERASEBKGND:

      return(0L);

#if 0
    /* Enabling following causes weird flashing when window 
       is resized.
    */
    {
       char className[50];
       RECT r;

       GetClassName( hWnd, className, 50);
       GetWindowRect(hWnd, &r);
      
      printf("RDS WM_ERASEBKGND HWND 0x%x Class %s\n", hWnd, className);
      printf("RDS Window Size ltrb (%d, %d) --> (%d, %d)\n", r.left, r.top, r.right, r.bottom);
      return(DefWindowProc(hWnd, message, wParam, lParam));
    }

#endif
     break;

    case WM_ACTIVATE:
#ifdef MAGELLAN
      if ( wParam == WA_ACTIVE )
       {
        PglMagellanSetWindow( hWnd );
       };
#endif /* MAGELLAN */
    break;

    case WM_NCHITTEST:
      return (pgli_Gdi32WMNchittestFunc(hWnd, message, wParam, lParam));
    break;

    case WM_ACTIVATEAPP:
      if (wParam == 1)
        pgli_gdi32_realize_pal_of_cur_wind(hWnd);
    break;

    case WM_PAINT:
    case WM_PRINTCLIENT:
      wm_paint_cnt++;
      if( ((message == WM_PAINT) && pgli_GdiWMPaintFunc(hWnd, &l_event)) ||
          ((message == WM_PRINTCLIENT) && pgli_GdiWMPrintClientFunc(hWnd, &l_event))
        )
      {
        if(hWnd != last_hwnd)
        {
          PglBool redraw_now = TRUE;
          PglWindow pgl_win;

          last_hwnd = hWnd;
          /* Get the PglWindow from the hWnd, and the exposure handler flag
           * Get the GLOBAL state of the immediate expose condition from
           * the function "nt_GetImmediateExpose().  This flag can be set by
           * external applications in the cases where the app wishes to
           * override the "delay redisplay".
           ***   See detailed comments in nt_SetImmediateExpose().
           */
          if (pgli_Gdi32FindWindow(hWnd, &window, &w_ptr, &pgl_win))
            PglWindowGetEventHandlerFlag(pgl_win, &redraw_now);
          immediate_expose = redraw_now | nt_GetImmediateExpose();
        }
        if(immediate_expose)
        {
          pgli_Gdi32Expose(&l_event, NULL);
          if(get_run_mode() == -17)
            btk_printf("WM_PAINT EVENT PROCESSED.  %d not processed.\n", wm_paint_cnt);

          wm_paint_cnt = 0;
          return(0L);
        }
        else
          process = TRUE;

        wm_paint_cnt = 0;
      }
    break;

    case WM_MOUSEWHEEL:
      pgli_GdiWMMouseWheelFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32MouseWheel(&l_event, &tmp_hwnd, tmp_pos, &event);
      process = TRUE;
      /* For some events it gives acceptable results to send the event on
         to the default handler. However, for wheel events this results
         in the PGL processing the event multiple times. Set this flag so
         that if the PGL uses the wheel event, it doesn't pass it on. */
      pass_to_wind_proc = FALSE;
      use_for_poll = FALSE;
      break;

    case WM_LBUTTONDBLCLK:
      pgli_GdiWMLDblClkFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_LBUTTONDOWN:
      pgli_GdiWMLButtonDownFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_LBUTTONUP:
      pgli_GdiWMLButtonUpFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonRelease(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_MBUTTONDBLCLK:
      pgli_GdiWMMDblClkFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_MBUTTONDOWN:
      pgli_GdiWMMButtonDownFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_MBUTTONUP:
      pgli_GdiWMMButtonUpFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonRelease(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_RBUTTONDBLCLK:
      pgli_GdiWMRDblClkFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_RBUTTONDOWN:
      pgli_GdiWMRButtonDownFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_RBUTTONUP:
      pgli_GdiWMRButtonUpFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonRelease(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_MOUSEMOVE:
      pgli_Gdi32WMMouseMoveFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32MotionNotify(&l_event, &tmp_hwnd, tmp_pos, &event);
      process = TRUE;
      use_for_poll = FALSE;
    break;

    case WM_CHAR:
      pgli_Gdi32WMCharFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32KeyPress(&l_event, &hWnd, NULL, &l_event.char_in,
                    &event);
      process = TRUE;
    break;

    case WM_KEYUP:
      if(pgli_GdiWMKeyUpFunc(hWnd, message, wParam, lParam, &l_event))
      {
         pgli_Gdi32KeyPress(&l_event, &hWnd, NULL, &l_event.char_in,
                       &event);
         process = TRUE;
      }
    break;

    case WM_KEYDOWN:
      if(pgli_GdiWMKeyDnFunc(hWnd, message, wParam, lParam, &l_event))
      {
         pgli_Gdi32KeyPress(&l_event, &hWnd, NULL, &l_event.char_in,
                       &event);
         process = TRUE;
      }
    break;

    case WM_QUIT:
      uOutputDebugString("WM_QUIT\n");
    break;

    case WM_CLOSE:
      uOutputDebugString("WM_CLOSE\n");
      event.type = D_WIN_CLOSE;
      event.keystate = 0;
      tmp_hwnd = hWnd;
      process = TRUE;
      use_for_poll = FALSE;
      process_immed = PGL_TRUE;
    break;

    case WM_DESTROY:
      pgli_Gdi32WMDestroyFunc(hWnd, wParam, lParam, &l_event);
    break;

    case WM_QUERYNEWPALETTE:
      pgli_gdi32_realize_pal_of_cur_wind(hWnd);
      return(1L);
    break;

    case WM_PALETTECHANGED:
    break;

    case WM_WINDOWPOSCHANGING:
      pgli_Gdi32WMPosChangingFunc(hWnd, lParam);
      break;

    case WM_WINDOWPOSCHANGED:
      if(pgli_Gdi32WMPosChangeFunc(hWnd, wParam, lParam, &l_event))
        {
        HWND hWindow;
        switch(l_event.type)
          {
          case MapNotify:
            pgli_Gdi32MapNotify(&l_event, &hWindow);
          break;
          case UnmapNotify:
            pgli_Gdi32UnmapNotify(&l_event, &hWindow);
          break;
          case ConfigureNotify:
            pgli_Gdi32ConfigureNotify(&l_event, &hWindow);
          break;
          default:
          break;
          }
        }
    break;


    case WM_DESTROYCLIPBOARD:
      break;

    case WM_MOVE:
    break;

    case WM_SETFOCUS:
    break;

    case WM_GETMINMAXINFO:
      lpmmi = (LPMINMAXINFO) lParam;
      lpmmi->ptMinTrackSize.x = 1;
      lpmmi->ptMinTrackSize.y = 1;
      return(0L);
    break;

    default:
#ifdef SPACEBALL
      si_message.hwnd    = hWnd;
      si_message.message = message;
      si_message.lParam  = lParam;
      si_message.wParam  = wParam;

      if (SpaceballProcessMessage(&si_message))
        return (DefWindowProc(hWnd, message, wParam, lParam));
#endif
#ifdef MAGELLAN
      {
        int type;

        type = MagellanProcessMessage(hWnd, message, wParam, lParam, &l_event);

        if(type)
        {
           if(Gdi32CreateMagellanEvent(type, l_event.button,
                                    (int *)l_event.client_data, &event))
           {
              process = TRUE;
           }
           break;
        }
      }
#endif /* MAGELLAN */

      return(DefWindowProc(hWnd, message, wParam, lParam));
    break;
    }

  if(process == TRUE)
    {
    PglWindow pgl_window;
    if(pgli_Gdi32FindWindow(hWnd, &window, &w_ptr, &pgl_window))
      {
      d_pos[0] = tmp_pos[0]; d_pos[1] = tmp_pos[1];
      if(!process_immed)
        PglWindowGetEventHandlerFlag(pgl_window, &process_immed);
      if((add_to_external_queue != NULLFN) && (!process_immed))
        {
          if(use_for_poll)
            (*add_to_external_queue)(&l_event);
        }
      else
        {
        event.window = window;
        status = pgli_process_event(&event);
        if((status == 0) && use_for_poll)
          {
/* If we're running PGL with Proe, we will need to add any unprocessed events
 *  to Proe's queue, so that proe callbacks can be processed for the PGL
 *  window and/or picks can passed up through the polling mechanism.  If
 *  we're running PGL with Proe, add_to_external_queue will be non-NULL. (We
 *  could check PGL's queue when checking proe's queue, but then we'd have to
 *  worry about synchronization - i.e. which event came first, one in the
 *  PGL queue or one in the Proe queue.)
 * If we're not running PGL with Proe, add any unprocessed events to PGL's
 *  queue, so that a PGL Polling app can poll for events.
 */
          if(add_to_external_queue != NULLFN)
            (*add_to_external_queue)(&l_event);
          else
            pgli_add_to_queue(&l_event);
          }
        }
      if(status && (pass_to_wind_proc == FALSE))
        return 0;
      }
    }

  return DefWindowProc (hWnd, message, wParam, lParam);
#endif
}


int pgli_Gdi32EventMMouseProc(
  HWND hWnd,      /* window handle         */
  UINT message,      /* type of message         */
  WPARAM wParam,      /* additional information       */
  LPARAM lParam)      /* additional information       */
{
  Gdi32Event l_event;
  Event event;
  int status, window, tmp_button, tmp_pos[2], process = FALSE;
  int use_for_poll = TRUE;
  HWND tmp_hwnd;
  _PglGfxWindow *w_ptr;
  char string[80];
  static int output = 0;

  l_event.key_state = 0;
  event.type = NULL_DEVICE;
  event.data.mouse.wheel = 0;
  event.keystate = 0;
  event.flags = 0;

  btk_sprintf(string,"message: %x (%d)\n", (int)message, (int)message);
  if(output)
    uOutputDebugString(string);

  switch(message)
    {
    case WM_MOUSEWHEEL:
      pgli_GdiWMMouseWheelFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32MouseWheel(&l_event, &tmp_hwnd, tmp_pos, &event);
      process = TRUE;
      break;

    case WM_MBUTTONDOWN:
      pgli_GdiWMMButtonDownFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonPress(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
    break;

    case WM_MBUTTONUP:
      pgli_GdiWMMButtonUpFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32ButtonRelease(&l_event, &tmp_hwnd, tmp_pos, &tmp_button, &event);
      process = TRUE;
    break;

    case WM_MOUSEMOVE:
      pgli_Gdi32WMMouseMoveFunc(hWnd, wParam, lParam, &l_event);
      pgli_Gdi32MotionNotify(&l_event, &tmp_hwnd, tmp_pos, &event);
      process = TRUE;
    break;

    }

 if(process == TRUE)
    {
    PglWindow pgl_window;

    if(pgli_Gdi32FindWindow(hWnd, &window, &w_ptr, &pgl_window))
      {
        event.window = window;
        status = pgli_process_event(&event);
        return status;
      }
    }

  return 0;
}

#ifdef SPACEBALL
void pgli_SbMotionEvent(SiOpenData *oData, SiGetEventData *eData,
                        SiSpwEvent *event, void *uData)
{
   float fdata[6];
   Event pgl_event;
   int i, win_id;
   _PglGfxWindow *w_ptr;

   for (i=0; i<6; i++)
      fdata[i] = (float)event->u.spwData.mData[i];

   if(create_spaceball_event(-1, fdata, &pgl_event))
     {
     if(pgli_Gdi32FindWindow((HWND)uData, &win_id, &w_ptr, NULL))
       {
       ipgl3dInputDeviceDispatchCallbacks(w_ptr->pgl_window, &pgl_event);
       }
     }
  return;
}

void pgli_SbButtonEvent(SiOpenData *oData, SiGetEventData *eData,
                        SiSpwEvent *event, void *uData)
{
   Event pgl_event;
   int button = SiButtonPressed(event), win_id;
   _PglGfxWindow *w_ptr;

   if (SiButtonPressed(event))
      SpaceballBeep("cC");

   if (button == SIBUTTON)
      button = 9;

   if(create_spaceball_event(button, NULL, &pgl_event))
     {
     if(pgli_Gdi32FindWindow((HWND)uData, &win_id, &w_ptr, NULL))
       {
       ipgl3dInputDeviceDispatchCallbacks(w_ptr->pgl_window, &pgl_event);
       }
     }
}
#endif /* SPACEBALL */

#ifdef RAW_SPACEBALL

static  _PglWindow *current_sb_pgl_window = NULL;

void clear_current_sb_pgl_window(void)
{
	current_sb_pgl_window = NULL;
}

int is_current_sb_pgl_window(void)
{
    return(current_sb_pgl_window != NULL);
}

/*------------------------------------------------------------------------
 *
 *  Function:  RawSbMotionEvent
 *
 *  Description:
 *    Called from Win23RawInput   Set up Spaceball motion
 *------------------------------------------------------------------------*/

void pgli_RawSbMotionEvent(float fdata[6], void *wData)
{
   Event pgl_event;

   if (!get_use_raw_spaceball() || current_sb_pgl_window == NULL)
	   return;

   if(create_raw_spaceball_event(-1, fdata, &pgl_event))
     {
       ipgl3dInputDeviceDispatchCallbacks(current_sb_pgl_window, &pgl_event);
     }
  return;
}

/*---------------------------------------------------------------------------
 *
 *  Function:  RawSbButtonEvent
 *
 *  Description:
 *    Called from Win23RawInput when a Spaceball button is pressed or released
 *
 *---------------------------------------------------------------------------*/

void pgli_RawSbButtonEvent(int idown, int idata, void *wData)
{
   Event pgl_event;
   int button = -1;

   if (!get_use_raw_spaceball() || current_sb_pgl_window == NULL)
	   return;
   
   switch (idata)
	{
	case V3DK_MENU:
		// Reserved. Activate a pop-up menu with 3D mouse specific settings.
		// See the Standard 3D Mouse SDK for more information on how to implement this.
		break;

	case V3DK_FIT:
		// Fit and reset center of rotation to center of volume. Do a "Zoom Extents"
		break;

	case V3DK_1:
		// Programmable function key 
		break;	

	case V3DK_2:
		// Programmable function key 
		break;

	case V3DK_3:
		// Programmable function key 
		break;

	case V3DK_4:
		// Programmable function key 
		break;

	case V3DK_5:
		// Programmable function key 
		break;

	case V3DK_6:
		// Programmable function key 
		break;

	case V3DK_7:
		// Programmable function key 
        button = 7;
		break;

	case V3DK_8:
		// Programmable function key 
		break;

	case V3DK_9:
		// Programmable function key 
		break;

	case V3DK_10:
		// Programmable function key 
		break;

	case V3DK_TOP:
		// Pre-defined view key 
		break;

	case V3DK_LEFT:
		// Pre-defined view key 
		break;

	case V3DK_RIGHT:
		// Pre-defined view key 
		break;

	case V3DK_FRONT:
		// Pre-defined view key 
		break;

	case V3DK_BOTTOM:
		// Pre-defined view key 
		break;

	case V3DK_BACK:
		// Pre-defined view key 
		break;

	case V3DK_ROLL_CW:
		// Pre-defined view key 
		break;

	case V3DK_ROLL_CCW:
		// Pre-defined view key 
		break;

	case V3DK_ISO1:
		// Pre-defined view key 
		break;

	case V3DK_ISO2:
		// Pre-defined view key 
		break;

	case V3DK_PLUS:
 	    button = 5;
		break;

	case V3DK_MINUS:
        button = 4;
		break;

	case V3DK_ROTATE:
 	    button = 2;
		break;

	case V3DK_PANZOOM:
        button = 1;
		break;

	case V3DK_DOMINANT:
 	    button = 3;
		break;

	}

   if(create_raw_spaceball_event(button, NULL, &pgl_event))
     {
      ipgl3dInputDeviceDispatchCallbacks(current_sb_pgl_window, &pgl_event);
     }
   return;
}

#endif /* SPACEBALL */

int pgli_SbFocusChanged(PglWindow win)
{
  _PglWindow *p_window = (_PglWindow *)win;
  _Pgl_DE_Window_GDI *p_de_window_gdi = (_Pgl_DE_Window_GDI *)p_window->p_de_window;
#ifdef RAW_SPACEBALL
  if (get_use_raw_spaceball())
    {
    current_sb_pgl_window = (_PglWindow *)win;
    InitializeRawInput(p_de_window_gdi->hCanvas);
    return(E_NO_ERROR);
    }
#endif
#ifdef SPACEBALL
  SpaceballSetWindow(p_de_window_gdi->hCanvas, pgli_SbMotionEvent,
                     pgli_SbMotionEvent, pgli_SbButtonEvent);
#endif
#ifdef MAGELLAN
  PglMagellanSetWindow(p_de_window_gdi->hCanvas);
#endif
  return(E_NO_ERROR);
}

/*********************************************************************
 Use this function instead of GetMessage() because GetMessage
 will return wide-char or multi-byte depending on the compile flags.
 By localizing we have more control of the behavior.
*********************************************************************/
BOOL pgli_LocalGetMessage(lpmsg, hwnd, uMsgFilterMin, uMsgFilterMax)
LPMSG lpmsg;
HWND hwnd;
UINT uMsgFilterMin;
UINT uMsgFilterMax;
/*********************************************************************/
{
   return(GetMessage(lpmsg, hwnd, uMsgFilterMin, uMsgFilterMax));
}

/*--------------------------------------------------------------------------*\
| Function: pgli_GetUnicodeMessage
| Purpose:  Convert ANSI WM_CHAR messages to UNICODE WM_CHAR messages
| Input:    message     - a pointer to the message structure to convert
| Output:   message     - a pointer to the converted message structure
| Return:   TRUE if the message was converted to UNICODE, otherwise FALSE
\*--------------------------------------------------------------------------*/
BOOL pgli_GetUnicodeMessage (MSG *message)
{
    static BYTE multi_bytes [2] = { 0, 0 };
    int         count = 1;
    UINT        code_page;
    WCHAR       wide_char;
    BOOL        status = TRUE;

    switch (message->message)
    {
        case WM_CHAR:
        {
            code_page = lang_get_code_page (-1);

            if (multi_bytes [0] == (BYTE) 0)
            {
                multi_bytes [0] = (BYTE) message->wParam;

                if (IsDBCSLeadByteEx (code_page, multi_bytes [0]))
                {
                    status = FALSE;
                    break;
                }
            }
            else
            {
                multi_bytes [1] = (BYTE) message->wParam;
                count++;
            }

            MultiByteToWideChar (code_page, MB_PRECOMPOSED,
                                 multi_bytes, count,
                                 &wide_char, 1);

            message->wParam = (WPARAM) wide_char;

            multi_bytes [0] = multi_bytes [1] = (BYTE) 0;

            break;
        }
    }

    return (status);
}


pgli_IsProeWindowMessage(MSG *pMsg)
{
  int mouse_pos[2], window, menu, item, position[2];

  return pgli_Gdi32FindWindow(pMsg->hwnd, &window, NULL, NULL);
}

int pgli_ProeWindowProcessMessage(MSG *pMsg)
{
  int handled = FALSE;

  if (pgli_IsProeWindowMessage(pMsg))
  {
    TranslateMessage(pMsg);
    DispatchMessage(pMsg);

    handled = TRUE;
  }

  return handled;
}

static void pgli_gdi32ProcessEventsWOutGalaxy(void)
{
  MSG msg;
  int processMessage = TRUE;

  GetMessageA(&msg, NULL, 0, 0);

  processMessage = pgli_GetUnicodeMessage(&msg);

  if (processMessage)
  {
#ifdef SPACEBALL
    if (!SpaceballProcessMessage(&msg) &&
        !pgli_ProeWindowProcessMessage(&msg))
#else
    if (!pgli_ProeWindowProcessMessage(&msg))
#endif
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}


void pgli_Gdi32NextEvent(Gdi32Event *pEvent)
{
  while (!pgli_copy_and_rel_queue_elt(pEvent))
  {
    pgli_gdi32ProcessEventsWOutGalaxy();
  }
}

/* Same as pgli_Gdi32GetTypedWindowEvent, but leaves other
   events in the message queue. This is necessary to have the same behaviour
   of NT and X. When running a Proe trail, we leave all events
   alone unless it is a mouse pick in a stop sign window. In particular,
   we do not want the comm events to be processed. */
/*********************************************************************/
int pgli_Gdi32GetTypedWindowEvent(HWND hCont,HWND hCanv,long emask,Gdi32Event *ev)
/*********************************************************************/
{
    MSG msg;

    /*
     * Translate the Windows Messages.
     */
    UINT filter_low, filter_high;
    switch (emask)
    {
      case ButtonPress:
        filter_low  = WM_MOUSEFIRST;
        filter_high = WM_MOUSELAST;
        break;

      case KeyPress:
        filter_low  = WM_KEYFIRST;
        filter_high = WM_KEYLAST;
        break;

      case Expose:
        filter_low  = WM_PAINT;
        filter_high = WM_PAINT;
        break;

      default:
        filter_low  = 0;
        filter_high = 0;
        dbg_err_crash("pgli_Gdi32GetTypedWindowEvent", "unknown event mask, tell Piotr or Simon ");
    }


    while(PeekMessage(&msg,hCanv,filter_low,filter_high,PM_REMOVE))
    {
        if (IsProNotifyMessage(msg.message))
        {
            /* should never got here */
            dbg_err_crash("pgli_Gdi32GetTypedWindowEvent",
                        "still handling comm events, tell Piotr or Simon ");
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    /* If we are running PGL with proe copy_and_rel_external_queue_elt should
       not be NULL and should point to function to copy and release queue 
       elements from Proe's queue in gdi32_mloop.c */
    if (copy_and_rel_external_queue_elt == NULL)
      copy_and_rel_external_queue_elt = pgli_copy_and_rel_this_queue_elt;

    if (emask == ButtonPress)
        if(copy_and_rel_external_queue_elt(ev,hCont,hCanv,ButtonPress))
            return TRUE;
    if (emask == KeyPress)
        if(copy_and_rel_external_queue_elt(ev,hCont,hCanv,KeyPress))
            return TRUE;
    if (emask == Expose)
        if(copy_and_rel_external_queue_elt(ev,hCont,hCanv,Expose))
            return TRUE;

    return FALSE;
}

/* This function will get all of the WM_MOUSEMOVE events off of NT's message
 *  queue until it encounters a button event.  If a WM_MOUSEMOVE is taken
 *  off the message queue, ev is updated and true is returned. Otherwise
 *  false is returned.
 * We can't simply take all WM_MOUSEMOVE events off the message queue because
 *  we really only want to process motion events together if they mean the
 *  same thing - motion with the button down can be very different than
 *  motion with the button up.
 */
PRO_STATIC int pgli_Gdi32GetAllMouseMotionEvents(Gdi32Event *ev)
{
  MSG msg;
  int ret_val = FALSE, key_state = 0;

  while(PeekMessage(&msg, 0, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE))
    {
    key_state = 0;
    pgli_GdiCheckwParamForButtons(msg.wParam, &key_state);
#if 0
    if(key_state == ev->key_state)
      uOutputDebugString("Mod Masks match\n");
   else
      uOutputDebugString("Mod Masks DON'T match\n");
#endif
    if((msg.message == WM_MOUSEMOVE) && (key_state == ev->key_state) &&
       (msg.hwnd == ev->hWindow))
      {
      PeekMessage(&msg, 0, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
      ev->x=(signed short)LOWORD(msg.lParam);
      ev->y=(signed short)HIWORD(msg.lParam);
      ret_val = TRUE;
      }
    else
      {
      return(ret_val);
      }
    }
  return(ret_val);
}

/*********************************************************************/
PRO_STATIC int pgli_Gdi32GetTypedEvent(long emask,Gdi32Event *ev)
/*********************************************************************/
{
  MSG msg;
  UINT filter_low, filter_high;
  int ret_val = FALSE;
  int reset_poll = FALSE;

/*
 * Translate the Windows Messages.
 */
    switch (emask)
    {
      case ButtonPress:
        filter_low  = WM_MOUSEFIRST;
        filter_high = WM_MOUSELAST;
        break;

      case KeyPress:
        filter_low  = WM_KEYFIRST;
        filter_high = WM_KEYLAST;
        break;

      case Expose:
        filter_low  = WM_PAINT;
        filter_high = WM_PAINT;
        break;

      case ConfigureNotify:
        filter_low  = WM_WINDOWPOSCHANGED;
        filter_high = WM_WINDOWPOSCHANGED;
        break;

      case MotionNotify:
        filter_low  = WM_MOUSEMOVE;
        filter_high = WM_MOUSEMOVE;
        break;

      default:
        filter_low  = 0;
        filter_high = 0;
/* or alternatively?? :
        filter_high = WM_USER - 1;
*/
        break;
    }

  switch(emask)
     {
     case ButtonPress:
     case KeyPress:
     case Expose:
     case ConfigureNotify:
       while(PeekMessage(&msg,0,filter_low,filter_high,PM_REMOVE))
         {
         if (IsProNotifyMessage(msg.message))
           {
           dbg_err_syserr("pgli_Gdi32GetTypedEvent",
                          "dispatched a comm event - TOO SOON!!");
           }
         else
           {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
           }
         }
       if(pgli_copy_and_rel_this_queue_elt(ev,(HWND)-1,(HWND)-1,emask))
          ret_val = TRUE;
       break;

     case MotionNotify:
       if(PeekMessage(&msg,0,filter_low,filter_high,PM_REMOVE))
         {
         if (IsProNotifyMessage(msg.message))
           {
           dbg_err_syserr("pgli_Gdi32GetTypedEvent",
                          "dispatched a comm event - TOO SOON!!");
           }
         else
           {
           ev->x=(signed short)LOWORD(msg.lParam);
           ev->y=(signed short)HIWORD(msg.lParam);
           ret_val = TRUE;
           }
         }
       break;

     default:
       while(PeekMessage(&msg,0,filter_low,filter_high,PM_REMOVE))
         {
         if (IsProNotifyMessage(msg.message))
           {
           dbg_err_syserr("pgli_Gdi32GetTypedEvent",
                          "dispatched a comm event - TOO SOON!!");
           }
         else
           {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
           }
         }
       ret_val = FALSE;
       break;
     }

  return ret_val;
}


/*****************************************************************************/
int pgli_gdi32_clear_window_exposures(w_ptr)
_PglGfxWindow *w_ptr;
/*****************************************************************************/
{
   int ret = E_NO_ERROR;
   Gdi32Event tmp_event;
   _Pgl_DE_Window_GDI *p_de_window_gdi = (_Pgl_DE_Window_GDI *)w_ptr->pgl_window->p_de_window;

   if (p_de_window_gdi)
   {
      while (pgli_Gdi32GetTypedWindowEvent(p_de_window_gdi->hContainer,
                                      p_de_window_gdi->hCanvas, Expose,
                                      &tmp_event));
   }
   else
      ret = E_ERROR;

   ValidateRect(p_de_window_gdi->hContainer, NULL);
   ValidateRect(p_de_window_gdi->hCanvas, NULL);

   return(ret);
}

PglError ipglGdiReleaseCapture()
{
  ReleaseCapture();
  return(PGL_E_OK);
}

#endif
