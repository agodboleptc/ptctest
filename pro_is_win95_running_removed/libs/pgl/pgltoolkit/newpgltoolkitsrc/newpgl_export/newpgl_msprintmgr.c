/*--------------------------------------------------------------------------*\
|
|  Name:    pgl_msprintmgr.c
|
|  Module Details:
|
|  Purpose: Microsoft Printmanager driver 
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  16-Nov-99 I-03-21 imj   ##1   Created
|  19-Nov-99 I-03-21+imj   ##2   Fixed compilation error
|  23-Nov-99 I-03-22 imj   ##3   MSPrintManager fixes.
|  09-Feb-01 J-01-28 cm    ##4   improve error checking
|  09-Feb-01 J-02-05 cm    ##5   improve error checking
|  03-Apr-01 J-01-31 cm    ##6   use dev_name instead of dmDeviceName
|  21-Aug-00 J-03-01 imj   ##7   Removed PglPrnWindow.
|  04-Apr-01         rmi         Fix compile error
|  20-May-01         aap         Use Pfa.
|  23-Jun-01 J-03-03 cm    ##8   moved p_cur_pad to _PglWindow
|  09-Jul-02 J-03-30 arm   ##9   Use hDC from PglPrinter, if defined.
|  20-Aug-02 J-03-32 arm   ##10  Set Pen FONTs based on logical pen type
|  24-Apr-06 L-01-07 TWH   ##11  use windows unicode wrappers
|  12-Jul-06 L-01-12 ksi   ##12  Unicode compliant changes
|  07-Aug-06 L-01-14 TWH   ##13  unicode switch for DEVMODE struct
|  12-Mar-12 P-20-01 AC    ##14  Updated for Project 13028358
|  27-Mar-12 P-20-02 LOK   ##15  Move headers at appropriate place
|  16-Jan-13 P-20-23 AMAG  ##16  modified ipglWin32GetDevmode
|  03-Mar-14 P-20-50 svenkata ##17 Angular value usage review
|  29-Oct-14 P-20-63 NERELLA ##18 added btk_u8stou16s_alloc declaration
|  17-May-20 P-80-04 rds     ##19 Functions moved from internal to API layer.
|  21-Jul-20 P-80-13 rds   $$1     Copied from pgltoolkitsrc

|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#include <ptc_win32.h>
#include <btkcstdio.h>
#include <newpgli_mgr.h>
#if OPER_SYS == WINDOWS_32
#include <winspool.h>

wchar_t *btk_u8stou16s_alloc(char *);
typedef struct _pglmsprintmgrdriver
{
  _PglPrinter *p_printer;
  Pfa        *fd;
  HDC          hdc;
  HPEN         hPen;
  LOGPEN       logPen;
  HBRUSH       hBrush;
  LOGBRUSH     logBrush;
    
  PglBool      initialized;
  _PglPrinterfns drvfns;

} _PglMSPrintmgrdriver;

static _PglMSPrintmgrdriver _mspmgrdrv =
{
  NULL,
  0,
  NULL
};

static void ipglMSPrintmgrPlot(_PglWindowPtr p_win, double x, double y, _PglPenstatus penstatus);
static void ipglMSPrintmgrResetClip(_PglWindowPtr p_win);
static void ipglMSPrintmgrLineWidth(_PglWindowPtr p_win, double dev_width);
static void ipglMSPrintmgrSetClip(_PglWindowPtr p_win, double p1[3], double p2[3]);
static void ipglMSPrintmgrReset(_PglWindowPtr p_win);
static void ipglMSPrintmgrCircle(_PglWindowPtr p_win, double center[3], double radius);
static void ipglMSPrintmgrArc(_PglWindowPtr p_win,  double center[3], double radius, double end1[3], double end2[3], double t0, double t1);
static void ipglMSPrintmgrFillPoly(_PglWindowPtr p_win, double pts[][3], int num_points);
static void ipglMSPrintmgrSetLineType(_PglWindowPtr p_win, int logical_type, int dash_count, double dash_list[], double offset);
static void ipglMSPrintmgrResetLineType(_PglWindowPtr p_win);
static void ipglMSPrintmgrSetup(_PglWindowPtr p_win);
static void ipglMSPrintmgrInit(_PglWindowPtr p_win, Pfa *fd);
static void ipglMSPrintmgrColor(_PglWindowPtr p_win, double rgb[3], int pen);
static void ipglMSPrintmgrUpdatePen();
static void ipglMSPrintmgrUpdateBrush();


/* ----------------------------------------------------------- *\
| Simulate PENUP, PENDOWN pen statuses. 
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrPlot(_PglWindowPtr p_win, double x, double y, _PglPenstatus penstatus)
{
#if 0
  printf("Called ipglMSPrintmgrPlot.\n");
  printf("position: (%f, %f).\n",
      x, y);
#endif

  switch(penstatus)
    {
    case PGL_PR_PENUP:
         MoveToEx(_mspmgrdrv.hdc,
            x,
            y,
            NULL);
         break;
    case PGL_PR_PENDOWN:
         LineTo(_mspmgrdrv.hdc,
            x,
            y);
         break;
    default:
         btk_printf("ps_plot: Unknown pen status %d.\n", penstatus);
         break;
  }

}


/* ----------------------------------------------------------- *\
| Reset clipping
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrResetClip(_PglWindowPtr p_win)
{
#if 0
  printf("Called ipglMSPrintmgrResetClip\n");
#endif
}


/* ----------------------------------------------------------- *\
| Set line width
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrLineWidth(_PglWindowPtr p_win, double dev_width)
{
#if 0
  printf("Called ipglMSPrintmgrLineWidth\n");
  printf("dev_width: %f\n", dev_width);
#endif

  _mspmgrdrv.logPen.lopnWidth.x = dev_width;
  ipglMSPrintmgrUpdatePen();
}


/* ----------------------------------------------------------- *\
| Define clipping
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrSetClip(_PglWindowPtr p_win, double p1[3], double p2[3])
{
#if 0
  printf("Called ipglMSPrintmgrSetClip\n");
#endif
}


/* ----------------------------------------------------------- *\
| Reset 
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrReset(_PglWindowPtr p_win)
{
  int prnstatus;

#if 0
  printf("Called ipglMSPrintmgrReset\n");
#endif

  (void)ipglWindowGetPrnStatus(p_win, &prnstatus);
  if ((prnstatus != PGL_PRNWINDOW_MULTI_WINDOW_LAST) &&
      (prnstatus != PGL_PRNWINDOW_SINGLE_WINDOW))
    return;
  
  if (EndPage(_mspmgrdrv.hdc) <= 0)
    dbg_err_crash("ipglMSPrintmgrReset", "Failed to EndPage");
  if (EndDoc(_mspmgrdrv.hdc) <= 0)
    dbg_err_crash("ipglMSPrintmgrReset",
           "Failed to EndDoc");
  DeleteDC(_mspmgrdrv.hdc);
  DeleteObject(_mspmgrdrv.hPen);
  DeleteObject(_mspmgrdrv.hBrush);
}


/* ----------------------------------------------------------- *\
| Draw a circle 
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrCircle(_PglWindowPtr p_win, double center[3], double radius)
{
#if 0
  printf("Called ipglMSPrintmgrCircle\n");
  printf("cx: %f, cy:%f, rad: %f\n", center[0], center[1], radius);
#endif
  AngleArc(_mspmgrdrv.hdc, center[0], center[1],
        radius,
        0.f,
        360.f);
}


/* ----------------------------------------------------------- *\
| Draw an arc
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrArc(_PglWindowPtr p_win,  double center[3], double radius, double end1[3], double end2[3], double t0, double t1)
{
  int iNPoints, i;
  double fAngleStep;
  static double fMinAngle = DEG_TO_RAD * 3.0;
#if 0
  printf("Called ipglMSPrintmgrArc\n");
  printf("cx: %f, cy:%f, rad: %f, end1: (%f, %f), end2(%f, %f), t0: %f, t1:%f\n",
      center[0], center[1], radius, end1[0], end1[1], end2[0], end2[1], t0, t1);
#endif
  if ((t1 - t0) == 0.)
    return;

  iNPoints = ceil((t1 - t0) / fMinAngle);
  fAngleStep = (t1 - t0) / (iNPoints);
  MoveToEx(_mspmgrdrv.hdc, end1[0], end1[1], NULL);
  for (i = 1; i <= iNPoints; i++)
    {
      LineTo(_mspmgrdrv.hdc,
          center[0] + radius * cos(t0 + i * fAngleStep),
          center[1] + radius * sin(t0 + i * fAngleStep));
    }
    
  /*  AngleArc(_mspmgrdrv.hdc, center[0], center[1],
        radius,
        t0 / PI * 180.f,
        (t1 - t0) / PI * 180.f);*/

  /*Arc(_mspmgrdrv.hdc,
      center[0] - radius,
      center[1] + radius,
      center[0] + radius,
      center[1] - radius,
      end1[0], end2[1],
      end2[0], end2[1]);*/
}


