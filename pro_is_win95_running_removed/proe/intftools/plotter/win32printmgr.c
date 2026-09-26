/******************************************************************************
  Date       Version    Author       Description
  ----------------------------------------------------------------------------
  27-Mar-97  H-01-29    CJL    $$1 Created.
  13-May-97  H-03-10    CJL    $$2 Fixed various win32 print manager bugs.
  23-Jun-97  H-03-14+   CJL    $$3 Added call to is_win32printmgr_supported()
  14-Oct-97  H-03-25    CJL    $$4 Added win32printmgr_bitmap().
  18-Nov-97  H-03-30    CJL    $$5 Fixed invalid args in win32printmgr_circle.
  10-Mar-98  H-03-40    CJL    $$6 Tesselate arcs for Windows NT print manager.
  16-Jun-98  I-01-11    CJL    $$7 Error message if no default printer set up.
  11-Aug-98  I-01-16    CJL    $$8 Fixed arc precision.
  10-Aug-98  I-01-16    hsu    $$9 Handle window exposures before PrintDlg
  09-Sep-98  I-01-18    CJL   $$10 Changed big_radius_threshold to 2000.
  24-Sep-98  I-01-20    CJL   $$11 Added trail support.
  07-Jan-99  I-01-29    CJL   $$12 Fixed plotting of arcs in print mgr.
  11-May-99  I-03-09    CJL   $$13 Fixed resource leak.
  01-Jun-99  I-03-10    imj   $$14 Fixed identical start and end arc bug
				   Tesselate arcs less than 45 degrees
  27-Jul-99  I-03-12    imj   $$15 Removed useless uninitialized variables
                                   Fixed arc inversion bug with windows 9x
                                   Fixed support for bitmaps in print manager
  24-Aug-99  I-03-13    CJL   $$16 Fixed problem with window popping.
  01-Oct-99  I-03-17    guy   $$17 Used pole_obj_draw().
  23-Nov-99  I-03-22    imj   $$18 Properly sets landscape vs. portrait
  01-Feb-00  J-01-01    imj   $$19 Switched to MM_ENGLISH mapping
                                   Fixed arc drawing bug
                                   Force stroking lines on Windows 9x
                                   Fixed clipping bug.
  01-Mar-99  J-01-04    guy   $$20 Added pole_plot().
  22-Mar-00  J-01-05    jhk   $$21 Added get_hardclip_margin() call
  19-Jun-00  J-03-01    Nina  $$22 Proimage is now opaque
  02-Jul-01  J-03-03    arm   $$23 Get correct HWND for PGL implementation
  10-Jul-01             arm        Add win32printmgr_get_hDC().
  12-Jul-01  J-03-03    arm   $$24 Fix compile error on NT.
  12-Jul-01  J-03-04    arm   $$25 Implemented Polyline draw, added BEVEL_JOIN
                                   to pen style definition (not migrate from J01).
  15-Aug-01  J-03-06    HMR   $$26 Removed unused FILE* variable.
  21-Sep-01  J-03-09    jas   $$27 Removed WINDOWS_95 macro
  15-Apr-02  J-03-23    TWH   $$28 check graphics_type in win32printmgr_ui
  12-Jun-03  K-01-17    arm   $$29 Don't override printer default for shd print
  24-Oct-03             arm        Honor the "Print to file" flag from PrintDlg
  26-Mar-04  K-01-26    TWH        Add win32printmgr_proe_papersize
  26-Mar-04  K-01-26    TWH   $$30 Subfunctioned off parts of win32printmgr_ui
  01-Jun-04  K-03-02+   TWH   $$31 Non-ansi prototypes for plot_table funcs
  06-Jul-04  K-03-05    MAZ   $$32 Set rasterimage = noop
  16-Aug-08  K-03-08    MAZ/TWH $$33 Add win32printmgr_rasterimage
  23-Aug-04  K-03-09    TWH   $$34 Add win32printmgr_adjustrasterimage
  07-Sep-04  K-03-10    TWH   $$35 1090767 init hDevNames from default
  07-Oct-04  K-03-11+   rmi   $$36 Change image export scaling
  28-Oct-04  K-03-14    TWH   $$37 win32printmgr_setup_paper() in non-graphics
  21-Jan-05  K-03-18   Chris  $$38 Code quality changes
  01-Feb-05  K-03-19    rmi   $$39 Args added to image export
  21-Mar-05  K-03-21    YYS   $$40 Removed pole_plot()
  05-Apr-05  K-03-22    MAZ        SPR 1124870 & 1124871: Fix call to Polyline 
  05-Apr-05  K-03-22    MAZ   $$41 SPR 1124868 & 1124869: Fix call to Polygon
  31-May-05  K-03-25    TWH   $$42 Debug code for raster images
  13-Jul-05  K-03-28    TWH   $$43 Rm win32printmgr_adjustrasterimage
                                   modify win32printmgr_rasterimage
  14-Jul-05  K-03-29    MAZ   $$44 SPR 1151092: tesselate arcs for MSPM if t0 
                                   is zero or almost zero
  10-Aug-05  K-03-31    TWH   $$45 Add PD_USEDEVMODECOPIESANDCOLLATE
  13-Sep-05  K-03-32    YYS   $$46 Pro_called call to pole_obj_draw 
  23-Nov-05  K-03-37    TWH   $$47 1100257 fix win32printmgr_arc
  24-Apr-06  L-01-07    TWH   $$48 use windows unicode wrappers
  12-Jul-06  L-01-12    ksi   $$49 Unicode compliant changes
  03-Nov-06  L-01-20    TWH   $$50 1296153 fix 1151092 MSPM bug again 
  24-Jan-07  L-01-25    JJE   $$51 1090168 see details below
  14-Jun-07  L-01-33    YMM   $$52 Rolled back #51, See detail below 
  19-Jul-07  L-01-35    AAK   $$53 Made MSPrintMgr functions PGL apis of 
                                   ADV type
  06-Mar-08  L-03-04    rds   $$54 Code Cleanup.
  11-Feb-08  L-03-04    YMM   $$55 Changed win32printmgr_symbol to win32printmgr_put_text that 
                                   takes Pro_text, and uses TTF
  23-Mar-08  L-03-05    YER   $$56 Included genutil1_proto.h
  17-May-08  L-03-09    YMM   $$57 support TTF rotation
  16-May-08  L-03-09    BI    $$58 Used syswindows.h instead of windows.h
  25-Sep-08  L-03-19    Esti  $$59 Modified win32printmgr_put_text to use 
                                   printlib_win_printmgr_put_text
  06-Nov-08  L-03-19+   Shay  $$60 Initialize di.lpszDatatype in win32printmgr_start_document
  19-Nov-08  L-03-21    Esti  $$61 Modify win32printmgr_ui using CU_PRO_CALL_DECLARE
  08-Jul-09  L-05-02    GHJ   $$62 Added appdata to win32printmgr_rasterimage()
  05-Oct-09  L-05-07    SCL   $$63 Use text session font ids.
  16-Nov-09  L-05-10    YMM   $$64 Reverted #14 tesselation test. This test is not reauired.
  07-Apr-10  L-05-20    Esti  $$65 stroke PTC symbols which are not TTF supported
  14-Apr-10  L-05-21    Esti  $$66 added TTF path and handle validation
  25-Jan-11  L-05-41    YMM   $$67 In put_text, concider text angle when calc position
  18-Mar-11  L-05-44    AMAG  $$68 call PglPrinterSetFillPage from win32printmgr_rasterimage 
  14-Mar-11  L-05-44    Esti  $$69 Moved PrintDlg call to secondary thread
  10-Apr-11  L-05-45    Esti  $$70 Fixed return status for PrintDlg on secondary thread 
  30-Mar-11  L-05-45    AMAG  $$71 Removed PglPrinterSetMultiPass call from 
                                   win32printmgr_rasterimage 
  09-Jun-11  P-10-01    pbhodsale $$72 In put_text, corrected the position of text,
                                  depending on angle
  13-Jan-12 P-10-16     pbhosale $$73 added support for muliple fill polygon
  12-Mar-12 P-20-01     AC       $$74 Updated for Project 13028358
  16-Apr-12 P-20-03     nkadam   $$75 Added fix for spr 2117595 and 2108086
  05-Jul-12 P-20-09     nkadam   $$76 Reverted ARC_TESSELATE_ANGLE with lesser value as 0.1
                                      for spr2133584,so that old spr 1893364 is also fixed. 

  12-Aug-12  P-20-12    nkadam $$77 Removed extra hard_clip margins SPR 2103813. 
  06-Nov-12  P-20-17    AC     $$78 Prototype compliance.
  10-Jun-13  P-20-32    NAK    $$79 Fixed spr 2166467.
  29-Oct-13  P-20-41    NAK    $$80 Fixed spr 2171213.
  29-May-14  P-20-55    NAK    $$81 Added plot_ms_print_offset config option.
  03-Sep-14  P-20-60    svenkata $$82 Fix for SPR 2223482 in win32printmgr_arc
  09-Jan-15  P-20-64    vbhatt  $$83 Fix for Windows Arc bug in win32printmgr_arc
  14-Sep-15  P-30-16    avanam       Corrected text alignment/height for windows print manger. 
  23-Sep-15  P-30-16    svenkata $$84  Added uit_object_wait_execute_func_CALL
  09-Jan-15  P-20-64    vbhatt  $$85 Fix for Windows Arc bug in win32printmgr_arc
  08-Sep-15  P-30-16	vbhatt       Fix for SPR 2852691 in win32printmgr_arc.
  08-Sep-15  P-30-16	avanam  $$86 Corrected text alignment/height for windows print manger. 
  16-Feb-17  P-50-11    nkadam  $$87 Corrected last fix for Snagit viewer.
  23-Jul-17  P-50-20    Asaf    $$88 Fixed error on void func returning value - C4098  
  11-Apr-18  P-50-49    AVR     $$89 Stroke symbol in annot text.
  19-Nov-18  P-60-25    AVR     $$90 Corrected ##83.
  13-Mar-19  P-70-01    DevOps  $$91 http://go.ptc.com/asan
  25-Jul-21  P-90-19    DevOps  $$92 Changed return type of some functions to void
  23-Sep-23  Q-11-33    AKA     $$93 Used interface for calling drawing/dim/2D plot
                                     stuff from proe/pro; fixed some lint warnings.
  10-Dec-23  Q-11-43    AKA     $$94 Used interface for calling more xtop funcs.
  15-Jul-24  Q-12-21    DevOps  $$95 Use standard wide string functions
  10-Apr-25  Q-13-04    PKHOT   $$96 Added win32printmgr_get_color_supported() func.
  11-Jul-25  Q-13-17    AVR     $$97 Fixed default printer/plotter selection in MS print manager.
  24-Sep-25  Q-13-28    DevOps  $$98 Fixed incorrect printf arguments
  09-Mar-26  Q-27-00    PROTO   $$99 Automatic prototype creation
******************************************************************************/

