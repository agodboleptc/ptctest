/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    uint_string.c
|
|  Purpose: Windows NT level string and wide string functions
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  13-Feb-96         jas         Created
|  21-Feb-96 G-03-03 UK    $$1   Automatic Submission
|  26-Feb-96         jas         Added sysdep_string_draw_grey
|  05-Mar-96 G-03-04 UK    $$2   Automatic Submission
|  22-Jul-96         pch         Change prototype of _ui_get_image_color to
|                                include a component
|  23-Jul-96         pch         Reverse previous change
|  30-Jul-96 H-01-03 UK    $$3   Automatic Submission
|  11-Sep-96         jas         Moved UNICODE macros from uint.h
|  17-Sep-96 H-01-09 UK    $$4   Automatic Submission
|  07-Oct-96         jas         Added support for DBCS for Windows 95
|  08-Oct-96 H-01-12 UK    $$5   Automatic Submission
|  21-Nov-96         rca         speed changes - using stored string sizes
|  26-Nov-96 H-01-18 UK    $$6   Automatic Submission
|  28-Feb-97         jas         Added support for multi-line text
|  11-Mar-97 H-03-03 UK    $$7   Automatic Submission
|  15-May-97         jas         Added sysdep_string_get_char_size
|  16-May-97         jas         Removed obsolete functions
|  21-May-97 H-03-11 UK    $$8   Automatic Submission
|  27-May-97         jas         Fixed IBM compilation bugs
|  27-May-97 H-03-11+UK    $$9   Patch Submission
|  17-Jul-97         jas         Added string flags
|  29-Jul-97 H-03-17 UK    $$10  Automatic Submission
|  06-Nov-97         jas         Added new color database
|  18-Nov-97 H-03-30 UK    $$11  Automatic Submission
|  28-May-98         jas         Added _uint_unicode_enabled
|  01-Jun-98 I-01-10 UK    $$12  Automatic Submission
|  03-Sep-98         jas         Use 3D grey color for insensitive text
|  08-Sep-98 I-01-18 UK    $$13  Automatic Submission
|  19-Mar-99         jas         Removed sysdep_string_draw_grey
|  09-Apr-99 I-03-07 UK    $$14  Automatic Submission
|  27-Oct-99         jas         Added UI_STRING_DRAW_ONCE
|  03-Nov-99 I-03-20 UK    $$15  Automatic Submission
|  21-Mar-00         jas         Added support for right-to-left drawing
|  29-Mar-00 J-01-05 UK    $$16  Automatic Submission
|  31-Mar-00         jas         Fixed right-to-left drawing problems
|  13-Apr-00         jas         Fixed non-TrueType font drawing problems
|  14-Apr-00         jas         Fixed Windows 2000 drawing problems
|  18-Apr-00 J-01-06 UK    $$17  Automatic Submission
|  04-May-00         jas         Changed prototype for relmem
|  04-May-00         jas         Added non-unicode support for Hebrew
|  05-May-00         jas         Added Hebrew support for non-Hebrew OS's
|  18-May-00 J-01-08 UK    $$18  Automatic Submission
|  07-Sep-01         jas         Fixed mix-mode string drawing problems
|  13-Sep-01 J-03-07 UK    $$19  Automatic Submission
|  09-Oct-01         jas         Use pro_is_win95_running
|  18-Oct-01 J-03-10 UK    $$20  Automatic Submission
|  10-Jan-02         jas         Added support for PTC graphics mode
|  17-Jan-02 J-03-17 UK    $$21  Automatic Submission
|  18-Sep-02         jas         Added support for Windows XP themes
|  25-Sep-02 J-03-34 UK    $$22  Automatic Submission
|  05-Mar-03         jas         Use GFX module
|  11-Mar-03 K-01-02 UK    $$23  Automatic Submission
|  14-Mar-03         jas         Removed _ui_string_get_section... functions
|  26-Mar-03 K-01-03 UK    $$24  Automatic Submission
|  27-Mar-03         jas         Fixed _uint_string_draw
|  08-Apr-03 K-01-04 UK    $$25  Automatic Submission
|  23-Apr-03         jas         Added const qualifiers
|  06-May-03 K-01-06 UK    $$26  Automatic Submission
|  26-Jun-03         jas         Obsoleted ui_memory.h
|  10-Jul-03 K-01-10 UK    $$27  Automatic Submission
|  11-Nov-03         jas         Added UI_STATIC
|  18-Nov-03 K-01-18 UK    $$28  Automatic Submission
|  26-Feb-04         jas         Modified sysdep_string_get_char_size
|  26-Feb-04         jas         Fixed problems with DrawText
|  02-Mar-04 K-01-24 UK    $$29  Automatic Submission
|  22-Dec-04         jas         Include const.h and ctwcfun.h
|  11-Jan-05 K-03-17 UK    $$30  Automatic Submission
|  25-Jan-05         jas         Include ctwcfun_proto.h instead of ctwcfun.h
|  25-Jan-05 K-03-18 UK    $$31  Automatic Submission
|  24-Feb-05         jas         Added UI_STRING_HIDE_MNEMONIC
|  01-Mar-05 K-03-20 UK    $$32  Automatic Submission
|  28-Jun-05         jas         Fixed _uint_string_draw_line
|  14-Jul-05 K-03-28 UK    $$33  Automatic Submission
|  14-Jul-05         jas         Fixed problems with UI_STRING_DRAW_ONCE
|  26-Jul-05 K-03-29 UK    $$34  Automatic Submission
|  31-Aug-05         AW          Undid change to _uint_string_draw_line
|  31-Aug-05 K-03-30+UK    $$35  Patch Submission
|  02-Sep-05         jas         Do not use _ui_string_get_char_direction
|  13-Sep-05 K-03-31 UK    $$36  Automatic Submission
|  21-Oct-05         jas         Reinstated _ui_string_get_char_direction
|  26-Oct-05 K-03-34 UK    $$37  Automatic Submission
|  11-Oct-05         jas         Removed _uint_unicode_enabled
|  11-Oct-05         jas         Removed pro_is_win95_running
|  11-Oct-05         jas         Removed pre-Windows 2000 code
|  31-Jan-06 L-01-01 UK    $$38  Automatic Submission
|  16-Feb-06         jas         Fixed use of wide-string literals
|  24-Feb-06         jas         Removed sysdep_string_get_size
|  28-Feb-06 L-01-03 UK    $$39  Automatic Submission
|  03-Apr-06         jas         Use font handle instead of sysdep font
|  11-Apr-06 L-01-06 UK    $$40  Automatic Submission
|  17-Jul-06         jas         Fixed resource leaks
|  26-Jul-06 L-01-13 UK    $$41  Automatic Submission
|  09-Aug-06         jas         Removed final temporary unicode flags
|  22-Aug-06 L-01-15 UK    $$42  Automatic Submission
|  10-Jan-07         jas         Fixed problems displaying mnemonics
|  10-Jan-07         jas         Added flags to _ui_wtounicode
|  16-Jan-07 L-01-24 UK    $$43  Automatic Submission
|  06-Feb-07         jas         Added text length to _ui_wtounicode
|  13-Feb-07 L-01-26 UK    $$44  Automatic Submission
|  16-May-07         jas         Replaced _ui_wstrnchr with btk_wcsnchr
|  05-Jun-07 L-01-32 UK    $$45  Automatic Submission
|  18-Nov-08         jas         Added support for UTF-16 surrogate pairs
|  02-Dec-08 L-03-21 UK    $$46  Automatic Submission
|  03-Dec-08         jas         Removed UI_STRING_DRAW_GREY/ONCE
|  16-Dec-08 L-03-22 UK    $$47  Automatic Submission
|  06-Aug-09         AW          Fixed UMR error in _uint_string_get_char_size
|  18-Aug-09 L-05-03 UK    $$48  Automatic Submission
|  15-Sep-09         jas         Fixed memory leak in _ui_wtounicode_dir
|  15-Sep-09 L-05-05 UK    $$49  Automatic Submission
|  11-Nov-09         jas         Added support for 32-bit contexts
|  24-Nov-09 L-05-10 UK    $$50  Automatic Submission
|  02-Mar-10         jas         Added sysdep_string_get_kerning_pairs
|  17-Mar-10 L-05-18 UK    $$51  Automatic Submission
|  16-Jul-10         jas         Fixed _ui_wtounicode and _ui_unicodetow
|  20-Jul-10 L-05-27 UK    $$52  Automatic Submission
|  13-Dec-10         jas         Added text origin support
|  04-Jan-11 L-05-39 UK    $$53  Automatic Submission
|  07-Apr-11         jas         Added LCS_DEVICE_CMYK
|  12-Apr-11 L-05-45 UK    $$54  Automatic Submission
|  13-May-11         jas         Fixed character overhang calculation
|  14-Jun-11 P-10-01 UK    $$55  Automatic Submission
|  25-Nov-11         jas         Added UI_STRING_CHAR_LRM/LRE/LRO
|  29-Nov-11 P-10-13 UK    $$56  Automatic Submission
|  01-Feb-12         jas         Improved 32-bit text rendering
|  07-Feb-12 P-10-17 UK    $$57  Automatic Submission
|  14-Mar-12         jas         Further improved 32-bit text rendering
|  20-Mar-12 P-20-01 UK    $$58  Automatic Submission
|  09-May-12         jas         Added _ui_string_compose
|  16-May-12 P-20-05 UK    $$59  Automatic Submission
|  15-Nov-12         jas         Further improved 32-bit text rendering
|  27-Nov-12 P-20-18 UK    $$60  Automatic Submission
|  01-Feb-13         jas         Added UI_STRING_CHAR_LEFTQUOTE
|  07-Feb-13 P-20-23 UK    $$61  Automatic Submission
|  19-Feb-13         jas         Improved precision of character sizing
|  06-Mar-13 P-20-25 UK    $$62  Automatic Submission
|  15-Mar-13         jas         Fixed _uint_string_get_kerning_pairs
|  20-Mar-13 P-20-26 UK    $$63  Automatic Submission
|  06-Nov-15         jas         Added sysdep_string_get_character_set
|  10-Nov-15 P-30-20 UK    $$64  Automatic Submission
|  11-Nov-15         jas         Fixed typo
|  25-Nov-15 P-30-21 UK    $$65  Automatic Submission
|  13-Jun-16         jas         Added convenience macros
|  21-Jun-16 P-30-34 UK    $$66  Automatic Submission
|  21-Feb-17         jas         Added sysdep_string_get_kerning
|  14-Mar-17 P-50-01 UK    $$67  Automatic Submission
|  19-Apr-17         jas         Removed sysdep_string_get_kerning
|  25-Apr-17 P-50-06 UK    $$68  Automatic Submission
|  27-Sep-17         jas         Removed uint_string.h
|  10-Oct-17 P-50-31 UK    $$69  Automatic Submission
|  30-Nov-17         jas         Allow surrogate pairs in ui_font_cs_t
|  05-Dec-17 P-50-39 UK    $$70  Automatic Submission
|  28-Sep-18         jas         Added _ui_font_handle
|  09-Oct-18 P-60-20 UK    $$71  Automatic Submission
|  09-Jan-19         jas         Added _uint_string_is_substituted
|  18-Jan-19 P-60-31 UK    $$72  Automatic Submission
|  09-Jan-19         jas         Removed sysdep_string_get_character_set
|  09-Jan-19         jas         Added UI_STRING_IS_SUBSTITUTED
|  23-Jan-19         jas         Added PRO_CREATE_AND_LOCK_STATIC_BUFFER
|  22-Feb-19         jas         Added UI_COLOR_BRIGHTNESS
|  27-Feb-19         jas         Added _ui_string_get_image
|  08-Mar-19         jas         Removed support for 32-bit contexts
|  19-Mar-19 P-70-01 UK    $$73  Automatic Submission
|  03-Jul-19         jas         Added DPI to string interfaces
|  08-Jul-19 P-70-17 UK    $$74  Automatic Submission
|  08-Jan-20         jas         Compose characters before sizing
|  14-Jan-20 P-70-41 UK    $$75  Automatic Submission
|  23-Apr-20         jas         Changed sysdep_string_draw to draw one line
|  05-May-20 P-80-02 UK    $$76  Automatic Submission
|  26-Sep-22         jas         Added Direct2D and DirectWrite interfaces
|  27-Sep-22         jas         Added sysdep_string_colored_glyphs
|  27-Sep-22         jas         Added UINT_DIRECT2D_MODE
|  28-Sep-22 Q-10-29 UK    $$77  Automatic Submission
|  30-Sep-22         jas         Allow for negative character overhangs
|  06-Oct-22 Q-10-30 UK    $$78  Automatic Submission
|  13-Oct-22         jas         Added WCHAR_T_BITS
|  18-Oct-22 Q-10-32 UK    $$79  Automatic Submission
|  25-Nov-22         jas         Fixed _uint_string_colored_glyphs
|  25-Nov-22         jas         Added fallback to CreateFontFromLOGFONT
|  29-Nov-22         jas         Added _ui_krn_font_get_private_data
|  30-Nov-22 Q-10-37 UK    $$80  Automatic Submission
|  05-Dec-22         jas         Added d2d_font_t
|  07-Dec-22 Q-10-38 UK    $$81  Automatic Submission
|  02-Mar-23         jas         Use floating point character sizes
|  08-Mar-23 Q-11-02 UK    $$82  Automatic Submission
|  28-Mar-23         jas         Added sysdep_string_check_kerning
|  31-Mar-23         jas         Use kerning only if it is significant
|  12-Apr-23 Q-11-07 UK    $$83  Automatic Submission
|  26-Jun-23         jas         Added reset to _uint_d2d_initialize
|  27-Jun-23 Q-11-18 UK    $$84  Automatic Submission
|  20-May-24         jas         Added _uint_d2d_renderer_create/destroy
|  22-May-24 Q-12-13 UK    $$85  Automatic Submission
|  29-Aug-24         jas         Fixed compilation warnings
|  04-Sep-24 Q-12-28 UK    $$86  Automatic Submission
|  25-Mar-25         jas         Added UI_STRING_LEGACY
|  02-Apr-25 Q-13-02 UK    $$87  Automatic Submission
|  10-Jul-25         jas         Fixed compilation warnings
|  15-Jul-25 Q-13-17 UK    $$88  Automatic Submission
|  10-Oct-25         jas         Added ID2D1RenderTarget
|  15-Oct-25 Q-13-30 UK    $$89  Automatic Submission
|  31-Oct-25         jas         Clip text rendering to the context
|  04-Nov-25 Q-13-33 UK    $$90  Automatic Submission
|  12-Nov-25         jas         Added support for DWriteCore
|  19-Nov-25 Q-13-35 UK    $$91  Automatic Submission
|  04-Dec-25         jas         Added sysdep_string_get_char_metrics color
|  04-Dec-25         jas         Added per-char check for colored glyphs
|  09-Dec-25 Q-13-38 UK    $$92  Automatic Submission
|  19-Dec-25         jas         Use per-char check for colored glyphs
|  06-Jan-26 Q-13-42 UK    $$93  Automatic Submission
|  29-May-26         jas         Added flags to sysdep_string_check_kerning
|  29-May-26 Q-27-12 UK    $$94  Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#if !defined (lint) && defined (SHOW_SCCS_ID)
static char uint_string_c_id [] = "@(#) uint_string.c 4212.1@(#)";
#endif