/* ----------------------------------------------------------- *\
| Draw a filled polygon 
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrFillPoly(_PglWindowPtr p_win, double pts[][3], int num_points)
{
  int i;
  POINT * pPoints;
#if 0
  printf("Called ipglMSPrintmgrFillPoly\n");
  printf("Number of points: %i\n", num_points);
#endif
  
  pPoints = getmem(sizeof(POINT) * num_points);
  for (i = 0; i < num_points; i++)
    {
      pPoints[i].x = pts[i][0];
      pPoints[i].y = pts[i][1];
    }
  
  Polygon(_mspmgrdrv.hdc, pPoints, num_points);

  relmem (&pPoints);
  
}


/*------------------------------------------------------------------*/
/* SET_LT_INSTR sets the specified line type for the plotter.       */
/*------------------------------------------------------------------*/
static void ipglMSPrintmgrSetLineType(_PglWindowPtr p_win, int logical_type,
                                      int dash_count, double dash_list[],
                                      double offset)
{
  switch (logical_type)
  {
   case DASHFONT:
     _mspmgrdrv.logPen.lopnStyle = PS_DASH;
     break;
   case DOTFONT:
     _mspmgrdrv.logPen.lopnStyle = PS_DOT;
     break;
   case SOLIDFONT:
     _mspmgrdrv.logPen.lopnStyle = PS_SOLID;
     break;
   case PHANTOMFONT:
     _mspmgrdrv.logPen.lopnStyle = PS_DASHDOTDOT;
     break;

  /* fall through case */
   case CTRLFONT:
   default:
     break;
  }
  ipglMSPrintmgrUpdatePen();

#if 0
  printf("Called ipglMSPrintmgrSetLineType\n");
  printf("Logical type: %i, dash_count: %i, offset: %f\n",
      logical_type, dash_count, offset);
#endif
}


