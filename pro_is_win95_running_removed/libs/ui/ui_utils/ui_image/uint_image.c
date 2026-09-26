/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    uint_image.c
|
|  Purpose: Windows NT level image handling functions
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  01-Sep-95         jas         Created.
|  07-Sep-95 G-01-05 UK    $$1   Automatic submission
|  16-Feb-96         jas         Added _uint_image_draw
|  19-Feb-96         jas         Use new image code
|  20-Feb-96         jas         Use MaskBlt instead of two BitBlts
|  21-Feb-96 G-03-03 UK    $$2   Automatic Submission
|  22-Feb-96         jas         Added grey images
|  26-Feb-96         jas         Fixed Windows 95 bugs
|  05-Mar-96 G-03-04 UK    $$3   Automatic Submission
|  08-Mar-96         jas         Fixed bug with monochrome images
|  12-Mar-96 G-03-05 UK    $$4   Automatic Submission
|  18-Apr-96         jas         Fixed Windows 95 monochrome bitmap bug
|  23-Apr-96 G-03-11 UK    $$5   Automatic Submission
|  26-Apr-96         jas         Fixed Windows 95 monochrome bitmap bug again
|  07-May-96 G-03-12 UK    $$6   Automatic Submission
|  22-Jul-96         pch         Change prototype of _ui_get_image_color to
|                                include a component
|  23-Jul-96         pch         Reverse previous change
|  30-Jul-96 H-01-03 UK    $$7   Automatic Submission
|  12-Sep-96         rgk         check for getmem (0)
|  17-Sep-96 H-01-09 UK    $$8   Automatic Submission
|  28-Oct-96         rca         changes to increase general speed of UI
|  30-Oct-96 H-01-15 UK    $$9   Automatic Submission
|  21-Nov-96         rca         speed changes - use image_data struct
|  22-Nov-96         rca         speed alterations
|  22-Nov-96         rca         correct silly mistake on WIN95
|  26-Nov-96 H-01-18 UK    $$10  Automatic Submission
|  27-Nov-96         jas         Use the palette when creating and drawing
|                                images
|  02-Dec-96 H-01-19 UK    $$11  Automatic Submission
|  11-Dec-96         jas         Temporarily do not destroy any images
|  18-Dec-96 H-01-21 UK    $$12  Automatic Submission
|  12-Feb-97         jas         Added new default image handling
|  14-Feb-97         jas         Added _ui_get_graphics_mode
|  26-Feb-97 H-03-02 UK    $$13  Automatic Submission
|  11-Mar-97         jas         Fixed destroy function
|  12-Mar-97         jas         Fixed create and destroy functions again
|  25-Mar-97 H-03-04 UK    $$14  Automatic Submission
|  16-Apr-97         jas         Fixed Windows 95 bitmap bugs
|  22-Apr-97 H-03-07 UK    $$15  Automatic Submission
|  05-Jun-97         jas         Fixed creation of grey images
|  10-Jun-97         jas         Added cursor support
|  17-Jun-97 H-03-14 UK    $$16  Automatic Submission
|  18-Jun-97         jas         Fixed status of sysdep_image_create_cursor
|  26-Jun-97 H-03-15 UK    $$17  Automatic Submission
|  06-Nov-97         jas         Added new color database
|  18-Nov-97 H-03-30 UK    $$18  Automatic Submission
|  18-Mar-98         jas         Added support for standard Windows cursors
|  31-Mar-98 I-01-01 UK    $$19  Automatic Submission
|  16-Jun-98         jas         Added new image code
|  16-Jun-98         jas         Added window colors to images
|  16-Jun-98         jas         Added image drawing flags
|  22-Jun-98 I-01-12 UK    $$20  Automatic Submission
|  07-Jul-98         jas         Implemented highlighted image drawing
|  13-Jul-98 I-01-14 UK    $$21  Automatic Submission
|  23-Jul-98         jas         Fixed grey color mapping
|  28-Jul-98 I-01-15 UK    $$22  Automatic Submission
|  28-Jul-98         jas         Fixed monochrome images
|  11-Aug-98 I-01-16 UK    $$23  Automatic Submission
|  03-Sep-98         jas         Use 3D grey color for insensitive images
|  08-Sep-98 I-01-18 UK    $$24  Automatic Submission
|  17-Sep-98         jas         Fixed Windows 95 problems
|  23-Sep-98 I-01-20 UK    $$25  Automatic Submission
|  28-Sep-98         jas         Fixed cursor sizing problems
|  30-Sep-98 I-01-21 UK    $$26  Automatic Submission
|  21-Oct-98         jas         Added sizing cursors
|  04-Nov-98 I-01-25 UK    $$27  Automatic Submission
|  08-Jan-99         jas         Use UINT_SYSTEM_NT instead of UINT_SYSTEM_95
|  01-Feb-99 I-03-01 UK    $$28  Automatic Submission
|  08-Feb-99 I-03-02 UK    $$29  Automatic Submission
|  06-Oct-99         jas         Cleaned up
|  06-Oct-99         jas         Added UI_IMAGE_DRAW_INVERTED
|  14-Oct-99 I-03-18 UK    $$30  Automatic Submission
|  21-Oct-99         jas         Removed memory leak
|  22-Oct-99 I-03-18+UK    $$31  Patch Submission
|  27-Oct-99         jas         Added UI_IMAGE_DRAW_ONCE
|  03-Nov-99 I-03-20 UK    $$32  Automatic Submission
|  13-Jan-00         jas         Added new window colors
|  14-Jan-00 I-03-26+UK    $$33  Patch Submission
|  25-Jan-00         jas         Added sysdep_image_get_menu_check_size
|  25-Jan-00 I-03-26+UK    $$34  Patch Submission
|  04-May-00         jas         Changed prototype for relmem
|  18-May-00 J-01-08 UK    $$35  Automatic Submission
|  13-Jun-00         jas         Added TrueColor image support
|  13-Jun-00         jas         Added sysdep_image_true_color_enabled
|  14-Jun-00 J-01-10 UK    $$36  Automatic Submission
|  15-Jun-00         jas         Removed sysdep_image_true_color_enabled
|  20-Jun-00         jas         Added sysdep_image_overlay_drawable
|  21-Jun-00 J-01-11 UK    $$37  Automatic Submission
|  23-Jun-00         jas         Fixed insensitive image generation
|  28-Jun-00         jas         Use correct color lookup functions
|  11-Jul-00 J-01-12 UK    $$38  Automatic Submission
|  30-Apr-01         jas         Fixed insensitive TrueColor image generation
|  12-Jun-01 J-03-01 UK    $$39  Automatic Submission
|  09-Oct-01         jas         Use pro_is_win95_running
|  18-Oct-01 J-03-10 UK    $$40  Automatic Submission
|  07-Nov-01         mdb         Allow window colors in TrueColor images
|  13-Nov-01 J-03-12 UK    $$41  Automatic Submission
|  10-Jan-02         jas         Added support for PTC graphics mode
|  17-Jan-02 J-03-17 UK    $$42  Automatic Submission
|  19-Feb-02         jas         Fixed _uint_image_draw
|  20-Feb-02 J-03-19 UK    $$43  Automatic Submission
|  20-Mar-02         jas         Modified insensitive TrueColor images
|  20-Mar-02 J-03-21 UK    $$44  Automatic Submission
|  19-Apr-02         jas         Added sysdep_image_create_mask
|  30-Apr-02 J-03-24 UK    $$45  Automatic Submission
|  01-May-02         jas         Fixed brightness calculation rounding errors
|  14-May-02 J-03-25 UK    $$46  Automatic Submission
|  21-Feb-02         jas         Fixed transparency problems
|  25-Feb-03 K-01-01 UK    $$47  Automatic Submission
|  05-Mar-03         jas         Use GFX module
|  11-Mar-03 K-01-02 UK    $$48  Automatic Submission
|  26-Jun-03         jas         Obsoleted ui_memory.h
|  10-Jul-03 K-01-10 UK    $$49  Automatic Submission
|  11-Nov-03         jas         Added UI_STATIC
|  18-Nov-03 K-01-18 UK    $$50  Automatic Submission
|  04-Mar-04         jas         Use _ui_gfx_mask_area
|  04-Mar-04         jas         Added _ui_gfx_alpha_blend
|  08-Mar-04         jas         Use current background color for selection
|  16-Mar-04 K-01-25 UK    $$51  Automatic Submission
|  22-Mar-04         jas         Added sysdep_image_create_grey
|  12-May-04 K-03-01 UK    $$52  Automatic Submission
|  26-May-05         jas         Added support for 32-bit images
|  01-Jun-05 K-03-25 UK    $$53  Automatic Submission
|  13-Jul-05         jas         Fixed _uint_image_draw
|  14-Jul-05 K-03-28 UK    $$54  Automatic Submission
|  14-Jul-05         jas         Fixed problems with UI_IMAGE_DRAW_ONCE
|  18-Jul-05 K-03-29 UK    $$55  Automatic Submission
|  03-Aug-05         AW          Fixed bitmap selection error
|  09-Aug-05 K-03-30 UK    $$56  Automatic Submission
|  11-Oct-05         jas         Removed pro_is_win95_running
|  31-Jan-06 L-01-01 UK    $$57  Automatic Submission
|  20-Jun-06         jas         Removed _ui_gfx_select_palette
|  27-Jun-06 L-01-11 UK    $$58  Automatic Submission
|  17-Jul-06         jas         Fixed resource leaks
|  26-Jul-06 L-01-13 UK    $$59  Automatic Submission
|  21-May-07         jas         Added support for non-32-bit displays
|  05-Jun-07 L-01-32 UK    $$60  Automatic Submission
|  14-Jun-07         jas         Added sysdep_image_read
|  19-Jun-07 L-01-33 UK    $$61  Automatic Submission
|  30-Nov-07         jas         Limited sysdep_image_read to 24-bit
|  29-Jan-08 L-03-01 UK    $$62  Automatic Submission
|  18-Feb-08         jas         Improved support for 32-bit images
|  26-Feb-08 L-03-03 UK    $$63  Automatic Submission
|  10-Jun-08         jas         Fixed use of UI_IMAGE_DRAW_ONCE
|  17-Jun-08 L-03-11 UK    $$64  Automatic Submission
|  14-Oct-08         jas         Added sysdep_image_draw_icon
|  04-Nov-08 L-03-19 UK    $$65  Automatic Submission
|  20-Feb-09         jas         Modified sysdep_image_create
|  24-Feb-09         jas         Fixed uninitialized memory problems
|  25-Feb-09         jas         Always use DIBs for 32-bit images
|  03-Mar-09 L-03-27 UK    $$66  Automatic Submission
|  22-May-09         jas         Fixed GDI resource leaks
|  27-May-09 L-03-33 UK    $$67  Automatic Submission
|  02-Sep-09         jas         Prevent ICM/WCS color space initialization
|  15-Sep-09 L-05-05 UK    $$68  Automatic Submission
|  28-Oct-09         jas         Added BITMAPV5INFO
|  05-Nov-09         jas         Fixed comments
|  10-Nov-09 L-05-09 UK    $$69  Automatic Submission
|  09-Dec-09         jas         Added sysdep_image_stretch
|  22-Dec-09 L-05-12 UK    $$70  Automatic Submission
|  06-Jan-10         jas         Fixed _uint_image_stretch
|  19-Jan-10 L-05-14 UK    $$71  Automatic Submission
|  05-Nov-10         jas         Added _ui_gfx_create_memory_context
|  09-Nov-10 L-05-35 UK    $$72  Automatic Submission
|  06-Apr-11         jas         Use CreateDIBitmap for 24/16/8-bit images
|  07-Apr-11         jas         Added LCS_DEVICE_CMYK
|  12-Apr-11 L-05-45 UK    $$73  Automatic Submission
|  20-May-11         jas         Removed overlay planes
|  14-Jun-11 P-10-01 UK    $$74  Automatic Submission
|  13-Mar-12         jas         Fixed compilation warnings
|  20-Mar-12 P-20-01 UK    $$75  Automatic Submission
|  31-May-12         jas         Added _uint_image_get_icon
|  12-Jun-12 P-20-07 UK    $$76  Automatic Submission
|  21-Nov-12         jas         Modified _uint_image_read
|  27-Nov-12 P-20-18 UK    $$77  Automatic Submission
|  11-Jul-13         jas         Added _ui_display_scale
|  16-Jul-13 P-20-34 UK    $$78  Automatic Submission
|  31-Jul-13         jas         Added UI_DISPLAY_DESCALE
|  14-Aug-13 P-20-36 UK    $$79  Automatic Submission
|  20-Oct-14         jas         Fixed performance problem with opaque images
|  21-Oct-14 P-20-62 UK    $$80  Automatic Submission
|  02-Dec-14         jas         Improved performance of opaque images
|  17-Dec-14 P-20-64 UK    $$81  Automatic Submission
|  23-Feb-16         jas         Scale cursors according to the DPI
|  02-Mar-16 P-30-27 UK    $$82  Automatic Submission
|  13-Jun-16         jas         Added convenience macros
|  21-Jun-16 P-30-34 UK    $$83  Automatic Submission
|  21-Nov-16         jas         Fixed _uint_image_draw
|  28-Nov-16         jas         Added _ui_gfx_intersect_region
|  20-Dec-16 P-30-42 UK    $$84  Automatic Submission
|  07-Mar-17         jas         Added dbg_err_crash on failed bitmap creation
|  14-Mar-17 P-50-01 UK    $$85  Automatic Submission
|  30-Aug-17         jas         Fixed _uint_image_get_menu_check_size
|  06-Sep-17 P-50-26 UK    $$86  Automatic Submission
|  28-Nov-17         jas         Fixed UI_IMAGE_DRAW_SELECTED
|  28-Nov-17         jas         Added _ui_color_tint
|  05-Dec-17 P-50-39 UK    $$87  Automatic Submission
|  21-Feb-18         jas         Added HICON support to _uint_image_read
|  11-Apr-18 P-50-49 UK    $$88  Automatic Submission
|  11-Apr-18         jas         Fixed error checking in _uint_image_read
|  24-Apr-18 P-60-01 UK    $$89  Automatic Submission
|  07-Jun-18         jas         Added tinting flag to sysdep_image_create
|  08-Jun-18         jas         Added _ui_color_get_rgbs
|  15-Jun-18         jas         Added UI_COLOR_BRIGHTNESS
|  18-Jun-18 P-60-07 UK    $$90  Automatic Submission
|  09-Jul-18         jas         Added UI_FOOTBALL_CURSOR
|  10-Jul-18 P-60-09 UK    $$91  Automatic Submission
|  16-Jul-18         jas         Removed UI_FOOTBALL_CURSOR
|  16-Jul-18 P-60-10 UK    $$92  Automatic Submission
|  02-Jul-19         jas         Added ui_dpi_t
|  05-Jul-19         jas         Added DPI interfaces for scaling
|  08-Jul-19 P-70-17 UK    $$93  Automatic Submission
|  12-Jul-19         jas         Added UI_DPI_SYSTEM
|  16-Jul-19 P-70-18 UK    $$94  Automatic Submission
|  30-Jul-19         jas         Added _ui_image_draw_sysdep_image
|  30-Jul-19 P-70-20 UK    $$95  Automatic Submission
|  18-Sep-19         jas         Added context to _ui_gfx_destroy_brush/pen
|  24-Sep-19 P-70-28 UK    $$96  Automatic Submission
|  15-Oct-19         jas         Added UI_IMAGE_DRAW_OPAQUE
|  16-Oct-19 P-70-30 UK    $$97  Automatic Submission
|  05-May-20         jas         Fixed sysdep_image_draw_icon scaling
|  11-May-20         jas         Fixed _uint_image_create_cursor
|  13-May-20 P-80-03 UK    $$98  Automatic Submission
|  01-Jun-20         jas         Fixed _uint_image_draw_icon
|  02-Jun-20 P-80-06 UK    $$99  Automatic Submission
|  03-Jun-20         jas         Fixed problems deleting icon bitmaps
|  03-Jun-20         jas         Improved performance of _uint_image_read
|  10-Jun-20 P-80-07 UK    $$100 Automatic Submission
|  02-Jul-20         jas         Added UI_IMAGE_PREMULTIPLY
|  02-Jul-20         jas         Added UI_IMAGE_IS_PREMULTIPLIED
|  07-Jul-20 P-80-11 UK    $$101 Automatic Submission
|  21-Oct-20         jas         Added flags to _ui_image_of_size
|  27-Oct-20 P-80-26 UK    $$102 Automatic Submission
|  27-Oct-20         jas         Renamed GFX region interfaces
|  05-Nov-20 P-80-27 UK    $$103 Automatic Submission
|  10-Nov-20         jas         Fixed compilation warnings
|  11-Nov-20 P-80-28 UK    $$104 Automatic Submission
|  17-Dec-20         jas         Added opaque flag to sysdep_image_read
|  05-Jan-21 P-80-35 UK    $$105 Automatic Submission
|  31-Mar-21         jas         Added DBG_ERR_CRASH/SYSERR macros
|  21-Apr-21 P-90-05 UK    $$106 Automatic Submission
|  05-Nov-21         jas         Added UIMSG_DBG_CRASH
|  09-Nov-21 P-90-33 UK    $$107 Automatic Submission
|  20-May-24         jas         Added _uint_d2d_renderer_destroy
|  22-May-24 Q-12-13 UK    $$108 Automatic Submission
|  12-Sep-24         jas         Added _uint_image_create_DIB_data
|  18-Sep-24 Q-12-30 UK    $$109 Automatic Submission
|  23-Sep-24         jas         Fixed typo
|  25-Sep-24 Q-12-31 UK    $$110 Automatic Submission
|  24-Oct-24         jas         Removed unnecessary code
|  30-Oct-24 Q-12-36 UK    $$111 Automatic Submission
|  06-Oct-25         jas         Added UI_IMAGE_IS_ACCELERATED
|  08-Oct-25 Q-13-29 UK    $$112 Automatic Submission
|  08-Oct-25         jas         Do not use StretchBlt for Direct2D images
|  10-Oct-25         jas         Removed stretch from _ui_gfx_alpha_blend
|  10-Oct-25         jas         Added ID2D1RenderTarget
|  15-Oct-25 Q-13-30 UK    $$113 Automatic Submission
|  27-Oct-25         jas         Removed UI_IMAGE_DRAW_INVERTED
|  28-Oct-25 Q-13-32 UK    $$114 Automatic Submission
|  06-Feb-26         jas         Fixed _ui_gfx_create_memory_context
|  06-Feb-26         jas         Added UINT_DEBUG
|  09-Feb-26 Q-13-47 UK    $$115 Automatic Submission
|  04-Mar-26         jas         Added UI_IMAGE_IS_NOT_PREMULTIPLIED
|  09-Mar-26         jas         Added UI_COLOR_RGB_BRIGHTNESS24
|  17-Mar-26 Q-27-01 UK    $$116 Automatic Submission
|  06-May-26         jas         Added _ui_cursor_get_size
|  06-May-26 Q-27-09 UK    $$117 Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#if !defined (lint) && defined (SHOW_SCCS_ID)
static char uint_image_c_id [] = "@(#) uint_image.c 4209.1@(#)";
#endif