#include <ptc_win32.h>
#include <btkcstdio.h>
#include <hardware.h>
#include <plot_intf.h>
#include <window_proto.h>
#include <utility3_proto.h>
#include <plotter_proto.h>
#if OPER_SYS == WINDOWS_32

static short win32printmgr_nt_papersize(short orig_nt_papersize, int proe_papersize);
static int win32printmgr_proe_papersize(short orig_nt_papersize, int * proe_papersize);
static UINT win32printmgr_print_hook_proc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
static int win32printmgr_set_color_supported(int val);
static int win32printmgr_begin_path(void);
static int win32printmgr_end_path(void);


/* INCLUDES */
#include <winspool.h>
#include <syswindows.h>
#include <sysstdio.h>
#include <errors.h>
#include <const.h>
#include <newcons.h>
#include <mathcons.h>
#include <pro_widec.h>
#include <plotstruct.h>
#include <view_str.h>
#include <sysmath.h>
#include <intftools_msg.h>
#include <pro_ole.h>
#include <pgl_appl.h>
#include <stroketxt.h>
#include <mkscpy.h>
#include <genutil1_proto.h>
#include <cu_pro_bind_macros.h>
#include <threadlib.h>
#include <uit_wait_object.h>
#include <runmode.h>
#include <dbg_crash.h>
#include <ctfileutil_proto.h>
#include <bindcall.h>
#include <ct_win_syscall_proto.h>
#include <cu_math_proto.h>
#include <cu_msgutil_proto.h>
#include <graphics_proto.h>
#include <plot_option.h>
#include <extcall_interfaces.h>
#include <proepro_extcall_interface.h>

/* EXTERNS */


/* DECLARATIONS */

/* STATICS */
static HWND cur_hwnd;
static PRINTDLG pd;
static double dpi = 0.0;
static int cur_penstyle = PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_BEVEL;
static int cur_penwidth = 1;
static LOGBRUSH cur_brush;
static int cur_penstylecount = 0;
static int *cur_penlinepattern = NULL;
static HPEN cur_hPen = INVALID_HANDLE_VALUE;
static HPEN orig_hPen = INVALID_HANDLE_VALUE;
static HBRUSH cur_hBrush = INVALID_HANDLE_VALUE;
static HBRUSH orig_hBrush = INVALID_HANDLE_VALUE;
static HRGN cur_hRgn = INVALID_HANDLE_VALUE;
static int win32printmgr_color_supported = FALSE;
static int begin_path = 0;
static int add_default_print_offset = TRUE;

typedef wchar_t* (*uit_object_wait_execute_FUNC_TYPE)(unsigned int duration, int *function, void * data1, void *data2);


/*****************************************************************************/
static int pole_obj_draw_pro_call( void* dc )
{
  const ProeproExtcallInterface* p_pro_intf = get_proepro_extcall_intf();
  return R_EXT_CALL_VIA_INTF(p_pro_intf, pole_cur_model_draw_all, -1, dc);
}


PRO_STATIC int uit_object_wait_execute_func_CALL(unsigned int duration, int *function, void * data1, void *data2)
{
  int status =0;
  static uit_object_wait_execute_FUNC_TYPE uit_object_wait_execute_func = (uit_object_wait_execute_FUNC_TYPE)NULL;

  if (uit_object_wait_execute_func  == (uit_object_wait_execute_FUNC_TYPE)NULL)
  {
    uit_object_wait_execute_func = (uit_object_wait_execute_FUNC_TYPE)pro_call("uit_object_wait_execute");
  }
  if (uit_object_wait_execute_func  != (uit_object_wait_execute_FUNC_TYPE)NULL)
  {
    status =
       (*uit_object_wait_execute_func)(duration, function, NULL, NULL) != NULL;
  }
 return status;
}

Bool GetPrinterDevice(char* pszPrinterName, HANDLE* phDevNames, HANDLE* phDevMode)
{
  // if NULL is passed, then assume we are setting app object's
  // devmode and devnames
  if (phDevMode == NULL || phDevNames == NULL)
      return FALSE;
  
  // Open printer
  HANDLE hPrinter;
  if (OpenPrinter(pszPrinterName, &hPrinter, NULL) == FALSE)
      return FALSE;
  
  // obtain PRINTER_INFO_2 structure and close printer
  DWORD dwBytesReturned, dwBytesNeeded;
  GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
  PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwBytesNeeded);
  
  if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded, &dwBytesReturned) == 0)
  {
     GlobalFree(p2);
     ClosePrinter(hPrinter);
     return FALSE;
  }
  ClosePrinter(hPrinter);
  
  // Allocate a global handle for DEVMODE
  HANDLE  hDevMode = GlobalAlloc(GHND, sizeof(*p2->pDevMode) + p2->pDevMode->dmDriverExtra);
  DEVMODE* pDevMode = (DEVMODE*)GlobalLock(hDevMode);
  
  // copy DEVMODE data from PRINTER_INFO_2::pDevMode
  memcpy(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode) + p2->pDevMode->dmDriverExtra);
  GlobalUnlock(hDevMode);
  
  // Compute size of DEVNAMES structure from PRINTER_INFO_2's data
  DWORD drvNameLen = lstrlen(p2->pDriverName)+1;  // driver name
  DWORD ptrNameLen = lstrlen(p2->pPrinterName)+1; // printer name
  DWORD porNameLen = lstrlen(p2->pPortName)+1;    // port name
  
  // Allocate a global handle big enough to hold DEVNAMES.
  HANDLE hDevNames = GlobalAlloc(GHND, sizeof(DEVNAMES) + (drvNameLen + ptrNameLen + porNameLen)*sizeof(TCHAR));
  DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);
  
  int tcOffset = sizeof(DEVNAMES)/sizeof(TCHAR);
  
  pDevNames->wDriverOffset = tcOffset;
  memcpy((char*)pDevNames + tcOffset, p2->pDriverName, drvNameLen*sizeof(TCHAR));
  tcOffset += drvNameLen;
  
  pDevNames->wDeviceOffset = tcOffset;
  memcpy((char*)pDevNames + tcOffset, p2->pPrinterName, ptrNameLen*sizeof(TCHAR));
  tcOffset += ptrNameLen;
  
  pDevNames->wOutputOffset = tcOffset;
  memcpy((char*)pDevNames + tcOffset, p2->pPortName, porNameLen*sizeof(TCHAR));
  pDevNames->wDefault = 0;
  
  GlobalUnlock(hDevNames);
  GlobalFree(p2); // free PRINTER_INFO_2
  
  // set the new hDevMode and hDevNames
  *phDevMode = hDevMode;
  *phDevNames = hDevNames;
  return TRUE;
}

HANDLE CopyHandle(HANDLE h)
{
  // Return a handle to a copy of the data
  // that the passed handle was for.
  if (!h) return NULL;
  
  BYTE* lpCopy;
  BYTE* lp;
  HANDLE hCopy;
  DWORD dwLen = GlobalSize(h);
  if (hCopy = GlobalAlloc(GHND, dwLen))
  {
    lpCopy = (BYTE*)GlobalLock(hCopy);
    lp     = (BYTE*)GlobalLock(h);
    
    CopyMemory(lpCopy,lp,dwLen);
    
    GlobalUnlock(hCopy);
    GlobalUnlock(h);
  }
  return hCopy;
}