/*---------------------------------------------------------------------*/
/* RESET_LT_INSTR resets line style for the plotter to solid. (default)*/
/*---------------------------------------------------------------------*/
static void ipglMSPrintmgrResetLineType(_PglWindowPtr p_win)
{
#if 0
  printf("Called ipglMSPrintmgrResetLineType\n");
#endif
}

/* ----------------------------------------------------------- *\
| Setup printmanager
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrSetup(_PglWindowPtr p_win)
{
#if 0
  printf("Called ipglMSPrintmgrSetup\n");
#endif
}


/* ----------------------------------------------------------- *\
| Initialize printmanager
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrInit(_PglWindowPtr p_win, Pfa *fd)
{
  DEVMODEA * pInitData;
  DOCINFOA docInfo;
  int iVertRes, iResult;
  int prnstatus;
  _ExportPadData * p_export_data = (_ExportPadData *)p_win->p_cur_pad->p_platform_data;
#if 0
  printf("Called ipglMSPrintmgrInit\n");
#endif

  (void)ipglWindowGetPrnStatus(p_win, &prnstatus);
  if ((prnstatus != PGL_PRNWINDOW_MULTI_WINDOW_FIRST) &&
      (prnstatus != PGL_PRNWINDOW_SINGLE_WINDOW))
    return;

  _mspmgrdrv.p_printer = p_export_data->p_printer;
  _mspmgrdrv.fd = fd;
  pInitData = (DEVMODEA *)(p_export_data->p_printer->pDevMode);


  /* Use the existing printer device context if it exists, otherwise
   * Create the printer device context
   */

  if ( _mspmgrdrv.p_printer->phDC != NULL )
    _mspmgrdrv.hdc = (HDC) p_export_data->p_printer->phDC;
  else
    _mspmgrdrv.hdc = uCreateDC(NULL,
                   p_export_data->p_printer->dev_name,
                   NULL,
                   pInitData);

  if (_mspmgrdrv.hdc == NULL)
    dbg_err_crash("ipglMSPrintmgrInit",
           "Failed to create Device Context.");

  /* Start the print document */
  {
    static char docName[] = "Document name";
    static char fileName[256];
    ZeroMemory(&docInfo, sizeof(docInfo));
    if (p_export_data->p_printer->destination != PGL_PRD_PRINTER)
      {
     wstrtos(fileName, p_export_data->p_printer->p_filename);
     docInfo.lpszOutput = fileName;
      }
    else
      docInfo.lpszOutput = NULL;
    docInfo.cbSize = sizeof(docInfo);
    docInfo.lpszDocName = docName;
    docInfo.lpszDatatype = NULL;
    docInfo.fwType = DI_APPBANDING;
    
    if (uStartDoc(_mspmgrdrv.hdc, &docInfo) <= 0)
      dbg_err_crash("ipglMSPrintmgrInit",
             "Failed to StartDoc");
  }

  if (StartPage(_mspmgrdrv.hdc) <= 0)
    dbg_err_crash("ipglMSPrintmgrInit",
           "Failed to StartPage");

  /* Set the mapping mode to HIMETRIC */
  iResult = SetMapMode(_mspmgrdrv.hdc, MM_HIMETRIC);
  iVertRes = GetDeviceCaps(_mspmgrdrv.hdc, VERTRES);
  SetViewportOrgEx(_mspmgrdrv.hdc, 0,
             iVertRes,
             NULL);
  if (iResult == 0)
    dbg_err_crash("ipglMSPrintmgrInit",
           "SetMapMode Failed.");

  /* Initialize drawing objects */
  _mspmgrdrv.logPen.lopnStyle = PS_SOLID;
  _mspmgrdrv.logPen.lopnWidth.x = 1;
  _mspmgrdrv.logPen.lopnColor = RGB(0, 0, 0);
  _mspmgrdrv.hPen = CreatePenIndirect(&(_mspmgrdrv.logPen));
  SelectObject(_mspmgrdrv.hdc, _mspmgrdrv.hPen);

  _mspmgrdrv.logBrush.lbStyle = BS_SOLID;
  _mspmgrdrv.logBrush.lbColor = RGB(0, 0, 0);
  _mspmgrdrv.logBrush.lbHatch = 0;
  _mspmgrdrv.hBrush = CreateBrushIndirect(&(_mspmgrdrv.logBrush));
  SelectObject(_mspmgrdrv.hdc, _mspmgrdrv.hBrush);         
}


