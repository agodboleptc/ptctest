/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    uint_util.c
|
|  Purpose: Windows NT level utility functions
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  23-Jun-95         pch         Created
|  23-Jun-95         pch         Change to beep prototype
|  19-Jul-95         gcn         Font & color util fns
|  24-Jul-95         AW          Corrected sccs id name
|  24-Jul-95 G-01-01 UK    $$1   Submit to new cut.
|  04-Aug-95         gcn         Palette may be NULL in set colors
|  08-Aug-95 G-01-02 UK    $$2   Submission
|  27-Sep-95         AW          Changed to new diagnosic system
|  02-Oct-95 G-01-08 UK    $$3   Automatic Submission
|  16-Oct-95         GCN         Added fn to get comp id from window handle
|  17-Oct-95 G-01-10 UK    $$4   Automatic Submission
|  20-Nov-95         rgk         Added refresh function
|  16-Nov-95 G-01-14 UK    $$5   Automatic Submission
|  23-Nov-95         jas         Implemented refresh function
|  05-Dec-95 G-01-15 UK    $$6   Automatic Submission
|  07-Nov-95         jas         Convert UNICODE strings to strings before
|                                calling _ui_krn_is_component_class (), and
|                                use lstrcmp in _uint_std_refresh (), now that
|                                we have Japanese language support
|  07-Dec-95 G-01-16 UK    $$7   Automatic Submission
|  20-Dec-95 G-01-17+jas   $$8   Fixed bug in _uint_set_colors to cope with
|                                palettes of INVALID_HANDLE_VALUE instead of
|                                just (HPALETTE) NULL
|  09-Jan-96         jas         Added _uint_get_dbl_click_time
|  10-Jan-96         jas         Added UI superclass and removed refresh func
|  15-Jan-96         jas         Added _uint_draw_nc_shadow_border and
|                                _uint_draw_shadow_border to draw 3D borders
|  23-Jan-96 G-03-01 UK    $$9   Automatic Submission
|  07-Feb-96         AW          Added _uint_get_string_extent & _uint_get_fonts
|  13-Feb-96         jas         Use new font and string code
|  14-Feb-96         jas         Removed _uint_get_string_extent &
|                                _uint_get_fonts
|  21-Feb-96 G-03-03 UK    $$10  Automatic Submission
|  25-Mar-96         gcn         Added _uint_root_to_comp
|  26-Mar-96 G-03-07 UK    $$11  Automatic Submission
|  11-Apr-96         jas         Moved key translations from uint_syminput.c
|  16-Apr-96 G-03-10 UK    $$12  Automatic Submission
|  10-Jul-96         pch         Add _uint_get_screen_size
|  16-Jul-96 H-01-02 UK    $$13  Automatic Submission
|  21-Aug-96         jas         Fixed GDI resources bugs
|  04-Sep-96         gcn         Added _uint_is_ui_window from uint_dialog.c
|  04-Sep-96 H-01-07 UK    $$14  Automatic Submission
|  17-Sep-96 H-01-09 jlc   $$15  made _uint_is_ui_window() also claim the
|                                timer window
|  23-Sep-96         jas         Integrated into UK tree
|  24-Sep-96 H-01-10 UK    $$16  Automatic Submission
|  02-Dec-96         jas         Added function keys to
|                                _uint_translate_key_data
|  05-Dec-96 H-01-19 UK    $$17  Automatic Submission
|  14-Feb-97         jas         Added _ui_get_graphics_mode
|  26-Feb-97 H-03-02 UK    $$18  Automatic Submission
|  26-Feb-97         rgk         added _uint_get_screen_pos
|  06-Mar-97         jas         Only check window class names up until the
|                                first whitespace
|  11-Mar-97 H-03-03 UK    $$19  Automatic Submission
|  16-Apr-97         jas         Added _uint_get_stipple_bitmap
|  22-Apr-97 H-03-07 UK    $$20  Automatic Submission
|  16-May-97         jas         Added shadow drawing functions
|  23-May-97         jas         Added focus drawing functions
|  28-May-97 H-03-12 UK    $$21  Automatic Submission
|  30-May-97         jas         Added new keyboard handling
|  04-Jun-97 H-03-13 UK    $$22  Automatic Submission
|   1-Jul-97         pch         Added _uint_get_server_type()
|   4-Jul-97         pch         Fix compiler warning
|  08-Jul-97 H-03-16 UK    $$23  Automatic Submission
|  24-Jul-97         gcn         Check if window is a Pro/E window before
|                                checking if it is a UI window.
|  29-Jul-97 H-03-17 UK    $$24  Automatic Submission
|  30-Jul-97         jas         Added _uint_get_menu_show_delay
|  31-Jul-97         jas         Added sysdep_get_menu_show_delay and
|                                sysdep_get_blink_rate
|  12-Aug-97 H-03-18 UK    $$25  Automatic Submission
|  03-Sep-97         jas         Added _uint_send_message
|  09-Sep-97 H-03-21 UK    $$26  Automatic Submission
|  16-Sep-97         jas         Added _uint_window_... functions
|  23-Sep-97 H-03-23 UK    $$27  Automatic Submission
|  17-Oct-97         jas         Added _uint_comp_from_message
|  20-Oct-97         jas         Check for UNICODE windows
|  21-Oct-97 H-03-27 UK    $$28  Automatic Submission
|  06-Nov-97         jas         Added new color database
|  18-Nov-97 H-03-30 UK    $$29  Automatic Submission
|  11-Dec-97         jas         Added _uint_dotted_polyline
|  12-Dec-97         jas         Use _uint_dotted_line under Windows NT
|  16-Dec-97 H-03-33 UK    $$30  Automatic Submission
|  16-Dec-97         jas         Added application window
|  16-Dec-97         jas         Obsoleted old graphics mode
|  22-Dec-97 H-03-34 UK    $$31  Automatic Submission
|  08-Jan-98         gcn         Get correct screen pos for POPUP style
|                                windows
|  13-Jan-98 H-03-36 UK    $$32  Automatic Submission
|  13-Jan-98         jas         Added _uint_set_cursor
|  14-Jan-98         jas         Added _ui_get_graphics_type
|  20-Jan-98 H-03-37 UK    $$33  Automatic Submission
|  17-Mar-98 H-03-41 UK    $$34  Automatic Submission
|  18-Feb-98         jas         Fixed _uint_draw_shadow for virtual-windows
|  19-Feb-98         jas         Added sysdep_full_drag_enabled
|  06-Mar-98         jas         Added support for work-area
|  11-Mar-98         jas         Added sysdep_get_screen_rect
|  12-Mar-98         jas         Changed _uint_set_cursor
|  31-Mar-98 I-01-01 UK    $$35  Automatic Submission
|  15-Apr-98         jas         Added sysdep_get_cursor_pos
|  15-Apr-98         jas         Added sysdep_message_beep
|  20-Apr-98 I-01-04 UK    $$36  Automatic Submission
|  12-May-98         jas         Fixed thin shadow drawing
|  18-May-98 I-01-08 UK    $$37  Automatic Submission
|  28-May-98         AW          Added _uint_flush()
|  01-Jun-98 I-01-10 UK    $$38  Automatic Submission
|   2-Jun-98         pch         Added _uint_get_scrbar_mindim()
|   3-Jun-98         pch         Use GetSystemMetrics() in
|                                _uint_get_scrbar_mindim()
|  15-Jun-98 I-01-11 UK    $$39  Automatic Submission
|  24-Jun-98         jas         Changed _uint_dotted_polyline
|  29-Jun-98 I-01-13 UK    $$40  Automatic Submission
|  30-Jun-98         jas         Added _uint_replay_message
|  30-Jun-98         jas         Added grab and ungrab functions
|  13-Jul-98 I-01-14 UK    $$41  Automatic Submission
|  18-Aug-98         jas         Added _uint_play_sound
|  27-Aug-98 I-01-17 UK    $$42  Automatic Submission
|  03-Sep-98         jas         Use _ui_get_sound_enabled
|  03-Sep-98         jas         Added _uint_notify_win_event
|  03-Sep-98         jas         Added _ui_snap_to_default
|  03-Sep-98         jas         Added _ui_set_cursor_pos
|  03-Sep-98         jas         Added _ui_get_menu_sizes
|  08-Sep-98 I-01-18 UK    $$43  Automatic Submission
|  23-Sep-98         AW          Added _uint_redraw_window()
|  30-Sep-98 I-01-21 UK    $$44  Automatic Submission
|  08-Jan-99         jas         Added support for Windows 98
|  01-Feb-99 I-03-01 UK    $$45  Automatic Submission
|  08-Feb-99 I-03-02 UK    $$46  Automatic Submission
|  07-Apr-99         jas         Changed bad variable names
|  09-Apr-99 I-03-07 UK    $$47  Automatic Submission
|  27-Apr-99         jas         Use _ui_grabs_enabled when capturing input
|  13-May-99         jas         Fixed _uint_redraw_window
|  17-May-99 I-03-09 UK    $$48  Automatic Submission
|  20-May-99         jas         Added dynamic update of registry settings
|  01-Jun-99 I-03-10 UK    $$49  Automatic Submission
|  07-Jul-99         jas         Fixed _uint_get_screen_pos
|  28-Jul-99 I-03-11 UK    $$50  Automatic Submission
|  10-Sep-99         jas         Added _ui_get_drag_bounding_rect
|  21-Sep-99         jas         Use WM_REALMOUSELAST
|  21-Sep-99         jas         Added _uint_get_mousewheel_scroll_lines
|  21-Sep-99         jas         Removed WM_REALMOUSELAST
|  22-Sep-99         jas         Fixed system check for Windows 95
|  28-Sep-99 I-03-16 UK    $$51  Automatic Submission
|  09-Dec-99         jas         Added multiple monitor support
|  15-Dec-99 I-03-24 UK    $$52  Automatic Submission
|  11-Jan-00         jas         Fixed _uint_get_screen_rect
|  12-Jan-00         jas         Added rectangle to _uint_get_screen_rect
|  13-Jan-00 I-03-26+UK    $$53  Automatic Submission
|  22-Mar-00         jas         Added sysdep_get/set_system_input_language
|  29-Mar-00 J-01-05 UK    $$54  Automatic Submission
|  04-May-00         jas         Changed prototype for relmem
|  18-May-00 J-01-08 UK    $$55  Automatic Submission
|  08-Jun-00         jas         Use PostMessage to replay messages
|  14-Jun-00 J-01-10 UK    $$56  Automatic Submission
|  05-Jul-00         jas         Fixed _uint_comp_from_window
|  11-Jul-00 J-01-12 UK    $$57  Automatic Submission
|  01-Aug-00         jas         Added 64-bit support
|  02-Aug-00         jas         Fixed 64-bit compilation problems
|  08-Aug-00 J-01-14 UK    $$58  Automatic Submission
|  10-Aug-00         jas         Fixed _uint_comp_from_window
|  15-Aug-00 J-01-15 UK    $$59  Automatic Submission
|  19-Sep-00         jas         Fixed _uint_comp_from_window
|  03-Oct-00 J-01-19 UK    $$60  Automatic Submission
|  18-Oct-00         jas         Added _uint_get_gui_thread_info
|  13-Nov-00         jas         Fixed _uint_send_message
|  16-Nov-00 J-01-21 UK    $$61  Automatic Submission
|  21-Nov-00         jas         Fixed _uint_is_ui_window
|  30-Nov-00 J-01-22 UK    $$62  Automatic Submission
|  14-Mar-01         jas         Improved _uint_replay_message
|  15-Mar-01 J-01-29 UK    $$63  Automatic Submission
|  25-Jan-01         jas         Added _uint_animate_window
|  26-Jan-01         jas         Added _uint_update_layered_window
|  15-Feb-01         jas         Added Windows XP graphics mode
|  30-Apr-01         jas         Fixed _uint_is_ui_window
|  12-Jun-01 J-03-01 UK    $$64  Automatic Submission
|  01-Aug-01         jas         Fixed compilation warnings
|  07-Aug-01 J-03-05 UK    $$65  Automatic Submission
|  09-Oct-01         jas         Use pro_is_win95_running
|  18-Oct-01 J-03-10 UK    $$66  Automatic Submission
|  17-Dec-01         AW          Added _uint_reparent_window
|  03-Jan-02 J-03-15 UK    $$67  Automatic Submission
|  10-Jan-02         jas         Added _uint_draw_rect
|  10-Jan-02         jas         Added support for PTC graphics mode
|  17-Jan-02 J-03-17 UK    $$68  Automatic Submission
|  29-Jan-02         jas         Added further support for PTC graphics mode
|  31-Jan-02 J-03-18 UK    $$69  Automatic Submission
|  01-Mar-02         jas         Removed obsolete functions
|  05-Mar-02 J-03-20 UK    $$70  Automatic Submission
|  15-Mar-02         jas         Added _uint_set_focus
|  20-Mar-02 J-03-21 UK    $$71  Automatic Submission
|  02-May-02         jas         Removed sysdep_set/get_cursor_pos
|  14-May-02 J-03-25 UK    $$72  Automatic Submission
|  06-Mar-03         jas         Removed _ui_draw_focus
|  06-Mar-03         jas         Removed _ui_draw_rect
|  06-Mar-03         jas         Use GFX for _ui_draw_shadow
|  11-Mar-03 K-01-02 UK    $$73  Automatic Submission
|  26-Jun-03         jas         Obsoleted ui_memory.h
|  10-Jul-03 K-01-10 UK    $$74  Automatic Submission
|  28-Aug-03         jas         Added more European language support
|  10-Sep-03 K-01-14 UK    $$75  Automatic Submission
|  27-Oct-03         jas         Fixed compilation warnings
|  04-Nov-03 K-01-17 UK    $$76  Automatic Submission
|  12-Nov-03         jas         Added UI_STATIC
|  12-Nov-03         jas         Fixed HP compiler warnings
|  18-Nov-03 K-01-18 UK    $$77  Automatic Submission
|  04-Mar-04         jas         Added _uint_alpha_blend
|  16-Mar-04 K-01-25 UK    $$78  Automatic Submission
|  12-May-04         jas         Added support for Slovak
|  25-May-04 K-03-02 UK    $$79  Automatic Submission
|  08-Jun-04         jas         Added _uint_gradient_fill
|  08-Jun-04 K-03-03 UK    $$80  Automatic Submission
|  17-Jun-04         jas         Added _uint_set_DC_brush/pen_color
|  22-Jun-04 K-03-04 UK    $$81  Automatic Submission
|  28-Jun-04         jas         Fixed compilation warnings
|  07-Jul-04 K-03-05 UK    $$82  Automatic Submission
|  05-Nov-04         AW          Improved performance of _uint_comp_from_window
|  10-Nov-04         AW          Fixed decl of _uint_record_window_class
|  18-Nov-04 K-03-14 UK    $$83  Automatic Submission
|  22-Nov-04 K-03-14+jas   $$84  Fixed _uint_comp_from_window
|  22-Dec-04         jas         Include const.h and ctwcfun.h
|  11-Jan-05 K-03-17 UK    $$85  Automatic Submission
|  25-Jan-05         jas         Include ctwcfun_proto.h instead of ctwcfun.h
|  25-Jan-05 K-03-18 UK    $$86  Automatic Submission
|  21-Feb-05         jas         Moved lookup calls to uint_init.c
|  23-Feb-05         jas         Added sysdep_hide_... functions
|  23-Feb-05         jas         Fixed _uint_replay_message
|  25-Feb-05         jas         Removed sysdep_hide_mnemonics/focus
|  25-Feb-05 K-03-20 UK    $$87  Automatic Submission
|  31-Aug-05 K-03-30+AW    $$88  Fixed mem leak in _uint_record_window_class
|  13-Sep-05 K-03-31 UK    $$89  Automatic Submission
|  11-Oct-05         jas         Removed pro_is_win95_running
|  11-Oct-05         jas         Removed _uint_set_cursor
|  11-Oct-05         jas         Removed pre-Windows 2000 code
|  27-Oct-05         jas         Added _uint_register_wndproc
|  31-Jan-06 L-01-01 UK    $$90  Automatic Submission
|  20-Jun-06         jas         Modified sysdep_redraw_window
|  26-Jun-06         jas         Added _uint_get_mousewheel_scroll_chars
|  27-Jun-06 L-01-11 UK    $$91  Automatic Submission
|  11-Dec-06         jas         Resurrected browser plug-in support
|  12-Dec-06         jas         Fixed _uint_is_in_ui_window
|  14-Dec-06 L-01-22 UK    $$92  Automatic Submission
|  09-Jan-07         jas         Added _uint_animate_window
|  12-Jan-07         jas         Removed unnecessary messages
|  12-Jan-07 L-01-24 UK    $$93  Automatic Submission
|  05-Feb-07         AW          Added _uint_get_comp_from_cursor_pos
|  08-Feb-07         jas         Use 1MB stack space for secondary threads
|  09-Feb-07         jas         Added _uint_set/get_wndproc
|  13-Feb-07 L-01-26 UK    $$94  Automatic Submission
|  28-Mar-07         jas         Fixed problems identifying top-level windows
|  17-Apr-07 L-01-30 UK    $$95  Automatic Submission
|  18-Apr-07         jas         Modified _uint_get_comp_from_cursor_pos
|  24-Apr-07 L-01-30+UK    $$96  Automatic Submission
|  16-Oct-07         jas         Added _uint_set/get_window_region
|  16-Oct-07         jas         Optimized _uint_set_window_region
|  16-Oct-07         jas         Added _uint_get_window_region_box
|  23-Oct-07 L-01-40 UK    $$97  Automatic Submission
|  27-Feb-08         jas         Modified sysdep_get_screen_rect
|  11-Mar-08 L-03-04 UK    $$98  Automatic Submission
|  22-May-09         jas         Fixed GDI resource leaks
|  27-May-09 L-03-33 UK    $$99  Automatic Submission
|  02-Dec-09         jas         Removed obsolete functions
|  08-Dec-09 L-05-11 UK    $$100 Automatic Submission
|  28-Apr-10         jas         Added redrawing flags
|  11-May-10 L-05-22 UK    $$101 Automatic Submission
|  27-May-10         jas         Added UI_NO_WIN32_DEMO subordinate UI code
|  09-Jun-10 L-05-24 UK    $$102 Automatic Submission
|  16-Jun-10         jas         Added _uint_redraw_windows
|  22-Jun-10 L-05-25 UK    $$103 Automatic Submission
|  29-Jun-10         jas         Added IsChildWindow
|  29-Jun-10         jas         Improved _uint_is_in_ui_window
|  30-Jun-10         jas         Added window long value convenience macros
|  07-Jul-10 L-05-26 UK    $$104 Automatic Submission
|  07-Sep-10         jas         Call GdiFlush from _uint_flush
|  14-Sep-10 L-05-31 UK    $$105 Automatic Submission
|  21-Sep-10         jas         Do not call GdiFlush from _uint_flush
|  28-Sep-10 L-05-32 UK    $$106 Automatic Submission
|  20-Oct-10         jas         Fixed _uint_is_in_ui_window
|  22-Oct-10         jas         Added sysdep_get_handedness
|  26-Oct-10 L-05-34 UK    $$107 Automatic Submission
|  12-Jan-11         jas         Use GetAncestor instead of GetWindowParent
|  18-Jan-11 L-05-40 UK    $$108 Automatic Submission
|  25-Jan-11         jas         Added _ui_get_language
|  01-Feb-11 L-05-41 UK    $$109 Automatic Submission
|  10-Mar-11         jas         Added IsChildWindowVisible
|  15-Mar-11 L-05-43 UK    $$110 Automatic Submission
|  11-Apr-11         jas         Fixed RedrawWindow calls
|  12-Apr-11 L-05-45 UK    $$111 Automatic Submission
|  18-Apr-11         jas         Added _uint_window_redraw
|  20-Apr-11         jas         Added _uint_server_type
|  27-Apr-11 L-05-46 UK    $$112 Automatic Submission
|  05-May-11         jas         Improved _uint_window_redraw
|  11-May-11 L-05-47 UK    $$113 Automatic Submission
|  23-Jun-11         jas         Removed PTC graphics mode
|  28-Jun-11 P-10-02 UK    $$114 Automatic Submission
|  27-Jul-11         jas         Added sysdep_blending_enabled
|  09-Aug-11 P-10-05 UK    $$115 Automatic Submission
|  22-Sep-11         jas         Added _uint_get_focus/capture
|  23-Sep-11         jas         Added _uint_has_focus/capture
|  04-Oct-11 P-10-09 UK    $$116 Automatic Submission
|  28-Nov-11         jas         Added _uint_set_display
|  29-Nov-11 P-10-13 UK    $$117 Automatic Submission
|  15-Dec-11         jas         Added display detection
|  15-Dec-11         jas         Added UI_display_info_Attr
|  15-Dec-11         jas         Added sysdep_get_server_info
|  10-Jan-12 P-10-15 UK    $$118 Automatic Submission
|  20-Jan-12         jas         Use _ui_krn_class_do_operation
|  24-Jan-12 P-10-16 UK    $$119 Automatic Submission
|  24-Jan-12         jas         Improved _uint_server_info
|  07-Feb-12 P-10-17 UK    $$120 Automatic Submission
|  13-Feb-12         jas         Added _uint_windows_enabled
|  16-Feb-12 P-10-17+UK    $$121 Automatic Submission
|  15-Mar-12         jas         Removed _uint_windows_enabled
|  20-Mar-12 P-20-01 UK    $$122 Automatic Submission
|  30-May-12         jas         Removed sysdep_root_to_comp
|  30-May-12         jas         Removed sysdep_draw_shadow
|  12-Jun-12 P-20-07 UK    $$123 Automatic Submission
|  04-Sep-12         jas         Added sysdep_get_ancestor_pos
|  05-Sep-12 P-20-13 UK    $$124 Automatic Submission
|  14-Nov-12         jas         Changed sysdep_get_ancestor_pos
|  14-Nov-12         jas         Changed sysdep_redraw_window
|  27-Nov-12 P-20-18 UK    $$125 Automatic Submission
|  30-Nov-12         jas         Added sysdep_get_monitor_rects
|  11-Dec-12 P-20-19 UK    $$126 Automatic Submission
|  09-Jan-13         jas         Added _uint_animate_window
|  10-Jan-13 P-20-21 UK    $$127 Automatic Submission
|  13-Mar-13         jas         Modified sysdep_get_monitor_rects
|  13-Mar-13         jas         Removed sysdep_get_screen_rect
|  20-Mar-13 P-20-26 UK    $$128 Automatic Submission
|  21-May-13         jas         Temporarily disabled blending
|  23-May-13 P-20-30 UK    $$129 Automatic Submission
|  11-Jul-13         jas         Added sysdep_display_scale
|  16-Jul-13 P-20-34 UK    $$130 Automatic Submission
|  20-Sep-13         jas         Improved _uint_redraw_windows
|  24-Sep-13 P-20-39 UK    $$131 Automatic Submission
|  27-Nov-13         jas         Added _uint_set_window_pos
|  27-Nov-13         jas         Enabled blending
|  17-Dec-13 P-20-44 UK    $$132 Automatic Submission
|  21-Jan-14         jas         Implemented _uint_flush
|  21-Jan-14 P-20-46 UK    $$133 Automatic Submission
|  18-Mar-14         jas         Added _uint_desktop_scale
|  18-Mar-14 P-20-50 UK    $$134 Automatic Submission
|  18-Mar-14         jas         Added _uint_get_applied_dpi
|  01-Apr-14 P-20-51 UK    $$135 Automatic Submission
|  11-Jun-14         jas         Added atom to _uint_register_wndproc
|  11-Jun-14         jas         Added _uint_is_ui_atom
|  24-Jun-14         jas         Fixed _uint_get_comp_from_cursor_pos
|  26-Jun-14 P-20-55 UK    $$136 Automatic Submission
|  08-Aug-14         jas         Added PORTUGUESE_BR
|  08-Aug-14 P-20-59 UK    $$137 Automatic Submission
|  08-Oct-14         AW          Single step dialog changes
|  21-Oct-14 P-20-62 UK    $$138 Automatic Submission
|  28-Oct-14         jas         Removed temporary code
|  04-Nov-14 P-20-63 UK    $$139 Automatic Submission
|  08-Jan-15         jas         Enabled high-DPI support by default
|  14-Jan-15         jas         Added _ui_component_from_event/window
|  29-Jan-15         jas         Added UINT_VERSION_WINDOWS_... macros
|  30-Jan-15         jas         Fixed _uint_set_window_region
|  03-Feb-15 P-30-01 UK    $$140 Automatic Submission
|  11-Feb-15         jas         Added sysdep_get_system_animation
|  11-Feb-15         jas         Added sysdep_drop_shadow_enabled
|  13-Feb-15         jas         Added sysdep_play_sound
|  17-Feb-15 P-30-02 UK    $$141 Automatic Submission
|  12-Mar-15         jas         Fixed _uint_is_in_ui_window
|  12-Mar-15         jas         Added UI_high_dpi_enabled_Attr app default
|  17-Mar-15 P-30-04 UK    $$142 Automatic Submission
|  11-Jun-15         jas         Added reporting of OS-defined DPI
|  23-Jun-15 P-30-11 UK    $$143 Automatic Submission
|  30-Jun-15         jas         Added desktops to sysdep_get_monitor_rects
|  01-Jul-15         jas         Place the primary monitor first in the list
|  07-Jul-15 P-30-12 UK    $$144 Automatic Submission
|  22-Sep-15         jas         Added IsWindowsXXXOrGreater
|  28-Sep-15         jas         Added UI_COMPONENT_SYSTEM_NT
|  30-Sep-15 P-30-17 UK    $$145 Automatic Submission
|  27-Nov-15         jas         Fixed _uint_is_in_ui_window
|  03-Dec-15         jas         Fixed compilation warnings
|  09-Dec-15 P-30-22 UK    $$146 Automatic Submission
|  01-Mar-16         jas         Fixed _uint_set_capture
|  02-Mar-16 P-30-27 UK    $$147 Automatic Submission
|  16-Mar-16         jas         Fixed _uint_get_system_animation
|  17-Mar-16 P-30-28 UK    $$148 Automatic Submission
|  03-Jun-16         jas         Added more UI_COMPONENT_SYSTEM_NT
|  07-Jun-16 P-30-28 UK    $$149 Automatic Submission
|  07-Jun-16         jas         Added more UI_COMPONENT_SYSTEM_NT
|  13-Jun-16         jas         Added convenience macros
|  21-Jun-16 P-30-34 UK    $$150 Automatic Submission
|  08-Jul-15         jas         Added sysdep_get/set_display_handle
|  08-Jul-15         jas         Changed void sysdep functions to int
|  19-Jul-16 P-30-36 UK    $$151 Automatic Submission
|  27-Sep-16         jas         Added UI_DISPLAY_TOUCH
|  13-Oct-16 P-30-41 UK    $$152 Automatic Submission
|  20-Dec-16         jas         Removed support for legacy graphics modes
|  20-Dec-16         jas         Removed unreferenced functions
|  24-Jan-17         jas         Fixed _uint_is_in_ui_window
|  24-Jan-17         jas         Moved monitor code into a separate module
|  25-Jan-17         jas         Removed UI_NO_WIN32_DEMO code
|  07-Feb-17         jas         Added UI_system_display_scale_Attr
|  14-Mar-17 P-50-01 UK    $$153 Automatic Submission
|  24-Mar-17         jas         Use software OpenGL with remote desktop
|  27-Mar-17 P-50-03 UK    $$154 Automatic Submission
|  30-Mar-17         jas         Added support for display reduction
|  05-Apr-17 P-50-04 UK    $$155 Automatic Submission
|  25-May-17         jas         Check for integrated touch screen devices
|  30-May-17 P-50-11 UK    $$156 Automatic Submission
|  30-Aug-17         jas         Fixed _uint_get_scrbar_mindim
|  06-Sep-17 P-50-26 UK    $$157 Automatic Submission
|  26-Oct-17         jas         Added UI_SYSTEM_ANIMATION_SCROLL
|  31-Oct-17 P-50-34 UK    $$158 Automatic Submission
|  13-Nov-17         jas         Added GetWindowThreadId
|  15-Nov-17 P-50-36 UK    $$159 Automatic Submission
|  01-May-18         jas         Added nt_get_registry_value/string
|  02-May-18 P-60-02 UK    $$160 Automatic Submission
|  30-Oct-18         jas         Removed uint_theme.h
|  12-Nov-18 P-60-24 UK    $$161 Automatic Submission
|  12-Nov-18         jas         Removed _uint_set_instance_handle
|  12-Nov-18         jas         Removed sysdep_set_display_handle
|  19-Nov-18 P-60-25 UK    $$162 Automatic Submission
|  25-Feb-19         jas         Removed sysdep_beep/sysdep_message_beep
|  19-Mar-19 P-70-01 UK    $$163 Automatic Submission
|  04-Apr-19         jas         Added _uint_is_ui_atom_or_wndproc
|  15-Apr-19 P-70-05 UK    $$164 Automatic Submission
|  26-Jun-19         jas         Added SetProcessDpiAwarenessContext
|  02-Jul-19 P-70-16 UK    $$165 Automatic Submission
|  11-Jul-19         jas         Changed drag bounding rect size
|  12-Jul-19         jas         Added UI_DPI_SYSTEM
|  16-Jul-19 P-70-18 UK    $$166 Automatic Submission
|  10-Jan-20         jas         Added UI_DISPLAY_REMOTEFX
|  14-Jan-20 P-70-41 UK    $$167 Automatic Submission
|  16-Jan-20         jas         Fixed RemoteFX detection
|  22-Jan-20 P-70-42 UK    $$168 Automatic Submission
|  27-Jan-20         jas         Added remote-but-not-remote-desktop detection
|  28-Jan-20 P-70-43 UK    $$169 Automatic Submission
|  10-Feb-20         jas         Added _uint_should_use_software_opengl
|  12-Feb-20 P-70-44 UK    $$170 Automatic Submission
|  21-Apr-20         jas         Fixed _uint_display_scale
|  05-May-20 P-80-02 UK    $$171 Automatic Submission
|  05-Jun-20         jas         Fixed sysdep_get_scrbar_mindim
|  10-Jun-20 P-80-07 UK    $$172 Automatic Submission
|  01-Jul-20         jas         Added DPI to menu_sizes and scrbar_mindim
|  07-Jul-20 P-80-11 UK    $$173 Automatic Submission
|  10-Nov-20         jas         Fixed compilation warnings
|  11-Nov-20 P-80-28 UK    $$174 Automatic Submission
|  12-Nov-20         jas         Added UI_DISPLAY_REMOTEAPP
|  12-Nov-20         jas         Added UI_DISPLAY_CLOUD
|  18-Nov-20 P-80-29 UK    $$175 Automatic Submission
|  18-May-21         jas         Always allow scroll animation
|  25-May-21 P-90-10 UK    $$176 Automatic Submission
|  11-Mar-22         jas         Added detection for Amazon Web Services
|  22-Mar-22 Q-10-03 UK    $$177 Automatic Submission
|  09-Mar-23         jas         Added sysdep_dpi_initialize
|  09-Mar-23         jas         Moved DPI code into a separate module
|  15-Mar-23 Q-11-03 UK    $$178 Automatic Submission
|  25-May-23         jas         Improved touch and RemoteApp notifications
|  30-May-23         jas         Allow Windows to control scroll animation
|  31-May-23 Q-11-14 UK    $$179 Automatic Submission
|  27-Jun-23         jas         Added sysdep_get_user_state
|  05-Jul-23 Q-11-19 UK    $$180 Automatic Submission
|  06-Nov-23         jas         Added support for Parallels
|  08-Nov-23 Q-11-37 UK    $$181 Automatic Submission
|  15-Nov-24         jas         Added UI_DISPLAY_CLOUDAVD/AWS
|  20-Nov-24 Q-12-39 UK    $$182 Automatic Submission
|  16-Jun-25         jas         Added _uint_get/set_wndproc_U
|  24-Jun-25 Q-13-14 UK    $$183 Automatic Submission
|  10-Jul-25         jas         Fixed compilation warnings
|  10-Jul-25 Q-13-17 UK    $$184 Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#if !defined (lint) && defined (SHOW_SCCS_ID)
static char uint_util_c_id [] = "@(#) uint_util.c 4117.1@(#)";
#endif

