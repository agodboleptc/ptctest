/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    uint_fonts.c
|
|  Purpose: Windows NT level font functions
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  13-Feb-96         jas         Created
|  14-Feb-96         jas         Added sysdep_font_init
|  21-Feb-96 G-03-03 UK    $$1   Automatic Submission
|   3-Jul-96         pch         Enhance to support more fonts
|   4-Jul-96         pch         Add _uint_font_set_from_string()
|   4-Jul-96         pch         In _uint_font_set_from_string() initialise
|                                return font to NULL
|   5-Jul-96         pch         Flesh out the font creation from strings
|  16-Jul-96 H-01-02 UK    $$2   Automatic Submission
|  01-Apr-97         jas         Added new font functions
|  09-Apr-97 H-03-05 UK    $$3   Automatic Submission
|  09-Apr-97         jas         Added small font for new shell
|  16-Apr-97 H-03-06 UK    $$4   Automatic Submission
|  16-Apr-97         jas         Use DEFAULT_GUI_FONT for new shell
|  22-Apr-97 H-03-07 UK    $$5   Automatic Submission
|  22-Apr-97         jas         Changed default font
|  23-Apr-97         jas         Added sysdep_font_get_system_fonts
|  23-Apr-97         jas         Added proe font functions
|  25-Apr-97         jas         Added Win95 font
|  29-Apr-97 H-03-08 UK    $$6   Automatic Submission
|  13-Jun-97         jas         Added sysdep_font_create and sysdep_font_free
|  17-Jun-97 H-03-14 UK    $$7   Automatic Submission
|  31-Jul-97         jas         Use DEFAULT_GUI_FONT for better compatibility
|  31-Jul-97         jas         Fixed TrueType (TM) font size calculation
|  01-Aug-97         jas         Added _ui_font_get_overhang
|  08-Aug-97         jas         Use MulDiv for size calculations
|  12-Aug-97 H-03-18 UK    $$8   Automatic Submission
|  23-Sep-97         jas         Use DEFAULT_GUI_FONT in Japanese and Russian
|  30-Sep-97 H-03-24 UK    $$9   Automatic Submission
|  15-Oct-97         jas         Use fonts from the registry
|  16-Oct-97         jas         Removed obsolete functions
|  21-Oct-97 H-03-27 UK    $$10  Automatic Submission
|  21-Oct-97         jas         Added new font code
|  23-Oct-97         jas         Added character specific overhangs
|  29-Oct-97 H-03-28 UK    $$11  Automatic Submission
|   4-Nov-97         pch         Dont check out the registry for fonts for
|                                the time being - It gets the wrong font
|  05-Nov-97 H-03-29 UK    $$12  Automatic Submission
|  10-Nov-97         jas         Fixed _uint_font_get_reg_font for Windows 95
|  18-Nov-97 H-03-30 UK    $$13  Automatic Submission
|  12-Dec-97         jas         Use MS Sans Serif 8.0 on Windows NT 3.51
|                                using Windows 95 graphics
|  16-Dec-97 H-03-33 UK    $$14  Automatic Submission
|  16-Dec-97         jas         Obsoleted old graphics mode
|  23-Dec-97 H-03-34 UK    $$15  Automatic Submission
|  03-Apr-98         jas         Added sysdep_font_get_info
|  06-Apr-98 I-01-02 UK    $$16  Automatic Submission
|  07-Apr-98         jas         Changed ui_font_t to store font handles
|  07-Apr-98         jas         Changed _uiFontDefs to ui_font_class_table_t
|  08-Apr-98         jas         Changed sysdep_font_get_info
|  15-Apr-98 I-01-03 UK    $$17  Automatic Submission
|  03-Sep-98         jas         Added message and titlebar fonts
|  08-Sep-98 I-01-18 UK    $$18  Automatic Submission
|  17-Sep-98         jas         Fixed GDI resource leaks
|  23-Sep-98 I-01-20 UK    $$19  Automatic Submission
|  24-Sep-98         jas         Added support for Korean
|  30-Sep-98 I-01-21 UK    $$20  Automatic Submission
|  19-May-99         jas         Fixed compilation warnings
|  01-Jun-99 I-03-10 UK    $$21  Automatic Submission
|  17-Jun-99         jas         Added support for all multi-byte languages
|  28-Jul-99 I-03-11 UK    $$22  Automatic Submission
|  10-Jan-00         jas         Changed font width calculation
|  13-Jan-00 I-03-26+UK    $$23  Automatic Submission
|  26-Apr-00         jas         Fixed registry font problems
|  04-May-00 J-01-07 UK    $$24  Automatic Submission
|  04-May-00         jas         Added further support for Windows fonts
|  18-May-00 J-01-08 UK    $$25  Automatic Submission
|  20-Sep-00         jas         Fixed problems with DEFAULT_GUI_FONT
|  03-Oct-00 J-01-19 UK    $$26  Automatic Submission
|  06-Nov-00         jas         Fixed style problems with TrueType fonts
|  16-Nov-00 J-01-21 UK    $$27  Automatic Submission
|  22-Nov-00         jas         Added font baseline support
|  22-Nov-00         jas         Added font character set encoding support
|  12-Jun-01 J-03-01 UK    $$28  Automatic Submission
|  09-Oct-01         jas         Use pro_is_win95_running
|  18-Oct-01 J-03-10 UK    $$29  Automatic Submission
|  29-Oct-01         jas         Added UI_FIXED_FONT
|  30-Oct-01 J-03-11 UK    $$30  Automatic Submission
|  10-Jan-02         jas         Added PTC graphics mode
|  17-Jan-02 J-03-17 UK    $$31  Automatic Submission
|  12-Feb-02         jas         Added further support for PTC graphics mode
|  20-Feb-02 J-03-19 UK    $$32  Automatic Submission
|  28-Aug-02         jas         Ignore far-eastern rotated fonts
|  10-Sep-02 J-03-33 UK    $$33  Automatic Submission
|  26-Jun-03         jas         Obsoleted ui_memory.h
|  10-Jul-03 K-01-10 UK    $$34  Automatic Submission
|  22-Sep-03         jas         Fixed _uint_font_init
|  23-Sep-03 K-01-15 UK    $$35  Automatic Submission
|  11-Nov-03         jas         Added UI_STATIC
|  18-Nov-03 K-01-18 UK    $$36  Automatic Submission
|  12-Feb-04         jas         Fixed resource leaks
|  02-Mar-04 K-01-24 UK    $$37  Automatic Submission
|  22-Dec-04         jas         Include const.h and ctwcfun.h
|  11-Jan-05 K-03-17 UK    $$38  Automatic Submission
|  25-Jan-05         jas         Include ctwcfun_proto.h instead of ctwcfun.h
|  25-Jan-05 K-03-18 UK    $$39  Automatic Submission
|  31-Jan-05         jas         Added support for scalable fonts
|  15-Feb-05 K-03-19 UK    $$40  Automatic Submission
|  11-Oct-05         jas         Removed pro_is_win95_running
|  31-Jan-06 L-01-01 UK    $$41  Automatic Submission
|  16-Feb-06         jas         Fixed use of NULL_CHAR
|  28-Feb-06 L-01-03 UK    $$42  Automatic Submission
|  03-Apr-06         jas         Added sysdep_font_get_locale_font
|  11-Apr-06 L-01-06 UK    $$43  Automatic Submission
|  14-Jun-06         jas         Added more font encodings
|  27-Jun-06 L-01-11 UK    $$44  Automatic Submission
|  10-Jan-07         jas         Added flags to _ui_wtounicode
|  16-Jan-07 L-01-24 UK    $$45  Automatic Submission
|  06-Feb-07         jas         Added text length to _ui_wtounicode
|  13-Feb-07 L-01-26 UK    $$46  Automatic Submission
|  11-Sep-07         jas         Added UI_DESKTOP_FONT
|  25-Sep-07 L-01-38 UK    $$47  Automatic Submission
|  23-Apr-08         jas         Modified sysdep_font_get_info
|  06-May-08 L-03-08 UK    $$48  Automatic Submission
|  29-Oct-10         jas         Added sysdep_font_refresh
|  09-Nov-10 L-05-35 UK    $$49  Automatic Submission
|  25-Jan-11         jas         Added _ui_get_language
|  01-Feb-11 L-05-41 UK    $$50  Automatic Submission
|  29-Mar-11         jas         Fixed _uint_font_get_size
|  12-Apr-11 L-05-45 UK    $$51  Automatic Submission
|  14-May-12         jas         Do not depend on DEFAULT_GUI_FONT
|  16-May-12 P-20-05 UK    $$52  Automatic Submission
|  07-May-14         jas         Added UI_FONT_ANTI_ALIASED
|  13-May-14 P-20-54 UK    $$53  Automatic Submission
|  23-Feb-15         jas         Added UI_SEMIBOLD
|  26-Feb-15         jas         Added UINT_VERSION_WINDOWS_... macros
|  03-Mar-15 P-30-03 UK    $$54  Automatic Submission
|  22-Sep-15         jas         Added IsWindowsXXXOrGreater
|  30-Sep-15 P-30-17 UK    $$55  Automatic Submission
|  23-Nov-15         jas         Added _ui_font_get_private_fonts
|  25-Nov-15 P-30-21 UK    $$56  Automatic Submission
|  22-Feb-16         jas         Fixed problems with the fixed-width font
|  02-Mar-16 P-30-27 UK    $$57  Automatic Submission
|  13-Jun-16         jas         Added convenience macros
|  21-Jun-16 P-30-34 UK    $$58  Automatic Submission
|  26-Aug-16         jas         Added UI_use_legacy_system_font_Attr
|  30-Aug-16 P-30-39 UK    $$59  Automatic Submission
|  26-Jan-17         jas         Added sysdep_font_get_sysdep_font
|  14-Mar-17 P-50-01 UK    $$60  Automatic Submission
|  12-Oct-17         jas         Added sysdep_font_get_path
|  24-Oct-17 P-50-32 UK    $$61  Automatic Submission
|  15-Dec-17         jas         Use the EM square width for TrueType fonts
|  19-Dec-17 P-50-41 UK    $$62  Automatic Submission
|  16-Feb-18         jas         Extended UI_FONT_ANTI_ALIASED
|  11-Apr-18 P-50-49 UK    $$63  Automatic Submission
|  03-May-18         jas         Added _uint_font_get_alt_names
|  15-May-18 P-60-03 UK    $$64  Automatic Submission
|  09-Jan-19         jas         Added sysdep_font_get_character_set
|  11-Jan-19         jas         Added _ui_font_find
|  11-Jan-19         jas         Added sysdep_font_load
|  26-Feb-19         jas         Added _ui_font_set_quality
|  19-Mar-19 P-70-01 UK    $$65  Automatic Submission
|  14-May-19         jas         Added sysdep_font_check_kerning_pair
|  28-May-19 P-70-11 UK    $$66  Automatic Submission
|  19-Jun-19         jas         Ignore non-Unicode alternative names
|  19-Jun-19         jas         Fixed _uint_font_check_kerning_pair
|  21-Jun-19         jas         Added _ui_font_size_scale
|  25-Jun-19 P-70-15 UK    $$67  Automatic Submission
|  09-Apr-20         jas         Fixed font kerning pair check
|  21-Apr-20 P-80-01 UK    $$68  Automatic Submission
|  28-May-20         jas         Extended _ui_font_quality
|  02-Jun-20 P-80-06 UK    $$69  Automatic Submission
|  11-Jun-20         jas         Fixed _uint_font_get_system_fonts
|  16-Jun-20 P-80-08 UK    $$70  Automatic Submission
|  24-Jun-20         jas         Fixed _uint_font_quality
|  30-Jun-20 P-80-10 UK    $$71  Automatic Submission
|  15-Jul-20         jas         Fixed _uint_font_create for small sizes
|  22-Jul-20 P-80-13 UK    $$72  Automatic Submission
|  21-Sep-20         jas         Added _uint_font_get_font_from_HFONT
|  30-Sep-20 P-80-22 UK    $$73  Automatic Submission
|  04-Mar-21         jas         Added internal leading support
|  21-Apr-21 P-90-05 UK    $$74  Automatic Submission
|  01-Oct-21         jas         Fixed default font quality
|  05-Oct-21 P-90-28 UK    $$75  Automatic Submission
|  18-Nov-21         jas         Added sysdep_font_get_metrics
|  23-Nov-21 P-90-35 UK    $$76  Automatic Submission
|  27-Sep-22         jas         Added UI_FONT_QUALITY_ALIASED
|  28-Sep-22 Q-10-29 UK    $$77  Automatic Submission
|  30-Sep-22         jas         Do not use kerning with DirectWrite
|  06-Oct-22 Q-10-30 UK    $$78  Automatic Submission
|  24-Nov-22         jas         Added private flag to _ui_krn_font_add_name
|  29-Nov-22         jas         Added _ui_krn_font_name_set_private_data
|  30-Nov-22 Q-10-37 UK    $$79  Automatic Submission
|  01-Dec-22         jas         Load private fonts separately from OS fonts
|  02-Dec-22         jas         Do not use wide strings for font filenames
|  05-Dec-22         jas         Added private data to sysdep_font_create
|  05-Dec-22         jas         Added d2d_font_t
|  05-Dec-22         jas         Removed data and size from sysdep_font_load
|  07-Dec-22 Q-10-38 UK    $$80  Automatic Submission
|  07-Feb-23         jas         Added _uint_font_path_wstrcmp
|  08-Feb-23 Q-10-47 UK    $$81  Automatic Submission
|  02-Mar-23         jas         Use floating point font kerning
|  08-Mar-23 Q-11-02 UK    $$82  Automatic Submission
|  09-Mar-23         jas         Fixed sysdep_font_create
|  15-Mar-23 Q-11-03 UK    $$83  Automatic Submission
|  28-Mar-23         jas         Removed sysdep_font_check_kerning_pair
|  12-Apr-23 Q-11-07 UK    $$84  Automatic Submission
|  30-Apr-25         jas         Added _ui_string_qcr_enabled
|  07-May-25 Q-13-07 UK    $$85  Automatic Submission
|  10-Jul-25         jas         Fixed compilation warnings
|  15-Jul-25 Q-13-17 UK    $$86  Automatic Submission
|  11-Sep-25         jas         Fixed _uint_font_d2d_free
|  11-Sep-25 Q-13-26 UK    $$87  Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#if !defined (lint) && defined (SHOW_SCCS_ID)
static char uint_fonts_c_id [] = "@(#) uint_fonts.c 4126.1@(#)";
#endif

