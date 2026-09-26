/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    uint_gfx.c
|
|  Purpose: Windows level graphics functions
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  26-Feb-03         jas         Created
|  11-Mar-03 K-01-02 UK    $$1   Automatic Submission
|  19-Mar-03         jas         Propogate window to memory contexts
|  26-Mar-03 K-01-03 UK    $$2   Automatic Submission
|  09-Apr-03         jas         Added const qualifiers
|  22-Apr-03 K-01-05 UK    $$3   Automatic Submission
|  05-Jun-03         jas         Added sysdep_gfx_mask_area
|  10-Jun-03 K-01-08 UK    $$4   Automatic Submission
|  26-Jun-03         jas         Obsoleted ui_memory.h
|  10-Jul-03 K-01-10 UK    $$5   Automatic Submission
|  06-Nov-03         jas         Use ui_rgb_t for colors
|  11-Nov-03         jas         Added UI_STATIC
|  18-Nov-03 K-01-18 UK    $$6   Automatic Submission
|  11-Feb-04         jas         Added sysdep_gfx_set/get_color_value
|  27-Feb-04         jas         Modified sysdep_gfx_end_paint
|  02-Mar-04 K-01-24 UK    $$7   Automatic Submission
|  04-Mar-04         jas         Added sysdep_gfx_alpha_blend
|  16-Mar-04 K-01-25 UK    $$8   Automatic Submission
|  08-Jun-04         jas         Added sysdep_gfx_fill_gradient_rect
|  08-Jun-04 K-03-03 UK    $$9   Automatic Submission
|  17-Jun-04         jas         Allow UI_BAD_COLOR_RGB for point drawing
|  18-Jun-04         jas         Added _uint_set_DC_brush/pen_color
|  22-Jun-04 K-03-04 UK    $$10  Automatic Submission
|  28-Jul-04         jas         Fixed _uint_gfx_mask_area
|  03-Aug-04 K-03-07 UK    $$11  Automatic Submission
|  25-Nov-04         jas         Fixed copy mode support
|  07-Dec-04 K-03-15 UK    $$12  Automatic Submission
|  21-Feb-05         jas         Added GradientFill
|  01-Mar-05 K-03-20 UK    $$13  Automatic Submission
|  12-Jul-05         jas         Modified sysdep_gfx_alpha_blend
|  13-Jul-05 K-03-28 UK    $$14  Automatic Submission
|  03-Aug-05         AW          Fixed bitmap selection errors
|  09-Aug-05 K-03-30 UK    $$15  Automatic Submission
|  24-Oct-05         jas         Fixed _uint_gfx_set_clip_rects
|  26-Oct-05 K-03-34 UK    $$16  Automatic Submission
|  11-Oct-05         jas         Removed pro_is_win95_running
|  25-Oct-05         jas         Fixed _uint_gfx_set_clip_region
|  31-Jan-06 L-01-01 UK    $$17  Automatic Submission
|  08-Feb-06         jas         Modified sysdep_gfx_create_pen
|  14-Feb-06 L-01-02 UK    $$18  Automatic Submission
|  19-Jun-06         jas         Removed sysdep_gfx_select_brush/pen
|  19-Jun-06         jas         Added sysdep_gfx_get_rect
|  20-Jun-06         jas         Removed sysdep_gfx_select_palette
|  21-Jun-06         jas         Added support for buffered contexts
|  27-Jun-06 L-01-11 UK    $$19  Automatic Submission
|  17-Jul-06         jas         Added _uint_gfx_use_bitmap
|  26-Jul-06 L-01-13 UK    $$20  Automatic Submission
|  25-Aug-06         jas         Added _uint_window_select_clip_rgn
|  12-Sep-06 L-01-16 UK    $$21  Automatic Submission
|  02-Oct-06         jas         Fixed _uint_gfx_set_clip_region
|  10-Oct-06 L-01-18 UK    $$22  Automatic Submission
|  13-Nov-06         jas         Added support for animated contexts
|  14-Nov-06 L-01-20 UK    $$23  Automatic Submission
|  15-Nov-06         jas         Modified _uint_gfx_setup_animation
|  20-Nov-06         jas         Modified _ui_gfx_setup/start_animation
|  23-Nov-06         jas         Modified _ui_gfx_setup_animation
|  23-Nov-06         jas         Added sysdep_gfx_get_buffered_context
|  28-Nov-06 L-01-21 UK    $$24  Automatic Submission
|  30-Nov-06         jas         Fixed _uint_gfx_release_buffered_context
|  04-Dec-06         jas         Added sysdep_gfx_send_animation
|  12-Dec-06         jas         Fixed _uint_gfx_set_clip_rects
|  14-Dec-06 L-01-22 UK    $$25  Automatic Submission
|  11-Oct-07         jas         Fixed _uint_gfx_fill_region
|  23-Oct-07 L-01-40 UK    $$26  Automatic Submission
|  03-Dec-07         jas         Fixed _uint_gfx_set_clip_region
|  29-Jan-08 L-03-01 UK    $$27  Automatic Submission
|  15-Apr-08         jas         Improved sysdep_gfx_fill_gradient_rect
|  22-Apr-08 L-03-07 UK    $$28  Automatic Submission
|  29-Apr-08         jas         Fixed _uint_gfx_draw_focus_rect
|  06-May-08 L-03-08 UK    $$29  Automatic Submission
|  09-May-08         jas         Fixed _uint_gfx_send_animation
|  22-May-08 L-03-09 UK    $$30  Automatic Submission
|  28-May-08         jas         Added sysdep_gfx_set_origin
|  04-Jun-08 L-03-10 UK    $$31  Automatic Submission
|  19-Feb-09         jas         Added sysdep_gfx_get_region_rects
|  25-Feb-09         jas         Added sysdep_gfx_combine_children
|  03-Mar-09 L-03-27 UK    $$32  Automatic Submission
|  17-Mar-09         jas         Added _uint_window_ext_select_clip_rgn
|  31-Mar-09 L-03-29 UK    $$33  Automatic Submission
|  07-Apr-09         jas         Improved _uint_gfx_fill_gradient_rect
|  07-Apr-09         jas         Fixed _uint_gfx_combine_children
|  14-Apr-09 L-03-30 UK    $$34  Automatic Submission
|  14-Apr-09         jas         Added meta-region to ui_gfx_t
|  15-Apr-09         jas         Added _uint_window_get_window
|  17-Apr-09         jas         Handle transparent child windows
|  21-Apr-09         jas         Fixed sysdep_gfx_send_animation
|  28-Apr-09 L-03-31 UK    $$35  Automatic Submission
|  29-Apr-09         jas         Use GFX for animation in virtual-windows
|  12-May-09 L-03-32 UK    $$36  Automatic Submission
|  20-May-09         jas         Detect regions in _uint_gfx_combine_children
|  27-May-09 L-03-33 UK    $$37  Automatic Submission
|  01-Jun-09         jas         Fixed _uint_gfx_copy_area
|  21-Jul-09 L-05-01 UK    $$38  Automatic Submission
|  19-Oct-09         jas         Use GFX for virtual-window buffered contexts
|  28-Oct-09 L-05-08 UK    $$39  Automatic Submission
|  29-Oct-09         jas         Added clipping to _ui_gfx_fill_gradient_rect
|  30-Oct-09         jas         Added sysdep_gfx_create_elliptic_region
|  02-Nov-09         jas         Added sysdep_gfx_create_arc_region
|  10-Nov-09 L-05-09 UK    $$40  Automatic Submission
|  11-Nov-09         jas         Added sysdep_gfx_set_layered
|  13-Nov-09         jas         Validate windows upon end of painting
|  13-Nov-09         jas         Added _uint_gfx_set_alpha
|  13-Nov-09         jas         Added sysdep_gfx_set_alpha_value
|  13-Nov-09         jas         Added _uint_gfx_get_layered_alpha
|  16-Nov-09         jas         Added layered support for ellipses and arcs
|  16-Nov-09         jas         Added _uint_gfx_create_bitmap_region
|  16-Nov-09         jas         Fixed _uint_gfx_begin_paint
|  16-Nov-09         jas         Modified _uint_gfx_set_alpha to use a region
|  16-Nov-09         jas         Clip child windows in layered contexts
|  17-Nov-09         jas         Removed inappropriate const qualifiers
|  17-Nov-09         jas         Added _uint_gfx_set_line_alpha
|  18-Nov-09         jas         Use GFX for layered window buffered contexts
|  24-Nov-09 L-05-10 UK    $$41  Automatic Submission
|  26-Nov-09         jas         Fixed conflict between regions and layers
|  26-Nov-09         jas         Fixed _uint_gfx_create_bitmap_region
|  02-Dec-09         jas         Fixed _uint_gfx_begin_paint
|  08-Dec-09 L-05-11 UK    $$42  Automatic Submission
|  20-Apr-10         jas         Fixed _uint_gfx_get_buffered_context
|  27-Apr-10 L-05-21 UK    $$43  Automatic Submission
|  12-May-10         jas         Added _uint_gfx_layered_windows
|  26-May-10 L-05-23 UK    $$44  Automatic Submission
|  29-Jun-10         jas         Added IsChildWindow
|  30-Jun-10         jas         Added window long value convenience macros
|  07-Jul-10 L-05-26 UK    $$45  Automatic Submission
|  04-Nov-10         jas         Added UI_GFX_ARC_FLAG/MASK
|  04-Nov-10         jas         Added sysdep_gfx_set_depth
|  04-Nov-10         jas         Added UI_GFX_MEMORY_FLAG/MASK
|  09-Nov-10 L-05-35 UK    $$46  Automatic Submission
|  11-Nov-10         jas         Added _uint_gfx_set_opaque
|  24-Nov-10 L-05-36 UK    $$47  Automatic Submission
|  30-Nov-10         jas         Added UI_GFX_CHILDREN_FLAG/MASK
|  07-Dec-10 L-05-37 UK    $$48  Automatic Submission
|  08-Dec-10         jas         Improved check for layered context support
|  08-Dec-10 L-05-37+UK    $$49  Automatic Submission
|  06-Jan-11         jas         Fixed compilation warning
|  13-Jan-11         jas         Added _uint_window_child_is_visible
|  18-Jan-11 L-05-40 UK    $$50  Automatic Submission
|  25-Jan-11         jas         Added _ui_getenv
|  01-Feb-11 L-05-41 UK    $$51  Automatic Submission
|  11-Feb-11         jas         Added IsChildWindowVisible
|  15-Feb-11 L-05-42 UK    $$52  Automatic Submission
|  07-Apr-11         jas         Added LCS_DEVICE_CMYK
|  12-Apr-11 L-05-45 UK    $$53  Automatic Submission
|  25-May-11         jas         Added _uint_gfx_needs_opaque
|  20-May-11         jas         Removed overlay planes
|  14-Jun-11 P-10-01 UK    $$54  Automatic Submission
|  25-Nov-11         jas         Fixed _uint_gfx_fill_gradient_rect
|  29-Nov-11         jas         Fixed _uint_gfx_release_layered_context
|  29-Nov-11 P-10-13 UK    $$55  Automatic Submission
|  01-Dec-11         jas         Fixed unclipped _uint_gfx_get_context
|  13-Dec-11 P-10-14 UK    $$56  Automatic Submission
|  23-Jan-12         jas         Fixed _uint_gfx_get_layered_buffer
|  24-Jan-12 P-10-16 UK    $$57  Automatic Submission
|  13-Mar-12         jas         Fixed compilation warnings
|  20-Mar-12 P-20-01 UK    $$58  Automatic Submission
|  11-Apr-12         jas         Added sysdep_gfx_create_memory_context
|  11-Apr-12         jas         Added sysdep_gfx_get_bitmap
|  18-Apr-12 P-20-03 UK    $$59  Automatic Submission
|  09-May-12         jas         Added UI_GFX_PIE/POLYGON_FLAG/MASK
|  16-May-12 P-20-05 UK    $$60  Automatic Submission
|  28-May-12         jas         Fixed UI_DOTTED
|  30-May-12 P-20-06 UK    $$61  Automatic Submission
|  30-May-12         jas         Fixed line end cap problems
|  12-Jun-12 P-20-07 UK    $$62  Automatic Submission
|  03-Dec-12         jas         Modified sysdep_gfx_set_origin
|  10-Dec-12         jas         Fixed _uint_gfx_get_layered_context
|  11-Dec-12 P-20-19 UK    $$63  Automatic Submission
|  14-Dec-12         jas         Fixed _uint_gfx_use_context
|  20-Dec-12         jas         Added _uint_gfx_set_layered_region
|  10-Jan-13 P-20-21 UK    $$64  Automatic Submission
|  18-Jan-13         jas         Added sysdep_gfx_offset_region
|  23-Jan-13 P-20-22 UK    $$65  Automatic Submission
|  13-Feb-13         jas         Fixed 32-bit polyline and polygon drawing
|  14-Feb-13         jas         Fixed 32-bit rectangle drawing
|  15-Feb-13         jas         Added origin to ui_gfx_t
|  19-Feb-13 P-20-24 UK    $$66  Automatic Submission
|  06-Jun-13         jas         Improved _uint_gfx_release_layered_context
|  07-Jun-13         jas         Added ScreenBlt
|  18-Jun-13 P-20-32 UK    $$67  Automatic Submission
|  03-Jan-14         jas         Fixed _uint_gfx_set_layered_region
|  08-Jan-14 P-20-45 UK    $$68  Automatic Submission
|  17-Jan-14         jas         Fixed problems with SelectClipRgn
|  19-Feb-14 P-20-48 UK    $$69  Automatic Submission
|  27-Jan-15         jas         Clip contexts to their region
|  29-Jan-15         jas         Added UINT_VERSION_WINDOWS_... macros
|  03-Feb-15 P-30-01 UK    $$70  Automatic Submission
|  05-Feb-15         jas         Added UI_GFX_CLIPPED_FLAG/MASK
|  17-Feb-15 P-30-02 UK    $$71  Automatic Submission
|  22-Sep-15         jas         Added IsWindowsXXXOrGreater
|  28-Sep-15         jas         Added UI_COMPONENT_SYSTEM_NT
|  30-Sep-15 P-30-17 UK    $$72  Automatic Submission
|  03-Dec-15         jas         Fixed compilation warnings
|  09-Dec-15 P-30-22 UK    $$73  Automatic Submission
|  02-Mar-16         jas         Do not allow asynchronous drawing when dirty
|  17-Mar-16 P-30-28 UK    $$74  Automatic Submission
|  06-May-16         jas         Fixed _uint_gfx_release_layered_context
|  12-May-16 P-30-32 UK    $$75  Automatic Submission
|  03-Jun-16         jas         Added more UI_COMPONENT_SYSTEM_NT
|  07-Jun-16 P-30-33 UK    $$76  Automatic Submission
|  13-Jun-16         jas         Added convenience macros
|  21-Jun-16 P-30-34 UK    $$77  Automatic Submission
|  06-Jul-16         jas         Added sysdep_gfx_fade_rect
|  19-Jul-16 P-30-36 UK    $$78  Automatic Submission
|  28-Nov-16         jas         Added _ui_gfx_intersect_region
|  20-Dec-16 P-30-42 UK    $$79  Automatic Submission
|  20-Dec-16         jas         Removed support for legacy graphics modes
|  17-Jan-17         jas         Removed unnecessary code
|  14-Mar-17 P-50-01 UK    $$80  Automatic Submission
|  27-Apr-17         jas         Added _ui_gfx_includes_children
|  03-May-17 P-50-07 UK    $$81  Automatic Submission
|  01-Aug-17         jas         Removed sysdep_gfx_exclude_clip_rect
|  01-Aug-17         jas         Added sysdep_gfx_copy_gradient_area
|  10-Aug-17 P-50-22 UK    $$82  Automatic Submission
|  13-Nov-17         jas         Added GetWindowThreadId
|  15-Nov-17 P-50-36 UK    $$83  Automatic Submission
|  07-Jun-18         jas         Removed sysdep_gfx_set_fg_color
|  07-Jun-18         jas         Fixed _uint_gfx_fill_gradient_rect
|  18-Jun-18 P-60-07 UK    $$84  Automatic Submission
|  10-Sep-18         jas         Preserve alpha in _uint_gfx_set_layered
|  20-Sep-18 P-60-18 UK    $$85  Automatic Submission
|  24-Oct-18         jas         Removed uint_theme.h
|  30-Oct-18 P-60-23 UK    $$86  Automatic Submission
|  12-Sep-19         jas         Added UI_GFX_HAS_FLAG
|  13-Sep-19         jas         Added IsWindowTransparentU
|  13-Sep-19         jas         Removed unnecessary code
|  18-Sep-19 P-70-27 UK    $$87  Automatic Submission
|  19-Sep-19         jas         Allow memory contexts created from nothing
|  24-Sep-19 P-70-28 UK    $$88  Automatic Submission
|  03-Oct-19         jas         Added sysdep_gfx_is_empty_region
|  14-Oct-19         jas         Allow memory contexts created from a window
|  16-Oct-19 P-70-30 UK    $$89  Automatic Submission
|  02-Dec-19         jas         Removed obsolete flag from create/destroy
|  03-Dec-19 P-70-36 UK    $$90  Automatic Submission
|  21-Oct-20         jas         Added sysdep_gfx_is_equal_region
|  27-Oct-20 P-80-26 UK    $$91  Automatic Submission
|  27-Oct-20         jas         Renamed GFX region interfaces
|  05-Nov-20 P-80-27 UK    $$92  Automatic Submission
|  10-Nov-20         jas         Removed unused code
|  11-Nov-20 P-80-28 UK    $$93  Automatic Submission
|  27-Jul-21         jas         Added support for maximized layered windows
|  28-Jul-21         jas         Removed UI_STYLE_GLASS
|  03-Aug-21 P-90-20 UK    $$94  Automatic Submission
|  06-Dec-21         jas         Disabled layered windows in RemoteApp
|  08-Dec-21 P-90-37 UK    $$95  Automatic Submission
|  01-Apr-22         jas         Fixed 32bpp inverting
|  05-Apr-22 Q-10-06 UK    $$96  Automatic Submission
|  21-Jun-22         jas         Added sysdep_gfx_region_create_from_image
|  21-Jun-22         jas         Added sysdep_gfx_set_region
|  30-Jun-22 Q-10-17 UK    $$97  Automatic Submission
|  27-Sep-22         jas         Allow creation of 32bpp memory contexts
|  28-Sep-22 Q-10-29 UK    $$98  Automatic Submission
|  25-May-23         jas         Use layered windows on Windows 11 RemoteApp
|  31-May-23 Q-11-14 UK    $$99  Automatic Submission
|  11-Jan-24         jas         Removed unnecessary code
|  17-Jan-24 Q-11-47 UK    $$100 Automatic Submission
|  20-May-24         jas         Added sysdep_gfx_set_accelerated
|  20-May-24         jas         Added _uint_d2d_renderer_create/destroy
|  22-May-24 Q-12-13 UK    $$101 Automatic Submission
|  21-Aug-24         jas         Added _uint_gfx_d2d_region
|  04-Sep-24 Q-12-28 UK    $$102 Automatic Submission
|  03-Jul-25         jas         Fixed _uint_gfx_fill_gradient_rect
|  15-Jul-25 Q-13-17 UK    $$103 Automatic Submission
|  08-Sep-25         jas         Check return value of BeginPaint
|  16-Sep-25 Q-13-26 UK    $$104 Automatic Submission
|  09-Oct-25         jas         Removed sysdep_gfx_set_clip_rects
|  10-Oct-25         jas         Removed stretch from sysdep_gfx_alpha_blend
|  10-Oct-25         jas         Added ID2D1BitmapRenderTarget
|  15-Oct-25 Q-13-30 UK    $$105 Automatic Submission
|  17-Oct-25         jas         Added _uint_d2d_renderer_set_origin
|  22-Oct-25 Q-13-31 UK    $$106 Automatic Submission
|  30-Oct-25         jas         Added _uint_d2d_renderer_region
|  04-Nov-25 Q-13-33 UK    $$107 Automatic Submission
|  06-Feb-26         jas         Fixed _uint_gfx_create_bitmap32
|  06-Feb-26 Q-13-47 UK    $$108 Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#if !defined (lint) && defined (SHOW_SCCS_ID)
static char uint_gfx_c_id [] = "@(#) uint_gfx.c 4147.2@(#)";
#endif

#include <const.h>
#include <ptc_xarray.h>
#include <sysmath.h>

#include <ui.h>
#include <uip.h>
#include <ui_colors.h>
#include <ui_image.h>
#include <ui_kernel.h>
#include <ui_monitors.h>
#include <ui_string.h>
#include <ui_utils.h>
#include <ui_gfx.h>
#include <ui_gfxp.h>

#ifdef UI_SYSTEM_NT

#include <uint.h>
#include <uint_d2d1.h>


/*
** The layered context cache
*/

static ui_gfx_t _uint_gfx_layered_context_cache = { 0 };
static HWND _uint_gfx_layered_window_cache = (HWND) NULL;
static HRGN _uint_gfx_layered_region_cache = (HRGN) NULL;


/*
** 32-bit DIB data
*/

typedef struct
{
    HBITMAP         bitmap;
    unsigned char  *data;
    int             bpr;

} nt_dib32_t;


/*
** Data to be used with LineDDA to set the alpha channel of a line
*/

typedef struct
{
    nt_dib32_t      dib;
    HRGN            clip_region;
    unsigned char   alpha;

} nt_linedda_data_t;


/*
** Menu selection fader window data structure
*/

typedef struct
{
    POINT       position;
    SIZE        size;
    HBITMAP     bitmap;
    int         alpha;
    int         rate;
    int         delta;
    HWND        window;
    HANDLE      event;
    UINT_PTR    timer;

} nt_fade_t;


/*
** Private functions
*/

static void _uint_gfx_use_palette (
    ui_gfx_t   *context,
    void       *palette
);

static void *_uint_gfx_use_bitmap (
    ui_gfx_t   *context,
    void       *bitmap
);

static void *_uint_gfx_use_brush (
    ui_gfx_t   *context,
    void      **brush
);

static void *_uint_gfx_use_pen (
    ui_gfx_t   *context,
    void      **pen
);

static HRGN _uint_gfx_region_create_from_bitmap (
    HBITMAP             bitmap,
    const ui_rect_t    *rect
);

static HBITMAP _uint_gfx_create_bitmap32 (
    HDC             dc,
    int             width,
    int             height,
    unsigned char **data,
    int            *bpr
);

static int _uint_gfx_destroy_bitmap32 (
    HBITMAP bitmap
);

static int _uint_gfx_cache_bitmap32 (
    nt_dib32_t *dib
);

static void _uint_gfx_set_alpha (
    ui_gfx_t       *context,
    unsigned char   alpha,
    void           *region
);

static void _uint_gfx_set_line_alpha (
    ui_gfx_t       *context,
    unsigned char   alpha,
    int             start_x,
    int             start_y,
    int             end_x,
    int             end_y
);

static VOID CALLBACK _uint_gfx_set_line_alpha_proc (
    int     x,
    int     y,
    LPARAM  dda_data
);

static unsigned int _uint_gfx_get_layered_alpha (
    HWND            window,
    unsigned int    alpha
);

static void *_uint_gfx_get_layered_buffer (
    HWND    window,
    int     width,
    int     height
);

static int _uint_gfx_get_layered_lock (
    HWND    window,
    int     lock
);

static int _uint_gfx_get_layered_context (
    ui_gfx_t   *context,
    int         allow_dirty
);