/*****************************************************************************/
int win32printmgr_setup_pd_struct(int init_from_default)
{
   LPDEVMODE lpDevMode = NULL;
   LPDEVNAMES lpDevNames = NULL;
   int nError;

   if (!is_win32printmgr_supported())
      return(E_ERROR);

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_setup_pd_struct\n");

   cur_hwnd = NULL;  /* TODO: Tie this to a parent window for PGL */

   memset((void *) &pd, 0, sizeof(PRINTDLG));
   pd.lStructSize = sizeof(PRINTDLG);
   pd.hwndOwner = cur_hwnd;
   pd.Flags = PD_RETURNDC|PD_NOPAGENUMS|PD_NOSELECTION|PD_ENABLEPRINTHOOK;
   pd.lpfnPrintHook = (LPPRINTHOOKPROC)win32printmgr_print_hook_proc;
   pd.hInstance = (HANDLE)NULL;
   pd.hDevMode = NULL;
   pd.hDevNames = NULL;

   if (init_from_default)
   {
     PRINTDLG pd_default;
     /* Get default printer settings. */
     memset ((void *) &pd_default, 0, sizeof(PRINTDLG));
     pd_default.lStructSize = sizeof(PRINTDLG);
     pd_default.hwndOwner = cur_hwnd;
     pd_default.Flags = PD_RETURNDEFAULT | PD_NOWARNING;
     pd_default.hInstance = (HANDLE)NULL;
     
     nError = PrintDlg(&pd_default);
     if (nError == FALSE)
     {
       msgID_put(WIN32PMGR_ERR_NO_DEFAULT);
       if (get_run_mode() == 7647)
         btk_printf("PrintDlg Error1: [%X]\n", CommDlgExtendedError());
       return(E_ERROR);
     }
     GlobalFree(pd.hDevNames);
     pd.hDevNames = pd_default.hDevNames;
     pd.hDevMode = pd_default.hDevMode;
   }
   else
   {
     HANDLE hhDevMode;
     HANDLE hhDevNames;
     char plotter_name[K_PATH_SIZE];
	 
     wstrtos(plotter_name, get_default_ms_plotter());
     GetPrinterDevice(&plotter_name, &hhDevNames, &hhDevMode);
	 
     pd.hDevMode = CopyHandle(hhDevMode);
     pd.hDevNames = CopyHandle(hhDevNames);
   }

   return (E_NO_ERROR);
}
/*****************************************************************************/

static int PrintDlg_inWorkerThread(void* arg1, void* arg2)
{
  int nError = PrintDlg(&pd);
  return nError;
}

/*****************************************************************************/
int win32printmgr_setup_hDC()
{
  LPDEVMODE lpDevMode = NULL;
  LPDEVNAMES lpDevNames = NULL;
  SizeInfo *paper_size = NULL;

  if (get_input_mode() == GRAPHICS_INPUT &&
      get_graphics_type() != NO_GRAPHICS)
  {
   
    /* Spr #2055100 : As per action queue bloating up faster in L05,
                      PrintDlg() can no longer be invoked 
                      on the main thread, as it opens a modal event loop
                      and prevents Proe to do message processing */
    int nError = uit_object_wait_execute_func_CALL(0U, PrintDlg_inWorkerThread, NULL, NULL);

    if (nError == FALSE)
    {
      if (get_run_mode() == 7647)
        btk_printf("PrintDlg Error2: [%X]\n", CommDlgExtendedError());
      return (E_ERROR);
    }

  }
  else
  {
    lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
    lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);

    pd.hDC = uCreateDC(&((char *)lpDevNames)[lpDevNames->wDriverOffset],
                     lpDevMode->dmDeviceName, NULL, lpDevMode);

    GlobalUnlock(pd.hDevNames);
    GlobalUnlock(pd.hDevMode);
    if (pd.hDC == NULL)
      return (E_ERROR);
  }
  paper_size = &(get_plot_option_data(PO_PAPER_SIZE)->size);
  if (paper_size != NULL)
  {
    lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
    paper_size->id = win32printmgr_proe_papersize(lpDevMode->dmPaperSize, &(paper_size->id));
    GlobalUnlock(pd.hDevMode);
  }
  return (E_NO_ERROR);
}

/*****************************************************************************/
/* setup windows pd print struct with paper info from Pro/e data structs     */
/*****************************************************************************/
int win32printmgr_setup_paper()
{
   LPDEVMODE lpDevMode = NULL;
   SizeInfo *paper_size = NULL;

   if (pd.hDevMode == NULL)
     return (E_ERROR);

   paper_size = &(get_plot_option_data(PO_PAPER_SIZE)->size);
   if (paper_size == NULL)
     return (E_ERROR);

   lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);

   if ((paper_size->width) > (paper_size->height))
     lpDevMode->dmOrientation = DMORIENT_LANDSCAPE;
   else
     lpDevMode->dmOrientation = DMORIENT_PORTRAIT;

   if (! (lpDevMode->dmFields & DM_ORIENTATION) )
     lpDevMode->dmFields |= DM_ORIENTATION;

   lpDevMode->dmPaperSize =
     win32printmgr_nt_papersize(lpDevMode->dmPaperSize, paper_size->id);

   if (! (lpDevMode->dmFields & DM_PAPERSIZE) )
     lpDevMode->dmFields |= DM_PAPERSIZE;

   GlobalUnlock(pd.hDevMode);
   return (E_NO_ERROR);
}

/*****************************************************************************/
void win32printmgr_cleanup_resources()
{
   if (cur_hBrush != INVALID_HANDLE_VALUE)
   {
      SelectObject(pd.hDC, orig_hBrush);
      DeleteObject(cur_hBrush);
      cur_hBrush = INVALID_HANDLE_VALUE;
      orig_hBrush = INVALID_HANDLE_VALUE;
   }

   if (cur_hPen != INVALID_HANDLE_VALUE)
   {
      SelectObject(pd.hDC, orig_hPen);
      DeleteObject(cur_hPen);
      cur_hPen = INVALID_HANDLE_VALUE;
      orig_hPen = INVALID_HANDLE_VALUE;
   }

   if (cur_hRgn != INVALID_HANDLE_VALUE)
   {
      SelectClipRgn(pd.hDC, NULL);
      DeleteObject(cur_hRgn);
      cur_hRgn = INVALID_HANDLE_VALUE;
   }

   GlobalFree(pd.hDevMode);
   GlobalFree(pd.hDevNames);

   DeleteDC(pd.hDC);

   set_big_radius_threshold(0.0);
}


/*****************************************************************************/
void win32printmgr_update_brush()
{
   HBRUSH new_hBrush = INVALID_HANDLE_VALUE;

   new_hBrush = CreateBrushIndirect(&cur_brush);
   SelectObject(pd.hDC, new_hBrush);

   if (cur_hBrush != INVALID_HANDLE_VALUE)
   {
      DeleteObject(cur_hBrush);
   }

   cur_hBrush = new_hBrush;
}


/*****************************************************************************/
void win32printmgr_update_pen()
{
   HPEN new_hPen = INVALID_HANDLE_VALUE;

   new_hPen = ExtCreatePen(cur_penstyle, cur_penwidth, &cur_brush,
                           cur_penstylecount, cur_penlinepattern);
   SelectObject(pd.hDC, new_hPen);

   if (cur_hPen != INVALID_HANDLE_VALUE)
   {
      DeleteObject(cur_hPen);
   }

   cur_hPen = new_hPen;
}


/*****************************************************************************/
int win32printmgr_start_document(docname)
char *docname;
{
   int nError;
   DOCINFOA di;

   /* Initialize the members of a DOCINFO structure. */
   di.cbSize = sizeof(DOCINFOA);
   di.lpszDocName = docname;
   di.lpszOutput = NULL;
   di.fwType = 0;
   di.lpszDatatype = NULL;

   /* Begin a print job by calling the StartDoc function. */
   nError = uStartDoc(pd.hDC, &di);
   if (nError == SP_ERROR)
   {
      if (get_run_mode() == 7647)
         btk_printf("ERROR: StartDoc\n");
      return(0);
   }
   return (1);
}


/*****************************************************************************/
int win32printmgr_end_document()
{
   int nError;

   /* Inform the driver that document has ended. */
   nError = EndDoc(pd.hDC);
   if (nError <= 0)
   {
      if (get_run_mode() == 7647)
         btk_printf("ERROR: EndDoc\n");
      return (0);
   }

   /* Enable the application's window. */
   EnableWindow(cur_hwnd, TRUE);

   win32printmgr_cleanup_resources();
   return (1);
}


/*****************************************************************************/
static short win32printmgr_nt_papersize(short orig_nt_papersize, int proe_papersize)
{
   short nt_papersize = orig_nt_papersize;

   if (get_run_mode() == 7647)
   {
      btk_printf("win32printmgr_nt_papersize: [%d] [%d]\n", orig_nt_papersize,
             proe_papersize);
   }

   switch(proe_papersize)
   {
      case A_SIZE_PLOT:
         nt_papersize = DMPAPER_LETTER;
         break;
      case B_SIZE_PLOT:
         nt_papersize = DMPAPER_11X17;
         break;
      case C_SIZE_PLOT:
         nt_papersize = DMPAPER_CSHEET;
         break;
      case D_SIZE_PLOT:
         nt_papersize = DMPAPER_DSHEET;
         break;
      case E_SIZE_PLOT:
         nt_papersize = DMPAPER_ESHEET;
         break;
      case A4_SIZE_PLOT:
         nt_papersize = DMPAPER_A4;
         break;
      case A3_SIZE_PLOT:
         nt_papersize = DMPAPER_A3;
         break;
      case A2_SIZE_PLOT:
         nt_papersize = DMPAPER_A2;
         break;

#if 0
      /* Could not find equivalent NT paper size values for these. */
      case A1_SIZE_PLOT:
         nt_papersize = DMPAPER_?;
         break;
      case A0_SIZE_PLOT:
         nt_papersize = DMPAPER_?;
         break;
      case F_SIZE_PLOT:
         nt_papersize = DMPAPER_?;
         break;
#endif

      default:
         nt_papersize = orig_nt_papersize;
         break;
   }

   return(nt_papersize);
}