#include <sysmath.h>
#include <pro_memory.h>
#include <const.h>

#include <ui.h>
#include <uip.h>
#include <ui_app_res.h>
#include <ui_colors.h>
#include <ui_cursor.h>
#include <ui_gfx.h>
#include <ui_string.h>
#include <ui_utils.h>
#include <ui_image.h>
#include <ui_imagep.h>

#ifdef UI_SYSTEM_NT

#include <uint.h>
#include <uint_d2d1.h>
#include <shellapi.h>
#include <shlobj.h>

static HBITMAP _uint_image_create_DDB (
    HDC             device_context,
    int             width,
    int             height,
    int             depth,
    unsigned char  *data,
    ui_point_t    **data_arrays,
    int            *count_arrays,
    int             array_count,
    int            *colors,
    int             tint
);

static HBITMAP _uint_image_create_DIB (
    HDC             device_context,
    int             width,
    int             height,
    unsigned char  *data,
    int             tint,
    int             is_premultiplied,
    int             want_premultiplied
);

static HBITMAP _uint_image_create_compatible (
    HDC             device_context,
    int             width,
    int             height,
    unsigned char  *data,
    int             tint,
    int             is_premultiplied,
    int             want_premultiplied
);

static void _uint_image_create_DIB_data (
    unsigned char  *dib_data,
    int             dib_bpr,
    BITMAPV5INFO   *dib_color_info,
    int             width,
    int             height,
    int             depth,
    unsigned char  *data,
    ui_point_t    **data_arrays,
    int            *count_arrays,
    int             array_count,
    int            *colors,
    int             tint,
    int             is_premultiplied,
    int             want_premultiplied,
    int             flipped
);

static void _uint_image_stretch_section (
    HDC dest_dc,
    int dx,
    int dy,
    int dw,
    int dh,
    HDC src_dc,
    int sx,
    int sy,
    int sw,
    int sh,
    int t
);

static void _uint_image_blend_section (
    HDC dest_dc,
    int dx,
    int dy,
    int dw,
    int dh,
    HDC src_dc,
    int sx,
    int sy,
    int sw,
    int sh,
    int t
);

static TCHAR *_uint_image_get_icon_handle (
    ui_image_data_t    *image_data,
    int                *cursor,
    int                *large
);

static HICON _uint_image_get_icon (
    ui_image_data_t    *image_data,
    int                *large
);

static int _uint_image_d2d_create_renderer (
    ID2D1DCRenderTarget   **renderer
);

static int _uint_image_d2d_renderer (
    ID2D1RenderTarget **renderer,
    const ui_gfx_t     *context,
    int                 flags
);

static int _uint_image_d2d_create (
    ID2D1Bitmap   **bitmap,
    int             width,
    int             height,
    int             depth,
    unsigned char  *data,
    ui_point_t    **data_arrays,
    int            *count_arrays,
    int             array_count,
    int            *colors,
    int             tint,
    int             is_premultiplied,
    int             want_premultiplied
);

static int _uint_image_d2d_destroy (
    ID2D1Bitmap    *bitmap
);