static void *_uint_gfx_get_layered_data (
    HWND    window,
    HWND   *popup
);

static void _uint_gfx_set_layered_region (
    ui_gfx_t   *context,
    HBITMAP     buffer,
    HWND        popup,
    HWND        parent
);

static int _uint_gfx_release_layered_context (
    ui_gfx_t   *context,
    int         width,
    int         height
);

static int _uint_gfx_layered_windows (
    void
);

static DWORD _uint_gfx_fade_thread (
    LPDWORD user_data
);

static LRESULT CALLBACK _uint_gfx_fade_wnd_proc (
    HWND    window,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
);

static int _uint_gfx_d2d_brush (
    ui_gfx_t               *context,
    void                   *object,
    int                     fill,
    ID2D1RenderTarget     **renderer,
    D2D1_POINT_2F          *origin,
    ID2D1SolidColorBrush  **brush,
    FLOAT                  *width
);

static int _uint_gfx_d2d_polygon (
    ui_gfx_t           *context,
    void               *object,
    const ui_point_t   *points,
    int                 count,
    int                 closed,
    int                 fill
);

static int _uint_gfx_d2d_rects (
    ui_gfx_t           *context,
    void               *object,
    const ui_rect_t    *rects,
    int                 count,
    int                 fill
);

static int _uint_gfx_d2d_ellipses (
    ui_gfx_t           *context,
    void               *object,
    const ui_rect_t    *rects,
    int                 count,
    int                 fill
);

static int _uint_gfx_d2d_arcs (
    ui_gfx_t       *context,
    void           *object,
    const ui_arc_t *arcs,
    int             count,
    int             fill
);

static int _uint_gfx_d2d_frame (
    ui_gfx_t           *context,
    void               *object,
    const ui_rect_t    *rect,
    int                 thickness
);

static int _uint_gfx_d2d_region (
    ui_gfx_t   *context,
    void       *object,
    void       *region
);

static int _uint_gfx_d2d_set_origin (
    ui_gfx_t   *context,
    int         x,
    int         y
);

static int _uint_gfx_d2d_set_clip_region (
    ui_gfx_t   *context,
    void       *region,
    int         mode
);