/*****************************************************************************/
static int win32printmgr_proe_papersize(short orig_nt_papersize, int *proe_papersize)
{
   switch(orig_nt_papersize)
   {
      case DMPAPER_LETTER:
         *proe_papersize = A_SIZE_PLOT;
         break;
      case DMPAPER_11X17:
         *proe_papersize = B_SIZE_PLOT;
         break;
      case DMPAPER_CSHEET:
         *proe_papersize = C_SIZE_PLOT;
         break;
      case DMPAPER_DSHEET:
         *proe_papersize = D_SIZE_PLOT;
         break;
      case DMPAPER_ESHEET:
         *proe_papersize = E_SIZE_PLOT;
         break;
      case DMPAPER_A4:
         *proe_papersize = A4_SIZE_PLOT;
         break;
      case DMPAPER_A3:
         *proe_papersize = A3_SIZE_PLOT;
         break;
      case DMPAPER_A2:
         *proe_papersize = A2_SIZE_PLOT;
         break;

#if 0
      /* Could not find equivalent NT paper size values for these. */
      case DMPAPER_?:
         *proe_papersize = A1_SIZE_PLOT;
         break;
      case DMPAPER_?:
         *proe_papersize = A0_SIZE_PLOT;
         break;
      case DMPAPER_?:
         *proe_papersize = F_SIZE_PLOT;
         break;
#endif

      default:
         break;
   }

   if (get_run_mode() == 7647)
   {
      btk_printf("win32printmgr_proe_papersize: [%d] [%d]\n", orig_nt_papersize,
             *proe_papersize);
   }

   return(*proe_papersize);
}

/*****************************************************************************/
static UINT APIENTRY win32printmgr_print_hook_proc(HWND hdlg, UINT uiMsg,
                                            WPARAM wParam, LPARAM lParam)
{
  /* if (uiMsg == WM_EXITSIZEMOVE)
   {
      pro_refresh_proe_graphics();
   }*/

   return(0);
}


/* ************************************************************************ *\
 *                                                                          *
 * Setup the data necessary for printing in Windows. (Paper, pen, brush)    *
 *                                                                          *
 * Input:                                                                   *
 *    plotter_ptr - the plotter info to be used when printing a drawing.    *
 *                                                                          *
 * NOTE: If this is NULL, we are printing a Shaded Image and will           *
 *       use the default printer page setup.                                *
\* ************************************************************************ */
int win32printmgr_ui(Plotter *plotter_ptr)
{
   wchar_t ws[K_PATH_SIZE];
   
   int nError;
   float fLogPelsX1, fLogPelsY1, fLogPelsX2, fLogPelsY2, fScaleX, fScaleY;
   SIZE szMetric;
   double x_dpi, y_dpi;
   LPDEVMODE lpDevMode = NULL;
   LPDEVNAMES lpDevNames = NULL;
   double left_margin=0.0, right_margin=0.0, top_margin=0.0, bottom_margin=0.0;
   int pox, poy, paper_height, paper_width;
   const ProeproExtcallInterface* p_pro_intf = get_proepro_extcall_intf();
   int in_2d_mode = R_EXT_CALL_VIA_INTF(p_pro_intf, in_plot_2d_mode, FALSE);

   if (!is_win32printmgr_supported())
      return(E_ERROR);

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_ui\n");

   if (get_input_mode() == GRAPHICS_INPUT &&
       get_graphics_type() != NO_GRAPHICS )
   {
     HANDLE hPrinter;
     char plotter_name[K_PATH_SIZE];

     wstrtos(plotter_name, get_default_ms_plotter());
     if (OpenPrinter(plotter_name, &hPrinter, NULL))
     {
       if (win32printmgr_setup_pd_struct(FALSE) != E_NO_ERROR)
         return (E_ERROR);
     }
     else
     {
       /* Get windows default printer settings. */
       if (win32printmgr_setup_pd_struct(TRUE) != E_NO_ERROR)
         return (E_ERROR);
     }

      

      /*
       * SPR 900887 - The check for a NULL plotter_ptr was added because
       *              when we printed a shaded image, we were only printing
       *              to a standard 8-1/2 x 11 sheet of paper.  Now, we will
       *              simply use the paper size defined in the Windows printer
       *              itself and not override them with an A-Size sheet.
       *
       * TWH - 1/24/2007 - satisfy above check with in_2d_mode now instead of 
       *              NULL plotter_ptr
       *
	*YMM - 13-JUN-2007
	*There is an inconsistency in printing shaded vs wire-frame models, in
	*that when printing shaded our dialog does NOT list the paper size,
	*So the win32 dialog shows ITS default paper size. 
	*But when printing a wire-frame model, our dialog provides the default
	*paper size and feed the selected paper size to be the default paper size
	*in win32 dlg.
	*This inconsistency is driven from the spec. This is how the Print UI
	*behaves.
	*This inconsistency is recorded in SPR 1090168.
	*The fix ( in K-03-32 ) to avoid setting the paper size for all cases
	*broke the functionality as recorded in SPR 1378546,1366868.
	*That fix was removed and the inconsistency is back in place. ( SPR
	*1090168 was reopened ).
       * 
       */
      if (plotter_ptr != NULL)
      {
        win32printmgr_setup_paper();
      }

      /* Create standard Windows 32 print dialog. */
      pd.Flags |= PD_USEDEVMODECOPIESANDCOLLATE;

      if (win32printmgr_setup_hDC() != E_NO_ERROR)
        return (E_ERROR);

      /*
       * SPR 1044807 - Even though MS PrintMgr does not have "to File" enabled
       *               on dialog, you get it again with the call to PrintDlg().
       *               Store this state in the plot options for later use.
       */
      if (pd.Flags & PD_PRINTTOFILE)
        set_plot_option(PO_PLOT_TO_FILE, TRUE, NULL);
   }
   else
   {
      /* Get default printer settings. */
      if (win32printmgr_setup_pd_struct(TRUE) != E_NO_ERROR)
        return (E_ERROR);

      if (in_2d_mode)
      	win32printmgr_setup_paper();

      /* See SPR 1044807 note above */
      if (pd.Flags & PD_PRINTTOFILE)
        set_plot_option(PO_PLOT_TO_FILE, TRUE, NULL);

      /* Create hDC no print dialog is shown. */
      if (win32printmgr_setup_hDC() != E_NO_ERROR)
        return (E_ERROR);
   }

   lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
   win32printmgr_set_color_supported(lpDevMode->dmColor == DMCOLOR_COLOR);
   GlobalUnlock(pd.hDevMode);

   orig_hPen = GetCurrentObject(pd.hDC, OBJ_PEN);
   orig_hBrush = GetCurrentObject(pd.hDC, OBJ_BRUSH);

   /*
      Retrieve the number of pixels-per-logical-inch in the horizontal and
      vertical directions for the display upon which the bitmap was created. */
   fLogPelsX1 = (float) GetDeviceCaps(pd.hDC, LOGPIXELSX);
   fLogPelsY1 = (float) GetDeviceCaps(pd.hDC, LOGPIXELSY);

   /* Retrieve the number of pixels-per-logical-inch in the horizontal and
      vertical directions for the printer upon which the bitmap will be
      printed. */
   fLogPelsX2 = (float)GetDeviceCaps(pd.hDC, LOGPIXELSX);
   fLogPelsY2 = (float)GetDeviceCaps(pd.hDC, LOGPIXELSY);

   /* Determine the scaling factors required to print the bitmap and retain its
      original proportions. */
   if (fLogPelsX1 > fLogPelsX2)
      fScaleX = (fLogPelsX1 / fLogPelsX2);
   else
      fScaleX = (fLogPelsX2 / fLogPelsX1);

   if (fLogPelsY1 > fLogPelsY2)
      fScaleY = (fLogPelsY1 / fLogPelsY2);
   else
      fScaleY = (fLogPelsY2 / fLogPelsY1);

   x_dpi = fLogPelsX2;
   y_dpi = fLogPelsY2;

   if (x_dpi > y_dpi)
      dpi = x_dpi;
   else
      dpi = y_dpi;

   pox = GetDeviceCaps(pd.hDC, PHYSICALOFFSETX);
   poy = GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);

#if 0
   /* DO NOT REMOVE THIS CODE.  IT IS USED OFTEN FOR DEBUGGING. */
   paper_width = GetDeviceCaps(pd.hDC, PHYSICALWIDTH);
   paper_height = GetDeviceCaps(pd.hDC, PHYSICALHEIGHT);

   printf("pox: [%d]\n", pox);
   printf("poy: [%d]\n", poy);
   printf("paper_width: [%d]\n", paper_width);
   printf("paper_height: [%d]\n", paper_height);
