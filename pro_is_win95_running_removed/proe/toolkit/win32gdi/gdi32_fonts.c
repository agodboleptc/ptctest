/*******************************************************************************/
/*
 * FILE:     gdi32_fonts.c
 *
 * DESC:     Contains functions for manipulating the fonts we use, including
 *           maintaining our tables of FONT handles, and also creating
 *           specified fonts.
 *
 *
 * History:
 *
 * Date      Version    Author              Description of modification
 * -----------------------------------------------------------------------------
 *
   14-Sep-93 D-02-48    CJH         $$1 Created.
   10-Jan-95 E-07-04    CJL         $$2 Added gdi32_change_current_font() and
                                        gdi32_restore_default_font().
   16-Mar-95 E-07-08    CJL         $$3 Fixed default fonts.
                                        Added menuitem_font config support.
   25-Jul-95 G-01-02    CFK         $$4 changed default fonts for Windows95
   17-Aug-95 G-01-03 jmichaud       $$5 Use XAR_COUNT() and BYTCPY() macros.
   12-Dec-95 G-01-17    Amin        $$6 changed default fonts for Windows95
   09-Feb-96 G-03-03    Amin        $$7 added fonts to be used for screen width
                                        less than 1024
   11-Aug-96 H-01-04    Amin        $$8 Fix font change on Windows 95
   17-Nov-96 H-01-17    amin        $$9 Fix releasing of resources
   26-Nov-96 H-01-18    GAA        $$10 Changed fonts and font sizes.
   03-Dec-96 H-01-19    GAA        $$11 Preserve old fonts under old UI
   16-Jan-97 H-01-24    GAA        $$12 Changed DEFAULT_FONT to "Courier New"
   15-Jan-97 H-01-24    YHC        $$13 Added generic language fonts.
   22-Jan-97 H-01-24    YHC        $$14 Fixed comp. errors.
   20-Jan-98 H-03-37    CJL        $$15 Fixed incorrect font names.
   05-Oct-98 I-01-21    cm         $$16 worked on resource leaks
   18-Nov-98 I-01-28    FBI    $$17 Use is_generic_language to chk GENERIC_LANG
   09-Jan-01 J-01-25    arm    $$18 Use language to define CharSet in SetFont()
   01-Feb-01 J-01-26    arm    $$19 Fix font issues for Win2K and WinNt.
   21-Sep-01 J-03-09    jas    $$20 Removed WINDOWS_95 macro
   24-Apr-06 L-01-07    TWH    $$21 use windows unicode wrappers
   12-Jul-06 L-01-12    ksi    $$22 Unicode compliant changes
   22-Mar-11 L-05-44    Shturm $$23 Prototyping.
   12-Mar-12 P-20-01    AC     $$24 Updated for Project 13028358
   14-Jan-14 P-20-46    lli    $$25 Added missing return statements
   25-Mar-21 P-90-05    jas    $$26 Removed unused code
   14-Feb-22 Q-10-01    jas    $$27 Fixed clang issues
   09-Mar-26 Q-27-00    PROTO  $$28 Automatic prototype creation
*/
/*******************************************************************************/

#include <ptc_win32.h>
#include <hardware.h>
#include <state2_proto.h>
#include <menumgr_proto.h>
#if OPER_SYS == WINDOWS_32
#include <win32gdi.h>
#include <pro_string.h>
#include <font_sizes.h>
#include <languages.h>
#include <ctwcfun_proto.h>
#include <ct_win_syscall_proto.h>
#include <const.h>
#include <dbg_crash.h>


typedef struct gdi32_win_font_
{
  long id;
  HFONT hFnt[2];
  LOGFONTA fonts[2];
} Gdi32WinFont;

/*
 * Our standard font tables.
 * Maybe we should adjust them if the display is 1280x1024 rather than
 * 1024x768, but we can do that in Gdi32InitializeStdFonts
 */

#define DEFAULT_FONT_NAME "Courier New"
#define MENU_FONT_NAME    "Arial"
#define TITLE_FONT_NAME   "Arial"