#include <sysstdio.h>
#include <sysmath.h>
#include <const.h>
#include <mkscpy.h>
#include <ctwcfun_proto.h>
#include <pro_memory.h>
#include <languages.h>
#include <ptc_xarray.h>
#include <ctmemmgr_proto.h>
#include <bindcall.h>
#include <mathcons.h>

#include <ui.h>
#include <uip.h>
#include <ui_fonts.h>
#include <ui_fontsp.h>
#include <ui_string.h>
#include <ui_utils.h>

#ifdef UI_SYSTEM_NT

#include <uint.h>
#include <uint_d2d1.h>
#include <uint_dwrite.h>


/*
** IDWriteFontCollectionLoader
*/

typedef struct
{
    IDWriteFontCollectionLoaderVtbl    *lpVtbl;
    ULONG                               refCount;

} d2d_font_collection_loader_t;


/*
** IDWriteFontFileEnumerator
*/

typedef struct
{
    IDWriteFontFileEnumeratorVtbl  *lpVtbl;
    ULONG                           refCount;

    IDWriteFactory                 *factory;
    WCHAR                          *filename;
    IDWriteFontFile                *current;
    WCHAR                          *next;

} d2d_font_file_enumerator_t;


/*
** System dependent font
*/

typedef struct
{
    HFONT               font;
    IDWriteTextFormat  *format;

} d2d_font_t;


/*
** Font enumeration data
*/

typedef struct
{
    HDC             device_context;
    char          **alt_names;
    const char     *filename;
    DBHandle       *handles;

} nt_font_enum_data_t;


/*
** Function prototypes
*/

static void _uint_font_load_fonts (
    const char *filename
);

static int CALLBACK _uint_font_enum_type_CB (
    CONST LOGFONT      *font_data_ex,
    CONST TEXTMETRIC   *text_metric,
    DWORD               font_type,
    LPARAM              lParam
);

static int CALLBACK _uint_font_enum_name_CB (
    CONST LOGFONT      *font_data_ex,
    CONST TEXTMETRIC   *text_metric,
    DWORD               font_type,
    LPARAM              lParam
);

static HFONT _uint_font_get_reg_font (
    char   *name
);

static DBHandle _uint_font_get_font_from_HFONT (
    HFONT   font
);

static DBHandle _uint_font_from_HFONT (
    HFONT       font,
    DBHandle    fallback
);

static int _uint_font_get_font_info (
    CONST LOGFONT      *font_data,
    CONST TEXTMETRIC   *text_metric,
    char              **name,
    int                *style,
    double             *size
);

static int _uint_font_add_font (
    LOGFONT    *font_data,
    DBHandle    name,
    int         style,
    double      size,
    int         is_raster
);

static int _uint_font_quality (
    void
);

static IDWriteFontCollection *_uint_font_d2d_collection (
    const char *filename
);

static int _uint_font_d2d_CreateFontFromLOGFONT (
    IDWriteFactory         *factory,
    HDC                     device_context,
    LOGFONTW               *logfont,
    IDWriteFontCollection  *collection,
    IDWriteFont           **font
);

static int _uint_font_d2d_GetFontName (
    IDWriteFont    *font,
    WCHAR          *name,
    int             length
);

static int _uint_font_d2d_CreateTextFormat (
    IDWriteFactory         *factory,
    HDC                     device_context,
    HFONT                   font_handle,
    IDWriteFontCollection  *collection,
    IDWriteTextFormat     **format
);

static IDWriteTextFormat *_uint_font_d2d_create (
    HFONT                   font,
    IDWriteFontCollection  *collection
);