#endif
  
   left_margin = (pox/dpi)*25.4; /* convert to mm */
   top_margin = (poy/dpi)*25.4;  /* convert to mm */

   /* Make sure margin is at least 0.25 inches. */ 
   if (get_ms_print_offset())
   {
   if ((pox/dpi) < 0.25)
   left_margin = 0.25*25.4; /* convert to mm */
   
   if ((poy/dpi) < 0.25)
   top_margin = 0.25*25.4; /* convert to mm */
   }   
   right_margin = left_margin;
   bottom_margin = top_margin;

   if (plotter_ptr != NULL)
   {
      plotter_ptr->margin[0] = (int)left_margin;
      plotter_ptr->margin[1] = (int)right_margin;
      plotter_ptr->margin[2] = (int)top_margin;
      plotter_ptr->margin[3] = (int)bottom_margin;
   }

   set_big_radius_threshold(2000.0);

   return(E_NO_ERROR);
}


/*****************************************************************************/
void win32printmgr_plotter_init(plotter_ptr, paper, plot_name, create_flag)
Plotter *plotter_ptr;
SizeInfo *paper;
int create_flag;
char *plot_name;
{
   int nError;
   int pox, poy, testwidth, testheight;
   int iVertRes;

   if (get_run_mode() == 7647)
   {
      btk_printf("win32printmgr_plotter_init\n");
      btk_printf("dpi: [%g]\n", dpi);
   }

   set_plotter_units_per_inch(1000.0);
   set_device_to_raster_scale(1.0);
   set_plot_userunit_resolution(2000.0);

   /* Initialize all static values. */
   cur_penstyle = PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_BEVEL;

   cur_penwidth = 1;
   cur_penstylecount = 0;
   cur_penlinepattern = NULL;

   if (cur_penlinepattern != NULL)
   {
      rlsmem(cur_penlinepattern);
      cur_penlinepattern = NULL;
   }

   /* Define standard LOGBRUSH structure. */
   cur_brush.lbStyle = BS_SOLID;
   cur_brush.lbColor = RGB(0, 0, 0);
   cur_brush.lbHatch = 0;

   /* Inform the driver that the application is about to begin sending data. */
   nError = StartPage(pd.hDC);
   if (nError <= 0)
   {
      if (get_run_mode() == 7647)
         btk_printf("ERROR: StartPage\n");
      return;
   }

   if (SetMapMode(pd.hDC, MM_HIENGLISH) == 0)
     {
       int iError = GetLastError();
       dbg_err_crash("win32printmgr_plotter_init", "Error calling SetMapMode");
     }

   iVertRes = GetDeviceCaps(pd.hDC, VERTRES);
   SetViewportOrgEx(pd.hDC, 0, iVertRes, NULL);

   win32printmgr_update_brush();
   win32printmgr_update_pen();

   begin_path = 0;

#if 0
   /* DO NOT REMOVE THIS CODE.  IT IS USED OFTEN FOR DEBUGGING. */
   pox = GetDeviceCaps(pd.hDC, PHYSICALOFFSETX);
   poy = GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);
   testwidth  = 6600; /* 600 times 11 */
   testheight = 5100; /* 600 times 8.5 */

   testwidth = testwidth - (pox * 2);
   testheight = testheight - (poy * 2);

   testwidth = testwidth - 1; /* 1 value discovered through experimentation. */
   testheight = testheight - 21; /* 21 value discovered same way. */

   printf("testwidth: [%d]\n", testwidth);
   printf("testheight: [%d]\n", testheight);

   /* test drawing a square */
   MoveToEx(pd.hDC, 150, 150, NULL);
   LineTo(pd.hDC, 0, 0);
   LineTo(pd.hDC, 0, testheight);
   LineTo(pd.hDC, testwidth, testheight);
   LineTo(pd.hDC, testwidth, 0);
   LineTo(pd.hDC, 0, 0);
#endif

   return;
}


/*****************************************************************************/
void win32printmgr_close(plotter_ptr, plot_name)
Plotter *plotter_ptr;
wchar_t *plot_name;
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_close\n");
}


/*-----------------------------------------------------------------------*/
/* Adjust P1 such that P1 becomes the hard clip origin limit for the     */
/* paper loaded. 							 */
/*-----------------------------------------------------------------------*/
static void win32printmgr_adjust_p1p2(double logical_outline[2][3],
	double p1p2_outline[2][3])
{
   double mm_to_inches, f_factor, margins[4];
   Plotter *p_ptr;
   
   int paper_status, i;
   int margin[4];

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_adjust_p1p2\n");

   mm_to_inches = 0.1 / 2.54;
   f_factor = get_plotter_units_per_inch();
   p_ptr = get_plotter_info();
   paper_status = compare_papers(get_plotter_paper_size(),
                                 get_plotter_plot_size());

   get_hardclip_margin(margin, NULL);
   p1p2_outline[1][0] *= f_factor;
   p1p2_outline[1][1] *= f_factor;
   for (i=0; i<4; i++)
      margins[i] = margin[i] * mm_to_inches * f_factor;

   if (paper_status == PCOMP_SMALLER ||
       get_plotter_forced_fit_status() == TRUE)
   { /* Shrink outline into plottable region */
      if (get_run_mode() == 7647)
         btk_printf("PCOMP_SMALLER\n");

      p1p2_outline[1][0] -= (margins[PLOT_LEFT]+margins[PLOT_RIGHT]);
      p1p2_outline[1][1] -= (margins[PLOT_BOTTOM]+margins[PLOT_TOP]);
      return;
   }

   if (paper_status == PCOMP_SAME)
   {
      if (get_run_mode() == 7647)
         btk_printf("PCOMP_SAME\n");

      p1p2_outline[0][0] -= margins[PLOT_LEFT];
      p1p2_outline[0][1] -= margins[PLOT_BOTTOM];
      p1p2_outline[1][0] -= margins[PLOT_LEFT];
      p1p2_outline[1][1] -= margins[PLOT_BOTTOM];

      return;
   }

   if (paper_status == PCOMP_LARGER)
   {
      if (get_run_mode() == 7647)
         btk_printf("PCOMP_LARGER\n");
      return;
   }

   return;
}


/*****************************************************************************/
void win32printmgr_setup(mapped_outline, device_outline)
double mapped_outline[2][3], device_outline[2][3];
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_setup\n");

   win32printmgr_adjust_p1p2(mapped_outline, device_outline);
}


/*****************************************************************************/
void win32printmgr_set_clip(p1, p2)
double p1[3], p2[3];
{
   float f_p1[3], f_p2[3];
   HRGN new_hRgn = INVALID_HANDLE_VALUE;
   POINT pCoordinates[2];

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_set_clip: [%g %g %g] [%g %g %g]\n",
             p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);

   apply_plotter_matrix(p1, f_p1);
   apply_plotter_matrix(p2, f_p2);
   pCoordinates[0].x = (int)(f_p1[0]+0.5);
   pCoordinates[0].y = (int)(f_p1[1]+0.5);
   pCoordinates[1].x = (int)(f_p2[0]+0.5);
   pCoordinates[1].y = (int)(f_p2[1]+0.5);

   LPtoDP(pd.hDC, pCoordinates, 2);

   new_hRgn = CreateRectRgn(pCoordinates[0].x, pCoordinates[0].y,
                            pCoordinates[1].x, pCoordinates[1].y);
   ExtSelectClipRgn(pd.hDC, new_hRgn, RGN_COPY);

   if (cur_hRgn != INVALID_HANDLE_VALUE)
   {
      DeleteObject(cur_hRgn);
   }

   cur_hRgn = new_hRgn;
}


/*****************************************************************************/
void win32printmgr_cancel_clip()
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_cancel_clip\n");

   SelectClipRgn(pd.hDC, NULL);

   if (cur_hRgn != INVALID_HANDLE_VALUE)
   {
      DeleteObject(cur_hRgn);
      cur_hRgn = INVALID_HANDLE_VALUE;
   }
}


/*****************************************************************************/
void win32printmgr_reset_plotter(create_flag)
int create_flag;
{
   int nError;

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_reset_plotter: [%d]\n", create_flag);

   /*
     draws on pd.hDC images of special OLE objects that were 
     created as part of report of curvature analysis
   */
   pole_obj_draw_pro_call( pd.hDC );
   nError = EndPage(pd.hDC);
   if (nError <= 0)
   {
      if (get_run_mode() == 7647)
         btk_printf("ERROR: EndPage\n");
      return;
   }
}


/*****************************************************************************/
void win32printmgr_moveto(x, y)
float x, y;
{
   int xi, yi;

   xi = (int)(x + 0.5);
   yi = (int)(y + 0.5);

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_moveto: [%d] [%d]\n", xi, yi);

   win32printmgr_begin_path();

   MoveToEx(pd.hDC, xi, yi, NULL);
}


/*****************************************************************************/
void win32printmgr_lineto(x, y)
float x, y;
{
   int xi, yi;

   xi = (int)(x + 0.5);
   yi = (int)(y + 0.5);

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_lineto: [%d] [%d]\n", xi, yi);

   win32printmgr_begin_path();

   LineTo(pd.hDC, xi, yi);

   win32printmgr_end_path();
}


/*****************************************************************************/
void win32printmgr_plot(x, y, penstatus)
float *x, *y;
int *penstatus;
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_plot: [%g] [%g] [%d]\n", *x, *y, *penstatus);

   /* Simulate PENUP, PENDOWN pen statuses. */
   switch (*penstatus)
   {
      case PENUP:
         win32printmgr_moveto(*x,*y);
         break;
      case PENDOWN:
         win32printmgr_lineto(*x,*y);
         break;
      default:
         if (get_run_mode() == 7647)
            btk_printf("win32printmgr_plot: Unknown pen status %d.\n", *penstatus);
         break;
   }
}

