/*--------------------------------------------------------------------------*\
|
|  Name: pglui_win32_img_config.c
|
|  Module Details: Win32 specific dialog functions for export
|
|  Purpose:
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  16-Nov-99 I-03-21  imj   $$1   Created
|  19-Nov-99 I-03-21+ imj   $$2   Fixed compilation error
|  23-Nov-99 I-03-22  imj   $$3   MSPrintManager fixes.
|  04-May-00 J-01-07  imj   $$4   Fixed alpha problem
|                                 Local printers are now enumerated.
|                                 Fixed memory leaks.
|  24-Aug-00 J-01-16  imj   $$5   Fixed paper size bug.
|  10-Oct-00 J-01-20  jhk   $$6   Fixed NT crash
|  01-Nov-00 J-01-21  JPE   $$7   IA64_NT
|  22-Nov-00 J-01-22  Nina  $$8   Fixed pgl_win32_img_config_update_from_devmode()
|  06-Feb-01 J-01-28  cm    $$9   improve error checking
|                                 free pPrinterInfo correctly
|  23-Aug-00 J-03-01  imj   $$10  Fixed print preview issues
|  20-Oct-00          mgs         Fixed NT compile error
|  21-Sep-01 J-03-09  jas   $$11  Removed WINDOWS_95 macro
|  26-Mar-02 J-03-22  arm   $$12  Updated to set Portrait/Landscape value.
|  03-Apr-02 J-03-23  arm   $$13  Synch Properties dialog Input with display
|  09-Sep-02 J-03-34  arm   $$14  Set DEVMODE default to Landscape for MSPM
|  18-Sep-02          arm         Don't reset DEVMODE when updating from ic_dlg
|  09-Oct-02 J-03-35  arm   $$15  Add _get_pgl_paperdata()
|  30-Mar-04 K-01-25  dad   $$16  Init paper size if set to invalid values
|  24-Apr-06 L-01-07  TWH   $$17  use windows unicode wrappers
|  07-Aug-06 L-01-14  TWH   $$18  Unicode chg for DEVMODE struct
|  25-Mar-08 L-03-06  AAK   $$19  Corrected argument type from wchar_t *
|                                 to char * in call to ipglWin32GetDevmode()
|  08-Dec-08 L-03-23  BI    $$20  Added syswindows.h
|  12-Mar-12 P-20-01  AC    $$21  Updated for Project 13028358
|  17-May-20 P-80-04  rds   $$22  Use PGL API instead of internal functions.
|  23-Feb-22 Q-10-01  jas   $$23  Fixed clang issues
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/


#include <pglui.h>
#include <pgluii_export.h>
#include <errors.h>
#include <dbg_crash.h>

#if OPER_SYS == WINDOWS_32
#include <syswindows.h>
#include <winerror.h>
#include <const.h>
#include <pro_widec.h>
#include <ct_win_syscall_proto.h>
#include <ctstrutil_proto.h>
#endif /* OPER_SYS == WINDOWS_32 */

PglError pgl_win32_free_printernames(void * v_pInfo);
static PglError _get_pgl_paperdata(_PglImgConfigDlg *p_ic_dlg,
                                   PglPapertype *paper_type,
                                   PglUnittype  *paper_unit,
                                   double *paper_width, double *paper_height);

#  define fWidthDefault 279.4
#  define fHeightDefault 215.9