#define DEFAULT_FONT_SIZE -14
#define MENU_FONT_SIZE    -12
#define TITLE_FONT_SIZE   -12

#define SMALL_FONT_SIZE_OFFSET  3
#define LARGE_FONT_SIZE_OFFSET -2


/*
 * Defines for Old UI
 */
#define OLD_DEFAULT_FONT_NAME "FixedSys"
#define OLD_DEFAULT_FONT_SIZE (-18)

#define  OLD_LAPTOP_FONT_NAME "Arial"
#define  OLD_LAPTOP_FONT_SIZE -11


Gdi32WinFont font_table[6] =
{
  {DEFAULT_FONT,{(HFONT)-1,(HFONT)-1},
   {{DEFAULT_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), DEFAULT_FONT_NAME},
    {DEFAULT_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), DEFAULT_FONT_NAME}}},
  {TITLE_FONT,{(HFONT)-1,(HFONT)-1},
   {{TITLE_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), TITLE_FONT_NAME},
    {TITLE_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), TITLE_FONT_NAME}}},
  {MENUITEM_FONT,{(HFONT)-1,(HFONT)-1},
   {{MENU_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), MENU_FONT_NAME},
    {MENU_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), MENU_FONT_NAME}}},
  {ALT_DEFAULT_FONT,{(HFONT)-1,(HFONT)-1},
   {{DEFAULT_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), DEFAULT_FONT_NAME},
    {DEFAULT_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), DEFAULT_FONT_NAME}}},
  {ALT_TITLE_FONT,{(HFONT)-1,(HFONT)-1},
   {{TITLE_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), TITLE_FONT_NAME},
    {TITLE_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), TITLE_FONT_NAME}}},
  {ALT_MENUITEM_FONT,{(HFONT)-1,(HFONT)-1},
   {{MENU_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), MENU_FONT_NAME},
    {MENU_FONT_SIZE,0,0,0,400,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
     DRAFT_QUALITY,(VARIABLE_PITCH | FF_ROMAN), MENU_FONT_NAME}}},
};
#define NUM_GDI32_STD_FONTS 6

/*
 * This function will return the character set use by LOGFONT
 * based on the language that is current.  The version check
 * sets this ONLY for Windows 2000 (and WinME)  We might want to
 * put in the additionsl check :
 *               info.dwPlatformId == VER_PLATFORM_WIN32_NT
 * if we find that Windows NT is having problems as well.
 *
*/

static int get_charset_from_lang (void)
{
  static int      charset = -1;
  OSVERSIONINFOA   info;

  if (charset == -1)
  {
    info.dwOSVersionInfoSize = sizeof (OSVERSIONINFOA);

    uGetVersionEx (&info);

    if (info.dwMajorVersion >= 5)
    {
      switch (get_language ())
      {
        case JAPANESE:
            charset = SHIFTJIS_CHARSET;
            break;

        case KOREAN:
            charset = HANGUL_CHARSET;
            break;

        case CHINESE_CN:
            charset = GB2312_CHARSET;
            break;

        case CHINESE_TW:
            charset = CHINESEBIG5_CHARSET;
            break;

        case HEBREW:
            charset = HEBREW_CHARSET;
            break;

        case RUSSIAN:
            charset = RUSSIAN_CHARSET;
            break;

        default:
            charset = ANSI_CHARSET;
      }
    }
    else
      charset = ANSI_CHARSET;
  }
  return (charset);
}