#include <const.h>
#include <ctwcfun_proto.h>
#include <mkscpy.h>
#include <pro_memory.h>
#include <sysmath.h>

#include <ui.h>
#include <uip.h>
#include <ui_gfx.h>
#include <ui_colors.h>
#include <ui_fonts.h>
#include <ui_utils.h>
#include <ui_string.h>
#include <ui_stringp.h>

#ifdef UI_SYSTEM_NT

#include <sdkddkver.h>
#if NTDDI_VERSION < 0x0A000011
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000011
#endif /* NTDDI_VERSION */
#include <uint.h>
#include <uint_d2d1.h>
#include <uint_dwrite.h>
#include <uint_dwrite_1.h>
#include <uint_dwrite_2.h>
#include <uint_dwrite_3.h>


/*
** System dependent font
*/

typedef struct
{
    HFONT               font;
    IDWriteTextFormat  *format;

} d2d_font_t;


/*
** DWriteCore IDWriteBitmapRenderTarget context for IDWriteTextRenderer
*/

typedef struct
{
    IDWriteFactory2    *factory2;
    BOOL                color;

} dw_color_t;


/*
** DWriteCore IDWriteBitmapRenderTarget context for IDWriteTextRenderer
*/

typedef struct
{
    IDWriteBitmapRenderTarget  *bitmap;
    IDWriteBitmapRenderTarget1 *bitmap1;
    IDWriteBitmapRenderTarget3 *bitmap3;
    IDWriteRenderingParams     *params;
    COLORREF                    color;
    ui_rect_t                   underline;

} dwc_context_t;


/*
** Custom IDWriteTextRenderer
*/

typedef struct
{
    IDWriteTextRendererVtbl    *lpVtbl;
    ULONG                       refCount;

} dw_string_renderer_t;


/*
** Function prototypes
*/

static TCHAR *_ui_wtounicode_dir (
    TCHAR          *unicode,
    const wchar_t  *wstring,
    size_t          length,
    int             flags,
    BOOL            direction
);

static int _uint_string_d2d_colored_glyphs (
    IDWriteTextFormat  *format,
    int                *per_char
);

static int _uint_string_d2d_CreateTextLayout (
    IDWriteFactory         *factory,
    const wchar_t          *text,
    IDWriteTextFormat      *format,
    DWRITE_RENDERING_MODE   mode,
    IDWriteTextLayout     **layout
);

static int _uint_string_d2d_get_char_metrics (
    const WCHAR        *characters,
    IDWriteTextFormat  *format,
    float              *width,
    float              *overhang,
    int                *color
);

static int _uint_string_d2d_check_kerning (
    IDWriteTextFormat  *format,
    ui_font_kp_t       *pair,
    const WCHAR        *text,
    float               width
);

static int _uint_string_d2d_draw (
    HDC                 device_context,
    ui_gfx_t           *context,
    const wchar_t      *string,
    UINT                flags,
    IDWriteTextFormat  *format,
    int                 height,
    int                 baseline,
    const RECT         *rect
);

static int _uint_string_dwrite_draw (
    HDC                 device_context,
    ui_gfx_t           *context,
    IDWriteFactory     *factory,
    IDWriteTextLayout  *layout,
    const RECT         *rect,
    const RECT         *clip_rect
);

static int _uint_string_dwrite_draw_colored (
    HDC                 device_context,
    ui_gfx_t           *context,
    IDWriteFactory     *factory,
    IDWriteTextLayout  *layout,
    const RECT         *rect,
    const RECT         *clip_rect
);