static int _uint_image_d2d_draw (
    ID2D1RenderTarget  *renderer,
    ID2D1Bitmap        *bitmap,
    int                 x,
    int                 y,
    int                 width,
    int                 height
);

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_mask
| Purpose:  Create an image mask with the given dimensions and data
| Input:    image_data      - the image data structure
| Output:   image_data      - the image data structure
| Return:   UI_SUCCESS if everything went well, othersize UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_create_mask (ui_image_data_t *image_data)
{
#ifdef UI_SYSTEM_NT

    int             width =  image_data->size.width;
    int             height =  image_data->size.height;
    int             mask_bpr;
    unsigned char  *mask_data, mask_byte, *ptr;
    int             i, j, k, column;
    HBITMAP         mask_bitmap = (HBITMAP) NULL;
    int             status = UI_ERROR;

    /*
    ** Create the mask bitmap (monochrome)
    */

    if (image_data->sysdep_mask_image == NULL &&
        height > 0)
    {
        mask_bpr = (((width + 15) / 16) * 2);

        mask_data = GET_ARRAY (unsigned char, (mask_bpr * height));

        switch (image_data->depth)
        {
            case 32:
            {
                for (ptr = image_data->data, i = 0; i < height; i++)
                {
                    column = 0;

                    for (j = 0; j < mask_bpr; j++)
                    {
                        mask_byte = 0;

                        for (k = 7; k >= 0; k--)
                        {
                            if (ptr [3] != 0)
                            {
                                mask_byte |= (1 << k);
                            }

                            ptr += 4;
                            column++;

                            if (column == width)
                            {
                                break;
                            }
                        }

                        mask_data [(i * mask_bpr) + j] = (mask_byte ^ 255);

                        if (column == width)
                        {
                            break;
                        }
                    }

                    if (j < (mask_bpr - 1))
                    {
                        mask_data [(i * mask_bpr) + j + 1] = 255;
                    }
                }

                break;
            }

            case 24:
            {
                for (ptr = image_data->data, i = 0; i < height; i++)
                {
                    column = 0;

                    for (j = 0; j < mask_bpr; j++)
                    {
                        mask_byte = 0;

                        for (k = 7; k >= 0; k--)
                        {
                            if (!(UI_COLOR_IS_RGB ((ptr [3] << 24))))
                            {
                                if (ptr [0] != UI_COLOR_TRANSPARENT)
                                {
                                    mask_byte |= (1 << k);
                                }
                            }
                            else if (!(UI_COLOR_IS_TRANSPARENT_RGB ((ptr [3] << 24))))
                            {
                                mask_byte |= (1 << k);
                            }

                            ptr += 4;
                            column++;

                            if (column == width)
                            {
                                break;
                            }
                        }

                        mask_data [(i * mask_bpr) + j] = (mask_byte ^ 255);

                        if (column == width)
                        {
                            break;
                        }
                    }

                    if (j < (mask_bpr - 1))
                    {
                        mask_data [(i * mask_bpr) + j + 1] = 255;
                    }
                }

                break;
            }

            default:
            {
                for (ptr = image_data->data, i = 0; i < height; i++)
                {
                    column = 0;

                    for (j = 0; j < mask_bpr; j++)
                    {
                        mask_byte = 0;

                        for (k = 7; k >= 0; k--)
                        {
                            if (ptr [0] != UI_COLOR_TRANSPARENT)
                            {
                                mask_byte |= (1 << k);
                            }

                            ptr++;
                            column++;

                            if (column == width)
                            {
                                break;
                            }
                        }

                        mask_data [(i * mask_bpr) + j] = (mask_byte ^ 255);

                        if (column == width)
                        {
                            break;
                        }
                    }

                    if (j < (mask_bpr - 1))
                    {
                        mask_data [(i * mask_bpr) + j + 1] = 255;
                    }
                }
            }
        }

        mask_bitmap = CreateBitmap (width, height, 1, 1, (void *) mask_data);

        relmem (&mask_data);

        image_data->sysdep_mask_image = (void *) mask_bitmap;

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_grey
| Purpose:  Create a grey image with the given dimensions and data
| Input:    image_data      - the image data structure
| Output:   image_data      - the image data structure
| Return:   UI_SUCCESS if everything went well, othersize UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_create_grey (ui_image_data_t *image_data)
{
#ifdef UI_SYSTEM_NT

    int             width =  image_data->size.width;
    int             height =  image_data->size.height;
    int             grey_bpr;
    unsigned char  *grey_data, grey_byte, *ptr;
    int             brightness, brightness_count;
    int             i, j, k, column;
    HBITMAP         grey_bitmap = (HBITMAP) NULL;
    int             status = UI_ERROR;

    /*
    ** Create the grey bitmap (monochrome)
    */

    if (image_data->sysdep_grey_image == NULL &&
        height > 0)
    {
        grey_bpr = (((width + 15) / 16) * 2);

        grey_data = GET_ARRAY (unsigned char, (grey_bpr * height));

        switch (image_data->depth)
        {
            case 32:
            {
                brightness = brightness_count = 0;

                for (i = (width * height), ptr = image_data->data;
                     i > 0;
                     i--, ptr += 4)
                {
                    if (ptr [3] != 0)
                    {
                        brightness +=
                            UI_COLOR_RGB_BRIGHTNESS32 (
                                ptr [0],
                                ptr [1],
                                ptr [2],
                                ptr [3]);

                        brightness_count++;
                    }
                }

                if (brightness_count > 0)
                {
                    brightness = (brightness / brightness_count);
                }

                for (ptr = image_data->data, i = 0; i < height; i++)
                {
                    column = 0;

                    for (j = 0; j < grey_bpr; j++)
                    {
                        grey_byte = 0;

                        for (k = 7; k >= 0; k--)
                        {
                            if (ptr [3] != 0)
                            {
                                if (UI_COLOR_RGB_BRIGHTNESS32 (
                                        ptr [0], ptr [1], ptr [2], ptr [3])
                                        <= brightness)
                                {
                                    grey_byte |= (1 << k);
                                }
                            }

                            ptr += 4;
                            column++;

                            if (column == width)
                            {
                                break;
                            }
                        }

                        grey_data [(i * grey_bpr) + j] = (grey_byte ^ 255);

                        if (column == width)
                        {
                            break;
                        }
                    }

                    if (j < (grey_bpr - 1))
                    {
                        grey_data [(i * grey_bpr) + j + 1] = 255;
                    }
                }

                break;
            }

            case 24:
            {
                brightness = brightness_count = 0;

                for (i = (width * height), ptr = image_data->data;
                     i > 0;
                     i--, ptr += 4)
                {
                    if (UI_COLOR_IS_RGB (ptr [3] << 24) &&
                        !(UI_COLOR_IS_TRANSPARENT_RGB (ptr [3] << 24)))
                    {
                        brightness +=
                            UI_COLOR_RGB_BRIGHTNESS24 (
                                ptr [0],
                                ptr [1],
                                ptr [2]);

                        brightness_count++;
                    }
                }

                if (brightness_count > 0)
                {
                    brightness = (brightness / brightness_count);
                }

                for (ptr = image_data->data, i = 0; i < height; i++)
                {
                    column = 0;

                    for (j = 0; j < grey_bpr; j++)
                    {
                        grey_byte = 0;

                        for (k = 7; k >= 0; k--)
                        {
                            if (!(UI_COLOR_IS_RGB ((ptr [3] << 24))))
                            {
                                if (ptr [0] != UI_COLOR_TRANSPARENT)
                                {
                                    switch (ptr [0])
                                    {
                                        case UI_COLOR_BLACK:
                                        case UI_COLOR_DK_RED:
                                        case UI_COLOR_DK_GREEN:
                                        case UI_COLOR_DK_YELLOW:
                                        case UI_COLOR_DK_BLUE:
                                        case UI_COLOR_DK_MAGENTA:
                                        case UI_COLOR_DK_CYAN:
                                        case UI_COLOR_DK_GREY:
                                        case UI_COLOR_RED:
                                        case UI_COLOR_GREEN:
                                        case UI_COLOR_BLUE:
                                        case UI_COLOR_MAGENTA:
                                        case UI_COLOR_3D_DARK_SHADOW:
                                        case UI_COLOR_3D_VERY_DARK_SHADOW:
                                        case UI_COLOR_3D_TEXT:
                                        case UI_COLOR_WINDOW_TEXT:
                                        case UI_COLOR_GREY_TEXT:
                                        case UI_COLOR_MENU_TEXT:
                                        case UI_COLOR_SELECTED_BACKGROUND:
                                        case UI_COLOR_POPUPHELP_TEXT:
                                        case UI_COLOR_ACTIVE_TITLEBAR:
                                        case UI_COLOR_INACTIVE_TITLEBAR:
                                        {
                                            grey_byte |= (1 << k);
                                            break;
                                        }
                                    }
                                }
                            }
                            else if (!(UI_COLOR_IS_TRANSPARENT_RGB ((ptr [3] << 24))))
                            {
                                if (UI_COLOR_RGB_BRIGHTNESS24 (
                                        ptr [0], ptr [1], ptr [2])
                                        <= brightness)
                                {
                                    grey_byte |= (1 << k);
                                }
                            }

                            ptr += 4;
                            column++;

                            if (column == width)
                            {
                                break;
                            }
                        }

                        grey_data [(i * grey_bpr) + j] = (grey_byte ^ 255);

                        if (column == width)
                        {
                            break;
                        }
                    }

                    if (j < (grey_bpr - 1))
                    {
                        grey_data [(i * grey_bpr) + j + 1] = 255;
                    }
                }

                break;
            }

            default:
            {
                for (ptr = image_data->data, i = 0; i < height; i++)
                {
                    column = 0;

                    for (j = 0; j < grey_bpr; j++)
                    {
                        grey_byte = 0;

                        for (k = 7; k >= 0; k--)
                        {
                            if (ptr [0] != UI_COLOR_TRANSPARENT)
                            {
                                switch (ptr [0])
                                {
                                    case UI_COLOR_DK_RED:
                                    {
                                        if (image_data->depth > 1)
                                        {
                                            grey_byte |= (1 << k);
                                        }

                                        break;
                                    }

                                    case UI_COLOR_BLACK:
                                    case UI_COLOR_DK_GREEN:
                                    case UI_COLOR_DK_YELLOW:
                                    case UI_COLOR_DK_BLUE:
                                    case UI_COLOR_DK_MAGENTA:
                                    case UI_COLOR_DK_CYAN:
                                    case UI_COLOR_DK_GREY:
                                    case UI_COLOR_RED:
                                    case UI_COLOR_GREEN:
                                    case UI_COLOR_BLUE:
                                    case UI_COLOR_MAGENTA:
                                    case UI_COLOR_3D_DARK_SHADOW:
                                    case UI_COLOR_3D_VERY_DARK_SHADOW:
                                    case UI_COLOR_3D_TEXT:
                                    case UI_COLOR_WINDOW_TEXT:
                                    case UI_COLOR_GREY_TEXT:
                                    case UI_COLOR_MENU_TEXT:
                                    case UI_COLOR_SELECTED_BACKGROUND:
                                    case UI_COLOR_POPUPHELP_TEXT:
                                    case UI_COLOR_ACTIVE_TITLEBAR:
                                    case UI_COLOR_INACTIVE_TITLEBAR:
                                    {
                                        grey_byte |= (1 << k);
                                        break;
                                    }
                                }
                            }

                            ptr++;
                            column++;

                            if (column == width)
                            {
                                break;
                            }
                        }

                        grey_data [(i * grey_bpr) + j] = (grey_byte ^ 255);

                        if (column == width)
                        {
                            break;
                        }
                    }

                    if (j < (grey_bpr - 1))
                    {
                        grey_data [(i * grey_bpr) + j + 1] = 255;
                    }
                }
            }
        }

        grey_bitmap = CreateBitmap (width, height, 1, 1, (void *) grey_data);

        relmem (&grey_data);

        image_data->sysdep_grey_image = (void *) grey_bitmap;

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create
| Purpose:  Create a bitmap with the given dimensions and data
| Input:    image_data      - the image data structure
|           flags           - the image flags
|           blend_context   - the context with which to blend the image data
|           data_arrays     - array of arrays of points to draw
|           count_arrays    - array of number of points to draw
|           array_count     - number of above arrays
|           colors          - the mappings of color indices to color values
|           tint            - TRUE to apply tinting to the image
| Output:   image_data      - the image data structure
| Return:   UI_SUCCESS if everything went well, othersize UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_create (ui_image_data_t *image_data, int flags,
                                  ui_gfx_t *blend_context,
                                  ui_point_t **data_arrays,
                                  int *count_arrays, int array_count,
                                  int *colors, int tint)
{
#ifdef UI_SYSTEM_NT

    static char    *error_message = "Failed to create %dx%d %s (%d) for \"%s\"";
    ID2D1Bitmap    *d2d_bitmap;
    HDC             device_context;
    HBITMAP         bitmap;
    char           *error_text;
    int             status = UI_SUCCESS;

    /*
    ** Create the image bitmap (color)
    */

    if (image_data->depth >= 32 &&
        blend_context != (ui_gfx_t *) NULL)
    {
        status = UI_ERROR;
    }
    else
    {
        if ((flags & UI_IMAGE_IS_ACCELERATED) &&
            image_data->sysdep_image2 == NULL)
        {
            if (_uint_image_d2d_create (
                    &d2d_bitmap, image_data->size.width,
                    image_data->size.height, image_data->depth,
                    image_data->data, data_arrays, count_arrays, array_count,
                    colors, tint, (flags & UI_IMAGE_IS_PREMULTIPLIED),
                    !(flags & UI_IMAGE_IS_NOT_PREMULTIPLIED)))
            {
                image_data->sysdep_image2 = (void *) d2d_bitmap;
            }
            else
            {
                flags &= ~UI_IMAGE_IS_ACCELERATED;
            }
        }

        if (!(flags & UI_IMAGE_IS_ACCELERATED) &&
            image_data->sysdep_image == NULL)
        {
            device_context = GetDC (GetDesktopWindow ());

            /*
            ** Use a DDB for all images which are not 32-bit
            */

            if (image_data->depth < 32)
            {
                if ((bitmap =
                     _uint_image_create_DDB (
                         device_context, image_data->size.width,
                         image_data->size.height, image_data->depth,
                         image_data->data, data_arrays, count_arrays,
                         array_count, colors, tint)) != (HBITMAP) NULL)
                {
                    error_text = (char *) NULL;
                }
                else
                {
                    error_text = "DDB";
                }
            }

            /*
            ** Use a DIB (slower) on a non-32-bit display
            */

            else if ((bitmap =
                      _uint_image_create_DIB (
                          device_context, image_data->size.width,
                          image_data->size.height, image_data->data, tint,
                          (flags & UI_IMAGE_IS_PREMULTIPLIED),
                          !(flags & UI_IMAGE_IS_NOT_PREMULTIPLIED)))
                          != (HBITMAP) NULL)
            {
                error_text = (char *) NULL;
            }

            /*
            ** Use a DDB (faster) as a fallback
            */

            else if ((bitmap =
                      _uint_image_create_compatible (
                          device_context, image_data->size.width,
                          image_data->size.height, image_data->data, tint,
                          (flags & UI_IMAGE_IS_PREMULTIPLIED),
                          !(flags & UI_IMAGE_IS_NOT_PREMULTIPLIED)))
                          != (HBITMAP) NULL)
            {
                error_text = "DIB";
            }
            else
            {
                error_text = "DIB or DDB";
            }

            if (error_text != (char *) NULL)
            {
                _ui_error_msg (
                    UIMSG_APP | UIMSG_DBG_CRASH,
                    error_message,
                    image_data->size.width,
                    image_data->size.height,
                    error_text,
                    GetLastError (),
                    image_data->name);
            }

            ReleaseDC (GetDesktopWindow (), device_context);

            image_data->sysdep_image = (void *) bitmap;
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_destroy
| Purpose:  Destroy the given bitmap
| Input:    image_data  - the image data structure
| Output:   image_data  - the image data structure
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_destroy (ui_image_data_t *image_data)
{
#ifdef UI_SYSTEM_NT

    int status = UI_SUCCESS;

    if (image_data->sysdep_image != NULL)
    {
        DeleteBitmap ((HBITMAP) image_data->sysdep_image);
        image_data->sysdep_image = NULL;
    }

    if (image_data->sysdep_image2 != NULL)
    {
        _uint_image_d2d_destroy ((ID2D1Bitmap *) image_data->sysdep_image2);
        image_data->sysdep_image2 = NULL;
    }

    if (image_data->sysdep_mask_image != NULL)
    {
        DeleteBitmap ((HBITMAP) image_data->sysdep_mask_image);
        image_data->sysdep_mask_image = NULL;
    }

    if (image_data->sysdep_grey_image != NULL)
    {
        DeleteBitmap ((HBITMAP) image_data->sysdep_grey_image);
        image_data->sysdep_grey_image = NULL;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_cursor
| Purpose:  Create a cursor from the given image
| Input:    image_data  - the image data structure
| Output:   image_data  - the image data structure
| Return:   UI_SUCCESS if everything went well, othersize UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_create_cursor (ui_image_data_t *image_data)
{
#ifdef UI_SYSTEM_NT

    TCHAR              *id;
    int                 is_cursor;
    ui_image_data_t     data = *image_data;
    ui_size_t           size;
    int                 i;
    int                 old_bpr, new_bpr;
    unsigned char      *old_data, *new_data;
    HDC                 device_context;
    HDC                 mem_device_context;
    HDC                 bitmap_device_context;
    HBITMAP             mask_bitmap = (HBITMAP) NULL, bitmap = (HBITMAP) NULL;
    HBITMAP             sys_mem_bitmap, sys_bitmap;
    HBRUSH              brush;
    RECT                rect;
    ICONINFO            icon_info;
    HCURSOR             cursor;
    int                 status = UI_ERROR;

    if ((id =
         _uint_image_get_icon_handle (image_data, &is_cursor, (int *) NULL))
            != (TCHAR *) NULL &&
        is_cursor)
    {
        cursor = LoadCursor ((HINSTANCE) NULL, id);
    }
    else
    {
        size.width = GetSystemMetrics (SM_CXCURSOR);
        size.height = GetSystemMetrics (SM_CYCURSOR);

        _ui_cursor_get_size (&size);

        _ui_image_of_size (
            &data,
            ((data.size.width * size.width) / 32),
            ((data.size.height * size.height) / 32),
            0,
            TRUE);

        _ui_image_inquire (&data, UI_IMAGE_IS_NOT_PREMULTIPLIED);

        if (data.size.width != size.width ||
            data.size.height != size.height)
        {
            old_bpr = (((data.size.width + 15) / 16) * 2);
            new_bpr = (((size.width + 15) / 16) * 2);

            old_data = GET_ARRAY (unsigned char, (old_bpr * data.size.height));
            new_data = GET_ARRAY (unsigned char, (new_bpr * size.height));

            GetBitmapBits (
                (HBITMAP) data.sysdep_mask_image,
                (old_bpr * data.size.height),
                old_data);

            memset (new_data, 255, (new_bpr * size.height));

            for (i = 0; i < MIN (size.height, data.size.height); i++)
            {
                memcpy (
                    (new_data + (i * new_bpr)),
                    (old_data + (i * old_bpr)),
                    MIN (new_bpr, old_bpr));
            }

            mask_bitmap =
                (void *)
                    CreateBitmap (
                        size.width,
                        size.height,
                        1,
                        1,
                        (void *) new_data);

            relmem (&old_data);
            relmem (&new_data);

            device_context = GetDC (GetDesktopWindow ());

            bitmap =
                CreateCompatibleBitmap (
                    device_context,
                    size.width,
                    size.height);

            mem_device_context = CreateCompatibleDC (device_context);
            sys_mem_bitmap = SelectBitmap (mem_device_context, bitmap);

            brush = CreateSolidBrush (COLOR_BLACK);
            SetRect (&rect, 0, 0, size.width, size.height);
            FillRect (mem_device_context, &rect, brush);
            DeleteBrush (brush);

            bitmap_device_context = CreateCompatibleDC (device_context);

            sys_bitmap =
                SelectBitmap (
                    bitmap_device_context,
                    (HBITMAP) data.sysdep_image);

            BitBlt (
                mem_device_context,
                0,
                0,
                size.width,
                size.height,
                bitmap_device_context,
                0,
                0,
                SRCCOPY);

            SelectBitmap (bitmap_device_context, sys_bitmap);
            DeleteDC (bitmap_device_context);

            SelectBitmap (mem_device_context, sys_mem_bitmap);
            DeleteDC (mem_device_context);

            ReleaseDC (GetDesktopWindow (), device_context);

            icon_info.hbmMask = mask_bitmap;
            icon_info.hbmColor = bitmap;
        }
        else
        {
            icon_info.hbmMask = (HBITMAP) data.sysdep_mask_image;
            icon_info.hbmColor = (HBITMAP) data.sysdep_image;
        }

        icon_info.fIcon = FALSE;

        if (data.hotspot.x < 0 ||
            data.hotspot.y < 0)
        {
            icon_info.xHotspot = (data.size.width / 2);
            icon_info.yHotspot = (data.size.height / 2);
        }
        else
        {
            icon_info.xHotspot = data.hotspot.x;
            icon_info.yHotspot = data.hotspot.y;
        }

        cursor = CreateIconIndirect (&icon_info);

        if (mask_bitmap != (HBITMAP) NULL)
        {
            DeleteBitmap (mask_bitmap);
        }

        if (bitmap != (HBITMAP) NULL)
        {
            DeleteBitmap (bitmap);
        }

        _ui_image_free (&data);
    }

    if (cursor != (HCURSOR) NULL)
    {
        image_data->sysdep_cursor = (void *) cursor;
        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_destroy_cursor
| Purpose:  Destroy the given cursor
| Input:    image_data  - the image data structure
| Output:   image_data  - the image data structure
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_destroy_cursor (ui_image_data_t *image_data)
{
#ifdef UI_SYSTEM_NT

    int is_cursor;
    int status = UI_SUCCESS;

    if (image_data->sysdep_cursor != NULL)
    {
        if (_uint_image_get_icon_handle (
                image_data, &is_cursor, (int *) NULL) == (TCHAR *) NULL ||
            !is_cursor)
        {
            DestroyIcon ((HCURSOR) image_data->sysdep_cursor);
        }

        image_data->sysdep_cursor = NULL;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_can_draw_accelerated
| Purpose:  Determine whether the image should try and use acceleration
| Input:    image_data  - image data structure
|           flags       - the image flags
| Output:
| Return:   UI_SUCCESS if the image should try and use acceleration
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_can_draw_accelerated (ui_image_data_t *image_data,
                                                int flags)
{
#ifdef UI_SYSTEM_NT

    return (_uint_image_d2d_renderer (
                (ID2D1RenderTarget **) NULL, image_data->context, flags) ?
            UI_SUCCESS :
            UI_ERROR);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_draw
| Purpose:  Draw the given image using the given mask, at the given position
|           using the given context
| Input:    image_data  - the image data structure
|           flags       - the image drawing flags
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_draw (ui_image_data_t *image_data, int flags)
{
#ifdef UI_SYSTEM_NT

    BLENDFUNCTION       blend_function =
        { AC_SRC_OVER, 0, 128, AC_SRC_ALPHA };
    HBITMAP             bitmap =
        (HBITMAP) image_data->sysdep_image;
    ID2D1Bitmap        *d2d_bitmap =
        (ID2D1Bitmap *) image_data->sysdep_image2;
    HBITMAP             mask_bitmap =
        (HBITMAP) image_data->sysdep_mask_image;
    HBITMAP             grey_bitmap =
        (HBITMAP) image_data->sysdep_grey_image;
    DIBSECTION          dib_data;
    HICON               icon = (HICON) NULL;
    ICONINFO            icon_info;
    int                 x = image_data->position.x;
    int                 y = image_data->position.y;
    int                 width = image_data->size.width;
    int                 height = image_data->size.height;
    int                 depth = image_data->depth;
    ui_gfx_t           *context = image_data->context;
    ID2D1RenderTarget  *d2d_renderer;
    ui_gfx_t            mem_context;
    COLORREF            sys_fg, sys_bg;
    int                 fg_color, bg_color;
    ui_gfx_t            alpha_context;
    ui_gfx_t           *draw_context;
    void               *stipple_brush;
    ui_gfx_t            stipple_context;
    HBITMAP             stipple_bitmap;
    RECT                rect;

    if (image_data->handle == DB_HANDLE_ERROR &&
        bitmap != (HBITMAP) NULL)
    {
        ZERO_OUT_STRUCT (dib_data);

        if ((icon = (HICON) bitmap) == INVALID_HANDLE_VALUE)
        {
            /*
            ** The handle is invalid
            */

            bitmap = mask_bitmap = (HBITMAP) NULL;
            icon = (HICON) NULL;
        }
        else
        {
            if (GetIconInfo (icon, &icon_info))
            {
                /*
                ** The handle is an icon
                */

                bitmap = icon_info.hbmColor;
                mask_bitmap = icon_info.hbmMask;
            }
            else
            {
                /*
                ** The handle is a bitmap and there is no separate mask
                */

                mask_bitmap = (HBITMAP) NULL;
                icon = (HICON) NULL;
            }

            if (GetObject (bitmap, sizeof (DIBSECTION), &dib_data) == 0 &&
                GetObject (bitmap, sizeof (BITMAP), &(dib_data.dsBm)) == 0)
            {
                /*
                ** The handle is not recognized
                */

                bitmap = mask_bitmap = (HBITMAP) NULL;
                icon = (HICON) NULL;
            }
        }

        if (bitmap != (HBITMAP) NULL)
        {
            width = (int) dib_data.dsBm.bmWidth;
            height = (int) dib_data.dsBm.bmHeight;
            depth = ((dib_data.dsBmih.biBitCount == 32) ? 32 : 24);
        }
        else
        {
            return (UI_ERROR);
        }
    }

    if ((d2d_bitmap == (ID2D1Bitmap *) NULL ||
         !(_uint_image_d2d_renderer (&d2d_renderer, context, flags)) ||
         !(_uint_image_d2d_draw (
               d2d_renderer, d2d_bitmap, x, y, width, height))) &&
        (bitmap != (HBITMAP) NULL ||
         (_ui_image_retrieve (image_data) == UI_SUCCESS &&
          (bitmap =
           (HBITMAP) image_data->sysdep_image) != (HBITMAP) NULL)))
    {
        _uint_d2d_renderer_destroy ((HDC) context->context);

        _ui_gfx_create_context (context, (void *) bitmap, &mem_context);

        SetMapMode (
            (HDC) mem_context.context,
            GetMapMode ((HDC) context->context));

        if (flags & (UI_IMAGE_DRAW_GREY | UI_IMAGE_DRAW_ONCE))
        {
            if (grey_bitmap != (HBITMAP) NULL)
            {
                /*
                ** Draw the image disabled
                */

                SelectBitmap ((HDC) mem_context.context, grey_bitmap);
                sys_fg = GetTextColor ((HDC) context->context);

                if ((flags & (UI_IMAGE_DRAW_GREY | UI_IMAGE_DRAW_ONCE))
                    != UI_IMAGE_DRAW_ONCE)
                {
                    /*
                    ** Draw the light shadow, offset by a pixel
                    */

                    if (!(flags & UI_IMAGE_DRAW_ONCE))
                    {
                        SetTextColor (
                            (HDC) context->context,
                            (COLORREF)
                                _ui_color_get_color (
                                    UI_COLOR_3D_VERY_LIGHT_SHADOW));

                        MaskBlt (
                            (HDC) context->context,
                            (x + 1),
                            (y + 1),
                            width,
                            height,
                            (HDC) mem_context.context,
                            0,
                            0,
                            grey_bitmap,
                            0,
                            0,
                            MAKEROP4 (DSTCOPY, SRCCOPY));
                    }

                    /*
                    ** Draw the dark shadow
                    */

                    SetTextColor (
                        (HDC) context->context,
                        (COLORREF)
                            _ui_color_get_color (
                                UI_COLOR_3D_DARK_SHADOW));
                }

                /*
                ** Draw the monochrome disabled image
                */

                MaskBlt (
                    (HDC) context->context,
                    x,
                    y,
                    width,
                    height,
                    (HDC) mem_context.context,
                    0,
                    0,
                    grey_bitmap,
                    0,
                    0,
                    MAKEROP4 (DSTCOPY, SRCCOPY));

                SetTextColor ((HDC) context->context, sys_fg);
            }
        }
        else
        {
            /*
            ** Draw the image normally
            */

            if (depth == 1)
            {
                SelectBitmap ((HDC) mem_context.context, grey_bitmap);
            }

            draw_context = &mem_context;

            if (flags & UI_IMAGE_DRAW_SELECTED)
            {
                /*
                ** Draw the image selected with an alpha-blend
                */

                if (_ui_gfx_create_memory_context (
                        context, width, height,
                        &alpha_context) == UI_SUCCESS)
                {
                    fg_color =
                        _ui_gfx_get_fg_color_value (&alpha_context);

                    bg_color =
                        _ui_gfx_get_bg_color_value (image_data->context);

                    _ui_gfx_set_fg_color_value (&alpha_context, bg_color);

                    _ui_gfx_fill_rect (
                        &alpha_context,
                        NULL,
                        0,
                        0,
                        width,
                        height);

                    _ui_gfx_set_fg_color_value (&alpha_context, fg_color);

                    if ((depth == 32 &&
                         AlphaBlend (
                             (HDC) alpha_context.context, 0, 0, width, height,
                             (HDC) mem_context.context, 0, 0, width, height,
                             blend_function)) ||
                        _ui_gfx_alpha_blend (
                            &alpha_context, 0, 0, width, height, &mem_context,
                            0, 0, 128, UI_OR) == UI_SUCCESS)
                    {
                        draw_context = &alpha_context;
                    }
                    else
                    {
                        /*
                        ** The alpha-blending failed, so use stippling instead
                        */

                        _ui_gfx_destroy_context (&alpha_context);
                    }
                }
            }

            if (icon != (HICON) NULL)
            {
                /*
                ** Draw the icon
                */

                DrawIconEx (
                    (HDC) context->context,
                    x,
                    y,
                    icon,
                    width,
                    height,
                    0,
                    (HBRUSH) NULL,
                    (DI_NORMAL | DI_COMPAT));
            }
            else if (((image_data->handle != DB_HANDLE_ERROR) ?
                      (_ui_krn_image_get_flags (image_data->handle)
                           & UI_IMAGE_IS_OPAQUE) :
                      (depth < 32)) != 0)
            {
                /*
                ** Draw the opaque image
                */

                _ui_gfx_copy_area (
                    context,
                    x,
                    y,
                    width,
                    height,
                    draw_context,
                    0,
                    0,
                    UI_COPY);
            }
            else if (depth == 32 &&
                     (draw_context != &alpha_context ||
                      alpha_context.depth == 32))
            {
                /*
                ** Draw the 32-bit image
                */

                _ui_gfx_alpha_blend (
                    context,
                    x,
                    y,
                    width,
                    height,
                    draw_context,
                    0,
                    0,
                    255,
                    ((flags & UI_IMAGE_DRAW_OPAQUE) ? UI_OR : UI_COPY));
            }
            else
            {
                /*
                ** Draw the non-rectangular image
                */

                _ui_gfx_mask_area (
                    context,
                    x,
                    y,
                    width,
                    height,
                    draw_context,
                    0,
                    0,
                    (void *) mask_bitmap,
                    0,
                    0,
                    UI_COPY);
            }

            if (flags & UI_IMAGE_DRAW_SELECTED)
            {
                if (draw_context == &alpha_context)
                {
                    /*
                    ** Free the alpha-blending context used for selection
                    */

                    _ui_gfx_destroy_context (&alpha_context);
                }
                else if ((stipple_brush =
                          _ui_gfx_create_brush (
                              context, UI_BAD_COLOR_RGB,
                              UI_MEDIUM_STIPPLE)) != NULL)
                {
                    /*
                    ** Draw the image selected with stippling
                    */

                    stipple_bitmap = CreateBitmap (width, height, 1, 1, NULL);
                    SetRect (&rect, 0, 0, width, height);

                    if (_ui_gfx_create_context (
                            context, (void *) stipple_bitmap,
                            &stipple_context) == UI_SUCCESS)
                    {
                        SetMapMode (
                            (HDC) stipple_context.context,
                            GetMapMode ((HDC) context->context));

                        SetTextColor ((HDC) stipple_context.context, COLOR_BLACK);
                        SetBkColor ((HDC) stipple_context.context, COLOR_BLACK);
                        SetROP2 ((HDC) stipple_context.context, R2_COPYPEN);

                        FillRect (
                            (HDC) stipple_context.context,
                            &rect,
                            stipple_brush);

                        if (mask_bitmap != (HBITMAP) NULL)
                        {
                            SelectBitmap ((HDC) mem_context.context, mask_bitmap);

                            BitBlt (
                                (HDC) stipple_context.context,
                                0,
                                0,
                                width,
                                height,
                                (HDC) mem_context.context,
                                0,
                                0,
                                NOTSRCAND);
                        }

                        sys_fg =
                            SetTextColor ((HDC) context->context, COLOR_WHITE);

                        sys_bg =
                            SetBkColor ((HDC) context->context, COLOR_BLACK);

                        BitBlt (
                            (HDC) context->context,
                            x,
                            y,
                            width,
                            height,
                            (HDC) stipple_context.context,
                            0,
                            0,
                            SRCAND);

                        SetTextColor ((HDC) context->context, COLOR_BLACK);
                        _ui_gfx_set_bg_color_value (context, bg_color);

                        BitBlt (
                            (HDC) context->context,
                            x,
                            y,
                            width,
                            height,
                            (HDC) stipple_context.context,
                            0,
                            0,
                            SRCPAINT);

                        SetTextColor ((HDC) context->context, sys_fg);
                        SetBkColor ((HDC) context->context, sys_bg);

                        _ui_gfx_destroy_context (&stipple_context);
                    }

                    DeleteBitmap (stipple_bitmap);

                    _ui_gfx_destroy_brush (context, &stipple_brush);
                }
            }
        }

        _ui_gfx_destroy_context (&mem_context);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_stretch
| Purpose:  Copy the given area from the given image data to the given
|           context, resizing or mirroring as necessary
| Input:    dest_context        - the destination context
|           dest_rect           - the destination rectangle
|           src_data            - the source image data
|           src_flags           - the source image flags
|           src_rect            - the source rectangle
|           clip_region         - the region to clip within
|           margins             - the sizing margins for the area
|           tile_horz           - flag indicating horizontal tiling
|           tile_vert           - flag indicating vertical tiling
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_stretch (ui_gfx_t *dest_context,
                                   const ui_rect_t *dest_rect,
                                   ui_image_data_t *src_data, int src_flags,
                                   const ui_rect_t *src_rect,
                                   void *clip_region,
                                   const ui_margin_t *margins,
                                   int tile_horz, int tile_vert)
{
#ifdef UI_SYSTEM_NT

    HRGN        orig_clip_region = (HRGN) NULL;
    POINT       origin;
    HDC         src_dc, dest_dc;
    HBITMAP     sys_bitmap;
    void      (*stretch) (HDC, int, int, int, int, HDC, int, int, int, int, int);
    int         left, top, right, bottom;
    int         dx, dy, dw, dh;
    int         sx, sy, sw, sh;
    void       *meta_region;

    if ((tile_horz ||
         tile_vert) &&
        (!(src_flags & UI_IMAGE_IS_OPAQUE) ||
         src_data->depth >= 32))
    {
        /*
        ** Do not use PatBlt for tiling non-opaque or 32-bit images
        */

        return (UI_ERROR);
    }
    else if (src_data->depth < 32 &&
             !(src_flags & UI_IMAGE_IS_OPAQUE))
    {
        /*
        ** Do not use BitBlt/StretchBlt for non-opaque images
        */

        return (UI_ERROR);
    }
    else if (_uint_image_d2d_renderer (
                 (ID2D1RenderTarget **) NULL, dest_context, 0))
    {
        /*
        ** Do not use BitBlt/StretchBlt for Direct2D images
        */

        return (UI_ERROR);
    }

    _uint_d2d_renderer_destroy ((HDC) dest_context->context);

    stretch =
        (((src_flags & UI_IMAGE_IS_OPAQUE) ||
          src_data->depth < 32) ?
         _uint_image_stretch_section :
         _uint_image_blend_section);

    dest_dc = (HDC) dest_context->context;
    SetStretchBltMode (dest_dc, COLORONCOLOR);

    if (dest_context->region != NULL ||
        clip_region != NULL)
    {
        orig_clip_region = CreateRectRgn (0, 0, 0, 0);

        if (GetClipRgn (dest_dc, orig_clip_region) != 1)
        {
            DeleteRgn (orig_clip_region);
            orig_clip_region = (HRGN) NULL;
        }

        if (dest_context->region != NULL)
        {
            _ui_gfx_set_clip_region (
                dest_context,
                dest_context->region,
                UI_AND);
        }

        if (clip_region != NULL)
        {
            _ui_gfx_set_clip_region (
                dest_context,
                clip_region,
                UI_AND);
        }
    }

    if (tile_horz ||
        tile_vert)
    {
        GetBrushOrgEx (dest_dc, &origin);
    }

    src_dc = (HDC) CreateCompatibleDC (dest_dc);
    sys_bitmap = SelectBitmap (src_dc, (HBITMAP) src_data->sysdep_image);

    if (margins != (const ui_margin_t *) NULL)
    {
        left = margins->left;
        top = margins->top;
        right = margins->right;
        bottom = margins->bottom;
    }
    else
    {
        left = top = right = bottom = 0;
    }

    /*
    ** Top row
    */

    dh = MIN (top, dest_rect->height);

    if (dh > 0)
    {
        dy = dest_rect->y;
        sy = src_rect->y;

        /*
        ** Top-left corner
        */

        dw = MIN (left, dest_rect->width);

        if (dw > 0)
        {
            dx = dest_rect->x;
            sx = src_rect->x;

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                dw,
                dh,
                4);
        }

        /*
        ** Top-center section
        */

        dw = (dest_rect->width - left - right);

        if (dw > 0)
        {
            sw = (src_rect->width - left - right);

            dx = (dest_rect->x + left);
            sx = (src_rect->x + left);

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                sw,
                dh,
                tile_horz);
        }

        /*
        ** Top-right corner
        */

        dw = MIN (right, dest_rect->width);

        if (dw > 0)
        {
            dx = (dest_rect->x + dest_rect->width - dw);
            sx = (src_rect->x + src_rect->width - dw);

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                dw,
                dh,
                4);
        }
    }

    /*
    ** Middle row
    */

    dh = (dest_rect->height - top - bottom);

    if (dh > 0)
    {
        sh = (src_rect->height - top - bottom);

        dy = (dest_rect->y + top);
        sy = (src_rect->y + top);

        /*
        ** Middle-left section
        */

        dw = MIN (left, dest_rect->width);

        if (dw > 0)
        {
            dx = dest_rect->x;
            sx = src_rect->x;

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                dw,
                sh,
                (tile_vert << 1));
        }

        /*
        ** Middle-center section
        */

        dw = (dest_rect->width - left - right);

        if (dw > 0)
        {
            sw = (src_rect->width - left - right);

            dx = (dest_rect->x + left);
            sx = (src_rect->x + left);

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                sw,
                sh,
                (tile_horz | (tile_vert << 1)));
        }

        /*
        ** Middle-right section
        */

        dw = MIN (right, dest_rect->width);

        if (dw > 0)
        {
            dx = (dest_rect->x + dest_rect->width - dw);
            sx = (src_rect->x + src_rect->width - dw);

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                dw,
                sh,
                (tile_vert << 1));
        }
    }

    /*
    ** Bottom row
    */

    dh = MIN (bottom, dest_rect->height);

    if (dh > 0)
    {
        dy = (dest_rect->y + dest_rect->height - dh);
        sy = (src_rect->y + src_rect->height - dh);

        /*
        ** Bottom-left corner
        */

        dw = MIN (left, dest_rect->width);

        if (dw > 0)
        {
            dx = dest_rect->x;
            sx = src_rect->x;

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                dw,
                dh,
                4);
        }

        /*
        ** Bottom-center section
        */

        dw = (dest_rect->width - left - right);

        if (dw > 0)
        {
            sw = (src_rect->width - left - right);

            dx = (dest_rect->x + left);
            sx = (src_rect->x + left);

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                sw,
                dh,
                tile_horz);
        }

        /*
        ** Bottom-right corner
        */

        dw = MIN (right, dest_rect->width);

        if (dw > 0)
        {
            dx = (dest_rect->x + dest_rect->width - dw);
            sx = (src_rect->x + src_rect->width - dw);

            (*stretch) (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                dw,
                dh,
                4);
        }
    }

    if (dest_context->depth >= 32 &&
        src_data->depth < 32 &&
        stretch == _uint_image_stretch_section)
    {
        meta_region = _ui_gfx_region_create_from_rect (dest_rect);

        if (clip_region != NULL)
        {
            _ui_gfx_region_intersect (meta_region, clip_region);
        }

        _ui_gfx_fill_region (dest_context, (void *) 255, meta_region);

        _ui_gfx_region_destroy (&meta_region);
    }

    if (dest_context->region != NULL ||
        clip_region != NULL)
    {
        SelectClipRgn (dest_dc, orig_clip_region);

        if (orig_clip_region != (HRGN) NULL)
        {
            DeleteRgn (orig_clip_region);
        }
    }

    SelectBitmap (src_dc, sys_bitmap);
    DeleteDC (src_dc);

    if (tile_horz ||
        tile_vert)
    {
        SetBrushOrgEx (dest_dc, origin.x, origin.y, (POINT *) NULL);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_DDB
| Purpose:  Create a Device Dependent Bitmap (DDB)
| Input:    device_context  - the device context of the desktop
|           width           - the image width
|           height          - the image height
|           depth           - the image depth (guaranteed less than 32)
|           data            - the pixel data
|           data_arrays     - array of arrays of points to draw
|           count_arrays    - array of number of points to draw
|           array_count     - number of above arrays
|           colors          - the mappings of color indices to color values
|           tint            - TRUE to apply tinting to the image
| Output:
| Return:   The created DDB
\*--------------------------------------------------------------------------*/
static HBITMAP _uint_image_create_DDB (HDC device_context, int width,
                                       int height, int depth,
                                       unsigned char *data,
                                       ui_point_t **data_arrays,
                                       int *count_arrays, int array_count,
                                       int *colors, int tint)
{
    ui_rgb_t       *rgbs = _ui_color_get_rgbs ();
    BITMAPINFO      color_info;
    int             color_bpr = ((((width * 24) + 31) & ~31) >> 3);
    unsigned char  *color_data;
    int             x, y;
    ui_rgb_t        rgb;
    unsigned char   r, g, b;
    unsigned char  *line;
    COLORREF        color;
    HBITMAP         bitmap = (HBITMAP) NULL;

    ZERO_OUT_STRUCT (color_info);

    color_info.bmiHeader.biSize = sizeof (color_info);
    color_info.bmiHeader.biWidth = width;
    color_info.bmiHeader.biHeight = height;
    color_info.bmiHeader.biPlanes = 1;
    color_info.bmiHeader.biBitCount = 24;
    color_info.bmiHeader.biCompression = BI_RGB;
    color_info.bmiHeader.biSizeImage = (color_bpr * height);

    color_data = GET_ARRAY (unsigned char, color_info.bmiHeader.biSizeImage);

    if (depth < 24)
    {
        for (x = 0; x < array_count; x++)
        {
            if (count_arrays [x] > 0)
            {
                rgb = rgbs [(depth > 4) ? x : colors [x]];

                if (tint)
                {
                    rgb = _ui_color_tint (rgb);
                }

                r = (unsigned char) UI_COLOR_RED_VALUE (rgb);
                g = (unsigned char) UI_COLOR_GREEN_VALUE (rgb);
                b = (unsigned char) UI_COLOR_BLUE_VALUE (rgb);

                for (y = 0; y < count_arrays [x]; y++)
                {
                    line =
                        (color_data +
                         ((height - 1 - data_arrays [x][y].y) *
                          color_bpr) +
                         (data_arrays [x][y].x * 3));

                    line [2] = r;
                    line [1] = g;
                    line [0] = b;
                }
            }
        }
    }
    else if (tint)
    {
        for (y = (height - 1); y >= 0; y--)
        {
            for (line = (color_data + (y * color_bpr)), x = 0;
                 x < width;
                 line += 3, x++, data += 4)
            {
                if (!(UI_COLOR_IS_RGB (data [3] << 24)))
                {
                    rgb = _ui_color_tint (rgbs [data [0]]);

                    color =
                        RGB (
                            UI_COLOR_RED_VALUE (rgb),
                            UI_COLOR_GREEN_VALUE (rgb),
                            UI_COLOR_BLUE_VALUE (rgb));
                }
                else if (UI_COLOR_IS_TRANSPARENT_RGB (data [3] << 24))
                {
                    color = COLOR_BLACK;
                }
                else
                {
                    rgb =
                        _ui_color_tint (
                            UI_COLOR_RGB (data [0], data [1], data [2]));

                    color =
                        RGB (
                            UI_COLOR_RED_VALUE (rgb),
                            UI_COLOR_GREEN_VALUE (rgb),
                            UI_COLOR_BLUE_VALUE (rgb));
                }

                line [2] = (unsigned char) GetRValue (color);
                line [1] = (unsigned char) GetGValue (color);
                line [0] = (unsigned char) GetBValue (color);
            }
        }
    }
    else
    {
        for (y = (height - 1); y >= 0; y--)
        {
            for (line = (color_data + (y * color_bpr)), x = 0;
                 x < width;
                 line += 3, x++, data += 4)
            {
                if (!(UI_COLOR_IS_RGB (data [3] << 24)))
                {
                    color = (COLORREF) rgbs [data [0]];
                }
                else if (UI_COLOR_IS_TRANSPARENT_RGB (data [3] << 24))
                {
                    color = COLOR_BLACK;
                }
                else
                {
                    color = RGB (data [0], data [1], data [2]);
                }

                line [2] = (unsigned char) GetRValue (color);
                line [1] = (unsigned char) GetGValue (color);
                line [0] = (unsigned char) GetBValue (color);
            }
        }
    }

    bitmap =
        CreateDIBitmap (
            device_context,
            &(color_info.bmiHeader),
            CBM_INIT,
            (VOID *) color_data,
            &color_info,
            DIB_RGB_COLORS);

    relmem (&color_data);

    return (bitmap);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_DIB
| Purpose:  Create a Device Independent Bitmap (DIB)
| Input:    device_context      - the device context of the desktop
|           width               - the image width
|           height              - the image height
|           data                - the 32bpp pixel data
|           tint                - TRUE to apply tinting to the image
|           is_premultiplied    - TRUE if the pixel data is premultiplied
|           want_premultiplied  - TRUE if the DIB data must be premultiplied
| Output:
| Return:   The created DIB
\*--------------------------------------------------------------------------*/
static HBITMAP _uint_image_create_DIB (HDC device_context, int width,
                                       int height, unsigned char *data,
                                       int tint, int is_premultiplied,
                                       int want_premultiplied)
{
    BITMAPV5INFO    color_info;
    int             color_bpr = ((((width << 5) + 31) & ~31) >> 3);
    unsigned char  *color_data;
    HBITMAP         bitmap;

    ZERO_OUT_STRUCT (color_info);

    color_info.bmiHeader.bV5Size = sizeof (color_info);
    color_info.bmiHeader.bV5Width = width;
    color_info.bmiHeader.bV5Height = height;
    color_info.bmiHeader.bV5Planes = 1;
    color_info.bmiHeader.bV5BitCount = 32;
    color_info.bmiHeader.bV5Compression = BI_BITFIELDS;
    color_info.bmiHeader.bV5SizeImage = (color_bpr * height);
    color_info.bmiHeader.bV5RedMask = 0x00ff0000U;
    color_info.bmiHeader.bV5GreenMask = 0x0000ff00U;
    color_info.bmiHeader.bV5BlueMask = 0x000000ffU;
    color_info.bmiHeader.bV5AlphaMask = 0xff000000U;
    color_info.bmiHeader.bV5CSType = LCS_DEVICE_CMYK;
    color_info.bmiHeader.bV5Intent = LCS_GM_IMAGES;

    color_info.bmiColors [0].rgbRed = 0xff;
    color_info.bmiColors [1].rgbGreen = 0xff;
    color_info.bmiColors [2].rgbBlue = 0xff;

    if ((bitmap =
         CreateDIBSection (
             device_context, (BITMAPINFO *) &color_info,
             DIB_RGB_COLORS, (VOID **) &color_data,
             (HANDLE) NULL, 0)) != (HBITMAP) NULL &&
        color_data != (unsigned char *) NULL)
    {
        GdiFlush ();

        _uint_image_create_DIB_data (
            color_data,
            color_bpr,
            &color_info,
            width,
            height,
            32,
            data,
            (ui_point_t **) NULL,
            (int *) NULL,
            0,
            (int *) NULL,
            tint,
            is_premultiplied,
            want_premultiplied,
            TRUE);
    }
    else if (bitmap != (HBITMAP) NULL)
    {
        DeleteBitmap (bitmap);

        bitmap = (HBITMAP) NULL;
    }

    return (bitmap);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_compatible
| Purpose:  Create a compatible bitmap
| Input:    device_context      - the device context of the desktop
|           width               - the image width
|           height              - the image height
|           data                - the 32bpp pixel data
|           tint                - TRUE to apply tinting to the image
|           is_premultiplied    - TRUE if the pixel data is premultiplied
|           want_premultiplied  - TRUE if the DIB data must be premultiplied
| Output:
| Return:   The created compatible bitmap
\*--------------------------------------------------------------------------*/
static HBITMAP _uint_image_create_compatible (HDC device_context, int width,
                                              int height, unsigned char *data,
                                              int tint, int is_premultiplied,
                                              int want_premultiplied)
{
    BITMAPV5INFO    color_info;
    int             color_bpr;
    unsigned char  *color_data;
    HBITMAP         bitmap;

    if ((bitmap =
         CreateCompatibleBitmap (device_context, width, height))
            != (HBITMAP) NULL)
    {
        ZERO_OUT_STRUCT (color_info);

        color_info.bmiHeader.bV5Size = sizeof (color_info);
        color_info.bmiHeader.bV5BitCount = 0;

        GetDIBits (
            device_context,
            bitmap,
            0,
            0,
            NULL,
            (BITMAPINFO *) &color_info,
            DIB_RGB_COLORS);

        color_info.bmiHeader.bV5BitCount = 32;
        color_info.bmiHeader.bV5Compression = BI_BITFIELDS;
        color_info.bmiHeader.bV5SizeImage = 0;

        GetDIBits (
            device_context,
            bitmap,
            0,
            color_info.bmiHeader.bV5Height,
            NULL,
            (BITMAPINFO *) &color_info,
            DIB_RGB_COLORS);

        color_bpr =
            (color_info.bmiHeader.bV5SizeImage /
             MAX (color_info.bmiHeader.bV5Height, 1));

        color_data =
            GET_ARRAY (
                unsigned char,
                color_info.bmiHeader.bV5SizeImage);

        _uint_image_create_DIB_data (
            color_data,
            color_bpr,
            &color_info,
            width,
            height,
            32,
            data,
            (ui_point_t **) NULL,
            (int *) NULL,
            0,
            (int *) NULL,
            tint,
            is_premultiplied,
            want_premultiplied,
            TRUE);

        SetDIBits (
            device_context,
            bitmap,
            0,
            color_info.bmiHeader.bV5Height,
            (VOID *) color_data,
            (BITMAPINFO *) &color_info,
            DIB_RGB_COLORS);

        relmem (&color_data);
    }

    return (bitmap);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_create_DIB_data
| Purpose:  Create Device Independent Bitmap (DIB) data for an image
| Input:    dib_bpr             - the DIB bytes-per-row
|           dib_color_info      - the DIB color info
|           width               - the image width
|           height              - the image height
|           depth               - the image depth
|           data                - the pixel data
|           data_arrays         - array of arrays of points to draw
|           count_arrays        - array of number of points to draw
|           array_count         - number of above arrays
|           colors              - the mappings of color indices to values
|           tint                - TRUE to apply tinting to the image
|           is_premultiplied    - TRUE if the pixel data is premultiplied
|           want_premultiplied  - TRUE if the DIB data must be premultiplied
|           flipped             - TRUE to flip the pixel data
| Output:   dib_data            - the DIB data
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_image_create_DIB_data (unsigned char *dib_data, int dib_bpr,
                                         BITMAPV5INFO *dib_color_info,
                                         int width, int height, int depth,
                                         unsigned char *data,
                                         ui_point_t **data_arrays,
                                         int *count_arrays, int array_count,
                                         int *colors, int tint,
                                         int is_premultiplied,
                                         int want_premultiplied, int flipped)
{
    unsigned int    rm, gm, bm, am, rs, gs, bs, as, m;
    ui_rgb_t       *rgbs;
    int             x, y, y0, y1, yi;
    unsigned char  *line, color8 [4];
    ui_rgb_t        rgb;
    COLORREF        color;

    if ((rm = dib_color_info->bmiHeader.bV5RedMask) == 0x00ff0000U)
    {
        rs = 16;
    }
    else
    {
        for (m = rm, rs = 0; m > 0 && !(m & 1); m >>= 1, rs++);
    }

    if ((gm = dib_color_info->bmiHeader.bV5GreenMask) == 0x0000ff00U)
    {
        gs = 8;
    }
    else
    {
        for (m = gm, gs = 0; m > 0 && !(m & 1); m >>= 1, gs++);
    }

    if ((bm = dib_color_info->bmiHeader.bV5BlueMask) == 0x000000ffU)
    {
        bs = 0;
    }
    else
    {
        for (m = bm, bs = 0; m > 0 && !(m & 1); m >>= 1, bs++);
    }

    if ((am = dib_color_info->bmiHeader.bV5AlphaMask) == 0)
    {
        dib_color_info->bmiHeader.bV5AlphaMask =
            am =
                ((rm | gm | bm) ^ 0xffffffffU);
    }

    if (am == 0xff000000U)
    {
        as = 24;
    }
    else
    {
        for (m = am, as = 0; m > 0 && !(m & 1); m >>= 1, as++);
    }

    if (flipped)
    {
        y0 = (height - 1);
        y1 = 0;
        yi = -1;
    }
    else
    {
        y0 = 0;
        y1 = (height - 1);
        yi = 1;
    }

    y1 += yi;

    if (depth >= 32)
    {
        if (tint)
        {
            if (is_premultiplied)
            {
                if (want_premultiplied)
                {
                    /*
                    ** tint = TRUE, is_pre = TRUE, want_pre = TRUE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            switch (data [3])
                            {
                                case 0:
                                {
                                    color = 0;
                                    break;
                                }

                                case 255u:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                data [0],
                                                data [1],
                                                data [2]));

                                    color =
                                        (((unsigned char)
                                              UI_COLOR_RED_VALUE (rgb)
                                              << rs) |
                                         ((unsigned char)
                                              UI_COLOR_GREEN_VALUE (rgb)
                                              << gs) |
                                         ((unsigned char)
                                              UI_COLOR_BLUE_VALUE (rgb)
                                              << bs) |
                                         (255u << as));

                                    break;
                                }

                                default:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                UI_IMAGE_UNPREMULTIPLY (
                                                    data [0],
                                                    data [3]),
                                                UI_IMAGE_UNPREMULTIPLY (
                                                    data [1],
                                                    data [3]),
                                                UI_IMAGE_UNPREMULTIPLY (
                                                    data [2],
                                                    data [3])));

                                    color =
                                        ((((unsigned char)
                                           UI_IMAGE_PREMULTIPLY (
                                               UI_COLOR_RED_VALUE (rgb),
                                               data [3])) << rs) |
                                         (((unsigned char)
                                           UI_IMAGE_PREMULTIPLY (
                                               UI_COLOR_GREEN_VALUE (rgb),
                                               data [3])) << gs) |
                                         (((unsigned char)
                                           UI_IMAGE_PREMULTIPLY (
                                               UI_COLOR_BLUE_VALUE (rgb),
                                               data [3])) << bs) |
                                         (data [3] << as));
                                }
                            }

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
                else
                {
                    /*
                    ** tint = TRUE, is_pre = TRUE, want_pre = FALSE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            switch (data [3])
                            {
                                case 0:
                                {
                                    color = 0;
                                    break;
                                }

                                case 255u:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                data [0],
                                                data [1],
                                                data [2]));

                                    color =
                                        (((unsigned char)
                                              UI_COLOR_RED_VALUE (rgb)
                                              << rs) |
                                         ((unsigned char)
                                              UI_COLOR_GREEN_VALUE (rgb)
                                              << gs) |
                                         ((unsigned char)
                                              UI_COLOR_BLUE_VALUE (rgb)
                                              << bs) |
                                         (255u << as));

                                    break;
                                }

                                default:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                UI_IMAGE_UNPREMULTIPLY (
                                                    data [0],
                                                    data [3]),
                                                UI_IMAGE_UNPREMULTIPLY (
                                                    data [1],
                                                    data [3]),
                                                UI_IMAGE_UNPREMULTIPLY (
                                                    data [2],
                                                    data [3])));

                                    color =
                                        (((unsigned char)
                                              UI_COLOR_RED_VALUE (rgb)
                                              << rs) |
                                         ((unsigned char)
                                              UI_COLOR_GREEN_VALUE (rgb)
                                              << gs) |
                                         ((unsigned char)
                                              UI_COLOR_BLUE_VALUE (rgb)
                                              << bs) |
                                         (data [3] << as));
                                }
                            }

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
            }
            else
            {
                if (want_premultiplied)
                {
                    /*
                    ** tint = TRUE, is_pre = FALSE, want_pre = TRUE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            switch (data [3])
                            {
                                case 0:
                                {
                                    color = 0;
                                    break;
                                }

                                case 255u:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                data [0],
                                                data [1],
                                                data [2]));

                                    color =
                                        (((unsigned char)
                                              UI_COLOR_RED_VALUE (rgb)
                                              << rs) |
                                         ((unsigned char)
                                              UI_COLOR_GREEN_VALUE (rgb)
                                              << gs) |
                                         ((unsigned char)
                                              UI_COLOR_BLUE_VALUE (rgb)
                                              << bs) |
                                         (255u << as));

                                    break;
                                }

                                default:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                data [0],
                                                data [1],
                                                data [2]));

                                    color =
                                        ((((unsigned char)
                                           UI_IMAGE_PREMULTIPLY (
                                               UI_COLOR_RED_VALUE (rgb),
                                               data [3])) << rs) |
                                         (((unsigned char)
                                           UI_IMAGE_PREMULTIPLY (
                                               UI_COLOR_GREEN_VALUE (rgb),
                                               data [3])) << gs) |
                                         (((unsigned char)
                                           UI_IMAGE_PREMULTIPLY (
                                               UI_COLOR_BLUE_VALUE (rgb),
                                               data [3])) << bs) |
                                         (data [3] << as));
                                }
                            }

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
                else
                {
                    /*
                    ** tint = TRUE, is_pre = FALSE, want_pre = FALSE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            switch (data [3])
                            {
                                case 0:
                                {
                                    color = 0;
                                    break;
                                }

                                default:
                                {
                                    rgb =
                                        _ui_color_tint (
                                            UI_COLOR_RGB (
                                                data [0],
                                                data [1],
                                                data [2]));

                                    color =
                                        (((unsigned char)
                                              UI_COLOR_RED_VALUE (rgb)
                                              << rs) |
                                         ((unsigned char)
                                              UI_COLOR_GREEN_VALUE (rgb)
                                              << gs) |
                                         ((unsigned char)
                                              UI_COLOR_BLUE_VALUE (rgb)
                                              << bs) |
                                         (data [3] << as));
                                }
                            }

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
            }
        }
        else
        {
            if (is_premultiplied)
            {
                if (want_premultiplied)
                {
                    /*
                    ** tint = FALSE, is_pre = TRUE, want_pre = TRUE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            color =
                                ((data [0] << rs) |
                                 (data [1] << gs) |
                                 (data [2] << bs) |
                                 (data [3] << as));

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
                else
                {
                    /*
                    ** tint = FALSE, is_pre = TRUE, want_pre = FALSE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            switch (data [3])
                            {
                                case 0:
                                {
                                    color = 0;
                                    break;
                                }

                                case 255u:
                                {
                                    color =
                                        ((data [0] << rs) |
                                         (data [1] << gs) |
                                         (data [2] << bs) |
                                         (255u << as));

                                    break;
                                }

                                default:
                                {
                                    rgb =
                                        UI_COLOR_RGB (
                                            UI_IMAGE_UNPREMULTIPLY (
                                                data [0],
                                                data [3]),
                                            UI_IMAGE_UNPREMULTIPLY (
                                                data [1],
                                                data [3]),
                                            UI_IMAGE_UNPREMULTIPLY (
                                                data [2],
                                                data [3]));

                                    color =
                                        (((unsigned char)
                                              UI_COLOR_RED_VALUE (rgb)
                                              << rs) |
                                         ((unsigned char)
                                              UI_COLOR_GREEN_VALUE (rgb)
                                              << gs) |
                                         ((unsigned char)
                                              UI_COLOR_BLUE_VALUE (rgb)
                                              << bs) |
                                         (data [3] << as));
                                }
                            }

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
            }
            else
            {
                if (want_premultiplied)
                {
                    /*
                    ** tint = FALSE, is_pre = FALSE, want_pre = TRUE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            switch (data [3])
                            {
                                case 0:
                                {
                                    color = 0;
                                    break;
                                }

                                case 255u:
                                {
                                    color =
                                        ((data [0] << rs) |
                                         (data [1] << gs) |
                                         (data [2] << bs) |
                                         (255u << as));

                                    break;
                                }

                                default:
                                {
                                    color =
                                        ((UI_IMAGE_PREMULTIPLY (
                                              data [0], data [3]) << rs) |
                                         (UI_IMAGE_PREMULTIPLY (
                                              data [1], data [3]) << gs) |
                                         (UI_IMAGE_PREMULTIPLY (
                                              data [2], data [3]) << bs) |
                                         (data [3] << as));
                                }
                            }

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
                else
                {
                    /*
                    ** tint = FALSE, is_pre = FALSE, want_pre = FALSE
                    */

                    for (y = y0; y != y1; y += yi)
                    {
                        for (line = (dib_data + (y * dib_bpr)), x = 0;
                             x < width;
                             line += 4, x++, data += 4)
                        {
                            color =
                                ((data [0] << rs) |
                                 (data [1] << gs) |
                                 (data [2] << bs) |
                                 (data [3] << as));

                            line [0] = (unsigned char) (color & 0xff);
                            line [1] = (unsigned char) ((color >> 8) & 0xff);
                            line [2] = (unsigned char) ((color >> 16) & 0xff);
                            line [3] = (unsigned char) ((color >> 24) & 0xff);
                        }
                    }
                }
            }
        }
    }
    else if (depth >= 24)
    {
        rgbs = _ui_color_get_rgbs ();

        if (tint)
        {
            for (y = y0; y != y1; y += yi)
            {
                for (line = (dib_data + (y * dib_bpr)), x = 0;
                     x < width;
                     line += 4, x++, data += 4)
                {
                    if (!(UI_COLOR_IS_RGB (data [3] << 24)))
                    {
                        if (data [0] == UI_COLOR_TRANSPARENT)
                        {
                            color = COLOR_BLACK;
                        }
                        else
                        {
                            rgb = _ui_color_tint (rgbs [data [0]]);

                            color =
                                (((unsigned char)
                                      UI_COLOR_RED_VALUE (rgb)
                                      << rs) |
                                 ((unsigned char)
                                      UI_COLOR_GREEN_VALUE (rgb)
                                      << gs) |
                                 ((unsigned char)
                                      UI_COLOR_BLUE_VALUE (rgb)
                                      << bs) |
                                 (255u << as));
                        }
                    }
                    else if (UI_COLOR_IS_TRANSPARENT_RGB (data [3] << 24))
                    {
                        color = COLOR_BLACK;
                    }
                    else
                    {
                        rgb =
                            _ui_color_tint (
                                UI_COLOR_RGB (
                                    data [0],
                                    data [1],
                                    data [2]));

                        color =
                            (((unsigned char)
                                  UI_COLOR_RED_VALUE (rgb)
                                  << rs) |
                             ((unsigned char)
                                  UI_COLOR_GREEN_VALUE (rgb)
                                  << gs) |
                             ((unsigned char)
                                  UI_COLOR_BLUE_VALUE (rgb)
                                  << bs) |
                             (255u << as));
                    }

                    line [0] = (unsigned char) (color & 0xff);
                    line [1] = (unsigned char) ((color >> 8) & 0xff);
                    line [2] = (unsigned char) ((color >> 16) & 0xff);
                    line [3] = (unsigned char) ((color >> 24) & 0xff);
                }
            }
        }
        else
        {
            for (y = y0; y != y1; y += yi)
            {
                for (line = (dib_data + (y * dib_bpr)), x = 0;
                     x < width;
                     line += 4, x++, data += 4)
                {
                    if (!(UI_COLOR_IS_RGB (data [3] << 24)))
                    {
                        if (data [0] == UI_COLOR_TRANSPARENT)
                        {
                            color = COLOR_BLACK;
                        }
                        else
                        {
                            rgb = rgbs [data [0]];

                            color =
                                (((unsigned char)
                                      UI_COLOR_RED_VALUE (rgb)
                                      << rs) |
                                 ((unsigned char)
                                      UI_COLOR_GREEN_VALUE (rgb)
                                      << gs) |
                                 ((unsigned char)
                                      UI_COLOR_BLUE_VALUE (rgb)
                                      << bs) |
                                 (255u << as));
                        }
                    }
                    else if (UI_COLOR_IS_TRANSPARENT_RGB (data [3] << 24))
                    {
                        color = COLOR_BLACK;
                    }
                    else
                    {
                        rgb = UI_COLOR_RGB (data [0], data [1], data [2]);

                        color =
                            (((unsigned char)
                                  UI_COLOR_RED_VALUE (rgb)
                                  << rs) |
                             ((unsigned char)
                                  UI_COLOR_GREEN_VALUE (rgb)
                                  << gs) |
                             ((unsigned char)
                                  UI_COLOR_BLUE_VALUE (rgb)
                                  << bs) |
                             (255u << as));
                    }

                    line [0] = (unsigned char) (color & 0xff);
                    line [1] = (unsigned char) ((color >> 8) & 0xff);
                    line [2] = (unsigned char) ((color >> 16) & 0xff);
                    line [3] = (unsigned char) ((color >> 24) & 0xff);
                }
            }
        }
    }
    else if (array_count > 0)
    {
        rgbs = _ui_color_get_rgbs ();

        for (x = 0; x < array_count; x++)
        {
            if (count_arrays [x] > 0)
            {
                rgb = rgbs [(depth > 4) ? x : colors [x]];

                if (tint)
                {
                    rgb = _ui_color_tint (rgb);
                }

                color =
                    (((unsigned char) UI_COLOR_RED_VALUE (rgb) << rs) |
                     ((unsigned char) UI_COLOR_GREEN_VALUE (rgb) << gs) |
                     ((unsigned char) UI_COLOR_BLUE_VALUE (rgb) << bs) |
                     (255u << as));

                color8 [0] = (unsigned char) (color & 0xff);
                color8 [1] = (unsigned char) ((color >> 8) & 0xff);
                color8 [2] = (unsigned char) ((color >> 16) & 0xff);
                color8 [3] = (unsigned char) ((color >> 24) & 0xff);

                for (y = 0; y < count_arrays [x]; y++)
                {
                    line =
                        (dib_data +
                         ((y0 + (yi * data_arrays [x][y].y)) *
                          dib_bpr) +
                         (data_arrays [x][y].x * 4));

                    line [0] = color8 [0];
                    line [1] = color8 [1];
                    line [2] = color8 [2];
                    line [3] = color8 [3];
                }
            }
        }
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_stretch_section
| Purpose:  Copy the given section from the given device context to the other
|           device context, resizing as necessary
| Input:    dest_dc             - the destination device context
|           dx                  - the destination rectangle left position
|           dy                  - the destination rectangle top position
|           dw                  - the destination rectangle width
|           dh                  - the destination rectangle height
|           src_dc              - the source device context
|           sx                  - the source rectangle left position
|           sy                  - the source rectangle top position
|           sw                  - the source rectangle width
|           sh                  - the source rectangle height
|           t                   - flags indicating tiling
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_image_stretch_section (HDC dest_dc, int dx, int dy,
                                         int dw, int dh, HDC src_dc,
                                         int sx, int sy, int sw, int sh,
                                         int t)
{
    HDC     dc;
    HBITMAP bitmap, sys_bitmap;
    int     bx, by, bw, bh;
    POINT   origin;
    HBRUSH  brush, sys_brush;

    switch (t)
    {
        case 4:
        {
            /*
            ** Copy
            */

            BitBlt (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                SRCCOPY);

            break;
        }

        case 0:
        {
            /*
            ** Stretch horizontally and vertically
            */

            StretchBlt (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                sw,
                sh,
                SRCCOPY);

            break;
        }

        default:
        {
            GetWindowOrgEx (dest_dc, &origin);

            bx = (dx - origin.x);
            by = (dy - origin.y);

            switch (t)
            {
                case 1:
                {
                    /*
                    ** Tile horizontally and stretch vertically
                    */

                    bw = sw;
                    bh = dh;

                    bx =
                        ((bx < 0) ?
                         (bw - ((-bx) % bw)) :
                         (bx % bw));

                    /*by = 0;*/

                    break;
                }

                case 2:
                {
                    /*
                    ** Stretch horizontally and tile vertically
                    */

                    bw = dw;
                    bh = sh;

                    /*bx = 0;*/

                    by =
                        ((by < 0) ?
                         (bh - ((-by) % bh)) :
                         (by % bh));

                    break;
                }

                case 3:
                {
                    /*
                    ** Tile horizontally and vertically
                    */

                    bw = sw;
                    bh = sh;

                    bx =
                        ((bx < 0) ?
                         (bw - ((-bx) % bw)) :
                         (bx % bw));

                    by =
                        ((by < 0) ?
                         (bh - ((-by) % bh)) :
                         (by % bh));

                    break;
                }
            }

            SetBrushOrgEx (dest_dc, bx, by, (POINT *) NULL);

            bitmap = CreateCompatibleBitmap (src_dc, bw, bh);

            dc = CreateCompatibleDC (src_dc);
            sys_bitmap = SelectBitmap (dc, bitmap);

            StretchBlt (dc, 0, 0, bw, bh, src_dc, sx, sy, sw, sh, SRCCOPY);

            SelectBitmap (dc, sys_bitmap);
            DeleteDC (dc);

            brush = CreatePatternBrush (bitmap);

            sys_brush = SelectBrush (dest_dc, brush);
            PatBlt (dest_dc, dx, dy, dw, dh, PATCOPY);
            SelectBrush (dest_dc, sys_brush);

            DeleteBrush (brush);

            DeleteBitmap (bitmap);
        }
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_blend_section
| Purpose:  Blend the given section from the given device context to the other
|           device context, resizing as necessary
| Input:    dest_dc             - the destination device context
|           dx                  - the destination rectangle left position
|           dy                  - the destination rectangle top position
|           dw                  - the destination rectangle width
|           dh                  - the destination rectangle height
|           src_dc              - the source device context
|           sx                  - the source rectangle left position
|           sy                  - the source rectangle top position
|           sw                  - the source rectangle width
|           sh                  - the source rectangle height
|           t                   - flags indicating tiling
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_image_blend_section (HDC dest_dc, int dx, int dy,
                                       int dw, int dh, HDC src_dc,
                                       int sx, int sy, int sw, int sh, int t)
{
    BLENDFUNCTION   blend_function = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    int             bx, by, bw, bh;
    POINT           origin;
    BITMAPV5INFO    color_info;
    unsigned char  *color_data;
    HDC             dc;
    HBITMAP         bitmap, sys_bitmap;
    HBRUSH          brush, sys_brush;

    switch (t)
    {
        case 4:
        case 0:
        {
            /*
            ** Stretch horizontally and vertically
            */

            AlphaBlend (
                dest_dc,
                dx,
                dy,
                dw,
                dh,
                src_dc,
                sx,
                sy,
                sw,
                sh,
                blend_function);

            break;
        }

        default:
        {
            GetWindowOrgEx (dest_dc, &origin);

            bx = (dx - origin.x);
            by = (dy - origin.y);

            switch (t)
            {
                case 1:
                {
                    /*
                    ** Tile horizontally and stretch vertically
                    */

                    bw = sw;
                    bh = dh;

                    bx =
                        ((bx < 0) ?
                         (bw - ((-bx) % bw)) :
                         (bx % bw));

                    by = 0;

                    break;
                }

                case 2:
                {
                    /*
                    ** Stretch horizontally and tile vertically
                    */

                    bw = dw;
                    bh = sh;

                    bx = 0;

                    by =
                        ((by < 0) ?
                         (bh - ((-by) % bh)) :
                         (by % bh));

                    break;
                }

                case 3:
                {
                    /*
                    ** Tile horizontally and vertically
                    */

                    bw = sw;
                    bh = sh;

                    bx =
                        ((bx < 0) ?
                         (bw - ((-bx) % bw)) :
                         (bx % bw));

                    by =
                        ((by < 0) ?
                         (bh - ((-by) % bh)) :
                         (by % bh));

                    break;
                }
            }

            SetBrushOrgEx (dest_dc, bx, by, (POINT *) NULL);

#if 0
            if (_ui_krn_color_get_depth () >= 32)
            {
                /*
                ** Use a DDB (faster) on a 32-bit display
                */

                bitmap = CreateCompatibleBitmap (src_dc, bw, bh);
            }
            else
#endif
            {
                /*
                ** Use a DIB (slower) on a non-32-bit display
                */

                ZERO_OUT_STRUCT (color_info);

                color_info.bmiHeader.bV5Size = sizeof (color_info);
                color_info.bmiHeader.bV5Width = bw;
                color_info.bmiHeader.bV5Height = bh;
                color_info.bmiHeader.bV5Planes = 1;
                color_info.bmiHeader.bV5BitCount = 32;
                color_info.bmiHeader.bV5Compression = BI_BITFIELDS;
                color_info.bmiHeader.bV5SizeImage = (((((bw << 5) + 31) & ~31) >> 3) * bh);
                color_info.bmiHeader.bV5RedMask = 0x00ff0000U;
                color_info.bmiHeader.bV5GreenMask = 0x0000ff00U;
                color_info.bmiHeader.bV5BlueMask = 0x000000ffU;
                color_info.bmiHeader.bV5AlphaMask = 0xff000000U;
                color_info.bmiHeader.bV5CSType = LCS_DEVICE_CMYK;
                color_info.bmiHeader.bV5Intent = LCS_GM_IMAGES;

                color_info.bmiColors [0].rgbRed = 0xff;
                color_info.bmiColors [1].rgbGreen = 0xff;
                color_info.bmiColors [2].rgbBlue = 0xff;

                bitmap =
                    CreateDIBSection (
                        src_dc,
                        (BITMAPINFO *) &color_info,
                        DIB_RGB_COLORS,
                        (VOID **) &color_data,
                        (HANDLE) NULL,
                        0);
            }

            dc = CreateCompatibleDC (src_dc);
            sys_bitmap = SelectBitmap (dc, bitmap);

            StretchBlt (dc, 0, 0, bw, bh, src_dc, sx, sy, sw, sh, SRCCOPY);

            SelectBitmap (dc, sys_bitmap);
            DeleteDC (dc);

            brush = CreatePatternBrush (bitmap);

            sys_brush = SelectBrush (dest_dc, brush);
            PatBlt (dest_dc, dx, dy, dw, dh, PATCOPY);
            SelectBrush (dest_dc, sys_brush);

            DeleteBrush (brush);

            DeleteBitmap (bitmap);
        }
    }
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_image_read
| Purpose:  Read the HBITMAP into the given image structure
| Input:    handle      - the HBITMAP handle
|           opaque      - TRUE if the image is opaque, FALSE if it is not,
|                         or < 0 if the opacity is unknown
| Output:   image       - the image structure
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_read (void *handle, int opaque, ui_image_t *image)
{
#ifdef UI_SYSTEM_NT

    HBITMAP             bitmap = (HBITMAP) handle, mask_bitmap;
    HICON               icon = (HICON) handle;
    ICONINFO            icon_info;
    HWND                window = GetDesktopWindow ();
    HDC                 device_context;
    BITMAPV4INFO       *color_info;
    unsigned char      *color_data;
    unsigned char      *data, *line, r, g, b, a;
    unsigned int        rm, gm, bm, am, rs, gs, bs, as, m;
    unsigned int        color;
    int                 x, y, color_bpr, f, depth = image->depth;
    BITMAPINFOHEADER   *mask_info;
    unsigned char      *mask_data, *mask_line;
    int                 mask_offset;
    int                 status = UI_ERROR;

    if (bitmap == INVALID_HANDLE_VALUE)
    {
        /*
        ** The handle is invalid
        */

        bitmap = mask_bitmap = (HBITMAP) NULL;
        icon = (HICON) NULL;
    }
    else if (GetIconInfo (icon, &icon_info))
    {
        /*
        ** The handle is an icon
        */

        bitmap = icon_info.hbmColor;
        mask_bitmap = icon_info.hbmMask;
    }
    else if (GetObject (bitmap, sizeof (BITMAP), NULL) > 0)
    {
        /*
        ** The handle is a bitmap, so there is no separate mask
        */

        mask_bitmap = (HBITMAP) NULL;
        icon = (HICON) NULL;
    }
    else
    {
        /*
        ** The handle is not recognized
        */

        bitmap = mask_bitmap = (HBITMAP) NULL;
        icon = (HICON) NULL;
    }

    if (bitmap == (HBITMAP) NULL)
    {
        image->flags = 0;
        image->width = image->height = image->depth = 0;
        image->hotspot.x = image->hotspot.y = -1;
        image->data = (unsigned char *) NULL;

        status = UI_ERROR;
    }
    else
    {
        /*
        ** Get the pixels of the bitmap as a 32-bit TrueColor bitmap
        */

        device_context = GetDC (window);

        color_info = GETSTRUCT (BITMAPV4INFO);

        color_info->bmiHeader.bV4Size = sizeof (color_info->bmiHeader);
        color_info->bmiHeader.bV4BitCount = 0;

        GetDIBits (
            device_context,
            bitmap,
            0,
            0,
            NULL,
            (BITMAPINFO *) color_info,
            DIB_RGB_COLORS);

        color_info->bmiHeader.bV4BitCount = 32;
        color_info->bmiHeader.bV4V4Compression = BI_BITFIELDS;
        color_info->bmiHeader.bV4SizeImage = 0;

        GetDIBits (
            device_context,
            bitmap,
            0,
            color_info->bmiHeader.bV4Height,
            NULL,
            (BITMAPINFO *) color_info,
            DIB_RGB_COLORS);

        rm = color_info->bmiHeader.bV4RedMask;
        gm = color_info->bmiHeader.bV4GreenMask;
        bm = color_info->bmiHeader.bV4BlueMask;

        if ((am = color_info->bmiHeader.bV4AlphaMask) == 0)
        {
            am = ((rm | gm | bm) ^ 0xffffffffU);
        }

        for (m = rm, rs = 0; m > 0 && !(m & 1); m >>= 1, rs++);
        for (m = gm, gs = 0; m > 0 && !(m & 1); m >>= 1, gs++);
        for (m = bm, bs = 0; m > 0 && !(m & 1); m >>= 1, bs++);
        for (m = am, as = 0; m > 0 && !(m & 1); m >>= 1, as++);

        color_data =
            GET_ARRAY (
                unsigned char,
                color_info->bmiHeader.bV4SizeImage);

        color_bpr =
            (color_info->bmiHeader.bV4SizeImage /
             MAX (color_info->bmiHeader.bV4Height, 1));

        GetDIBits (
            device_context,
            bitmap,
            0,
            color_info->bmiHeader.bV4Height,
            (VOID *) color_data,
            (BITMAPINFO *) color_info,
            DIB_RGB_COLORS);

        ReleaseDC (window, device_context);

        /*
        ** Copy the bitmap information into the image structure
        */

        image->flags =
            (UI_IMAGE_WIDTH |
             UI_IMAGE_HEIGHT |
             UI_IMAGE_DEPTH |
             UI_IMAGE_DATA);

        image->width = color_info->bmiHeader.bV4Width;
        image->height = color_info->bmiHeader.bV4Height;
        image->hotspot.x = image->hotspot.y = -1;

        /*
        ** Free the bitmap information
        */

        relmem (&color_info);

        /*
        ** Determine whether the bitmap contains any transparency information
        */

        if (!opaque)
        {
            image->depth = 32;
        }
        else
        {
            image->depth = 24;

            for (f = 0, y = (image->height - 1); f == 0 && y >= 0; y--)
            {
                for (line = (color_data + (y * color_bpr)), x = 0;
                     x < image->width;
                     line += 4, x++)
                {
                    color =
                        (line [0] |
                         (line [1] << 8) |
                         (line [2] << 16) |
                         (line [3] << 24));

                    if ((color & am) == am)
                    {
                        f = UI_COLOR_TRANSPARENT_RGB_FLAG;
                        break;
                    }
                }
            }

            /*
            ** The GDI often applies an indeterminate alpha value to pixels,
            ** especially when rendered as text with ClearType enabled.  This
            ** causes the following code to mark such pixels as
            ** semi-transparent, which is undesirable.
            **
            ** jas - 30-Nov-07
            **
            ** Only attempt to retain full 32-bit pixel information when asked
            ** to do so by passing in an image depth that is greater than 24.
            **
            ** jas - 21-Nov-12
            */

            if (depth > 24 &&
                f == UI_COLOR_TRANSPARENT_RGB_FLAG)
            {
                for (a = 0, y = (image->height - 1); a == 0 && y >= 0; y--)
                {
                    for (line = (color_data + (y * color_bpr)), x = 0;
                         x < image->width;
                         line += 4, x++)
                    {
                        color =
                            (line [0] |
                             (line [1] << 8) |
                             (line [2] << 16) |
                             (line [3] << 24));

                        if ((color & am) != 0 &&
                            (color & am) != am)
                        {
                            a = -1;
                            break;
                        }
                    }
                }

                if (a != 0)
                {
                    image->depth = 32;
                }
            }
        }

        /*
        ** Copy the pixels from the bitmap into the image
        */

        image->data =
            GET_ARRAY (unsigned char, (4 * image->width * image->height));

        if (image->depth == 32)
        {
            for (data = image->data, y = (image->height - 1); y >= 0; y--)
            {
                for (line = (color_data + (y * color_bpr)), x = 0;
                     x < image->width;
                     line += 4, x++)
                {
                    color =
                        (line [0] |
                         (line [1] << 8) |
                         (line [2] << 16) |
                         (line [3] << 24));

                    r = ((color & rm) >> rs);
                    g = ((color & gm) >> gs);
                    b = ((color & bm) >> bs);

                    if ((a = ((color & am) >> as)) == 0 ||
                        a == 255)
                    {
                        *data++ = r;
                        *data++ = g;
                        *data++ = b;
                    }
                    else
                    {
                        *data++ = UI_IMAGE_UNPREMULTIPLY (r, a);
                        *data++ = UI_IMAGE_UNPREMULTIPLY (g, a);
                        *data++ = UI_IMAGE_UNPREMULTIPLY (b, a);
                    }

                    *data++ = a;
                }
            }
        }
        else
        {
            for (data = image->data, y = (image->height - 1); y >= 0; y--)
            {
                for (line = (color_data + (y * color_bpr)), x = 0;
                     x < image->width;
                     line += 4, x++)
                {
                    color =
                        (line [0] |
                         (line [1] << 8) |
                         (line [2] << 16) |
                         (line [3] << 24));

                    r = ((color & rm) >> rs);
                    g = ((color & gm) >> gs);
                    b = ((color & bm) >> bs);
                    a = ((color & am) >> as);

                    *data++ = r;
                    *data++ = g;
                    *data++ = b;

                    *data++ =
                        UI_COLOR_RGB_FLAGS (
                            UI_COLOR_RGB_FLAG |
                            ((a == 0) ? f : 0));
                }
            }
        }

        /*
        ** Free the bitmap pixels
        */

        relmem (&color_data);

        /*
        ** Process the mask bitmap
        */

        if (image->depth == 32 &&
            mask_bitmap != (HBITMAP) NULL)
        {
            /*
            ** Get the pixels of the mask as a monochrome bitmap
            */

            device_context = GetDC (window);

            mask_info =
                (BITMAPINFOHEADER *)
                    getmem (
                        sizeof (BITMAPINFOHEADER) +
                        (2 * sizeof (RGBQUAD)));

            mask_info->biSize = sizeof (BITMAPINFOHEADER);
            mask_info->biBitCount = 0;

            GetDIBits (
                device_context,
                mask_bitmap,
                0,
                0,
                NULL,
                (BITMAPINFO *) mask_info,
                DIB_PAL_COLORS);

            mask_info->biBitCount = 1;
            mask_info->biCompression = BI_RGB;
            mask_info->biSizeImage = 0;

            GetDIBits (
                device_context,
                mask_bitmap,
                0,
                mask_info->biHeight,
                NULL,
                (BITMAPINFO *) mask_info,
                DIB_PAL_COLORS);

            mask_data = GET_ARRAY (unsigned char, mask_info->biSizeImage);

            mask_offset =
                (mask_info->biSizeImage / MAX (mask_info->biHeight, 1));

            GetDIBits (
                device_context,
                mask_bitmap,
                0,
                mask_info->biHeight,
                (VOID *) mask_data,
                (BITMAPINFO *) mask_info,
                DIB_PAL_COLORS);

            relmem (&mask_info);

            ReleaseDC (window, device_context);

            /*
            ** Apply the mask onto the image
            */

            for (data = (image->data + 3), y = (image->height - 1);
                 y >= 0;
                 y--)
            {
                for (mask_line = (mask_data + (y * mask_offset)),
                     f = (1 << 7),
                     x = 0;
                     x < image->width;
                     data += 4,
                     x++)
                {
                    if (*mask_line & f)
                    {
                        *data = 0;
                    }

                    f >>= 1;

                    if (f == 0)
                    {
                        f = (1 << 7);
                        mask_line++;
                    }
                }
            }

            /*
            ** Free the mask bitmap pixels
            */

            relmem (&mask_data);
        }

        status = UI_SUCCESS;
    }

    if (icon != (HICON) NULL)
    {
        /*
        ** Free the icon bitmap information created by GetIconInfo
        */

        if (icon_info.hbmMask != (HBITMAP) NULL)
        {
            DeleteBitmap (icon_info.hbmMask);
        }

        if (icon_info.hbmColor != (HBITMAP) NULL)
        {
            DeleteBitmap (icon_info.hbmColor);
        }
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_get_menu_check_size
| Purpose:  Get the Menu CheckButton image size
| Input:
| Output:   width       - the width of the image
|           height      - the height of the image
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_get_menu_check_size (int *width, int *height)
{
#ifdef UI_SYSTEM_NT

    if (width != (int *) NULL)
    {
        *width =
            _ui_dpi_descale (
                UI_DPI_SYSTEM,
                _ui_dpi_scale (
                    UI_DPI_DEFAULT,
                    GetSystemMetrics (SM_CXMENUCHECK)));
    }

    if (height != (int *) NULL)
    {
        *height =
            _ui_dpi_descale (
                UI_DPI_SYSTEM,
                _ui_dpi_scale (
                    UI_DPI_DEFAULT,
                    GetSystemMetrics (SM_CYMENUCHECK)));
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_draw_icon
| Purpose:  Draw the given image as an OS-supplied icon, either at the given
|           position using the given context, or into one black and one white
|           bitmap
| Input:    image_data  - the image data structure
| Output:   image_data  - the image data structure which includes the size
|           bitmap_b    - the black bitmap
|           bitmap_w    - the white bitmap
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_image_draw_icon (ui_image_data_t *image_data,
                                     void **bitmap_b, void **bitmap_w)
{
#ifdef UI_SYSTEM_NT

    HICON       icon = (HICON) NULL;
    TCHAR      *id = (TCHAR *) NULL;
    int         is_cursor = FALSE, large = FALSE;
    ICONINFO    icon_info = { 0 };
    ui_gfx_t    context, mem_context;
    int         status = UI_ERROR;

    if ((icon = _uint_image_get_icon (image_data, &large)) != (HICON) NULL ||
        (id = _uint_image_get_icon_handle (image_data, &is_cursor, &large))
        != (TCHAR *) NULL)
    {
        if (is_cursor)
        {
            image_data->size.width = GetSystemMetrics (SM_CXCURSOR);
            image_data->size.height = GetSystemMetrics (SM_CYCURSOR);

            icon = (HICON) LoadCursor ((HINSTANCE) NULL, id);

            if (GetIconInfo (icon, &icon_info))
            {
                if (icon_info.hbmMask != (HBITMAP) NULL)
                {
                    DeleteBitmap (icon_info.hbmMask);
                }

                if (icon_info.hbmColor != (HBITMAP) NULL)
                {
                    DeleteBitmap (icon_info.hbmColor);
                }

                image_data->hotspot.x = (int) icon_info.xHotspot;
                image_data->hotspot.y = (int) icon_info.yHotspot;
            }
            else
            {
                image_data->hotspot.x = image_data->hotspot.y = -1;
            }
        }
        else
        {
            image_data->size.width =
                GetSystemMetrics (large ? SM_CXICON : SM_CXSMICON);

            image_data->size.height =
                GetSystemMetrics (large ? SM_CYICON : SM_CYSMICON);

            if (icon == (HICON) NULL &&
                id != (TCHAR *) NULL)
            {
                icon = LoadIcon ((HINSTANCE) NULL, id);
            }

            image_data->hotspot.x = image_data->hotspot.y = -1;
        }

        if (image_data->context != (ui_gfx_t *) NULL)
        {
            DrawIconEx (
                (HDC) image_data->context->context,
                image_data->position.x,
                image_data->position.y,
                icon,
                image_data->size.width,
                image_data->size.height,
                0,
                (HBRUSH) NULL,
                (DI_NORMAL | DI_COMPAT));
        }
        else
        {
            context.window = (void *) GetDesktopWindow ();
            context.context = (void *) GetDC ((HWND) context.window);
            _ui_gfx_use_context (context.window, context.context, &context);

            *bitmap_b =
                _ui_gfx_create_bitmap (
                    &context,
                    image_data->size.width,
                    image_data->size.height);

            _ui_gfx_create_context (
                &context,
                *bitmap_b,
                &mem_context);

            _ui_gfx_set_fg_color_value (&mem_context, COLOR_BLACK);

            _ui_gfx_fill_rect (
                &mem_context,
                NULL,
                0,
                0,
                image_data->size.width,
                image_data->size.height);

            DrawIconEx (
                (HDC) mem_context.context,
                0,
                0,
                icon,
                image_data->size.width,
                image_data->size.height,
                0,
                (HBRUSH) NULL,
                (DI_NORMAL | DI_COMPAT));

            _ui_gfx_destroy_context (&mem_context);

            *bitmap_w =
                _ui_gfx_create_bitmap (
                    &context,
                    image_data->size.width,
                    image_data->size.height);

            _ui_gfx_create_context (
                &context,
                *bitmap_w,
                &mem_context);

            _ui_gfx_set_fg_color_value (&mem_context, COLOR_WHITE);

            _ui_gfx_fill_rect (
                &mem_context,
                NULL,
                0,
                0,
                image_data->size.width,
                image_data->size.height);

            DrawIconEx (
                (HDC) mem_context.context,
                0,
                0,
                icon,
                image_data->size.width,
                image_data->size.height,
                0,
                (HBRUSH) NULL,
                (DI_NORMAL | DI_COMPAT));

            _ui_gfx_destroy_context (&mem_context);

            _ui_gfx_release_context (&context);
        }

        status = UI_SUCCESS;
    }

    return (status);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_image_get_icon_handle
| Purpose:  Get the icon handle (the ID... identifier) of the given image
| Input:    image_data  - the image data structure
| Output:   cursor      - TRUE if the handle represents a cursor
|           large       - TRUE if the handle is a large icon
| Return:   The icon handle of the given image, or NULL if none found
\*--------------------------------------------------------------------------*/
static TCHAR *_uint_image_get_icon_handle (ui_image_data_t *image_data,
                                           int *cursor, int *large)
{
    typedef struct
    {
        char       *name;
        DBHandle    handle;
        TCHAR      *id;
        int         large;

    } nt_icon_t;

    static nt_icon_t    system_icons [] =
    {
        { UI_ERROR_IMAGE, DB_HANDLE_ERROR, IDI_ERROR, TRUE },
        { UI_SMALL_ERROR_IMAGE, DB_HANDLE_ERROR, IDI_ERROR, FALSE },
        { UI_WARNING_IMAGE, DB_HANDLE_ERROR, IDI_WARNING, TRUE },
        { UI_SMALL_WARNING_IMAGE, DB_HANDLE_ERROR, IDI_WARNING, FALSE },
        { UI_INFO_IMAGE, DB_HANDLE_ERROR, IDI_INFORMATION, TRUE },
        { UI_SMALL_INFO_IMAGE, DB_HANDLE_ERROR, IDI_INFORMATION, FALSE },
        { UI_QUESTION_IMAGE, DB_HANDLE_ERROR, IDI_QUESTION, TRUE },
        { UI_SMALL_QUESTION_IMAGE, DB_HANDLE_ERROR, IDI_QUESTION, FALSE }
    };

    static nt_icon_t    system_cursors [] =
    {
        { UI_ARROW_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_ARROW },
        { UI_APP_STARTING_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_APPSTARTING },
        { UI_BUSY_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_WAIT },
        { UI_IBEAM_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_IBEAM },
        { UI_ILLEGAL_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_NO },
        { UI_HELP_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_HELP },
        { UI_HORZ_SIZE_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_SIZEWE },
        { UI_VERT_SIZE_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_SIZENS },
        { UI_ALL_SIZE_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_SIZEALL },
        { UI_NWSE_SIZE_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_SIZENWSE },
        { UI_NESW_SIZE_CURSOR_IMAGE, DB_HANDLE_ERROR, IDC_SIZENESW }
    };

    int             i;
    TCHAR          *id = (TCHAR *) NULL;

    if (system_icons [0].handle == DB_HANDLE_ERROR)
    {
        for (i = (NUM_ELEM_IN_ARR (system_icons) - 1); i >= 0; i--)
        {
            system_icons [i].handle =
                _ui_krn_image_handle_from_name (system_icons [i].name);
        }

        for (i = (NUM_ELEM_IN_ARR (system_cursors) - 1); i >= 0; i--)
        {
            system_cursors [i].handle =
                _ui_krn_image_handle_from_name (system_cursors [i].name);
        }
    }

    if (image_data->handle == DB_HANDLE_ERROR)
    {
        image_data->handle =
            _ui_krn_image_handle_from_name (image_data->name);
    }

    for (i = (NUM_ELEM_IN_ARR (system_icons) - 1); i >= 0; i--)
    {
        if (image_data->handle == system_icons [i].handle)
        {
            INIT_ARG (cursor, FALSE);
            INIT_ARG (large, system_icons [i].large);
            id = system_icons [i].id;
            break;
        }
    }

    if (id == (TCHAR *) NULL)
    {
        for (i = (NUM_ELEM_IN_ARR (system_cursors) - 1); i >= 0; i--)
        {
            if (image_data->handle == system_cursors [i].handle)
            {
                INIT_ARG (cursor, TRUE);
                INIT_ARG (large, TRUE);
                id = system_cursors [i].id;
                break;
            }
        }
    }

    return (id);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_get_icon
| Purpose:  Get the icon of the given image
| Input:    image_data  - the image data structure
| Output:   large       - TRUE if the handle is a large icon
| Return:   The icon of the given image, or NULL if none found
\*--------------------------------------------------------------------------*/
static HICON _uint_image_get_icon (ui_image_data_t *image_data, int *large)
{
    typedef struct
    {
        char       *name;
        DBHandle    handle;
        HICON       icon;
        int         id;
        int         large;
        int         open;

    } nt_icon_t;

    static nt_icon_t    csidl_icons [] =
    {
        {
            UI_DIR_OPEN_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            CSIDL_RESOURCES,
            FALSE,
            TRUE
        },
        {
            UI_DIR_CLOSED_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            CSIDL_RESOURCES,
            FALSE,
            FALSE
        },
        {
            UI_DIR_CURRENT_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            CSIDL_RESOURCES,
            FALSE,
            TRUE
        },
        {
            UI_DESKTOP_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            CSIDL_DESKTOP,
            FALSE,
            FALSE
        },
        {
            UI_HOST_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            CSIDL_DRIVES,
            FALSE,
            FALSE
        },
        {
            UI_NETWORK_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            CSIDL_NETWORK,
            FALSE,
            FALSE
        }
    };

    static nt_icon_t    shell_icons [] =
    {
        {
            UI_FILE_SELECT_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            235,
            TRUE,
            FALSE
        },
        {
            UI_FILE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            2,
            FALSE,
            FALSE
        },
        {
            UI_FLOPPY_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            7,
            FALSE,
            FALSE
        },
        {
            UI_FIXED_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            9,
            FALSE,
            FALSE
        },
        {
            UI_CDROM_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            12,
            FALSE,
            FALSE
        },
        {
            UI_REMOTE_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            10,
            FALSE,
            FALSE
        },
        {
            UI_MEMORY_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            13,
            FALSE,
            FALSE
        },
        {
            UI_REMOVABLE_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            8,
            FALSE,
            FALSE
        },
        {
            UI_FLOPPY525_DEVICE_IMAGE,
            DB_HANDLE_ERROR,
            INVALID_HANDLE_VALUE,
            6,
            FALSE,
            FALSE
        }
    };

    int             i;
    LPITEMIDLIST    pidl;
    SHFILEINFO      file_info = { 0 };
    HICON           icon = (HICON) NULL;

    if (csidl_icons [0].handle == DB_HANDLE_ERROR)
    {
        for (i = (NUM_ELEM_IN_ARR (csidl_icons) - 1); i >= 0; i--)
        {
            csidl_icons [i].handle =
                _ui_krn_image_handle_from_name (csidl_icons [i].name);
        }

        for (i = (NUM_ELEM_IN_ARR (shell_icons) - 1); i >= 0; i--)
        {
            shell_icons [i].handle =
                _ui_krn_image_handle_from_name (shell_icons [i].name);
        }
    }

    if (image_data->handle == DB_HANDLE_ERROR)
    {
        image_data->handle =
            _ui_krn_image_handle_from_name (image_data->name);
    }

    for (i = (NUM_ELEM_IN_ARR (csidl_icons) - 1);
         i >= 0 && image_data->handle != csidl_icons [i].handle;
         i--);

    if (i >= 0)
    {
        if ((icon = csidl_icons [i].icon) == INVALID_HANDLE_VALUE)
        {
            if (SHGetFolderLocation (
                    (HWND) NULL, csidl_icons [i].id, (HANDLE) NULL, 0, &pidl)
                    == S_OK)
            {
                SHGetFileInfo (
                    (TCHAR *) pidl,
                    -1,
                    &file_info,
                    sizeof (file_info),
                    (SHGFI_PIDL |
                     SHGFI_ICON |
                     (csidl_icons [i].large ?
                      SHGFI_LARGEICON :
                      SHGFI_SMALLICON) |
                     (csidl_icons [i].open ?
                      SHGFI_OPENICON :
                      0)));

                ILFree (pidl);
            }

            icon = csidl_icons [i].icon = file_info.hIcon;
        }

        INIT_ARG (large, csidl_icons [i].large);
    }
    else
    {
        for (i = (NUM_ELEM_IN_ARR (shell_icons) - 1);
             i >= 0 && image_data->handle != shell_icons [i].handle;
             i--);

        if (i >= 0)
        {
            if ((icon = shell_icons [i].icon) == INVALID_HANDLE_VALUE)
            {
                icon = (HICON) NULL;

                ExtractIconEx (
                    TEXT ("SHELL32.DLL"),
                    -(shell_icons [i].id),
                    (shell_icons [i].large ? &icon : (HICON *) NULL),
                    (shell_icons [i].large ? (HICON *) NULL : &icon),
                    1);

                shell_icons [i].icon = icon;
            }

            INIT_ARG (large, shell_icons [i].large);
        }
    }

    return (icon);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_d2d_create_renderer
| Purpose:  Create the renderer for Direct2D bitmaps
| Input:
| Output:   renderer        - the Direct2D renderer
| Return:   TRUE if a renderer was created
\*--------------------------------------------------------------------------*/
static int _uint_image_d2d_create_renderer (ID2D1DCRenderTarget **renderer)
{
    static D2D1_RENDER_TARGET_PROPERTIES    properties =
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

    ID2D1Factory                           *factory;

    return (_uint_d2d_factory (&factory) &&
            ID2D1Factory_CreateDCRenderTarget (
                factory, &properties, renderer) == S_OK &&
            *renderer != (ID2D1DCRenderTarget *) NULL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_d2d_renderer
| Purpose:  Get the renderer for drawing images using Direct2D
| Input:    context         - the drawing context
|           flags           - image drawing flags
| Output:   renderer        - the Direct2D renderer
| Return:   TRUE if the image should try drawing using Direct2D
\*--------------------------------------------------------------------------*/
static int _uint_image_d2d_renderer (ID2D1RenderTarget **renderer,
                                     const ui_gfx_t *context, int flags)
{
    return (!(flags &
                  (UI_IMAGE_DRAW_GREY |
                   UI_IMAGE_DRAW_SELECTED |
                   UI_IMAGE_DRAW_ONCE)) &&
            _ui_gfx_is_accelerated (context) &&
            _uint_d2d_renderer_create (
                (HDC) context->context, (RECT *) NULL, FALSE, renderer));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_d2d_create
| Purpose:  Create a Direct2D bitmap
| Input:    width               - the image width
|           height              - the image height
|           depth               - the image depth (guaranteed less than 32)
|           data                - the pixel data
|           data_arrays         - array of arrays of points to draw
|           count_arrays        - array of number of points to draw
|           array_count         - number of above arrays
|           colors              - the mappings of color indices to values
|           tint                - TRUE to apply tinting to the image
|           is_premultiplied    - TRUE if the pixel data is premultiplied
|           want_premultiplied  - TRUE if the DIB data must be premultiplied
| Output:   bitmap              - the created Direct2D bitmap
| Return:   TRUE if a bitmap was created
\*--------------------------------------------------------------------------*/
static int _uint_image_d2d_create (ID2D1Bitmap **bitmap, int width,
                                   int height, int depth, unsigned char *data,
                                   ui_point_t **data_arrays,
                                   int *count_arrays, int array_count,
                                   int *colors, int tint,
                                   int is_premultiplied,
                                   int want_premultiplied)
{
    static ID2D1DCRenderTarget     *renderer = (ID2D1DCRenderTarget *) NULL;
    static D2D1_BITMAP_PROPERTIES   properties =
    {
        {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED
        },
        0.0f,
        0.0f
    };

    BITMAPV5INFO                    color_info;
    int                             color_bpr =
        ((((width << 5) + 31) & ~31) >> 3);
    unsigned char                  *color_data;
    D2D1_SIZE_U                     size;
    int                             status = FALSE;

    ZERO_OUT_STRUCT (color_info);

    color_info.bmiHeader.bV5RedMask = 0x00ff0000U;
    color_info.bmiHeader.bV5GreenMask = 0x0000ff00U;
    color_info.bmiHeader.bV5BlueMask = 0x000000ffU;
    color_info.bmiHeader.bV5AlphaMask = 0xff000000U;

    color_data = GET_ARRAY (unsigned char, (color_bpr * height));

    _uint_image_create_DIB_data (
        color_data,
        color_bpr,
        &color_info,
        width,
        height,
        depth,
        data,
        data_arrays,
        count_arrays,
        array_count,
        colors,
        tint,
        is_premultiplied,
        want_premultiplied,
        FALSE);

    size.width = (UINT) width;
    size.height = (UINT) height;

#ifndef UINT_DEBUG

    /*
    ** ID2D1DCRenderTarget_CreateBitmap is a macro defined as
    **
    **     (This)->lpVtbl->Base.CreateBitmap
    **
    ** which conflicts with the debug overload of CreateBitmap()
    **
    ** The simplest solution is to skip this code when debugging GDI
    **
    ** jas - 06-Feb-26
    */

    if ((renderer != (ID2D1DCRenderTarget *) NULL ||
         _uint_image_d2d_create_renderer (&renderer)) &&
        ID2D1DCRenderTarget_CreateBitmap (
            renderer, size, color_data, (UINT32) color_bpr, &properties,
            bitmap) == S_OK &&
        *bitmap != (ID2D1Bitmap *) NULL)
    {
        status = TRUE;
    }

#endif /* UINT_DEBUG */

    relmem (&color_data);

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_d2d_destroy
| Purpose:  Destroy a Direct2D bitmap
| Input:    bitmap          - the Direct2D bitmap
| Output:
| Return:   TRUE if the bitmap was destroyed
\*--------------------------------------------------------------------------*/
static int _uint_image_d2d_destroy (ID2D1Bitmap *bitmap)
{
    if (bitmap != (ID2D1Bitmap *) NULL)
    {
        ID2D1Bitmap_Release (bitmap);
    }

    return (TRUE);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_image_d2d_draw
| Purpose:  Draw a Direct2D bitmap
| Input:    renderer        - the Direct2D render target
|           bitmap          - the Direct2D bitmap
|           x               - the image left position
|           y               - the image top position
|           width           - the image width
|           height          - the image height
| Output:
| Return:   TRUE if the bitmap was drawn
\*--------------------------------------------------------------------------*/
static int _uint_image_d2d_draw (ID2D1RenderTarget *renderer,
                                 ID2D1Bitmap *bitmap, int x, int y, int width,
                                 int height)
{
    static D2D1_BITMAP_PROPERTIES   properties =
    {
        {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED
        },
        0.0f,
        0.0f
    };

    ID2D1Bitmap                    *shared_bitmap = (ID2D1Bitmap *) NULL;
    D2D1_RECT_F                     rect;
    int                             status = FALSE;

    if (ID2D1RenderTarget_CreateSharedBitmap (
            renderer, &IID_ID2D1Bitmap, bitmap, &properties,
            &shared_bitmap) == S_OK &&
        shared_bitmap != (ID2D1Bitmap *) NULL)
    {
        rect.left = (FLOAT) x;
        rect.top = (FLOAT) y;
        rect.right = (rect.left + (FLOAT) width);
        rect.bottom = (rect.top + (FLOAT) height);

        ID2D1RenderTarget_DrawBitmap (
            renderer,
            shared_bitmap,
            &rect,
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            (D2D1_RECT_F *) NULL);

        ID2D1Bitmap_Release (shared_bitmap);

        status = TRUE;
    }

    return (status);
}

#endif /* UI_SYSTEM_NT */