static void _uint_font_d2d_free (
    IDWriteTextFormat  *format
);

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_system_fonts
| Purpose:  Store all of the fonts on the system
| Input:
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_get_system_fonts (void)
{
#ifdef UI_SYSTEM_NT

    BOOL                smoothing = FALSE;
    UINT                smoothing_type = 0;
    ui_font_quality_t   quality;

    switch (_uint_font_quality ())
    {
        case NONANTIALIASED_QUALITY:
        {
            quality = UI_FONT_QUALITY_ALIASED;
            break;
        }

        case ANTIALIASED_QUALITY:
        {
            quality = UI_FONT_QUALITY_ANTIALIASED;
            break;
        }

        case CLEARTYPE_QUALITY:
        {
            quality = UI_FONT_QUALITY_CLEARTYPE;
            break;
        }

        case CLEARTYPE_NATURAL_QUALITY:
        {
            quality = UI_FONT_QUALITY_GREYSCALE;
            break;
        }

        default:
        {
            if (!(SystemParametersInfo (
                      SPI_GETFONTSMOOTHING, 0, &smoothing, 0)) ||
                !smoothing)
            {
                quality = UI_FONT_QUALITY_ALIASED;
            }
            else if (!(SystemParametersInfo (
                           SPI_GETFONTSMOOTHINGTYPE, 0, &smoothing_type, 0)) ||
                     smoothing_type != FE_FONTSMOOTHINGCLEARTYPE)
            {
                quality = UI_FONT_QUALITY_ANTIALIASED;
            }
            else
            {
                quality = UI_FONT_QUALITY_GREYSCALE;
            }
        }
    }

    _ui_font_set_quality (quality);

    (void) _uint_d2d_enabled ();

    _uint_font_load_fonts ((char *) NULL);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_load
| Purpose:  Load a font
| Input:    filename    - the name of the font
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_load (const char *filename)
{
#ifdef UI_SYSTEM_NT

    wchar_t    *name;
    int         status = UI_ERROR;

    if (filename != (const char *) NULL &&
        (name = make_strtows (filename)) != (wchar_t *) NULL)
    {
        if (AddFontResourceExW (name, FR_PRIVATE, NULL) > 0)
        {
            _uint_font_load_fonts (filename);

            status = UI_SUCCESS;
        }

        relmem (&name);
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_init
| Purpose:  Initialize the fonts for Windows NT
| Input:    class_table     - font class table
| Output:   class_table     - font class table
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_init (ui_font_class_table_t *class_table)
{
#ifdef UI_SYSTEM_NT

    HFONT               font;
    HFONT               menu_font;
    HFONT               icon_font;
    HFONT               status_font;
    HFONT               message_font;
    HFONT               caption_font;
    DBHandle            font_handle;
    DBHandle            menu_font_handle;
    DBHandle            icon_font_handle;
    DBHandle            status_font_handle;
    DBHandle            message_font_handle;
    DBHandle            caption_font_handle;
    DBHandle            fixed_font_handle = DB_HANDLE_ERROR;
    HKEY                key;
    DWORD               type;
    DWORD               fixed_size;
    WCHAR               value [256];
    DWORD               length;
    char               *name;
    int                 style;
    double              size;
    NONCLIENTMETRICS    ncm;
    LOGFONT             data;
    int                 i;
    int                 legacy = FALSE;

    /*
    ** Set the default font
    */

    switch (_ui_get_graphics_mode ())
    {
        case UI_GRAPHICS_MODE_MOTIF:
        case UI_GRAPHICS_MODE_WIN31:
        {
            if (_ui_using_input_method () ||
                _ui_get_language () == RUSSIAN)
            {
                name = (char *) NULL;

                font_handle =
                    _uint_font_get_font_from_HFONT (
                        GetStockFont (SYSTEM_FONT));

                menu_font_handle = font_handle;
                icon_font_handle = font_handle;
                status_font_handle = font_handle;
                message_font_handle = font_handle;
                caption_font_handle = font_handle;

                fixed_font_handle =
                    _uint_font_get_font_from_HFONT (
                        GetStockFont (SYSTEM_FIXED_FONT));
            }
            else
            {
                font_handle = _ui_font_find ("system", UI_BOLD, 10.0);

                menu_font_handle = font_handle;
                icon_font_handle = font_handle;

                status_font_handle =
                    _ui_font_find ("sans serif", UI_REGULAR, 8.0);

                message_font_handle = font_handle;
                caption_font_handle = font_handle;

                fixed_font_handle =
                    _ui_font_find ("fixedsys", UI_REGULAR, 9.0);
            }

            break;
        }

        default:
        {
            /*
            ** Use the DEFAULT_GUI_FONT but extract the information from it first
            ** so that we can use the real font from the database
            */

            font = GetStockFont (DEFAULT_GUI_FONT);

            ncm.cbSize = sizeof (NONCLIENTMETRICS);

            if (SystemParametersInfo (
                    SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0))
            {
                menu_font = CreateFontIndirect (&(ncm.lfMenuFont));
                status_font = CreateFontIndirect (&(ncm.lfStatusFont));
                message_font = CreateFontIndirect (&(ncm.lfMessageFont));
                caption_font = CreateFontIndirect (&(ncm.lfCaptionFont));
            }
            else
            {
                menu_font = _uint_font_get_reg_font ("MenuFont");
                status_font = _uint_font_get_reg_font ("StatusFont");
                message_font = _uint_font_get_reg_font ("MessageFont");
                caption_font = _uint_font_get_reg_font ("CaptionFont");
            }

            if (SystemParametersInfo (
                    SPI_GETICONTITLELOGFONT, sizeof (LOGFONT), &data, 0))
            {
                icon_font = CreateFontIndirect (&data);
            }
            else
            {
                icon_font = _uint_font_get_reg_font ("IconFont");
            }

            /*
            ** As per MS: do not use DEFAULT_GUI_FONT on Windows Vista and
            ** later.  Instead use the user-defined message font as the
            ** base GUI font, which allows user customization quickly and
            ** easily via Control Panel.
            **
            ** jas - 14-May-12
            */

            if (message_font != (HFONT) NULL &&
                IsWindowsVistaOrGreater () &&
                (_ui_krn_global_inquire (
                     UI_use_legacy_system_font_Attr, &legacy, NULL)
                     != UI_SUCCESS ||
                 !legacy))
            {
                font = message_font;
            }

            name = (char *) NULL;

            _uint_font_get_info (font, &name, &style, &size);

            size = _ui_font_size_scale (size);

            font_handle = _ui_font_find (name, style, size);

            fixed_size = (DWORD) ((size + 1.0) * 10.0);

            menu_font_handle =
                _uint_font_from_HFONT (menu_font, font_handle);

            icon_font_handle =
                _uint_font_from_HFONT (icon_font, font_handle);

            status_font_handle =
                _uint_font_from_HFONT (status_font, font_handle);

            message_font_handle =
                _uint_font_from_HFONT (message_font, font_handle);

            caption_font_handle =
                _uint_font_from_HFONT (caption_font, font_handle);

#if 0

            /*
            ** Override the default OS-defined fixed-width font with the font used
            ** by Windows Notepad
            */

            if (RegOpenKeyExW (
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Microsoft\\Notepad\\DefaultFonts", (DWORD) 0,
                    KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
            {
                if ((length = (DWORD) sizeof (value)) > 0 &&
                    RegQueryValueExW (
                        key, L"lfFaceName", (DWORD *) NULL, &type,
                        (BYTE *) value, &length) == ERROR_SUCCESS &&
                    type == REG_SZ &&
                    (name = _ui_wstrtos_copy_buf ((wchar_t *) value))
                        != (char *) NULL &&
                    (length = (DWORD) sizeof (fixed_size)) > 0 &&
                    RegQueryValueExW (
                        key, L"iPointSize", (DWORD *) NULL, &type,
                        (BYTE *) &fixed_size, &length) == ERROR_SUCCESS &&
                    type == REG_DWORD)
                {
                    fixed_font_handle =
                        _ui_font_find (
                            name,
                            UI_REGULAR,
                            _ui_font_size_scale ((double) fixed_size / 10.0));
                }

                RegCloseKey (key);
            }

#else

            /*
            ** Override the default OS-defined fixed-width font with the font used
            ** by the Windows Console
            */

            if (RegOpenKeyExW (
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Console\\TrueTypeFont",
                    (DWORD) 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
            {
                if ((length = (DWORD) sizeof (value)) > 0 &&
                    RegQueryValueExW (
                        key, L"0", (DWORD *) NULL, &type,
                        (BYTE *) value, &length) == ERROR_SUCCESS &&
                    type == REG_SZ &&
                    (name = _ui_wstrtos_copy_buf ((wchar_t *) value))
                        != (char *) NULL)
                {
                    fixed_font_handle =
                        _ui_font_find (
                            name,
                            UI_REGULAR,
                            ((double) fixed_size / 10.0));
                }

                RegCloseKey (key);
            }

#endif

            if (fixed_font_handle == DB_HANDLE_ERROR)
            {
                fixed_font_handle =
                    _uint_font_get_font_from_HFONT (
                        GetStockFont (SYSTEM_FIXED_FONT));
            }
        }
    }

    /*
    ** Set for all the fonts
    */

    for (i = 0; class_table [i].font_class != UI_NOT_A_FONT; i++)
    {
        switch (class_table [i].font_class)
        {
            case UI_MENU_FONT:
            {
                class_table [i].font = menu_font_handle;
                break;
            }

            case UI_DESKTOP_FONT:
            {
                class_table [i].font = icon_font_handle;
                break;
            }

            case UI_POPUPHELP_FONT:
            {
                class_table [i].font = status_font_handle;
                break;
            }

            case UI_MESSAGE_FONT:
            {
                class_table [i].font = message_font_handle;
                break;
            }

            case UI_TITLEBAR_FONT:
            {
                class_table [i].font = caption_font_handle;
                break;
            }

            case UI_FIXED_FONT:
            {
                class_table [i].font = fixed_font_handle;
                break;
            }

            default:
            {
                class_table [i].font = font_handle;
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_refresh
| Purpose:  Refresh the fonts for Windows NT
| Input:    class_table     - font class table
| Output:   class_table     - font class table
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_refresh (ui_font_class_table_t *class_table)
{
#ifdef UI_SYSTEM_NT

    return (_uint_font_init (class_table));

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_alt_names
| Purpose:  Find the alternate names of a font
| Input:    device_context  - the device context to use
|           data            - the font data
| Output:   alt_names       - xarray of alternate font names
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_font_get_alt_names (HDC device_context, const LOGFONT *data,
                                      char ***alt_names)
{

#define TTF_USHORT(w) \
(USHORT) (((w)[0] << 8) | (w)[1])

    HFONT           font = CreateFontIndirect (data);
    HFONT           sys_font = SelectFont (device_context, font);
    DWORD           size;
    unsigned char  *font_data, *tt_data, *name_data;
    USHORT          offset, count, platform, length, locale, i, j;
    WCHAR          *name;
    char           *alt_name;

    /*
    ** Read the face names from the OpenType font specification Naming Table
    **
    ** https://docs.microsoft.com/en-us/typography/opentype/spec/name
    */

    if ((size =
         GetFontData (device_context, 'eman', 0, NULL, 0))
            != GDI_ERROR)
    {
        font_data = tt_data = GET_ARRAY (unsigned char, size);

        if ((size =
             GetFontData (device_context, 'eman', 0, tt_data, size))
                != GDI_ERROR)
        {
            offset = TTF_USHORT (tt_data + 4);
            name_data = (tt_data + offset);

            count = TTF_USHORT (tt_data + 2);

            for (tt_data += 6; count > 0u; count--, tt_data += 12)
            {
                /*
                ** Look for the Font Family name (nameID == 1)
                */

                if (TTF_USHORT (tt_data + 6) == 1 &&
                    ((platform = TTF_USHORT (tt_data)) == 0 ||
                     platform == 3) &&
                    (length = TTF_USHORT (tt_data + 8)) > 0 &&
                    (locale = TTF_USHORT (tt_data + 4)) != 0)
                {
                    /*
                    ** Convert the big-endian data to WCHAR
                    **
                    ** N.B. This code assumes the name is Unicode.
                    */

                    offset = TTF_USHORT (tt_data + 10);

                    name = GET_ARRAY (WCHAR, ((length / 2) + 1));

                    for (i = j = 0u; j < length; i++, j += 2)
                    {
                        name [i] = (WCHAR) TTF_USHORT (name_data + offset + j);
                    }

                    /*
                    ** Ignore the face name used by Windows (which will vary
                    ** according to the system locale of the OS)
                    */

                    if (wstrcmp (name, data->lfFaceName))
                    {
                        /*
                        ** Build a list of alternate face names
                        */

#if 0
                        language =
                            _uint_get_language_from_locale ((HKL) locale);
#endif

                        alt_name = make_wstrtos (name);

                        length = (USHORT) XAR_COUNT (alt_names);

                        for (i = 0;
                             i < length && strcmp ((*alt_names) [i], alt_name);
                             i++);

                        if (i < length)
                        {
                            relmem (&alt_name);
                        }
                        else
                        {
                            XAR_APPEND (alt_names, 1, &alt_name);
                        }
                    }

                    relmem (&name);
                }
            }
        }

        relmem (&font_data);
    }

    SelectFont (device_context, sys_font);
    DeleteFont (font);

#undef TTF_USHORT

}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_load_fonts
| Purpose:  Store all of the fonts on the system
| Input:    filename    - the filename of the fonts to be loaded
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_font_load_fonts (const char *filename)
{
    nt_font_enum_data_t data;
    HFONT               default_font = GetStockFont (DEFAULT_GUI_FONT);
    LOGFONT             font;

    data.device_context = GetDC ((HWND) NULL);
    data.alt_names = (char **) NULL;
    data.filename = filename;
    data.handles = (DBHandle *) NULL;

    /*
    ** Enumerate the DEFAULT_GUI_FONT first
    */

    GetObject (default_font, sizeof (LOGFONT), &font);
    font.lfCharSet = DEFAULT_CHARSET;

    EnumFontFamiliesEx (
        data.device_context,
        &font,
        (FONTENUMPROC) _uint_font_enum_type_CB,
        (LPARAM) &data,
        (DWORD) 0);

    /*
    ** Now enumerate all other available fonts
    */

    font.lfFaceName [0] = (TCHAR) 0;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfPitchAndFamily = 0;

    EnumFontFamiliesEx (
        data.device_context,
        &font,
        (FONTENUMPROC) _uint_font_enum_type_CB,
        (LPARAM) &data,
        (DWORD) 0);

    XAR_FREE (&(data.handles));

    ReleaseDC ((HWND) NULL, data.device_context);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_enum_type_CB
| Purpose:  Enumerate all of the name of the fonts for a font type
| Input:    font_data_ex    - pointer to ENUMLOGFONTEX structure of font data
|           text_metric     - font character metrics
|           font_type       - the type of the font
|           lParam          - enumeration data
| Output:
| Return:   TRUE (ignored)
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static int CALLBACK _uint_font_enum_type_CB (CONST LOGFONT *font_data_ex,
                                             CONST TEXTMETRIC *text_metric,
                                             DWORD font_type, LPARAM lParam)
{
    nt_font_enum_data_t    *data = (nt_font_enum_data_t *) lParam;
    LOGFONT                 font_data =
        ((ENUMLOGFONTEX *) font_data_ex)->elfLogFont;

    /*
    ** Ignore vertical fonts
    */

    if (font_data.lfFaceName [0] != L'@')
    {
        font_data.lfPitchAndFamily =
            ((font_data.lfCharSet == HEBREW_CHARSET ||
              font_data.lfCharSet == ARABIC_CHARSET) ?
             MONO_FONT :
             0);

        /*
        ** Check for any alternate face names
        */

        data->alt_names = XAR_BEGIN (char *, 4);

        if (font_type & TRUETYPE_FONTTYPE)
        {
            _uint_font_get_alt_names (
                data->device_context,
                &font_data,
                &(data->alt_names));
        }

        EnumFontFamiliesEx (
            data->device_context,
            &font_data,
            (FONTENUMPROC) _uint_font_enum_name_CB,
            lParam,
            (DWORD) 0);

        rls_string_xarray (&(data->alt_names));
    }

    return (TRUE);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_enum_name_CB
| Purpose:  Store a font in the database
| Input:    font_data_ex    - pointer to ENUMLOGFONTEX structure of font data
|           text_metric     - font character metrics
|           font_type       - the type of the font
|           lParam          - enumeration data
| Output:
| Return:   TRUE (ignored)
\*--------------------------------------------------------------------------*/
static int CALLBACK _uint_font_enum_name_CB (CONST LOGFONT *font_data_ex,
                                             CONST TEXTMETRIC *text_metric,
                                             DWORD font_type, LPARAM lParam)
{
    nt_font_enum_data_t    *data = (nt_font_enum_data_t *) lParam;
    LOGFONT                *font_data =
        &(((ENUMLOGFONTEX *) font_data_ex)->elfLogFont);
    int                     count, i;
    char                   *font_name, *name;
    int                     style;
    double                  size;
    DBHandle                handle;
    LOGFONT                 font_data_copy;

    _uint_font_get_font_info (
        font_data,
        text_metric,
        &font_name,
        &style,
        &size);

    for (count = XAR_COUNT (&(data->alt_names)), i = -1; i < count; i++)
    {
        name =
            ((i < 0) ?
             font_name :
             data->alt_names [i]);

        /*
        ** Determine whether to add the font
        */

        if (data->filename == (const char *) NULL)
        {
            handle = _ui_krn_font_add_name (name, (const char *) NULL);
        }
        else if ((handle =
                  _ui_krn_font_handle_from_name (name))
                     == DB_HANDLE_ERROR)
        {
            handle = _ui_krn_font_add_name (name, data->filename);

            if (_uint_d2d_enabled ())
            {
                _ui_krn_font_name_set_private_data (
                    handle,
                    _uint_font_d2d_collection (data->filename));
            }

            if (data->handles == (DBHandle *) NULL)
            {
                data->handles = XAR_BEGIN (DBHandle, 16);
            }

            id_insert ((int **) &(data->handles), (int) handle);
        }
        else if (binary_search_int (
                     XAR_COUNT (&(data->handles)), (int *) data->handles,
                     (int) handle) < 0)
        {
            handle = DB_HANDLE_ERROR;
        }

        if (handle != DB_HANDLE_ERROR)
        {
            if (font_type & TRUETYPE_FONTTYPE)
            {
                font_data_copy = *font_data;
                font_data_copy.lfWidth = font_data_copy.lfHeight = 0L;

                _uint_font_add_font (
                    &font_data_copy,
                    handle,
                    style,
                    0.0,
                    TRUE);
            }
            else
            {
                _uint_font_add_font (
                    font_data,
                    handle,
                    style,
                    size,
                    ((font_type & RASTER_FONTTYPE) &&
                     !(font_type & DEVICE_FONTTYPE)));
            }
        }
    }

    return (TRUE);
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_metrics
| Purpose:  Get the metrics in pixels of characters of a font
| Input:    font        - the font to find the size of
| Output:   metrics     - the metrics of the font, in pixels
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_get_metrics (void *font, ui_font_metric_t *metrics)
{
#ifdef UI_SYSTEM_NT

    HWND        window = GetDesktopWindow ();
    HDC         device_context = GetDC (window);
    HFONT       sys_font =
        SelectFont (
            device_context,
            (_uint_d2d_enabled () ?
             ((d2d_font_t *) font)->font :
             (HFONT) font));
    TEXTMETRIC  text_metrics;
    int         status = UI_SUCCESS;

    GetTextMetrics (device_context, &text_metrics);

    SelectFont (device_context, sys_font);
    ReleaseDC (window, device_context);

    /*
    ** If the font is NOT a fixed width font (note the opposite
    ** meaning of the TMPF_FIXED_PITCH flag above, i.e. it is a fixed
    ** width font if the bit is NOT set) then use the twice the
    ** average character width plus one as the font width.  This
    ** corrects well for those fonts whose maximum character width
    ** is extremely large compared to the average character width
    ** because the font includes some very wide characters.  This
    ** easily happens if the user has installed Office 2000 or indeed
    ** is running under Windows 2000.
    **
    ** jas - 10-Jan-00
    **
    ** Use the minimum of the two possible values, to prevent cases
    ** where the font *is* marked as a fixed width, yet has a maximum
    ** character width that far exceeds the average character width,
    ** e.g. running on Windows 7 in Chinese.
    **
    ** jas - 29-Mar-11
    */

    if (text_metrics.tmPitchAndFamily & TMPF_TRUETYPE)
    {
        /*
        ** Use the EM square width for TrueType fonts.
        **
        ** By definition the EM square is a square, so the width is equal
        ** to the ascent of the font.
        **
        ** jas - 15-Dec-17
        */

        metrics->fixed_width =
            (unsigned short)
                MIN (text_metrics.tmAscent, text_metrics.tmMaxCharWidth);
    }
    else
    {
        metrics->fixed_width =
            (unsigned short)
                MIN (
                    ((text_metrics.tmAveCharWidth * 2) + 1),
                    text_metrics.tmMaxCharWidth);
    }

    metrics->fixed_height = (unsigned short) text_metrics.tmHeight;
    metrics->baseline = (unsigned short) text_metrics.tmAscent;
    metrics->leading = (unsigned short) text_metrics.tmInternalLeading;

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_character_set
| Purpose:  Get the character set of a font
| Input:    font            - the font
| Output:   character_set   - xarray list of characters defined by the font
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_get_character_set (void *font,
                                            ui_font_cs_t **character_set)
{
#ifdef UI_SYSTEM_NT

    HWND            window = GetDesktopWindow ();
    HDC             device_context = GetDC (window);
    HFONT           sys_font =
        SelectFont (
            device_context,
            (_uint_d2d_enabled () ?
             ((d2d_font_t *) font)->font :
             (HFONT) font));
    DWORD           count, i;
    int             start, end, j;
    GLYPHSET       *glyph_set;
    ui_font_cs_t    range;
    int             status = UI_ERROR;

    /*
    ** Get the table of unicode character ranges from the font
    */

    if ((count = GetFontUnicodeRanges (device_context, (GLYPHSET *) NULL)) > 0)
    {
        glyph_set = (GLYPHSET *) getmem (count);
        glyph_set->cbThis = count;

        GetFontUnicodeRanges (device_context, glyph_set);

        /*
        ** Construct our own ordered table of character ranges
        */

        count = glyph_set->cRanges;

        *character_set = XAR_BEGIN (ui_font_cs_t, count);

        for (i = 0; i < count; i++)
        {
            range.first = (wchar_t) glyph_set->ranges [i].wcLow;

            range.last =
                (range.first + (wchar_t) glyph_set->ranges [i].cGlyphs - 1);

            for (start = 0, end = (XAR_COUNT (character_set) - 1);
                 end >= start;
                 )
            {
                j = ((start + end + 1) / 2);

                if ((*character_set) [j].first < range.first)
                {
                    start = (j + 1);
                }
                else if ((*character_set) [j].first > range.first)
                {
                    end = (j - 1);
                }
                else
                {
                    break;
                }
            }

            if (end < start)
            {
                XAR_INSERT (character_set, start, 1, &range);
            }
        }

        if (XAR_COUNT (character_set) > 0)
        {
            status = UI_SUCCESS;
        }
        else
        {
            XAR_FREE (character_set);
        }

        relmem (&glyph_set);
    }

    SelectFont (device_context, sys_font);
    ReleaseDC (window, device_context);

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_kerning_pairs
| Purpose:  Get the list of kerning pairs of a font
| Input:    font        - the font
| Output:   pairs       - xarray list of kerning pairs defined by the font
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_get_kerning_pairs (void *font, ui_font_kp_t **pairs)
{
#ifdef UI_SYSTEM_NT

    HWND            window = GetDesktopWindow ();
    HDC             device_context = GetDC (window);
    HFONT           sys_font =
        SelectFont (
            device_context,
            (_uint_d2d_enabled () ?
             ((d2d_font_t *) font)->font :
             (HFONT) font));
    DWORD           count, i, j;
    int             start, end;
    KERNINGPAIR    *kerning_pairs;
    ui_font_kp_t    pair;
    int             status = UI_ERROR;

    /*
    ** Get the table of kerning-pairs from the font
    */

    if ((count =
         GetKerningPairs (device_context, 0, (KERNINGPAIR *) NULL)) > 0)
    {
        kerning_pairs = GET_ARRAY (KERNINGPAIR, count);

        count = GetKerningPairs (device_context, count, kerning_pairs);

        /*
        ** Construct our own ordered table of non-zero kerning-pairs
        */

        *pairs = XAR_BEGIN (ui_font_kp_t, 16);

        pair.verified = FALSE;

        for (i = 0; i < count; i++)
        {
            pair.first = (unsigned short) kerning_pairs [i].wFirst;
            pair.second = (unsigned short) kerning_pairs [i].wSecond;
            pair.kerning = (float) kerning_pairs [i].iKernAmount;

            for (start = 0, end = (XAR_COUNT (pairs) - 1);
                 end >= start;
                 )
            {
                j = ((start + end + 1) / 2);

                if ((*pairs) [j].first < pair.first)
                {
                    start = (j + 1);
                }
                else if ((*pairs) [j].first > pair.first)
                {
                    end = (j - 1);
                }
                else if ((*pairs) [j].second < pair.second)
                {
                    start = (j + 1);
                }
                else if ((*pairs) [j].second > pair.second)
                {
                    end = (j - 1);
                }
                else
                {
                    break;
                }
            }

            if (end < start)
            {
                XAR_INSERT (pairs, start, 1, &pair);
            }
        }

        relmem (&kerning_pairs);

        status = UI_SUCCESS;
    }

    SelectFont (device_context, sys_font);
    ReleaseDC (window, device_context);

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_reg_font
| Purpose:  Get the handle of a font from the registry
| Input:    name        - the name of the font to return
| Output:
| Return:   The handle of the font
\*--------------------------------------------------------------------------*/
static HFONT _uint_font_get_reg_font (char *name)
{
    static wchar_t  value_name [1024];
    HKEY            key;
    DWORD           type = 0;
    LOGFONT         font_data;
    DWORD           length = sizeof (LOGFONT);
    HFONT           font = (HFONT) NULL;

    if (RegOpenKeyExW (
            HKEY_CURRENT_USER, L"Control Panel\\Desktop\\WindowMetrics",
            (DWORD) 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
    {
        /*
        ** The data stored in the Windows NT registry is always a WIDE
        ** LOGFONT structure, i.e. a LOGFONTW, so we must use this
        ** structure when retrieving the font data
        */

        strtows (value_name, name);

        if (RegQueryValueExW (
                key, value_name, (DWORD *) NULL, &type, (BYTE *) &font_data,
                &length) == ERROR_SUCCESS &&
            type == REG_BINARY)
        {
            font = CreateFontIndirect (&font_data);
        }

        RegCloseKey (key);
    }

    return (font);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_font_from_HFONT
| Purpose:  Get the font handle from an HFONT
| Input:    font        - the HFONT
|           handle      - the font handle to fall back to
| Output:
| Return:   The font handle of the HFONT
\*--------------------------------------------------------------------------*/
static DBHandle _uint_font_get_font_from_HFONT (HFONT font)
{
    char   *name = (char *) NULL;
    int     style;
    double  size;

    _uint_font_get_info (font, &name, &style, &size);

    return (_ui_font_find (name, style, _ui_font_size_scale (size)));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_from_HFONT
| Purpose:  Convert an HFONT to a font handle
| Input:    font        - the HFONT
|           handle      - the font handle to fall back to
| Output:
| Return:   The font handle of the HFONT
\*--------------------------------------------------------------------------*/
static DBHandle _uint_font_from_HFONT (HFONT font, DBHandle handle)
{
    if (font != (HFONT) NULL)
    {
        handle = _uint_font_get_font_from_HFONT (font);

        DeleteFont (font);
    }

    return (handle);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_font_info
| Purpose:  Get the name, style and size from a font data
| Input:    font_data   - pointer to LOGFONT structure of font data
|           text_metric - font character metrics
| Output:   name        - the name of the font
|           style       - the style of the font
|           size        - the size of the font
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_font_get_font_info (CONST LOGFONT *font_data,
                                     CONST TEXTMETRIC *text_metric,
                                     char **name, int *style, double *size)
{
    static char font_name [1024];
    int         font_style = UI_REGULAR;
    int         font_size;
    int         resolution =
        _ui_dpi_int (UI_DPI_SYSTEM, USER_DEFAULT_SCREEN_DPI);

    wstrtos (
        font_name,
        _ui_unicodetow ((wchar_t *) NULL, font_data->lfFaceName));

    if (font_data->lfItalic)
    {
        font_style |= UI_ITALIC;
    }

    if (font_data->lfWeight >= FW_BOLD)
    {
        font_style |= UI_BOLD;
    }
    else if (font_data->lfWeight >= FW_SEMIBOLD)
    {
        font_style |= UI_SEMIBOLD;
    }
    else if (font_data->lfWeight <= FW_LIGHT)
    {
        font_style |= UI_LIGHT;
    }

    switch (font_data->lfCharSet)
    {
        case HEBREW_CHARSET:
        {
            font_style |= UI_ANSI1255_ENCODING;
            break;
        }

        case ARABIC_CHARSET:
        {
            font_style |= UI_ANSI1256_ENCODING;
            break;
        }

        case GREEK_CHARSET:
        {
            font_style |= UI_ANSI1253_ENCODING;
            break;
        }

        case TURKISH_CHARSET:
        {
            font_style |= UI_ANSI1254_ENCODING;
            break;
        }

        case THAI_CHARSET:
        {
            font_style |= UI_ANSI874_ENCODING;
            break;
        }

        case EASTEUROPE_CHARSET:
        {
            font_style |= UI_ANSI1250_ENCODING;
            break;
        }

        case RUSSIAN_CHARSET:
        {
            font_style |= UI_ANSI1251_ENCODING;
            break;
        }

        case BALTIC_CHARSET:
        {
            font_style |= UI_ANSI1257_ENCODING;
            break;
        }

        case ANSI_CHARSET:
        case DEFAULT_CHARSET:
        {
            font_style |= UI_ISO8859_15_ENCODING;
            break;
        }

        case SHIFTJIS_CHARSET:
        {
            font_style |= UI_SJIS_ENCODING;
            break;
        }

        case HANGUL_CHARSET:
        {
            font_style |= UI_EUC_KR_ENCODING;
            break;
        }

        case GB2312_CHARSET:
        {
            font_style |= UI_GB2312_ENCODING;
            break;
        }

        case CHINESEBIG5_CHARSET:
        {
            font_style |= UI_BIG5_ENCODING;
            break;
        }

        case SYMBOL_CHARSET:
        {
            font_style |= UI_SYMBOLIC_ENCODING;
            break;
        }

        default:
        {
            font_style |= UI_UNKNOWN_ENCODING;
        }
    }

    font_size =
        ((font_data->lfHeight < 0) ?
         -font_data->lfHeight :
         (text_metric->tmHeight - text_metric->tmInternalLeading));

    *name = font_name;
    *style = font_style;
    *size = _ui_font_size_descale ((font_size * 72.0) / resolution);

    return (UI_SUCCESS);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_add_font
| Purpose:  Store a font in the database
| Input:    font_data   - pointer to LOGFONT structure of font data
|           name        - the handle of the name of the font
|           style       - the style of the font
|           size        - the size of the font
|           is_raster   - TRUE if the font is to be treated as a raster font
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_font_add_font (LOGFONT *font_data, DBHandle name, int style,
                                double size, int is_raster)
{
    LOGFONT    *font_data_copy = INIT_STRUCT (LOGFONT, font_data);

    if (UI_FONT_TEXT_STYLE (style) == UI_REGULAR &&
        is_raster)
    {
        font_data_copy->lfItalic = (BYTE) -1;
        style = (UI_FONT_ENCODING (style) | UI_FONT_TEXT_STYLE (-1));
    }

    _ui_krn_font_add_font (name, style, size, font_data_copy);

    return (UI_SUCCESS);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_quality
| Purpose:  Determine the quality of the font to be enforced
| Input:
| Output:
| Return:   The font quality to be enforced, or DEFAULT_QUALITY
\*--------------------------------------------------------------------------*/
static int _uint_font_quality (void)
{
    static int  quality = -1;
    char       *env;

    if (quality < 0)
    {
        quality = DEFAULT_QUALITY;

        if (pro_is_bound ("get_dialog_font_quality"))
        {
            switch ((pro_call ("get_dialog_font_quality")) ())
            {
                case 0:
                {
                    quality = NONANTIALIASED_QUALITY;
                    break;
                }

                case 1:
                {
                    quality = ANTIALIASED_QUALITY;
                    break;
                }

                case 2:
                {
                    quality = CLEARTYPE_QUALITY;
                    break;
                }

                case 3:
                {
                    quality = CLEARTYPE_NATURAL_QUALITY;
                    break;
                }
            }
        }

        if (_ui_string_qcr_enabled ())
        {
            quality = NONANTIALIASED_QUALITY;
        }
        else if ((env = _ui_getenv ("UI_FONT_ANTI_ALIASED")) != (char *) NULL)
        {
            if (*env == 'f' ||
                *env == 'F' ||
                !u_strcmp (env, "N") ||
                !u_strcmp (env, "NO"))
            {
                quality = NONANTIALIASED_QUALITY;
            }
            else if (*env == 't' ||
                     *env == 'T' ||
                     *env == 'y' ||
                     *env == 'Y')
            {
                quality = ANTIALIASED_QUALITY;
            }
            else if (*env == 'c' ||
                     *env == 'C')
            {
                quality = CLEARTYPE_QUALITY;
            }
            else if (*env == 'n' ||
                     *env == 'N')
            {
                quality = CLEARTYPE_NATURAL_QUALITY;
            }
        }
    }

    return (quality);
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_font_create
| Purpose:  Create the system dependent font
| Input:    data            - the system dependent font data
|           private_data    - the private font data
|           style           - the desired style (if applicable)
|           size            - the desired size (if applicable)
| Output:   font            - the system dependent font
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_create (void *data, void *private_data, int style,
                                 double size, void **font)
{
#ifdef UI_SYSTEM_NT

    HFONT       nt_font = (HFONT) *font;
    LOGFONT     font_data;
    int         resolution, quality;
    d2d_font_t *d2d_font;

    if (nt_font == (HFONT) NULL &&
        data != NULL)
    {
        font_data = *((LOGFONT *) data);

        if (font_data.lfItalic == (BYTE) -1)
        {
            /*
            ** If the font is a Raster font then we need to add the italic and
            ** bold styles since Windows will only give us the regular style as
            ** the GDI is capable of creating italic and bold styles from any
            ** non-device dependent raster font
            */

            font_data.lfItalic = ((style & UI_ITALIC) != 0);

            if (style & UI_BOLD)
            {
                font_data.lfWeight = FW_BOLD;
            }
            else if (style & UI_SEMIBOLD)
            {
                font_data.lfWeight = FW_SEMIBOLD;
            }
            else if (style & UI_LIGHT)
            {
                font_data.lfWeight = FW_LIGHT;
            }
        }

        if (font_data.lfHeight == 0 &&
            (size = _ui_font_size_scale (size)) > 0.0)
        {
            /*
            ** If the font is a TrueType (TM) font then we need to give it
            ** some reasonable sizes since the size given by Windows will
            ** be 0
            */

            resolution = _ui_dpi_int (UI_DPI_SYSTEM, USER_DEFAULT_SCREEN_DPI);

            font_data.lfHeight =
                (0 - (int) floor (((size * resolution) / 72.0) + 0.5));
        }

        if ((quality = _uint_font_quality ()) != DEFAULT_QUALITY)
        {
            font_data.lfQuality =
                ((quality == CLEARTYPE_NATURAL_QUALITY) ?
                 CLEARTYPE_QUALITY :
                 (BYTE) quality);
        }

        nt_font = CreateFontIndirect (&font_data);
    }

    if (_uint_d2d_enabled ())
    {
        d2d_font = GETSTRUCT (d2d_font_t);

        d2d_font->font = nt_font;
        d2d_font->format = _uint_font_d2d_create (nt_font, private_data);

        *font = d2d_font;
    }
    else
    {
        *font = nt_font;
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_sysdep_font
| Purpose:  Get the handle of a font for use with UI_get_sysdep_font_Op
| Input:    font        - the system dependent font
| Output:   font        - the handle for use with UI_get_sysdep_font_Op
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_get_sysdep_font (void **font)
{
#ifdef UI_SYSTEM_NT

    if (_uint_d2d_enabled ())
    {
        *font = ((d2d_font_t *) *font)->font;
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_free
| Purpose:  Free a system dependent font
| Input:    font        - the system dependent font to be freed
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_free (void *font)
{
#ifdef UI_SYSTEM_NT

    HFONT       nt_font;
    d2d_font_t *d2d_font;

    if (_uint_d2d_enabled ())
    {
        d2d_font = (d2d_font_t *) font;

        nt_font = d2d_font->font;

        _uint_font_d2d_free (d2d_font->format);

        relmem (&d2d_font);
    }
    else
    {
        nt_font = (HFONT) font;
    }

    DeleteFont (nt_font);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_info
| Purpose:  Get the name, style and size of a font
| Input:    font        - the system dependent font
|           name        - the suggested name of the font
| Output:   name        - the name of the font
|           style       - the style of the font
|           size        - the size of the font
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_font_get_info (void *font, char **name, int *style,
                                   double *size)
{
#ifdef UI_SYSTEM_NT

    HDC         device_context = GetDC ((HWND) NULL);
    HDC         mem_device_context = CreateCompatibleDC (device_context);
    HFONT       sys_font = SelectFont (mem_device_context, (HFONT) font);
    LOGFONT     font_data;
    TEXTMETRIC  text_metric;

    GetObject ((HFONT) font, sizeof (LOGFONT), &font_data);
    GetTextMetrics (mem_device_context, &text_metric);

    SelectFont (mem_device_context, sys_font);
    DeleteDC (mem_device_context);
    ReleaseDC ((HWND) NULL, device_context);

    return (_uint_font_get_font_info (
                &font_data, &text_metric, name, style, size));

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_locale_font
| Purpose:  Get the font to be used to draw text in a font in an encoding
| Input:    font        - the base font to be used
|           encoding    - the desired encoding
| Output:   font        - the best font to be used to draw text in the
|                         given font in an encoding
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
UI_STATIC int _uint_font_get_locale_font (ui_font_t *font, int encoding)
{
#ifdef UI_SYSTEM_NT

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_font_path_wstrcmp
| Purpose:  wstrcmp which ignores whitespace and '-'
| Input:    s1          - the first string
|           s2          - the second string
| Output:
| Return:   As per wstrcmp
\*--------------------------------------------------------------------------*/
static int _uint_font_path_wstrcmp (const wchar_t *s1, const wchar_t *s2)
{
    const wchar_t  *p1 = s1, *p2 = s2;
    int             status = 0;

    do
    {
        while (*p1 == '-' || btk_iswspace (*p1))
        {
            p1++;
        }

        while (*p2 == '-' || btk_iswspace (*p2))
        {
            p2++;
        }

        status = (*p1 - *p2);
    }
    while (!status && *(p1++) != NULL_WCHAR && *(p2++) != NULL_WCHAR);

    return (status);
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_font_get_path
| Purpose:  Get the full path to the font file which defines a font
| Input:    name        - the font name
|           style       - the font style
|           size        - the font size
| Output:   path        - the full path to the font file
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
UI_STATIC int _uint_font_get_path (const char *name, int style, double size,
                                   char **path)
{
#ifdef UI_SYSTEM_NT

    HKEY        key;
    DWORD       i;
    DWORD       map_name_max_length, short_name_max_length;
    wchar_t    *test_name;
    DWORD       map_name_length, short_name_length, type;
    WCHAR      *map_name, *short_name;
    wchar_t   **font_names, *font_name, *comma;
    int         font_name_length, j;
    WCHAR       windows_path [1024] = { 0 };
    int         status = UI_ERROR;

    /*
    ** Open the Windows font mapper database
    */

    if (RegOpenKeyExW (
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
            (DWORD) 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
    {
        /*
        ** Count the fonts in the font mapper
        */

        if (RegQueryInfoKeyW (
                key, (WCHAR *) NULL, (DWORD *) NULL, (DWORD *) NULL,
                (DWORD *) NULL, (DWORD *) NULL, (DWORD *) NULL, &i,
                &map_name_max_length, &short_name_max_length, (DWORD *) NULL,
                (FILETIME *) NULL) == ERROR_SUCCESS)
        {
            /*
            ** Build the font name to be tested
            */

            test_name =
                make_strtows (
                    pro_sprintf (
                        (char *) NULL,
                        "%s%s%s",
                        name,
                        ((style & UI_BOLD) ? " Bold" : ""),
                        ((style & UI_ITALIC) ? " Italic" : "")));

            /*
            ** Allocate buffers for every name and filename in the font mapper
            */

            map_name = GET_ARRAY (WCHAR, (map_name_max_length + 1));
            short_name = GET_ARRAY (WCHAR, (short_name_max_length + 1));

            /*
            ** Loop through the fonts in the font mapper
            */

            while (status != UI_SUCCESS && i-- > 0)
            {
                /*
                ** Get the font name and filename from the registry
                */

                map_name_length = map_name_max_length;
                short_name_length = short_name_max_length;

                if (RegEnumValueW (
                        key, i, map_name, &map_name_length, (DWORD *) NULL,
                        &type, (BYTE *) short_name, &short_name_length)
                        == ERROR_SUCCESS &&
                        type == REG_SZ)
                {
                    /*
                    ** Force the name and filename to be null-terminated
                    */

                    map_name [map_name_length] =
                        short_name [short_name_length] = 0;

                    /*
                    ** Break multiple names separated by '&'
                    */

                    j = make_wstr_sections (map_name, L'&', &font_names);

                    for (j--; status != UI_SUCCESS && j >= 0; j--)
                    {
                        font_name = font_names [j];
                        font_name_length = (int) wstrlen (font_name);

                        /*
                        ** Remove any trailing "(TrueType)" suffix
                        */

                        if (font_name_length > 10 &&
                            !wstrcmp (
                                 (font_name + font_name_length - 10),
                                 L"(TrueType)"))
                        {
                            font_name_length -= 10;

                            font_name [font_name_length] = NULL_WCHAR;
                        }

                        /*
                        ** Remove any non-TrueType font sizes
                        */

                        if ((comma = wstrchr (font_name, L','))
                                != (wchar_t *) NULL)
                        {
                            for (comma--;
                                 comma >= font_name &&
                                 btk_iswdigit (*comma);
                                 comma--);

                            *(++comma) = NULL_WCHAR;

                            font_name_length =
                                (int)(long) (comma - font_name);
                        }

                        /*
                        ** See if the name matches, ignoring whitespace or '-'
                        */

                        if (!(_uint_font_path_wstrcmp (test_name, font_name)))
                        {
                            /*
                            ** Check whether the filename defines a path
                            ** (in which case it will be a full path)
                            */

                            if (wstrchr (short_name, L'\\')
                                    == (wchar_t *) NULL)
                            {
                                /*
                                ** If the filename does not define a path
                                ** then use %SystemRoot%\Fonts
                                */

                                GetWindowsDirectoryW (
                                    windows_path,
                                    NUM_ELEM_IN_ARR (windows_path));
                            }

                            /*
                            ** Build the full path to the font as a
                            ** UTF-8 char *
                            */

                            *path =
                                make_scopy (
                                    pro_sprintf (
                                        (char *) NULL,
                                        "%ws%s%ws",
                                        windows_path,
                                        ((windows_path [0] != 0) ?
                                         "\\Fonts\\" :
                                         ""),
                                        short_name));

                            status = UI_SUCCESS;
                        }
                    }

                    free_wstr_sections (&font_names);
                }
            }

            /*
            ** Free the allocated buffers
            */

            relmem (&short_name);
            relmem (&map_name);

            /*
            ** Free the constructed font name
            */

            relmem (&test_name);
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_enumerator_QueryInterface
| Purpose:  IUnknown::QueryInterface
| Input:    enumerator      - IDWriteFontFileEnumerator object
|           riid            - IID to query
| Output:   object          - Object if query successful
| Return:   S_OK if everything went well, otherwise E_NOINTERFACE
\*--------------------------------------------------------------------------*/
static HRESULT CALLBACK _uint_font_d2d_enumerator_QueryInterface (IUnknown *enumerator,
                                                                  REFIID riid,
                                                                  void **object)
{
    static CONST IID    IID_IDWriteFontFileEnumerator =
        { 0x72755049, 0x5ff7, 0x435d, { 0x83, 0x48, 0x4b, 0xe9, 0x7c, 0xfa, 0x6c, 0x7c } };

    HRESULT             status = E_NOINTERFACE;

    if (IsEqualIID (riid, &IID_IDWriteFontFileEnumerator) ||
        IsEqualIID (riid, &IID_IUnknown))
    {
        if (object != (void **) NULL)
        {
            *object = enumerator;
        }

        IDWriteFontFileEnumerator_AddRef (
            (IDWriteFontFileEnumerator *) enumerator);

        status = S_OK;
    }
    else if (object != (void **) NULL)
    {
        *object = NULL;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_enumerator_AddRef
| Purpose:  IUnknown::AddRef
| Input:    enumerator      - IDWriteFontFileEnumerator object
| Output:
| Return:   The incremented reference count of the object
\*--------------------------------------------------------------------------*/
static ULONG CALLBACK _uint_font_d2d_enumerator_AddRef (IUnknown *enumerator)
{
    d2d_font_file_enumerator_t *object =
        (d2d_font_file_enumerator_t *) enumerator;

    return (InterlockedIncrement ((long *) &(object->refCount)));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_enumerator_Release
| Purpose:  IUnknown::Release
| Input:    enumerator      - IDWriteFontFileEnumerator object
| Output:
| Return:   The decremented reference count of the object
\*--------------------------------------------------------------------------*/
static ULONG CALLBACK _uint_font_d2d_enumerator_Release (IUnknown *enumerator)
{
    d2d_font_file_enumerator_t *object =
        (d2d_font_file_enumerator_t *) enumerator;
    ULONG                       refCount =
        InterlockedDecrement ((long *) &(object->refCount));

    if (refCount == 0ul)
    {
        relmem (&(object->filename));
        relmem (&enumerator);
    }

    return (refCount);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_enumerator_MoveNext
| Purpose:  IDWriteFontFileEnumerator::MoveNext
| Input:    enumerator      - IDWriteFontFileEnumerator object
| Output:   has_file        - TRUE if there is a current file after the move
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
static HRESULT CALLBACK _uint_font_d2d_enumerator_MoveNext (IDWriteFontFileEnumerator *enumerator,
                                                            BOOL *has_file)
{
    d2d_font_file_enumerator_t *object =
        (d2d_font_file_enumerator_t *) enumerator;
    HRESULT                     status = S_OK;

    if (object->current != (IDWriteFontFile *) NULL)
    {
        IDWriteFontFile_Release (object->current);

        object->current = (IDWriteFontFile *) NULL;
    }

    INIT_ARG (
        has_file,
        (object->next != (WCHAR *) NULL &&
         (status =
          IDWriteFactory_CreateFontFileReference (
              object->factory,
              object->next,
              NULL,
              &(object->current))) == S_OK));

    object->next = (WCHAR *) NULL;

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_enumerator_GetCurrentFontFile
| Purpose:  IDWriteFontFileEnumerator::GetCurrentFontFile
| Input:    enumerator      - IDWriteFontFileEnumerator object
| Output:   file            - the current IDWriteFontFile
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
static HRESULT CALLBACK _uint_font_d2d_enumerator_GetCurrentFontFile (IDWriteFontFileEnumerator *enumerator,
                                                                      IDWriteFontFile **file)
{
    d2d_font_file_enumerator_t *object =
        (d2d_font_file_enumerator_t *) enumerator;
    HRESULT                     status = S_OK;

    if ((*file = object->current) == (IDWriteFontFile *) NULL)
    {
        status = E_FAIL;
    }
    else
    {
        IDWriteFontFile_AddRef (object->current);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_loader_QueryInterface
| Purpose:  IUnknown::QueryInterface
| Input:    loader          - IDWriteFontCollectionLoader object
|           riid            - IID to query
| Output:   object          - Object if query successful
| Return:   S_OK if everything went well, otherwise E_NOINTERFACE
\*--------------------------------------------------------------------------*/
static HRESULT CALLBACK _uint_font_d2d_loader_QueryInterface (IUnknown *loader,
                                                              REFIID riid,
                                                              void **object)
{
    static CONST IID    IID_IDWriteFontCollectionLoader =
        { 0xcca920e4, 0x52f0, 0x492b, { 0xbf, 0xa8, 0x29, 0xc7, 0x2e, 0xe0, 0xa4, 0x68 } };

    HRESULT             status = E_NOINTERFACE;

    if (IsEqualIID (riid, &IID_IDWriteFontCollectionLoader) ||
        IsEqualIID (riid, &IID_IUnknown))
    {
        if (object != (void **) NULL)
        {
            *object = loader;
        }

        IDWriteFontCollectionLoader_AddRef (
            (IDWriteFontCollectionLoader *) loader);

        status = S_OK;
    }
    else if (object != (void **) NULL)
    {
        *object = NULL;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_loader_AddRef
| Purpose:  IUnknown::AddRef
| Input:    loader          - IDWriteFontCollectionLoader object
| Output:
| Return:   The incremented reference count of the object
\*--------------------------------------------------------------------------*/
static ULONG CALLBACK _uint_font_d2d_loader_AddRef (IUnknown *loader)
{
    d2d_font_collection_loader_t   *object =
        (d2d_font_collection_loader_t *) loader;

    return (InterlockedIncrement ((long *) &(object->refCount)));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_loader_Release
| Purpose:  IUnknown::Release
| Input:    loader          - IDWriteFontCollectionLoader object
| Output:
| Return:   The decremented reference count of the object
\*--------------------------------------------------------------------------*/
static ULONG CALLBACK _uint_font_d2d_loader_Release (IUnknown *loader)
{
    d2d_font_collection_loader_t   *object =
        (d2d_font_collection_loader_t *) loader;
    ULONG                           refCount =
        InterlockedDecrement ((long *) &(object->refCount));

    if (refCount == 0ul)
    {
        relmem (&loader);
    }

    return (refCount);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_loader_CreateEnumeratorFromKey
| Purpose:  IDWriteFontCollectionLoader::CreateEnumeratorFromKey
| Input:    loader          - IDWriteFontCollectionLoader object
|           factory         - the IDWriteFactory
|           key             - the key (defined by the caller)
|           key_size        - the size of the key
| Output:   enumerator      - the created IDWriteFontFileEnumerator object
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_font_d2d_loader_CreateEnumeratorFromKey (IDWriteFontCollectionLoader *loader,
                                                                       IDWriteFactory *factory,
                                                                       const void *key,
                                                                       UINT32 key_size,
                                                                       IDWriteFontFileEnumerator **enumerator)
{
    static IDWriteFontFileEnumeratorVtbl    methods =
    {
        {
            _uint_font_d2d_enumerator_QueryInterface,
            _uint_font_d2d_enumerator_AddRef,
            _uint_font_d2d_enumerator_Release
        },

        _uint_font_d2d_enumerator_MoveNext,
        _uint_font_d2d_enumerator_GetCurrentFontFile
    };

    d2d_font_file_enumerator_t             *object =
        GETSTRUCT (d2d_font_file_enumerator_t);
    char                                   *filename =
        INIT_ARRAY (char, (const char *) key, key_size);
    HRESULT                                 status = S_OK;

    object->lpVtbl = &methods;

    IDWriteFactory_AddRef (factory);
    object->factory = factory;

    object->filename = make_strtows (filename);
    relmem (&filename);

    object->next = object->filename;

    *enumerator = (IDWriteFontFileEnumerator *) object;

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_collection
| Purpose:  Get the custom IDWriteFontCollection for a font for use
|           with Direct2D
| Input:    filename    - the name of the font
| Output:
| Return:   The custom IDWriteFontCollection for use with Direct2D
\*--------------------------------------------------------------------------*/
static IDWriteFontCollection *_uint_font_d2d_collection (const char *filename)
{
    static IDWriteFontCollectionLoaderVtbl  methods =
    {
        {
            _uint_font_d2d_loader_QueryInterface,
            _uint_font_d2d_loader_AddRef,
            _uint_font_d2d_loader_Release
        },

        _uint_font_d2d_loader_CreateEnumeratorFromKey
    };

    static d2d_font_collection_loader_t    *d2d_collection_loader =
        (d2d_font_collection_loader_t *) NULL;

    IDWriteFactory                         *factory;
    IDWriteFontCollection                  *collection;

    _uint_dwrite_factory (&factory);

    if (d2d_collection_loader == (d2d_font_collection_loader_t *) NULL)
    {
        d2d_collection_loader = GETSTRUCT (d2d_font_collection_loader_t);
        d2d_collection_loader->lpVtbl = &methods;

        if (IDWriteFactory_RegisterFontCollectionLoader (
                factory,
                (IDWriteFontCollectionLoader *) d2d_collection_loader) != S_OK)
        {
            relmem (&d2d_collection_loader);
        }
    }

    if (d2d_collection_loader == (d2d_font_collection_loader_t *) NULL ||
        IDWriteFactory_CreateCustomFontCollection (
            factory, (IDWriteFontCollectionLoader *) d2d_collection_loader,
            filename, ((int) strlen (filename) + 1), &collection) != S_OK)
    {
        collection = (IDWriteFontCollection *) NULL;
    }

    return (collection);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_CreateFontFromLOGFONT
| Purpose:  Create an IDWriteFont from a LOGFONT
| Input:    factory         - the IDWriteFactory
|           device_context  - the device context
|           logfont         - the LOGFONTW
|           collection      - the IDWriteFontCollection defining the font
| Output:   font            - the IDWriteFont
| Return:   TRUE if the font was created
\*--------------------------------------------------------------------------*/
static int _uint_font_d2d_CreateFontFromLOGFONT (IDWriteFactory *factory,
                                                 HDC device_context,
                                                 LOGFONTW *logfont,
                                                 IDWriteFontCollection *collection,
                                                 IDWriteFont **font)
{
    IDWriteGdiInterop  *gdi;
    IDWriteFontFace    *face;
    LOGFONTW            face_logfont;
    int                 status = FALSE;

    if (IDWriteFactory_GetGdiInterop (factory, &gdi) == S_OK)
    {
        status =
            (IDWriteGdiInterop_CreateFontFromLOGFONT (gdi, logfont, font)
                 == S_OK);

        /*
        ** If the font creation fails then we try to find a matching font
        ** by triggering DirectWrite to substitute the font passed in with
        ** a matching font.
        **
        ** This forces DirectWrite to follow any font substitution that has
        ** been made by GDI, e.g. via
        **
        ** HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\FontSubstitutes
        **
        ** https://chromium.googlesource.com/chromium/src/+/0d6b74070c3f53f680264156369e8e2c53993d84
        **
        ** jas - 25-Nov-22
        */

        if (!status &&
            IDWriteGdiInterop_CreateFontFaceFromHdc (
                gdi, device_context, &face) == S_OK)
        {
            if (IDWriteGdiInterop_ConvertFontFaceToLOGFONT (
                    gdi, face, &face_logfont) == S_OK &&
                ((!wstrncmp (
                       logfont->lfFaceName,
                       face_logfont.lfFaceName,
                       (NUM_ELEM_IN_ARR (face_logfont.lfFaceName) - 1)) ||
                  memcpy (
                      logfont->lfFaceName,
                      face_logfont.lfFaceName,
                      sizeof (face_logfont.lfFaceName)) == NULL ||
                  (status =
                       (IDWriteGdiInterop_CreateFontFromLOGFONT (
                            gdi, logfont, font) == S_OK)) == FALSE) &&
                 collection != (IDWriteFontCollection *) NULL))
            {
                status =
                    (IDWriteFontCollection_GetFontFromFontFace (
                         collection, face, font) == S_OK);
            }

            IDWriteFontFace_Release (face);
        }

        IDWriteGdiInterop_Release (gdi);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_GetFontName
| Purpose:  Get the name of a font
| Input:    font        - the IDWriteFont
|           name        - the buffer to hold the name
|           length      - the length of the buffer (in characters)
| Output:   name        - the name of the font
| Return:   TRUE if font name was retrieved
\*--------------------------------------------------------------------------*/
static int _uint_font_d2d_GetFontName (IDWriteFont *font, WCHAR *name,
                                       int length)
{
    IDWriteFontFamily          *family;
    IDWriteLocalizedStrings    *names;
    int                         status = FALSE;

    if (IDWriteFont_GetFontFamily (font, &family) == S_OK)
    {
        if (IDWriteFontFamily_GetFamilyNames (family, &names) == S_OK)
        {
            status =
                (IDWriteLocalizedStrings_GetString (names, 0, name, length)
                     == S_OK);

            IDWriteLocalizedStrings_Release (names);
        }

        IDWriteFontFamily_Release (family);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_CreateTextFormat
| Purpose:  Create an IDWriteTextFormat from an HFONT
| Input:    factory         - the IDWriteFactory
|           device_context  - the device context
|           font_handle     - the font handle
|           collection      - the IDWriteFontCollection defining the font
| Output:   format          - the IDWriteTextFormat
| Return:   TRUE if the text format was created
\*--------------------------------------------------------------------------*/
static int _uint_font_d2d_CreateTextFormat (IDWriteFactory *factory,
                                            HDC device_context,
                                            HFONT font_handle,
                                            IDWriteFontCollection *collection,
                                            IDWriteTextFormat **format)
{
    LOGFONTW                font_data;
    IDWriteFont            *font;
    float                   font_size = 0.0f;
    DWRITE_FONT_METRICS     font_metrics;
    float                   cell_height;
    WCHAR                   font_name [1024];
    WCHAR                   locale_name [LOCALE_NAME_MAX_LENGTH];
    int                     status = FALSE;

    if (GetObject (font_handle, sizeof (LOGFONTW), &font_data) > 0 &&
        _uint_font_d2d_CreateFontFromLOGFONT (
            factory, device_context, &font_data, collection, &font))
    {
        if ((font_size = (font_data.lfHeight * (96.0f / 96.0f))) < 0.0f)
        {
            /*
            ** Negative lfHeight represents the size of the EM square
            */

            font_size = -font_size;
        }
        else
        {
            /*
            ** Positive lfHeight represents the cell height
            ** (ascent + descent)
            */

            IDWriteFont_GetMetrics (font, &font_metrics);

            /*
            ** Convert the cell height from design units to EMs
            */

            cell_height =
                ((float) (font_metrics.ascent + font_metrics.descent) /
                 font_metrics.designUnitsPerEm);

            /*
            ** Divide the font size by the cell height to get the EM size
            */

            font_size /= cell_height;
        }

        /*
        ** The text format includes a locale name. Ideally, this would
        ** be the language of the text, which may or may not be the same
        ** as the primary language of the user. However, for our purposes
        ** the user locale will do.
        */

        if (_uint_font_d2d_GetFontName (
                font, font_name, NUM_ELEM_IN_ARR (font_name)) &&
            GetUserDefaultLocaleName (
                locale_name, LOCALE_NAME_MAX_LENGTH) > 0 &&
            IDWriteFactory_CreateTextFormat (
                factory, font_name, collection, IDWriteFont_GetWeight (font),
                IDWriteFont_GetStyle (font), IDWriteFont_GetStretch (font),
                font_size, locale_name, format) == S_OK)
        {
            status = TRUE;
        }

        IDWriteFont_Release (font);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_create
| Purpose:  Create the IDWriteTextFormat for use with Direct2D
| Input:    font            - the Windows font handle
|           collection      - the IDWriteFontCollection for the font
| Output:
| Return:   The IDWriteTextFormat for use with Direct2D
\*--------------------------------------------------------------------------*/
static IDWriteTextFormat *_uint_font_d2d_create (HFONT font,
                                                 IDWriteFontCollection *collection)
{
    HWND                window;
    HDC                 device_context;
    HFONT               sys_font;
    IDWriteFactory     *factory;
    IDWriteTextFormat  *format = (IDWriteTextFormat *) NULL;

    if (font != (HFONT) NULL)
    {
        window = GetDesktopWindow ();
        device_context = GetDC (window);
        sys_font = SelectFont (device_context, font);

        _uint_dwrite_factory (&factory);

        if (!(_uint_font_d2d_CreateTextFormat (
                  factory, device_context, font, collection, &format)))
        {
            format = (IDWriteTextFormat *) NULL;
        }

        SelectFont (device_context, sys_font);
        ReleaseDC (window, device_context);
    }

    return (format);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_font_d2d_free
| Purpose:  Free the IDWriteTextFormat for use with Direct2D
| Input:    font        - the IDWriteTextFormat to be freed
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_font_d2d_free (IDWriteTextFormat *format)
{
    if (format != (IDWriteTextFormat *) NULL)
    {
        IDWriteTextFormat_Release (format);
    }
}

#endif /* UI_SYSTEM_NT */