#include <btkcdir.h>
#include <const.h>
#include <ctwcfun_proto.h>
#include <ctstrutil_proto.h>
#include <cu_trail_coordinator.h>
#include <cu_init_proto.h>
#include <cu_getsetconfig.h>
#include <languages.h>
#include <mathcons.h>
#include <nt_registry.h>
#include <pfa.h>
#include <syslimits.h>
#include <sysmath.h>
#include <utility.h>

#include <ui.h>
#include <uip.h>
#include <ui_app_res.h>
#include <ui_classesp.h>
#include <ui_colors.h>
#include <ui_gfx.h>
#include <ui_kernel.h>
#include <ui_mgr.h>
#include <ui_string.h>
#include <ui_trail.h>
#include <ui_utils.h>
#include <ui_utilsp.h>

#ifdef UI_SYSTEM_NT

#include <uint.h>

#include <dwmapi.h>
#include <TlHelp32.h>

/*
** Convenience wrapper for SystemParametersInfo
*/

#ifdef GetSystemParameter
#undef GetSystemParameter
#endif /* GetSystemParameter */

#define GetSystemParameter(p,v) \
(SystemParametersInfo ((p), (UINT) 0, &(v), (UINT) 0) && (v))


/*
** UI window procedure database
*/

static LONG_PTR *wndproc_database = (LONG_PTR *) NULL;
static ATOM *atom_database = (ATOM *) NULL;


