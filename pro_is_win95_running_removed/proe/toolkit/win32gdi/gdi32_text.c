/******************************************************************************
 *
 * FILE:      gdi32_text.c
 *
 * DESC:      All the text drawing functions in the WIN32_GDI graphics interface.
 *
 *
 * History:
 *
 * Date      Version    Author              Description of modification
 * -----------------------------------------------------------------------------
 *
 * 14-Sep-93 D-02-48    CJH         $$1 Created.
 * 12-Jan-94 E-03-06    mjb         $$2 macros for gdi data
 * 12-Dec-93 E-01-31    CJH         $$3 Support For UNICODE/Japanese
 * 31-Mar-95 E-07-09    dac         $$4 fixed menu height/width
 * 10-Oct-95 G-01-09    amin        $$5 Fix gdi32_get_text_outline_for_font &
 *                                      gdi32_get_text_outline for Windows 95
 * 17-Nov-96 H-01-17    amin        $$6 Fix releasing of resources
 * 05-Oct-98 I-01-21    cm          $$7 use Gdi32SetFont
 * 21-Sep-01 J-03-09    jas         $$8 Removed WINDOWS_95 macro
 * 24-Apr-06 L-01-07    TWH         $$9 use windows unicode wrappers
 * 12-Jul-06 L-01-12    ksi         $$10 Unicode compliant changes
 * 12-Mar-12 P-20-01    AC          $$11 Updated for Project 13028358
 * 13-Nov-20 P-80-29    jas         $$12 Fixed compilation warnings
 * 25-Mar-21 P-90-05    jas         $$13 Removed unused code
 *
 ******************************************************************************/
#include <ptc_win32.h>
#include <hardware.h>
#if OPER_SYS == WINDOWS_32
#include <win32gdi.h>
#include <windmac.h>
#include <const.h>
#include <pro_widec.h>
#include <ct_win_syscall_proto.h>

/******************************************************************************
 *
 * Function: gdi32_get_text_outline_for_font
 *
 * Desc    : Get the height and width of the string given, in the font given.
 *
 * Args in :
 *
 * Args out:
 *
 * Returns :
 *
 ******************************************************************************/
int gdi32_get_text_outline_for_font(struct window *window_ptr,int font_id,
                                    wchar_t *string,double *width,double *height)
{
  Gdi32WindowInfo *dep_struct;
  TEXTMETRIC tmet;
  SIZE tsize;
  HDC  theDC;

  if(window_ptr)
  {
    dep_struct = WINDOW_DEP_STRUCTURE(window_ptr);
    theDC = dep_struct->hDC;
  }
  else
  {
    theDC = uCreateDC("DISPLAY",NULL,NULL,NULL);
    Gdi32SetFont(theDC,font_id,0);
  }

  GetTextMetrics(theDC,&tmet);
  GetTextExtentPointW(theDC,string,wstrlen(string),&tsize);
  if (width) *width = (double)tsize.cx;
  if (height) *height = (double)tsize.cy;

  if (!window_ptr)
  {
    Gdi32SetFont(theDC, -1, 0);
    DeleteDC(theDC);
  }
  return E_NO_ERROR;
}

#endif