static int _uint_string_dwritecore_draw (
    HDC                     device_context,
    ui_gfx_t               *context,
    IDWriteFactory         *factory,
    IDWriteTextLayout      *layout,
    DWRITE_RENDERING_MODE   mode,
    const RECT             *rect,
    const RECT             *clip_rect
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_QueryInterface (
    IUnknown   *renderer,
    REFIID      riid,
    void      **object
);

static ULONG CALLBACK _uint_string_IDWriteTextRenderer_AddRef (
    IUnknown   *renderer
);

static ULONG CALLBACK _uint_string_IDWriteTextRenderer_Release (
    IUnknown   *renderer
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_IsPixelSnappingDisabled (
    IDWritePixelSnapping   *renderer,
    void                   *context,
    BOOL                   *disabled
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_GetCurrentTransform (
    IDWritePixelSnapping   *renderer,
    void                   *context,
    DWRITE_MATRIX          *transform
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_GetPixelsPerDip (
    IDWritePixelSnapping   *renderer,
    void                   *context,
    FLOAT                  *pixels
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawGlyphRun (
    IDWriteTextRenderer                *renderer,
    void                               *context,
    FLOAT                               x,
    FLOAT                               y,
    DWRITE_MEASURING_MODE               mode,
    const DWRITE_GLYPH_RUN             *glyphs,
    const DWRITE_GLYPH_RUN_DESCRIPTION *descriptions,
    IUnknown                           *effect
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawUnderline (
    IDWriteTextRenderer    *renderer,
    void                   *context,
    FLOAT                   x,
    FLOAT                   y,
    const DWRITE_UNDERLINE *underline,
    IUnknown               *effect
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawStrikethrough (
    IDWriteTextRenderer        *renderer,
    void                       *context,
    FLOAT                       x,
    FLOAT                       y,
    const DWRITE_STRIKETHROUGH *strikethrough,
    IUnknown                   *effect
);

static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawInlineObject (
    IDWriteTextRenderer    *renderer,
    void                   *context,
    FLOAT                   x,
    FLOAT                   y,
    IDWriteInlineObject    *inline_object,
    BOOL                    sideways,
    BOOL                    right_to_left,
    IUnknown               *effect
);

static HRESULT _uint_string_IDWriteTextRenderer_Ctor (
    IDWriteFactory         *factory,
    IDWriteTextRenderer   **object
);

static HRESULT CALLBACK _uint_string_dw_color_DrawGlyphRun (
    IDWriteTextRenderer                *renderer,
    void                               *context,
    FLOAT                               x,
    FLOAT                               y,
    DWRITE_MEASURING_MODE               mode,
    const DWRITE_GLYPH_RUN             *glyphs,
    const DWRITE_GLYPH_RUN_DESCRIPTION *descriptions,
    IUnknown                           *effect
);

static HRESULT _uint_string_dw_color_Ctor (
    IDWriteFactory         *factory,
    IDWriteTextRenderer   **object
);

static HRESULT CALLBACK _uint_string_dwc_renderer_GetCurrentTransform (
    IDWritePixelSnapping   *renderer,
    void                   *context,
    DWRITE_MATRIX          *transform
);

static HRESULT CALLBACK _uint_string_dwc_renderer_GetPixelsPerDip (
    IDWritePixelSnapping   *renderer,
    void                   *context,
    FLOAT                  *pixels
);

static HRESULT CALLBACK _uint_string_dwc_renderer_DrawGlyphRun (
    IDWriteTextRenderer                *renderer,
    void                               *context,
    FLOAT                               x,
    FLOAT                               y,
    DWRITE_MEASURING_MODE               mode,
    const DWRITE_GLYPH_RUN             *glyphs,
    const DWRITE_GLYPH_RUN_DESCRIPTION *descriptions,
    IUnknown                           *effect
);

static HRESULT CALLBACK _uint_string_dwc_renderer_DrawUnderline (
    IDWriteTextRenderer    *renderer,
    void                   *context,
    FLOAT                   x,
    FLOAT                   y,
    const DWRITE_UNDERLINE *underline,
    IUnknown               *effect
);

static HRESULT _uint_string_dwc_renderer_Ctor (
    IDWriteFactory         *factory,
    IDWriteTextRenderer   **object
);

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_string_colored_glyphs
| Purpose:  Determine whether a font can render glyphs in color
| Input:    font        - the font
| Output:   per_char    - TRUE if individual glyphs can be rendered in color
| Return:   TRUE if glyphs may be rendered in color
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_string_colored_glyphs (ui_font_t font, int *per_char)
{
#ifdef UI_SYSTEM_NT

    d2d_font_t *d2d_font;

    return (_uint_d2d_enabled () &&
            (d2d_font = _ui_font_handle (font)) != (d2d_font_t *) NULL &&
            d2d_font->format != (IDWriteTextFormat *) NULL &&
            _uint_string_d2d_colored_glyphs (d2d_font->format, per_char));

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_get_char_metrics
| Purpose:  Get the size in pixels of a character
| Input:    character   - the character to find the size of
|           font        - the font to use for the character
|           kerning     - the kerning between two consecutive characters
|           in_range    - TRUE if the character is defined by the font
| Output:   width       - the width of the character
|           overhang    - the overhang of the character
|           color       - TRUE if the character can render in color
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_string_get_char_metrics (unsigned int character,
                                             ui_font_t font, double kerning,
                                             int in_range, float *width,
                                             float *overhang, int *color)
{
#ifdef UI_SYSTEM_NT

    void       *nt_font = _ui_font_handle (font);
    d2d_font_t *d2d_font =
        (d2d_font_t *) (_uint_d2d_enabled () ? nt_font : NULL);
    HWND        window;
    HDC         device_context;
    HFONT       sys_font;
    wchar_t     string;
    WCHAR       characters [5];
    DWORD       length;
    ABC         abc = { 0 };
    RECT        rect = { 0 };
    float       w, o;
    int         is_color = FALSE;

#if WCHAR_T_BITS == 16

    if (UI_STRING_CHAR_IS_SURROGATE_PAIR (character))
    {
        characters [0] = characters [2] =
            (WCHAR) UI_STRING_CHAR_SURROGATE_PAIR_LEAD (character);

        characters [1] = characters [3] =
            (WCHAR) UI_STRING_CHAR_SURROGATE_PAIR_TAIL (character);

        characters [4] = 0;
        length = 2;

        in_range = FALSE;
    }
    else

#endif /* WCHAR_T_BITS */

    {
        string = (wchar_t) character;

        _ui_wtounicode (
            (TCHAR *) characters,
            &string,
            1,
            UI_STRING_USE_SUBSTITUTES);

        characters [1] = characters [0];
        characters [2] = 0;
        length = 1;
    }

    if (d2d_font == (d2d_font_t *) NULL ||
        d2d_font->format == (IDWriteTextFormat *) NULL ||
        _uint_string_d2d_get_char_metrics (
            (characters + length), d2d_font->format, &w, &o,
            &is_color) != UI_SUCCESS)
    {
        window = GetDesktopWindow ();
        device_context = GetDC (window);

        sys_font =
            SelectFont (
                device_context,
                ((d2d_font != (d2d_font_t *) NULL) ?
                 d2d_font->font :
                 (HFONT) nt_font));

        if (in_range &&
            GetCharABCWidthsW (
                device_context, characters [0], characters [0], &abc))
        {
            w = (float) (abc.abcA + abc.abcB + MAX (abc.abcC, 0));

            o = (float) -(MIN (abc.abcC, 0));
        }
        else
        {
            DrawTextW (
                device_context,
                characters,
                length,
                &rect,
                (DT_CALCRECT | DT_NOPREFIX));

            w = (float) (rect.right - rect.left);

            SetRect (&rect, 0, 0, 0, 0);

            DrawTextW (
                device_context,
                characters,
                (length * 2),
                &rect,
                (DT_CALCRECT | DT_NOPREFIX));

            o = ((2 * w) - (float) (rect.right - rect.left) - (float) kerning);
        }

        SelectFont (device_context, sys_font);
        ReleaseDC (window, device_context);
    }

    INIT_ARG (width, w);
    INIT_ARG (overhang, o);
    INIT_ARG (color, is_color);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_check_kerning
| Purpose:  Verify whether a kerning pair is actually being used by a font
| Input:    font        - the font
|           pair        - the kerning pair
|           flags       - the string flags to use
|           width       - the unverified width of the pair
| Output:
| Return:   TRUE if the kerning pair is actually being used
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_string_check_kerning (ui_font_t font, ui_font_kp_t *pair,
                                          int flags, double width)
{
#ifdef UI_SYSTEM_NT

    void       *nt_font = _ui_font_handle (font);
    d2d_font_t *d2d_font =
        (d2d_font_t *) (_uint_d2d_enabled () ? nt_font : NULL);
    HWND        window;
    HDC         device_context;
    HFONT       sys_font;
    WCHAR       text [3];
    RECT        rect = { 0 };
    float       pair_width;

    text [0] = (WCHAR) pair->first;
    text [1] = (WCHAR) pair->second;
    text [2] = (WCHAR) 0;

    if ((flags & UI_STRING_LEGACY) ||
        d2d_font == (d2d_font_t *) NULL ||
        d2d_font->format == (IDWriteTextFormat *) NULL ||
        _uint_string_d2d_check_kerning (
            d2d_font->format, pair, text, (float) width) != UI_SUCCESS)
    {
        window = GetDesktopWindow ();
        device_context = GetDC (window);

        sys_font =
            SelectFont (
                device_context,
                ((d2d_font != (d2d_font_t *) NULL) ?
                 d2d_font->font :
                 (HFONT) nt_font));

        DrawTextW (device_context, text, 2, &rect, (DT_CALCRECT | DT_NOPREFIX));

        SelectFont (device_context, sys_font);
        ReleaseDC (window, device_context);

        /*
        ** If the sizes do not agree, then the kerning-pair information
        ** is wrong - so correct it
        */

        if ((pair_width = (float) (rect.right - rect.left)) != (float) width)
        {
            pair->kerning += (pair_width - (float) width);
        }
    }

    pair->verified = TRUE;

    /*
    ** Use the kerning-pair only if it is significant
    */

    return (fabsf (pair->kerning) > 0.001f);

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_draw
| Purpose:  Draw a string into a context
| Input:    string      - the string to draw
|           flags       - the string flags to use
|           font        - the font to use to draw the string
|           dpi         - the DPI
|           rect        - the position and size of the string
|           context     - the context
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
UI_STATIC int _uint_string_draw (wchar_t *string, int flags, ui_font_t font,
                                 ui_dpi_t dpi, const ui_rect_t *rect,
                                 ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

#if 0
    static D2D1_RENDER_TARGET_PROPERTIES    gdi_properties =
    {
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED
        },
        0.0f,
        0.0f,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
        D2D1_FEATURE_LEVEL_DEFAULT
    };
#endif

    int                             width = rect->width;
    HDC                             device_context = (HDC) context->context;
    void                           *nt_font = _ui_font_handle (font);
    d2d_font_t                     *d2d_font =
        (d2d_font_t *) (_uint_d2d_enabled () ? nt_font : NULL);
    int                             x = rect->x, y = rect->y;
    int                             sys_bk_mode;
    HFONT                           sys_font;
    COLORREF                        sys_fg, sys_bg;
    UINT                            nt_flags = (DT_LEFT | DT_NOCLIP);
    TEXTMETRIC                      text_metrics;
    RECT                            nt_rect;
    BOOL                            fast_draw = FALSE;
    TCHAR                          *nt_string;
    int                             length;
    WCHAR                          *glyphs;
    GCP_RESULTS                     glyph_results;
    int                             rop2;
    HDC                             mem_device_context, draw_device_context;
    HBITMAP                         mem_bitmap, sys_bitmap;
    RECT                            mem_rect;
    DWORD                           mem_rop2;
#if 0
    ID2D1RenderTarget              *renderer =
        (ID2D1RenderTarget *) NULL;
    ID2D1GdiInteropRenderTarget    *gdi_renderer =
        (ID2D1GdiInteropRenderTarget *) NULL;
    HDC                             gdi_device_context = (HDC) NULL;
    HFONT                           gdi_sys_font = (HFONT) NULL;
    HDC                             gdi_draw_device_context = (HDC) NULL;
#endif

    if ((rop2 = GetROP2 (device_context)) != R2_COPYPEN)
    {
        /*
        ** Account for the extra pixels drawn by ClearType
        */

        width++;

        mem_device_context = CreateCompatibleDC (device_context);
        SetMapMode (mem_device_context, GetMapMode (device_context));

        mem_bitmap = CreateBitmap (width, rect->height, 1, 1, NULL);

        sys_bitmap = SelectBitmap (mem_device_context, mem_bitmap);

        SetRect (&mem_rect, 0, 0, width, rect->height);

        FillRect (
            mem_device_context,
            &mem_rect,
            GetStockBrush (WHITE_BRUSH));

        SetTextColor (mem_device_context, COLOR_BLACK);

        x = y = 0;

        draw_device_context = mem_device_context;
    }
    else
    {
        draw_device_context = device_context;
    }

    sys_bk_mode = SetBkMode (draw_device_context, TRANSPARENT);

    sys_font =
        SelectFont (
            draw_device_context,
            ((d2d_font != (d2d_font_t *) NULL) ?
             d2d_font->font :
             (HFONT) nt_font));

    if (!(flags & UI_STRING_HAS_MNEMONIC))
    {
        nt_flags |= DT_NOPREFIX;

        if ((((flags & UI_STRING_IS_SUBSTITUTED) &&
              IsWindows8OrGreater ()) ||
             ((flags & (UI_STRING_IS_ORDERED | UI_STRING_RIGHT_TO_LEFT)) &&
              (GetFontLanguageInfo (draw_device_context) & GCP_GLYPHSHAPE))) &&
            GetTextMetrics (draw_device_context, &text_metrics) &&
            (text_metrics.tmPitchAndFamily & TMPF_TRUETYPE))
        {
            fast_draw = TRUE;

            SetTextAlign (draw_device_context, TA_LEFT);
        }
    }
    else if (flags & UI_STRING_HIDE_MNEMONIC)
    {
        nt_flags |= DT_HIDEPREFIX;
    }

    nt_rect.left = x;
    nt_rect.top = y;
    nt_rect.right = (nt_rect.left + width);
    nt_rect.bottom = (nt_rect.top + rect->height);

    nt_string =
        _ui_wtounicode_dir (
            (TCHAR *) NULL,
            string,
            MAX__SIZE_T,
            (flags | UI_STRING_USE_SUBSTITUTES),
            !fast_draw);

    if ((flags & UI_STRING_LEGACY) ||
        d2d_font == (d2d_font_t *) NULL ||
        d2d_font->format == (IDWriteTextFormat *) NULL ||
        _uint_string_d2d_draw (
            draw_device_context, context, (wchar_t *) nt_string,
            nt_flags, d2d_font->format, _ui_font_get_height (font),
            _ui_font_get_ascent (font), &nt_rect) != UI_SUCCESS)
    {
#if 0
        if (_ui_gfx_force_accelerated (context) &&
            _uint_d2d_renderer_create (
                draw_device_context, (RECT *) NULL, FALSE, &renderer) &&
            ID2D1RenderTarget_IsSupported (renderer, &gdi_properties) &&
            ID2D1RenderTarget_QueryInterface (
                renderer, &IID_ID2D1GdiInteropRenderTarget,
                (void **) &gdi_renderer) == S_OK &&
            gdi_renderer != (ID2D1GdiInteropRenderTarget *) NULL &&
            ID2D1GdiInteropRenderTarget_GetDC (
                gdi_renderer, D2D1_DC_INITIALIZE_MODE_COPY,
                &gdi_device_context) == S_OK &&
            gdi_device_context != (HDC) NULL)
        {
            gdi_sys_font =
                SelectFont (
                    gdi_device_context,
                    ((d2d_font != (d2d_font_t *) NULL) ?
                     d2d_font->font :
                     (HFONT) nt_font));

            gdi_draw_device_context = draw_device_context;

            draw_device_context = gdi_device_context;
        }
#endif

        if (fast_draw)
        {
            length = (int) lstrlen (nt_string);

            glyph_results.lStructSize = sizeof (glyph_results);
            glyph_results.lpOutString = (TCHAR *) NULL;
            glyph_results.lpOrder = (UINT *) NULL;
            glyph_results.lpDx = (int *) NULL;
            glyph_results.lpCaretPos = (int *) NULL;
            glyph_results.lpClass = (char *) NULL;

            /*
            ** Uniscribe requires 1.5 times more glyphs than the length of the
            ** string, as documented in ScriptStringAnalyse():
            **
            ** https://docs.microsoft.com/en-us/windows/desktop/api/usp10/nf-usp10-scriptstringanalyse
            */

            glyph_results.nGlyphs = (((length * 3) / 2) + 16);

            PRO_CREATE_AND_LOCK_STATIC_BUFFER (
                glyphs, 1024, (int) glyph_results.nGlyphs);
            {
                glyph_results.lpGlyphs = glyphs;

                GetCharacterPlacement (
                    draw_device_context,
                    nt_string,
                    length,
                    0,
                    &glyph_results,
                    GCP_GLYPHSHAPE);

                ExtTextOut (
                    draw_device_context,
                    nt_rect.left,
                    nt_rect.top,
                    ETO_GLYPH_INDEX,
                    &nt_rect,
                    (TCHAR *) glyph_results.lpGlyphs,
                    glyph_results.nGlyphs,
                    (int *) NULL);
            }
            PRO_UNLOCK_STATIC_BUFFER (glyphs);
        }
        else
        {
            DrawText (
                draw_device_context,
                nt_string,
                -1,
                &nt_rect,
                nt_flags);
        }

#if 0
        if (gdi_renderer != (ID2D1GdiInteropRenderTarget *) NULL)
        {
            draw_device_context = gdi_draw_device_context;

            if (gdi_device_context != (HDC) NULL)
            {
                SelectFont (gdi_device_context, gdi_sys_font);

                ID2D1GdiInteropRenderTarget_ReleaseDC (
                    gdi_renderer,
                    &nt_rect);
            }

            ID2D1GdiInteropRenderTarget_Release (gdi_renderer);
        }
#endif
    }

    SelectFont (draw_device_context, sys_font);
    SetBkMode (draw_device_context, sys_bk_mode);

    if (rop2 != R2_COPYPEN)
    {
        switch (rop2)
        {
            case R2_NOT:
            {
                sys_fg = SetTextColor (device_context, COLOR_WHITE);
                sys_bg = SetBkColor (device_context, COLOR_BLACK);
                mem_rop2 = SRCINVERT;
                break;
            }

            case R2_MASKPEN:
            {
                mem_rop2 = SRCAND;
                break;
            }

            case R2_MERGEPEN:
            {
                mem_rop2 = SRCPAINT;
                break;
            }

            case R2_XORPEN:
            {
                sys_fg =
                    SetTextColor (
                        device_context,
                        (GetTextColor (device_context) ^
                         GetBkColor (device_context)));

                sys_bg = SetBkColor (device_context, COLOR_BLACK);

                mem_rop2 = SRCINVERT;
                break;
            }

            default:
            {
                mem_rop2 = SRCCOPY;
            }
        }

        BitBlt (
            device_context,
            rect->x,
            rect->y,
            width,
            rect->height,
            mem_device_context,
            0,
            0,
            mem_rop2);

        switch (rop2)
        {
            case R2_NOT:
            case R2_XORPEN:
            {
                SetTextColor (device_context, sys_fg);
                SetBkColor (device_context, sys_bg);
                break;
            }
        }

        SelectBitmap (mem_device_context, sys_bitmap);
        DeleteDC (mem_device_context);
        DeleteBitmap (mem_bitmap);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _ui_wtounicode
| Purpose:  Convert a wide string to a unicode string
| Input:    wstring     - wide string
|           length      - the length of the string
|           flags       - the string flags to use
| Output:   unicode     - unicode string
| Return:   unicode, unless unicode is NULL, in which case the return value
|           is a pointer to an internal buffer
\*--------------------------------------------------------------------------*/
TCHAR *_ui_wtounicode (TCHAR *unicode, const wchar_t *wstring, size_t length,
                       int flags)
{
    return (_ui_wtounicode_dir (unicode, wstring, length, flags, FALSE));
}


/*--------------------------------------------------------------------------*\
| Function: _ui_wtounicode_dir
| Purpose:  Convert a wide string to a unicode string
| Input:    wstring     - wide string
|           length      - the length of the string
|           flags       - the string flags to use
|           direction   - flag indicating whether directional text is being
|                         used and hence whether directional control
|                         characters should be inserted into the string
| Output:   unicode     - unicode string
| Return:   unicode, unless unicode is NULL, in which case the return value
|           is a pointer to an internal buffer
\*--------------------------------------------------------------------------*/
static TCHAR *_ui_wtounicode_dir (TCHAR *unicode, const wchar_t *wstring,
                                  size_t length, int flags, BOOL direction)
{
    wchar_t    *string =
        _ui_string_compose (wstring, length, flags, (int) direction, L"\r\n");

    return ((TCHAR *)
            ((unicode != (TCHAR *) NULL) ?
             wstrcpy ((wchar_t *) unicode, string) :
             string));
}


/*--------------------------------------------------------------------------*\
| Function: _ui_unicodetow
| Purpose:  Convert a unicode string to a wide string
| Input:    unicode     - unicode string
| Output:   wstring     - wide string
| Return:   wstring, unless wstring is NULL, in which case the return value
|           is a pointer to an internal buffer
\*--------------------------------------------------------------------------*/
wchar_t *_ui_unicodetow (wchar_t *wstring, const TCHAR *unicode)
{
    static wchar_t *wstr_buffer = (wchar_t *) NULL;
    WCHAR          *wchar_string;
    wchar_t        *ptr;

    if (wstring == (wchar_t *) NULL)
    {
        wstr_buffer =
            relocmem (
                wstr_buffer,
                (sizeof (wchar_t) * ((int) lstrlen (unicode) + 1)));

        wstring = wstr_buffer;
    }

    for (wchar_string = (WCHAR *) unicode, ptr = wstring;
         *wchar_string != (WCHAR) 0;
         wchar_string++, ptr++)
    {
        if (*wchar_string == (WCHAR) '\r' &&
            *(wchar_string + 1) == (WCHAR) '\n')
        {
            wchar_string++;
        }

        *ptr = (wchar_t) *wchar_string;
    }

    *ptr = NULL_WCHAR;

    return (wstring);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_d2d_colored_glyphs
| Purpose:  Determine whether a font using Direct2D can render glyphs in color
| Input:    format      - the IDWriteTextFormat defining the font
| Output:   per_char    - TRUE if individual glyphs can be rendered in color
| Return:   TRUE if glyphs may be rendered in color
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static int _uint_string_d2d_colored_glyphs (IDWriteTextFormat *format,
                                            int *per_char)
{
    /*
    ** Enabling this flag will force DirectDraw to be used only to draw
    ** colored glyphs, falling back to GDI for all other characters
    **
    ** jas - 04-Dec-25
    */

    INIT_ARG (per_char, TRUE);

    return (IsWindows8Point1OrGreater ());
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_d2d_CreateTextLayout
| Purpose:  Create an IDWriteTextLayout for a text string
| Input:    factory         - the IDWriteFactory
|           text            - the text
|           format          - the IDWriteTextFormat defining the font
|           mode            - the DWRITE_RENDERING_MODE
| Output:   layout          - the IDWriteTextLayout
| Return:   TRUE if the text layout was created
\*--------------------------------------------------------------------------*/
static int _uint_string_d2d_CreateTextLayout (IDWriteFactory *factory,
                                              const wchar_t *text,
                                              IDWriteTextFormat *format,
                                              DWRITE_RENDERING_MODE mode,
                                              IDWriteTextLayout **layout)
{
    int status = FALSE;

    if (format != (IDWriteTextFormat *) NULL)
    {
        IDWriteTextFormat_SetWordWrapping (
            format,
            DWRITE_WORD_WRAPPING_NO_WRAP);

        switch (mode)
        {
            case DWRITE_RENDERING_MODE_NATURAL:
            {
                status =
                    (IDWriteFactory_CreateTextLayout (
                         factory, text, (int) wstrlen (text), format, 0.0f,
                         0.0f, layout) == S_OK);

                break;
            }

            case DWRITE_RENDERING_MODE_GDI_NATURAL:
            {
                status =
                    (IDWriteFactory_CreateGdiCompatibleTextLayout (
                         factory, text, (int) wstrlen (text), format, 0.0f,
                         0.0f, 1.0f, (DWRITE_MATRIX *) NULL, TRUE,
                         layout) == S_OK);

                break;
            }

            default:
            {
                /*
                ** IDWriteTextLayout for DWRITE_RENDERING_MODE_GDI_CLASSIC
                */

                status =
                    (IDWriteFactory_CreateGdiCompatibleTextLayout (
                         factory, text, (int) wstrlen (text), format, 0.0f,
                         0.0f, 1.0f, (DWRITE_MATRIX *) NULL, FALSE,
                         layout) == S_OK);
            }
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_d2d_get_char_metrics
| Purpose:  Get the size in pixels of a character
| Input:    characters      - the characters to find the size of
|           format          - the IDWriteTextFormat defining the font
| Output:   width           - the width of the characters
|           overhang        - the overhang of the characters
|           color           - TRUE if the character can render in color
| Return:   UI_SUCCESS if the characters were sized using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_string_d2d_get_char_metrics (const WCHAR *characters,
                                              IDWriteTextFormat *format,
                                              float *width, float *overhang,
                                              int *color)
{
    static CONST IID            IID_IDWriteFactory2 =
        { 0x0439fc60, 0xca44, 0x4994, { 0x8d, 0xee, 0x3a, 0x9a, 0xf7, 0xb7, 0x32, 0xec } };

    static IDWriteFactory2     *factory2 = (IDWriteFactory2 *) NULL;
    static IDWriteTextRenderer *renderer = (IDWriteTextRenderer *) NULL;

    IDWriteFactory             *factory;
    DWRITE_RENDERING_MODE       mode = _uint_dwrite_factory (&factory);
    int                         per_char = FALSE;
    IDWriteTextLayout          *layout;
    DWRITE_TEXT_METRICS         metrics;
    DWRITE_OVERHANG_METRICS     overhangs;
    dw_color_t                  context;
    int                         status = UI_ERROR;

    if (mode != DWRITE_RENDERING_MODE_DEFAULT &&
        _uint_string_d2d_CreateTextLayout (
            factory, characters, format, mode, &layout))
    {
        if (_uint_string_d2d_colored_glyphs (format, &per_char))
        {
            if (!per_char)
            {
                status = UI_SUCCESS;
            }

            if (factory2 == (IDWriteFactory2 *) NULL)
            {
                IDWriteFactory_QueryInterface (
                    factory,
                    &IID_IDWriteFactory2,
                    (void **) &factory2);
            }

            if (renderer == (IDWriteTextRenderer *) NULL)
            {
                _uint_string_dw_color_Ctor (factory, &renderer);
            }

            if ((context.factory2 = factory2) != (IDWriteFactory2 *) NULL &&
                renderer != (IDWriteTextRenderer *) NULL)
            {
                context.color = FALSE;

                IDWriteTextLayout_Draw (
                    layout,
                    &context,
                    (IDWriteTextRenderer *) renderer,
                    0.0f,
                    0.0f);

                if (context.color)
                {
                    *color = TRUE;

                    status = UI_SUCCESS;
                }
            }
        }
        else
        {
            status = UI_SUCCESS;
        }

        if (status == UI_SUCCESS &&
            IDWriteTextLayout_GetMetrics (layout, &metrics) == S_OK)
        {
            if (IDWriteTextLayout_GetOverhangMetrics (layout, &overhangs)
                    == S_OK &&
                overhangs.right > metrics.widthIncludingTrailingWhitespace)
            {
                overhangs.right -= metrics.widthIncludingTrailingWhitespace;
            }
            else
            {
                overhangs.right = 0.0f;
            }

            *width =
                (metrics.widthIncludingTrailingWhitespace + overhangs.right);

            *overhang = overhangs.right;

            status = UI_SUCCESS;
        }
        else
        {
            *color = FALSE;

            status = UI_ERROR;
        }

        IDWriteTextLayout_Release (layout);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_d2d_check_kerning
| Purpose:  Verify whether a kerning pair is actually being used by a font
|           using Direct2D
| Input:    format          - the IDWriteTextFormat defining the font
|           pair            - the kerning pair
|           text            - the kerning pair as a text string
|           width           - the unverified width of the pair
| Output:
| Return:   UI_SUCCESS if the kerning was verified using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_string_d2d_check_kerning (IDWriteTextFormat *format,
                                           ui_font_kp_t *pair,
                                           const WCHAR *text, float width)
{
    IDWriteFactory         *factory;
    DWRITE_RENDERING_MODE   mode = _uint_dwrite_factory (&factory);
    IDWriteTextLayout      *layout;
    DWRITE_TEXT_METRICS     metrics;
    int                     status = UI_ERROR;

    if (mode != DWRITE_RENDERING_MODE_DEFAULT &&
        _uint_string_d2d_CreateTextLayout (
            factory, text, format, mode, &layout))
    {
        if (IDWriteTextLayout_GetMetrics (layout, &metrics) == S_OK)
        {
            /*
            ** If the sizes do not agree, then the kerning-pair information
            ** is wrong - so correct it
            */

            if (metrics.widthIncludingTrailingWhitespace != width)
            {
                pair->kerning +=
                    (metrics.widthIncludingTrailingWhitespace - width);
            }

            status = UI_SUCCESS;
        }

        IDWriteTextLayout_Release (layout);

        if (status != UI_SUCCESS)
        {
            pair->kerning = 0.0f;

            status = UI_SUCCESS;
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_d2d_draw
| Purpose:  Draw a string into a context using Direct2D
| Input:    device_context  - the device context
|           string          - the string to draw
|           flags           - the drawing flags
|           format          - the IDWriteTextFormat defining the font
|           height          - the height of the font
|           baseline        - the baseline of the font
|           rect            - the position and size of the string
| Output:
| Return:   UI_SUCCESS if the string was drawn using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_string_d2d_draw (HDC device_context, ui_gfx_t *context,
                                  const wchar_t *string, UINT flags,
                                  IDWriteTextFormat *format, int height,
                                  int baseline, const RECT *rect)
{
    RECT                    clip_rect = *rect;
    HRGN                    clip_region;
    ID2D1Factory           *d2d_factory;
    IDWriteFactory         *dwrite_factory;
    DWRITE_RENDERING_MODE   mode = _uint_dwrite_factory (&dwrite_factory);
    DWRITE_TEXT_RANGE       underline = { (UINT) -1, 1 };
    wchar_t                 mnemonic = NULL_WCHAR;
    IDWriteTextLayout      *layout;
    int                     per_char = FALSE;
    int                     status = UI_ERROR;

    if (context->region != NULL)
    {
        clip_region = CreateRectRgnIndirect (rect);
        CombineRgn (clip_region, clip_region, (HRGN) context->region, RGN_AND);
        GetRgnBox (clip_region, &clip_rect);
        DeleteRgn (clip_region);
    }

    if (clip_rect.right <= clip_rect.left ||
        clip_rect.bottom <= clip_rect.top)
    {
        status = UI_SUCCESS;
    }
    else if (mode != DWRITE_RENDERING_MODE_DEFAULT)
    {
        if (!(flags & DT_NOPREFIX))
        {
            string =
                _ui_string_remove_mnemonic_char (
                    string,
                    MAX__SIZE_T,
                    &mnemonic,
                    ((flags & DT_HIDEPREFIX) ?
                     (int *) NULL :
                     (int *) &(underline.startPosition)));
        }

        if (_uint_string_d2d_CreateTextLayout (
                dwrite_factory, string, format, mode, &layout))
        {
            if (mnemonic != NULL_WCHAR)
            {
                IDWriteTextLayout_SetUnderline (layout, TRUE, underline);
            }

            IDWriteTextLayout_SetLineSpacing (
                layout,
                DWRITE_LINE_SPACING_METHOD_UNIFORM,
                (float) height,
                (float) baseline);

            if (!(_uint_d2d_factory (&d2d_factory)))
            {
                status =
                    _uint_string_dwritecore_draw (
                        device_context,
                        context,
                        dwrite_factory,
                        layout,
                        mode,
                        rect,
                        &clip_rect);
            }
            else if (_uint_string_d2d_colored_glyphs (
                         (IDWriteTextFormat *) NULL, &per_char) &&
                     per_char)
            {
                status =
                    _uint_string_dwrite_draw_colored (
                        device_context,
                        context,
                        dwrite_factory,
                        layout,
                        rect,
                        &clip_rect);
            }
            else
            {
                status =
                    _uint_string_dwrite_draw (
                        device_context,
                        context,
                        dwrite_factory,
                        layout,
                        rect,
                        &clip_rect);
            }

            IDWriteTextLayout_Release (layout);
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwrite_draw
| Purpose:  Draw a string into a context using DirectWrite
| Input:    device_context  - the device context
|           context         - the drawing context
|           factory         - the IDWriteFactory
|           layout          - the IDWriteTextLayout to draw
|           rect            - the position and size of the string
|           clip_rect       - the clip rectangle
| Output:
| Return:   UI_SUCCESS if the string was drawn using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_string_dwrite_draw (HDC device_context, ui_gfx_t *context,
                                     IDWriteFactory *factory,
                                     IDWriteTextLayout *layout,
                                     const RECT *rect, const RECT *clip_rect)
{
    COLORREF                fg_color = GetTextColor (device_context);
    D2D1_COLOR_F            color;
    RECT                    renderer_rect;
    D2D1_POINT_2F           origin = { 0.0f, 0.0f };
    ID2D1RenderTarget      *renderer;
    ID2D1SolidColorBrush   *brush;
    int                     status = UI_ERROR;

    color.r = ((float) GetRValue (fg_color) / 255.0f);
    color.g = ((float) GetGValue (fg_color) / 255.0f);
    color.b = ((float) GetBValue (fg_color) / 255.0f);
    color.a = 1.0f;

    if (_ui_gfx_is_accelerated (context))
    {
        renderer_rect.left = context->rect.x;
        renderer_rect.top = context->rect.y;
        renderer_rect.right = (renderer_rect.left + context->rect.width);
        renderer_rect.bottom = (renderer_rect.top + context->rect.height);

        origin.x = (FLOAT) clip_rect->left;
        origin.y = (FLOAT) clip_rect->top;
    }
    else
    {
        renderer_rect = *clip_rect;
    }

    if (_uint_d2d_renderer_create (
            device_context, &renderer_rect, TRUE, &renderer) &&
        ID2D1RenderTarget_CreateSolidColorBrush (
            renderer, &color, (D2D1_BRUSH_PROPERTIES *) NULL,
            &brush) == S_OK)
    {
        if (rect->left < clip_rect->left)
        {
            origin.x += (FLOAT) (rect->left - clip_rect->left);
        }

        if (rect->top < clip_rect->top)
        {
            origin.y += (FLOAT) (rect->top - clip_rect->top);
        }

        ID2D1RenderTarget_DrawTextLayout (
            renderer,
            origin,
            layout,
            (ID2D1Brush *) brush,
            (_uint_string_d2d_colored_glyphs (
                 (IDWriteTextFormat *) NULL, (int *) NULL) ?
             D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT :
             D2D1_DRAW_TEXT_OPTIONS_NONE));

        ID2D1SolidColorBrush_Release (brush);

        if (!(_ui_gfx_is_accelerated (context)))
        {
            _uint_d2d_renderer_destroy (device_context);
        }

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwrite_draw_colored
| Purpose:  Draw a string into a context using DirectWrite
| Input:    device_context  - the device context
|           context         - the drawing context
|           factory         - the IDWriteFactory
|           layout          - the IDWriteTextLayout to draw
|           rect            - the position and size of the string
|           clip_rect       - the clip rectangle
| Output:
| Return:   UI_SUCCESS if the string was drawn using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_string_dwrite_draw_colored (HDC device_context,
                                             ui_gfx_t *context,
                                             IDWriteFactory *factory,
                                             IDWriteTextLayout *layout,
                                             const RECT *rect,
                                             const RECT *clip_rect)
{
    static D2D1_RENDER_TARGET_PROPERTIES    properties =
    {
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED
        },
        0,
        0,
        D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
        D2D1_FEATURE_LEVEL_DEFAULT
    };

    static ID2D1DCRenderTarget             *renderer =
        (ID2D1DCRenderTarget *) NULL;

    ID2D1Factory                           *d2d_factory;
    ID2D1SolidColorBrush                   *brush;
    HRESULT                                 result;
    COLORREF                                fg_color =
        GetTextColor (device_context);
    D2D1_COLOR_F                            color;
    D2D1_POINT_2F                           origin = { 0.0f, 0.0f };
    int                                     i;
    int                                     status = UI_ERROR;

    color.r = ((float) GetRValue (fg_color) / 255.0f);
    color.g = ((float) GetGValue (fg_color) / 255.0f);
    color.b = ((float) GetBValue (fg_color) / 255.0f);
    color.a = 1.0f;

    if (rect->left < clip_rect->left)
    {
        origin.x += (FLOAT) (rect->left - clip_rect->left);
    }

    if (rect->top < clip_rect->top)
    {
        origin.y += (FLOAT) (rect->top - clip_rect->top);
    }

    for (i = 2; i > 0; i--)
    {
        if (renderer != (ID2D1DCRenderTarget *) NULL ||
            (_uint_d2d_factory (&d2d_factory) &&
             ID2D1Factory_CreateDCRenderTarget (
                 d2d_factory, &properties, &renderer) == S_OK &&
             renderer != (ID2D1DCRenderTarget *) NULL))
        {
            if ((result =
                 ID2D1DCRenderTarget_BindDC (
                     renderer, device_context, clip_rect)) == S_OK)
            {
                ID2D1DCRenderTarget_BeginDraw (renderer);

                if (ID2D1DCRenderTarget_CreateSolidColorBrush (
                        renderer, &color, (D2D1_BRUSH_PROPERTIES *) NULL,
                        &brush) == S_OK)
                {
                    ID2D1DCRenderTarget_DrawTextLayout (
                        renderer,
                        origin,
                        layout,
                        (ID2D1Brush *) brush,
                        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

                    ID2D1SolidColorBrush_Release (brush);

                    status = UI_SUCCESS;
                }

                result =
                    ID2D1DCRenderTarget_EndDraw (
                        renderer,
                        (D2D1_TAG *) NULL,
                        (D2D1_TAG *) NULL);
            }

            if (result == S_OK)
            {
                break;
            }
            else if (result == D2DERR_RECREATE_TARGET)
            {
                ID2D1DCRenderTarget_Release (renderer);

                renderer = (ID2D1DCRenderTarget *) NULL;
            }
            else
            {
                status = UI_ERROR;

                break;
            }
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwritecore_draw
| Purpose:  Draw a string into a context using DWriteCore
| Input:    device_context  - the device context
|           context         - the drawing context
|           factory         - the DWriteCore IDWriteFactory
|           layout          - the IDWriteTextLayout to draw
|           mode            - the DWRITE_RENDERING_MODE
|           rect            - the position and size of the string
|           clip_rect       - the clip rectangle
| Output:
| Return:   UI_SUCCESS if the string was drawn using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_string_dwritecore_draw (HDC device_context,
                                         ui_gfx_t *context,
                                         IDWriteFactory *factory,
                                         IDWriteTextLayout *layout,
                                         DWRITE_RENDERING_MODE mode,
                                         const RECT *rect,
                                         const RECT *clip_rect)
{
    static CONST IID                IID_IDWriteBitmapRenderTarget1 =
        { 0x791e8298, 0x3ef3, 0x4230, { 0x98, 0x80, 0xc9, 0xbd, 0xec, 0xc4, 0x20, 0x64 } };
    static CONST IID                IID_IDWriteBitmapRenderTarget3 =
        { 0xAEEC37DB, 0xC337, 0x40F1, { 0x8E, 0x2A, 0x9A, 0x41, 0xB1, 0x67, 0xB2, 0x38 } };
    static IDWriteGdiInterop       *gdi = (IDWriteGdiInterop *) NULL;
    static IDWriteRenderingParams  *params = (IDWriteRenderingParams *) NULL;
    static IDWriteTextRenderer     *renderer = (IDWriteTextRenderer *) NULL;
    FLOAT                           gamma;
    FLOAT                           enhanced_contrast;
    FLOAT                           cleartype_level;
    DWRITE_PIXEL_GEOMETRY           pixel_geometry;
    dwc_context_t                   bitmap_context;
    HDC                             bitmap_device_context;
    int                             x = clip_rect->left;
    int                             y = clip_rect->top;
    int                             width = (clip_rect->right - x);
    int                             height = (clip_rect->bottom - y);
    BLENDFUNCTION                   blend_function =
        { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    int                             status = UI_ERROR;

    if (gdi == (IDWriteGdiInterop *) NULL)
    {
        IDWriteFactory_GetGdiInterop (factory, &gdi);
    }

    if (params == (IDWriteRenderingParams *) NULL)
    {
        IDWriteFactory_CreateMonitorRenderingParams (
            factory,
            MonitorFromWindow (
                _uint_get_app_window (),
                MONITOR_DEFAULTTOPRIMARY),
            &params);

        gamma =
            IDWriteRenderingParams_GetGamma (params);

        enhanced_contrast =
            IDWriteRenderingParams_GetEnhancedContrast (params);

        cleartype_level =
            IDWriteRenderingParams_GetClearTypeLevel (params);

#if 0
        pixel_geometry =
            IDWriteRenderingParams_GetPixelGeometry (params);
#else
        pixel_geometry = DWRITE_PIXEL_GEOMETRY_FLAT;
#endif

        if (mode != DWRITE_RENDERING_MODE_NATURAL &&
            mode != DWRITE_RENDERING_MODE_GDI_NATURAL)
        {
            /*
            ** DWRITE_RENDERING_MODE_GDI_CLASSIC
            */

            mode =
                ((_ui_font_quality () == UI_FONT_QUALITY_ALIASED) ?
                 DWRITE_RENDERING_MODE_ALIASED :
                 DWRITE_RENDERING_MODE_GDI_CLASSIC);
        }

        IDWriteRenderingParams_Release (params);

        IDWriteFactory_CreateCustomRenderingParams (
            factory,
            gamma,
            enhanced_contrast,
            cleartype_level,
            pixel_geometry,
            mode,
            &params);
    }

    if (renderer == (IDWriteTextRenderer *) NULL)
    {
        _uint_string_dwc_renderer_Ctor (factory, &renderer);
    }

    if (gdi != (IDWriteGdiInterop *) NULL &&
        params != (IDWriteRenderingParams *) NULL &&
        renderer != (IDWriteTextRenderer *) NULL &&
        IDWriteGdiInterop_CreateBitmapRenderTarget (
            gdi, device_context, width, height,
            &(bitmap_context.bitmap)) == S_OK &&
        IDWriteBitmapRenderTarget_QueryInterface (
            bitmap_context.bitmap, &IID_IDWriteBitmapRenderTarget1,
            (void **) &(bitmap_context.bitmap1)) == S_OK &&
        bitmap_context.bitmap1 != (IDWriteBitmapRenderTarget1 *) NULL)
    {
        if (IDWriteBitmapRenderTarget_QueryInterface (
            bitmap_context.bitmap, &IID_IDWriteBitmapRenderTarget3,
            (void **) &(bitmap_context.bitmap3)) != S_OK)
        {
            bitmap_context.bitmap3 = (IDWriteBitmapRenderTarget3 *) NULL;
        }

        bitmap_context.params = params;

        bitmap_context.color = GetTextColor (device_context);

        ZERO_OUT_STRUCT (bitmap_context.underline);

        IDWriteBitmapRenderTarget_SetPixelsPerDip (
            bitmap_context.bitmap,
            1.0f);

        IDWriteBitmapRenderTarget1_SetTextAntialiasMode (
            bitmap_context.bitmap1,
            DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        if ((bitmap_device_context =
             IDWriteBitmapRenderTarget_GetMemoryDC (bitmap_context.bitmap))
                != (HDC) NULL &&
            PatBlt (bitmap_device_context, 0, 0, width, height, BLACKNESS) &&
            IDWriteTextLayout_Draw (
                layout, &bitmap_context, (IDWriteTextRenderer *) renderer,
                (FLOAT) (rect->left - x), (FLOAT) (rect->top - y)) == S_OK)
        {
            AlphaBlend (
                device_context,
                x,
                y,
                width,
                height,
                bitmap_device_context,
                0,
                0,
                width,
                height,
                blend_function);

            status = UI_SUCCESS;
        }

        if (bitmap_context.bitmap3 != (IDWriteBitmapRenderTarget3 *) NULL)
        {
            IDWriteBitmapRenderTarget_Release (
                (IDWriteBitmapRenderTarget *) bitmap_context.bitmap3);
        }

        IDWriteBitmapRenderTarget_Release (
            (IDWriteBitmapRenderTarget *) bitmap_context.bitmap1);

        IDWriteBitmapRenderTarget_Release (bitmap_context.bitmap);

        if (bitmap_context.underline.width > 0)
        {
            _ui_gfx_fill_rects (
                context,
                NULL,
                &(bitmap_context.underline),
                1);
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_QueryInterface
| Purpose:  IUnknown::QueryInterface
| Input:    renderer        - IDWriteTextRenderer object
|           riid            - IID to query
| Output:   object          - Object if query successful
| Return:   S_OK if everything went well, otherwise E_NOINTERFACE
\*--------------------------------------------------------------------------*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_QueryInterface (IUnknown *renderer,
                                                                         REFIID riid,
                                                                         void **object)
{
    static CONST IID    IID_IDWriteTextRenderer =
        { 0xef8a8135, 0x5cc6, 0x45fe, { 0x88, 0x25, 0xc5, 0xa0, 0x72, 0x4e, 0xb8, 0x19 } };

    static CONST IID    IID_IDWritePixelSnapping =
        { 0xeaf3a2da, 0xecf4, 0x4d24, { 0xb6, 0x44, 0xb3, 0x4f, 0x68, 0x42, 0x02, 0x4b } };

    HRESULT             status = E_NOINTERFACE;

    if (IsEqualIID (riid, &IID_IDWriteTextRenderer) ||
        IsEqualIID (riid, &IID_IDWritePixelSnapping) ||
        IsEqualIID (riid, &IID_IUnknown))
    {
        if (object != (void **) NULL)
        {
            *object = renderer;
        }

        IDWriteTextRenderer_AddRef ((IDWriteTextRenderer *) renderer);

        status = S_OK;
    }
    else if (object != (void **) NULL)
    {
        *object = NULL;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_AddRef
| Purpose:  IUnknown::AddRef
| Input:    renderer        - IDWriteTextRenderer object
| Output:
| Return:   The incremented reference count of the object
\*--------------------------------------------------------------------------*/
static ULONG CALLBACK _uint_string_IDWriteTextRenderer_AddRef (IUnknown *renderer)
{
    dw_string_renderer_t   *object = (dw_string_renderer_t *) renderer;

    return (InterlockedIncrement ((long *) &(object->refCount)));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_Release
| Purpose:  IUnknown::Release
| Input:    renderer        - IDWriteTextRenderer object
| Output:
| Return:   The decremented reference count of the object
\*--------------------------------------------------------------------------*/
static ULONG CALLBACK _uint_string_IDWriteTextRenderer_Release (IUnknown *renderer)
{
    dw_string_renderer_t   *object = (dw_string_renderer_t *) renderer;
    ULONG                   refCount =
        InterlockedDecrement ((long *) &(object->refCount));

    if (refCount == 0ul)
    {
        relmem (&renderer);
    }

    return (refCount);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_IsPixelSnappingDisabled
| Purpose:  IDWritePixelSnapping::IsPixelSnappingDisabled
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
| Output:   disabled        - TRUE to disable pixel snapping
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_IsPixelSnappingDisabled (IDWritePixelSnapping *renderer,
                                                                                  void *context,
                                                                                  BOOL *disabled)
{
    *disabled = FALSE;

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_GetCurrentTransform
| Purpose:  IDWritePixelSnapping::GetCurrentTransform
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
| Output:   transform       - the current transform
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_GetCurrentTransform (IDWritePixelSnapping *renderer,
                                                                              void *context,
                                                                              DWRITE_MATRIX *transform)
{
    DWRITE_MATRIX   identity = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

    *transform = identity;

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_GetPixelsPerDip
| Purpose:  IDWritePixelSnapping::GetPixelsPerDip
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
| Output:   pixels          - the pixels per DIP
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_GetPixelsPerDip (IDWritePixelSnapping *renderer,
                                                                          void *context,
                                                                          FLOAT *pixels)
{
    *pixels = 1.0f;

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_DrawGlyphRun
| Purpose:  IDWriteTextRenderer::DrawGlyphRun
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           mode            - the measuring mode
|           glyphs          - the glyph run
|           descriptions    - the glyph descriptions
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawGlyphRun (IDWriteTextRenderer *renderer,
                                                                       void *context,
                                                                       FLOAT x,
                                                                       FLOAT y,
                                                                       DWRITE_MEASURING_MODE mode,
                                                                       const DWRITE_GLYPH_RUN *glyphs,
                                                                       const DWRITE_GLYPH_RUN_DESCRIPTION *descriptions,
                                                                       IUnknown *effect)
{
    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_DrawUnderline
| Purpose:  IDWriteTextRenderer::DrawUnderline
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           underline       - the underline data
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawUnderline (IDWriteTextRenderer *renderer,
                                                                        void *context,
                                                                        FLOAT x,
                                                                        FLOAT y,
                                                                        const DWRITE_UNDERLINE *underline,
                                                                        IUnknown *effect)
{
    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_DrawStrikethrough
| Purpose:  IDWriteTextRenderer::DrawStrikethrough
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           strikethrough   - the strike-through data
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawStrikethrough (IDWriteTextRenderer *renderer,
                                                                            void *context,
                                                                            FLOAT x,
                                                                            FLOAT y,
                                                                            const DWRITE_STRIKETHROUGH *strikethrough,
                                                                            IUnknown *effect)
{
    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_DrawInlineObject
| Purpose:  IDWriteTextRenderer::DrawInlineObject
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           object          - the object
|           sideways        - TRUE to draw sideways
|           right_to_left   - TRUE to draw right-to-left
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_IDWriteTextRenderer_DrawInlineObject (IDWriteTextRenderer *renderer,
                                                                           void *context,
                                                                           FLOAT x,
                                                                           FLOAT y,
                                                                           IDWriteInlineObject *inline_object,
                                                                           BOOL sideways,
                                                                           BOOL right_to_left,
                                                                           IUnknown *effect)
{
    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_IDWriteTextRenderer_Ctor
| Purpose:  IDWriteTextRenderer::IDWriteTextRenderer
| Input:
| Output:   object          - the constructed object
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
static HRESULT _uint_string_IDWriteTextRenderer_Ctor (IDWriteFactory *factory,
                                                      IDWriteTextRenderer **object)
{
    static IDWriteTextRendererVtbl  methods =
    {
        {
            {
                _uint_string_IDWriteTextRenderer_QueryInterface,
                _uint_string_IDWriteTextRenderer_AddRef,
                _uint_string_IDWriteTextRenderer_Release
            },

            _uint_string_IDWriteTextRenderer_IsPixelSnappingDisabled,
            _uint_string_IDWriteTextRenderer_GetCurrentTransform,
            _uint_string_IDWriteTextRenderer_GetPixelsPerDip
        },

        _uint_string_IDWriteTextRenderer_DrawGlyphRun,
        _uint_string_IDWriteTextRenderer_DrawUnderline,
        _uint_string_IDWriteTextRenderer_DrawStrikethrough,
        _uint_string_IDWriteTextRenderer_DrawInlineObject
    };

    *object = (IDWriteTextRenderer *) GETSTRUCT (dw_string_renderer_t);

    (*object)->lpVtbl = &methods;

    IDWriteTextRenderer_AddRef (*object);

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dw_color_DrawGlyphRun
| Purpose:  IDWriteTextRenderer::DrawGlyphRun
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           mode            - the measuring mode
|           glyphs          - the glyph run
|           descriptions    - the glyph descriptions
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_dw_color_DrawGlyphRun (IDWriteTextRenderer *renderer,
                                                            void *v_context,
                                                            FLOAT x,
                                                            FLOAT y,
                                                            DWRITE_MEASURING_MODE mode,
                                                            const DWRITE_GLYPH_RUN *glyphs,
                                                            const DWRITE_GLYPH_RUN_DESCRIPTION *descriptions,
                                                            IUnknown *effect)
{
    dw_color_t                     *context = (dw_color_t *) v_context;
    IDWriteColorGlyphRunEnumerator *enumerator =
        (IDWriteColorGlyphRunEnumerator *) NULL;

    context->color =
        (IDWriteFactory2_TranslateColorGlyphRun (
             context->factory2, x, y, glyphs, descriptions, mode,
             (DWRITE_MATRIX *) NULL, 0, &enumerator) == S_OK);

    if (enumerator != (IDWriteColorGlyphRunEnumerator *) NULL)
    {
        IDWriteColorGlyphRunEnumerator_Release (enumerator);
    }

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dw_color_Ctor
| Purpose:  IDWriteTextRenderer::IDWriteTextRenderer
| Input:
| Output:   object          - the constructed object
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
static HRESULT _uint_string_dw_color_Ctor (IDWriteFactory *factory,
                                           IDWriteTextRenderer **object)
{
    static IDWriteTextRendererVtbl  methods;
    static int                      initialized = FALSE;
    HRESULT                         result =
        _uint_string_IDWriteTextRenderer_Ctor (factory, object);

    if (result == S_OK)
    {
        if (!initialized)
        {
            initialized = TRUE;

            methods = *((*object)->lpVtbl);

            methods.DrawGlyphRun = _uint_string_dw_color_DrawGlyphRun;
        }

        (*object)->lpVtbl = &methods;
    }

    return (result);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwc_renderer_GetCurrentTransform
| Purpose:  IDWritePixelSnapping::GetCurrentTransform
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
| Output:   transform       - the current transform
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_dwc_renderer_GetCurrentTransform (IDWritePixelSnapping *renderer,
                                                                       void *v_context,
                                                                       DWRITE_MATRIX *transform)
{
    dwc_context_t  *context = (dwc_context_t *) v_context;

    return (IDWriteBitmapRenderTarget_GetCurrentTransform (
                context->bitmap, transform));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwc_renderer_GetPixelsPerDip
| Purpose:  IDWritePixelSnapping::GetPixelsPerDip
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
| Output:   pixels          - the pixels per DIP
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_dwc_renderer_GetPixelsPerDip (IDWritePixelSnapping *renderer,
                                                                   void *v_context,
                                                                   FLOAT *pixels)
{
    dwc_context_t  *context = (dwc_context_t *) v_context;

    *pixels = IDWriteBitmapRenderTarget_GetPixelsPerDip (context->bitmap);

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwc_renderer_DrawGlyphRun
| Purpose:  IDWriteTextRenderer::DrawGlyphRun
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           mode            - the measuring mode
|           glyphs          - the glyph run
|           descriptions    - the glyph descriptions
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_dwc_renderer_DrawGlyphRun (IDWriteTextRenderer *renderer,
                                                                void *v_context,
                                                                FLOAT x,
                                                                FLOAT y,
                                                                DWRITE_MEASURING_MODE mode,
                                                                const DWRITE_GLYPH_RUN *glyphs,
                                                                const DWRITE_GLYPH_RUN_DESCRIPTION *descriptions,
                                                                IUnknown *effect)
{
    dwc_context_t  *context = (dwc_context_t *) v_context;
    HRESULT         status;

    if (context->bitmap3 != (IDWriteBitmapRenderTarget3 *) NULL)
    {
        status =
            IDWriteBitmapRenderTarget3_DrawGlyphRunWithColorSupport (
                context->bitmap3,
                x,
                y,
                mode,
                glyphs,
                context->params,
                context->color,
                0,
                (RECT *) NULL);
    }
    else
    {
        status =
            IDWriteBitmapRenderTarget_DrawGlyphRun (
                context->bitmap,
                x,
                y,
                mode,
                glyphs,
                context->params,
                context->color,
                (RECT *) NULL);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwc_renderer_DrawUnderline
| Purpose:  IDWriteTextRenderer::DrawUnderline
| Input:    renderer        - IDWriteTextRenderer object
|           context         - the caller-defined drawing context
|           x               - x coordinate of the baseline origin of the text
|           y               - y coordinate of the baseline origin of the text
|           underline       - the underline data
|           effect          - drawing effects
| Output:
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
static HRESULT CALLBACK _uint_string_dwc_renderer_DrawUnderline (IDWriteTextRenderer *renderer,
                                                                 void *v_context,
                                                                 FLOAT x,
                                                                 FLOAT y,
                                                                 const DWRITE_UNDERLINE *underline,
                                                                 IUnknown *effect)
{
    dwc_context_t  *context = (dwc_context_t *) v_context;

    if (context->underline.width == 0)
    {
        context->underline.x = (int) x;
        context->underline.y = (int) (y + underline->offset);
        context->underline.width = (int) roundf (underline->width);
        context->underline.height = (int) roundf (underline->thickness);
    }

    return (S_OK);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_string_dwc_renderer_Ctor
| Purpose:  IDWriteTextRenderer::IDWriteTextRenderer
| Input:
| Output:   object          - the constructed object
| Return:   S_OK if everything went well
\*--------------------------------------------------------------------------*/
static HRESULT _uint_string_dwc_renderer_Ctor (IDWriteFactory *factory,
                                               IDWriteTextRenderer **object)
{
    static IDWriteTextRendererVtbl  methods;
    static int                      initialized = FALSE;
    HRESULT                         result =
        _uint_string_IDWriteTextRenderer_Ctor (factory, object);

    if (result == S_OK)
    {
        if (!initialized)
        {
            initialized = TRUE;

            methods = *((*object)->lpVtbl);

            methods.Base.GetCurrentTransform =
                _uint_string_dwc_renderer_GetCurrentTransform;

            methods.Base.GetPixelsPerDip =
                _uint_string_dwc_renderer_GetPixelsPerDip;

            methods.DrawGlyphRun =
                _uint_string_dwc_renderer_DrawGlyphRun;

            methods.DrawUnderline =
                _uint_string_dwc_renderer_DrawUnderline;
        }

        (*object)->lpVtbl = &methods;
    }

    return (result);
}

#endif /* UI_SYSTEM_NT */