static LOGFONTA gdi32_logfont;
/********************************************************************************/
static LOGFONTA *gdi32_parse_font_string(font_string)
char *font_string;
/********************************************************************************/
{
   /* Font string format: "-FaceName-Height-Weight-Italic" */
   /* FaceName must not exceed 32 characters in length.    */
   /* Height is an integer value.                          */
   /* Weight is an integer value.                          */
   /* Italic is a boolean(0 or 1) value.                   */

   char local_font_string[GDI32_FONT_STRING_SIZE], face_name[33], *begin, *end;
   int height, weight, italic, status;

   status = E_NO_ERROR;
   if (font_string == NULL)
      status = E_ERROR;
   else
   {
      strcpy(local_font_string, font_string);
      face_name[0] = '\0';
      height = 0;
      weight = 0;
      italic = FALSE;

      /* Parse font string. */
      end = local_font_string;

      begin = end + 1;
      end = strchr(begin, '-');
      if (end == NULL)
         status = E_ERROR;
      else
      {
         *end = '\0';
         strcpy(face_name, begin);

         begin = end + 1;
         end = strchr(begin, '-');
         if (end == NULL)
            status = E_ERROR;
         else
         {
            *end = '\0';
            height = -atoi(begin);

            begin = end + 1;
            end = strchr(begin, '-');
            if (end == NULL)
               status = E_ERROR;
            else
            {
               *end = '\0';
               weight = atoi(begin);

               begin = end + 1;
               italic = atoi(begin);
            }
         }
      }
   }

   /* Create static LOGFONT structure. */
   gdi32_logfont.lfWidth = 0;
   gdi32_logfont.lfEscapement = 0;
   gdi32_logfont.lfOrientation = 0;
   gdi32_logfont.lfUnderline = FALSE;
   gdi32_logfont.lfStrikeOut = FALSE;
   gdi32_logfont.lfCharSet = ANSI_CHARSET;
   gdi32_logfont.lfOutPrecision = OUT_STROKE_PRECIS;
   gdi32_logfont.lfClipPrecision = CLIP_STROKE_PRECIS;
   gdi32_logfont.lfQuality = DRAFT_QUALITY;
   gdi32_logfont.lfPitchAndFamily = (VARIABLE_PITCH | FF_ROMAN);

   if (status == E_NO_ERROR)
   {  /* set parsed info */
      gdi32_logfont.lfHeight = height;
      gdi32_logfont.lfWeight = weight;
      gdi32_logfont.lfItalic = italic;
      strcpy(gdi32_logfont.lfFaceName, face_name);
   }
   else
   {  /* set default info */
      gdi32_logfont.lfHeight = 18;
      gdi32_logfont.lfWeight = 400;
      gdi32_logfont.lfItalic = 0;
      strcpy(gdi32_logfont.lfFaceName, "FixedSys");
   }

   return(&gdi32_logfont);
}


/*******************************************************************************/
/*
 * Function: Gdi32InitializeStdFonts
 * Desc    : Creates Font Handles for each of the fonts which we have in our
 *           table, from the LOGFONT structure in the table.
 *
 *******************************************************************************/