/*****************************************************************************/
static void win32printmgr_dispose_ttf_font (int session_font_id, HFONT* hf)
{
	wchar_t   font_path_buff[K_PATH_SIZE] = {NULL_WCHAR};
	char*     font_path                   = NULL;

	get_full_ttf_font_path_by_id (session_font_id,font_path_buff); 
	font_path = make_wstostr(font_path_buff);
	printlib_win_printmgr_release_font(font_path, hf);

	relmem(&font_path);
}

/*****************************************************************************/
static void win32printmgr_create_ttf_font (Pro_text* pro_text, double height, HFONT* hf)
{
 
  wchar_t*  font_label_ws = NULL;
  char*     font_label    = NULL;
  wchar_t   font_path_buff[K_PATH_SIZE]  = {NULL_WCHAR};
  char*     font_path     = NULL;
  long      angle         = (long)pro_text->angle;
  	

  /* Font */		
  get_font_label_from_id(pro_text->session_font_id, &font_label_ws);
  font_label = make_wstostr(font_label_ws);

  get_full_ttf_font_path_by_id (pro_text->session_font_id,font_path_buff);
  /* continue handeling only for valied paths */
  if (wstrcmp(font_path_buff,L""))
  {
    font_path = make_wstostr(font_path_buff);

    printlib_win_printmgr_alloc_font(font_path, font_label,(float)height, angle, pro_text->underlined, hf);

    relmem(&font_path);
  }
    relmem(&font_label_ws);
    relmem(&font_label); 
}


/*****************************************************************************/
void win32printmgr_put_text(Pro_text* pro_text, int text_color)
{
  const ProeproExtcallInterface* p_pro_intf = get_proepro_extcall_intf();
  Bool should_stroke = FALSE;

  if (wstrcmp(pro_text->text_value, L"\n") == 0)
    return;
  
  if (wstrchr (pro_text->text_value, 1) != NULL ||
      R_EXT_CALL_VIA_INTF(p_pro_intf, annot_text_contains_symbols, FALSE,
                          pro_text->text_value))
  {
    /* Some PTC symbols are not supported in TTF. 
     * Those symbols starts with ASCII(1) and end with ASCII(2)
     * In case those exists - we need to stroke 
     */
    should_stroke = TRUE;
  }
  /* 
   * Some text features are still not supported in TTF. 
   * If one of them is used - stroke font 
   */
  if (pro_text->slant_angle != 0 ||
      pro_text->thickness != 0 ||
      pro_text->mirrored != 0 ||
      pro_text-> kerning!= 0 
    )
  {
    should_stroke = TRUE;
  }
  if (!should_stroke)
  {	
    double    height	= 0.0;
	double    height_txt	= 0.0;
    HFONT     hf            = NULL;
    float     f_point[3]    = {0};
    double    rgb_color[3]  = {0};
    COLORREF  true_color    = 0;
    int       hold_color    = 0;
    
    apply_plotter_scale (pro_text->height,&height);
	height_txt = height;
    height = get_ttf_font_size(height, pro_text->session_font_id);
    apply_plotter_matrix(pro_text->coord, f_point); 
    if (is_plotter_matrix_rotated())
        pro_text->angle = pro_text->angle + 90.0;


    /* As we have the bottom corner and win32 expect to get to top corner 
     * we have to compensate and to add the components of height on x and Y
     */
	f_point[0] -= (height_txt + (height - height_txt)/2) * sin (deg_to_rad(pro_text->angle));
	f_point[1] += (height_txt + (height - height_txt)) * cos (deg_to_rad(pro_text->angle));
     
    /* Create Font */
    win32printmgr_create_ttf_font(pro_text, height, &hf);
    if (hf != NULL)
    {
      char*     str_text      = make_wstostr(pro_text->text_value); 
      /* Set Text Color */
      hold_color = get_color();
      set_color(text_color);
      set_plotter_from_color();

      if (get_run_mode() == 7647)
	btk_printf("win32printmgr_put_text: [%g] [%g] [%s] \n", f_point[0], f_point[1], str_text);

      printlib_win_printmgr_put_text(pd.hDC, hf, (int)(f_point[0] + 0.5), (int)(f_point[1] + 0.5), str_text);

      set_color(hold_color);

      win32printmgr_dispose_ttf_font(pro_text->session_font_id, &hf);
      relmem(&str_text);

    }
    else
    {
      should_stroke = TRUE;
    }
  }
  if (should_stroke)
  {
    if (get_run_mode() == 7647)
      btk_printf("win32printmgr_put_text: %ws was stroked", pro_text->text_value);
    plotter_stroke_string(pro_text, text_color);
  }

}


/*****************************************************************************/
void win32printmgr_circle(center, radius)
double center[3], radius;
{
   float new_center[3];
   double new_radius;

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_circle: [%g] [%g] [%g] [%g]\n", center[0],
             center[1], center[2], radius);

   if (get_tesselate_arcs() == TRUE)
      return;

   apply_plotter_matrix(center, new_center);
   apply_plotter_scale(radius, &new_radius);

   win32printmgr_begin_path();

   MoveToEx(pd.hDC, (int)(new_center[0]+new_radius+0.5),
            (int)(new_center[1]+0.5), NULL);
   AngleArc(pd.hDC, (int)(new_center[0]+0.5), (int)(new_center[1]+0.5),
            (DWORD)(new_radius+0.5), 0.0, 360.0);

   win32printmgr_end_path();
}


/*****************************************************************************/
void win32printmgr_arc(center, radius, end1, end2, t0, t1)
double center[3], radius, end1[3], end2[3], t0, t1;
{
   double start_x, start_y, end_x, end_y;
   double box_x, box_y, box_width, box_height;
   float new_center[3];
   double new_radius;
   Plotter *p_ptr;
   int old_plotgraph_linestyle;
   double (*ret_array)[][3];
   double scale;
   int retnum;
   int iStartX, iStartY, iEndX, iEndY;
   double tdiff;
   double delta_t;


   #define ARC_TESSELATE_ANGLE 0.7854 /*Value of PI/4 as per Piping standards*/


   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_arc: [%g] [%g] [%g] [%g] [%g] [%g]\n", center[0],
             center[1], center[2], radius, t0, t1);

   /* For SPR 801264 [imj] */
   tdiff = fabs(t1 - t0 - TWOPI);
   delta_t = fabs(t1 - t0);

   if (tdiff < 0.001)
     {
       win32printmgr_circle(center, radius);
       return;
     }

   apply_plotter_matrix(center, new_center);
   apply_plotter_scale(radius, &new_radius);

   box_x = new_center[0] - new_radius;
   box_y = new_center[1] - new_radius;
   box_width = box_height = 2.0 * new_radius;

   
   /* SPR 1151092: tesselate arcs for MSPM if t0 almost zero
      the Windows bug in Arc seems to also happen when 
      t0 is close to 0. Arcs get plotted beyond the end points. 
      we will tesselate arcs in this case too */
   if (((delta_t < ARC_TESSELATE_ANGLE) ||
	   (get_tesselate_arcs() == TRUE) || 
	   (t0 == 0.0) || 
	   (t0 <= ALMOST_ZERO) ||
	   (t0 == PID2) ||
	   ((t0 - PID2) <= ALMOST_ZERO) ||
	   (t0 == PI) ||
	   ((t0 - PI) <= ALMOST_ZERO) ||
	   (t0 == THREE_PID2) ||
	   ((t0 - THREE_PID2) <= ALMOST_ZERO) ||
	   (t0 == TWOPI) ||
	   ((t0 - TWOPI) <= ALMOST_ZERO) ||
           ((tdiff - PI) <= ALMOST_ZERO)) 
	)
     {
       scale = radius / new_radius;

       p_ptr = get_plotter_info();
       if (p_ptr->type == GR_MODEL)
         {
	   old_plotgraph_linestyle = pro_get_line_style();
	   set_line_style(SOLIDFONT, NULL);
	   plot_arc_segments(center, radius, t0, t1);
	   set_line_style(old_plotgraph_linestyle, NULL);
         }
       else
         plot_arc_segments(center, radius, t0, t1);
     }
   else
     {
       start_x = (new_center[0] + new_radius*cos(t0));
       end_x = (new_center[0] + new_radius*cos(t1));
       start_y = (new_center[1] + new_radius * sin(t0));
       end_y = (new_center[1] + new_radius * sin(t1));

       iStartX = (int) (start_x + 0.5);
       iStartY = (int) (start_y + 0.5);
       iEndX = (int) (end_x + 0.5);
       iEndY = (int) (end_y + 0.5);

	/* This is for cases where t1-t0 ~ TWOPI 
         * but not close enough for the test case to call
	 * circle.  (SPR 1100257) 
	 */
       if ((iStartX == iEndX) && (iStartY == iEndY))
         {
           MoveToEx(pd.hDC, iStartX, iStartY, NULL);
           LineTo(pd.hDC, iEndX, iEndY);
           return;
         }

	   if(get_use_smooth_arc())
	   {
		   MoveToEx(pd.hDC, iStartX, iStartY, NULL);
		   AngleArc(pd.hDC, new_center[0], new_center[1], new_radius, 360-(t0*RAD_TO_DEG), (t0*RAD_TO_DEG-t1*RAD_TO_DEG));
	   }
	   else
       Arc(pd.hDC, (int)(box_x+0.5), (int)(box_y+0.5),
           (int)(box_x+box_width+0.5), (int)(box_y+box_height+0.5),
           iStartX, iStartY, iEndX, iEndY);
     }
   #undef ARC_TESSELATE_ANGLE

}