/* ----------------------------------------------------------- *\
| Set color
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrColor(_PglWindowPtr p_win, double rgb[3], int pen)
{
  HPEN hNewPen;
  HBRUSH hNewBrush;

#if 0
  printf("Called ipglMSPrintmgrColor\n");
  printf("rbg: (%f, %f, %f), pen: %i\n",
      rgb[0], rgb[1], rgb[2], pen);
#endif

  _mspmgrdrv.logPen.lopnColor = RGB(rgb[0] * 255, rgb[1] * 255, rgb[2] * 255);

  ipglMSPrintmgrUpdatePen();
  ipglMSPrintmgrUpdateBrush();
}

/* ----------------------------------------------------------- *\
| Update Pen
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrUpdatePen()
{
  HPEN hNewPen;

  if (_mspmgrdrv.logPen.lopnStyle != PS_SOLID &&
      _mspmgrdrv.logPen.lopnWidth.x > 1)
  {
    /*
     * Non SOLID pens require a width of 1. (See TODO below)
     */
    int old_width = _mspmgrdrv.logPen.lopnWidth.x;

    _mspmgrdrv.logPen.lopnWidth.x = 1;
    hNewPen = CreatePenIndirect(&(_mspmgrdrv.logPen));
    _mspmgrdrv.logPen.lopnWidth.x = old_width;
  }
  
  else
    hNewPen = CreatePenIndirect(&(_mspmgrdrv.logPen));

  SelectObject(_mspmgrdrv.hdc, hNewPen);
  DeleteObject(_mspmgrdrv.hPen);
  _mspmgrdrv.hPen = hNewPen;
}