PglError pgl_win32_img_config_get_printer_names(_PglImgConfigDlg * p_ic_dlg)
{
#if OPER_SYS == WINDOWS_32
  DWORD iBytesNeeded;
  /* On NT, PRINTER_INFO_4 should be used.  On Windows 9x, PRINTER_INFO_5. */

  BYTE * pPrinterInfo = NULL;
  int iLevel = 4;
  int iPrinterNameLength, i;
  _PglWin32PrintmgrInfo * pInfo = (_PglWin32PrintmgrInfo *) p_ic_dlg->win32_printmgr_info;
  PglBool bNoPrinters = FALSE;
  char sNoPrinters[] = "No printers installed.";
  char *name;

  /* Clean up any previous printer names */
  pgl_win32_free_printernames(pInfo);

  /* Figure out the total number of printers */
  if (EnumPrinters(PRINTER_ENUM_LOCAL |
		   PRINTER_ENUM_CONNECTIONS,
		   NULL,
		   iLevel,
		   NULL,
		   0,
		   &iBytesNeeded,
		   &(pInfo->iNPrinters)) == 0)
    {
      /* This should happen anytime a printer is installed,
       but the error should be ERROR_INSUFFICIENT_BUFFER*/
      DWORD iError = GetLastError();
      if (iError != ERROR_INSUFFICIENT_BUFFER)
	{
	  /* Something else went wrong */
	  dbg_err_crash("pgl_win32_img_config_get_printer_names",
			"_img_config_get_printer_names - Error in EnumPrinters 1.");
	  return PGL_E_ERROR;
	}
    }

  if (iBytesNeeded == 0)
  {
    /* No printers are installed */
    pInfo->iNPrinters = 1;
    bNoPrinters = TRUE;
  }
  else
  {
    /* Obtain info about the printers */
    pPrinterInfo = getmem(iBytesNeeded);

    if (EnumPrinters(PRINTER_ENUM_LOCAL |
                     PRINTER_ENUM_CONNECTIONS,
                     NULL,
                     iLevel,
                     pPrinterInfo,
                     iBytesNeeded,
                     &iBytesNeeded,
                     &(pInfo ->iNPrinters)) == 0)
    {
      /* This time we should have enough memory allocated. */
      dbg_err_crash("pgl_win32_img_config_get_printer_names",
                    "_img_config_get_printer_names - Error in EnumPrinters 2.");
      return PGL_E_ERROR;
    }
  }

  /* Allocate memory to store the printer names */
  pInfo->psPrinterNames = getmem(sizeof(char *) * pInfo->iNPrinters);
  pInfo->pwsPrinterLabels = getmem(sizeof(wchar_t *) * pInfo->iNPrinters);
  if (!bNoPrinters)
  {
    for (i = 0; i < pInfo->iNPrinters; i++)
    {
      if (iLevel == 4)
      {
        name = ((PRINTER_INFO_4 *) pPrinterInfo)[i].pPrinterName;
      }
      else
      {
        name = ((PRINTER_INFO_5 *) pPrinterInfo)[i].pPrinterName;
      }

      iPrinterNameLength = strlen(name) + 1;
      pInfo->psPrinterNames[i] =
        getmem(sizeof(char) * iPrinterNameLength);
      pInfo->pwsPrinterLabels[i] =
        getmem(sizeof(wchar_t) * iPrinterNameLength);
      strcpy(pInfo->psPrinterNames[i], name);
      pro_str_to_wstr(pInfo->pwsPrinterLabels[i], pInfo->psPrinterNames[i]);
    }

    relmem(&pPrinterInfo);
  }
  else
  {
    iPrinterNameLength = strlen(sNoPrinters) + 1;
    pInfo->psPrinterNames[0] = getmem(sizeof(char) * iPrinterNameLength);
    pInfo->pwsPrinterLabels[0] = getmem(sizeof(wchar_t) *
                                        iPrinterNameLength);
    strcpy(pInfo->psPrinterNames[0], sNoPrinters);
    pro_str_to_wstr(pInfo->pwsPrinterLabels[0], sNoPrinters);
  }

  /* Update the dropdown list with the printer names. */
  ui_modify(_imgconf_dev, "o_choose_printer",
	    UI_names_Attr, pInfo->iNPrinters, pInfo->psPrinterNames,
	    UI_labels_Attr, pInfo->iNPrinters, pInfo->pwsPrinterLabels,
	    UI_visible_rows_Attr, (pInfo->iNPrinters > 8) ?
	    8 : pInfo->iNPrinters,
	    UI_sel_names_Attr, 1, &(pInfo->psPrinterNames[pInfo->iCurrentPrinter]),
	    UI_sensitive_Attr, (bNoPrinters) ? FALSE : TRUE,
	    NULL);

  if (bNoPrinters)
    return PGL_E_ERROR;
#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}

/* TODO - This function should probably be changed to take an _PglImgConfigDlg
          struct as input.  This would allow us to create a better "inDevMode"
          used by PglPrinterGetWin32Devmode() to create the p_info DEVMODE.
 */
/*--------------------------------------------------------------------------*\
| INTERNAL_FUNCTION
|
| Purpose: Create the Win32 DEVMODE struct stored on the _PglWin32PrintmgrInfo.
|
| Input Arguments:
|    v_pInfo      - the _PglWin32PrintmgrInfo struct containing the DEVMODE
|                   to be initialized (cast to void *)
|
| Output Arguments:
|    none
|
| Return Values:
|    PGL_E_OK              - DEVMODE created successfully
|    PGL_E_ERROR           - general failure
|
| See Also:
|    pgl_win32_img_config_update_devmode()
|
| Note:
|    This function is a NOOP if we are not in Win32.
\*--------------------------------------------------------------------------*/

PglError pgl_win32_initialize_devmode(void * v_pInfo)
{
  PglError status = PGL_E_OK;
#if OPER_SYS == WINDOWS_32
  _PglWin32PrintmgrInfo * pInfo = (_PglWin32PrintmgrInfo *) v_pInfo;

    /* Update the DEVMODE structure.  If it was NULL, it will be filled in with
     the defaults for the current printer. Otherwise we leave it alone */
  if (pInfo->pDevMode == NULL)
  {
	if (!btkUnicodeIsEnabled())
	{
	  DEVMODEA inDevMode;
      /*
       * First, set the size of the DevMode struct.
       * Then, set the default orientation to LANDSCAPE
       * we also set appropriate flags of the dmFields member
       */

	  inDevMode.dmSize = sizeof(DEVMODEA);

      /* Orientation */
      inDevMode.dmOrientation = DMORIENT_LANDSCAPE; 
      inDevMode.dmFields = DM_ORIENTATION;

      status = PglPrinterGetWin32Devmode(pInfo->psPrinterNames[pInfo->iCurrentPrinter],
                                 PGL_FALSE,
                                 &inDevMode,
                                 &(pInfo->pDevMode));
	}
	else /* unicode */
	{
      DEVMODEW inDevMode;
      /*
       * First, set the size of the DevMode struct.
       * Then, set the default orientation to LANDSCAPE
       * we also set appropriate flags of the dmFields member
       */

	  inDevMode.dmSize = sizeof(DEVMODEW);

      /* Orientation */
      inDevMode.dmOrientation = DMORIENT_LANDSCAPE; 
      inDevMode.dmFields = DM_ORIENTATION;

      status = PglPrinterGetWin32Devmode(pInfo->psPrinterNames[pInfo->iCurrentPrinter],
                                 PGL_FALSE,
                                 &inDevMode,
                                 &(pInfo->pDevMode));
	}
  }
#endif
  return status;
}

PglError pgl_win32_free_devmode(void * v_pInfo)
{
#if OPER_SYS == WINDOWS_32
  _PglWin32PrintmgrInfo * pInfo = (_PglWin32PrintmgrInfo *) v_pInfo;

  /* Remove devmode */
  if (pInfo->pDevMode != NULL)
    {
      relmem(&(pInfo->pDevMode));
      pInfo->pDevMode = NULL;
    }

#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}

PglError pgl_win32_img_config_update_from_devmode(_PglImgConfigDlg * p_ic_dlg)
{
#if OPER_SYS == WINDOWS_32
  _PglWin32PrintmgrInfo * pInfo =
    (_PglWin32PrintmgrInfo *) p_ic_dlg->win32_printmgr_info;
  char * * ppsSelection;
  char *portrait = "Portrait";
  char *landscape = "Landscape";
  char * paper_type_name;
  
  if (!btkUnicodeIsEnabled())
  {
	DEVMODEA * pDevMode = pInfo->pDevMode;

    /* Orientation */
    if (pDevMode->dmOrientation == DMORIENT_PORTRAIT)
    {
      p_ic_dlg->placement.orientation = 1;
      ppsSelection = &portrait;
    }
    else
    {
      p_ic_dlg->placement.orientation = 0;
      ppsSelection = &landscape;
    }
    ui_modify(_imgconf_dev, "r_orientation",
	    UI_sel_names_Attr, 1, ppsSelection, NULL);

    /* Number of copies */
    p_ic_dlg->quantity = pDevMode->dmCopies;
    ui_modify(_imgconf_dev, "i_quantity",
	      UI_integer_Attr, p_ic_dlg->quantity, NULL);

    /*  Scale Factor (Zoom) */

    p_ic_dlg->placement.magnification = pDevMode->dmScale / 100.;
    ui_modify(_imgconf_dev, "i_scale",
	    UI_input_type_Attr, UI_DOUBLE,
	    UI_double_Attr, &p_ic_dlg->placement.magnification, NULL);

    /*  Paper size */

    _get_pgl_paperdata(p_ic_dlg, &p_ic_dlg->paper_type, &p_ic_dlg->paper_unit,
                     &p_ic_dlg->paper_width, &p_ic_dlg->paper_height);

    /* Depth */
    p_ic_dlg->pix_depth_acc = PGL_IMG_24BIT_ACCESS;

    paper_type_name =  _get_size_name(p_ic_dlg->paper_type);

    /* Update UI components because we read UI components when OK button is pressed */
    ui_modify(_imgconf_dev, "i_height",
	    UI_double_Attr, &p_ic_dlg->paper_height,
	    UI_sensitive_Attr, FALSE,NULL);

    ui_modify(_imgconf_dev, "i_width",
	    UI_double_Attr, &p_ic_dlg->paper_width,
	    UI_sensitive_Attr, FALSE,NULL);

    ui_modify(_imgconf_dev, "o_size",
		        UI_sel_names_Attr, 1, &paper_type_name,
		        UI_sensitive_Attr, FALSE, NULL);
  }
  else /* unicode */
  {
	DEVMODEW * pDevMode = pInfo->pDevMode;

    /* Orientation */
    if (pDevMode->dmOrientation == DMORIENT_PORTRAIT)
    {
      p_ic_dlg->placement.orientation = 1;
      ppsSelection = &portrait;
    }
    else
    {
      p_ic_dlg->placement.orientation = 0;
      ppsSelection = &landscape;
    }
    ui_modify(_imgconf_dev, "r_orientation",
	    UI_sel_names_Attr, 1, ppsSelection, NULL);

    /* Number of copies */
    p_ic_dlg->quantity = pDevMode->dmCopies;
    ui_modify(_imgconf_dev, "i_quantity",
	      UI_integer_Attr, p_ic_dlg->quantity, NULL);

    /*  Scale Factor (Zoom) */

    p_ic_dlg->placement.magnification = pDevMode->dmScale / 100.;
    ui_modify(_imgconf_dev, "i_scale",
	    UI_input_type_Attr, UI_DOUBLE,
	    UI_double_Attr, &p_ic_dlg->placement.magnification, NULL);

    /*  Paper size */

    _get_pgl_paperdata(p_ic_dlg, &p_ic_dlg->paper_type, &p_ic_dlg->paper_unit,
                     &p_ic_dlg->paper_width, &p_ic_dlg->paper_height);

    /* Depth */
    p_ic_dlg->pix_depth_acc = PGL_IMG_24BIT_ACCESS;

    paper_type_name =  _get_size_name(p_ic_dlg->paper_type);

    /* Update UI components because we read UI components when OK button is pressed */
    ui_modify(_imgconf_dev, "i_height",
	    UI_double_Attr, &p_ic_dlg->paper_height,
	    UI_sensitive_Attr, FALSE,NULL);

    ui_modify(_imgconf_dev, "i_width",
	    UI_double_Attr, &p_ic_dlg->paper_width,
	    UI_sensitive_Attr, FALSE,NULL);

    ui_modify(_imgconf_dev, "o_size",
		        UI_sel_names_Attr, 1, &paper_type_name,
		        UI_sensitive_Attr, FALSE, NULL);
  }
#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}

PglError pgl_win32_img_config_update_devmode(_PglImgConfigDlg * p_ic_dlg)
{
  PglError status = PGL_E_OK;
#if OPER_SYS == WINDOWS_32
  _PglWin32PrintmgrInfo * pInfo =
    (_PglWin32PrintmgrInfo *) p_ic_dlg->win32_printmgr_info;
  char **sels;
  int cnt;
  char *portrait = "Portrait";

  if (!btkUnicodeIsEnabled())
  {
	DEVMODEA * pDevMode = pInfo->pDevMode;

    /* Orientation */
    pDevMode->dmFields |= DM_ORIENTATION;
    ui_retrieve(_imgconf_dev, "r_orientation",
	      UI_sel_names_Attr, &cnt, &sels, NULL);
    if (strcmp(sels[0], portrait) == 0)
      p_ic_dlg->placement.orientation = 1;
    else
      p_ic_dlg->placement.orientation = 0;

    pDevMode->dmOrientation = (p_ic_dlg->placement.orientation == 1) ?
      DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;

    /* Number of copies */
    ui_retrieve(_imgconf_dev, "i_quantity",
	      UI_integer_Attr, &p_ic_dlg->quantity, NULL);
    pDevMode->dmCopies = p_ic_dlg->quantity;
    pDevMode->dmFields |= DM_COPIES;

    /* Scale (or Zoom) factor */
    {
      double scale;
      ui_retrieve(_imgconf_dev, "i_scale", UI_double_Attr, &scale, NULL);
      pDevMode->dmScale = (int) (scale * 100.);
      pDevMode->dmFields |= DM_SCALE;
    }
  }
  else /* unicode */
  {
	DEVMODEW * pDevMode = pInfo->pDevMode;

    /* Orientation */
    pDevMode->dmFields |= DM_ORIENTATION;
    ui_retrieve(_imgconf_dev, "r_orientation",
	      UI_sel_names_Attr, &cnt, &sels, NULL);
    if (strcmp(sels[0], portrait) == 0)
      p_ic_dlg->placement.orientation = 1;
    else
      p_ic_dlg->placement.orientation = 0;

    pDevMode->dmOrientation = (p_ic_dlg->placement.orientation == 1) ?
      DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;

    /* Number of copies */
    ui_retrieve(_imgconf_dev, "i_quantity",
	      UI_integer_Attr, &p_ic_dlg->quantity, NULL);
    pDevMode->dmCopies = p_ic_dlg->quantity;
    pDevMode->dmFields |= DM_COPIES;

    /* Scale (or Zoom) factor */
    {
      double scale;
      ui_retrieve(_imgconf_dev, "i_scale", UI_double_Attr, &scale, NULL);
      pDevMode->dmScale = (int) (scale * 100.);
      pDevMode->dmFields |= DM_SCALE;
    }
  }
#endif /* OPER_SYS == WINDOWS_32 */
  return status;
}

PglError pgl_win32_free_printernames(void * v_pInfo)
{
#if OPER_SYS == WINDOWS_32
  int i;
  _PglWin32PrintmgrInfo * pInfo = (_PglWin32PrintmgrInfo *) v_pInfo;

  /* Remove printer names */
  if (pInfo->psPrinterNames != NULL)
    {
      for (i = 0; i < pInfo -> iNPrinters; i++)
	{
	  relmem(&(pInfo->psPrinterNames[i]));
	  relmem(&(pInfo->pwsPrinterLabels[i]));
	}
      relmem(&(pInfo->psPrinterNames));
      relmem(&(pInfo->pwsPrinterLabels));
      pInfo->psPrinterNames = NULL;
      pInfo->pwsPrinterLabels = NULL;
    }

#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}


PglError pgl_win32_init_pgl_printmgr_info(void * v_ppInfo)
{
#if OPER_SYS == WINDOWS_32
  _PglWin32PrintmgrInfo * * ppInfo = (_PglWin32PrintmgrInfo * *) v_ppInfo;
  _PglWin32PrintmgrInfo * pInfo;

  pInfo = getmem(sizeof(_PglWin32PrintmgrInfo));
  pInfo -> iNPrinters = 0;
  pInfo -> iCurrentPrinter = 0;
  pInfo -> psPrinterNames = NULL;
  pInfo -> pwsPrinterLabels = NULL;
  /*  pInfo -> iNResolutions = 0;
  pInfo -> iCurrentResolution = 0;
  pInfo -> piResolutionValues = NULL;
  pInfo -> psResolutionNames = NULL;
  pInfo -> pwsResolutionLabels = NULL;
  pInfo -> iNPapers = 0;
  pInfo -> iCurrentPaper = 0;
  pInfo -> psPaperNames = NULL;
  pInfo -> pwsPaperLabels = NULL;
  pInfo -> fCurrentWidth = 0.0;
  pInfo -> fCurrentHeight = 0.0;
  pInfo -> bLandscape = 1;*/
  pInfo -> pDevMode = NULL;
  *ppInfo = pInfo;

#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}

PglError pgl_win32_copy_pgl_printmgr_info(void * v_pInfoDestination,
					  void * v_pInfoSource)
{
  PglError status = PGL_E_OK;
#if OPER_SYS == WINDOWS_32

  int i;
  int iNChar;
  _PglWin32PrintmgrInfo * pInfoDestination = (_PglWin32PrintmgrInfo *) v_pInfoDestination;
  _PglWin32PrintmgrInfo * pInfoSource = (_PglWin32PrintmgrInfo *) v_pInfoSource;

  /* Printers */
  pgl_win32_free_printernames(pInfoDestination);

  pInfoDestination -> iNPrinters = pInfoSource -> iNPrinters;
  pInfoDestination -> iCurrentPrinter = pInfoSource -> iCurrentPrinter;
  pInfoDestination -> psPrinterNames =
    getmem(sizeof(char *) * pInfoSource -> iNPrinters);
  pInfoDestination -> pwsPrinterLabels =
    getmem(sizeof(wchar_t *) * pInfoSource -> iNPrinters);
  for (i = 0; i <  pInfoSource -> iNPrinters; i++)
    {
      iNChar = strlen(pInfoSource -> psPrinterNames[i]) + 1;
      pInfoDestination -> psPrinterNames[i] = getmem(sizeof(char) * iNChar);
      strcpy(pInfoDestination -> psPrinterNames[i],
	     pInfoSource -> psPrinterNames[i]);
      pInfoDestination -> pwsPrinterLabels[i] = getmem(sizeof(wchar_t) * iNChar);
      wstrcpy(pInfoDestination -> pwsPrinterLabels[i],
	      pInfoSource -> pwsPrinterLabels[i]);
    }

  pgl_win32_free_devmode(pInfoDestination);

  if (pInfoDestination->iNPrinters == 0)
  {
    pInfoDestination->pDevMode = NULL;
  }
  else
  {
    status = PglPrinterGetWin32Devmode(pInfoDestination->psPrinterNames[pInfoDestination->iCurrentPrinter],
                                 PGL_FALSE,
                                 pInfoSource->pDevMode,
                                 &(pInfoDestination->pDevMode));
  }
#endif /* OPER_SYS == WINDOWS_32 */
  return status;
}

/* Get the dimensions of the currently selected page size */
PglError pgl_win32_img_config_get_printer_page_dimensions(void * v_pInfo)
{
#if OPER_SYS == WINDOWS_32
  int i;
  double fWidth, fHeight;
  _PglWin32PrintmgrInfo * pInfo = (_PglWin32PrintmgrInfo *) v_pInfo;

  if (!btkUnicodeIsEnabled())
  {
	DEVMODEA *pDevMode = pInfo->pDevMode;
    if (pDevMode->dmOrientation == DMORIENT_PORTRAIT)
    {
      fHeight = pDevMode->dmPaperLength;
      fWidth = pDevMode->dmPaperWidth;
    }
    else
    {
      fWidth = pDevMode->dmPaperLength;
      fHeight = pDevMode->dmPaperWidth;
    }
  }
  else /* unicode */
  {
    DEVMODEW *pDevMode = pInfo->pDevMode;
    if (pDevMode->dmOrientation == DMORIENT_PORTRAIT)
    {
      fHeight = pDevMode->dmPaperLength;
      fWidth = pDevMode->dmPaperWidth;
    }
    else
    {
      fWidth = pDevMode->dmPaperLength;
      fHeight = pDevMode->dmPaperWidth;
    }
  }
  ui_modify(_imgconf_dev, "i_height",
	    UI_double_Attr, &fHeight,
	    UI_sensitive_Attr, FALSE,
	    NULL);

  ui_modify(_imgconf_dev, "i_width",
	    UI_double_Attr, &fWidth,
	    UI_sensitive_Attr, FALSE,
	    NULL);
#endif /* OPER_SYS == WINDOWS_32 */
  return PGL_E_OK;
}

/* ------------------------------------------------------------------------ *
 * Static Internal function
 *
 * _get_pgl_paperdata - Converts a DEVMODE struct to a PGL paper definition
 *
 * input:
 *   p_ic_dlg - the Image Config Dialog struct containing the DEVMODE
 *              struct from which to obtain data
 *
 * output:
 *   paper_type  - the PglPapertype equivalent to the dmPapertype
 *   paper_unit  - units in which width and height are defined
 *   paper_width - width of page (defined by Portrait or Landscape orientation)
 *   paper_height - height of page
 */
static PglError _get_pgl_paperdata(_PglImgConfigDlg *p_ic_dlg,
                                   PglPapertype *paper_type,
                                   PglUnittype  *paper_unit,
                                   double *paper_width, double *paper_height)

{
#if OPER_SYS == WINDOWS_32
  _PglWin32PrintmgrInfo *pInfo =
    (_PglWin32PrintmgrInfo *) p_ic_dlg->win32_printmgr_info;
  double fHeight, fWidth, fTemp;
  POINT * piPaperDimensions;
  int iNPaperSizes, nPapers;
  WORD *array_dmPaperSize;
  int num_dmPaperSize, i, idx;
  PglBool paper_defined = PGL_FALSE;
  PglError status = PGL_E_OK;

  if (!btkUnicodeIsEnabled())
  {
	DEVMODEA *pDevMode = pInfo->pDevMode;

    /*
     * First, see if this is a User-Defined Width and height
     */
    if (pDevMode->dmPaperSize == 0)
    {
      *paper_type = PGL_VAR_SIZE_MM_PAPER;
      *paper_unit = PGL_UNIT_MM;
      /* DevMode values are sometimes set to 0.0 possibly due to print */
      /* server errors. Use default size in such case to avoid crash.  */
      if (pDevMode->dmPaperWidth <= 0.0 || pDevMode->dmPaperLength <= 0.0)
      {
      *paper_width = fWidthDefault;
      *paper_height = fHeightDefault;
      }
      else
      {
      *paper_width = pDevMode->dmPaperWidth * 0.1;
      *paper_height = pDevMode->dmPaperLength * 0.1;
      }
      return PGL_E_OK;
    }

    /*
     * Since this is NOT a user-defined hHeight and Width, we first see if
     * these are defined by PGL data types (pgl_export.h).  If they are, we
     * set the Width and Height appropriately.
     *
     * -------------- * NOTE * NOTE * NOTE * NOTE * NOTE * -----------------*
     * | The MSPrinter is expects to be in "portrait" mode by default. That |
     * | is the Width is less than the height, and it uses the value of     | 
     * | "dmOrientation" in the DEVMODE struct to tell it  whether to print | 
     * | rotated, or not.  The paper orientation in PgluPaperGetSize()      |
     * | is Landscape by default.  Flip the Height and Width retrieval      |
     * -------------- * NOTE * NOTE * NOTE * NOTE * NOTE * -----------------*
     */

    switch (pDevMode->dmPaperSize)
    {
    case DMPAPER_LETTER:
      *paper_type = PGL_A_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_A_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_11X17:
      *paper_type = PGL_B_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_B_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;
 
    case DMPAPER_A3:
      *paper_type = PGL_A3_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_A3_SIZE_PAPER, paper_unit,
                              &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_A4:
      *paper_type = PGL_A4_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_A4_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_CSHEET:
      *paper_type = PGL_C_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_C_SIZE_PAPER, paper_unit,
                              &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_DSHEET:
      *paper_type = PGL_D_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_D_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_ESHEET:
      *paper_type = PGL_E_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_E_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    default:
      *paper_type = PGL_VAR_SIZE_MM_PAPER;
      *paper_unit = PGL_UNIT_MM;
      fWidth = fWidthDefault;
      fHeight = fHeightDefault;
      break;
    }
    /*
     * If we got here, then we are not at a paper size that is pre-defined
     * by the PGL, nor is it a user-defined paper.  Get paper information
     * from the device itself.
     */
    if (!paper_defined)
    {
      /* Read paper size code array */
      num_dmPaperSize = DeviceCapabilitiesA(pDevMode->dmDeviceName,
                                           NULL,
                                           DC_PAPERS,
                                           NULL,
                                           NULL);

      if(num_dmPaperSize > 0)
      {
        array_dmPaperSize = (WORD *)getmem(sizeof(WORD) * num_dmPaperSize);
        num_dmPaperSize = DeviceCapabilitiesA(pDevMode->dmDeviceName,
                                           NULL,
                                           DC_PAPERS,
                                           (LPSTR)array_dmPaperSize,
                                           NULL);

        /* Find current Paper size code in the array. Now idx has the index. */
        idx = -1;
        for(i=0; i<num_dmPaperSize; i++)
        {
          if(array_dmPaperSize[i] == pDevMode->dmPaperSize)
          {
            idx = i;
            break;
          }
        }

        /* Read size(x,y) array */
        iNPaperSizes = DeviceCapabilitiesA(pDevMode->dmDeviceName,
                                        NULL,
                                        DC_PAPERSIZE,
                                        NULL,
                                        NULL);

        if(iNPaperSizes > 0 )
        {
          piPaperDimensions = getmem(sizeof(POINT) * iNPaperSizes);

          /*
           * Three steps to retrieve correct paper size:
           *  1. Get Paper Dimensions from DeviceCapabilities(),
           *  2. If Paper Index is included in returned Dimensions, use it
           *  3. Use default A paper
           */

          nPapers = DeviceCapabilitiesA(pInfo->psPrinterNames[pInfo->iCurrentPrinter],
                                    NULL,
                                    DC_PAPERSIZE,
                                    piPaperDimensions,
                                    NULL);
          if(nPapers > 0) 
          {
            /*
             * When DeviceCapabilities() is successful, The pOutput buffer
             * (piPaperDimensions) receives an array of POINT structures.
             * Each structure contains the width (x-dimension) and 
             * length (y-dimension) of a paper size as if the paper were 
             * in the DMORIENT_PORTRAIT orientation.
             */
            if((idx >= 0) && (idx < nPapers))
            {
              fWidth = piPaperDimensions[idx].x * 0.1;
              fHeight = piPaperDimensions[idx].y * 0.1;
            }
          }

          relmem(&piPaperDimensions);
          relmem(&array_dmPaperSize);
        }
      }

      /*
       * If we got here, then check to see if the dmPaperWidth/Length is valid
       * Otherwise, the defaults defined in the original SWITCH() will rule.
       */
      else if((pDevMode->dmPaperWidth != 0.0) && 
              (pDevMode->dmPaperLength != 0.0))
      {
        fWidth = pDevMode->dmPaperWidth * 0.1;
        fHeight = pDevMode->dmPaperLength * 0.1;
      }
    }
    /*
     * Flip the Width and Height, if this is a "Landscape" orientation
     */
    if (pDevMode->dmOrientation == DMORIENT_LANDSCAPE)
    {
      fTemp = fWidth;
      fWidth = fHeight;
      fHeight = fTemp;
    }

    *paper_width = fWidth;
    *paper_height = fHeight;
  }
  else /* unicode */
  {
	DEVMODEW *pDevMode = pInfo->pDevMode;

    /*
     * First, see if this is a User-Defined Width and height
     */
    if (pDevMode->dmPaperSize == 0)
    {
      *paper_type = PGL_VAR_SIZE_MM_PAPER;
      *paper_unit = PGL_UNIT_MM;
      /* DevMode values are sometimes set to 0.0 possibly due to print */
      /* server errors. Use default size in such case to avoid crash.  */
      if (pDevMode->dmPaperWidth <= 0.0 || pDevMode->dmPaperLength <= 0.0)
      {
      *paper_width = fWidthDefault;
      *paper_height = fHeightDefault;
      }
      else
      {
      *paper_width = pDevMode->dmPaperWidth * 0.1;
      *paper_height = pDevMode->dmPaperLength * 0.1;
      }
      return PGL_E_OK;
    }

    /*
     * Since this is NOT a user-defined hHeight and Width, we first see if
     * these are defined by PGL data types (pgl_export.h).  If they are, we
     * set the Width and Height appropriately.
     *
     * -------------- * NOTE * NOTE * NOTE * NOTE * NOTE * -----------------*
     * | The MSPrinter is expects to be in "portrait" mode by default. That |
     * | is the Width is less than the height, and it uses the value of     | 
     * | "dmOrientation" in the DEVMODE struct to tell it  whether to print | 
     * | rotated, or not.  The paper orientation in PgluPaperGetSize()      |
     * | is Landscape by default.  Flip the Height and Width retrieval      |
     * -------------- * NOTE * NOTE * NOTE * NOTE * NOTE * -----------------*
     */

    switch (pDevMode->dmPaperSize)
    {
    case DMPAPER_LETTER:
      *paper_type = PGL_A_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_A_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_11X17:
      *paper_type = PGL_B_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_B_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;
 
    case DMPAPER_A3:
      *paper_type = PGL_A3_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_A3_SIZE_PAPER, paper_unit,
                              &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_A4:
      *paper_type = PGL_A4_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_A4_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_CSHEET:
      *paper_type = PGL_C_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_C_SIZE_PAPER, paper_unit,
                              &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_DSHEET:
      *paper_type = PGL_D_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_D_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    case DMPAPER_ESHEET:
      *paper_type = PGL_E_SIZE_PAPER;
      status = PgluPaperGetSize(PGL_E_SIZE_PAPER, paper_unit,
                                &fHeight, &fWidth);
      if (status == PGL_E_OK)
        paper_defined = PGL_TRUE;
      break;

    default:
      *paper_type = PGL_VAR_SIZE_MM_PAPER;
      *paper_unit = PGL_UNIT_MM;
      fWidth = fWidthDefault;
      fHeight = fHeightDefault;
      break;
    }
    /*
     * If we got here, then we are not at a paper size that is pre-defined
     * by the PGL, nor is it a user-defined paper.  Get paper information
     * from the device itself.
     */
    if (!paper_defined)
    {
      /* Read paper size code array */
      num_dmPaperSize = DeviceCapabilitiesW(pDevMode->dmDeviceName,
                                           NULL,
                                           DC_PAPERS,
                                           NULL,
                                           NULL);

      if(num_dmPaperSize > 0)
      {
        array_dmPaperSize = (WORD *)getmem(sizeof(WORD) * num_dmPaperSize);
        num_dmPaperSize = DeviceCapabilitiesW(pDevMode->dmDeviceName,
                                           NULL,
                                           DC_PAPERS,
                                           array_dmPaperSize,
                                           NULL);

        /* Find current Paper size code in the array. Now idx has the index. */
        idx = -1;
        for(i=0; i<num_dmPaperSize; i++)
        {
          if(array_dmPaperSize[i] == pDevMode->dmPaperSize)
          {
            idx = i;
            break;
          }
        }

        /* Read size(x,y) array */
        iNPaperSizes = DeviceCapabilitiesW(pDevMode->dmDeviceName,
                                        NULL,
                                        DC_PAPERSIZE,
                                        NULL,
                                        NULL);

        if(iNPaperSizes > 0 )
        {
		  wchar_t *lptext1 = btk_u8stou16s_alloc(pInfo->psPrinterNames[pInfo->iCurrentPrinter]);
          piPaperDimensions = getmem(sizeof(POINT) * iNPaperSizes);

          /*
           * Three steps to retrieve correct paper size:
           *  1. Get Paper Dimensions from DeviceCapabilities(),
           *  2. If Paper Index is included in returned Dimensions, use it
           *  3. Use default A paper
           */

          nPapers = DeviceCapabilitiesW(lptext1,
                                    NULL,
                                    DC_PAPERSIZE,
                                    (LPWSTR)piPaperDimensions,
                                    NULL);
		  rlsmem(lptext1);
          if(nPapers > 0) 
          {
            /*
             * When DeviceCapabilities() is successful, The pOutput buffer
             * (piPaperDimensions) receives an array of POINT structures.
             * Each structure contains the width (x-dimension) and 
             * length (y-dimension) of a paper size as if the paper were 
             * in the DMORIENT_PORTRAIT orientation.
             */
            if((idx >= 0) && (idx < nPapers))
            {
              fWidth = piPaperDimensions[idx].x * 0.1;
              fHeight = piPaperDimensions[idx].y * 0.1;
            }
          }

          relmem(&piPaperDimensions);
          relmem(&array_dmPaperSize);
        }
      }

      /*
       * If we got here, then check to see if the dmPaperWidth/Length is valid
       * Otherwise, the defaults defined in the original SWITCH() will rule.
       */
      else if((pDevMode->dmPaperWidth != 0.0) && 
              (pDevMode->dmPaperLength != 0.0))
      {
        fWidth = pDevMode->dmPaperWidth * 0.1;
        fHeight = pDevMode->dmPaperLength * 0.1;
      }
    }
    /*
     * Flip the Width and Height, if this is a "Landscape" orientation
     */
    if (pDevMode->dmOrientation == DMORIENT_LANDSCAPE)
    {
      fTemp = fWidth;
      fWidth = fHeight;
      fHeight = fTemp;
    }

    *paper_width = fWidth;
    *paper_height = fHeight;
  }

#else
  /*
   * simply return default type and unit for non-Windows
   */
  *paper_type = PGL_VAR_SIZE_MM_PAPER;
  *paper_unit = PGL_UNIT_MM;
  *paper_width = fWidthDefault;
  *paper_height = fHeightDefault;
#endif /* OPER_SYS == WINDOWS_32 */
  
  return PGL_E_OK;
}