/*****************************************************************************/
void win32printmgr_fillpoly(point_array, numpoints)
double point_array[][3];
int numpoints;
{
   int i, npoints = numpoints;
   POINT points[256];

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_fillpoly: [%d]\n", numpoints);

   /* check for i<256 to avoid memory problems -- MAZ */
   for (i=0; (i < numpoints) && (i < 256); i++)
   {
      points[i].x = (long)(point_array[i][0] + 0.5);
      points[i].y = (long)(point_array[i][1] + 0.5);
   }

   win32printmgr_end_path();
  
   if (npoints == 1)
   {
		/* plot a single point. This will make sense if we end
		up with many single points to plot */
		points[1].x = points[0].x;
		points[1].y = points[0].y;
		npoints = 2;
    }

   /* SPR 1124868 & 1124869: Fix call to Polygon */
   /* PREFIX: numpoints must be >= 2 according to MSDN */
   if (npoints >= 2)
   {
       Polygon(pd.hDC, points, npoints);
   }
}

/*****************************************************************************/
void win32printmgr_fillMultipoly(double point_array[][3], int poly_num_of_vertex_count_list[], int num_polygons)
{
   int i,j,k; 
   int polygon_count=0;
   int polygon_vertex_count=0;
   POINT *points= NULL;
   int num_of_points = 0;
   int cur_point_position = 0;
   int new_point_position = 0;

   /* Calculate total number of points */
   for (j = 0; j < num_polygons; j++)
   {
	   polygon_vertex_count = poly_num_of_vertex_count_list[j];
	   /* if polygon vertex count is 1 then make it to 2 */
	   if (polygon_vertex_count == 1) polygon_vertex_count = 2;
	   num_of_points = num_of_points + polygon_vertex_count;
   }

   /* allocate memory for total number of points */
   points = (POINT*)xar_alloc(0, sizeof(POINT), num_of_points);

   if (get_run_mode() == 7647) 
   {

      btk_printf("win32printmgr_fillMultipoly Number of polygons: [%d]\n", num_polygons);  
      btk_printf("win32printmgr_fillMultipoly Number of points: [%d]\n", num_of_points);

   }
   for (j = 0; j < num_polygons; j++)
   {
	    polygon_vertex_count = poly_num_of_vertex_count_list[j];
		 for(i = cur_point_position, k=new_point_position ; (i < (cur_point_position + polygon_vertex_count)) && (k < (new_point_position + polygon_vertex_count)); i++,k++)
		 { 
			 
			 points[k].x = (long)(point_array[i][0] + 0.5);
			 points[k].y = (long)(point_array[i][1] + 0.5); 
			 if (polygon_vertex_count == 1)
			 {
				 points[k+1].x = points[i].x;
		         points[k+1].y = points[i].y;
			 }
		 }

	   cur_point_position = cur_point_position + polygon_vertex_count;
	   if (polygon_vertex_count == 1)
	      new_point_position = new_point_position + 2*polygon_vertex_count;
	   else
	      new_point_position = new_point_position + polygon_vertex_count;
   }
     
    win32printmgr_end_path();
    
	PolyPolygon(pd.hDC, points, poly_num_of_vertex_count_list,num_polygons);
   
}

/*****************************************************************************/
void win32printmgr_polyline(double point_array[][3], int numpoints)
{
   int i, npoints = numpoints;
   POINT points[256];

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_polyline: [%d]\n", numpoints);

   /* check for i<256 to avoid memory problems -- MAZ */
   for (i=0; (i < numpoints) && (i < 256); i++)
   {
      points[i].x = (long)(point_array[i][0] + 0.5);
      points[i].y = (long)(point_array[i][1] + 0.5);
   }

   win32printmgr_end_path();

   if (npoints == 1)
   {
		/* plot a single point. This will make sense if we end
		up with many single points to plot */
		points[1].x = points[0].x;
		points[1].y = points[0].y;
		npoints = 2;
    }

   /* SPR 1124870 & 1124871: Fix call to Polyline */
   /* PREFIX: numpoints must be >= 2 according to MSDN */
   if (npoints >= 2)
   {	
       Polyline(pd.hDC, points, npoints);
   }
}


/*****************************************************************************/
void win32printmgr_access(plotter_ptr, plot_name, create)
Plotter *plotter_ptr;
char *plot_name;
int create;
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_access: [%d] - not required\n", create);
}


/*****************************************************************************/
static int win32printmgr_set_color_supported(int val)
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_set_color_supported: [%d]\n", val);

   win32printmgr_color_supported = val;
   return 0;
}

/*****************************************************************************/
int win32printmgr_get_color_supported()
{
  return win32printmgr_color_supported;
}

/*****************************************************************************/
void win32printmgr_set_color(rgb_color, index, alt_rgb, alt_pen)
double rgb_color[3], alt_rgb[3];
int index, alt_pen;
{
   if (!win32printmgr_color_supported)
      return;

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_set_color: [%g] [%g] [%g] [%d] [%g] [%g] [%g]\n",
             rgb_color[0], rgb_color[1], rgb_color[2], index, alt_rgb[0],
             alt_rgb[1], alt_rgb[2]);

   cur_brush.lbColor = RGB((int)(alt_rgb[0]*255.0), (int)(alt_rgb[1]*255.0),
                           (int)(alt_rgb[2]*255.0));

   SetTextColor(pd.hDC, cur_brush.lbColor);
   win32printmgr_update_brush();
   win32printmgr_update_pen();
}


/*****************************************************************************/
void win32printmgr_setlinetype(logical_type, dash_count, dash_list, offset)
int logical_type;
int dash_count;
double *dash_list;
double offset;
{
   int i;

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_setlinetype: [%d] [%d] [%p] [%g]\n", logical_type,
             dash_count, dash_list, offset);

   cur_penstyle = PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_BEVEL;

   switch(logical_type)
   {
      case SOLIDFONT:
         cur_penstyle |= PS_SOLID;
         cur_penstylecount = 0;
         if (cur_penlinepattern != NULL)
         {
            rlsmem(cur_penlinepattern);
            cur_penlinepattern = NULL;
         }
         break;
      default:
         if (dash_count)
         {
            cur_penstyle |= PS_USERSTYLE;

            if (cur_penlinepattern != NULL)
               rlsmem(cur_penlinepattern);
            cur_penlinepattern = (int *)getmem(sizeof(int)*(dash_count+1));

            for (i=0; i<dash_count; i++)
            {
               cur_penlinepattern[i] = (DWORD)(dash_list[i] + 0.5);
               if (cur_penlinepattern[i] < 1)
                  cur_penlinepattern[i] = 1;
            }
            cur_penstylecount = dash_count;
         }
         else
         {
            cur_penstylecount = 0;
            cur_penstyle |= PS_DASH;
            if (cur_penlinepattern != NULL)
            {
               rlsmem(cur_penlinepattern);
               cur_penlinepattern = NULL;
            }
         }
         break;
   }

   win32printmgr_update_pen();
}


/*****************************************************************************/
void win32printmgr_resetlinetype()
{
   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_resetlinetype\n");

   cur_penstyle = PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_BEVEL;
   cur_penstylecount = 0;
   if (cur_penlinepattern != NULL)
   {
      rlsmem(cur_penlinepattern);
      cur_penlinepattern = NULL;
   }

   win32printmgr_update_pen();
}


/*****************************************************************************/
void win32printmgr_set_linewidth(lwidth)
float lwidth;
{
   int new_penwidth;

   new_penwidth = (int)lwidth;
   if (new_penwidth < 1)
      new_penwidth = 1;

   if (get_run_mode() == 7647)
      btk_printf("win32printmgr_set_linewidth: [%d]\n", new_penwidth);

   if (new_penwidth != cur_penwidth)
   {
      cur_penwidth = new_penwidth;
      win32printmgr_update_pen();
   }
}