int Gdi32InitializeStdFonts()
{
   int  i, check_menuitem_font;
   int  size_of_fonts, screen_width;
   int  curr_lang_id;
   int  win2k_charset = get_charset_from_lang();
   char font_string[GDI32_FONT_STRING_SIZE];
   char	*def_font, *menu_font;

    check_menuitem_font = TRUE;
    curr_lang_id = get_language();
    if (is_generic_language(curr_lang_id))
    {
	def_font = get_generic_language_font( DEFAULT_FONT, 0 );
	menu_font = get_generic_language_font( MENUITEM_FONT, 0 );
	if (   def_font != NULL
	    || menu_font != NULL)
	{
	    /* generic_language_font takes precedence of menuitem_font. */
	    check_menuitem_font = FALSE;
	}
	if (def_font != NULL)
	{
	    BYTCPY(&(font_table[ DEFAULT_FONT ].fonts[0]),
		   gdi32_parse_font_string(def_font), sizeof( LOGFONTA ));
	    if (menu_font == NULL)
		BYTCPY(&(font_table[ MENUITEM_FONT ].fonts[0]),
		       gdi32_parse_font_string(def_font), sizeof( LOGFONTA ));
	}
	if (menu_font != NULL)
	    BYTCPY(&(font_table[ MENUITEM_FONT ].fonts[0]),
		   gdi32_parse_font_string(menu_font), sizeof( LOGFONTA ));
    }
    if (   check_menuitem_font
	&& get_config_menuitem_font_name(0, font_string))
	BYTCPY(&(font_table[ MENUITEM_FONT ].fonts[0]),
	       gdi32_parse_font_string(font_string), sizeof( LOGFONTA ));

   if( menu_dialog_enabled() )
   {
      size_of_fonts = get_size_of_fonts_to_use();

      for (i=0; i<NUM_GDI32_STD_FONTS; i++)
      {
         if ( size_of_fonts == USE_SMALL_FONTS )
         {
            font_table[i].fonts[0].lfHeight += SMALL_FONT_SIZE_OFFSET;
            font_table[i].fonts[1].lfHeight += SMALL_FONT_SIZE_OFFSET;
         }
         else if ( size_of_fonts == USE_LARGE_FONTS )
         {
            font_table[i].fonts[0].lfHeight += LARGE_FONT_SIZE_OFFSET;
            font_table[i].fonts[1].lfHeight += LARGE_FONT_SIZE_OFFSET;
         }

         font_table[i].hFnt[0] = uCreateFontIndirect(&(font_table[i].fonts[0]));
         font_table[i].fonts[1].lfCharSet = win2k_charset;
         font_table[i].hFnt[1] = uCreateFontIndirect(&(font_table[i].fonts[1]));
      }
   }
   else
   {
      screen_width = GetSystemMetrics(SM_CXSCREEN);

      for (i=0; i<NUM_GDI32_STD_FONTS; i++)
      {
         if ( screen_width < 1024 )
         {
            strcpy(font_table[i].fonts[0].lfFaceName, OLD_LAPTOP_FONT_NAME);
            strcpy(font_table[i].fonts[1].lfFaceName, OLD_LAPTOP_FONT_NAME);
            font_table[i].fonts[0].lfHeight = OLD_LAPTOP_FONT_SIZE;
            font_table[i].fonts[1].lfHeight = OLD_LAPTOP_FONT_SIZE;
         }
         else
         {
            strcpy(font_table[i].fonts[0].lfFaceName, OLD_DEFAULT_FONT_NAME);
            strcpy(font_table[i].fonts[1].lfFaceName, OLD_DEFAULT_FONT_NAME);
            font_table[i].fonts[0].lfHeight = OLD_DEFAULT_FONT_SIZE;
            font_table[i].fonts[1].lfHeight = OLD_DEFAULT_FONT_SIZE;
         }

         font_table[i].hFnt[0] = uCreateFontIndirect(&(font_table[i].fonts[0]));
         font_table[i].hFnt[1] = uCreateFontIndirect(&(font_table[i].fonts[1]));
      }
   }

   return (E_NO_ERROR);
}

void Gdi32DeleteStdFonts()
{
   int i;

   for (i=0; i<NUM_GDI32_STD_FONTS; i++)
   {
      DeleteObject(font_table[i].hFnt[0]);
      DeleteObject(font_table[i].hFnt[1]);
   }
}


/*******************************************************************************/
/*
 * Function: Gdi32SetFont
 * Desc    : Given a font_id and a code page, this functions sets the font for
 * the given DC.
 *
 *******************************************************************************/
void Gdi32SetFont(void *hDC, long font_id, int code_page)
{
  HFONT font_to_set;
  OSVERSIONINFOA   info;

  info.dwOSVersionInfoSize = sizeof (OSVERSIONINFOA);
  uGetVersionEx (&info);

  /*
   * SPR 857635 && 844972 - Japanese font problems
   *
   * I have not seen the use of any code page except 0, so just to be safe,
   * verify that it is always 0 with dbg_err_crash() check.
   * Set code page to "1" if we are on Win2K system to support MUI.
   */
  if (code_page != 0)
    dbg_err_crash("Gdi32SetFont","Surprize! Code_page is not 0");

  if (info.dwMajorVersion >= 5)
    code_page = 1;

  if (font_id == -1)
    font_to_set = GetStockObject(SYSTEM_FONT);
  else
    font_to_set = font_table[font_id].hFnt[code_page];

  /*
   * If we are on Win2K, or this is NOT Japanese, set the font here.
   * (this is a carry-over of old logic as was done earlier.
   */
  if (info.dwMajorVersion >= 5 || get_language() != JAPANESE)
    SelectObject((HDC)hDC, font_to_set);
}

#endif