#if 0
/*
 * TODO: Create wide non-SOLID pens using ExtCreatePen().
 *
 * Plotting might be better if using the ExtCreatePen() function
 * to create a wide non-SOLID pen, but using this did not give very good
 * results for plotting a graph (SPR 9022898).  It might give better results
 * for other plots.
 *
 * here is the code you could use to implement ExtCreatePen() (note that
 * Win95 support is different from WinNT....)
 *
  {
    if(!pro_is_win95_running())
    {
      DWORD dwPenStyle = _mspmgrdrv.logPen.lopnStyle | PS_GEOMETRIC;

      hNewPen = ExtCreatePen(dwPenStyle, _mspmgrdrv.logPen.lopnWidth.x,
                             &_mspmgrdrv.logBrush, 0, NULL);
    }
    else
    {
      int old_width = _mspmgrdrv.logPen.lopnWidth.x;

      _mspmgrdrv.logPen.lopnWidth.x = 1;
      hNewPen = CreatePenIndirect(&(_mspmgrdrv.logPen));
      _mspmgrdrv.logPen.lopnWidth.x = old_width;
    }
  }
 *
 * the existing function: pgli_gdi32_set_line_style() tries to do this, but
 * I am not confident that the dash layout is properly done (e.g. not scaling
 * to the page size as is appropriate).
 */
#endif

/* ----------------------------------------------------------- *\
| Update Brush
\* ----------------------------------------------------------- */
static void ipglMSPrintmgrUpdateBrush()
{
  HBRUSH hNewBrush;
  
  hNewBrush = CreateBrushIndirect(&(_mspmgrdrv.logBrush));
  SelectObject(_mspmgrdrv.hdc, hNewBrush);
  DeleteObject(_mspmgrdrv.hBrush);
  _mspmgrdrv.hBrush = hNewBrush;
} 



/* ----------------------------------------------------------- *\
| Get print manager driver functions 
\* ----------------------------------------------------------- */
_PglPrinterfns *ipglGetMSPrintmgrDriver()
{
  if (!_mspmgrdrv.initialized)
   {
     _mspmgrdrv.drvfns.init           = ipglMSPrintmgrInit;
     _mspmgrdrv.drvfns.reset          = ipglMSPrintmgrReset;
     _mspmgrdrv.drvfns.setup          = ipglMSPrintmgrSetup;
     _mspmgrdrv.drvfns.set_clip       = ipglMSPrintmgrSetClip;
     _mspmgrdrv.drvfns.cancel_clip    = ipglMSPrintmgrResetClip;
     _mspmgrdrv.drvfns.setlinetype    = ipglMSPrintmgrSetLineType;
     _mspmgrdrv.drvfns.resetlinetype  = ipglMSPrintmgrResetLineType;
     _mspmgrdrv.drvfns.set_color      = ipglMSPrintmgrColor;
     _mspmgrdrv.drvfns.set_linewidth  = ipglMSPrintmgrLineWidth;
     _mspmgrdrv.drvfns.plot           = ipglMSPrintmgrPlot;
     _mspmgrdrv.drvfns.circle         = ipglMSPrintmgrCircle;
     _mspmgrdrv.drvfns.arc            = ipglMSPrintmgrArc;
     _mspmgrdrv.drvfns.fillpoly       = ipglMSPrintmgrFillPoly;

     _mspmgrdrv.initialized = PGL_TRUE;
   }

  return &(_mspmgrdrv.drvfns);
}
#endif /* OPER_SYS == WINDOWS_32 */