/*****************************************************************************/
void win32printmgr_rasterimage(PlotImageSegment segment, PglImage pglimage,
                               double w, double h, double trans_x,
                               double trans_y, int raster_dpi, void **appdata)
{
  PglPrinter pgl_printer = NULL;

  if (segment == PLOT_PGL_IMAGE_HEADER ||
      segment == PLOT_PGL_IMAGE_SINGLE_PASS)
  {
    /* code from shadedump.c */
    PglRasterExporttype raster_type = PGL_IE_MS_PM;
    PglImageDepth image_depth = PGL_IMAGE_DEPTH_24;
    SizeInfo *paper_size = NULL;
    void *hDC = NULL;

    if (get_run_mode() == 7647)
    {
      btk_printf("win32printmgr_rasterimage\n");
    }
	
    paper_size = &(get_plot_option_data(PO_PAPER_SIZE)->size);

    PglPrinterCreate(L"2dexportmsprintmgr", PGL_PR_RASTER, NULL, &pgl_printer);
    PglPrinterSetRasterType(pgl_printer, raster_type);
    PglPrinterSetRasterDPI(pgl_printer, raster_dpi);
    PglPrinterSetRasterImageDepth(pgl_printer, image_depth);
    PglPrinterSetFillPage(pgl_printer, PGL_FALSE);
  
    /* Width and height will be in inches */
    if (paper_size != NULL)
    {
      double width, height;
      width = paper_size->width;
      height = paper_size->height;		      
      PglPrinterSetPaper(pgl_printer, PGL_VAR_SIZE_PAPER,
                        PGL_UNIT_INCH, width, height);
    }
 
    /* don't print showpage - leave it to ProE */
    PglPrinterSetPSShowpageFlag(pgl_printer, PGL_FALSE);

    win32printmgr_get_hDC(&hDC);
    PglPrinterSetWin32hDC(pgl_printer, hDC);

    PglMSPrintMgrWriteRasterHead(pglimage, pgl_printer);
    *appdata = pgl_printer;
  }

  if (segment == PLOT_PGL_IMAGE_BODY ||
      segment == PLOT_PGL_IMAGE_SINGLE_PASS)
  {
    double p_scale;
    p_scale = get_plotter_units_per_inch() / raster_dpi;
 
    if (get_run_mode() == 7647)
      btk_printf("msprintmgr_write_raster at x = %f, y = %f, scale = %f\n",
              trans_x, trans_y, p_scale);

    PglMSPrintMgrWriteRasterBody(pglimage, (*appdata),
                                 trans_x, trans_y, p_scale);
  }

  if (segment == PLOT_PGL_IMAGE_TAIL ||
      segment == PLOT_PGL_IMAGE_SINGLE_PASS)
  {
    pgl_printer = (*appdata);
    PglMSPrintMgrWriteRasterTail(pglimage, pgl_printer);
    PglPrinterDelete(pgl_printer);
    *appdata = NULL;
  }
}

/*****************************************************************************/
int win32printmgr_init_jumptable()
{
   struct pjmptable *table;

   get_plot_table(&table);
   table->init_plotter =  win32printmgr_plotter_init;
   table->close =         win32printmgr_close;
   table->setup =         win32printmgr_setup;
   table->set_clip =      win32printmgr_set_clip;
   table->cancel_clip =   win32printmgr_cancel_clip;
   table->reset_plotter = win32printmgr_reset_plotter;
   table->set_fill_mode = noop;
   table->definepen =     noop;
   table->polyline =      win32printmgr_polyline;
   table->plot =          win32printmgr_plot;
   table->symbol =        win32printmgr_put_text;
   table->circle =        win32printmgr_circle;
   table->arc =           win32printmgr_arc;
   table->fillpoly =      win32printmgr_fillpoly;
   table->fillmultipoly = win32printmgr_fillMultipoly;
   table->access =        win32printmgr_access;
   table->set_lineattr =  generic_line_attr;
   table->set_color =     win32printmgr_set_color;
   table->setlinetype =   win32printmgr_setlinetype;
   table->resetlinetype = win32printmgr_resetlinetype;
   table->set_linewidth = win32printmgr_set_linewidth;
   table->rasterimage =   win32printmgr_rasterimage;
   table->adjustrasterimage = noop;

   return 0;
}


/*****************************************************************************/
int win32printmgr_bitmap(Proimage *in_image)
{
  int outline[2][2], target_outline[2][2];
  int sizex, sizey, bitmap_width;
  int row, out_row, out_col, (*p_outline)[2][2];
  int source_x, source_y, dest_x, dest_y;
  unsigned char *next_line_ptr, *into_bitmap_data;
  void *bitmap_data;
  int i, *inten, *cur_inten, counter;
  BITMAPINFO *bmInfo;
  BITMAPINFOHEADER bmInfoHdr;
  int *colors, release_colors = FALSE;
  double rgb_color[3];
  static int iyOffset = 0;
  int iDeviceHeight;
  double fMagnification;

  p_outline = proimage_get_outline(in_image);
  outline[0][0] = (*p_outline)[0][0];
  outline[0][1] = (*p_outline)[0][1];
  outline[1][0] = (*p_outline)[1][0];
  outline[1][1] = (*p_outline)[1][1];

  target_outline[0][0] = 0;
  target_outline[0][1] = 0;
  target_outline[1][0] = (outline[1][0] - outline[0][0]);
  target_outline[1][1] = (outline[1][1] - outline[0][1]);

  if ( proimage_get_format(in_image) == NO_COMPRESSION )
    convert_proimage_pixel_format( in_image, PIX_ARGB );

  sizex = target_outline[1][0] - target_outline[0][0] + 1;
  sizey = target_outline[1][1] - target_outline[0][1] + 1;

  fMagnification = (GetDeviceCaps(pd.hDC, PHYSICALWIDTH) -
		    2 * GetDeviceCaps(pd.hDC, PHYSICALOFFSETX)) /
    (double) sizex;

  source_x = outline[0][0];
  source_y = outline[0][1];

  /* the gdi32 bitmap is long aligned. */
  if (sizex%4 == 0)
    bitmap_width = sizex;
  else
    bitmap_width = sizex + (4-sizex%4);

  bitmap_data = (void *)GlobalAlloc(GMEM_FIXED, 3 * (bitmap_width * sizey) *
				    sizeof(unsigned char));
  into_bitmap_data = (unsigned char *)bitmap_data;
  next_line_ptr = into_bitmap_data + (bitmap_width * 3);
  inten = *(proimage_get_image(in_image));
  for (out_row=0, row=source_y; out_row<sizey; row++, out_row++)
    {
      cur_inten = inten + (row * proimage_get_width(in_image)) + source_x;
      for (out_col=0; out_col<sizex; out_col++)
	{
	  *into_bitmap_data = (*cur_inten) & 0x000000ff; /* BLUE */
	  *(into_bitmap_data+1) = ((*cur_inten) >> 8) & 0x000000ff; /* GREEN */
	  *(into_bitmap_data+2) = ((*cur_inten) >> 16) & 0x000000ff; /* RED */
	  into_bitmap_data=into_bitmap_data+3;
	  cur_inten++;
	}
      into_bitmap_data = next_line_ptr;
      next_line_ptr = into_bitmap_data + (bitmap_width * 3);
    }
  if (!(bmInfo = (BITMAPINFO *)GlobalAlloc(GMEM_FIXED, sizeof(BITMAPINFO))))
    {
      GlobalFree(bitmap_data);
      return(E_NO_ERROR);
    }
  bmInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmInfo->bmiHeader.biWidth = bitmap_width;
  bmInfo->bmiHeader.biHeight = sizey;
  bmInfo->bmiHeader.biPlanes = 1;
  bmInfo->bmiHeader.biBitCount = 24;
  bmInfo->bmiHeader.biCompression = BI_RGB;
  bmInfo->bmiHeader.biSizeImage = 3 * bitmap_width * sizey;
  bmInfo->bmiHeader.biClrImportant = 0; /* All colors important. */
  bmInfo->bmiHeader.biClrUsed = 0;

  StretchDIBits(pd.hDC,
		0, (int) (iyOffset * fMagnification),
		(int)(sizex * fMagnification), (int)(sizey * fMagnification),
		0, 0,
		sizex, sizey,
		bitmap_data, bmInfo,
		DIB_RGB_COLORS, SRCCOPY);
  GdiFlush();

  /* Free the Device contexts, and the Bitmap */
  GlobalFree(bitmap_data);
  GlobalFree(bmInfo);

  /* Increment y offset so that the next pass can be blitted in the right
     position */
  iyOffset += sizey;
  iDeviceHeight = GetDeviceCaps(pd.hDC, PHYSICALHEIGHT)
    - 2 * GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);
  if ((int)((iyOffset + sizey) * fMagnification) > iDeviceHeight)
    iyOffset = 0; /* Done with all passes, reset offset to 0.*/
  return(E_NO_ERROR);
}

/*****************************************************************************/
int win32printmgr_get_device_dimensions( double * pfWidth,
					 double * pfHeight,
					 int * piDpi)
{
  if (pd.hDC == NULL)
    return E_ERROR;
  if (dpi == 0)
    return E_ERROR;
  * pfWidth = (GetDeviceCaps(pd.hDC, PHYSICALWIDTH)
    - 2 * GetDeviceCaps(pd.hDC, PHYSICALOFFSETX)) / dpi;
  * pfHeight = (GetDeviceCaps(pd.hDC, PHYSICALHEIGHT)
    - 2 * GetDeviceCaps(pd.hDC, PHYSICALOFFSETY)) / dpi;
  * piDpi = (int) dpi;
  return E_NO_ERROR;
}

/*****************************************************************************/
/* INTERNAL GRAPHICS FUNCTION - DO NOT USE */

int win32printmgr_get_hDC(void **pv_phDC)
{
  if (pd.hDC == NULL)
    return E_ERROR;
  *pv_phDC = (void *) pd.hDC;
  return E_NO_ERROR;
}

/*****************************************************************************/
static int win32printmgr_begin_path()
{
   return(1);
}


/*****************************************************************************/
static int win32printmgr_end_path()
{
   return(1);
}

int get_ms_print_offset()
{ return add_default_print_offset; }

int set_ms_print_offset(int val)
{
	int old = add_default_print_offset;
	add_default_print_offset = val;
	return old;
}
#endif