static int _uint_gfx_d2d_alpha_blend (
    ui_gfx_t       *dest_context,
    int             dest_x,
    int             dest_y,
    int             dest_width,
    int             dest_height,
    const ui_gfx_t *src_context,
    int             src_x,
    int             src_y,
    int             src_width,
    int             src_height,
    int             src_alpha,
    int             mode
);

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_context
| Purpose:  Get the context of the given window
| Input:    window              - the window
| Output:   context             - the context
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_get_context (void *window, ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    context->window = window;

    if (!(_uint_gfx_get_layered_context (context, FALSE)))
    {
        context->context =
            (void *)
                (_ui_gfx_includes_children (context) ?
                 GetDCEx (
                     (HWND) window,
                     (HRGN) NULL,
                     (DCX_CLIPSIBLINGS | DCX_CACHE)) :
                 GetDC ((HWND) window));

        context->depth = 24;

        _uint_gfx_use_palette (context, _ui_krn_color_get_colormap ());
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_release_context
| Purpose:  Release the given context
| Input:    context             - the context
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_release_context (ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    _uint_d2d_renderer_destroy ((HDC) context->context);

    SelectBrush ((HDC) context->context, GetStockBrush (NULL_BRUSH));
    SelectPen ((HDC) context->context, GetStockPen (NULL_PEN));
    SetROP2 ((HDC) context->context, R2_COPYPEN);
    SetPolyFillMode ((HDC) context->context, ALTERNATE);
    SetArcDirection ((HDC) context->context, AD_CLOCKWISE);

    if (!(_uint_gfx_release_layered_context (context, 0, 0)))
    {
        ReleaseDC ((HWND) context->window, (HDC) context->context);
    }

    context->window = context->context = NULL;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_use_context
| Purpose:  Use the given context of the given window
| Input:    window              - the window
|           current_context     - the current context of the window
| Output:   context             - the context
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_use_context (void *window, void *current_context,
                                     ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    nt_dib32_t  dib = { (HBITMAP) NULL, (unsigned char *) NULL, 0 };

    context->window = window;
    context->context = current_context;

    if (context->window == _uint_gfx_layered_context_cache.window &&
        context->context == _uint_gfx_layered_context_cache.context)
    {
        *context = _uint_gfx_layered_context_cache;

        UI_GFX_CLEAR_FLAG (context, UI_GFX_CHILDREN_FLAG);
    }
    else if ((dib.bitmap =
              GetCurrentObject ((HDC) current_context, OBJ_BITMAP))
                 != (HBITMAP) NULL &&
             _uint_gfx_cache_bitmap32 (&dib))
    {
        if (_uint_gfx_layered_region_cache != (HRGN) NULL)
        {
            DeleteRgn (_uint_gfx_layered_region_cache);
        }

        _uint_gfx_set_layered_region (
            context,
            dib.bitmap,
            (HWND) NULL,
            (HWND) NULL);

        _uint_gfx_layered_region_cache = (HRGN) context->region;

        context->depth = 32;
    }
    else
    {
        context->depth = 24;

        _uint_gfx_use_palette (context, _ui_krn_color_get_colormap ());
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_create_context
| Purpose:  Create a new context which is compatible with the given context,
|           using the given bitmap
| Input:    context             - the context
|           bitmap              - the bitmap to use
| Output:   new_context         - the new context
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_create_context (const ui_gfx_t *context,
                                        void *bitmap, ui_gfx_t *new_context)
{
#ifdef UI_SYSTEM_NT

    nt_dib32_t  dib = { (HBITMAP) NULL, (unsigned char *) NULL, 0 };
    int         status = UI_ERROR;

    new_context->drawable = bitmap;

    if (new_context->depth >= 32 &&
        (dib.bitmap = (HBITMAP) bitmap) != (HBITMAP) NULL &&
        !(_uint_gfx_cache_bitmap32 (&dib)))
    {
        new_context->depth = 24;
    }

    if ((new_context->context =
         (void *) CreateCompatibleDC ((HDC) context->context)) != NULL)
    {
        _uint_gfx_use_bitmap (new_context, bitmap);

        if (new_context->depth < 32)
        {
            _uint_gfx_use_palette (new_context, _ui_krn_color_get_colormap ());
        }

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_create_memory_context
| Purpose:  Create a new context which is compatible with the given context,
|           using a memory bitmap
| Input:    context             - the context
|           width               - the width of the bitmap
|           height              - the height of the bitmap
| Output:   new_context         - the new context
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_create_memory_context (const ui_gfx_t *context,
                                               int width, int height,
                                               ui_gfx_t *new_context)
{
#ifdef UI_SYSTEM_NT

    static ui_gfx_t null_context = UI_GFX_INIT;
    HDC             dc;
    int             status = UI_ERROR;

    if (width > 0 &&
        height > 0 &&
        UI_GFX_IS_INVALID (context))
    {
        *new_context = null_context;

        if (context == (const ui_gfx_t *) NULL ||
            (new_context->window = context->window) == NULL)
        {
            new_context->window = (void *) _uint_get_app_window ();

            new_context->depth =
                ((context != (const ui_gfx_t *) NULL &&
                  context->depth == 32) ?
                 32 :
                 24);
        }
        else if (_uint_gfx_get_layered_data (
                     (HWND) new_context->window, (HWND *) NULL) != NULL)
        {
            new_context->depth = 32;
        }
        else
        {
            new_context->depth = 24;
        }

        if ((dc = GetDC ((HWND) new_context->window)) != (HDC) NULL)
        {
            if (new_context->depth < 32)
            {
                new_context->drawable =
                    (void *) CreateCompatibleBitmap (dc, width, height);
            }
            else
            {
                new_context->drawable =
                    (void *)
                        _uint_gfx_create_bitmap32 (
                            dc,
                            width,
                            height,
                            (unsigned char **) NULL,
                            (int *) NULL);
            }

            if (new_context->drawable != NULL)
            {
                new_context->context = (void *) CreateCompatibleDC (dc);

                _uint_gfx_use_palette (
                    new_context,
                    _ui_krn_color_get_colormap ());

                _uint_gfx_use_bitmap (new_context, new_context->drawable);

                status = UI_SUCCESS;
            }
            else
            {
                new_context->context = NULL;
            }

            ReleaseDC ((HWND) new_context->window, dc);
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_destroy_context
| Purpose:  Destroy the given context
| Input:    context             - the context
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_destroy_context (ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    _uint_d2d_renderer_destroy ((HDC) context->context);

    _uint_gfx_use_bitmap (context, NULL);

    if (UI_GFX_HAS_FLAG (context, UI_GFX_CLIPPED_FLAG))
    {
        UI_GFX_CLEAR_FLAG (context, UI_GFX_CLIPPED_FLAG);

        SelectClipRgn ((HDC) context->context, (HRGN) NULL);
    }

    DeleteDC ((HDC) context->context);

    context->window = context->context = context->drawable = NULL;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_begin_paint
| Purpose:  Begin painting in the given window
| Input:    window              - the window
|           data                - reserved
| Output:   context             - the context to use for drawing
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_begin_paint (void *window, void *data,
                                     ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    int status = UI_SUCCESS;

    context->window = window;

    if ((context->context =
         (void *) BeginPaint ((HWND) window, (PAINTSTRUCT *) data)) == NULL ||
        context->context == (void *) 1)
    {
        status = UI_ERROR;
    }
    else if (_uint_gfx_get_layered_context (context, TRUE))
    {
        EndPaint ((HWND) window, (PAINTSTRUCT *) data);

        if (data != NULL)
        {
            ((PAINTSTRUCT *) data)->hdc = (HDC) context->context;

            GetClientRect (
                (HWND) window,
                &(((PAINTSTRUCT *) data)->rcPaint));
        }

        context->display = NULL;
    }
    else
    {
        context->display = data;
        context->depth = 24;

        _uint_gfx_use_palette (context, _ui_krn_color_get_colormap ());
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_end_paint
| Purpose:  End painting in the given context
| Input:    context             - the context used for drawing
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_end_paint (ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    HWND    hwnd = (HWND) context->window;
    int     status = _uint_gfx_release_context (context);

    if (context->display != NULL)
    {
        if (context->display != NULL &&
            ((PAINTSTRUCT *) context->display)->hdc != (HDC) NULL)
        {
            SelectClipRgn (
                ((PAINTSTRUCT *) context->display)->hdc,
                (HRGN) NULL);
        }

        EndPaint (hwnd, (PAINTSTRUCT *) context->display);

        context->display = NULL;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_rect
| Purpose:  Determine the bounding rectangle of the window
| Input:    window              - the window
| Output:   rect                - the bounding rectangle of the window
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_get_rect (void *window, ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    RECT    nt_rect;

    GetClientRect ((HWND) window, &nt_rect);

    rect->x = nt_rect.left;
    rect->y = nt_rect.top;
    rect->width = (nt_rect.right - nt_rect.left);
    rect->height = (nt_rect.bottom - nt_rect.top);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_bitmap
| Purpose:  Retrieve the bitmap of the context
| Input:    context             - the context
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_get_bitmap (ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    return (UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_origin
| Purpose:  Set the drawing origin of the context
| Input:    context             - the context
|           x                   - the x co-ordinate of the origin
|           y                   - the y co-ordinate of the origin
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_origin (ui_gfx_t *context, int x, int y)
{
#ifdef UI_SYSTEM_NT

    POINT   origin = { 0 };
    RECT    rect;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_set_origin (context, x, y) != UI_SUCCESS)
    {
        _uint_d2d_renderer_destroy ((HDC) context->context);
    }

    if (context->depth >= 32 &&
        context->drawable == NULL &&
        IsMaximized ((HWND) context->window) &&
        IsWindowLayered ((HWND) context->window))
    {
        MapWindowPoint ((HWND) context->window, (HWND) NULL, &origin);

        GetWindowRect ((HWND) context->window, &rect);

        origin.x = (rect.left - origin.x);
        origin.y = (rect.top - origin.y);
    }

    origin.x += x;
    origin.y += y;

    SetWindowOrgEx (
        (HDC) context->context,
        origin.x,
        origin.y,
        (POINT *) NULL);

    if (context->region != NULL)
    {
        OffsetRgn (
            (HRGN) context->region,
            (context->origin.x + origin.x),
            (context->origin.y + origin.y));
    }

    context->origin.x = -x;
    context->origin.y = -y;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_copy_area
| Purpose:  Copy the given area from the given context to the other context
| Input:    dest_context        - the destination context
|           dest_x              - the destination rectangle left position
|           dest_y              - the destination rectangle top position
|           width               - the rectangle width
|           height              - the rectangle height
|           src_context         - the source context
|           src_x               - the source rectangle left position
|           src_y               - the source rectangle top position
|           mode                - the copy mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_copy_area (ui_gfx_t *dest_context, int dest_x,
                                   int dest_y, int width, int height,
                                   const ui_gfx_t *src_context, int src_x,
                                   int src_y, int mode)
{
#ifdef UI_SYSTEM_NT

    DWORD       rop2;
    HRGN        clip_region;
    ui_rect_t   rect;
    void       *meta_region;

    if (!(_ui_gfx_is_accelerated (src_context)) ||
        !(_ui_gfx_is_accelerated (dest_context)) ||
        _uint_gfx_d2d_alpha_blend (
            dest_context, dest_x, dest_y, width, height, src_context, src_x,
            src_y, width, height, 255, mode) != UI_SUCCESS)
    {
        _uint_d2d_renderer_destroy ((HDC) src_context->context);

        if (dest_context->context != src_context->context)
        {
            _uint_d2d_renderer_destroy ((HDC) dest_context->context);
        }

        switch (mode)
        {
            case UI_NOT:
            {
                rop2 = NOTSRCCOPY;
                break;
            }

            case UI_AND:
            {
                rop2 = SRCAND;
                break;
            }

            case UI_OR:
            {
                rop2 = SRCPAINT;
                break;
            }

            case UI_XOR:
            {
                rop2 = SRCINVERT;
                break;
            }

            default:
            {
                rop2 = SRCCOPY;
            }
        }

        if (src_context->region != NULL)
        {
            clip_region = CreateRectRgn (0, 0, 0, 0);

            if (GetClipRgn ((HDC) dest_context->context, clip_region) != 1)
            {
                DeleteRgn (clip_region);
                clip_region = (HRGN) NULL;
            }

            if (dest_x != src_x ||
                dest_y != src_y)
            {
                meta_region = (void *) CreateRectRgn (0, 0, 0, 0);

                CopyRgn ((HRGN) meta_region, (HRGN) src_context->region);

                OffsetRgn ((HRGN) meta_region, (dest_x - src_x), (dest_y - src_y));
            }
            else
            {
                meta_region = src_context->region;
            }

            _uint_gfx_set_clip_region (dest_context, meta_region, UI_AND);

            if (meta_region != src_context->region)
            {
                DeleteRgn ((HRGN) meta_region);
            }
        }

        if (src_context->drawable == NULL &&
            rop2 == SRCCOPY)
        {
            ScreenBlt (
                (HDC) dest_context->context,
                dest_x,
                dest_y,
                width,
                height,
                (HDC) src_context->context,
                src_x,
                src_y,
                rop2);
        }
        else
        {
            BitBlt (
                (HDC) dest_context->context,
                dest_x,
                dest_y,
                width,
                height,
                (HDC) src_context->context,
                src_x,
                src_y,
                rop2);
        }

        if (dest_context->depth >= 32 &&
            src_context->depth < 32)
        {
            rect.x = src_x;
            rect.y = src_y;
            rect.width = width;
            rect.height = height;

            meta_region = _ui_gfx_region_create_from_rect (&rect);

            if (src_context->region != NULL)
            {
                _ui_gfx_region_intersect (meta_region, src_context->region);
            }

            if (dest_x != src_x ||
                dest_y != src_y)
            {
                OffsetRgn (
                    (HRGN) meta_region,
                    (dest_x - src_x),
                    (dest_y - src_y));
            }

            _uint_gfx_set_alpha (dest_context, 255, meta_region);

            _ui_gfx_region_destroy (&meta_region);
        }

        if (src_context->region != NULL)
        {
            SelectClipRgn ((HDC) dest_context->context, clip_region);

            if (clip_region != (HRGN) NULL)
            {
                DeleteRgn (clip_region);
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_stretch_area
| Purpose:  Copy the given area from the given context to the other context,
|           resizing or mirroring as necessary
| Input:    dest_context        - the destination context
|           dest_x              - the destination rectangle left position
|           dest_y              - the destination rectangle top position
|           dest_width          - the destination rectangle width
|           dest_height         - the destination rectangle height
|           src_context         - the source context
|           src_x               - the source rectangle left position
|           src_y               - the source rectangle top position
|           src_width           - the source rectangle width
|           src_height          - the source rectangle height
|           mode                - the copy mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_stretch_area (ui_gfx_t *dest_context, int dest_x,
                                      int dest_y, int dest_width,
                                      int dest_height,
                                      const ui_gfx_t *src_context, int src_x,
                                      int src_y, int src_width,
                                      int src_height, int mode)
{
#ifdef UI_SYSTEM_NT

    DWORD   rop2;
    HRGN    clip_region, meta_region;

    if (!(_ui_gfx_is_accelerated (src_context)) ||
        !(_ui_gfx_is_accelerated (dest_context)) ||
        _uint_gfx_d2d_alpha_blend (
            dest_context, dest_x, dest_y, dest_width, dest_height,
            src_context, src_x, src_y, src_width, src_height, 255,
            mode) != UI_SUCCESS)
    {
        _uint_d2d_renderer_destroy ((HDC) src_context->context);

        if (dest_context->context != src_context->context)
        {
            _uint_d2d_renderer_destroy ((HDC) dest_context->context);
        }

        switch (mode)
        {
            case UI_NOT:
            {
                rop2 = NOTSRCCOPY;
                break;
            }

            case UI_AND:
            {
                rop2 = SRCAND;
                break;
            }

            case UI_OR:
            {
                rop2 = SRCPAINT;
                break;
            }

            case UI_XOR:
            {
                rop2 = SRCINVERT;
                break;
            }

            default:
            {
                rop2 = SRCCOPY;
            }
        }

        SetStretchBltMode ((HDC) dest_context->context, HALFTONE);

        if (src_context->region != NULL)
        {
            clip_region = CreateRectRgn (0, 0, 0, 0);

            if (GetClipRgn ((HDC) dest_context->context, clip_region) != 1)
            {
                DeleteRgn (clip_region);
                clip_region = (HRGN) NULL;
            }

            if (dest_x != src_x ||
                dest_y != src_y)
            {
                meta_region = CreateRectRgn (0, 0, 0, 0);

                CopyRgn (meta_region, (HRGN) src_context->region);

                OffsetRgn (meta_region, (dest_x - src_x), (dest_y - src_y));
            }
            else
            {
                meta_region = (HRGN) src_context->region;
            }

            _uint_gfx_set_clip_region (
                dest_context,
                (void *) meta_region,
                UI_AND);

            if (meta_region != (HRGN) src_context->region)
            {
                DeleteRgn (meta_region);
            }
        }

        StretchBlt (
            (HDC) dest_context->context,
            dest_x,
            dest_y,
            dest_width,
            dest_height,
            (HDC) src_context->context,
            src_x,
            src_y,
            src_width,
            src_height,
            rop2);

        if (dest_context->depth >= 32 &&
            src_context->depth < 32)
        {
            _ui_gfx_fill_rect (
                dest_context,
                (void *) 255,
                dest_x,
                dest_y,
                dest_width,
                dest_height);
        }

        if (src_context->region != NULL)
        {
            SelectClipRgn ((HDC) dest_context->context, clip_region);

            if (clip_region != (HRGN) NULL)
            {
                DeleteRgn (clip_region);
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_mask_area
| Purpose:  Copy the given area from the given context to the other context
|           using the given monochrome mask bitmap
| Input:    dest_context        - the destination context
|           dest_x              - the destination rectangle left position
|           dest_y              - the destination rectangle top position
|           width               - the rectangle width
|           height              - the rectangle height
|           src_context         - the source context
|           src_x               - the source rectangle left position
|           src_y               - the source rectangle top position
|           mask                - the monochrome bitmap to use as a mask
|           mask_x              - the left offset into the mask bitmap
|           mask_y              - the top offset into the mask bitmap
|           mode                - the copy mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_mask_area (ui_gfx_t *dest_context, int dest_x,
                                   int dest_y, int width, int height,
                                   const ui_gfx_t *src_context, int src_x,
                                   int src_y, void *mask, int mask_x,
                                   int mask_y, int mode)
{
#ifdef UI_SYSTEM_NT

    COLORREF    sys_fg, sys_bg;
    HDC         mask_context;
    HBITMAP     src_bitmap, sys_bitmap;
    BITMAP      src_bitmap_data;
    DWORD       rop2;
    ui_rect_t   rect;
    void       *meta_region;

    _uint_d2d_renderer_destroy ((HDC) src_context->context);

    if (dest_context->context != src_context->context)
    {
        _uint_d2d_renderer_destroy ((HDC) dest_context->context);
    }

    switch (mode)
    {
        case UI_NOT:
        {
            rop2 = NOTSRCCOPY;
            break;
        }

        case UI_AND:
        {
            rop2 = SRCAND;
            break;
        }

        case UI_OR:
        {
            rop2 = SRCPAINT;
            break;
        }

        case UI_XOR:
        {
            rop2 = SRCINVERT;
            break;
        }

        default:
        {
            rop2 = SRCCOPY;
        }
    }

    if (!(MaskBlt (
              (HDC) dest_context->context, dest_x, dest_y, width, height,
              (HDC) src_context->context, src_x, src_y, (HBITMAP) mask,
              mask_x, mask_y, MAKEROP4 (DSTCOPY, rop2))) &&
        (mask_context =
         CreateCompatibleDC ((HDC) dest_context->context)) != (HDC) NULL)
    {
        /*
        ** Implement our own version of MaskBlt using multiple calls to
        ** BitBlt
        */

        SetMapMode (
            mask_context,
            GetMapMode ((HDC) dest_context->context));

        sys_bitmap = SelectBitmap (mask_context, (HBITMAP) mask);

        sys_fg = SetTextColor ((HDC) dest_context->context, COLOR_WHITE);
        sys_bg = SetBkColor ((HDC) dest_context->context, COLOR_BLACK);

        BitBlt (
            (HDC) dest_context->context,
            dest_x,
            dest_y,
            width,
            height,
            mask_context,
            mask_x,
            mask_y,
            NOTSRCAND);

        if ((src_bitmap =
             (HBITMAP)
                 GetCurrentObject ((HDC) src_context->context, OBJ_BITMAP))
                != (HBITMAP) NULL &&
            GetObject (
                src_bitmap, sizeof (BITMAP), &src_bitmap_data) > 0 &&
            src_bitmap_data.bmPlanes == 1 &&
            src_bitmap_data.bmBitsPixel == 1)
        {
            SetTextColor ((HDC) dest_context->context, sys_bg);

            BitBlt (
                (HDC) dest_context->context,
                dest_x,
                dest_y,
                width,
                height,
                mask_context,
                mask_x,
                mask_y,
                SRCPAINT);

            SetTextColor ((HDC) dest_context->context, COLOR_WHITE);

            BitBlt (
                (HDC) dest_context->context,
                dest_x,
                dest_y,
                width,
                height,
                (HDC) src_context->context,
                src_x,
                src_y,
                NOTSRCAND);

            SetTextColor ((HDC) dest_context->context, sys_fg);

            BitBlt (
                (HDC) dest_context->context,
                dest_x,
                dest_y,
                width,
                height,
                (HDC) src_context->context,
                src_x,
                src_y,
                SRCPAINT);

            SetTextColor ((HDC) dest_context->context, sys_fg);
            SetBkColor ((HDC) dest_context->context, sys_bg);
        }
        else
        {
            SetTextColor ((HDC) dest_context->context, sys_fg);
            SetBkColor ((HDC) dest_context->context, sys_bg);

            BitBlt (
                (HDC) dest_context->context,
                dest_x,
                dest_y,
                width,
                height,
                (HDC) src_context->context,
                src_x,
                src_y,
                SRCPAINT);
        }

        SelectBitmap (mask_context, sys_bitmap);
        DeleteDC (mask_context);
    }

    if (dest_context->depth >= 32)
    {
        rect.x = src_x;
        rect.y = src_y;
        rect.width = width;
        rect.height = height;

        if ((meta_region =
             _uint_gfx_region_create_from_bitmap ((HBITMAP) mask, &rect))
                != (HRGN) NULL)
        {
            if (src_context->region != NULL)
            {
                _ui_gfx_region_intersect (meta_region, src_context->region);
            }

            if (dest_x != src_x ||
                dest_y != src_y)
            {
                OffsetRgn (
                    (HRGN) meta_region,
                    (dest_x - src_x),
                    (dest_y - src_y));
            }

            _uint_gfx_set_alpha (dest_context, 255, meta_region);

            _ui_gfx_region_destroy (&meta_region);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_alpha_blend
| Purpose:  Copy the given area from the given context to the other context
|           using the given alpha value
| Input:    dest_context        - the destination context
|           dest_x              - the destination rectangle left position
|           dest_y              - the destination rectangle top position
|           width               - the rectangle width
|           height              - the rectangle height
|           src_context         - the source context
|           src_x               - the source rectangle left position
|           src_y               - the source rectangle top position
|           src_alpha           - the source alpha value
|           mode                - the copy mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_alpha_blend (ui_gfx_t *dest_context, int dest_x,
                                     int dest_y, int width, int height,
                                     const ui_gfx_t *src_context, int src_x,
                                     int src_y, int src_alpha, int mode)
{
#ifdef UI_SYSTEM_NT

    BLENDFUNCTION   blend_function = { AC_SRC_OVER, 0, 0, 0 };
    int             dest_alpha;
    int             sx, sy, dx, dy, dw, dh;
    HBITMAP         src_bitmap;
    BITMAP          src_bitmap_data;
    COLORREF        src_color, dest_color, fg_color, bg_color;
    nt_dib32_t      dest_dib = { (HBITMAP) NULL, (unsigned char *) NULL, 0 };
    nt_dib32_t      src_dib = { (HBITMAP) NULL, (unsigned char *) NULL, 0 };
    unsigned int    r, g, b;
    int             status;

    if (!(_ui_gfx_is_accelerated (src_context)) ||
        !(_ui_gfx_is_accelerated (dest_context)) ||
        (status =
         _uint_gfx_d2d_alpha_blend (
             dest_context, dest_x, dest_y, width, height,
             src_context, src_x, src_y, width, height, src_alpha,
             mode)) != UI_SUCCESS)
    {
        _uint_d2d_renderer_destroy ((HDC) src_context->context);

        if (dest_context->context != src_context->context)
        {
            _uint_d2d_renderer_destroy ((HDC) dest_context->context);
        }

        blend_function.SourceConstantAlpha = src_alpha;
        blend_function.AlphaFormat = ((mode == UI_COPY) ? AC_SRC_ALPHA : 0);

        if (AlphaBlend (
                (HDC) dest_context->context, dest_x, dest_y, width, height,
                (HDC) src_context->context, src_x, src_y, width, height,
                blend_function))
        {
            status = UI_SUCCESS;
        }
        else if (width < 0 ||
                 height < 0)
        {
            status = UI_ERROR;
        }
        else
        {
            if ((src_bitmap =
                 (HBITMAP)
                     GetCurrentObject ((HDC) src_context->context, OBJ_BITMAP))
                    != (HBITMAP) NULL &&
                GetObject (src_bitmap, sizeof (BITMAP), &src_bitmap_data) > 0 &&
                src_bitmap_data.bmPlanes == 1 &&
                src_bitmap_data.bmBitsPixel == 1)
            {
                fg_color = GetTextColor ((HDC) src_context->context);
                bg_color = GetBkColor ((HDC) src_context->context);
            }
            else
            {
                fg_color = bg_color = CLR_INVALID;
            }

            src_alpha = MAX (MIN (src_alpha, 255), 0);
            dest_alpha = (255 - src_alpha);

            dw = (dest_x + width);
            dh = (dest_y + height);

            if (dest_context->depth < 32)
            {
                if (fg_color != CLR_INVALID &&
                    bg_color != CLR_INVALID)
                {
                    for (sy = src_y, dy = dest_y; dy < dh; dy++, sy++)
                    {
                        for (sx = src_x, dx = dest_x; dx < dw; dx++, sx++)
                        {
                            src_color =
                                (GetPixel ((HDC) src_context->context, sx, sy) ?
                                 fg_color :
                                 bg_color);

                            dest_color =
                                GetPixel ((HDC) dest_context->context, dx, dy);

                            r = ((((GetRValue (src_color) * src_alpha) +
                                   (GetRValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            g = ((((GetGValue (src_color) * src_alpha) +
                                   (GetGValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            b = ((((GetBValue (src_color) * src_alpha) +
                                   (GetBValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            SetPixelV (
                                (HDC) dest_context->context,
                                dx,
                                dy,
                                RGB (r, g, b));
                        }
                    }
                }
                else
                {
                    for (sy = src_y, dy = dest_y; dy < dh; dy++, sy++)
                    {
                        for (sx = src_x, dx = dest_x; dx < dw; dx++, sx++)
                        {
                            src_color =
                                GetPixel ((HDC) src_context->context, sx, sy);

                            dest_color =
                                GetPixel ((HDC) dest_context->context, dx, dy);

                            r = ((((GetRValue (src_color) * src_alpha) +
                                   (GetRValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            g = ((((GetGValue (src_color) * src_alpha) +
                                   (GetGValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            b = ((((GetBValue (src_color) * src_alpha) +
                                   (GetBValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            SetPixelV (
                                (HDC) dest_context->context,
                                dx,
                                dy,
                                RGB (r, g, b));
                        }
                    }
                }
            }
            else
            {
                dest_dib.bitmap =
                    GetCurrentObject ((HDC) dest_context->context, OBJ_BITMAP);

                _uint_gfx_cache_bitmap32 (&dest_dib);

                if (src_context->depth >= 32)
                {
                    src_dib.bitmap =
                        GetCurrentObject ((HDC) src_context->context, OBJ_BITMAP);

                    _uint_gfx_cache_bitmap32 (&src_dib);
                }

                if (fg_color != CLR_INVALID &&
                    bg_color != CLR_INVALID)
                {
                    for (sy = src_y, dy = dest_y; dy < dh; dy++, sy++)
                    {
                        for (sx = src_x, dx = dest_x; dx < dw; dx++, sx++)
                        {
                            src_color =
                                (GetPixel ((HDC) src_context->context, sx, sy) ?
                                 fg_color :
                                 bg_color);

                            dest_color =
                                GetPixel ((HDC) dest_context->context, dx, dy);

                            r = ((((GetRValue (src_color) * src_alpha) +
                                   (GetRValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            g = ((((GetGValue (src_color) * src_alpha) +
                                   (GetGValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            b = ((((GetBValue (src_color) * src_alpha) +
                                   (GetBValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            SetPixelV (
                                (HDC) dest_context->context,
                                dx,
                                dy,
                                RGB (r, g, b));
                        }
                    }
                }
                else
                {
                    for (sy = src_y, dy = dest_y; dy < dh; dy++, sy++)
                    {
                        for (sx = src_x, dx = dest_x; dx < dw; dx++, sx++)
                        {
                            src_color =
                                GetPixel ((HDC) src_context->context, sx, sy);

                            dest_color =
                                GetPixel ((HDC) dest_context->context, dx, dy);

                            r = ((((GetRValue (src_color) * src_alpha) +
                                   (GetRValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            g = ((((GetGValue (src_color) * src_alpha) +
                                   (GetGValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            b = ((((GetBValue (src_color) * src_alpha) +
                                   (GetBValue (dest_color) * dest_alpha)) /
                                  255) &
                                 255);

                            SetPixelV (
                                (HDC) dest_context->context,
                                dx,
                                dy,
                                RGB (r, g, b));
                        }
                    }
                }
            }

            status = UI_SUCCESS;
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_copy_gradient_area
| Purpose:  Copy the given area from the given context to the other context,
|           given alpha gradient
| Input:    dest_context        - the destination context
|           dest_x              - the destination rectangle left position
|           dest_y              - the destination rectangle top position
|           width               - the rectangle width
|           height              - the rectangle height
|           src_context         - the source context
|           src_x               - the source rectangle left position
|           src_y               - the source rectangle top position
|           orientation         - the orientation
|           first_alpha         - the left / top source alpha value
|           last_alpha          - the right / bottom source alpha value
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_copy_gradient_area (ui_gfx_t *dest_context,
                                            int dest_x, int dest_y,
                                            int width, int height,
                                            const ui_gfx_t *src_context,
                                            int src_x, int src_y,
                                            int orientation, int first_alpha,
                                            int last_alpha)
{
#ifdef UI_SYSTEM_NT

    return (UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_create_from_rect
| Purpose:  Create a region from the given rectangle
| Input:    rect                - the rectangle
| Output:   region              - the created region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_create_from_rect (void **region,
                                                 const ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    *region =
        (void *)
            CreateRectRgn (
                rect->x,
                rect->y,
                (rect->x + rect->width),
                (rect->y + rect->height));

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_create_from_polygon
| Purpose:  Create a region from the given polygon
| Input:    points              - the points to link
|           count               - the number of points
|           mode                - the polygon-fill mode
| Output:   region              - the created region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_create_from_polygon (void **region,
                                                    const ui_point_t *points,
                                                    int count, int mode)
{
#ifdef UI_SYSTEM_NT

    POINT  *dyn_points;
    int     i;

    PRO_CREATE_AND_LOCK_STATIC_BUFFER (dyn_points, 16, count);
    {
        for (i = 0; i < count; i++)
        {
            dyn_points [i].x = points [i].x;
            dyn_points [i].y = points [i].y;
        }

        *region =
            (void *)
                CreatePolygonRgn (
                    dyn_points,
                    count,
                    ((mode == UI_ALTERNATE) ?
                     ALTERNATE :
                     WINDING));
    }
    PRO_UNLOCK_STATIC_BUFFER (dyn_points);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_create_from_ellipse
| Purpose:  Create an elliptical region from the given rectangle
| Input:    rect                - the rectangle
| Output:   region              - the created region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_create_from_ellipse (void **region,
                                                    const ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    *region =
        (void *)
            CreateEllipticRgn (
                rect->x,
                rect->y,
                (rect->x + rect->width),
                (rect->y + rect->height));

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_create_from_arc
| Purpose:  Create an arc region from the given data
| Input:    arc                 - the arc
|           mode                - the arc fill-mode
|           direction           - the arc direction
| Output:   region              - the created region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_create_from_arc (void **region,
                                                const ui_arc_t *arc, int mode,
                                                int direction)
{
#ifdef UI_SYSTEM_NT

    HWND    desktop_window = GetDesktopWindow ();
    HDC     desktop_context, context;
    POINT   offset = { 0, 0 };

    desktop_context = GetDC (desktop_window);
    context = CreateCompatibleDC (desktop_context);

    SetArcDirection (
        context,
        ((direction == UI_CLOCKWISE) ?
         AD_CLOCKWISE :
         AD_COUNTERCLOCKWISE));

    BeginPath (context);

    if (mode == UI_PIE)
    {
        Pie (
            context,
            arc->rect.x,
            arc->rect.y,
            (arc->rect.x + arc->rect.width),
            (arc->rect.y + arc->rect.height),
            arc->start.x,
            arc->start.y,
            arc->end.x,
            arc->end.y);
    }
    else
    {
        Chord (
            context,
            arc->rect.x,
            arc->rect.y,
            (arc->rect.x + arc->rect.width),
            (arc->rect.y + arc->rect.height),
            arc->start.x,
            arc->start.y,
            arc->end.x,
            arc->end.y);
    }

    EndPath (context);

    if ((*region = (void *) PathToRegion (context)) != NULL &&
        DPtoLP (context, &offset, 1) &&
        (offset.x != 0 ||
         offset.y != 0))
    {
        OffsetRgn ((HRGN) *region, offset.x, offset.y);
    }

    AbortPath (context);

    DeleteDC (context);
    ReleaseDC (desktop_window, desktop_context);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_create_from_image
| Purpose:  Create an image region from the given data
| Input:    image               - the image data
| Output:   region              - the created region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_create_from_image (void **region,
                                                  const ui_image_data_t *image)
{
#ifdef UI_SYSTEM_NT

    int             bpp;
    unsigned char  *ptr;
    int             count;
    int             x, y, width;
    int             trans;
    RGNDATA        *region_data;
    RECT           *region_rects;

    *region = NULL;

    if (image != (const ui_image_data_t *) NULL)
    {
        bpp = ((image->depth > 8) ? 4 : 1);

        for (ptr = image->data, count = y = 0;
             y < image->size.height;
             y++)
        {
            for (width = x = 0;
                 x < image->size.width;
                 x++, ptr += bpp)
            {
                if (image->depth == 32)
                {
                    trans = (ptr [3] < 0x10);
                }
                else if (bpp == 4 &&
                         UI_COLOR_IS_RGB (ptr [3] << 24))
                {
                    trans =
                        UI_COLOR_IS_TRANSPARENT_RGB (ptr [3] << 24);
                }
                else
                {
                    trans = (ptr [0] == UI_COLOR_TRANSPARENT);
                }

                if (!trans)
                {
                    width++;
                }
                else if (width > 0)
                {
                    width = 0;
                    count++;
                }
            }

            if (width > 0)
            {
                width = 0;
                count++;
            }
        }

        if (count > 0)
        {
            region_data =
                (RGNDATA *)
                    getmem (sizeof (RGNDATAHEADER) + (sizeof (RECT) * count));

            if (region_data != (RGNDATA *) NULL)
            {
                region_data->rdh.dwSize = sizeof (RGNDATAHEADER);
                region_data->rdh.iType = RDH_RECTANGLES;

                SetRect (
                    &(region_data->rdh.rcBound),
                    0,
                    0,
                    image->size.width,
                    image->size.height);

                region_rects = (RECT *) &(region_data->Buffer);

                for (ptr = image->data, count = y = 0;
                     y < image->size.height;
                     y++)
                {
                    for (width = x = 0;
                         x < image->size.width;
                         x++, ptr += bpp)
                    {
                        if (image->depth == 32)
                        {
                            trans = (ptr [3] < 0x10);
                        }
                        else if (bpp == 4 &&
                                 UI_COLOR_IS_RGB (ptr [3] << 24))
                        {
                            trans =
                                UI_COLOR_IS_TRANSPARENT_RGB (ptr [3] << 24);
                        }
                        else
                        {
                            trans = (ptr [0] == UI_COLOR_TRANSPARENT);
                        }

                        if (!trans)
                        {
                            width++;
                        }
                        else if (width > 0)
                        {
                            if (count > 0 &&
                                region_rects [count - 1].bottom == y &&
                                region_rects [count - 1].right == x &&
                                region_rects [count - 1].left == (x - width))
                            {
                                region_rects [count - 1].bottom++;
                            }
                            else
                            {
                                region_rects [count].left = (x - width);
                                region_rects [count].top = y;
                                region_rects [count].right = x;
                                region_rects [count].bottom = (y + 1);

                                count++;
                            }

                            width = 0;
                        }
                    }

                    if (width > 0)
                    {
                        if (count > 0 &&
                            region_rects [count - 1].bottom == y &&
                            region_rects [count - 1].right == x &&
                            region_rects [count - 1].left == (x - width))
                        {
                            region_rects [count - 1].bottom++;
                        }
                        else
                        {
                            region_rects [count].left = (x - width);
                            region_rects [count].top = y;
                            region_rects [count].right = x;
                            region_rects [count].bottom = (y + 1);

                            count++;
                        }

                        width = 0;
                    }
                }

                if ((region_data->rdh.nCount = count) > 0)
                {
                    region_data->rdh.nRgnSize = (sizeof (RECT) * count);

                    *region =
                        (void *)
                            ExtCreateRegion (
                                (XFORM *) NULL,
                                (DWORD)
                                    (region_data->rdh.dwSize +
                                     region_data->rdh.nRgnSize),
                                region_data);
                }

                relmem (&region_data);
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_destroy
| Purpose:  Destroy the given region
| Input:    region              - the region
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_destroy (void **region)
{
#ifdef UI_SYSTEM_NT

    DeleteRgn ((HRGN) *region);

    *region = NULL;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_box
| Purpose:  Get the bounding box of the given region
| Input:    region              - the region
| Output:   rect                - the bounding rectangle of the region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_box (void *region, ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    RECT    box;

    GetRgnBox ((HRGN) region, &box);

    rect->x = box.left;
    rect->y = box.top;
    rect->width = (box.right - box.left);
    rect->height = (box.bottom - box.top);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_rects
| Purpose:  Get the rectangles which make up the given region
| Input:    region              - the region
| Output:   count               - the number of rectangles which make up
|                                 the region
|           rects               - the rectangles of the region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_rects (void *region, int *count,
                                      ui_rect_t *rects)
{
#ifdef UI_SYSTEM_NT

    DWORD           size;
    unsigned char  *buffer;
    RGNDATA        *data;
    RECT           *rect;
    DWORD           i;
    int             status = UI_ERROR;

    if ((size = GetRegionData ((HRGN) region, 0, (RGNDATA *) NULL)) > 0)
    {
        if (rects == (ui_rect_t *) NULL)
        {
            /*
            ** We know the size required to store the full RGNDATA.
            **
            ** We also know that the RGNDATA structure is made up of a
            ** RGNDATAHEADER followed by an array of RECT structures.
            **
            ** So to avoid any memory allocation and a second call to
            ** GetRegionData, we instead calculate the number of rectangles
            ** using simple arithmetic.  This does not take into account any
            ** padding for the RGNDATAHEADER structure, but any such padding
            ** will be fewer bytes than the size of a RECT structure, and so
            ** will not affect the calculation.
            **
            ** jas - 21-Aug-24
            */

            *count = ((size - sizeof (RGNDATAHEADER)) / sizeof (RECT));
        }
        else
        {
            PRO_CREATE_AND_LOCK_STATIC_BUFFER (buffer, 1024, size);
            {
                if ((data = (RGNDATA *) buffer) != (RGNDATA *) NULL &&
                    GetRegionData ((HRGN) region, size, data) &&
                    data->rdh.iType == RDH_RECTANGLES)
                {
                    *count = (int) data->rdh.nCount;

                    for (rect = (RECT *) data->Buffer, i = 0;
                         i < data->rdh.nCount;
                         i++, rect++)
                    {
                        rects [i].x = (int) rect->left;
                        rects [i].y = (int) rect->top;
                        rects [i].width = (int) (rect->right - rect->left);
                        rects [i].height = (int) (rect->bottom - rect->top);
                    }
                }
            }
            PRO_UNLOCK_STATIC_BUFFER (buffer);
        }

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_is_empty
| Purpose:  Determine whether a region is empty
| Input:    region              - the region
| Output:
| Return:   UI_SUCCESS if the rectangle is empty, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_is_empty (void *region)
{
#ifdef UI_SYSTEM_NT

    RECT    box;

    return ((GetRgnBox ((HRGN) region, &box) == NULLREGION) ?
            UI_SUCCESS :
            UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_is_equal
| Purpose:  Determine whether one region is identical to another
| Input:    region1             - the first region
|           region2             - the second region
| Output:
| Return:   UI_SUCCESS if the two regions are identical, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_is_equal (void *region1, void *region2)
{
#ifdef UI_SYSTEM_NT

    return ((EqualRgn ((HRGN) region1, (HRGN) region2) == TRUE) ?
            UI_SUCCESS :
            UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_contains_rect
| Purpose:  Determine whether a rectangle is wholly within a region
| Input:    region              - the region
|           rect                - the rectangle
| Output:
| Return:   UI_SUCCESS if the rectangle is wholly within the region,
|           otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_contains_rect (void *region,
                                              const ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    RECT    nt_rect;
    HRGN    rect_region;
    BOOL    status = ERROR;

    SetRect (
        &nt_rect,
        rect->x,
        rect->y,
        (rect->x + rect->width),
        (rect->y + rect->height));

    if (RectInRegion ((HRGN) region, &nt_rect))
    {
        rect_region = CreateRectRgnIndirect (&nt_rect);

        status =
            CombineRgn (rect_region, rect_region, (HRGN) region, RGN_DIFF);

        DeleteRgn (rect_region);
    }

    return ((status == NULLREGION) ?
            UI_SUCCESS :
            UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_intersects_rect
| Purpose:  Determine whether a rectangle intersects a region
| Input:    region              - the region
|           rect                - the rectangle
| Output:
| Return:   UI_SUCCESS if the rectangle is within the region, otherwise
|           UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_intersects_rect (void *region,
                                                const ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    RECT    nt_rect;

    SetRect (
        &nt_rect,
        rect->x,
        rect->y,
        (rect->x + rect->width),
        (rect->y + rect->height));

    return (RectInRegion ((HRGN) region, &nt_rect) ?
            UI_SUCCESS :
            UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_offset
| Purpose:  Offset the given region by the given amount
| Input:    region              - the region
|           x                   - the offset in the x-direction
|           y                   - the offset in the y-direction
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_offset (void *region, int x, int y)
{
#ifdef UI_SYSTEM_NT

    OffsetRgn ((HRGN) region, x, y);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_combine
| Purpose:  Combine the two given regions according to the given mode
| Input:    region1             - the first region
|           region2             - the second region
|           mode                - the combination mode
| Output:   target              - the resulting region
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_combine (void **target, void *region1,
                                        void *region2, int mode)
{
#ifdef UI_SYSTEM_NT

    switch (mode)
    {
        case UI_NOT:
        {
            mode = RGN_DIFF;
            break;
        }

        case UI_AND:
        {
            mode = RGN_AND;
            break;
        }

        case UI_OR:
        {
            mode = RGN_OR;
            break;
        }

        case UI_XOR:
        {
            mode = RGN_XOR;
            break;
        }

        default:
        {
            mode = RGN_COPY;
        }
    }

    CombineRgn ((HRGN) *target, (HRGN) region1, (HRGN) region2, mode);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_combine_children
| Purpose:  Combine the regions of the children of the given window with the
|           given region according to the given mode
| Input:    region              - the region
|           parent              - the window of whose children to process
|           mode                - the combination mode
|           force               - flag indicating whether to perform
|                                 seemingly unnecessary work
| Output:   target              - the resulting region
| Return:   UI_SUCCESS if any regions were combined, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_region_combine_children (void **target, void *region,
                                                 void *parent, int mode,
                                                 int force)
{
#ifdef UI_SYSTEM_NT

    HWND    child;
    int     region_status;
    RECT    rect;
    void   *child_region;
    int     status = UI_ERROR;

    if (!(IsWindow ((HWND) parent)) ||
        (mode == UI_NOT &&
         !force &&
         (GetWindowStyle ((HWND) parent) & WS_CLIPCHILDREN)))
    {
        child = (HWND) NULL;
    }
    else
    {
        child = GetFirstChild ((HWND) parent);
    }

    for (; child != (HWND) NULL; child = GetNextSibling (child))
    {
        if (IsChildWindowVisible (child) &&
            !(IsWindowTransparentU (child)))
        {
            GetClientRect (child, &rect);
            MapWindowRect (child, (HWND) parent, &rect);

            child_region = (void *) CreateRectRgn (0, 0, 0, 0);

            if ((region_status =
                 _uint_get_window_region (child, child_region)) != ERROR &&
                region_status != NULLREGION)
            {
                OffsetRgn ((HRGN) child_region, rect.left, rect.top);
            }
            else
            {
                DeleteRgn ((HRGN) child_region);

                child_region = (void *) CreateRectRgnIndirect (&rect);
            }

            if (mode == UI_COPY)
            {
                mode = UI_OR;

                region = child_region;
            }

            _uint_gfx_region_combine (target, region, child_region, mode);

            DeleteRgn ((HRGN) child_region);

            region = *target;

            status = UI_SUCCESS;
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_clip_region
| Purpose:  Set the clipping region of the given context
| Input:    context             - the context
|           region              - the region to use for clipping
|           mode                - the combination mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_clip_region (ui_gfx_t *context, void *region,
                                         int mode)
{
#ifdef UI_SYSTEM_NT

    POINT   offset = { 0, 0 };
    HRGN    clip_region = (HRGN) region;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_set_clip_region (context, region, mode) != UI_SUCCESS)
    {
        _uint_d2d_renderer_destroy ((HDC) context->context);
    }

    if (region != NULL &&
        LPtoDP ((HDC) context->context, &offset, 1) &&
        (offset.x != 0 ||
         offset.y != 0))
    {
        clip_region = CreateRectRgn (0, 0, 0, 0);

        CopyRgn (clip_region, (HRGN) region);
        OffsetRgn (clip_region, offset.x, offset.y);
    }

    switch (mode)
    {
        case UI_NOT:
        {
            mode = RGN_DIFF;
            break;
        }

        case UI_AND:
        {
            mode = RGN_AND;
            break;
        }

        case UI_OR:
        {
            mode = RGN_OR;
            break;
        }

        case UI_XOR:
        {
            mode = RGN_XOR;
            break;
        }

        default:
        {
            mode = RGN_COPY;
        }
    }

    if (context->drawable != NULL ||
        region != NULL)
    {
        ExtSelectClipRgn ((HDC) context->context, clip_region, mode);

        if (context->region == NULL)
        {
            if (region == NULL)
            {
                UI_GFX_CLEAR_FLAG (context, UI_GFX_CLIPPED_FLAG);
            }
        }
        else if (context->region == region)
        {
            UI_GFX_SET_FLAG (context, UI_GFX_CLIPPED_FLAG);
        }
        else
        {
            _uint_gfx_set_clip_region (context, context->region, UI_AND);
        }
    }
    else
    {
        ExtSelectClipRgn ((HDC) context->context, clip_region, mode);
    }

    if (clip_region != (HRGN) region)
    {
        DeleteRgn (clip_region);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_accelerated
| Purpose:  Enable or disable accelerated drawing for the given context
| Input:    context             - the context
|           accelerated         - TRUE to enable accelerated drawing
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_accelerated (ui_gfx_t *context, int accelerated)
{
#ifdef UI_SYSTEM_NT

    int status = UI_ERROR;

    if (_ui_gfx_is_accelerated (context) == !accelerated)
    {
        if (accelerated)
        {
            UI_GFX_SET_FLAG (context, UI_GFX_ACCELERATED_FLAG);
        }
        else
        {
            _uint_d2d_renderer_destroy ((HDC) context->context);

            UI_GFX_CLEAR_FLAG (context, UI_GFX_ACCELERATED_FLAG);
        }

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_create_bitmap
| Purpose:  Create a bitmap of the given size which is compatible with the
|           given context
| Input:    context             - the context
|           width               - the width of the bitmap
|           height              - the height of the bitmap
| Output:   bitmap              - the created bitmap
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_create_bitmap (const ui_gfx_t *context, int width,
                                       int height, void **bitmap)
{
#ifdef UI_SYSTEM_NT

    if (context->depth < 32)
    {
        *bitmap =
            (void *)
                CreateCompatibleBitmap (
                    (HDC) context->context,
                    width,
                    height);
    }
    else
    {
        *bitmap =
            (void *)
                _uint_gfx_create_bitmap32 (
                    (HDC) context->context,
                    width,
                    height,
                    (unsigned char **) NULL,
                    (int *) NULL);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_destroy_bitmap
| Purpose:  Destroy the given bitmap
| Input:    bitmap              - the bitmap
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_destroy_bitmap (void **bitmap)
{
#ifdef UI_SYSTEM_NT

    if (!(_uint_gfx_destroy_bitmap32 ((HBITMAP) *bitmap)))
    {
        DeleteBitmap ((HBITMAP) *bitmap);
    }

    *bitmap = NULL;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_create_brush
| Purpose:  Create a brush of the given color in the given context
| Input:    context             - the context
|           color               - the color of the brush
|           style               - the style of the brush
| Output:   brush               - the created brush
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_create_brush (const ui_gfx_t *context,
                                      ui_rgb_t color, int style,
                                      void **brush)
{
#ifdef UI_SYSTEM_NT

    static unsigned char    light_stipple_data [] =
    {
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00,
        0xee, 0x00, 0xbb, 0x00
    };

    static unsigned char    medium_stipple_data [] =
    {
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00,
        0x55, 0x00, 0xaa, 0x00
    };

    static unsigned char    heavy_stipple_data [] =
    {
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00,
        0x11, 0x00, 0x44, 0x00
    };

    HBITMAP                 stipple;

    switch (style)
    {
        case UI_LIGHT_STIPPLE:
        {
            stipple = CreateBitmap (8, 8, 1, 1, light_stipple_data);
            *brush = (void *) CreatePatternBrush (stipple);
            DeleteBitmap (stipple);
            break;
        }

        case UI_MEDIUM_STIPPLE:
        {
            stipple = CreateBitmap (8, 8, 1, 1, medium_stipple_data);
            *brush = (void *) CreatePatternBrush (stipple);
            DeleteBitmap (stipple);
            break;
        }

        case UI_HEAVY_STIPPLE:
        {
            stipple = CreateBitmap (8, 8, 1, 1, heavy_stipple_data);
            *brush = (void *) CreatePatternBrush (stipple);
            DeleteBitmap (stipple);
            break;
        }

        default:
        {
            *brush =
                (void *)
                    CreateSolidBrush (
                        (COLORREF)
                            ((color == UI_BAD_COLOR_RGB) ?
                             _uint_gfx_get_fg_color_value (context) :
                             _ui_color_get_color (color)));
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_destroy_brush
| Purpose:  Destroy the given brush
| Input:    brush               - the brush
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_destroy_brush (void **brush)
{
#ifdef UI_SYSTEM_NT

    DeleteBrush ((HBRUSH) *brush);

    *brush = NULL;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_create_pen
| Purpose:  Create a pen of the given color in the given context
| Input:    context             - the context
|           color               - the color of the pen
|           style               - the style of the pen
|           width               - the width of the pen
| Output:   pen                 - the created pen
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_create_pen (const ui_gfx_t *context, ui_rgb_t color,
                                    int style, int width, void **pen)
{
#ifdef UI_SYSTEM_NT

    LOGBRUSH    lb = { BS_SOLID };

    lb.lbColor =
        (COLORREF)
            ((color == UI_BAD_COLOR_RGB) ?
             _uint_gfx_get_fg_color_value (context) :
             _ui_color_get_color (color));

    switch (style)
    {
        case UI_DOTTED:
        {
            style = (PS_COSMETIC | PS_ALTERNATE);
            break;
        }

        case UI_DASHED:
        {
            style = (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE | PS_JOIN_MITER);
            break;
        }

        default:
        {
            style =
                (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE | PS_JOIN_MITER);
        }
    }

    *pen =
        (void *) ExtCreatePen ((DWORD) style, width, &lb, 0, (DWORD *) NULL);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_destroy_pen
| Purpose:  Destroy the given pen
| Input:    pen                 - the pen
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_destroy_pen (void **pen)
{
#ifdef UI_SYSTEM_NT

    DeletePen ((HPEN) *pen);

    *pen = NULL;

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_depth
| Purpose:  Set the depth of the given context
| Input:    context             - the context
|           depth               - the depth
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_depth (ui_gfx_t *context, int depth)
{
#ifdef UI_SYSTEM_NT

    void   *bitmap = context->drawable;
    BITMAP  bitmap_data;
    int     status = UI_ERROR;

    if (GetObject ((HBITMAP) bitmap, sizeof (BITMAP), &bitmap_data) > 0)
    {
        context->depth = depth;

        context->drawable =
            _ui_gfx_create_bitmap (
                context,
                (int) bitmap_data.bmWidth,
                (int) bitmap_data.bmHeight);

        SelectBitmap ((HDC) context->context, (HBITMAP) context->drawable);

        DeleteBitmap ((HBITMAP) bitmap);

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_fg_color_value
| Purpose:  Set the foreground color value of the given context
| Input:    context             - the context
|           color               - the foreground color value
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_fg_color_value (ui_gfx_t *context, int color)
{
#ifdef UI_SYSTEM_NT

    SetTextColor ((HDC) context->context, (COLORREF) color);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_fg_color_value
| Purpose:  Get the foreground color value of the given context
| Input:    context             - the context
| Output:
| Return:   The foreground color value of the context
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_get_fg_color_value (const ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    return ((int) GetTextColor ((HDC) context->context));

#else

    return ((int) UI_BAD_COLOR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_bg_color
| Purpose:  Set the background color of the given context
| Input:    context             - the context
|           color               - the background color
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_bg_color (ui_gfx_t *context, ui_rgb_t color)
{
#ifdef UI_SYSTEM_NT

    SetBkColor (
        (HDC) context->context,
        (COLORREF) _ui_color_get_color (color));

    SetBkMode (
        (HDC) context->context,
        ((color == UI_COLOR_TRANSPARENT ||
          (UI_COLOR_IS_RGB (color) &&
           UI_COLOR_IS_TRANSPARENT_RGB (color))) ?
         TRANSPARENT :
         OPAQUE));

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_bg_color_value
| Purpose:  Set the background color value of the given context
| Input:    context             - the context
|           color               - the background color value
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_bg_color_value (ui_gfx_t *context, int color)
{
#ifdef UI_SYSTEM_NT

    SetBkColor ((HDC) context->context, (COLORREF) color);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_bg_color_value
| Purpose:  Get the background color value of the given context
| Input:    context             - the context
| Output:
| Return:   The background color value of the context
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_get_bg_color_value (const ui_gfx_t *context)
{
#ifdef UI_SYSTEM_NT

    return ((int) GetBkColor ((HDC) context->context));

#else

    return ((int) UI_BAD_COLOR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_mix_mode
| Purpose:  Set the mix mode of the given context
| Input:    context             - the context
|           mode                - the mix mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_mix_mode (ui_gfx_t *context, int mode)
{
#ifdef UI_SYSTEM_NT

    switch (mode)
    {
        case UI_NOT:
        {
            mode = R2_NOT;
            break;
        }

        case UI_AND:
        {
            mode = R2_MASKPEN;
            break;
        }

        case UI_OR:
        {
            mode = R2_MERGEPEN;
            break;
        }

        case UI_XOR:
        {
            mode = R2_XORPEN;
            break;
        }

        default:
        {
            mode = R2_COPYPEN;
        }
    }

    SetROP2 ((HDC) context->context, mode);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_polygon_fill_mode
| Purpose:  Set the polygon-fill mode of the given context
| Input:    context             - the context
|           mode                - the polygon-fill mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_polygon_fill_mode (ui_gfx_t *context, int mode)
{
#ifdef UI_SYSTEM_NT

    SetPolyFillMode (
        (HDC) context->context,
        ((mode == UI_ALTERNATE) ?
         ALTERNATE :
         WINDING));

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_arc_fill_mode
| Purpose:  Set the arc-fill mode of the given context
| Input:    context             - the context
|           mode                - the arc-fill mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_arc_fill_mode (ui_gfx_t *context, int mode)
{
#ifdef UI_SYSTEM_NT

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_arc_direction
| Purpose:  Set the arc direction of the given context
| Input:    context             - the context
|           direction           - the arc direction
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_arc_direction (ui_gfx_t *context, int direction)
{
#ifdef UI_SYSTEM_NT

    SetArcDirection (
        (HDC) context->context,
        ((direction == UI_CLOCKWISE) ?
         AD_CLOCKWISE :
         AD_COUNTERCLOCKWISE));

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_points
| Purpose:  Draw the given points using the given color
| Input:    context             - the context
|           color               - the color
|           points              - the points to draw
|           count               - the number of points
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_points (ui_gfx_t *context, ui_rgb_t color,
                                     const ui_point_t *points, int count)
{
#ifdef UI_SYSTEM_NT

    COLORREF    pixel =
        (COLORREF)
            ((color == UI_BAD_COLOR_RGB) ?
             _uint_gfx_get_fg_color_value (context) :
             _ui_color_get_color (color));
    int         i;

    for (i = 0; i < count; i++)
    {
        SetPixelV ((HDC) context->context, points [i].x, points [i].y, pixel);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_polyline
| Purpose:  Draw the given polyline using the given pen
| Input:    context             - the context
|           pen                 - the pen
|           points              - the points to link
|           count               - the number of points
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_polyline (ui_gfx_t *context, void *pen,
                                       const ui_point_t *points, int count)
{
#ifdef UI_SYSTEM_NT

    POINT  *dyn_points;
    int     i, s;
    void   *sys_pen;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_polygon (context, pen, points, count, FALSE, FALSE)
            != UI_SUCCESS)
    {
        if (pen <= (void *) 0 ||
            pen > (void *) 255)
        {
            sys_pen = _uint_gfx_use_pen (context, &pen);

            PRO_CREATE_AND_LOCK_STATIC_BUFFER (dyn_points, 16, count);
            {
                for (i = 0; i < count; i++)
                {
                    dyn_points [i].x = points [i].x;
                    dyn_points [i].y = points [i].y;
                }

                Polyline ((HDC) context->context, dyn_points, count);
            }
            PRO_UNLOCK_STATIC_BUFFER (dyn_points);

            SelectPen ((HDC) context->context, (HPEN) sys_pen);

            if (pen != NULL)
            {
                DeletePen ((HPEN) pen);
            }

            pen = (void *) 255;
        }

        if (context->depth >= 32 &&
            count > 1)
        {
            for (i = 1; i < count; i++)
            {
                s = (i - 1);

                if (points [i].x == points [s].x)
                {
                    _ui_gfx_fill_rect (
                        context,
                        pen,
                        points [s].x,
                        MIN (points [s].y, points [i].y),
                        1,
                        (MAX (points [s].y, points [i].y) -
                         MIN (points [s].y, points [i].y)));
                }
                else if (points [i].y == points [s].y)
                {
                    _ui_gfx_fill_rect (
                        context,
                        pen,
                        MIN (points [s].x, points [i].x),
                        points [s].y,
                        (MAX (points [s].x, points [i].x) -
                         MIN (points [s].x, points [i].x)),
                        1);
                }
                else
                {
                    _uint_gfx_set_line_alpha (
                        context,
                        (unsigned char)(VoidToInt) pen,
                        points [s].x,
                        points [s].y,
                        points [i].x,
                        points [i].y);
                }
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_polygon
| Purpose:  Draw the given polygon using the given pen
| Input:    context             - the context
|           pen                 - the pen
|           points              - the points to link
|           count               - the number of points
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_polygon (ui_gfx_t *context, void *pen,
                                      const ui_point_t *points, int count)
{
#ifdef UI_SYSTEM_NT

    POINT  *dyn_points;
    int     i, s;
    void   *sys_pen;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_polygon (context, pen, points, count, TRUE, FALSE)
            != UI_SUCCESS)
    {
        if (pen <= (void *) 0 ||
            pen > (void *) 255)
        {
            sys_pen = _uint_gfx_use_pen (context, &pen);

            PRO_CREATE_AND_LOCK_STATIC_BUFFER (dyn_points, 16, count);
            {
                for (i = 0; i < count; i++)
                {
                    dyn_points [i].x = points [i].x;
                    dyn_points [i].y = points [i].y;
                }

                MoveToEx (
                    (HDC) context->context,
                    dyn_points [count - 1].x,
                    dyn_points [count - 1].y,
                    NULL);

                PolylineTo ((HDC) context->context, dyn_points, count);
            }
            PRO_UNLOCK_STATIC_BUFFER (dyn_points);

            SelectPen ((HDC) context->context, (HPEN) sys_pen);

            if (pen != NULL)
            {
                DeletePen ((HPEN) pen);
            }

            pen = (void *) 255;
        }

        if (context->depth >= 32 &&
            count > 1)
        {
            for (i = 0; i < count; i++)
            {
                s = ((i > 0) ? (i - 1) : (count - 1));

                if (points [i].x == points [s].x)
                {
                    _ui_gfx_fill_rect (
                        context,
                        pen,
                        points [s].x,
                        MIN (points [s].y, points [i].y),
                        1,
                        (MAX (points [s].y, points [i].y) -
                         MIN (points [s].y, points [i].y)));
                }
                else if (points [i].y == points [s].y)
                {
                    _ui_gfx_fill_rect (
                        context,
                        pen,
                        MIN (points [s].x, points [i].x),
                        points [s].y,
                        (MAX (points [s].x, points [i].x) -
                         MIN (points [s].x, points [i].x)),
                        1);
                }
                else
                {
                    _uint_gfx_set_line_alpha (
                        context,
                        (unsigned char)(VoidToInt) pen,
                        points [s].x,
                        points [s].y,
                        points [i].x,
                        points [i].y);
                }
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fill_polygon
| Purpose:  Fill the given polygon using the given brush
| Input:    context             - the context
|           brush               - the brush
|           points              - the points to link
|           count               - the number of points
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_fill_polygon (ui_gfx_t *context, void *brush,
                                      const ui_point_t *points, int count)
{
#ifdef UI_SYSTEM_NT

    POINT  *dyn_points;
    int     i;
    void   *sys_brush;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_polygon (context, brush, points, count, TRUE, TRUE)
            != UI_SUCCESS)
    {
        sys_brush = _uint_gfx_use_brush (context, &brush);

        PRO_CREATE_AND_LOCK_STATIC_BUFFER (dyn_points, 16, count);
        {
            for (i = 0; i < count; i++)
            {
                dyn_points [i].x = points [i].x;
                dyn_points [i].y = points [i].y;
            }

            Polygon ((HDC) context->context, dyn_points, count);
        }
        PRO_UNLOCK_STATIC_BUFFER (dyn_points);

        SelectBrush ((HDC) context->context, (HBRUSH) sys_brush);

        if (brush != NULL)
        {
            DeleteBrush ((HBRUSH) brush);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_rects
| Purpose:  Draw the given rectangles using the given pen
| Input:    context             - the context
|           pen                 - the pen
|           rects               - the rectangles to draw
|           count               - the number of rectangles
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_rects (ui_gfx_t *context, void *pen,
                                    const ui_rect_t *rects, int count)
{
#ifdef UI_SYSTEM_NT

    void       *sys_pen;
    int         i;
    ui_rect_t   rects32 [4];

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_rects (context, pen, rects, count, FALSE)
            != UI_SUCCESS)
    {
        if (pen <= (void *) 0 ||
            pen > (void *) 255)
        {
            sys_pen = _uint_gfx_use_pen (context, &pen);

            for (i = 0; i < count; i++)
            {
                Rectangle (
                    (HDC) context->context,
                    rects [i].x,
                    rects [i].y,
                    (rects [i].x + rects [i].width),
                    (rects [i].y + rects [i].height));
            }

            SelectPen ((HDC) context->context, (HPEN) sys_pen);

            if (pen != NULL)
            {
                DeletePen ((HPEN) pen);
            }

            pen = (void *) 255;
        }

        if (context->depth >= 32)
        {
            for (i = 0; i < count; i++)
            {
                rects32 [0] = rects [i];
                rects32 [0].height = 1;

                rects32 [1] = rects [i];
                rects32 [1].width = 1;

                rects32 [2] = rects32 [1];
                rects32 [2].x += (rects [i].width - 1);

                rects32 [3] = rects32 [0];
                rects32 [3].y += (rects [i].height - 1);

                _uint_gfx_fill_rects (context, pen, rects32, 4);
            }
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fill_rects
| Purpose:  Fill the given rectangles using the given brush
| Input:    context             - the context
|           brush               - the brush
|           rects               - the rectangles to fill
|           count               - the number of rectangles
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_fill_rects (ui_gfx_t *context, void *brush,
                                    const ui_rect_t *rects, int count)
{
#ifdef UI_SYSTEM_NT

    void       *sys_brush;
    int         i;
    RGNDATA    *region_data;
    RECT       *region_rects;
    HRGN        region;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_rects (context, brush, rects, count, TRUE)
            != UI_SUCCESS)
    {
        if (brush <= (void *) 0 ||
            brush > (void *) 255)
        {
            sys_brush = _uint_gfx_use_brush (context, &brush);

            for (i = 0; i < count; i++)
            {
                Rectangle (
                    (HDC) context->context,
                    rects [i].x,
                    rects [i].y,
                    (rects [i].x + rects [i].width + 1),
                    (rects [i].y + rects [i].height + 1));
            }

            SelectBrush ((HDC) context->context, (HBRUSH) sys_brush);

            if (brush != NULL)
            {
                DeleteBrush ((HBRUSH) brush);
            }

            brush = (void *) 255;
        }

        if (context->depth >= 32)
        {
            region_data =
                (RGNDATA *)
                    getmem (
                        sizeof (RGNDATAHEADER) +
                        (sizeof (RECT) * count));

            region_data->rdh.dwSize = sizeof (RGNDATAHEADER);
            region_data->rdh.iType = RDH_RECTANGLES;
            region_data->rdh.nCount = count;
            region_data->rdh.nRgnSize = (sizeof (RECT) * count);

            region_rects = (RECT *) &(region_data->Buffer);

            SetRect (
                &(region_data->rdh.rcBound),
                rects [0].x,
                rects [0].y,
                (rects [0].x + rects [0].width),
                (rects [0].y + rects [0].height));

            region_rects [0] = region_data->rdh.rcBound;

            for (i = 1; i < count; i++)
            {
                SetRect (
                    &(region_rects [i]),
                    rects [i].x,
                    rects [i].y,
                    (rects [i].x + rects [i].width),
                    (rects [i].y + rects [i].height));

                region_data->rdh.rcBound.left =
                    MIN (
                        region_data->rdh.rcBound.left,
                        region_rects [i].left);

                region_data->rdh.rcBound.top =
                    MIN (
                        region_data->rdh.rcBound.top,
                        region_rects [i].top);

                region_data->rdh.rcBound.right =
                    MAX (
                        region_data->rdh.rcBound.right,
                        region_rects [i].right);

                region_data->rdh.rcBound.bottom =
                    MAX (
                        region_data->rdh.rcBound.bottom,
                        region_rects [i].bottom);
            }

            region =
                ExtCreateRegion (
                    (XFORM *) NULL,
                    (DWORD)
                        (region_data->rdh.dwSize +
                         region_data->rdh.nRgnSize),
                    region_data);

            relmem (&region_data);

            _uint_gfx_set_alpha (
                context,
                (unsigned char) ((VoidToInt) brush),
                (void *) region);

            DeleteRgn (region);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_ellipses
| Purpose:  Draw the given ellipses using the given pen
| Input:    context             - the context
|           pen                 - the pen
|           rects               - the bounding rectangles of the ellipses
|           count               - the number of ellipses
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_ellipses (ui_gfx_t *context, void *pen,
                                       const ui_rect_t *rects, int count)
{
#ifdef UI_SYSTEM_NT

    int     i;
    void   *sys_pen;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_ellipses (context, pen, rects, count, FALSE)
            != UI_SUCCESS)
    {
        sys_pen = _uint_gfx_use_pen (context, &pen);

        for (i = 0; i < count; i++)
        {
            Arc ((HDC) context->context,
                 rects [i].x,
                 rects [i].y,
                 (rects [i].x + rects [i].width),
                 (rects [i].y + rects [i].height),
                 (rects [i].x + rects [i].width),
                 (rects [i].y + (rects [i].height / 2)),
                 (rects [i].x + rects [i].width),
                 (rects [i].y + (rects [i].height / 2)));
        }

        SelectPen ((HDC) context->context, (HPEN) sys_pen);

        if (pen != NULL)
        {
            DeletePen ((HPEN) pen);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fill_ellipses
| Purpose:  Fill the given ellipses using the given brush
| Input:    context             - the context
|           brush               - the brush
|           rects               - the bounding rectangles of the ellipses
|           count               - the number of ellipses
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_fill_ellipses (ui_gfx_t *context, void *brush,
                                       const ui_rect_t *rects, int count)
{
#ifdef UI_SYSTEM_NT

    int     i;
    void   *sys_brush;
    void   *region, *ellipse_region;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_ellipses (context, brush, rects, count, TRUE)
            != UI_SUCCESS)
    {
        if (brush <= (void *) 0 ||
            brush > (void *) 255)
        {
            sys_brush = _uint_gfx_use_brush (context, &brush);

            for (i = 0; i < count; i++)
            {
                Ellipse (
                    (HDC) context->context,
                    rects [i].x,
                    rects [i].y,
                    (rects [i].x + rects [i].width),
                    (rects [i].y + rects [i].height));
            }

            SelectBrush ((HDC) context->context, (HBRUSH) sys_brush);

            if (brush != NULL)
            {
                DeleteBrush ((HBRUSH) brush);
            }

            brush = (void *) 255;
        }

        if (context->depth >= 32)
        {
            _uint_gfx_region_create_from_ellipse (&region, rects);

            for (i = 1; i < count; i++)
            {
                _uint_gfx_region_create_from_ellipse (
                    &ellipse_region,
                    (rects + i));

                _uint_gfx_region_combine (&region, region, ellipse_region, UI_OR);

                _uint_gfx_region_destroy (&ellipse_region);
            }

            _uint_gfx_set_alpha (
                context,
                (unsigned char) ((VoidToInt) brush),
                region);

            _uint_gfx_region_destroy (&region);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_arcs
| Purpose:  Draw the given arcs using the given pen
| Input:    context             - the context
|           pen                 - the pen
|           arcs                - the arcs
|           count               - the number of arcs
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_arcs (ui_gfx_t *context, void *pen,
                                   const ui_arc_t *arcs, int count)
{
#ifdef UI_SYSTEM_NT

    int     i;
    void   *sys_pen;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_arcs (context, pen, arcs, count, FALSE)
            != UI_SUCCESS)
    {
        sys_pen = _uint_gfx_use_pen (context, &pen);

        for (i = 0; i < count; i++)
        {
            Arc (
                (HDC) context->context,
                arcs [i].rect.x,
                arcs [i].rect.y,
                (arcs [i].rect.x + arcs [i].rect.width),
                (arcs [i].rect.y + arcs [i].rect.height),
                arcs [i].start.x,
                arcs [i].start.y,
                arcs [i].end.x,
                arcs [i].end.y);
        }

        SelectPen ((HDC) context->context, (HPEN) sys_pen);

        if (pen != NULL)
        {
            DeletePen ((HPEN) pen);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fill_arcs
| Purpose:  Fill the given arcs using the given brush
| Input:    context             - the context
|           brush               - the brush
|           arcs                - the arcs
|           count               - the number of arcs
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_fill_arcs (ui_gfx_t *context, void *brush,
                                   const ui_arc_t *arcs, int count)
{
#ifdef UI_SYSTEM_NT

    int     i;
    void   *sys_brush;
    void   *region, *arc_region;
    int     mode, direction;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_arcs (context, brush, arcs, count, TRUE)
            != UI_SUCCESS)
    {
        if (brush <= (void *) 0 ||
            brush > (void *) 255)
        {
            sys_brush = _uint_gfx_use_brush (context, &brush);

            if (UI_GFX_HAS_FLAG (context, UI_GFX_PIE_FLAG))
            {
                /*
                ** UI_PIE
                */

                for (i = 0; i < count; i++)
                {
                    Pie (
                        (HDC) context->context,
                        arcs [i].rect.x,
                        arcs [i].rect.y,
                        (arcs [i].rect.x + arcs [i].rect.width),
                        (arcs [i].rect.y + arcs [i].rect.height),
                        arcs [i].start.x,
                        arcs [i].start.y,
                        arcs [i].end.x,
                        arcs [i].end.y);
                }
            }
            else
            {
                /*
                ** UI_CHORD
                */

                for (i = 0; i < count; i++)
                {
                    Chord (
                        (HDC) context->context,
                        arcs [i].rect.x,
                        arcs [i].rect.y,
                        (arcs [i].rect.x + arcs [i].rect.width),
                        (arcs [i].rect.y + arcs [i].rect.height),
                        arcs [i].start.x,
                        arcs [i].start.y,
                        arcs [i].end.x,
                        arcs [i].end.y);
                }
            }

            SelectBrush ((HDC) context->context, (HBRUSH) sys_brush);

            if (brush != NULL)
            {
                DeleteBrush ((HBRUSH) brush);
            }

            brush = (void *) 255;
        }

        if (context->depth >= 32)
        {
            mode =
                (UI_GFX_HAS_FLAG (context, UI_GFX_PIE_FLAG) ?
                 UI_PIE :
                 UI_CHORD);

            direction =
                ((GetArcDirection ((HDC) context->context) == AD_CLOCKWISE) ?
                 UI_CLOCKWISE :
                 UI_COUNTERCLOCKWISE);

            _uint_gfx_region_create_from_arc (&region, arcs, mode, direction);

            for (i = 1; i < count; i++)
            {
                _uint_gfx_region_create_from_arc (
                    &arc_region,
                    (arcs + i),
                    mode,
                    direction);

                _uint_gfx_region_combine (&region, region, arc_region, UI_OR);

                _uint_gfx_region_destroy (&arc_region);
            }

            _uint_gfx_set_alpha (
                context,
                (unsigned char) ((VoidToInt) brush),
                region);

            _uint_gfx_region_destroy (&region);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_frame_rect
| Purpose:  Draw a frame around the given rectangle of the given thickness,
|           using the given pen
| Input:    context             - the context
|           pen                 - the pen
|           rect                - the rectangle
|           thickness           - the thickness of the frame
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_frame_rect (ui_gfx_t *context, void *pen,
                                         const ui_rect_t *rect,
                                         int thickness)
{
#ifdef UI_SYSTEM_NT

    void       *sys_pen;
    RECT        frame_rect;
    int         i;
    ui_rect_t   rects32 [4];

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_frame (context, pen, rect, thickness) != UI_SUCCESS)
    {
        if (pen <= (void *) 0 ||
            pen > (void *) 255)
        {
            sys_pen = _uint_gfx_use_pen (context, &pen);

            SetRect (
                &frame_rect,
                rect->x,
                rect->y,
                (rect->x + rect->width),
                (rect->y + rect->height));

            for (i = 0; i < thickness; i++)
            {
                Rectangle (
                    (HDC) context->context,
                    frame_rect.left,
                    frame_rect.top,
                    frame_rect.right,
                    frame_rect.bottom);

                frame_rect.left++;
                frame_rect.top++;
                frame_rect.right--;
                frame_rect.bottom--;
            }

            SelectPen ((HDC) context->context, (HPEN) sys_pen);

            if (pen != NULL)
            {
                DeletePen ((HPEN) pen);
            }

            pen = (void *) 255;
        }

        if (context->depth >= 32)
        {
            rects32 [0] = *rect;
            rects32 [0].height = thickness;

            rects32 [1] = *rect;
            rects32 [1].width = thickness;

            rects32 [2] = rects32 [1];
            rects32 [2].x += (rect->width - thickness);

            rects32 [3] = rects32 [0];
            rects32 [3].y += (rect->height - thickness);

            _uint_gfx_fill_rects (context, pen, rects32, 4);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_draw_focus_rect
| Purpose:  Draw the given rectangle in the style used to indicate that the
|           rectangle has the focus
| Input:    context             - the context
|           rect                - the rectangle
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_draw_focus_rect (ui_gfx_t *context,
                                         const ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    RECT        focus_rect;
    COLORREF    fg, bg;

    _uint_d2d_renderer_destroy ((HDC) context->context);

    SetRect (
        &focus_rect,
        rect->x,
        rect->y,
        (rect->x + rect->width),
        (rect->y + rect->height));

    fg = SetTextColor ((HDC) context->context, COLOR_BLACK);
    bg = SetBkColor ((HDC) context->context, COLOR_WHITE);

    DrawFocusRect ((HDC) context->context, &focus_rect);

    SetTextColor ((HDC) context->context, fg);
    SetBkColor ((HDC) context->context, bg);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fill_region
| Purpose:  Fill the given region using the given brush
| Input:    context             - the context
|           brush               - the brush
|           region              - the region
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_fill_region (ui_gfx_t *context, void *brush,
                                     void *region)
{
#ifdef UI_SYSTEM_NT

    void   *sys_brush;

    if (!(_ui_gfx_is_accelerated (context)) ||
        _uint_gfx_d2d_region (context, brush, region) != UI_SUCCESS)
    {
        if (brush <= (void *) 0 ||
            brush > (void *) 255)
        {
            sys_brush = _uint_gfx_use_brush (context, &brush);

            sys_brush =
                (void *)
                    SelectBrush ((HDC) context->context, (HBRUSH) sys_brush);

            FillRgn (
                (HDC) context->context,
                (HRGN) region,
                (HBRUSH) sys_brush);

            if (brush != NULL)
            {
                DeleteBrush ((HBRUSH) brush);
            }

            brush = (void *) 255;
        }

        if (context->depth >= 32)
        {
            _uint_gfx_set_alpha (
                context,
                (unsigned char) ((VoidToInt) brush),
                region);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_invert_rect
| Purpose:  Invert the given rectangle
| Input:    context             - the context
|           rect                - the rectangle
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_invert_rect (ui_gfx_t *context,
                                     const ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    void   *region = _ui_gfx_region_create_from_rect (rect);

    _uint_gfx_invert_region (context, region);

    _uint_gfx_region_destroy (&region);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_invert_region
| Purpose:  Invert the given region
| Input:    context             - the context
|           region              - the region
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_invert_region (ui_gfx_t *context, void *region)
{
#ifdef UI_SYSTEM_NT

    _uint_d2d_renderer_destroy ((HDC) context->context);

    InvertRgn ((HDC) context->context, (HRGN) region);

    if (context->depth >= 32)
    {
        _uint_gfx_set_alpha (context, (unsigned char) 255, region);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fill_gradient_rect
| Purpose:  Fill the given rectangle with a gradient between the given colors
| Input:    context             - the context
|           rect                - the rectangle
|           clip_region         - the region within which to clip
|           orientation         - the orientation
|           first_color         - the left / top color
|           last_color          - the right / bottom color
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_fill_gradient_rect (ui_gfx_t *context,
                                            const ui_rect_t *rect,
                                            void *clip_region,
                                            int orientation,
                                            ui_rgb_t first_color,
                                            ui_rgb_t last_color)
{
#ifdef UI_SYSTEM_NT

    HRGN            region;
    TRIVERTEX       points [2];
    GRADIENT_RECT   gradient = { 0UL, 1UL };
    int             x, y;
    unsigned int    r, g, b, a, a1, a2;
    COLORREF        c1, c2, c;
    void           *bitmap;
    ui_gfx_t        mem_context;
    POINT           origin;
    HBRUSH          brush, sys_brush;
    void           *meta_region;
    int             status;

    if (_ui_gfx_is_accelerated (context))
    {
        status = UI_ERROR;
    }
    else
    {
        _uint_d2d_renderer_destroy ((HDC) context->context);

        if (clip_region != NULL ||
            context->region != NULL)
        {
            region = CreateRectRgn (0, 0, 0, 0);

            if (GetClipRgn ((HDC) context->context, region) != 1)
            {
                DeleteRgn (region);
                region = (HRGN) NULL;
            }

            if (clip_region != NULL)
            {
                _uint_gfx_set_clip_region (context, clip_region, UI_AND);
            }

            if (context->region != NULL)
            {
                _uint_gfx_set_clip_region (context, context->region, UI_AND);
            }
        }

        c1 = (COLORREF) _ui_color_get_color (first_color);
        c2 = (COLORREF) _ui_color_get_color (last_color);

        points [0].x = (LONG) rect->x;
        points [0].y = (LONG) rect->y;
        points [0].Red = ((COLOR16) GetRValue (c1) << 8);
        points [0].Green = ((COLOR16) GetGValue (c1) << 8);
        points [0].Blue = ((COLOR16) GetBValue (c1) << 8);
        points [0].Alpha = 0;

        points [1].x = (LONG) (rect->x + rect->width);
        points [1].y = (LONG) (rect->y + rect->height);
        points [1].Red = ((COLOR16) GetRValue (c2) << 8);
        points [1].Green = ((COLOR16) GetGValue (c2) << 8);
        points [1].Blue = ((COLOR16) GetBValue (c2) << 8);
        points [1].Alpha = 0;

        if (GradientFill (
                (HDC) context->context, points, 2, (VOID *) &gradient, 1,
                ((orientation == UI_HORIZONTAL) ?
                 GRADIENT_FILL_RECT_H :
                 GRADIENT_FILL_RECT_V)))
        {
            status = UI_SUCCESS;
        }
        else if (rect->width < 0 ||
                 rect->height < 0)
        {
            status = UI_ERROR;
        }
        else
        {
            if (orientation == UI_HORIZONTAL)
            {
                _uint_gfx_create_bitmap (context, rect->width, 1, &bitmap);
            }
            else
            {
                _uint_gfx_create_bitmap (context, 1, rect->height, &bitmap);
            }

            mem_context.flags = 0;

            _uint_gfx_create_context (context, bitmap, &mem_context);

            if (orientation == UI_HORIZONTAL)
            {
                a = MAX ((rect->width - 1), 1);

                for (x = 0; x < rect->width; x++)
                {
                    a2 = ((x * 0xffff) / a);
                    a1 = (0xffff - a2);

                    r = ((((GetRValue (c1) * a1) +
                           (GetRValue (c2) * a2)) /
                          0xffff) &
                         255);

                    g = ((((GetGValue (c1) * a1) +
                           (GetGValue (c2) * a2)) /
                          0xffff) &
                         255);

                    b = ((((GetBValue (c1) * a1) +
                           (GetBValue (c2) * a2)) /
                          0xffff) &
                         255);

                    c = RGB (r, g, b);

                    SetPixelV ((HDC) mem_context.context, x, 0, c);
                }
            }
            else
            {
                a = MAX ((rect->height - 1), 1);

                for (y = 0; y < rect->height; y++)
                {
                    a2 = ((y * 0xffff) / a);
                    a1 = (0xffff - a2);

                    r = ((((GetRValue (c1) * a1) +
                           (GetRValue (c2) * a2)) /
                          0xffff) &
                         255);

                    g = ((((GetGValue (c1) * a1) +
                           (GetGValue (c2) * a2)) /
                          0xffff) &
                         255);

                    b = ((((GetBValue (c1) * a1) +
                           (GetBValue (c2) * a2)) /
                          0xffff) &
                         255);

                    c = RGB (r, g, b);

                    SetPixelV ((HDC) mem_context.context, 0, y, c);
                }
            }

            _uint_gfx_destroy_context (&mem_context);

            SetBrushOrgEx ((HDC) context->context, rect->x, rect->y, &origin);

            brush = CreatePatternBrush (bitmap);
            sys_brush = SelectBrush ((HDC) context->context, brush);

            PatBlt (
                (HDC) context->context,
                rect->x,
                rect->y,
                rect->width,
                rect->height,
                PATCOPY);

            SelectBrush ((HDC) context->context, sys_brush);
            DeleteBrush (brush);

            SetBrushOrgEx (
                (HDC) context->context,
                origin.x,
                origin.y,
                (POINT *) NULL);

            _uint_gfx_destroy_bitmap (&bitmap);

            status = UI_SUCCESS;
        }

        if (status == UI_SUCCESS &&
            context->depth >= 32)
        {
            meta_region = _ui_gfx_region_create_from_rect (rect);

            if (clip_region != NULL)
            {
                _uint_gfx_region_combine (
                    &meta_region,
                    meta_region,
                    clip_region,
                    UI_AND);
            }

            _uint_gfx_set_alpha (context, 255, meta_region);

            _uint_gfx_region_destroy (&meta_region);
        }

        if (clip_region != NULL ||
            context->region != NULL)
        {
            SelectClipRgn ((HDC) context->context, region);

            if (region != (HRGN) NULL)
            {
                DeleteRgn (region);
            }
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_region
| Purpose:  Define the region for the given window
| Input:    window              - the window
|           region              - the region
|           width               - the width to use for the window region
|           height              - the height to use for the window region
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_region (void *window, void *region, int width,
                                    int height)
{
#ifdef UI_SYSTEM_NT

    RECT        region_rect;
    DWORD       style;
    ui_rect_t   window_rect, screen_rect;

    /*
    ** If the region is the rectangle of the window, then act as if no region
    ** had been passed
    */

    if (region != NULL &&
        width > 0 &&
        height > 0 &&
        GetRgnBox ((HRGN) region, &region_rect) == SIMPLEREGION &&
        region_rect.left == 0 &&
        region_rect.top == 0 &&
        region_rect.right == width &&
        region_rect.bottom == height)
    {
        _uint_gfx_region_destroy (&region);
    }

    _ui_gfx_set_layered (window, 0, 0);

    if (width > 0 &&
        height > 0 &&
        region == NULL &&
        ((style = GetWindowStyle ((HWND) window)) & WS_CAPTION))
    {
        /*
        ** If the window has a caption then a null region corresponds to
        ** the entire rectangle, rather than the shape provided by Windows
        */

        if (style & WS_MAXIMIZE)
        {
            /*
            ** If the window is maximized then define the region to cover
            ** only the area of the monitor on which the window is displayed
            */

            SetRectEmpty (&region_rect);

            AdjustWindowRectEx (
                &region_rect,
                style,
                (GetMenu ((HWND) window) != (HMENU) NULL),
                GetWindowExStyle ((HWND) window));

            region_rect.top = -region_rect.bottom;

            window_rect.x = -region_rect.left;
            window_rect.y = -region_rect.top;
            window_rect.width = (region_rect.right - region_rect.left);
            window_rect.height = (region_rect.bottom - region_rect.top);

            GetWindowRect ((HWND) window, &region_rect);

            window_rect.x = region_rect.left;
            window_rect.y = region_rect.top;
            window_rect.width += width;
            window_rect.height += height;

            _ui_monitor_workspace_from_rect (&window_rect, &screen_rect);

            SetRect (
                &region_rect,
                (MAX (screen_rect.x, window_rect.x) - window_rect.x),
                (MAX (screen_rect.y, window_rect.y) - window_rect.y),
                (MIN (
                     (screen_rect.x + screen_rect.width),
                     (window_rect.x + window_rect.width)) -
                 window_rect.x),
                (MIN (
                     (screen_rect.y + screen_rect.height),
                     (window_rect.y + window_rect.height)) -
                 window_rect.y));
        }
        else
        {
            SetRect (&region_rect, 0, 0, width, height);
        }

        region = (void *) CreateRectRgnIndirect (&region_rect);
    }

    _uint_set_window_region ((HWND) window, (HRGN) region, FALSE);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_layered
| Purpose:  Enable or disable layering support for the given window
| Input:    window              - the window
|           width               - the width to use for layering
|           height              - the height to use for layering
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_layered (void *window, int width, int height)
{
#ifdef UI_SYSTEM_NT

    LONG        style = GetWindowExStyle ((HWND) window);
    DWORD       flags = 0;
    BYTE        alpha = 255;
    RECT        rect;
    HRGN        region = (HRGN) NULL;
    ui_gfx_t    context;
    int         status = UI_SUCCESS;

    if (!(_uint_gfx_layered_windows ()))
    {
        status = UI_ERROR;
    }
    else if (window == NULL)
    {
        status = UI_SUCCESS;
    }
    else if (IsChildWindow ((HWND) window))
    {
        status = UI_ERROR;
    }
    else if (width > 0 &&
             height > 0)
    {
        if (!(style & WS_EX_LAYERED))
        {
            SetWindowExStyle ((HWND) window, (style | WS_EX_LAYERED));
        }
        else if (!(GetLayeredWindowAttributes (
                       (HWND) window, (COLORREF *) NULL, &alpha, &flags)) ||
                 !(flags & LWA_ALPHA))
        {
            alpha = 255;
        }

        if (IsMaximized ((HWND) window))
        {
            GetWindowRect ((HWND) window, &rect);

            width += (rect.right - rect.left);
            height += (rect.bottom - rect.top);

            GetClientRect ((HWND) window, &rect);

            width -= (rect.right - rect.left);
            height -= (rect.bottom - rect.top);
        }
        else if (GetWindowStyle ((HWND) window) & WS_CAPTION)
        {
            region = CreateRectRgn (0, 0, width, height);
        }

        _uint_set_window_region ((HWND) window, region, FALSE);

        _uint_gfx_get_layered_buffer ((HWND) window, width, height);

        _ui_gfx_get_context (window, TRUE, &context);

        _uint_send_message (
            (HWND) window,
            WM_PRINT,
            (WPARAM) context.context,
            (LPARAM) (PRF_CHILDREN | PRF_CLIENT | PRF_NONCLIENT));

        _uint_gfx_release_layered_context (&context, width, height);
    }
    else
    {
        if (style & WS_EX_LAYERED)
        {
            if (!(GetLayeredWindowAttributes (
                      (HWND) window, (COLORREF *) NULL, &alpha, &flags)) ||
                !(flags & LWA_ALPHA))
            {
                alpha = 255;
            }

            if (_uint_gfx_get_layered_buffer (
                    (HWND) window, UI_UNUSED, UI_UNUSED) != NULL)
            {
                SetWindowExStyle ((HWND) window, (style & ~WS_EX_LAYERED));
            }
        }

        _uint_gfx_get_layered_buffer ((HWND) window, 0, 0);
    }

    if (alpha < 255)
    {
        _uint_gfx_set_alpha_value (window, (unsigned int) alpha);
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_alpha_value
| Purpose:  Set the alpha value of the given window
| Input:    window              - the window
|           alpha               - the alpha value for the window
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_gfx_set_alpha_value (void *window, unsigned int alpha)
{
#ifdef UI_SYSTEM_NT

    unsigned int    current_alpha;
    ui_gfx_t        context;
    LONG            style;

    if (!(IsChildWindow ((HWND) window)) &&
        _uint_gfx_get_layered_buffer ((HWND) window, UI_UNUSED, UI_UNUSED)
            != NULL)
    {
        current_alpha =
            _uint_gfx_get_layered_alpha ((HWND) window, (unsigned int) -1);

        if (alpha != current_alpha)
        {
            _uint_gfx_get_layered_alpha ((HWND) window, alpha);

            _ui_gfx_get_context (window, TRUE, &context);
            _ui_gfx_release_context (&context);
        }
    }
    else
    {
        style = GetWindowExStyle ((HWND) window);

        if (alpha != 255u ||
            (style & WS_EX_LAYERED))
        {
            if (!(style & WS_EX_LAYERED))
            {
                SetWindowExStyle ((HWND) window, (style | WS_EX_LAYERED));
            }

            SetLayeredWindowAttributes (
                (HWND) window,
                (COLORREF) 0,
                (BYTE) alpha,
                LWA_ALPHA);
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fade_rect
| Purpose:  Use a secondary thread to fade out a bitmap rendered into a
|           rectangle on the screen
| Input:    bitmap              - the bitmap
|           rect                - the screen rectangle
|           duration            - the duration of the fade animation
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
int _uint_gfx_fade_rect (void *bitmap, const ui_rect_t *rect, int duration)
{
#ifdef UI_SYSTEM_NT

    nt_fade_t  *data = GETSTRUCT (nt_fade_t);
    HANDLE      event;
    HANDLE      thread_handle;
    DWORD       thread_id;
    int         status = UI_ERROR;

    data->position.x = rect->x;
    data->position.y = rect->y;
    data->size.cx = rect->width;
    data->size.cy = rect->height;
    data->bitmap = (HBITMAP) bitmap;
    data->alpha = 255;
    data->rate = 25;
    data->delta = ((data->alpha * data->rate) / duration);

    data->event =
        event =
            CreateEventA (
                (SECURITY_ATTRIBUTES *) NULL,
                FALSE,
                FALSE,
                (char *) NULL);

    thread_handle =
        CreateThread (
            (SECURITY_ATTRIBUTES *) NULL,
            1048576,
            (LPTHREAD_START_ROUTINE) _uint_gfx_fade_thread,
            data,
            0,
            &thread_id);

    if (event == (HANDLE) NULL ||
        thread_handle == (HANDLE) NULL)
    {
        relmem (&data);
    }
    else
    {
        WaitForSingleObject (event, INFINITE);

        status = UI_SUCCESS;
    }

    if (event != (HANDLE) NULL)
    {
        CloseHandle (event);
    }

    if (thread_handle != (HANDLE) NULL)
    {
        CloseHandle (thread_handle);
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_use_palette
| Purpose:  Use the given palette in the given context
| Input:    context             - the context
|           palette             - the palette
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_gfx_use_palette (ui_gfx_t *context, void *palette)
{
    if (palette != NULL &&
        (HPALETTE) palette != INVALID_HANDLE_VALUE)
    {
        SelectPalette ((HDC) context->context, (HPALETTE) palette, FALSE);
        RealizePalette ((HDC) context->context);
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_use_bitmap
| Purpose:  Use the given bitmap in the given context
| Input:    context             - the context
|           bitmap              - the bitmap
| Output:
| Return:   The previous bitmap used in the context
\*--------------------------------------------------------------------------*/
static void *_uint_gfx_use_bitmap (ui_gfx_t *context, void *bitmap)
{
    static HBITMAP  sys_bitmap = (HBITMAP) NULL;

    if (bitmap != NULL)
    {
        sys_bitmap = SelectBitmap ((HDC) context->context, (HBITMAP) bitmap);
        bitmap = (void *) sys_bitmap;
    }
    else
    {
        bitmap = (void *) SelectBitmap ((HDC) context->context, sys_bitmap);
    }

    return (bitmap);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_use_brush
| Purpose:  Use the given brush in the given context
| Input:    context             - the context
|           brush               - the brush
| Output:   brush               - the brush
| Return:   The system brush
\*--------------------------------------------------------------------------*/
static void *_uint_gfx_use_brush (ui_gfx_t *context, void **brush)
{
    void   *sys_brush;

    SelectPen ((HDC) context->context, GetStockPen (NULL_PEN));

    if (*brush != NULL)
    {
        sys_brush = *brush;
        *brush = NULL;
    }
    else if ((sys_brush = (void *) GetStockBrush (DC_BRUSH)) != NULL)
    {
        SetDCBrushColor (
            (HDC) context->context,
            (COLORREF) _uint_gfx_get_fg_color_value (context));
    }
    else
    {
        _uint_gfx_create_brush (
            context,
            UI_BAD_COLOR_RGB,
            UI_SOLID,
            &sys_brush);

        *brush = sys_brush;
    }

    return ((void *)
                SelectBrush ((HDC) context->context, (HBRUSH) sys_brush));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_use_pen
| Purpose:  Use the given pen in the given context
| Input:    context             - the context
|           pen                 - the pen
| Output:   pen                 - the pen
| Return:   The system pen
\*--------------------------------------------------------------------------*/
static void *_uint_gfx_use_pen (ui_gfx_t *context, void **pen)
{
    void   *sys_pen;

    SelectBrush ((HDC) context->context, GetStockBrush (NULL_BRUSH));

    if (*pen != NULL)
    {
        sys_pen = *pen;
        *pen = NULL;
    }
    else if ((sys_pen = (void *) GetStockPen (DC_PEN)) != NULL)
    {
        SetDCPenColor (
            (HDC) context->context,
            (COLORREF) _uint_gfx_get_fg_color_value (context));
    }
    else
    {
        _uint_gfx_create_pen (
            context,
            UI_BAD_COLOR_RGB,
            UI_SOLID,
            1,
            &sys_pen);

        *pen = sys_pen;
    }

    return ((void *) SelectPen ((HDC) context->context, (HPEN) sys_pen));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_region_create_from_bitmap
| Purpose:  Create a region from the given bitmap
| Input:    bitmap              - the bitmap
|           rect                - the rectangle
| Output:
| Return:   The region created from the bitmap
\*--------------------------------------------------------------------------*/
static HRGN _uint_gfx_region_create_from_bitmap (HBITMAP bitmap,
                                                 const ui_rect_t *rect)
{
    int             bpr;
    unsigned char  *data, *line, *ptr;
    int             x, y, w, m;
    int             count;
    RGNDATA        *region_data;
    RECT           *region_rects;
    HRGN            region;

    bpr = (((rect->width + 15) / 16) * 2);

    data = GET_ARRAY (unsigned char, (bpr * rect->height));
    GetBitmapBits (bitmap, (bpr * rect->height), data);

    for (line = data, count = y = 0; y < rect->height; y++, line += bpr)
    {
        for (ptr = line, x = w = 0; x < rect->width; ptr++)
        {
            for (m = (1 << 7); m > 0 && x < rect->width; m >>= 1, x++)
            {
                if (!(*ptr & m))
                {
                    w++;
                }
                else if (w > 0)
                {
                    count++;
                    w = 0;
                }
            }
        }

        if (w > 0)
        {
            count++;
            w = 0;
        }
    }

    region_data =
        (RGNDATA *)
            getmem (sizeof (RGNDATAHEADER) + (sizeof (RECT) * count));

    region_data->rdh.dwSize = sizeof (RGNDATAHEADER);
    region_data->rdh.iType = RDH_RECTANGLES;

    SetRect (&(region_data->rdh.rcBound), 0, 0, rect->width, rect->height);

    region_rects = (RECT *) &(region_data->Buffer);

    for (line = data, count = y = 0; y < rect->height; y++, line += bpr)
    {
        for (ptr = line, x = w = 0; x < rect->width; ptr++)
        {
            for (m = (1 << 7); m > 0 && x < rect->width; m >>= 1, x++)
            {
                if (!(*ptr & m))
                {
                    w++;
                }
                else if (w > 0)
                {
                    if (count > 0 &&
                        region_rects [count - 1].bottom == y &&
                        region_rects [count - 1].right == x &&
                        region_rects [count - 1].left == (x - w))
                    {
                        region_rects [count - 1].bottom++;
                    }
                    else
                    {
                        region_rects [count].left = (x - w);
                        region_rects [count].top = y;
                        region_rects [count].right = x;
                        region_rects [count].bottom = (y + 1);

                        count++;
                    }

                    w = 0;
                }
            }
        }

        if (w > 0)
        {
            if (count > 0 &&
                region_rects [count - 1].bottom == y &&
                region_rects [count - 1].right == x &&
                region_rects [count - 1].left == (x - w))
            {
                region_rects [count - 1].bottom++;
            }
            else
            {
                region_rects [count].left = (x - w);
                region_rects [count].top = y;
                region_rects [count].right = x;
                region_rects [count].bottom = (y + 1);

                count++;
            }

            w = 0;
        }
    }

    region_data->rdh.nCount = count;
    region_data->rdh.nRgnSize = (sizeof (RECT) * count);

    region =
        ExtCreateRegion (
            (XFORM *) NULL,
            (DWORD) (region_data->rdh.dwSize + region_data->rdh.nRgnSize),
            region_data);

    relmem (&region_data);

    OffsetRgn (region, rect->x, rect->y);

    relmem (&data);

    return (region);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_create_bitmap32
| Purpose:  Create a 32-bit DIB
| Input:    dc                  - the device context to use
|           width               - the width of the bitmap
|           height              - the height of the bitmap
| Output:   data                - the bitmap bits
| Return:   The buffer to use, or NULL if the window isn't layered
\*--------------------------------------------------------------------------*/
static HBITMAP _uint_gfx_create_bitmap32 (HDC dc, int width, int height,
                                          unsigned char **data, int *bpr)
{
    BITMAPV5INFO    color_info;
    nt_dib32_t      dib;

    dib.bpr = ((((width << 5) + 31) & ~31) >> 3);

    ZERO_OUT_STRUCT (color_info);

    color_info.bmiHeader.bV5Size = sizeof (color_info);
    color_info.bmiHeader.bV5Width = width;
    color_info.bmiHeader.bV5Height = -height;
    color_info.bmiHeader.bV5Planes = 1;
    color_info.bmiHeader.bV5BitCount = 32;
    color_info.bmiHeader.bV5Compression = BI_BITFIELDS;
    color_info.bmiHeader.bV5SizeImage = (dib.bpr * height);
    color_info.bmiHeader.bV5RedMask = 0x00ff0000U;
    color_info.bmiHeader.bV5GreenMask = 0x0000ff00U;
    color_info.bmiHeader.bV5BlueMask = 0x000000ffU;
    color_info.bmiHeader.bV5AlphaMask = 0xff000000U;
    color_info.bmiHeader.bV5CSType = LCS_DEVICE_CMYK;
    color_info.bmiHeader.bV5Intent = LCS_GM_IMAGES;

    color_info.bmiColors [0].rgbRed = 0xff;
    color_info.bmiColors [1].rgbGreen = 0xff;
    color_info.bmiColors [2].rgbBlue = 0xff;

    if ((dib.bitmap =
         CreateDIBSection (
             dc, (BITMAPINFO *) &color_info, DIB_RGB_COLORS,
             (VOID **) &(dib.data), (HANDLE) NULL, 0)) == (HBITMAP) NULL)
    {
        dib.data = (unsigned char *) NULL;
    }
    else if (dib.data == (unsigned char *) NULL)
    {
        DeleteBitmap (dib.bitmap);

        dib.bitmap = (HBITMAP) NULL;
    }
    else
    {
        GdiFlush ();

        memset (dib.data, 0, color_info.bmiHeader.bV5SizeImage);

        _uint_gfx_cache_bitmap32 (&dib);
    }

    INIT_ARG (data, dib.data);
    INIT_ARG (bpr, dib.bpr);

    return (dib.bitmap);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_destroy_bitmap32
| Purpose:  Destroy a 32-bit DIB
| Input:    bitmap              - the handle of the DIB
| Output:
| Return:   TRUE if the DIB was destroyed, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_gfx_destroy_bitmap32 (HBITMAP bitmap)
{
    nt_dib32_t  dib = { (HBITMAP) NULL, (unsigned char *) NULL, 1 };
    int         status;

    dib.bitmap = bitmap;

    if ((status = _uint_gfx_cache_bitmap32 (&dib)) == TRUE)
    {
        DeleteBitmap (bitmap);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_cache_bitmap32
| Purpose:  Cache the data for a 32-bit DIB, to be used later
| Input:    dib                 - the input DIB data
| Output:   dib                 - the output DIB data
| Return:   TRUE of FALSE, depending on the data in the DIB
\*--------------------------------------------------------------------------*/
static int _uint_gfx_cache_bitmap32 (nt_dib32_t *dib)
{
    static nt_dib32_t  *dib_cache = (nt_dib32_t *) NULL;
    int                 start, end, i;
    int                 status = FALSE;

    for (start = 0, end = (XAR_COUNT (&dib_cache) - 1); end >= start; )
    {
        i = ((start + end + 1) >> 1);

        if (dib->bitmap > dib_cache [i].bitmap)
        {
            start = (i + 1);
        }
        else if (dib->bitmap < dib_cache [i].bitmap)
        {
            end = (i - 1);
        }
        else
        {
            break;
        }
    }

    if (dib->data != (unsigned char *) NULL)
    {
        /*
        ** Store the data in the cache
        */

        if (end < start)
        {
            if (dib_cache == (nt_dib32_t *) NULL)
            {
                dib_cache = XAR_BEGIN (nt_dib32_t, 16);
            }

            XAR_INSERT (&dib_cache, start, 1, dib);
            status = TRUE;
        }
    }
    else if (dib->bpr != 0)
    {
        /*
        ** Remove the data from the cache
        */

        if (end >= start)
        {
            XAR_REMOVE (&dib_cache, i, 1);
            status = TRUE;
        }
    }
    else
    {
        /*
        ** Find the data in the cache
        */

        if (end >= start)
        {
            *dib = dib_cache [i];
            status = TRUE;
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_alpha
| Purpose:  Set the alpha value of the given region
| Input:    context             - the context
|           alpha               - the alpha value
|           region              - the region to modify
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_gfx_set_alpha (ui_gfx_t *context, unsigned char alpha,
                                 void *region)
{
    nt_dib32_t      dib = { (HBITMAP) NULL, (unsigned char *) NULL, 0 };
    POINT           origin = { 0 };
    DWORD           size;
    unsigned char  *buffer;
    RGNDATA        *data;
    RECT           *rect;
    DWORD           i;
    HRGN            clip_region;
    unsigned char  *line, *ptr;
    int             x, y;

    if ((dib.bitmap = GetCurrentObject ((HDC) context->context, OBJ_BITMAP))
        != (HBITMAP) NULL &&
        _uint_gfx_cache_bitmap32 (&dib))
    {
        clip_region = CreateRectRgn (0, 0, 0, 0);

        if (GetClipRgn ((HDC) context->context, clip_region) == 1)
        {
            if (DPtoLP ((HDC) context->context, &origin, 1) &&
                (origin.x != 0 ||
                 origin.y != 0))
            {
                OffsetRgn (clip_region, origin.x, origin.y);
            }

            IntersectRgn (clip_region, (HRGN) region, clip_region);
        }
        else
        {
            CopyRgn (clip_region, (HRGN) region);
        }

        if (context->region != NULL)
        {
            IntersectRgn (clip_region, (HRGN) context->region, clip_region);
        }

        if ((size = GetRegionData (clip_region, 0, (RGNDATA *) NULL)) > 0)
        {
            PRO_CREATE_AND_LOCK_STATIC_BUFFER (buffer, 1024, size);
            {
                if ((data = (RGNDATA *) buffer) != (RGNDATA *) NULL &&
                    GetRegionData (clip_region, size, data) &&
                    data->rdh.iType == RDH_RECTANGLES &&
                    data->rdh.nCount > 0)
                {
                    GetWindowOrgEx ((HDC) context->context, &origin);

                    GdiFlush ();

                    for (rect = (RECT *) data->Buffer, i = 0;
                         i < data->rdh.nCount;
                         i++, rect++)
                    {
                        for (line =
                                 (dib.data +
                                  ((rect->top - origin.y) * dib.bpr) +
                                  ((rect->left - origin.x) << 2)),
                             y = rect->top;
                             y < rect->bottom;
                             line += dib.bpr,
                             y++)
                        {
                            for (ptr = (line + 3),
                                 x = rect->left;
                                 x < rect->right;
                                 ptr += 4,
                                 x++)
                            {
                                *ptr = alpha;
                            }
                        }
                    }
                }
            }
            PRO_UNLOCK_STATIC_BUFFER (buffer);
        }

        DeleteRgn (clip_region);
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_line_alpha
| Purpose:  Set the alpha value of the given line
| Input:    context             - the context
|           alpha               - the alpha value
|           start_x             - the x position of the start of the line
|           start_y             - the y position of the start of the line
|           end_x               - the x position of the end of the line
|           end_y               - the y position of the end of the line
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_gfx_set_line_alpha (ui_gfx_t *context, unsigned char alpha,
                                      int start_x, int start_y, int end_x,
                                      int end_y)
{
    nt_linedda_data_t   data =
    {
        { (HBITMAP) NULL, (unsigned char *) NULL, 0 },
        (HRGN) NULL,
        0
    };

    POINT               origin = { 0 };

    if ((data.dib.bitmap =
         GetCurrentObject ((HDC) context->context, OBJ_BITMAP))
            != (HBITMAP) NULL &&
        _uint_gfx_cache_bitmap32 (&(data.dib)))
    {
        data.clip_region = CreateRectRgn (0, 0, 0, 0);

        if (GetClipRgn ((HDC) context->context, data.clip_region) == 1)
        {
            if (DPtoLP ((HDC) context->context, &origin, 1) &&
                (origin.x != 0 ||
                 origin.y != 0))
            {
                OffsetRgn (data.clip_region, origin.x, origin.y);
            }

            if (context->region != NULL)
            {
                IntersectRgn (
                    data.clip_region,
                    (HRGN) context->region,
                    data.clip_region);
            }
        }
        else if (context->region != NULL)
        {
            CopyRgn (data.clip_region, (HRGN) context->region);
        }
        else
        {
            DeleteRgn (data.clip_region);
            data.clip_region = (HRGN) NULL;
        }

        GetWindowOrgEx ((HDC) context->context, &origin);

        if (data.clip_region != (HRGN) NULL)
        {
            OffsetRgn (data.clip_region, -origin.x, -origin.y);
        }

        GdiFlush ();

        data.alpha = alpha;

        LineDDA (
            (start_x - origin.x),
            (start_y - origin.y),
            (end_x - origin.x),
            (end_y - origin.y),
            _uint_gfx_set_line_alpha_proc,
            (LPARAM) &data);

        if (data.clip_region != (HRGN) NULL)
        {
            DeleteRgn (data.clip_region);
        }
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_line_alpha_proc
| Purpose:  Set the alpha value of the given pixel
| Input:    x                   - the x position
|           y                   - the y position
|           dda_data            - the data to use to set the alpha value
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static VOID CALLBACK _uint_gfx_set_line_alpha_proc (int x, int y,
                                                    LPARAM dda_data)
{
    nt_linedda_data_t *data = (nt_linedda_data_t *) dda_data;

    if (x >= 0 &&
        y >= 0 &&
        (data->clip_region == (HRGN) NULL ||
         PtInRegion (data->clip_region, x, y)))
    {
        data->dib.data [(y * data->dib.bpr) + (x << 2) + 3] = data->alpha;
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_layered_alpha
| Purpose:  Get the alpha value to be used with the given layered window
| Input:    window              - the window
|           alpha               - the alpha value to use for layering
| Output:
| Return:   The alpha value to use
\*--------------------------------------------------------------------------*/
static unsigned int _uint_gfx_get_layered_alpha (HWND window,
                                                 unsigned int alpha)
{
    static ATOM alpha_atom = (ATOM) 0;

    if (alpha_atom == (ATOM) 0)
    {
        alpha_atom =
            _uint_app_window_add_atom (TEXT ("UINT Layered Window Alpha"));
    }

    if (alpha != (unsigned int) -1)
    {
        SetProp (
            window,
            MAKEINTATOM (alpha_atom),
            (HANDLE)(ptc_intptr) (alpha + 1));
    }
    else if ((alpha =
              (unsigned int)(ptc_intptr)
                  GetProp (window, MAKEINTATOM (alpha_atom))) == 0)
    {
        alpha = 255;
    }
    else
    {
        alpha--;
    }

    return (alpha);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_layered_buffer
| Purpose:  Get the buffer to be used with the given layered window
| Input:    window              - the window
|           width               - the width to use for layering
|           height              - the height to use for layering
| Output:
| Return:   The buffer to use, or NULL if the window isn't layered
\*--------------------------------------------------------------------------*/
static void *_uint_gfx_get_layered_buffer (HWND window, int width, int height)
{
    static ATOM     buffer_atom = (ATOM) 0;
    HDC             dc, old_dc, new_dc;
    BITMAP          old_data;
    void           *old_buffer = NULL, *buffer = NULL;

    if (width != UI_UNUSED &&
        height != UI_UNUSED)
    {
        if (buffer_atom != (ATOM) 0)
        {
            old_buffer =
                (void *) RemoveProp (window, MAKEINTATOM (buffer_atom));
        }

        if (width > 0 &&
            height > 0)
        {
            dc = GetDC (window);

            if ((buffer =
                 (void *)
                     _uint_gfx_create_bitmap32 (
                         dc, width, height, (unsigned char **) NULL,
                         (int *) NULL)) != NULL)
            {
                if (old_buffer != NULL &&
                    GetObject (
                        (HBITMAP) old_buffer, sizeof (BITMAP),
                        &old_data) > 0 &&
                    old_data.bmWidth > 0 &&
                    old_data.bmHeight > 0)
                {
                    old_dc = CreateCompatibleDC (dc);

                    old_buffer =
                        (void *) SelectBitmap (old_dc, (HBITMAP) old_buffer);

                    new_dc = CreateCompatibleDC (dc);

                    buffer = (void *) SelectBitmap (new_dc, (HBITMAP) buffer);

                    BitBlt (
                        new_dc,
                        0,
                        0,
                        MIN ((int) old_data.bmWidth, width),
                        MIN ((int) old_data.bmHeight, height),
                        old_dc,
                        0,
                        0,
                        SRCCOPY);

                    buffer = (void *) SelectBitmap (new_dc, (HBITMAP) buffer);

                    DeleteDC (new_dc);

                    old_buffer =
                        (void *) SelectBitmap (old_dc, (HBITMAP) old_buffer);

                    DeleteDC (old_dc);
                }

                if (buffer_atom == (ATOM) 0)
                {
                    buffer_atom =
                        _uint_app_window_add_atom (
                            TEXT ("UINT Layered Window Buffer"));
                }

                SetProp (window, MAKEINTATOM (buffer_atom), (HANDLE) buffer);
            }

            ReleaseDC (window, dc);
        }

        if (old_buffer != NULL)
        {
            _uint_gfx_destroy_bitmap32 ((HBITMAP) old_buffer);
        }
    }
    else if (buffer_atom != (ATOM) 0)
    {
        buffer = (void *) GetProp (window, MAKEINTATOM (buffer_atom));
    }

    return (buffer);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_layered_lock
| Purpose:  Determine whether the given layered window is locked for painting
| Input:    window              - the window
|           lock                - the flag indicating whether to lock the
|                                 window
| Output:
| Return:   TRUE if the window is locked, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_gfx_get_layered_lock (HWND window, int lock)
{
    static ATOM lock_atom = (ATOM) 0;

    if (lock_atom == (ATOM) 0)
    {
        lock_atom =
            _uint_app_window_add_atom (TEXT ("UINT Layered Window Lock"));
    }

    if (lock != -1)
    {
        SetProp (window, MAKEINTATOM (lock_atom), (HANDLE)(ptc_intptr) lock);
    }
    else
    {
        lock = (VoidToInt) GetProp (window, MAKEINTATOM (lock_atom));
    }

    return (lock);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_layered_context
| Purpose:  Get the context to be used with the given layered window
| Input:    allow_dirty     - TRUE to allow a dirty context
| Output:   context         - the context
| Return:   TRUE if the window is layered, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_gfx_get_layered_context (ui_gfx_t *context, int allow_dirty)
{
    HWND    parent = (HWND) context->window, popup;
    void   *buffer;
    HDC     dc;
    int     status = FALSE;

    /*
    ** Find the buffer bitmap for the popup window, if the popup is layered
    */

    if ((buffer = _uint_gfx_get_layered_data (parent, &popup)) == NULL)
    {
        status = FALSE;
    }
    else if (!allow_dirty &&
             GetUpdateRect (popup, (RECT *) NULL, FALSE))
    {
        /*
        ** Return a null context if the layered window is dirty so that no
        ** asynchronous drawing can be done until the window has been cleaned
        */

        context->context = NULL;

        status = TRUE;
    }
    else
    {
        /*
        ** Create the memory DC for the layer
        */

        dc = GetDC (popup);
        context->context = (void *) CreateCompatibleDC (dc);
        ReleaseDC (popup, dc);

        /*
        ** Set the depth of the context to reflect the layer
        */

        context->depth = 32;

        /*
        ** Set the region of the context
        */

        _uint_gfx_set_layered_region (
            context,
            (HBITMAP) buffer,
            popup,
            parent);

        /*
        ** Select the buffer bitmap into the DC for drawing
        */

        _uint_gfx_use_bitmap (context, buffer);

        /*
        ** Cache the context
        */

        _uint_gfx_layered_context_cache = *context;
        _uint_gfx_layered_window_cache = popup;

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_get_layered_data
| Purpose:  Get the layered buffer and popup window handle to be used with
|           a window
| Input:    window          - the window handle
| Output:   popup           - the popup window handle
| Return:   The layered buffer if the window is layered, otherwise NULL
\*--------------------------------------------------------------------------*/
static void *_uint_gfx_get_layered_data (HWND window, HWND *popup)
{
    HWND    parent;
    void   *buffer;

    /*
    ** Find the popup window ancestor which might be layered
    */

    for (parent = window;
         parent != (HWND) NULL &&
         IsChildWindow (parent);
         parent = GetAncestor (parent, GA_PARENT));

    /*
    ** Find the buffer bitmap for the popup, if the popup is layered
    */

    if ((buffer =
         _uint_gfx_get_layered_buffer (parent, UI_UNUSED, UI_UNUSED))
            == NULL ||
        GetWindowThreadId (parent) != GetCurrentThreadId () ||
        GetWindowInstanceU (parent) != GetWindowInstance (window))
    {
        buffer = NULL;
    }

    INIT_ARG (popup, parent);

    return (buffer);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_set_layered_region
| Purpose:  Set the region to be used on the given layered window context
| Input:    context         - the context
|           buffer          - the buffer of the layered window
|           popup           - the layered window
|           parent          - the window of the context
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_gfx_set_layered_region (ui_gfx_t *context, HBITMAP buffer,
                                          HWND popup, HWND parent)
{
    HWND    window;
    BITMAP  buffer_data;
    POINT   origin = { 0 };
    RECT    rect = { 0 }, parent_rect;

    /*
    ** Find the popup window ancestor which might be layered
    */

    if (parent == (HWND) NULL)
    {
        parent = (HWND) context->window;
    }

    if (popup == (HWND) NULL)
    {
        for (popup = parent;
             popup != (HWND) NULL &&
             IsChildWindow (popup);
             popup = GetAncestor (popup, GA_PARENT));
    }

    /*
    ** Get the bounding rectangle and origin for the context
    */

    _uint_gfx_get_rect (context->window, &(context->rect));

    if (parent != popup)
    {
        MapWindowPoint (parent, popup, &origin);
    }
    else if (IsMaximized (parent))
    {
        MapWindowPoint (parent, (HWND) NULL, &origin);

        GetWindowRect (parent, &rect);

        origin.x -= rect.left;
        origin.y -= rect.top;
    }

    SetWindowOrgEx ((HDC) context->context, 0, 0, (POINT *) NULL);

    /*
    ** Get the rectangle of the layered popup window
    */

    if (GetObject (buffer, sizeof (BITMAP), &buffer_data) > 0 &&
        buffer_data.bmWidth > 0 &&
        buffer_data.bmHeight > 0)
    {
        SetRect (
            &rect,
            0,
            0,
            (int) buffer_data.bmWidth,
            (int) buffer_data.bmHeight);

        if (context->window == (void *) popup)
        {
            context->rect.width = rect.right;
            context->rect.height = rect.bottom;
        }
    }
    else
    {
        GetClientRect (popup, &rect);
    }

    /*
    ** Clip the drawing region to the viewable area of the window
    */

    for (window = parent;
         parent != popup;
         parent = GetAncestor (parent, GA_PARENT))
    {
        if (IsChildWindowVisible (parent))
        {
            GetClientRect (parent, &parent_rect);
            MapWindowRect (parent, popup, &parent_rect);

            rect.left = MAX (rect.left, parent_rect.left);
            rect.top = MAX (rect.top, parent_rect.top);
            rect.right = MIN (rect.right, parent_rect.right);
            rect.bottom = MIN (rect.bottom, parent_rect.bottom);
        }
        else
        {
            rect.right = rect.left;
            rect.bottom = rect.top;
        }
    }

    rect.right = MAX (rect.left, rect.right);
    rect.bottom = MAX (rect.top, rect.bottom);

    context->region = (void *) CreateRectRgnIndirect (&rect);

    /*
    ** SelectClipRgn() should return NULLREGION iff the region is empty, but
    ** its behaviour has been found to not follow this rule.  Thus we also
    ** check directly whether the rectangle of the region is itself empty.
    **
    ** jas - 17-Feb-14
    */

    if (SelectClipRgn ((HDC) context->context, (HRGN) context->region)
            != NULLREGION ||
        !(IsRectEmpty (&rect)))
    {
        /*
        ** Clip the drawing region further, excluding any clipped children
        */

        if (!(_ui_gfx_includes_children (context)) &&
            (GetWindowStyle (window) & WS_CLIPCHILDREN))
        {
            for (parent = GetFirstChild (window);
                 parent != (HWND) NULL;
                 parent = GetNextSibling (parent))
            {
                if (IsChildWindowVisible (parent) &&
                    !(IsWindowTransparentU (parent)))
                {
                    GetClientRect (parent, &parent_rect);
                    MapWindowRect (parent, popup, &parent_rect);

                    ExcludeClipRect (
                        (HDC) context->context,
                        parent_rect.left,
                        parent_rect.top,
                        parent_rect.right,
                        parent_rect.bottom);
                }
            }
        }

        /*
        ** Clip the drawing region further, taking into account overlapping
        ** siblings within the hierarchy
        */

        for (parent = window;
             parent != popup;
             parent = GetAncestor (parent, GA_PARENT))
        {
            window = parent;

            while ((window = GetPrevSibling (window)) != (HWND) NULL)
            {
                if (IsChildWindowVisible (window))
                {
                    GetClientRect (window, &parent_rect);
                    MapWindowRect (window, popup, &parent_rect);

                    ExcludeClipRect (
                        (HDC) context->context,
                        parent_rect.left,
                        parent_rect.top,
                        parent_rect.right,
                        parent_rect.bottom);
                }
            }
        }
    }

    /*
    ** Offset the clipping region according to the origin of the window
    */

    GetClipRgn ((HDC) context->context, (HRGN) context->region);

    SetWindowOrgEx (
        (HDC) context->context,
        -origin.x,
        -origin.y,
        (POINT *) NULL);

    OffsetRgn ((HRGN) context->region, -origin.x, -origin.y);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_release_layered_context
| Purpose:  Release the given layered window context
| Input:    context             - the context
| Output:
| Return:   TRUE if the context applied to a layered window, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_gfx_release_layered_context (ui_gfx_t *context, int width,
                                              int height)
{
    static int      lock = 0;
    HWND            parent = (HWND) context->window, popup;
    RECT            rect;
    SIZE            size;
    void           *buffer;
    POINT           origin = { 0 };
    BLENDFUNCTION   blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    BOOL            updated;
    LONG            style;
    int             count, i;
    DWORD           flags = 0;
    int             status = FALSE;

    /*
    ** Determine whether the context is layered
    */

    if (context->drawable != NULL)
    {
        status = FALSE;
    }
    else if (context->window == _uint_gfx_layered_context_cache.window &&
             context->context == _uint_gfx_layered_context_cache.context)
    {
        popup = _uint_gfx_layered_window_cache;

        ZERO_OUT_STRUCT (_uint_gfx_layered_context_cache);

        _uint_gfx_layered_window_cache = (HWND) NULL;

        if (_uint_gfx_layered_region_cache != (HRGN) NULL)
        {
            DeleteRgn (_uint_gfx_layered_region_cache);

            _uint_gfx_layered_region_cache = (HRGN) NULL;
        }

        status = TRUE;
    }
    else if (_uint_gfx_get_layered_data (parent, &popup) != NULL)
    {
        status = TRUE;
    }

    /*
    ** If we found a layered context then release it
    */

    if (status)
    {
        /*
        ** Destroy the context region and reset the drawing origin
        */

        DeleteRgn ((HRGN) context->region);

        SetWindowOrgEx ((HDC) context->context, 0, 0, (POINT *) NULL);

        /*
        ** Validate the area of the window
        */

        ValidateRect ((HWND) context->window, (RECT *) NULL);

#if 0
        /*
        ** Save a PNG of the layer
        */

        {
            HBITMAP     b = GetCurrentObject ((HDC) context->context, OBJ_BITMAP);
            ui_image_t  image;

            if (ui_global_do_operation (
                    UI_read_sysdep_image_Op, (void *) b, &image) == UI_SUCCESS)
            {
                static int      fred = 0;
                char            filename [64];
                Pfa            *fp;
                unsigned char  *buffer;
                int             length;

                image.flags |= UI_IMAGE_PNG;

                sprintf (filename, "image%d.png", ++fred);

                if (ui_global_do_operation (
                        UI_write_image_data_Op, filename, &image, &buffer,
                        &length) != UI_SUCCESS)
                {
                    printf ("Error writing data\n");
                }
                else if ((fp =
                          pfa_fopen_file_vers (filename, "wb", THIS_VERSION))
                             == (Pfa *) NULL)
                {
                    relmem (&buffer);
                    printf ("Error opening file\n");
                }
                else
                {
                    pfa_write_file (fp, length, (char *) buffer);
                    pfa_dispose (&fp);

                    relmem (&buffer);
                }

                relmem (&(image.data));
            }
        }
#endif

        /*
        ** Deselect the buffer bitmap from the DC
        */

        buffer = _uint_gfx_use_bitmap (context, NULL);

        /*
        ** Lock the layer
        */

        if (!(_uint_gfx_get_layered_lock (popup, -1)))
        {
            /*
            ** If the window is not locked for painting, then lock it now
            ** before forcing all child windows to be updated, before finally
            ** unlocking it and updating the completed layer.
            **
            ** This improves performance by minimizing the number of calls to
            ** the graphics card to update the layer.
            **
            ** jas - 16-Nov-09
            **
            ** Perform at most 16 redraws before updating the layer, to guard
            ** against any code which results in looping redraws.
            **
            ** jas - 06-Jun-13
            */

            _uint_gfx_get_layered_lock (popup, TRUE);

            for (i = 16, count = !lock; i > 0 && count != lock; i--)
            {
                count = lock;

                RedrawWindow (
                    popup,
                    (RECT *) NULL,
                    (HRGN) NULL,
                    (RDW_UPDATENOW | RDW_ALLCHILDREN));
            }

            _uint_gfx_get_layered_lock (popup, FALSE);

            /*
            ** Select the buffer bitmap back into the DC
            */

            _uint_gfx_use_bitmap (context, buffer);

            /*
            ** Determine the size of the layer
            */

            if (width > 0 &&
                height > 0)
            {
                size.cx = width;
                size.cy = height;
            }
            else
            {
                GetWindowRect (popup, &rect);

                size.cx = (rect.right - rect.left);
                size.cy = (rect.bottom - rect.top);
            }

            /*
            ** Setup the alpha value of the layer
            */

            blend.SourceConstantAlpha =
                (BYTE) _uint_gfx_get_layered_alpha (popup, (unsigned int) -1);

            /*
            ** Update the layer
            */

            updated =
                UpdateLayeredWindow (
                    popup,
                    (HDC) NULL,
                    (POINT *) NULL,
                    &size,
                    (HDC) context->context,
                    &origin,
                    (COLORREF) 0,
                    &blend,
                    ULW_ALPHA);

            /*
            ** Deselect the buffer bitmap from the DC
            */

            _uint_gfx_use_bitmap (context, NULL);
        }
        else
        {
            lock++;

            updated = TRUE;
        }

        /*
        ** Reset the clipping region and drawing origin
        */

        SelectClipRgn ((HDC) context->context, (HRGN) NULL);

        SetWindowOrgEx ((HDC) context->context, 0, 0, (POINT *) NULL);

        /*
        ** Destroy the memory DC
        */

        DeleteDC ((HDC) context->context);

        /*
        ** Check that the layer was actually updated
        */

        if (!updated &&
            ((style = GetWindowExStyle (popup)) & WS_EX_LAYERED) &&
            GetLayeredWindowAttributes (
                popup, (COLORREF *) NULL, (BYTE *) NULL, &flags) &&
            flags != 0)
        {
            /*
            ** UpdateLayeredWindow will fail if SetLayeredWindowAttributes has
            ** been called on the window.  Clearing and setting the layered
            ** window style bit WS_EX_LAYERED is the documented technique to
            ** reset the behaviour
            **
            ** jas - 11-Nov-09
            */

            SetWindowExStyle (popup, (style & ~WS_EX_LAYERED));
            SetWindowExStyle (popup, style);
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_layered_windows
| Purpose:  Determine whether layered windows are enabled
| Input:
| Output:
| Return:   TRUE if layered windows are enabled, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_gfx_layered_windows (void)
{
    static int  enabled = UI_ERROR;
    char       *ptr;

    if (enabled == UI_ERROR)
    {
        if ((ptr = _ui_getenv ("UINT_LAYERED_WINDOWS")) != (char *) NULL)
        {
            enabled = (*ptr != 'f' && *ptr != 'F');
        }
        else
        {
            /*
            ** Unless the user specifies otherwise, use layered windows
            ** if we are not running as a remote app, or we are running on
            ** Windows 11 or later
            */

            enabled =
                (!(_ui_server_is_remote_app ()) ||
                 IsWindows11OrGreater ());
        }

        _ui_info_msg (
            UIMSG_UI,
            "%ssing layered windows for translucency",
            (enabled ? "U" : "Not u"));
    }

    return (enabled);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fade_thread
| Purpose:  Thread Procedure for the selection fading window
| Input:    user_data   - the data passed from the parent thread
| Output:
| Return:   0
\*--------------------------------------------------------------------------*/
static DWORD _uint_gfx_fade_thread (LPDWORD user_data)
{
#ifdef CreateCompatibleDC
#undef CreateCompatibleDC
#endif /* CreateCompatibleDC */

#ifdef DeleteDC
#undef DeleteDC
#endif /* DeleteDC */

    static ATOM     fade_class = 0;
    WNDCLASS        window_class;
    nt_fade_t      *data = (nt_fade_t *) user_data;
    BLENDFUNCTION   blend = { AC_SRC_OVER, 0, 0, 0 };
    HDC             device_context, mem_device_context;
    HBITMAP         sys_bitmap;
    MSG             message;

    if (fade_class == 0)
    {
        window_class.style = CS_SAVEBITS;
        window_class.lpfnWndProc = (WNDPROC) _uint_gfx_fade_wnd_proc;
        window_class.cbClsExtra = 0;
        window_class.cbWndExtra = 0;
        window_class.hInstance = _uint_get_current_instance ();
        window_class.hIcon = LoadIcon (NULL, IDI_APPLICATION);
        window_class.hCursor = LoadCursor (NULL, IDC_ARROW);
        window_class.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
        window_class.lpszMenuName = NULL;
        window_class.lpszClassName = TEXT ("Fade");

        fade_class = RegisterClass (&window_class);

#if 0
        if (fade_class != 0)
        {
            _uint_register_wndproc (
                (LONG_PTR) window_class.lpfnWndProc,
                fade_class);
        }
#endif
    }

    __try
    {
        data->window =
            CreateWindowEx (
                (WS_EX_LAYERED |
                 WS_EX_TRANSPARENT |
                 WS_EX_TOPMOST |
                 WS_EX_TOOLWINDOW),
                MAKEINTATOM (fade_class),
                NULL,
                WS_POPUP,
                data->position.x,
                data->position.y,
                data->size.cx,
                data->size.cy,
                _uint_get_app_window (),
                (HMENU) 0,
                _uint_get_current_instance (),
                NULL);

        if (data->window != (HWND) NULL)
        {
            SetWindowPos (
                data->window,
                (HWND) NULL,
                0,
                0,
                0,
                0,
                (SWP_NOACTIVATE |
                 SWP_NOMOVE |
                 SWP_NOSIZE |
                 SWP_NOZORDER |
                 SWP_SHOWWINDOW));

            device_context = GetDC (data->window);
            mem_device_context = CreateCompatibleDC (device_context);
            sys_bitmap = SelectBitmap (mem_device_context, data->bitmap);

            data->position.x = data->position.y = 0;

            blend.SourceConstantAlpha = data->alpha;

            UpdateLayeredWindow (
                data->window,
                (HDC) NULL,
                (POINT *) NULL,
                &(data->size),
                mem_device_context,
                &(data->position),
                (COLORREF) 0,
                &blend,
                ULW_ALPHA);

            SelectBitmap (mem_device_context, sys_bitmap);
            DeleteDC (mem_device_context);
            ReleaseDC (data->window, device_context);
        }

        DeleteBitmap (data->bitmap);

        SetEvent (data->event);

        if (data->window != (HWND) NULL)
        {
            data->timer =
                SetTimer (
                    data->window,
                    (UINT_PTR) data,
                    data->rate,
                    (TIMERPROC) NULL);

            if (data->timer != (UINT_PTR) NULL)
            {
                while (GetMessage (&message, (HWND) NULL, (UINT) 0, (UINT) 0))
                {
                    TranslateMessage (&message);
                    DispatchMessage (&message);
                }
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    if (data->window != (HWND) NULL)
    {
        DestroyWindow (data->window);
    }

    relmem (&data);

    return (0);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_fade_wnd_proc
| Purpose:  Window Procedure for the fader window
| Input:    window      - window handle of the fader window
|           message     - message sent
|           wParam      - first parameter
|           lParam      - second parameter
| Output:
| Return:   The return from DefWindowProc
\*--------------------------------------------------------------------------*/
static LRESULT CALLBACK _uint_gfx_fade_wnd_proc (HWND window, UINT message,
                                                 WPARAM wParam, LPARAM lParam)
{
    nt_fade_t      *data;
    BLENDFUNCTION   blend = { AC_SRC_OVER, 0, 0, 0 };
    LRESULT         result;

    switch (message)
    {
        case WM_TIMER:
        {
            data = (nt_fade_t *) wParam;

            if ((data->alpha -= data->delta) >= 0)
            {
                blend.SourceConstantAlpha = data->alpha;

                UpdateLayeredWindow (
                    window,
                    (HDC) NULL,
                    (POINT *) NULL,
                    (SIZE *) NULL,
                    (HDC) NULL,
                    (POINT *) NULL,
                    (COLORREF) 0,
                    &blend,
                    ULW_ALPHA);
            }
            else
            {
                KillTimer (window, data->timer);
                PostMessage (window, WM_QUIT, (WPARAM) 0, (LPARAM) 0);
            }

            result = 0;
            break;
        }

        case WM_ERASEBKGND:
        {
            result = TRUE;
            break;
        }

        default:
        {
            result = DefWindowProc (window, message, wParam, lParam);
        }
    }

    return (result);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_brush
| Purpose:  Create a Direct2D brush for a shape from a pen/brush
| Input:    context             - the context
|           object              - the pen/brush
|           fill                - TRUE to fill the shape
| Output:   renderer            - the Direct2D render target
|           origin              - the origin into the render target
|           brush               - the Direct2D brush
|           width               - the stroke width
| Return:   TRUE if a Direct2D brush was created
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_brush (ui_gfx_t *context, void *object, int fill,
                                ID2D1RenderTarget **renderer,
                                D2D1_POINT_2F *origin,
                                ID2D1SolidColorBrush **brush, FLOAT *width)
{
    ID2D1RenderTarget      *render_target;
    EXTLOGPEN               pen_data = { 0 };
    LOGBRUSH                brush_data;
    COLORREF                fg_color;
    int                     pen_width = 0;
    D2D1_COLOR_F            color;
    ID2D1SolidColorBrush   *d2d_brush;
    int                     status = FALSE;

    if (_uint_d2d_renderer_create (
            (HDC) context->context, (RECT *) NULL, FALSE, &render_target) &&
        (object <= (void *) 0 ||
         object > (void *) 255))
    {
        if (object <= (void *) 0)
        {
            fg_color = GetTextColor ((HDC) context->context);

            pen_width = 1;
        }
        else if (fill &&
                 GetObject ((HBRUSH) object, sizeof (LOGBRUSH), &brush_data) &&
                 brush_data.lbStyle == BS_SOLID)
        {
            fg_color = brush_data.lbColor;

            pen_width = 1;
        }
        else if (!fill &&
                 GetObject ((HPEN) object, sizeof (EXTLOGPEN), &pen_data) &&
                 (pen_data.elpPenStyle & PS_STYLE_MASK) == PS_SOLID)
        {
            fg_color = pen_data.elpColor;

            pen_width = pen_data.elpWidth;
        }

        if (pen_width > 0)
        {
            color.r = ((float) GetRValue (fg_color) / 255.0f);
            color.g = ((float) GetGValue (fg_color) / 255.0f);
            color.b = ((float) GetBValue (fg_color) / 255.0f);
            color.a = 1.0f;

            if (ID2D1RenderTarget_CreateSolidColorBrush (
                    render_target, &color, (D2D1_BRUSH_PROPERTIES *) NULL,
                    &d2d_brush) == S_OK)
            {
                INIT_ARG (renderer, render_target)

/*
                origin->x = ((FLOAT) render_target.origin.x + 0.5f);
                origin->y = ((FLOAT) render_target.origin.y + 0.5f);
*/

                origin->x = origin->y = 0.5f;

                INIT_ARG (brush, d2d_brush)

                INIT_ARG (width, (FLOAT) pen_width);

                status = TRUE;
            }
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_polygon
| Purpose:  Draw or fill a polyline/polygon using Direct2D
| Input:    context             - the context
|           object              - the pen/brush
|           points              - the points to link
|           count               - the number of points
|           closed              - TRUE to close the polygon
|           fill                - TRUE to fill the polygon
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_polygon (ui_gfx_t *context, void *object,
                                  const ui_point_t *points, int count,
                                  int closed, int fill)
{
    ID2D1RenderTarget      *renderer;
    D2D1_POINT_2F           origin;
    ID2D1SolidColorBrush   *brush;
    FLOAT                   width;
    ID2D1Factory           *factory;
    ID2D1PathGeometry      *geometry;
    ID2D1GeometrySink      *sink;
    D2D1_POINT_2F           start, end, *dyn_points;
    int                     i;
    int                     status = UI_ERROR;

    if (_uint_gfx_d2d_brush (
            context, object, fill, &renderer, &origin, &brush, &width))
    {
        start.x = (origin.x + (FLOAT) points [0].x);
        start.y = (origin.y + (FLOAT) points [0].y);

        points++;

        if (--count == 1)
        {
            end.x = (origin.x + (FLOAT) points [0].x);
            end.y = (origin.y + (FLOAT) points [0].y);

            ID2D1RenderTarget_DrawLine (
                renderer,
                start,
                end,
                (ID2D1Brush *) brush,
                (FLOAT) width,
                (ID2D1StrokeStyle *) NULL);
        }
        else
        {
            _uint_d2d_factory (&factory);

            ID2D1Factory_CreatePathGeometry (factory, &geometry);

            ID2D1PathGeometry_Open (geometry, &sink);

            ID2D1GeometrySink_SetFillMode (
                sink,
                ((GetPolyFillMode ((HDC) context->context) == ALTERNATE) ?
                 D2D1_FILL_MODE_ALTERNATE :
                 D2D1_FILL_MODE_WINDING));

            ID2D1GeometrySink_BeginFigure (
                sink,
                start,
                (fill ?
                 D2D1_FIGURE_BEGIN_FILLED :
                 D2D1_FIGURE_BEGIN_HOLLOW));

            PRO_CREATE_AND_LOCK_STATIC_BUFFER (dyn_points, 16, count);
            {
                for (i = 0; i < count; i++)
                {
                    dyn_points [i].x = (origin.x + (FLOAT) points [i].x);
                    dyn_points [i].y = (origin.y + (FLOAT) points [i].y);
                }

                ID2D1GeometrySink_AddLines (sink, dyn_points, count);
            }
            PRO_UNLOCK_STATIC_BUFFER (dyn_points);

            ID2D1GeometrySink_EndFigure (
                sink,
                (closed ?
                 D2D1_FIGURE_END_CLOSED :
                 D2D1_FIGURE_END_OPEN));

            ID2D1GeometrySink_Close (sink);

            ID2D1GeometrySink_Release (sink);

            if (fill)
            {
                ID2D1RenderTarget_FillGeometry (
                    renderer,
                    (ID2D1Geometry *) geometry,
                    (ID2D1Brush *) brush,
                    (ID2D1Brush *) NULL);
            }
            else
            {
                ID2D1RenderTarget_DrawGeometry (
                    renderer,
                    (ID2D1Geometry *) geometry,
                    (ID2D1Brush *) brush,
                    (FLOAT) width,
                    (ID2D1StrokeStyle *) NULL);
            }

            ID2D1PathGeometry_Release (geometry);
        }

        ID2D1SolidColorBrush_Release (brush);

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_rects
| Purpose:  Draw or fill rectangles using Direct2D
| Input:    context             - the context
|           object              - the pen/brush
|           rects               - the rectangles
|           count               - the number of rectangles
|           fill                - TRUE to fill the rectangles
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_rects (ui_gfx_t *context, void *object,
                                const ui_rect_t *rects, int count, int fill)
{
    ID2D1RenderTarget      *renderer;
    D2D1_POINT_2F           origin;
    ID2D1SolidColorBrush   *brush;
    FLOAT                   width;
    D2D1_RECT_F             rect;
    int                     i;
    int                     status = UI_ERROR;

    if (_uint_gfx_d2d_brush (
            context, object, fill, &renderer, &origin, &brush, &width))
    {
        for (i = 0; i < count; i++)
        {
            rect.left = (origin.x + (FLOAT) rects [i].x);
            rect.top = (origin.y + (FLOAT) rects [i].y);
            rect.right = (rect.left + (FLOAT) rects [i].width);
            rect.bottom = (rect.top + (FLOAT) rects [i].height);

            if (fill)
            {
                ID2D1RenderTarget_FillRectangle (
                    renderer,
                    &rect,
                    (ID2D1Brush *) brush);
            }
            else
            {
                ID2D1RenderTarget_DrawRectangle (
                    renderer,
                    &rect,
                    (ID2D1Brush *) brush,
                    (FLOAT) width,
                    (ID2D1StrokeStyle *) NULL);
            }
        }

        ID2D1SolidColorBrush_Release (brush);

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_ellipses
| Purpose:  Draw or fill ellipses using Direct2D
| Input:    context             - the context
|           object              - the pen/brush
|           rects               - the rectangles of the ellipses
|           count               - the number of rectangles
|           fill                - TRUE to fill the rectangles
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_ellipses (ui_gfx_t *context, void *object,
                                   const ui_rect_t *rects, int count,
                                   int fill)
{
    ID2D1RenderTarget      *renderer;
    D2D1_POINT_2F           origin;
    ID2D1SolidColorBrush   *brush;
    FLOAT                   width;
    D2D1_ELLIPSE            ellipse;
    int                     i;
    int                     status = UI_ERROR;

    if (_uint_gfx_d2d_brush (
            context, object, fill, &renderer, &origin, &brush, &width))
    {
        for (i = 0; i < count; i++)
        {
            ellipse.radiusX =
                (((FLOAT) rects [i].width) * 0.5f);

            ellipse.radiusY =
                (((FLOAT) rects [i].height) * 0.5f);

            ellipse.point.x =
                (origin.x + (FLOAT) rects [i].x + ellipse.radiusX);

            ellipse.point.y =
                (origin.y + (FLOAT) rects [i].y + ellipse.radiusY);

            if (fill)
            {
                ID2D1RenderTarget_FillEllipse (
                    renderer,
                    &ellipse,
                    (ID2D1Brush *) brush);
            }
            else
            {
                ID2D1RenderTarget_DrawEllipse (
                    renderer,
                    &ellipse,
                    (ID2D1Brush *) brush,
                    (FLOAT) width,
                    (ID2D1StrokeStyle *) NULL);
            }
        }

        ID2D1SolidColorBrush_Release (brush);

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_arcs
| Purpose:  Draw or fill arcs using Direct2D
| Input:    context             - the context
|           object              - the pen/brush
|           arcs                - the arcs
|           count               - the number of arcs
|           fill                - TRUE to fill the arcs
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_arcs (ui_gfx_t *context, void *object,
                               const ui_arc_t *arcs, int count, int fill)
{
    ID2D1RenderTarget      *renderer;
    D2D1_POINT_2F           origin;
    ID2D1SolidColorBrush   *brush;
    FLOAT                   width;
    ID2D1Factory           *factory;
    ID2D1PathGeometry      *geometry;
    ID2D1GeometrySink      *sink;
    D2D1_RECT_F             ellipse, box;
    D2D1_POINT_2F           start, center;
    float                   start_angle, end_angle;
    int                     large, sweep;
    D2D1_ARC_SEGMENT        arc;
    int                     i;
    int                     status = UI_ERROR;

    if (_uint_gfx_d2d_brush (
            context, object, fill, &renderer, &origin, &brush, &width))
    {
        _uint_d2d_factory (&factory);

        ID2D1Factory_CreatePathGeometry (factory, &geometry);

        ID2D1PathGeometry_Open (geometry, &sink);

        for (i = 0; i < count; i++)
        {
            if (arcs [i].start.x == arcs [i].end.x &&
                arcs [i].start.y == arcs [i].end.y)
            {
                /*
                ** For coincident points, draw the complete ellipse
                */

                status =
                    _uint_gfx_d2d_ellipses (
                        context,
                        object,
                        &(arcs [i].rect),
                        1,
                        fill);
            }
            else
            {
                /*
                ** Calculate the bounding box
                */

                ellipse.left = ((FLOAT) arcs [i].rect.x);
                ellipse.top = ((FLOAT) arcs [i].rect.y);
                ellipse.right = (ellipse.left + (FLOAT) arcs [i].rect.width);
                ellipse.bottom = (ellipse.top + (FLOAT) arcs [i].rect.height);

                /*
                ** Account for the pen width
                */

                box.left = (ellipse.left - width);
                box.top = (ellipse.top - width);
                box.right = (ellipse.right + width);
                box.bottom = (ellipse.bottom + width);

                if (fill)
                {
                    width = 0.0f;
                }

                /*
                ** Calculate the coordinates
                */

                center.x = ((box.right - box.left) * 0.5f);
                center.y = ((box.bottom - box.top) * 0.5f);

                arc.size.width =
                    ((ellipse.right - ellipse.left - width) * 0.5f);

                arc.size.height =
                    ((ellipse.bottom - ellipse.top - width) * 0.5f);

                start.x = ((FLOAT) arcs [i].start.x - box.left);
                start.y = ((FLOAT) arcs [i].start.y - box.top);

                arc.point.x = ((FLOAT) arcs [i].end.x - box.left);
                arc.point.y = ((FLOAT) arcs [i].end.y - box.top);

                /*
                ** Calculate the sweep angle
                */

                start_angle =
                    atan2f (
                        (start.y - center.y),
                        (start.x - center.x));

                end_angle =
                    atan2f (
                        (arc.point.y - center.y),
                        (arc.point.x - center.x));

                arc.rotationAngle =
                    (((end_angle - start_angle) * 180.0f) / acosf (-1.0f));

                if (arc.rotationAngle < -360.0f)
                {
                    arc.rotationAngle += 720.0f;
                }
                else if (arc.rotationAngle < 0.0f)
                {
                    arc.rotationAngle += 360.0f;
                }
                else if (arc.rotationAngle >= 360.0f)
                {
                    arc.rotationAngle -= 360.0f;
                }

                /*
                ** Recalculate the coordinates on the rim of the ellipse
                ** based on the calculated angles.
                **
                ** This ensures that the coordinates lie perfectly on the
                ** rim of the ellipse which lies within the rectangle.
                */

                center.x += origin.x;
                center.y += origin.y;

                start.x =
                    (center.x + (arc.size.width * cosf (start_angle)));

                start.y =
                    (center.y + (arc.size.height * sinf (start_angle)));

                arc.point.x =
                    (center.x + (arc.size.width * cosf (end_angle)));

                arc.point.y =
                    (center.y + (arc.size.height * sinf (end_angle)));

                /*
                ** Calculate the large-angle and sweep flags
                */

                large =
                    sweep =
                        (GetArcDirection ((HDC) context->context)
                             == AD_CLOCKWISE);

                if (arc.rotationAngle < 180.0f)
                {
                    large = !large;
                }

                /*
                ** Add the arc
                */

                ID2D1GeometrySink_BeginFigure (
                    sink,
                    start,
                    (fill ?
                     D2D1_FIGURE_BEGIN_FILLED :
                     D2D1_FIGURE_BEGIN_HOLLOW));

                arc.sweepDirection =
                    (sweep ?
                     D2D1_SWEEP_DIRECTION_CLOCKWISE :
                     D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE);

                arc.arcSize =
                    (large ?
                     D2D1_ARC_SIZE_LARGE :
                     D2D1_ARC_SIZE_SMALL);

                ID2D1GeometrySink_AddArc (sink, &arc);

                if (fill &&
                    UI_GFX_HAS_FLAG (context, UI_GFX_PIE_FLAG))
                {
                    ID2D1GeometrySink_AddLine (sink, center);
                }

                ID2D1GeometrySink_EndFigure (
                    sink,
                    (fill ?
                     D2D1_FIGURE_END_CLOSED :
                     D2D1_FIGURE_END_OPEN));
            }
        }

        ID2D1GeometrySink_Close (sink);

        ID2D1GeometrySink_Release (sink);

        if (fill)
        {
            ID2D1RenderTarget_FillGeometry (
                renderer,
                (ID2D1Geometry *) geometry,
                (ID2D1Brush *) brush,
                (ID2D1Brush *) NULL);
        }
        else
        {
            ID2D1RenderTarget_DrawGeometry (
                renderer,
                (ID2D1Geometry *) geometry,
                (ID2D1Brush *) brush,
                (FLOAT) width,
                (ID2D1StrokeStyle *) NULL);
        }

        ID2D1PathGeometry_Release (geometry);

        ID2D1SolidColorBrush_Release (brush);

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_frame
| Purpose:  Draw a frame around a rectangle using Direct2D
| Input:    context             - the context
|           object              - the pen
|           rect                - the rectangle
|           thickness           - the thickness of the frame
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_frame (ui_gfx_t *context, void *object,
                                const ui_rect_t *rect, int thickness)
{
    ID2D1RenderTarget      *renderer;
    D2D1_POINT_2F           origin;
    ID2D1SolidColorBrush   *brush;
    FLOAT                   width;
    D2D1_RECT_F             frame;
    int                     i;
    int                     status = UI_ERROR;

    if (_uint_gfx_d2d_brush (
            context, object, FALSE, &renderer, &origin, &brush, &width))
    {
        frame.left = (origin.x + (FLOAT) rect->x);
        frame.top = (origin.y + (FLOAT) rect->y);
        frame.right = (frame.left + (FLOAT) rect->width);
        frame.bottom = (frame.top + (FLOAT) rect->height);

        for (i = 0; i < thickness; i++)
        {
            ID2D1RenderTarget_DrawRectangle (
                renderer,
                &frame,
                (ID2D1Brush *) brush,
                (FLOAT) width,
                (ID2D1StrokeStyle *) NULL);

            frame.left++;
            frame.top++;
            frame.right--;
            frame.bottom--;
        }

        ID2D1SolidColorBrush_Release (brush);

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_region
| Purpose:  Fill a region using Direct2D
| Input:    context             - the context
|           object              - the pen/brush
|           region              - the region
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_region (ui_gfx_t *context, void *object,
                                 void *region)
{
    DWORD                   region_size;
    unsigned char          *buffer;
    RGNDATA                *region_data;
    ID2D1RenderTarget      *renderer;
    D2D1_POINT_2F           origin;
    ID2D1SolidColorBrush   *brush;
    FLOAT                   width;
    RECT                   *region_rect;
    D2D1_RECT_F             rect;
    int                     status = UI_ERROR;

    if ((region_size =
         GetRegionData ((HRGN) region, 0, (RGNDATA *) NULL)) > 0 &&
        _uint_gfx_d2d_brush (
            context, object, TRUE, &renderer, &origin, &brush, &width))
    {
        PRO_CREATE_AND_LOCK_STATIC_BUFFER (buffer, 1024, region_size);
        {
            if ((region_data = (RGNDATA *) buffer) != (RGNDATA *) NULL &&
                GetRegionData ((HRGN) region, region_size, region_data) &&
                region_data->rdh.iType == RDH_RECTANGLES)
            {
                for (region_rect = (RECT *) region_data->Buffer;
                     region_data->rdh.nCount > 0;
                     region_rect++, region_data->rdh.nCount--)
                {
                    rect.left = (origin.x + (FLOAT) region_rect->left);
                    rect.top = (origin.y + (FLOAT) region_rect->top);
                    rect.right = (origin.x + (FLOAT) region_rect->right);
                    rect.bottom = (origin.y + (FLOAT) region_rect->bottom);

                    ID2D1RenderTarget_FillRectangle (
                        renderer,
                        &rect,
                        (ID2D1Brush *) brush);
                }
            }
        }
        PRO_UNLOCK_STATIC_BUFFER (buffer);

        ID2D1SolidColorBrush_Release (brush);

        status = UI_SUCCESS;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_set_origin
| Purpose:  Set the drawing origin for Direct2D
| Input:    context             - the context
|           x                   - the x co-ordinate of the origin
|           y                   - the y co-ordinate of the origin
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_set_origin (ui_gfx_t *context, int x, int y)
{
    return (_uint_d2d_renderer_set_origin ((HDC) context->context, x, y) ?
            UI_SUCCESS :
            UI_ERROR);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_set_clip_region
| Purpose:  Set the clipping region for Direct2D
| Input:    context             - the context
|           region              - the region to use for clipping
|           mode                - the combination mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_set_clip_region (ui_gfx_t *context, void *region,
                                          int mode)
{
    return (_uint_d2d_renderer_clip ((HDC) context->context, region, mode) ?
            UI_SUCCESS :
            UI_ERROR);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_gfx_d2d_alpha_blend
| Purpose:  Copy the given area from the given context to the other context,
|           resizing or mirroring as necessary, using the given alpha value
| Input:    dest_context        - the destination context
|           dest_x              - the destination rectangle left position
|           dest_y              - the destination rectangle top position
|           dest_width          - the destination rectangle width
|           dest_height         - the destination rectangle height
|           src_context         - the source context
|           src_x               - the source rectangle left position
|           src_y               - the source rectangle top position
|           src_width           - the source rectangle width
|           src_height          - the source rectangle height
|           src_alpha           - the source alpha value
|           mode                - the copy mode
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_gfx_d2d_alpha_blend (ui_gfx_t *dest_context, int dest_x,
                                      int dest_y, int dest_width,
                                      int dest_height,
                                      const ui_gfx_t *src_context, int src_x,
                                      int src_y, int src_width,
                                      int src_height, int src_alpha, int mode)
{
    static D2D1_BITMAP_PROPERTIES   bitmap_properties =
    {
        {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED
        },
        0.0f,
        0.0f
    };

    ID2D1RenderTarget              *dest_renderer;
    ID2D1RenderTarget              *src_renderer;
    ID2D1BitmapRenderTarget        *src_bitmap_renderer;
    void                           *src_region;
    RECT                            clip_rect;
    HRGN                            clip_region;
    int                             unclip = FALSE;
    int                             copy = TRUE;
    ID2D1Bitmap                    *src_bitmap;
    D2D1_RECT_F                     dest_rect;
    D2D1_RECT_F                     src_rect;
    D2D1_MATRIX_3X2_F               src_transform;
    D2D1_SIZE_F                     src_size;
    FLOAT                           src_offset;
    ID2D1Bitmap                    *shared_bitmap = (ID2D1Bitmap *) NULL;
    int                             status = UI_ERROR;

    if (_uint_d2d_renderer_create (
            (HDC) dest_context->context, (RECT *) NULL, FALSE,
            &dest_renderer) &&
        _uint_d2d_renderer_create (
            (HDC) src_context->context, (RECT *) NULL, FALSE,
            &src_renderer) &&
        ID2D1RenderTarget_QueryInterface (
            src_renderer, &IID_ID2D1BitmapRenderTarget,
            (void **) &src_bitmap_renderer) == S_OK)
    {
        if ((src_region =
             _uint_d2d_renderer_region ((HDC) src_context->context)) != NULL)
        {
            clip_rect.left = src_x;
            clip_rect.top = src_y;
            clip_rect.right = (clip_rect.left + src_width);
            clip_rect.bottom = (clip_rect.top + src_height);

            clip_region = CreateRectRgnIndirect (&clip_rect);

            unclip =
                (CombineRgn (
                     clip_region, clip_region, (HRGN) src_region,
                     RGN_AND) == COMPLEXREGION);

            GetRgnBox (clip_region, &clip_rect);

            DeleteRgn (clip_region);

            if (clip_rect.right > clip_rect.left &&
                clip_rect.bottom > clip_rect.top)
            {
                if (src_x != clip_rect.left)
                {
                    dest_x += (clip_rect.left - src_x);
                    src_x = clip_rect.left;
                }

                if (src_y != clip_rect.top)
                {
                    dest_y += (clip_rect.top - src_y);
                    src_y = clip_rect.top;
                }

                src_width = dest_width = (clip_rect.right - clip_rect.left);
                src_height = dest_height = (clip_rect.bottom - clip_rect.top);
            }
            else
            {
                copy = FALSE;
            }
        }

        if (copy)
        {
            if (unclip)
            {
                _uint_d2d_renderer_unclip ((HDC) src_context->context, FALSE);
            }

            /*
            ** Flush any pending drawing on the source so that this will be
            ** captured in the copy
            */

            ID2D1BitmapRenderTarget_Flush (
                src_bitmap_renderer,
                (D2D1_TAG *) NULL,
                (D2D1_TAG *) NULL);

            /*
            ** Get the contents of the source as a bitmap
            */

            if (ID2D1BitmapRenderTarget_GetBitmap (
                    src_bitmap_renderer, &src_bitmap) == S_OK)
            {
                /*
                ** Calculate the destination rectangle
                */

                dest_rect.left = (FLOAT) dest_x;
                dest_rect.top = (FLOAT) dest_y;
                dest_rect.right = (dest_rect.left + (FLOAT) dest_width);
                dest_rect.bottom = (dest_rect.top + (FLOAT) dest_height);

                /*
                ** Calculate the source rectangle, taking into account the current
                ** drawing origin
                */

                ID2D1BitmapRenderTarget_GetTransform (
                    src_bitmap_renderer,
                    &src_transform);

                src_rect.left = ((FLOAT) src_x + src_transform.dx);
                src_rect.top = ((FLOAT) src_y + src_transform.dy);
                src_rect.right = (src_rect.left + (FLOAT) src_width);
                src_rect.bottom = (src_rect.top + (FLOAT) src_height);

                /*
                ** Direct2D does not allow copying from outside the bitmap.
                **
                ** Any attempt to do this will result in scaling as Direct2D clips
                ** the rectangle overlapping the bitmap and scales the result
                */

                ID2D1Bitmap_GetSize (src_bitmap, &src_size);

                if (src_rect.left < 0.0f)
                {
                    src_offset = (0.0f - src_rect.left);

                    src_rect.left += src_offset;
                    dest_rect.left += src_offset;
                }

                if (src_rect.top < 0.0f)
                {
                    src_offset = (0.0f - src_rect.top);

                    src_rect.top += src_offset;
                    dest_rect.top += src_offset;
                }

                if (src_rect.right > src_size.width)
                {
                    src_offset = (src_size.width - src_rect.right);

                    src_rect.right += src_offset;
                    dest_rect.right += src_offset;
                }

                if (src_rect.bottom > src_size.height)
                {
                    src_offset = (src_size.height - src_rect.bottom);

                    src_rect.bottom += src_offset;
                    dest_rect.bottom += src_offset;
                }

                /*
                ** Draw the bitmap, first creating a shared bitmap in this
                ** render target so that it can be rendered
                */

                if (ID2D1RenderTarget_CreateSharedBitmap (
                        dest_renderer, &IID_ID2D1Bitmap, src_bitmap,
                        &bitmap_properties, &shared_bitmap) == S_OK &&
                    shared_bitmap != (ID2D1Bitmap *) NULL)
                {
                    ID2D1RenderTarget_DrawBitmap (
                        dest_renderer,
                        shared_bitmap,
                        &dest_rect,
                        (src_alpha / 255.0f),
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                        &src_rect);

                    ID2D1Bitmap_Release (shared_bitmap);
                }

                /*
                ** Release the bitmap
                */

                ID2D1Bitmap_Release (src_bitmap);
            }

            if (unclip)
            {
                _uint_d2d_renderer_unclip ((HDC) src_context->context, TRUE);
            }

            ID2D1BitmapRenderTarget_Release (src_bitmap_renderer);
        }

        status = UI_SUCCESS;
    }

    return (status);
}

#endif /* UI_SYSTEM_NT */