PglError ipglWin32GetDevmode(char * psPrinterName, PglBool prompt,
                      void * v_pInDevMode, void  * v_ppOutDevMode)
{
#if OPER_SYS == WINDOWS_32
  HANDLE hPrinter;
  LONG iNBytesNeeded;
  DWORD mode = DM_OUT_BUFFER;
  LONG result;

  if ((psPrinterName == NULL) || (v_ppOutDevMode == NULL))
    return PGL_E_BAD_INPUTS;

  if (!btkUnicodeIsEnabled())
  {
     DEVMODEA * pNewDevMode;
     /* DEVMODEA * pInDevMode = (DEVMODEA *) v_pInDevMode; */
     DEVMODEA * * ppOutDevMode = (DEVMODEA * *) v_ppOutDevMode;
    
    /* Start by opening the printer */ 
     if (OpenPrinterA(psPrinterName, &hPrinter, NULL))
    { 
      /*
         * Step 1:
         * Allocate a buffer of the correct size.
      */ 
      iNBytesNeeded = DocumentPropertiesA(NULL,
                          hPrinter,
                          psPrinterName,
                          NULL,
                          NULL,
                          0);
      if (iNBytesNeeded < 0)
        return PGL_E_ERROR;

      pNewDevMode = getmem(iNBytesNeeded);
      
      /*
         * Step 2:
         * Get the default DevMode for the printer and
         * modify it for your needs.
      */ 
      result = DocumentPropertiesA(NULL,
                      hPrinter,
                      psPrinterName,
                      pNewDevMode,
                      NULL,
                      mode);
                      
      if (result != IDOK)
         {
                return PGL_E_ERROR;
         }
      
      if (prompt)
                mode |= DM_IN_PROMPT;
       
       /*
        * Make changes to the DevMode which are supported.
      */             
      if (pNewDevMode != NULL)
      {   
           pNewDevMode->dmOrientation = DMORIENT_LANDSCAPE;
           pNewDevMode->dmFields = DM_ORIENTATION;
                 mode |= DM_IN_BUFFER;
       }     
      
      /*
         * Step 3:
         * Merge the new settings with the old.
         * This gives the driver an opportunity to update any private
         * portions of the DevMode structure.
      */ 
      result = DocumentPropertiesA(NULL,
                       hPrinter,
                       psPrinterName,
                       pNewDevMode,
                       pNewDevMode,
                       mode); 
       if (result != IDOK)
          {
               return PGL_E_ERROR;
          }       
       *ppOutDevMode = pNewDevMode;
    }
    else
      return PGL_E_ERROR;
  }
  else // unicode
  {
     DEVMODEW * pNewDevMode;
     DEVMODEW * pInDevMode = (DEVMODEW *) v_pInDevMode;
     DEVMODEW * * ppOutDevMode = (DEVMODEW * *) v_ppOutDevMode;
     wchar_t *lptext1 = btk_u8stou16s_alloc(psPrinterName);
     
    /* Start by opening the printer */ 
     if (OpenPrinterW(lptext1, &hPrinter, NULL))
    { 
      /*
         * Step 1:
         * Allocate a buffer of the correct size.
      */ 
      iNBytesNeeded = DocumentPropertiesW(NULL,
                          hPrinter,
                          lptext1,
                          NULL,
                          NULL,
                          0);
      if (iNBytesNeeded < 0)
        return PGL_E_ERROR;

      pNewDevMode = getmem(iNBytesNeeded);
      
      /*
         * Step 2:
         * Get the default DevMode for the printer and
         * modify it for your needs.
      */ 
      result = DocumentPropertiesW(NULL,
                      hPrinter,
                      lptext1,
                      pNewDevMode,
                      NULL,
                      DM_OUT_BUFFER);
       if (result != IDOK)
        {
               return PGL_E_ERROR;
        }
     if (prompt)
                mode |= DM_IN_PROMPT;
     if (pNewDevMode != NULL)
     {   
           pNewDevMode->dmOrientation = DMORIENT_LANDSCAPE;
           pNewDevMode->dmFields = DM_ORIENTATION;
                 mode |= DM_IN_BUFFER;
      }     
     
     /*
         * Step 3:
         * Merge the new settings with the old.
         * This gives the driver an opportunity to update any private
         * portions of the DevMode structure.
      */ 
     result = DocumentPropertiesW(NULL,
                       hPrinter,
                       lptext1,
                       pNewDevMode,
                       pNewDevMode,
                       mode); 
     if (result != IDOK)
        {
               return PGL_E_ERROR;
        }       
     *ppOutDevMode = pNewDevMode;
    }
    else
      return PGL_E_ERROR;
  }
#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}


PglError PglPrinterGetWin32Devmode(char * psPrinterName, PglBool prompt,
                                   void * v_pInDevMode, void  * v_ppOutDevMode)
{
   return ipglWin32GetDevmode( psPrinterName, prompt, v_pInDevMode, v_ppOutDevMode);
}