/*
** Static functions
*/

static int _uint_is_ui_wndproc (
    LONG_PTR    wndproc
);

static int _uint_is_ui_atom (
    ATOM        atom
);

#define _uint_is_ui_atom_or_wndproc(w) \
(_uint_is_ui_atom (GetClassAtom (w)) || \
 _uint_is_ui_wndproc ((LONG_PTR) _uint_get_wndproc_U (w)))

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_get_scrbar_mindim
| Purpose:  Get the minimum size of a ScrollBar
| Input:    dpi         - the DPI
| Output:
| Return:   The minimum size of a ScrollBar, scaled to the DPI
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_scrbar_mindim (ui_dpi_t dpi)
{
#ifdef UI_SYSTEM_NT

    return (_ui_dpi_descale (
                UI_DPI_SYSTEM,
                _ui_dpi_scale (dpi, GetSystemMetrics (SM_CXVSCROLL))));

#else

    return (0);

#endif /* UI_SYSTEM_NT */

}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_set_use_software_opengl
| Purpose:  Prevent hardware OpenGL from being used
| Input:
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_set_use_software_opengl (void)
{
    /*
    ** Temporarily set the graphics priority to 1 to allow us to force the
    ** graphics code to use software OpenGL.
    **
    ** This function is only ever called during initialization, immediately
    ** after config.pro has been read and immediately before the
    ** graphics code is initialized, so this temporary adjustment to the
    ** graphics priority - though illegal - is not harmful.
    **
    ** jas - 24-Mar-17
    */

    int priority = set_graphics_priority (1);

    set_use_software_opengl (TRUE);

    set_graphics_priority (priority);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_should_use_software_opengl
| Purpose:  Determine whether we should attempt to use software OpenGL
| Input:
| Output:
| Return:   TRUE if we should attempt to use software OpenGL
\*--------------------------------------------------------------------------*/
static int _uint_should_use_software_opengl (void)
{
    int         i;
    char        path [CHAR_PATH_SIZE];
    wchar_t    *user_path;
    Pfa        *fp;
    int         status = FALSE;

    for (i = 0; !status && i < 2; i++)
    {
        if (((i == 0 &&
              btk_getcwd (path, (CHAR_PATH_SIZE - 16)) != (char *) NULL) ||
             (i != 0 &&
              (user_path =
               get_software_opengl_path ()) != (wchar_t *) NULL &&
              wstrtos (path, user_path) != (char *) NULL)) &&
            path [0] != NULL_CHAR)
        {
            btk_sprintf (
                (path + strlen (path)),
                "%c%s",
                PATH_SEPARATOR,
                "OpenGL32.dll");

            if ((fp = pfa_alloc_nondir_file (path)) != (Pfa *) NULL)
            {
                if (pfa_access (fp, ACCESS_READ))
                {
                    status = TRUE;
                }

                pfa_free_pro_file (&fp);
            }
        }
    }

    if (status)
    {
        _ui_info_msg (
            UIMSG_UI,
            "Using software OpenGL graphics from %s",
            path);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_server_info
| Purpose:  Return the server information
| Input:
| Output:
| Return:   The server information
\*--------------------------------------------------------------------------*/
static int _uint_server_info (void)
{
    DISPLAY_DEVICEA     data = { sizeof (DISPLAY_DEVICEA) };
    char                vendor [128];
    const char         *vmware = "vmware", *parallels = "parallels";
    HKEY                parent, key, subkey;
    WCHAR               name [1024];
    DWORD               type = 0, length = NUM_ELEM_IN_ARR (vendor), i;
    char               *touch = "";
    DWORD               glass = 0, current = 0;
    HANDLE              snapshot;
    DWORD               process_id;
    PROCESSENTRY32      process_entry;
    BOOL                process_status;
    int                 info = NT_SERVER;

    if (EnumDisplayDevicesA ((CHAR *) NULL, 0, &data, 0))
    {
        /*
        ** Get the display vendor
        */

        convert_to_lower (data.DeviceString, vendor);

        /*
        ** Determine whether the display is touch
        */

        if (GetSystemMetrics (SM_MAXIMUMTOUCHES) > 1 &&
            (GetSystemMetrics (SM_DIGITIZER) &
             (NID_INTEGRATED_TOUCH | NID_READY))
                == (NID_INTEGRATED_TOUCH | NID_READY))
        {
            info |= UI_DISPLAY_TOUCH;

            touch = "touch ";
        }

        /*
        ** Determine whether the display is virtual
        */

        if (strstr (vendor, vmware))
        {
            info |= UI_DISPLAY_VIRTUAL;
        }
        else if (RegOpenKeyExW (
                     HKEY_LOCAL_MACHINE,
                     L"SYSTEM\\CurrentControlSet\\Control\\Class", (DWORD) 0,
                     KEY_ENUMERATE_SUB_KEYS, &parent) == ERROR_SUCCESS)
        {
            for (i = 0;
                 !(UI_DISPLAY_IS_VIRTUAL (info)) &&
                 RegEnumKeyW (parent, i, name, NUM_ELEM_IN_ARR (name))
                     == ERROR_SUCCESS;
                 i++)
            {
                if (RegOpenKeyExW (
                        parent, name, (DWORD) 0, KEY_QUERY_VALUE, &key)
                        == ERROR_SUCCESS)
                {
                    length = NUM_ELEM_IN_ARR (vendor);

                    if (RegQueryValueExA (
                            key, "Class", (DWORD *) NULL, &type,
                            (LPBYTE) vendor, &length) == ERROR_SUCCESS &&
                        type == REG_SZ &&
                        !u_strcmp (vendor, "display") &&
                        RegOpenKeyExA (
                            key, "0000", (DWORD) 0, KEY_QUERY_VALUE, &subkey)
                            == ERROR_SUCCESS)
                    {
                        length = NUM_ELEM_IN_ARR (vendor);

                        if ((RegQueryValueExA (
                                 subkey, "ProviderName", (DWORD *) NULL,
                                 &type, (LPBYTE) vendor, &length)
                                 == ERROR_SUCCESS ||
                             RegQueryValueExA (
                                 subkey, "DriverDesc", (DWORD *) NULL,
                                 &type, (LPBYTE) vendor, &length)
                                 == ERROR_SUCCESS) &&
                            type == REG_SZ)
                        {
                            convert_to_lower (vendor, vendor);

                            if (strstr (vendor, vmware) ||
                                strstr (vendor, parallels))
                            {
                                info |= UI_DISPLAY_VIRTUAL;
                            }
                        }

                        RegCloseKey (subkey);
                    }

                    RegCloseKey (key);
                }
            }

            RegCloseKey (parent);
        }

        if (UI_DISPLAY_IS_VIRTUAL (info))
        {
            _ui_info_msg (
                UIMSG_UI,
                "Using virtual %sdisplay <%s>",
                touch,
                data.DeviceString);
        }

        /*
        ** Determine whether the display is remote
        */

        if (IsWindows10OrGreater () &&
            nt_get_registry_value (
                HKEY_LOCAL_MACHINE,
                "SYSTEM\\CurrentControlSet\\Control\\Terminal Server",
                "GlassSessionId", &glass) &&
            ProcessIdToSessionId (GetCurrentProcessId (), &current) &&
            current != glass)
        {
            info |= (UI_DISPLAY_REMOTE | UI_DISPLAY_REMOTEFX);

            _ui_info_msg (
                UIMSG_UI,
                "Using RemoteFX vGPU %sdisplay",
                touch);
        }
        else
        {
            convert_to_lower (data.DeviceKey, vendor);

            if (strstr (vendor, "\\rdp"))
            {
                _ui_info_msg (
                    UIMSG_UI,
                    "Using Remote Desktop %sdisplay",
                    touch);

                info |= UI_DISPLAY_REMOTE;

                /*
                ** Use software OpenGL with remote desktop without RemoteFX
                */

                if (_ui_get_graphics_type () == OPENGL &&
                    _uint_should_use_software_opengl ())
                {
                    _uint_set_use_software_opengl ();
                }
            }
            else if (GetSystemMetrics (SM_REMOTESESSION))
            {
                /*
                ** The documentation and sample code provided by Microsoft is
                ** wrong:
                **
                ** https://docs.microsoft.com/en-us/windows/win32/termserv/detecting-the-terminal-services-environment
                **
                ** Specifically, GetSystemMetrics(SM_REMOTESESSION) can
                ** continue to return TRUE even when RemoteFX is being used.
                **
                ** Therefore the SM_REMOTESESSION check must be done *after*
                ** testing for RemoteFX.
                **
                ** jas - 20-Jan-16
                */

                _ui_info_msg (
                    UIMSG_UI,
                    "Using remote %sdisplay",
                    touch);

                info |= UI_DISPLAY_REMOTE;
            }
        }

        if (!(UI_DISPLAY_IS_VIRTUAL (info)) &&
            !(UI_DISPLAY_IS_REMOTE (info)))
        {
            _ui_info_msg (
                UIMSG_UI,
                "Using %s %sdisplay",
                data.DeviceString,
                touch);
        }

        if ((UI_DISPLAY_IS_VIRTUAL (info) ||
             UI_DISPLAY_IS_REMOTE (info)) &&
            nt_get_registry_string (
                HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows Azure",
                "VMType", sizeof (vendor), vendor))
        {
            /*
            ** Azure Virtual Desktop has been detected
            */

            info |= UI_DISPLAY_CLOUDAVD;
        }
        else if (!(UI_DISPLAY_IS_VIRTUAL (info)) &&
                 !(UI_DISPLAY_IS_REMOTE (info)) &&
                 nt_get_registry_string (
                     HKEY_LOCAL_MACHINE,
                     "SYSTEM\\CurrentControlSet\\Control\\SystemInformation",
                     "SystemManufacturer", sizeof (vendor), vendor) &&
                 strstr (vendor, "Amazon") &&
                 _ui_getenv ("AppStream_Session_ID") != (char *) NULL)
        {
            /*
            ** Amazon Web Services has been detected
            */

            info |= UI_DISPLAY_CLOUDAWS;

            /*
            ** Assume we are running in native-application mode at all times.
            **
            ** At the time of writing we have no way to detect the change
            ** between classic mode and native application mode.  To prevent
            ** 32bpp transparency effects from being attempted in the latter,
            ** we cause the app to fall back to 24bpp+1bpp.
            */

            info |= UI_DISPLAY_REMOTEAPP;
        }

        if (UI_DISPLAY_IS_CLOUD (info))
        {
            _ui_info_msg (
                UIMSG_UI,
                "Using %s cloud hosting",
                vendor);

            if (!(UI_DISPLAY_IS_REMOTEAPP (info)))
            {
                /*
                ** Look for a parent process which is rdpinit.exe
                **
                ** https://superuser.com/questions/707823/can-an-application-detect-that-it-is-running-as-a-remoteapp
                **
                ** and use this as a shiboleth for distinguishing RemoteApp
                ** from Remote Desktop
                **
                ** https://talk-about-it.ca/creating-remoteapp-rdp-sessions/
                **
                ** jas - 12-Nov-20
                */

                if ((process_id = GetCurrentProcessId ()) != (DWORD) 0 &&
                    (snapshot =
                     CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0))
                        != (HANDLE) 0 &&
                    snapshot != INVALID_HANDLE_VALUE)
                {
                    while (process_id != (DWORD) 0)
                    {
                        for (process_entry.dwSize = sizeof (process_entry),
                             process_status =
                                 Process32First (snapshot, &process_entry);
                             process_status &&
                             process_entry.th32ProcessID != process_id;
                             process_entry.dwSize = sizeof (process_entry),
                             process_status =
                                 Process32Next (snapshot, &process_entry));

                        if (!process_status)
                        {
                            process_id = (DWORD) 0;
                        }
                        else if (wu_strcmp (
                                     process_entry.szExeFile,
                                     L"rdpinit.exe"))
                        {
                            process_id = process_entry.th32ParentProcessID;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (process_id != (DWORD) 0)
                    {
                        info |= UI_DISPLAY_REMOTEAPP;
                    }

                    CloseHandle (snapshot);
                }
            }
        }
    }

    return (info);
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_get_server_info
| Purpose:  Return the server type
| Input:
| Output:
| Return:   The server type
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_server_info (void)
{
#ifdef UI_SYSTEM_NT

    static int  type = -1;

    if (type == -1)
    {
        type = _uint_server_info ();
    }

    return (type);

#else

    return (-1);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_drag_bounding_rect
| Purpose:  Get the coordinates of the bounding rectangle for drag & drop
|           operations
| Input:
| Output:   rect        - the drag rectangle
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_drag_bounding_rect (ui_rect_t *rect)
{
#ifdef UI_SYSTEM_NT

    rect->width = GetSystemMetrics (SM_CXDRAG);
    rect->height = GetSystemMetrics (SM_CYDRAG);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_play_sound
| Purpose:  Play a sound
| Input:    sound       - the sound to play
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_play_sound (int sound)
{
#ifdef UI_SYSTEM_NT

    switch (sound)
    {
        case UI_SOUND_MENU_POPUP:
        {
            PlaySound (
                TEXT ("MenuPopup"),
                (HMODULE) NULL,
                (SND_ALIAS | SND_NODEFAULT | SND_ASYNC));

            break;
        }

        case UI_SOUND_MENU_COMMAND:
        {
            PlaySound (
                TEXT ("MenuCommand"),
                (HMODULE) NULL,
                (SND_ALIAS | SND_NODEFAULT | SND_ASYNC));

            break;
        }

        case UI_SOUND_MESSAGE_BEEP:
        {
            MessageBeep (MB_OK);
            break;
        }

        case UI_SOUND_MESSAGE_ERROR:
        {
            MessageBeep (MB_ICONHAND);
            break;
        }

        case UI_SOUND_MESSAGE_WARNING:
        {
            MessageBeep (MB_ICONEXCLAMATION);
            break;
        }

        case UI_SOUND_MESSAGE_INFO:
        {
            MessageBeep (MB_ICONASTERISK);
            break;
        }

        case UI_SOUND_MESSAGE_QUESTION:
        {
            MessageBeep (MB_ICONQUESTION);
            break;
        }
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_dbl_click_time
| Purpose:  Return the Windows NT double click time
| Input:
| Output:
| Return:   The double click time
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_dbl_click_time (void)
{
#ifdef UI_SYSTEM_NT

    return (GetDoubleClickTime ());

#else

    return (-1);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_blink_rate
| Purpose:  Return the Windows NT cursor blink rate
| Input:
| Output:
| Return:   The blink rate
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_blink_rate (void)
{
#ifdef UI_SYSTEM_NT

    return (GetCaretBlinkTime ());

#else

    return (-1);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_menu_show_delay
| Purpose:  Return the Windows NT menu show delay
| Input:
| Output:
| Return:   The menu show delay
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_menu_show_delay (void)
{
#ifdef UI_SYSTEM_NT

    char    value [16];

    return (nt_get_registry_string (
                HKEY_CURRENT_USER, "Control Panel\\Desktop", "MenuShowDelay",
                sizeof (value), value) ?
            atoi (value) :
            0);

#else

    return (-1);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_handedness
| Purpose:  Return the Windows NT handedness for menu drop alignment
| Input:
| Output:
| Return:   The handedness for menu drop alignment
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_handedness (void)
{
#ifdef UI_SYSTEM_NT

    BOOL    flag;

    return (GetSystemParameter (SPI_GETMENUDROPALIGNMENT, flag) ?
            UI_RIGHT :
            UI_LEFT);

#else

    return (UI_LEFT);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_blending_enabled
| Purpose:  Return whether the system supports 32-bit blended windows
| Input:
| Output:
| Return:   TRUE if the system supports blended windows, otherwise FALSE
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_blending_enabled (void)
{
#ifdef UI_SYSTEM_NT

    return (IsWindowsVistaOrGreater () ||
            _ui_get_graphics_type () != OPENGL);

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_drop_shadow_enabled
| Purpose:  Return whether the system has drop-shadows enabled
| Input:
| Output:
| Return:   TRUE if the system has drop-shadows enabled, otherwise FALSE
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_drop_shadow_enabled (void)
{
#ifdef UI_SYSTEM_NT

    BOOL    flag;

    return (GetSystemParameter (SPI_GETDROPSHADOW, flag) &&
            !(_ui_server_is_remote_app ()));

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_full_drag_enabled
| Purpose:  Return whether the system has full drag enabled
| Input:
| Output:
| Return:   TRUE if the system has full drag enabled, otherwise FALSE
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_full_drag_enabled (void)
{
#ifdef UI_SYSTEM_NT

    char    value [16];

    return ((nt_get_registry_string (
                 HKEY_CURRENT_USER, "Control Panel\\Desktop", "DragFullWindows",
                 sizeof (value), value) &&
             atoi (value)) ||
            _ui_server_is_remote_app ());

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_snap_to_default
| Purpose:  Determine whether the pointer should snap to the default button
|           in a dialog when the dialog is displayed
| Input:
| Output:
| Return:   TRUE if default button snapping is enabled, otherwise FALSE
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_snap_to_default (void)
{
#ifdef UI_SYSTEM_NT

    char    value [16];

    return (nt_get_registry_string (
                HKEY_CURRENT_USER, "Control Panel\\Mouse",
                "SnapToDefaultButton", sizeof (value), value) &&
            atoi (value));

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_hide_cursor_when_typing
| Purpose:  Determine whether the pointer should be hidden when the user is
|           typing
| Input:
| Output:
| Return:   TRUE if the pointer should be hidden, otherwise FALSE
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_hide_cursor_when_typing (void)
{
#ifdef UI_SYSTEM_NT

    BOOL    flag;

    return (GetSystemParameter (SPI_GETMOUSEVANISH, flag));

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_system_animation
| Purpose:  Determine whether a named OS-defined animation is supported
| Input:    component   - the component id
| Output:
| Return:   TRUE if the OS-defined animation is supported, otherwise FALSE
\*--------------------------------------------------------------------------*/
int _uint_get_system_animation (int type, int *slide, int *fade)
{
#ifdef UI_SYSTEM_NT

    int     remote_app = _ui_server_is_remote_app ();
    BOOL    flag = FALSE;
    int     status = FALSE;

    switch (type)
    {
        case UI_SYSTEM_ANIMATION_HELP:
        {
            if (!remote_app &&
                GetSystemParameter (SPI_GETTOOLTIPANIMATION, flag))
            {
                if (GetSystemParameter (SPI_GETTOOLTIPFADE, flag))
                {
                    INIT_ARG (fade, TRUE);
                }
                else
                {
                    INIT_ARG (slide, TRUE);
                }

                status = TRUE;
            }

            break;
        }

        case UI_SYSTEM_ANIMATION_MENU:
        {
            if (!remote_app &&
                GetSystemParameter (SPI_GETMENUANIMATION, flag))
            {
                if (GetSystemParameter (SPI_GETMENUFADE, flag))
                {
                    INIT_ARG (fade, TRUE);
                }
                else
                {
                    INIT_ARG (slide, TRUE);
                }

                status = TRUE;
            }

            break;
        }

        case UI_SYSTEM_ANIMATION_SELECTION:
        {
            if (!remote_app &&
                GetSystemParameter (SPI_GETSELECTIONFADE, flag))
            {
                INIT_ARG (fade, TRUE);

                status = TRUE;
            }

            break;
        }

        case UI_SYSTEM_ANIMATION_DROPDOWN:
        {
            if (!remote_app &&
                GetSystemParameter (SPI_GETCOMBOBOXANIMATION, flag))
            {
                INIT_ARG (slide, TRUE);

                status = TRUE;
            }

            break;
        }

        case UI_SYSTEM_ANIMATION_STYLE:
        case UI_SYSTEM_ANIMATION_TAB:
        case UI_SYSTEM_ANIMATION_RESIZE:
        case UI_SYSTEM_ANIMATION_SCROLL:
        {
            //if (GetSystemParameter (SPI_GETLISTBOXSMOOTHSCROLLING, flag))
            if (remote_app ||
                _ui_server_is_cloud () ||
                GetSystemParameter (SPI_GETCLIENTAREAANIMATION, flag))
            {
                INIT_ARG (slide, TRUE);

                status = TRUE;
            }

            break;
        }
    }

    return (status);

#else

    return (FALSE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_menu_sizes
| Purpose:  Get the sizes of the menubar and menu items
| Input:    dpi             - the DPI
| Output:   menubar_height  - the height of the menubar
|           menuitem_width  - the width of a menuitem
|           menuitem_height - the height of a menuitem
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_menu_sizes (ui_dpi_t dpi, int *menubar_height,
                                    int *menuitem_width, int *menuitem_height)
{
#ifdef UI_SYSTEM_NT

    if (menubar_height != (int *) NULL)
    {
        *menubar_height =
            _ui_dpi_descale (
                UI_DPI_SYSTEM,
                _ui_dpi_scale (dpi, GetSystemMetrics (SM_CYMENU)));
    }

    if (menuitem_width != (int *) NULL)
    {
        *menuitem_width =
            _ui_dpi_descale (
                UI_DPI_SYSTEM,
                _ui_dpi_scale (dpi, GetSystemMetrics (SM_CXMENUSIZE)));
    }

    if (menuitem_height != (int *) NULL)
    {
        *menuitem_height =
            _ui_dpi_descale (
                UI_DPI_SYSTEM,
                _ui_dpi_scale (dpi, GetSystemMetrics (SM_CYMENUSIZE)));
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_send_message
| Purpose:  Faster version of SendMessage
| Input:    window      - window handle
|           message     - message id
|           wParam      - wParam
|           lParam      - lParam
| Output:
| Return:   The return from the window's window procedure
\*--------------------------------------------------------------------------*/
LRESULT _uint_send_message (HWND window, UINT message, WPARAM wParam,
                            LPARAM lParam)
{
    WNDPROC wnd_proc;
    LRESULT result = (LRESULT) 0;

    if (GetWindowInstanceU (window) == _uint_get_current_instance () &&
        (wnd_proc = _uint_get_wndproc_U (window)) != (WNDPROC) NULL)
    {
        result = (wnd_proc) (window, message, wParam, lParam);
    }

    return (result);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_animate_window
| Purpose:  Animate the given window in the given manner
| Input:    window      - the window to animate
|           duration    - the duration of the animation (in milliseconds)
|           flags       - flags representing the type of animation
| Output:
| Return:   Return value from the call to AnimateWindow
\*--------------------------------------------------------------------------*/
BOOL _uint_animate_window (HWND window, DWORD duration, DWORD flags)
{
    HRGN    region;
    BOOL    dwm = FALSE;
    BOOL    status = TRUE;

    if (IsChildWindow (window) ||
        IsWindowLayered (window))
    {
        /*
        ** Do not attempt to animate child window or layered windows
        */

        status = FALSE;
    }
    else
    {
        region = CreateRectRgn (0, 0, 0, 0);

        switch (_uint_get_window_region (window, region))
        {
            case SIMPLEREGION:
            case COMPLEXREGION:
            {
                if (DwmIsCompositionEnabled (&dwm) == S_OK &&
                    dwm)
                {
                    /*
                    ** Do not attempt to animate a window which has a
                    ** defined region whilst running with the DWM enabled
                    */

                    status = FALSE;
                }

                break;
            }
        }

        DeleteRgn (region);
    }

    if (status)
    {
        status = AnimateWindow (window, duration, flags);
    }
    else if (flags & AW_HIDE)
    {
        status = ShowWindow (window, SW_HIDE);
    }
    else if (flags & AW_ACTIVATE)
    {
        status = !(ShowWindow (window, SW_SHOW));
    }
    else
    {
        status = !(ShowWindow (window, SW_SHOWNA));
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_set_window_region
| Purpose:  Set the given region onto the given window
| Input:    window          - the window
|           region          - the region
|           redraw          - flag indicating whether to redraw the window
| Output:
| Return:   Non-zero if everything went, otherwise zero
\*--------------------------------------------------------------------------*/
int _uint_set_window_region (HWND window, HRGN region, BOOL redraw)
{
    BOOL    set = FALSE;
    HRGN    existing_region = CreateRectRgn (0, 0, 0, 0);
    RECT    rect, window_rect;
    int     status = GetWindowRgn (window, existing_region);

    if (region != (HRGN) NULL &&
        (status == ERROR ||
         status == NULLREGION ||
         !(EqualRgn (region, existing_region))))
    {
        if (GetRgnBox (region, &rect) == SIMPLEREGION &&
            GetClientRect (window, &window_rect) &&
            rect.left == window_rect.left &&
            rect.top == window_rect.top &&
            rect.right == window_rect.right &&
            rect.bottom == window_rect.bottom &&
            IsChildWindow (window))
        {
            /*
            ** Do not define child-window regions which correspond to the
            ** entire client-area of the window.
            */

            DeleteRgn (region);

            region = (HRGN) NULL;
        }
        else
        {
            set = TRUE;
        }
    }

    if (set ||
        (region == (HRGN) NULL &&
         status != ERROR &&
         status != NULLREGION))
    {
        status = SetWindowRgn (window, region, redraw);
    }
    else if (region != (HRGN) NULL)
    {
        DeleteRgn (region);
    }

    DeleteRgn (existing_region);

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_window_region
| Purpose:  Get the region of the given window
| Input:    window          - the window
| Output:   region          - the region
| Return:   The type of the region
\*--------------------------------------------------------------------------*/
int _uint_get_window_region (HWND window, HRGN region)
{
    return (GetWindowRgn (window, region));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_mousewheel_scroll_lines
| Purpose:  Get the number of lines to scroll for a WM_MOUSEWHEEL message
| Input:
| Output:
| Return:   The number of lines to scroll for a WM_MOUSEWHEEL message
\*--------------------------------------------------------------------------*/
int _uint_get_mousewheel_scroll_lines (void)
{
    int count = 3;

    SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &count, 0);

    return (count);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_mousewheel_scroll_chars
| Purpose:  Get the number of characters to scroll for a WM_MOUSEHWHEEL
|           message
| Input:
| Output:
| Return:   The number of characters to scroll for a WM_MOUSEHWHEEL message
\*--------------------------------------------------------------------------*/
int _uint_get_mousewheel_scroll_chars (void)
{
    int count = 3;

    SystemParametersInfo (SPI_GETWHEELSCROLLCHARS, 0, &count, 0);

    return (count);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_comp_from_message
| Purpose:  Get the component of the given message
| Input:    message     - the message
| Output:
| Return:   The component id of the given message, or DB_HANDLE_ERROR
\*--------------------------------------------------------------------------*/
ui_comp_t _uint_comp_from_message (MSG *message)
{
    ui_comp_t   component = DB_HANDLE_ERROR;

    _ui_component_from_event (message, &component);

    return (component);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_is_ui_window
| Purpose:  Determines if the given window was created by the UI
| Input:    window      - the window to check
| Output:
| Return:   TRUE if the window is a UI window, otherwise FALSE
\*--------------------------------------------------------------------------*/
int _uint_is_ui_window (HWND window)
{
    return (window != (HWND) NULL &&
            (_ui_component_from_window (
                 (void *) window, (ui_comp_t *) NULL) ||
             _uint_is_ui_atom_or_wndproc (window)));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_is_in_ui_window
| Purpose:  Determines if the given window was created by the UI
| Input:    window      - the window to check
| Output:   device      - the handle of the Dialog owner of the window
| Return:   TRUE if the window is a UI window, otherwise FALSE
\*--------------------------------------------------------------------------*/
int _uint_is_in_ui_window (HWND window, ui_comp_t *device)
{
    DWORD   thread_id;
    int     found = FALSE;
    int     status = FALSE;

    if (window != (HWND) NULL &&
        IsWindow (window) &&
        !(_ui_find_window ((void *) window)))
    {
        for (thread_id = GetCurrentThreadId ();
             window != (HWND) NULL &&
             (GetWindowThreadId (window) != thread_id ||
              ((status =
                found =
                _ui_component_from_window ((void *) window, device))
                   == FALSE &&
               (status = _uint_is_ui_atom_or_wndproc (window)) == FALSE));
             window = GetAncestor (window, GA_PARENT));
    }

    INIT_ARG (
        device,
        ((found &&
          *device != DB_HANDLE_ERROR) ?
         _ui_krn_get_device (*device) :
         DB_HANDLE_ERROR));

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_dlg_can_get_user_input
| Purpose : Determines if the given dialog window may currently
|           receive user input.
| Input   : dialog - to inquire of.
| Output  :
| Return  : TRUE  - if dialog may currently receive user input.
|           FALSE - otherwise.
\*--------------------------------------------------------------------------*/
int _uint_dlg_can_get_user_input (ui_comp_t dialog)
{
    int status = FALSE;

    /*
     * Check that the given window handle really is that of
     * a UI dialog
     */
    if (dialog == DB_HANDLE_ERROR)
    {
        /*
         * No device returned by _uint_is_in_ui_window, which means we are
         * dealing with messages for popup menus which are always active
         */

        status = UI_ACTIVE;
    }
    else if ((status = _ui_dlg_can_get_user_input (dialog)) == UI_ERROR)
    {
        _ui_error_msg (UIMSG_UI,
            "%s - failed to get user input state for dialog: %s",
            "_uint_dlg_can_get_user_input",
            _ui_msg_component_path (dialog));

        status = FALSE;
    }
#if 0
    else if (status == UI_ACTIVE &&
             _ui_trail_single_step () &&
             !(_ui_dlg_input_in_step (dialog)))
    {
        status = UI_BLOCKED;
    }
#endif
    return (status);
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_get_graphics_type
| Purpose:  Return the current graphics type of the system
| Input:
| Output:
| Return:   The current graphics type
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_graphics_type (void)
{
#ifdef UI_SYSTEM_NT

    return (WIN32_GDI);

#else

    return (NO_GRAPHICS);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_graphics_mode
| Purpose:  Return the current graphics mode of the system
| Input:
| Output:
| Return:   The current graphics mode
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_graphics_mode (void)
{
#ifdef UI_SYSTEM_NT

    return (IsWindowsXPOrGreater () ?
            UI_GRAPHICS_MODE_WINXP :
            UI_GRAPHICS_MODE_WIN98);

#else

    return (UI_GRAPHICS_MODE_NONE);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_flush
| Purpose:  Flush the pending GDI requests
| Input:
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_flush (void)
{
#ifdef UI_SYSTEM_NT

    int         i;
    ui_comp_t  *devices;
    void       *window;

    if (!(_ui_in_trail_mode ()) &&
        (i = _ui_krn_get_device_list (&devices)) > 0)
    {
        _ui_redraw_list_enable ();

        for (i--; i >= 0; i--)
        {
            if (_ui_krn_is_managed (devices [i]) &&
                _ui_krn_class_do_operation (
                    devices [i], UI_get_comp_dep_info_Op, (void **) NULL,
                    &window, (void **) NULL) == UI_SUCCESS &&
                window != NULL &&
                IsWindowVisible ((HWND) window))
            {
                RedrawWindow (
                    (HWND) window,
                    (RECT *) NULL,
                    (HRGN) NULL,
                    (RDW_UPDATENOW | RDW_ALLCHILDREN));
            }
        }

        _ui_krn_free_child_list (devices);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_set_system_input_language
| Purpose:  Set the input language to be used by the operating system
| Input:    language        - the language id
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_set_system_input_language (int language)
{
#ifdef UI_SYSTEM_NT

    HKL    *locale_ids;
    int     count, i;
    WORD    lang_id, sublang_id = (WORD) -1, not_sublang_id = (WORD) -1;

    if (!(is_generic_language (language)) &&
        (count = GetKeyboardLayoutList (0, (HKL *) NULL)) > 0)
    {
        locale_ids = (HKL *) getmem (sizeof (HKL) * count);

        if ((count = GetKeyboardLayoutList (count, locale_ids)) > 0)
        {
            switch (language)
            {
                case USASCII:
                {
                    lang_id = LANG_ENGLISH;
                    break;
                }

                case JAPANESE:
                {
                    lang_id = LANG_JAPANESE;
                    break;
                }

                case FRENCH:
                {
                    lang_id = LANG_FRENCH;
                    break;
                }

                case GERMAN:
                {
                    lang_id = LANG_GERMAN;
                    break;
                }

                case RUSSIAN:
                {
                    lang_id = LANG_RUSSIAN;
                    break;
                }

                case ITALIAN:
                {
                    lang_id = LANG_ITALIAN;
                    break;
                }

                case SPANISH:
                {
                    lang_id = LANG_SPANISH;
                    break;
                }

                case KOREAN:
                {
                    lang_id = LANG_KOREAN;
                    break;
                }

                case CHINESE_TW:
                {
                    lang_id = LANG_CHINESE;
                    sublang_id = SUBLANG_CHINESE_TRADITIONAL;
                    break;
                }

                case CHINESE_CN:
                {
                    lang_id = LANG_CHINESE;
                    not_sublang_id = SUBLANG_CHINESE_TRADITIONAL;
                    break;
                }

                case HEBREW:
                {
                    lang_id = LANG_HEBREW;
                    break;
                }

                case GREEK:
                {
                    lang_id = LANG_GREEK;
                    break;
                }

                case TURKISH:
                {
                    lang_id = LANG_TURKISH;
                    break;
                }

                case CZECH:
                {
                    lang_id = LANG_CZECH;
                    break;
                }

                case POLISH:
                {
                    lang_id = LANG_POLISH;
                    break;
                }

                case HUNGARIAN:
                {
                    lang_id = LANG_HUNGARIAN;
                    break;
                }

                case SLOVENIAN:
                {
                    lang_id = LANG_SLOVENIAN;
                    break;
                }

                case PORTUGUESE:
                {
                    lang_id = LANG_PORTUGUESE;
                    not_sublang_id = SUBLANG_PORTUGUESE_BRAZILIAN;
                    break;
                }

                case SLOVAK:
                {
                    lang_id = LANG_SLOVAK;
                    break;
                }

                case PORTUGUESE_BR:
                {
                    lang_id = LANG_PORTUGUESE;
                    sublang_id = SUBLANG_PORTUGUESE_BRAZILIAN;
                    break;
                }

                default:
                {
                    lang_id = LANG_NEUTRAL;
                }
            }

            for (i = 0; i < count; i++)
            {
                if (PRIMARYLANGID (LANGIDFROMLCID ((VoidToInt) locale_ids [i]))
                        == lang_id &&
                    (sublang_id == (WORD) -1 ||
                     SUBLANGID (LANGIDFROMLCID ((VoidToInt) locale_ids [i]))
                         == sublang_id) &&
                    (not_sublang_id == (WORD) -1 ||
                     SUBLANGID (LANGIDFROMLCID ((VoidToInt) locale_ids [i]))
                         != not_sublang_id))
                {
                    ActivateKeyboardLayout (locale_ids [i], KLF_SETFORPROCESS);
                    break;
                }
            }
        }

        relmem (&locale_ids);
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_system_input_language
| Purpose:  Get the input language used by the operating system
| Input:
| Output:
| Return:   The current input language id
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_system_input_language (void)
{
#ifdef UI_SYSTEM_NT

    return (_uint_get_language_from_locale (GetKeyboardLayout ((DWORD) 0)));

#else

    return (_ui_get_language ());

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_get_language_from_locale
| Purpose:  Get the language id of the given locale id
| Input:    locale_id       - the locale identifier
| Output:
| Return:   The language id of the locale
\*--------------------------------------------------------------------------*/
int _uint_get_language_from_locale (HKL locale_id)
{
    int language;

    switch (PRIMARYLANGID (LANGIDFROMLCID ((VoidToInt) locale_id)))
    {
        case LANG_NEUTRAL:
        case LANG_ENGLISH:
        {
            language = USASCII;
            break;
        }

        case LANG_JAPANESE:
        {
            language = JAPANESE;
            break;
        }

        case LANG_FRENCH:
        {
            language = FRENCH;
            break;
        }

        case LANG_GERMAN:
        {
            language = GERMAN;
            break;
        }

        case LANG_RUSSIAN:
        {
            language = RUSSIAN;
            break;
        }

        case LANG_ITALIAN:
        {
            language = ITALIAN;
            break;
        }

        case LANG_SPANISH:
        {
            language = SPANISH;
            break;
        }

        case LANG_KOREAN:
        {
            language = KOREAN;
            break;
        }

        case LANG_CHINESE:
        {
            language =
                ((SUBLANGID (LANGIDFROMLCID ((VoidToInt) locale_id))
                     == SUBLANG_CHINESE_TRADITIONAL) ?
                 CHINESE_TW :
                 CHINESE_CN);

            break;
        }

        case LANG_HEBREW:
        {
            language = HEBREW;
            break;
        }

        case LANG_GREEK:
        {
            language = GREEK;
            break;
        }

        case LANG_TURKISH:
        {
            language = TURKISH;
            break;
        }

        case LANG_CZECH:
        {
            language = CZECH;
            break;
        }

        case LANG_POLISH:
        {
            language = POLISH;
            break;
        }

        case LANG_HUNGARIAN:
        {
            language = HUNGARIAN;
            break;
        }

        case LANG_SLOVENIAN:
        {
            language = SLOVENIAN;
            break;
        }

        case LANG_PORTUGUESE:
        {
            language =
                ((SUBLANGID (LANGIDFROMLCID ((VoidToInt) locale_id))
                     == SUBLANG_PORTUGUESE_BRAZILIAN) ?
                 PORTUGUESE_BR :
                 PORTUGUESE);

            break;
        }

        case LANG_SLOVAK:
        {
            language = SLOVAK;
            break;
        }

        default:
        {
            language = UNKNOWN_LANG;
        }
    }

    return (language);
}

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_get_display_handle
| Purpose:  Get the display handle
| Input:
| Output:   display     - the display handle
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_display_handle (void **display)
{
#ifdef UI_SYSTEM_NT

    *display = (void *) _uint_get_current_instance ();

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_user_state
| Purpose:  Get the user state - the set of hints that show what the user
|           might be doing
| Input:    state       - the user state
| Output:   state       - the user state
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
UI_STATIC int _uint_get_user_state (int *state)
{
#ifdef UI_SYSTEM_NT

    DWORD           thread_id = GetWindowThreadId (_uint_get_app_window ());
    GUITHREADINFO   info = { sizeof (GUITHREADINFO) };

    /*
    ** Is the OS running its own modal event loop?
    */

    if (thread_id != (DWORD) 0 &&
        GetGUIThreadInfo (thread_id, &info) &&
        (info.flags &
             (GUI_INMENUMODE |
              GUI_INMOVESIZE |
              GUI_POPUPMENUMODE |
              GUI_SYSTEMMENUMODE)))
    {
        *state |= UI_USER_STATE_SYSTEM;
    }

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_register_wndproc
| Purpose:  Register the given window procedure as being owned by the UI
| Input:    wndproc     - the window procedure to register
| Output:
| Return:
\*--------------------------------------------------------------------------*/
void _uint_register_wndproc (LONG_PTR wndproc, ATOM atom)
{
    int count = XAR_COUNT (&wndproc_database), i;

    for (i = 0; i < count; i++)
    {
        if (wndproc_database [i] == wndproc)
        {
            break;
        }
        else if (wndproc_database [i] > wndproc)
        {
            XAR_INSERT (&wndproc_database, i, 1, &wndproc);
            break;
        }
    }

    if (i == count)
    {
        if (wndproc_database == (LONG_PTR *) NULL)
        {
            wndproc_database = XAR_BEGIN (LONG_PTR, 4);
        }

        XAR_APPEND (&wndproc_database, 1, &wndproc);
    }

    if (atom != 0)
    {
        count = XAR_COUNT (&atom_database);

        for (i = 0; i < count; i++)
        {
            if (atom_database [i] == atom)
            {
                break;
            }
            else if (atom_database [i] > atom)
            {
                XAR_INSERT (&atom_database, i, 1, &atom);
                break;
            }
        }

        if (i == count)
        {
            if (atom_database == (ATOM *) NULL)
            {
                atom_database = XAR_BEGIN (ATOM, 4);
            }

            XAR_APPEND (&atom_database, 1, &atom);
        }
    }
}


/*--------------------------------------------------------------------------*\
| Function: _uint_is_ui_wndproc
| Purpose:  Determine if the given window procedure is owned by the UI
| Input:    wndproc     - the window procedure to test
| Output:
| Return:   TRUE if the window procedure is owned by the UI, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_is_ui_wndproc (LONG_PTR wndproc)
{
    int start, end, i;

    for (start = 0, end = (XAR_COUNT (&wndproc_database) - 1); end >= start; )
    {
        i = ((start + end + 1) >> 1);

        if (wndproc_database [i] < wndproc)
        {
            start = (i + 1);
        }
        else if (wndproc_database [i] > wndproc)
        {
            end = (i - 1);
        }
        else
        {
            break;
        }
    }

    return (end >= start);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_is_ui_atom
| Purpose:  Determine if the given class atom is owned by the UI
| Input:    atom        - the class atom to test
| Output:
| Return:   TRUE if the class atom is owned by the UI, otherwise FALSE
\*--------------------------------------------------------------------------*/
static int _uint_is_ui_atom (ATOM atom)
{
    int start, end, i;

    for (start = 0, end = (XAR_COUNT (&atom_database) - 1); end >= start; )
    {
        i = ((start + end + 1) >> 1);

        if (atom_database [i] < atom)
        {
            start = (i + 1);
        }
        else if (atom_database [i] > atom)
        {
            end = (i - 1);
        }
        else
        {
            break;
        }
    }

    return (end >= start);
}

#endif /* UI_SYSTEM_NT */
