/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    @(#) uint.h 4203.1@(#)
|
|  Purpose: Windows NT level private header file
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  11-Apr-95 E-07-10 UK    $$1   Created
|  29-Jun-95         jas         Added WM_MOUSE... messages
|  04-Jul-95         jas         Added CBN... messages
|  12-Jul-95         jas         Added preliminary 3D support
|  17-Jul-95         gcn         Function protos for Pro/E fnlty
|  19-Jul-95         gcn         Function protos for font/color utils
|  20-Jul-95         gcn         Nt_data structure
|  24-Jul-95 G-01-01 UK    $$2   Submit to new cut
|  04-Aug-95         gcn         Protos for owner draw buttons
|  08-Aug-95 G-01-02 UK    $$3   Submission
|  05-Sep-95         jas         Added WM_POSTHSCROLL
|  07-Sep-95 G-01-05 UK    $$4   Automatic submission
|  04-Oct-95         jas         Added WM_CTLCOLORBKGND message
|  10-Oct-95 G-01-09 UK    $$5   Automatic Submission
|  16-Oct-95         gcn         Proto for _uint_comp_from_window
|  17-Oct-95 G-01-10 UK    $$6   Automatic Submission
|  09-Nov-95         jas         Removed obsolete function prototypes
|  16-Nov-95 G-01-13 UK    $$7   Automatic Submission
|  06-Dec-95         jas         Moved #define for UNICODE before #include of
|                                windows.h
|  07-Dec-95 G-01-16 UK    $$8   Automatic Submission
|  11-Dec-95         jas         Only define UNICODE if we're not using
|                                Windows 95
|  14-Dec-95 G-01-17 UK    $$9   Automatic Submission
|  09-Jan-96         jas         Added WM_POSTLBUTTONDOWN and ...UP messages
|  15-Jan-96         jas         Removed include for ctl3d.h
|  23-Jan-96 G-03-01 UK    $$10  Automatic Submission
|  23-Jan-96         jas         Added runtime 3D support
|  31-Jan-96         jas         Added Windows 95 runtime 3D support
|                                Shifted Windows 95 messages up by 0x7000 to
|                                avoid conflicts with system messages
|  06-Feb-96         jas         Changed CBN_DROPPED to be a user message
|  07-Feb-96 G-03-02 UK    $$11  Automatic Submission
|  13-Feb-96         jas         Use new string and font code
|  20-Feb-96         jas         Define DSTCOPY ROP code
|  21-Feb-96 G-03-03 UK    $$12  Automatic Submission
|  26-Feb-96         jas         Define NOTSRCAND ROP code
|  26-Feb-96         jas         Added UINT_COLOR_ENV_STRING
|  05-Mar-96 G-03-04 UK    $$13  Automatic Submission
|  31-May-96         jas         Changed WM_CTLCOLORBKGND to BM_GETCOLOR
|  04-Jun-96 G-03-16 UK    $$14  Automatic Submission
|  05-Jun-96         jas         Added support for Windows NT 4.0
|  11-Jun-96 G-03-17 UK    $$15  Automatic Submission
|  22-Aug-96         jas         Shifted Windows NT messages up by 0x7000 to
|                                avoid conflicts with Pro/E messages
|  04-Sep-96         gcn         Protos for _uint_is_dialog/menu_window
|  04-Sep-96 H-01-07 UK    $$16  Automatic Submission
|  09-Sep-96         jas         Added UINT_SYSTEM_... macros
|  11-Sep-96 H-01-08 UK    $$17  Automatic Submission
|  11-Sep-96         jas         Moved UNICODE macros to uint_string.c
|  12-Sep-96         jas         Removed MSVC++ 2.0 work-around for I486_NT
|  17-Sep-96 H-01-09 UK    $$18  Automatic Submission
|  18-Sep-96         jas         Added SIGNED_LOWORD and SIGNED_HIWORD
|  24-Sep-96 H-01-10 UK    $$19  Automatic Submission
|  18-Oct-96         jas         Added WM_POSTACTIVATE
|  22-Oct-96 H-01-14 UK    $$20  Automatic Submission
|  15-Jan-97         jas         Added CBN_SETCOMBOLBOXWINDOW
|  21-Jan-97 H-01-24 UK    $$21  Automatic Submission
|  12-Feb-97         jas         Added SC_LOWER
|  26-Feb-97 H-03-02 UK    $$22  Automatic Submission
|  16-Apr-97         jas         Added black and white color codes
|  22-Apr-97 H-03-07 UK    $$23  Automatic Submission
|  30-May-97         jas         Added new keyboard handling
|  04-Jun-97 H-03-13 UK    $$24  Automatic Submission
|  02-Jul-97         jas         Added WM_STOP_WORKING
|  08-Jul-97 H-03-16 UK    $$25  Automatic Submission
|  30-Jul-97         jas         Added _uint_get_menu_show_delay
|  12-Aug-97 H-03-18 UK    $$26  Automatic Submission
|  29-Aug-97         jas         Added WM_POSTACTIVATEAPP
|  03-Sep-97 H-03-20 UK    $$27  Automatic Submission
|  03-Sep-97         jas         Added _uint_send_message
|  09-Sep-97 H-03-21 UK    $$28  Automatic Submission
|  16-Oct-97         jas         Undefine WM_MOUSEENTER and WM_MOUSELEAVE
|                                before defining them here
|  17-Oct-97         jas         Added _uint_comp_from_message
|  21-Oct-97 H-03-27 UK    $$29  Automatic Submission
|  10-Nov-97         jas         Removed UINT_COLOR_ENV_STRING
|  18-Nov-97 H-03-30 UK    $$30  Automatic Submission
|  16-Dec-97 H-03-33 UK    $$31  Automatic Submission
|  16-Dec-97         jas         Added application window
|  17-Dec-97         jas         Obsoleted UINT_3D
|  23-Dec-97 H-03-34 UK    $$32  Automatic Submission
|  29-Dec-97         jas         Added WM_RAISEWINDOW
|  06-Jan-98 H-03-35 UK    $$33  Automatic Submission
|  13-Jan-98         jas         Added _uint_set_cursor
|  20-Jan-98 H-03-37 UK    $$34  Automatic Submission
|  20-Jan-98         jas         Changed WM_RAISEWINDOW to WM_POSTSETFOCUS
|  10-Feb-98 H-03-38 UK    $$35  Automatic Submission
|  15-Apr-98         jas         Added WM_COMPOSECHAR
|  20-Apr-98 I-01-04 UK    $$36  Automatic Submission
|  28-May-98         jas         Added _uint_unicode_enabled
|  01-Jun-98 I-01-10 UK    $$37  Automatic Submission
|  17-Jun-98         AW          Conditionally compile in sccs id
|  22-Jun-98 I-01-12 UK    $$38  Automatic Submission
|  24-Jun-98         jas         Changed _uint_dotted_polyline
|  29-Jun-98 I-01-13 UK    $$39  Automatic Submission
|  30-Jun-98         jas         Added _uint_replay_message
|  30-Jun-98         jas         Added grab and ungrab functions
|  13-Jul-98 I-01-14 UK    $$40  Automatic Submission
|  18-Aug-98         jas         Added _uint_play_sound
|  18-Aug-98         jas         Added menu sounds
|  27-Aug-98 I-01-17 UK    $$41  Automatic Submission
|  03-Sep-98         jas         Removed unused functions
|  03-Sep-98         jas         Added _uint_notify_win_event
|  08-Sep-98 I-01-18 UK    $$42  Automatic Submission
|  03-Sep-99         AW          Added hack for WM_MOUSEWHEEL on win95
|  09-Sep-99 I-03-14 UK    $$43  Automatic Submission
|  21-Sep-99         jas         Added IntelliMouse(tm) support for Windows 95
|  21-Sep-99         jas         Added _uint_get_mousewheel_scroll_lines
|  21-Sep-99         jas         Removed WM_REALMOUSELAST
|  28-Sep-99 I-03-16 UK    $$44  Automatic Submission
|  09-Dec-99         jas         Added multiple monitor support
|  15-Dec-99 I-03-24 UK    $$45  Automatic Submission
|  17-Dec-99         jas         Added multiple monitor wrappers
|  06-Jan-00 I-03-26 UK    $$46  Automatic Submission
|  22-Mar-00         jas         Added _uint_get_language_from_locale
|  29-Mar-00 J-01-05 UK    $$47  Automatic Submission
|  01-Aug-00         jas         Added 64-bit support
|  02-Aug-00         jas         Fixed 64-bit compilation problems
|  08-Aug-00 J-01-14 UK    $$48  Automatic Submission
|  18-Oct-00         jas         Added _uint_get_gui_thread_info
|  16-Nov-00 J-01-21 UK    $$49  Automatic Submission
|  14-Mar-01         jas         Added _uint_get_current_window
|  15-Mar-01 J-01-29 UK    $$50  Automatic Submission
|  17-Nov-00         jas         Removed UNICODE dependency
|  25-Jan-01         jas         Added _uint_animate_window
|  26-Jan-01         jas         Added _uint_update_layered_window
|  29-Jan-01         jas         Added WS_EX_LAYERED
|  12-Jun-01 J-03-01 UK    $$51  Automatic Submission
|  04-Jul-01         jas         Added GlobalFindAtom
|  09-Jul-01         jas         Added further 64-bit support
|  12-Jul-01 J-03-03 UK    $$52  Automatic Submission
|  09-Oct-01         jas         Use pro_is_win95_running
|  18-Oct-01 J-03-10 UK    $$53  Automatic Submission
|  18-Oct-01         jas         Added RegisterClipboardFormat
|  19-Oct-01         jas         Removed _uint_get_windows_system
|  30-Oct-01 J-03-11 UK    $$54  Automatic Submission
|  28-Jan-02         jas         Define COBJMACROS before including windows.h
|  31-Jan-02 J-03-18 UK    $$55  Automatic Submission
|  01-Mar-02         jas         Removed obsolete functions
|  05-Mar-02 J-03-20 UK    $$56  Automatic Submission
|  15-Mar-02         jas         Added _uint_set_focus
|  20-Mar-02 J-03-21 UK    $$57  Automatic Submission
|  09-May-02         jas         Removed _uint_is_menu_window
|  14-May-02 J-03-25 UK    $$58  Automatic Submission
|  02-Oct-02         jas         Added MapVirtualKeyEx
|  17-Oct-02 J-03-35 UK    $$59  Automatic Submission
|  02-Oct-03         jas         Added 64-bit processor architectures
|  09-Oct-03 K-01-16 UK    $$60  Automatic Submission
|  14-Jan-04         jas         Fixed return of DispatchMessage
|  20-Jan-04 K-01-22 UK    $$61  Automatic Submission
|  26-Feb-04         jas         Added _uint_app_window_add_atom
|  02-Mar-04 K-01-24 UK    $$62  Automatic Submission
|  04-Mar-04         jas         Added _uint_alpha_blend
|  16-Mar-04 K-01-25 UK    $$63  Automatic Submission
|  19-Mar-04         jas         Added WM_MOUSEHWHEEL
|  12-May-04 K-03-01 UK    $$64  Automatic Submission
|  08-Jun-04         jas         Added _uint_gradient_fill
|  08-Jun-04 K-03-03 UK    $$65  Automatic Submission
|  18-Jun-04         jas         Added _uint_set_DC_brush/pen_color
|  22-Jun-04 K-03-04 UK    $$66  Automatic Submission
|  12-Jul-04         AW          Removed _uint_get_previous_instance
|  13-Jul-04         AW          Added _uint_set_instance_handle
|  20-Jul-04 K-03-06 UK    $$67  Automatic Submission
|  01-Sep-04         jas         Added GetWindowText and GetWindowTextLength
|  21-Sep-04 K-03-10 UK    $$68  Automatic Submission
|  05-Nov-04         AW          Added _uint_record_window_class
|  10-Nov-04         AW          Removed _uint_get_ui_class_name_by_windproc
|  18-Nov-04 K-03-14 UK    $$69  Automatic Submission
|  21-Feb-05         jas         Added GetGUIThreadInfo
|  23-Feb-05         jas         Added SM_TABLETPC and SM_MEDIACENTER
|  24-Feb-05         jas         Added DT_HIDEPREFIX
|  25-Feb-05         jas         Added WM_UPDATEUISTATE
|  01-Mar-05 K-03-20 UK    $$70  Automatic Submission
|  11-Oct-05         jas         Removed _uint_unicode_enabled
|  11-Oct-05         jas         Removed pro_is_win95_running
|  11-Oct-05         jas         Removed _uint_set_cursor
|  11-Oct-05         jas         Removed pre-Windows 2000 code
|  27-Oct-05         jas         Added _uint_register_wndproc
|  31-Jan-06 L-01-01 UK    $$71  Automatic Submission
|  01-Feb-06         jas         Removed more pre-Windows 2000 code
|  14-Feb-06 L-01-02 UK    $$72  Automatic Submission
|  06-Mar-06         jas         Added IsHungAppWindow
|  16-Mar-06 L-01-04 UK    $$73  Automatic Submission
|  26-Jun-06         jas         Added _uint_get_mousewheel_scroll_chars
|  27-Jun-06 L-01-11 UK    $$74  Automatic Submission
|  16-Nov-06         jas         Added WM_PARENTENTER and WM_PARENTLEAVE
|  28-Nov-06 L-01-21 UK    $$75  Automatic Submission
|  18-Dec-06         jas         Added mouse crossing detail notifications
|  03-Jan-07 L-01-23 UK    $$76  Automatic Submission
|  09-Jan-07         jas         Added _uint_animate_window
|  10-Jan-07 L-01-24 UK    $$77  Automatic Submission
|  05-Feb-07         AW          Added _uint_get_comp_from_cursor_pos
|  09-Feb-07         jas         Added _uint_set/get_wndproc
|  13-Feb-07 L-01-26 UK    $$78  Automatic Submission
|  18-Apr-07         jas         Removed _uint_get_comp_from_cursor_pos
|  24-Apr-07 L-01-30+UK    $$79  Automatic Submission
|  23-May-07         jas         Renamed WM_MOUSELEAVE to WM_MOUSEEXIT
|  23-May-07         jas         Added _uint_set_current_window
|  05-Jun-07 L-01-32 UK    $$80  Automatic Submission
|  16-Oct-07         jas         Added _uint_set/get_window_region
|  16-Oct-07         jas         Added _uint_get_window_region_box
|  23-Oct-07 L-01-40 UK    $$81  Automatic Submission
|  13-Jun-08         jas         Removed BM_GETCOLOR
|  17-Jun-08 L-03-11 UK    $$82  Automatic Submission
|  28-Oct-09         jas         Added BITMAPV5INFO
|  10-Nov-09 L-05-09 UK    $$83  Automatic Submission
|  02-Dec-09         jas         Removed obsolete functions
|  08-Dec-09 L-05-11 UK    $$84  Automatic Submission
|  22-Mar-10         jas         Added WM_GETTITLEBARINFOEX
|  31-Mar-10 L-05-19 UK    $$85  Automatic Submission
|  21-Apr-10         jas         Added DwmIsCompositionEnabled
|  27-Apr-10 L-05-21 UK    $$86  Automatic Submission
|  15-Jun-10         jas         Moved DwmIsCompositionEnabled to uint_theme.h
|  22-Jun-10 L-05-25 UK    $$87  Automatic Submission
|  30-Jun-10         jas         Added window long value convenience macros
|  07-Jul-10 L-05-26 UK    $$88  Automatic Submission
|  08-Jul-10         jas         Modified IsChildWindow
|  16-Jul-10         jas         Added WM_NCUAHDRAWCAPTION/FRAME
|  20-Jul-10 L-05-27 UK    $$89  Automatic Submission
|  26-Oct-10         jas         Added SWP_STATECHANGED
|  09-Nov-10 L-05-35 UK    $$90  Automatic Submission
|  13-Jan-11         jas         Added IsChildWindowVisible
|  18-Jan-11 L-05-40 UK    $$91  Automatic Submission
|  01-Apr-11         jas         Added WM_SYSTIMER
|  07-Apr-11         jas         Added LCS_DEVICE_CMYK
|  12-Apr-11 L-05-45 UK    $$92  Automatic Submission
|  22-Sep-11         jas         Added _uint_get_focus/capture
|  23-Sep-11         jas         Added _uint_has_focus/capture
|  04-Oct-11 P-10-09 UK    $$93  Automatic Submission
|  25-Nov-11         jas         Wrap CreateCompatibleDC and DeleteDC
|  29-Nov-11 P-10-13 UK    $$94  Automatic Submission
|  13-Feb-12         jas         Added _uint_windows_enabled
|  16-Feb-12 P-10-17+UK    $$95  Automatic Submission
|  02-Mar-12         jas         Added ChangeWindowMessageFilterEx
|  06-Mar-12 P-10-17+UK    $$96  Automatic Submission
|  23-Feb-12         jas         Added UI_PRIVATE
|  15-Mar-12         jas         Removed _uint_windows_enabled
|  20-Mar-12 P-20-01 UK    $$97  Automatic Submission
|  22-Mar-12         jas         Added protection to COBJMACROS and UNICODE
|  23-Mar-12         jas         Fixed compilation problems
|  03-Apr-12 P-20-02 UK    $$98  Automatic Submission
|  14-Aug-12         jas         Added _uint_RegisterClass
|  21-Aug-12 P-20-12 UK    $$99  Automatic Submission
|  26-Sep-12         jas         Added WM_NOTIFYKEY
|  16-Oct-12 P-20-15 UK    $$100 Automatic Submission
|  26-Oct-12         jas         Added WM_POSTENABLE
|  30-Oct-12 P-20-16 UK    $$101 Automatic Submission
|  06-Nov-12         jas         Removed unnecessary custom window messages
|  13-Nov-12 P-20-17 UK    $$102 Automatic Submission
|  09-Jan-13         jas         Added _uint_animate_window
|  10-Jan-13 P-20-21 UK    $$103 Automatic Submission
|  07-Jun-13         jas         Added ScreenBlt
|  18-Jun-13 P-20-32 UK    $$104 Automatic Submission
|  28-Jun-13         jas         Added _uint_set_event_filter_func
|  02-Jul-13 P-20-33 UK    $$105 Automatic Submission
|  11-Jul-13         jas         Added SetProcessDPIAware
|  16-Jul-13 P-20-34 UK    $$106 Automatic Submission
|  17-Oct-13         jas         Added SetProcessDpiAwareness
|  22-Oct-13 P-20-41 UK    $$107 Automatic Submission
|  27-Nov-13         jas         Added _uint_set_window_pos
|  06-Dec-13         jas         Added _uint_is_current_window
|  17-Dec-13 P-20-44 UK    $$108 Automatic Submission
|  18-Mar-14         jas         Added _uint_desktop_scale
|  18-Mar-14 P-20-50 UK    $$109 Automatic Submission
|  11-Jun-14         jas         Added atom to _uint_register_wndproc
|  11-Jun-14         jas         Added window class convenience macros
|  26-Jun-14 P-20-55 UK    $$110 Automatic Submission
|  30-Jan-15         jas         Added UINT_VERSION_WINDOWS_... macros
|  03-Feb-15 P-30-01 UK    $$111 Automatic Submission
|  03-Feb-15         jas         Changed _uint_get_windows_version to an int
|  10-Feb-15         jas         Added Get/SetWindowOwner
|  13-Feb-15         jas         Removed _uint_play_sound
|  17-Feb-15 P-30-02 UK    $$112 Automatic Submission
|  02-Jul-15         jas         Added UINT_VERSION_WINDOWS_10
|  07-Jul-15 P-30-12 UK    $$113 Automatic Submission
|  13-Jul-15         jas         Added SPI_GETMOUSEWHEELROUTING
|  23-Jul-15         jas         Added STATUS_NOT_IMPLEMENTED
|  04-Aug-15 P-30-13 UK    $$114 Automatic Submission
|  07-Aug-15         jas         Added IsKeyDown
|  07-Sep-15         jas         Added IsPenEvent and IsTouchEvent
|  16-Sep-15 P-30-16 UK    $$115 Automatic Submission
|  16-Sep-15         jas         Changed version identifiers to match SDK
|  30-Sep-15 P-30-17 UK    $$116 Automatic Submission
|  21-Oct-15         jas         Added WM_GESTURE
|  22-Oct-15         jas         Added IsPenOrTouchEvent
|  27-Oct-15         jas         Added IsTouchMessage
|  28-Oct-15 P-30-19 UK    $$117 Automatic Submission
|  02-Dec-15         jas         Added WM_TABLET_FLICK
|  09-Dec-15 P-30-22 UK    $$118 Automatic Submission
|  07-Jun-16         jas         Removed obsolete functions
|  10-Jun-16         jas         Added IsWindowLayered
|  13-Jun-16         jas         Added more convenience macros
|  21-Jun-16 P-30-34 UK    $$119 Automatic Submission
|  01-Sep-16         jas         Added more UI_COMPONENT_SYSTEM_NT
|  13-Sep-16 P-30-40 UK    $$120 Automatic Submission
|  27-Sep-16         jas         Added SM_MAXIMUMTOUCHES
|  27-Sep-16         jas         Added SM_DIGITIZER
|  13-Oct-16 P-30-41 UK    $$121 Automatic Submission
|  20-Dec-16         jas         Removed support for legacy graphics modes
|  20-Dec-16         jas         Removed unreferenced functions
|  25-Jan-17         jas         Added UI_NO_WIN32_DEMO
|  25-Jan-17         jas         Removed _uint_get_menu_show_delay
|  08-Feb-17         jas         Added support for the Windows 8.1 SDK
|  14-Mar-17 P-50-01 UK    $$122 Automatic Submission
|  27-Apr-17         jas         Removed pre-Windows 7 definitions
|  03-May-17 P-50-07 UK    $$123 Automatic Submission
|  27-Sep-17         jas         Added _uint_is_windows_version_or_greater
|  27-Sep-17         jas         Removed uint_string.h
|  10-Oct-17 P-50-31 UK    $$124 Automatic Submission
|  13-Nov-17         jas         Added GetWindowThreadId
|  15-Nov-17 P-50-36 UK    $$125 Automatic Submission
|  22-Mar-18         jas         Added _uint_app_window_atoms
|  24-Apr-18 P-60-01 UK    $$126 Automatic Submission
|  12-Nov-18         jas         Removed _uint_set_instance_handle
|  19-Nov-18 P-60-25 UK    $$127 Automatic Submission
|  04-Apr-19         jas         Removed UI_NO_WIN32_DEMO
|  15-Apr-19 P-70-05 UK    $$128 Automatic Submission
|  26-Jun-19         jas         Added SetProcessDpiAwarenessContext
|  02-Jul-19 P-70-16 UK    $$129 Automatic Submission
|  11-Sep-19         jas         Added SetWindowCompositionAttribute
|  18-Sep-19 P-70-27 UK    $$130 Automatic Submission
|  07-Oct-19         jas         Added ACCENT_ENABLE_ACRYLICBLURBEHIND
|  08-Oct-19         jas         Added ACCENT_DRAW_ALLBORDERS
|  16-Oct-19 P-70-30 UK    $$131 Automatic Submission
|  03-Jun-20         jas         Added UINT_DEBUG
|  10-Jun-20 P-80-07 UK    $$132 Automatic Submission
|  02-Jul-21         jas         Added IsWindows11OrGreater
|  06-Jul-21 P-90-16 UK    $$133 Automatic Submission
|  06-Sep-21         jas         Added GET_X/Y_LPARAM
|  07-Sep-21         jas         Added SWP_AEROSNAP
|  14-Sep-21 P-90-25 UK    $$134 Automatic Submission
|  07-Oct-21         jas         Added DWM_WINDOW_CORNER_PREFERENCE
|  13-Oct-21 P-90-29 UK    $$135 Automatic Submission
|  26-Sep-22         jas         Added _uint_d2d_initialize
|  28-Sep-22 Q-10-29 UK    $$136 Automatic Submission
|  03-Oct-22         jas         Added WM_UAHDESTROYWINDOW
|  06-Oct-22 Q-10-30 UK    $$137 Automatic Submission
|  09-Mar-23         jas         Removed _uint_desktop_scale
|  15-Mar-23 Q-11-03 UK    $$138 Automatic Submission
|  19-May-23         jas         Added WM_GETDPISCALEDSIZE
|  19-May-23         jas         Added AdjustWindowRectExForDpi
|  23-May-23 Q-11-13 UK    $$139 Automatic Submission
|  26-Jun-23         jas         Added reset to _uint_d2d_initialize
|  27-Jun-23 Q-11-18 UK    $$140 Automatic Submission
|  18-Sep-23         jas         Added GetDpiAwarenessContextForProcess
|  20-Sep-23 Q-11-30 UK    $$141 Automatic Submission
|  20-May-24         jas         Added _uint_d2d_renderer_create/destroy
|  22-May-24 Q-12-13 UK    $$142 Automatic Submission
|  16-Jun-25         jas         Added _uint_get/set_wndproc_U
|  24-Jun-25 Q-13-14 UK    $$143 Automatic Submission
|  26-Sep-25         jas         Added _uint_exception_name
|  30-Sep-25 Q-13-28 UK    $$144 Automatic Submission
|  17-Oct-25         jas         Added _uint_d2d_renderer_set_origin
|  22-Oct-25 Q-13-31 UK    $$145 Automatic Submission
|  30-Oct-25         jas         Added _uint_d2d_renderer_region
|  04-Nov-25 Q-13-33 UK    $$146 Automatic Submission
|  27-Mar-26         jas         Added WM_CAPTUREMOUSEMOVE
|  27-Mar-26 Q-27-03 UK    $$147 Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#ifndef UINT_H
#define UINT_H

#include <ui.h>
#include <uip.h>
#include <ui_kernel.h>

#ifdef UI_SYSTEM_NT

/*
** Define all C++ Object Interface macros
*/

#ifndef COBJMACROS
#define COBJMACROS
#endif /* COBJMACROS */


/*
** Use Unicode APIs
*/

#ifndef UNICODE
#define UNICODE
#endif /* UNICODE */


/*
** Include windows.h
*/

#include <syswindows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <ole2.h>
#include <recapis.h>
#include <tabflicks.h>
#include <shtypes.h>
#include <dwmapi.h>


#ifdef UINT_DEBUG
#include <uint_debug.h>
#endif /* UINT_DEBUG */


/*
** Windows NT UI messages
*/

#define WM_UI_USER                      (WM_USER + 0x7000)


/*
** Mouse Crossing messages
*/

#ifdef WM_MOUSEENTER
#undef WM_MOUSEENTER
#endif /* WM_MOUSEENTER */

#define WM_MOUSEENTER                   (WM_UI_USER + 0)

#ifdef WM_MOUSEEXIT
#undef WM_MOUSEEXIT
#endif /* WM_MOUSEEXIT */

#define WM_MOUSEEXIT                    (WM_UI_USER + 1)


/*
** Mouse Crossing Detail Notifications
*/

#define MCDN_ANCESTOR                   (1 << 0)
#define MCDN_VIRTUAL                    (1 << 1)
#define MCDN_INFERIOR                   (1 << 2)
#define MCDN_NONLINEAR                  (1 << 3)
#define MCDN_NONLINEARVIRTUAL           (MCDN_NONLINEAR | MCDN_VIRTUAL)


/*
** Keyboard messages
*/

#define WM_COMPOSECHAR                  (WM_UI_USER + 2)
#define WM_NOTIFYKEY                    (WM_UI_USER + 3)


/*
** Key states
*/

#define IsKeyDown(k)                    (GetKeyState (k) < 0)
#define IsKeyToggled(k)                 (GetKeyState (k) & 1)

#ifndef IsLButtonDown
#define IsLButtonDown()                 IsKeyDown (VK_LBUTTON)
#endif /* IsLButtonDown */

#ifndef IsMButtonDown
#define IsMButtonDown()                 IsKeyDown (VK_MBUTTON)
#endif /* IsMButtonDown */

#ifndef IsRButtonDown
#define IsRButtonDown()                 IsKeyDown (VK_RBUTTON)
#endif /* IsRButtonDown */


/*
** Mouse capture messages
*/

#define WM_CAPTUREMOUSEMOVE             (WM_UI_USER + 4)


/*
** Pen and Touch input
*/

#ifndef IsPenOrTouchEvent
#define IsPenOrTouchEvent(e)            (((e) & 0xffffff00) == 0xff515700)
#endif /* IsPenOrTouchEvent */

#ifndef IsPenOrTouchMessage
#define IsPenOrTouchMessage()           IsPenOrTouchEvent (GetMessageExtraInfo ())
#endif /* IsPenOrTouchMessage */

#ifndef IsPenEvent
#define IsPenEvent(e)                   (((e) & 0xffffff80) == 0xff515700)
#endif /* IsPenEvent */

#ifndef IsPenMessage
#define IsPenMessage()                  IsPenEvent (GetMessageExtraInfo ())
#endif /* IsPenMessage */

#ifndef IsTouchEvent
#define IsTouchEvent(e)                 (((e) & 0xffffff80) == 0xff515780)
#endif /* IsTouchEvent */

#ifndef IsTouchMessage
#define IsTouchMessage()                IsTouchEvent (GetMessageExtraInfo ())
#endif /* IsTouchMessage */


/*
** Black and White color codes
*/

#define COLOR_BLACK                     RGB (0, 0, 0)
#define COLOR_WHITE                     RGB (255, 255, 255)


/*
** Extra Raster Operation Codes
*/

#define NOTSRCAND                       (DWORD) 0x00220326
#define DSTCOPY                         (DWORD) 0x00AA0029


/*
** Convenience macros for getting coordinates from message parameters
*/

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                (int)(short) LOWORD(lp)
#endif /* GET_X_LPARAM */

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                (int)(short) HIWORD(lp)
#endif /* GET_Y_LPARAM */


/*
** Standard Error values (from ntstatus.h)
*/

#ifndef STATUS_NOT_IMPLEMENTED
#define STATUS_NOT_IMPLEMENTED          ((NTSTATUS) 0xC0000002L)
#endif /* STATUS_NOT_IMPLEMENTED */


/*
** SystemParametersInfo ids
*/

#ifndef SPI_GETMOUSEWHEELROUTING
#define SPI_GETMOUSEWHEELROUTING        0x201C
#endif /* SPI_GETMOUSEWHEELROUTING */

#ifndef MOUSEWHEEL_ROUTING_FOCUS
#define MOUSEWHEEL_ROUTING_FOCUS        0
#endif /* MOUSEWHEEL_ROUTING_FOCUS */

#ifndef MOUSEWHEEL_ROUTING_HYBRID
#define MOUSEWHEEL_ROUTING_HYBRID       1
#endif /* MOUSEWHEEL_ROUTING_HYBRID */

#ifndef MOUSEWHEEL_ROUTING_MOUSE_POS
#define MOUSEWHEEL_ROUTING_MOUSE_POS    2
#endif /* MOUSEWHEEL_ROUTING_MOUSE_POS */


/*
** SetWindowPos / WINDOWPOS flags
*/

#ifndef SWP_NOCLIENTSIZE
#define SWP_NOCLIENTSIZE                0x0800
#endif /* SWP_NOCLIENTSIZE */

#ifndef SWP_NOCLIENTMOVE
#define SWP_NOCLIENTMOVE                0x1000
#endif /* SWP_NOCLIENTMOVE */

#ifndef SWP_STATECHANGED
#define SWP_STATECHANGED                0x8000
#endif /* SWP_STATECHANGED */

#ifndef SWP_AEROANIMATED
#define SWP_AEROANIMATED                0x00080000
#endif /* SWP_AEROANIMATED */

#ifndef SWP_AEROSNAP
#define SWP_AEROSNAP                    0x00100000
#endif /* SWP_AEROSNAP */

#ifndef SWP_AEROPRIVATE
#define SWP_AEROPRIVATE                 0x00200000
#endif /* SWP_AEROPRIVATE */


/*
** UxTheme caption and frame drawing messages for Windows Visual Styles
*/

#ifndef WM_UAHDESTROYWINDOW
#define WM_UAHDESTROYWINDOW             0x0090
#endif /* WM_UAHDESTROYWINDOW */

#ifndef WM_UAHDRAWMENU
#define WM_UAHDRAWMENU                  0x0091
#endif /* WM_UAHDRAWMENU */

#ifndef WM_UAHDRAWMENUITEM
#define WM_UAHDRAWMENUITEM              0x0092
#endif /* WM_UAHDRAWMENUITEM */

#ifndef WM_UAHINITMENU
#define WM_UAHINITMENU                  0x0093
#endif /* WM_UAHINITMENU */

#ifndef WM_UAHMEASUREMENUITEM
#define WM_UAHMEASUREMENUITEM           0x0094
#endif /* WM_UAHMEASUREMENUITEM */

#ifndef WM_UAHNCPAINTMENUPOPUP
#define WM_UAHNCPAINTMENUPOPUP          0x0095
#endif /* WM_UAHNCPAINTMENUPOPUP */

#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION             0x00AE
#endif /* WM_NCUAHDRAWCAPTION */

#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME               0x00AF
#endif /* WM_NCUAHDRAWFRAME */


/*
** System timer message
*/

#ifndef WM_SYSTIMER
#define WM_SYSTIMER                     0x0118
#endif /* WM_SYSTIMER */


/*
** System global data copy message
*/

#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDATA               0x0049
#endif /* WM_COPYGLOBALDATA */


/*
** High-DPI APIs for Windows 8.1 and later
*/

#ifndef DPI_ENUMS_DECLARED
#define DPI_ENUMS_DECLARED

typedef enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2

} PROCESS_DPI_AWARENESS;

typedef enum MONITOR_DPI_TYPE
{
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI

} MONITOR_DPI_TYPE;

#endif /* DPI_ENUMS_DECLARED */

#ifndef _DPI_AWARENESS_CONTEXTS_

typedef HANDLE DPI_AWARENESS_CONTEXT;

#define DPI_AWARENESS_CONTEXT_UNAWARE               ((DPI_AWARENESS_CONTEXT) -1)
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE          ((DPI_AWARENESS_CONTEXT) -2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE     ((DPI_AWARENESS_CONTEXT) -3)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2  ((DPI_AWARENESS_CONTEXT) -4)
#define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED     ((DPI_AWARENESS_CONTEXT) -5)

#endif /* _DPI_AWARENESS_CONTEXTS_ */

#ifndef WM_DPICHANGED
#define WM_DPICHANGED                   0x02E0
#endif /* WM_DPICHANGED */

#ifndef WM_DPICHANGED_BEFOREPARENT
#define WM_DPICHANGED_BEFOREPARENT      0x02E2
#endif /* WM_DPICHANGED_BEFOREPARENT */

#ifndef WM_DPICHANGED_AFTERPARENT
#define WM_DPICHANGED_AFTERPARENT       0x02E3
#endif /* WM_DPICHANGED_AFTERPARENT */

#ifndef WM_GETDPISCALEDSIZE
#define WM_GETDPISCALEDSIZE             0x02E4
#endif /* WM_GETDPISCALEDSIZE */


/*
** DWM APIs for Windows 11 and later
*/

#ifndef DWMWA_COLOR_DEFAULT

#define DWMWA_USE_HOSTBACKDROPBRUSH             17  // [set] BOOL, Allows the use of host backdrop brushes for the window.
#define DWMWA_USE_IMMERSIVE_DARK_MODE           20  // [set] BOOL, Allows a window to either use the accent color, or dark, according to the user Color Mode preferences.
#define DWMWA_WINDOW_CORNER_PREFERENCE          33  // [set] WINDOW_CORNER_PREFERENCE, Controls the policy that rounds top-level window corners
#define DWMWA_BORDER_COLOR                      34  // [set] COLORREF, The color of the thin border around a top-level window
#define DWMWA_CAPTION_COLOR                     35  // [set] COLORREF, The color of the caption
#define DWMWA_TEXT_COLOR                        36  // [set] COLORREF, The color of the caption text
#define DWMWA_VISIBLE_FRAME_BORDER_THICKNESS    37  // [get] UINT, width of the visible border around a thick frame window
#define DWMWA_LAST                              38

typedef enum
{
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3

} DWM_WINDOW_CORNER_PREFERENCE;

#define DWMWA_COLOR_DEFAULT                     0xFFFFFFFF
#define DWMWA_COLOR_NONE                        0xFFFFFFFE

#endif /* DWMWA_COLOR_DEFAULT */


/*
** SetWindowCompositionAttribute definitions
*/

#define WCA_UNDEFINED                       0
#define WCA_NCRENDERING_ENABLED             1       /* Use DWMWA_NCRENDERING_ENABLED */
#define WCA_NCRENDERING_POLICY              2       /* Use DWMWA_NCRENDERING_POLICY */
#define WCA_TRANSITIONS_FORCEDISABLED       3       /* Use DWMWA_TRANSITIONS_FORCEDISABLED */
#define WCA_ALLOW_NCPAINT                   4       /* Use DWMWA_ALLOW_NCPAINT */
#define WCA_CAPTION_BUTTON_BOUNDS           5       /* Use DWMWA_CAPTION_BUTTON_BOUNDS */
#define WCA_NONCLIENT_RTL_LAYOUT            6       /* Use DWMWA_NONCLIENT_RTL_LAYOUT */
#define WCA_FORCE_ICONIC_REPRESENTATION     7       /* Use DWMWA_FORCE_ICONIC_REPRESENTATION */
#define WCA_EXTENDED_FRAME_BOUNDS           8       /* Use DWMWA_EXTENDED_FRAME_BOUNDS */
#define WCA_HAS_ICONIC_BITMAP               9       /* Use DWMWA_HAS_ICONIC_BITMAP */
#define WCA_THEME_ATTRIBUTES                10
#define WCA_NCRENDERING_EXILED              11
#define WCA_NCADORNMENTINFO                 12
#define WCA_EXCLUDED_FROM_LIVEPREVIEW       13      /* Use DWMWA_EXCLUDED_FROM_PEEK */
#define WCA_VIDEO_OVERLAY_ACTIVE            14
#define WCA_FORCE_ACTIVEWINDOW_APPEARANCE   15
#define WCA_DISALLOW_PEEK                   16      /* Use DWMWA_DISALLOW_PEEK */
#define WCA_CLOAK                           17      /* Use DWMWA_CLOAK */
#define WCA_CLOAKED                         18      /* Use DWMWA_CLOAKED */
#define WCA_ACCENT_POLICY                   19      /* ACCENTPOLICY - Accent policy */
#define WCA_FREEZE_REPRESENTATION           20      /* Use DWMWA_FREEZE_REPRESENTATION */
#define WCA_EVER_UNCLOAKED                  21
#define WCA_VISUAL_OWNER                    22
#define WCA_HOLOGRAPHIC                     23
#define WCA_EXCLUDED_FROM_DDA               24
#define WCA_PASSIVEUPDATEMODE               25      /* Use DWMWA_PASSIVE_UPDATE_MODE */
#define WCA_USEDARKMODECOLORS               26      /* Use DWMWA_USE_IMMERSIVE_DARK_MODE */
#define WCA_CORNER_STYLE                    27      /* Use DWMWA_WINDOW_CORNER_PREFERENCE */
#define WCA_PART_COLOR                      28
#define WCA_DISABLE_MOVESIZE_FEEDBACK       29
#define WCA_LAST                            30

typedef struct _WINDOWCOMPOSITIONATTRIBUTE
{
    DWORD   attribute;
    PVOID   pvData;
    SIZE_T  cbData;

} WINDOWCOMPOSITIONATTRIBUTE;

#define ACCENT_DISABLED                     0
#define ACCENT_ENABLE_GRADIENT              1
#define ACCENT_ENABLE_TRANSPARENTGRADIENT   2
#define ACCENT_ENABLE_BLURBEHIND            3
#define ACCENT_ENABLE_ACRYLICBLURBEHIND     4
#define ACCENT_ENABLE_HOSTBACKDROP          5
#define ACCENT_INVALID_STATE                6

#define ACCENT_DRAW_LEFTBORDER              0x00000020
#define ACCENT_DRAW_TOPBORDER               0x00000040
#define ACCENT_DRAW_RIGHTBORDER             0x00000080
#define ACCENT_DRAW_BOTTOMBORDER            0x00000100
#define ACCENT_DRAW_ALLBORDERS              \
(ACCENT_DRAW_LEFTBORDER | ACCENT_DRAW_TOPBORDER | ACCENT_DRAW_RIGHTBORDER | ACCENT_DRAW_BOTTOMBORDER)

typedef struct _ACCENTPOLICY
{
    DWORD   accentState;
    DWORD   accentFlags;
    DWORD   gradientColor;
    DWORD   animationId;

} ACCENTPOLICY;


/*
** Win32 API functions called dynamically
*/

#ifdef GetProcessDpiAwareness
#undef GetProcessDpiAwareness
#endif /* GetProcessDpiAwareness */

#define GetProcessDpiAwareness \
_uint_GetProcessDpiAwareness

HRESULT WINAPI GetProcessDpiAwareness (
    HANDLE,
    PROCESS_DPI_AWARENESS *
);

#ifdef SetProcessDpiAwareness
#undef SetProcessDpiAwareness
#endif /* SetProcessDpiAwareness */

#define SetProcessDpiAwareness \
_uint_SetProcessDpiAwareness

HRESULT WINAPI SetProcessDpiAwareness (
    PROCESS_DPI_AWARENESS
);

#ifdef GetDpiAwarenessContextForProcess
#undef GetDpiAwarenessContextForProcess
#endif /* GetDpiAwarenessContextForProcess */

#define GetDpiAwarenessContextForProcess \
_uint_GetDpiAwarenessContextForProcess

DPI_AWARENESS_CONTEXT WINAPI GetDpiAwarenessContextForProcess (
    HANDLE
);

#ifdef SetProcessDpiAwarenessContext
#undef SetProcessDpiAwarenessContext
#endif /* SetProcessDpiAwarenessContext */

#define SetProcessDpiAwarenessContext \
_uint_SetProcessDpiAwarenessContext

BOOL WINAPI SetProcessDpiAwarenessContext (
    DPI_AWARENESS_CONTEXT
);

#ifdef AreDpiAwarenessContextsEqual
#undef AreDpiAwarenessContextsEqual
#endif /* AreDpiAwarenessContextsEqual */

#define AreDpiAwarenessContextsEqual \
_uint_AreDpiAwarenessContextsEqual

BOOL WINAPI AreDpiAwarenessContextsEqual (
    DPI_AWARENESS_CONTEXT,
    DPI_AWARENESS_CONTEXT
);

#ifdef GetDpiForMonitor
#undef GetDpiForMonitor
#endif /* GetDpiForMonitor */

#define GetDpiForMonitor \
_uint_GetDpiForMonitor

HRESULT WINAPI GetDpiForMonitor (
    HMONITOR,
    MONITOR_DPI_TYPE,
    UINT *,
    UINT *
);

#ifdef GetScaleFactorForMonitor
#undef GetScaleFactorForMonitor
#endif /* GetScaleFactorForMonitor */

#define GetScaleFactorForMonitor \
_uint_GetScaleFactorForMonitor

HRESULT WINAPI GetScaleFactorForMonitor (
    HMONITOR,
    DEVICE_SCALE_FACTOR *
);

#ifdef AdjustWindowRectExForDpi
#undef AdjustWindowRectExForDpi
#endif /* AdjustWindowRectExForDpi */

#define AdjustWindowRectExForDpi \
_uint_AdjustWindowRectExForDpi

BOOL WINAPI AdjustWindowRectExForDpi (
    RECT *,
    DWORD,
    BOOL,
    DWORD,
    UINT
);

#ifdef SetWindowCompositionAttribute
#undef SetWindowCompositionAttribute
#endif /* SetWindowCompositionAttribute */

#define SetWindowCompositionAttribute \
_uint_SetWindowCompositionAttribute

BOOL WINAPI SetWindowCompositionAttribute (
    HWND,
    WINDOWCOMPOSITIONATTRIBUTE *
);


/*
** Win32 functions to go transparently through wrappers
*/

#ifndef UINT_NO_WIN32_WRAP

#ifdef CreateCompatibleDC
#undef CreateCompatibleDC
#endif /* CreateCompatibleDC */

#define CreateCompatibleDC \
_uint_CreateCompatibleDC

HDC WINAPI CreateCompatibleDC (
    HDC
);

#ifdef DeleteDC
#undef DeleteDC
#endif /* DeleteDC */

#define DeleteDC \
_uint_DeleteDC

BOOL WINAPI DeleteDC (
    HDC
);

#endif /* UINT_NO_WIN32_WRAP */


/*
** Win32 API for BitBlt calls which copy data from the screen.
**
** Some 3rd-party DRM software (e.g. Softcamp Secure Workplace 2.0 as
** used by HKMC) can cause BitBlt to fail when copying from the screen
** into a memory buffer.
**
** To counter this, we instead use MaskBlt with no mask bitmap supplied:
** the Win32 documentation stating that "If no mask bitmap is supplied,
** this function behaves exactly like BitBlt, using the foreground
** raster operation code."
*/

#ifdef ScreenBlt
#undef ScreenBlt
#endif /* ScreenBlt */

#define ScreenBlt(dc,dx,dy,w,h,sc,sx,sy,r) \
MaskBlt ((dc), (dx), (dy), (w), (h), (sc), (sx), (sy), \
(HBITMAP) NULL, 0, 0, MAKEROP4 ((r), (r)));


/*
** Virtual key codes
*/

#define VK_0                            (int) '0'
#define VK_1                            (int) '1'
#define VK_2                            (int) '2'
#define VK_3                            (int) '3'
#define VK_4                            (int) '4'
#define VK_5                            (int) '5'
#define VK_6                            (int) '6'
#define VK_7                            (int) '7'
#define VK_8                            (int) '8'
#define VK_9                            (int) '9'
#define VK_A                            (int) 'A'
#define VK_B                            (int) 'B'
#define VK_C                            (int) 'C'
#define VK_D                            (int) 'D'
#define VK_E                            (int) 'E'
#define VK_F                            (int) 'F'
#define VK_G                            (int) 'G'
#define VK_H                            (int) 'H'
#define VK_I                            (int) 'I'
#define VK_J                            (int) 'J'
#define VK_K                            (int) 'K'
#define VK_L                            (int) 'L'
#define VK_M                            (int) 'M'
#define VK_N                            (int) 'N'
#define VK_O                            (int) 'O'
#define VK_P                            (int) 'P'
#define VK_Q                            (int) 'Q'
#define VK_R                            (int) 'R'
#define VK_S                            (int) 'S'
#define VK_T                            (int) 'T'
#define VK_U                            (int) 'U'
#define VK_V                            (int) 'V'
#define VK_W                            (int) 'W'
#define VK_X                            (int) 'X'
#define VK_Y                            (int) 'Y'
#define VK_Z                            (int) 'Z'


/*
** Convenience macro for messages
*/

#ifndef SetMessage
#define SetMessage(msg,h,m,w,l) \
(msg)->hwnd = h; (msg)->message = m; (msg)->wParam = w; (msg)->lParam = l
#endif /* SetMessage */


/*
** Unicode convenience macros for class and window values
*/

#define GetClassLongU(w,i) \
(IsWindowUnicode (w) ? \
 GetClassLongW ((w), (i)) : \
 GetClassLongA ((w), (i)))

#define SetClassLongU(w,i,v) \
(IsWindowUnicode (w) ? \
 SetClassLongW ((w), (i), (v)) : \
 SetClassLongA ((w), (i), (v)))

#define GetClassLongPtrU(w,i) \
(IsWindowUnicode (w) ? \
 GetClassLongPtrW ((w), (i)) : \
 GetClassLongPtrA ((w), (i)))

#define SetClassLongPtrU(w,i,v) \
(IsWindowUnicode (w) ? \
 SetClassLongPtrW ((w), (i), (v)) : \
 SetClassLongPtrA ((w), (i), (v)))

#define GetWindowLongU(w,i) \
(IsWindowUnicode (w) ? \
 GetWindowLongW ((w), (i)) : \
 GetWindowLongA ((w), (i)))

#define SetWindowLongU(w,i,v) \
(IsWindowUnicode (w) ? \
 SetWindowLongW ((w), (i), (v)) : \
 SetWindowLongA ((w), (i), (v)))

#define GetWindowLongPtrU(w,i) \
(IsWindowUnicode (w) ? \
 GetWindowLongPtrW ((w), (i)) : \
 GetWindowLongPtrA ((w), (i)))

#define SetWindowLongPtrU(w,i,v) \
(IsWindowUnicode (w) ? \
 SetWindowLongPtrW ((w), (i), (v)) : \
 SetWindowLongPtrA ((w), (i), (v)))


/*
** Convenience macros for class and window values
*/

#ifdef GetClassAtom
#undef GetClassAtom
#endif /* GetClassAtom */

#define GetClassAtom(w) \
(ATOM) GetClassWord ((w), GCW_ATOM)

#ifdef GetWindowProc
#undef GetWindowProc
#endif /* GetWindowProc */

#define GetWindowProc(w) \
(WNDPROC) GetWindowLongPtr ((w), GWLP_WNDPROC)

#define SetWindowProc(w,v) \
(WNDPROC) SetWindowLongPtr ((w), GWLP_WNDPROC, (LONG_PTR) (v))

#define GetWindowProcA(w) \
(WNDPROC) GetWindowLongPtrA ((w), GWLP_WNDPROC)

#define SetWindowProcA(w,v) \
(WNDPROC) SetWindowLongPtrA ((w), GWLP_WNDPROC, (LONG_PTR) (v))

#define GetWindowProcW(w) \
(WNDPROC) GetWindowLongPtrW ((w), GWLP_WNDPROC)

#define SetWindowProcW(w,v) \
(WNDPROC) SetWindowLongPtrW ((w), GWLP_WNDPROC, (LONG_PTR) (v))

#define GetWindowProcU(w) \
(WNDPROC) GetWindowLongPtrU ((w), GWLP_WNDPROC)

#define SetWindowProcU(w,v) \
(WNDPROC) SetWindowLongPtrU ((w), GWLP_WNDPROC, (LONG_PTR) (v))

#ifdef GetWindowInstance
#undef GetWindowInstance
#endif /* GetWindowInstance */

#define GetWindowInstance(w) \
(HINSTANCE) GetWindowLongPtr ((w), GWLP_HINSTANCE)

#define SetWindowInstance(w,v) \
(HINSTANCE) SetWindowLongPtr ((w), GWLP_HINSTANCE, (LONG_PTR) (v))

#define GetWindowInstanceA(w) \
(HINSTANCE) GetWindowLongPtrA ((w), GWLP_HINSTANCE)

#define SetWindowInstanceA(w,v) \
(HINSTANCE) SetWindowLongPtrA ((w), GWLP_HINSTANCE, (LONG_PTR) (v))

#define GetWindowInstanceW(w) \
(HINSTANCE) GetWindowLongPtrW ((w), GWLP_HINSTANCE)

#define SetWindowInstanceW(w,v) \
(HINSTANCE) SetWindowLongPtrW ((w), GWLP_HINSTANCE, (LONG_PTR) (v))

#define GetWindowInstanceU(w) \
(HINSTANCE) GetWindowLongPtrU ((w), GWLP_HINSTANCE)

#define SetWindowInstanceU(w,v) \
(HINSTANCE) SetWindowLongPtrU ((w), GWLP_HINSTANCE, (LONG_PTR) (v))

#ifdef GetWindowParent
#undef GetWindowParent
#endif /* GetWindowParent */

#define GetWindowParent(w) \
(HWND) GetWindowLongPtr ((w), GWLP_HWNDPARENT)

#define SetWindowParent(w,v) \
(HWND) SetWindowLongPtr ((w), GWLP_HWNDPARENT, (LONG_PTR) (v))

#define GetWindowParentA(w) \
(HWND) GetWindowLongPtrA ((w), GWLP_HWNDPARENT)

#define SetWindowParentA(w,v) \
(HWND) SetWindowLongPtrA ((w), GWLP_HWNDPARENT, (LONG_PTR) (v))

#define GetWindowParentW(w) \
(HWND) GetWindowLongPtrW ((w), GWLP_HWNDPARENT)

#define SetWindowParentW(w,v) \
(HWND) SetWindowLongPtrW ((w), GWLP_HWNDPARENT, (LONG_PTR) (v))

#define GetWindowParentU(w) \
(HWND) GetWindowLongPtrU ((w), GWLP_HWNDPARENT)

#define SetWindowParentU(w,v) \
(HWND) SetWindowLongPtrU ((w), GWLP_HWNDPARENT, (LONG_PTR) (v))

#ifdef GetWindowStyle
#undef GetWindowStyle
#endif /* GetWindowStyle */

#define GetWindowStyle(w) \
(DWORD) GetWindowLong ((w), GWL_STYLE)

#define SetWindowStyle(w,v) \
(DWORD) SetWindowLong ((w), GWL_STYLE, (LONG) (v))

#define GetWindowStyleA(w) \
(DWORD) GetWindowLongA ((w), GWL_STYLE)

#define SetWindowStyleA(w,v) \
(DWORD) SetWindowLongA ((w), GWL_STYLE, (LONG) (v))

#define GetWindowStyleW(w) \
(DWORD) GetWindowLongW ((w), GWL_STYLE)

#define SetWindowStyleW(w,v) \
(DWORD) SetWindowLongW ((w), GWL_STYLE, (LONG) (v))

#define GetWindowStyleU(w) \
(DWORD) GetWindowLongU ((w), GWL_STYLE)

#define SetWindowStyleU(w,v) \
(DWORD) SetWindowLongU ((w), GWL_STYLE, (LONG) (v))

#ifdef GetWindowExStyle
#undef GetWindowExStyle
#endif /* GetWindowExStyle */

#define GetWindowExStyle(w) \
(DWORD) GetWindowLong ((w), GWL_EXSTYLE)

#define SetWindowExStyle(w,v) \
(DWORD) SetWindowLong ((w), GWL_EXSTYLE, (LONG) (v))

#define GetWindowExStyleA(w) \
(DWORD) GetWindowLongA ((w), GWL_EXSTYLE)

#define SetWindowExStyleA(w,v) \
(DWORD) SetWindowLongA ((w), GWL_EXSTYLE, (LONG) (v))

#define GetWindowExStyleW(w) \
(DWORD) GetWindowLongW ((w), GWL_EXSTYLE)

#define SetWindowExStyleW(w,v) \
(DWORD) SetWindowLongW ((w), GWL_EXSTYLE, (LONG) (v))

#define GetWindowExStyleU(w) \
(DWORD) GetWindowLongU ((w), GWL_EXSTYLE)

#define SetWindowExStyleU(w,v) \
(DWORD) SetWindowLongU ((w), GWL_EXSTYLE, (LONG) (v))

#ifdef GetWindowUserData
#undef GetWindowUserData
#endif /* GetWindowUserData */

#define GetWindowUserData(w) \
GetWindowLongPtr ((w), GWLP_USERDATA)

#define SetWindowUserData(w,v) \
SetWindowLongPtr ((w), GWLP_USERDATA, (LONG_PTR) (v))

#define GetWindowUserDataA(w) \
GetWindowLongPtrA ((w), GWLP_USERDATA)

#define SetWindowUserDataA(w,v) \
SetWindowLongPtrA ((w), GWLP_USERDATA, (LONG_PTR) (v))

#define GetWindowUserDataW(w) \
GetWindowLongPtrW ((w), GWLP_USERDATA)

#define SetWindowUserDataW(w,v) \
SetWindowLongPtrW ((w), GWLP_USERDATA, (LONG_PTR) (v))

#define GetWindowUserDataU(w) \
GetWindowLongPtrU ((w), GWLP_USERDATA)

#define SetWindowUserDataU(w,v) \
SetWindowLongPtrU ((w), GWLP_USERDATA, (LONG_PTR) (v))

#ifdef GetWindowID
#undef GetWindowID
#endif /* GetWindowID */

#define GetWindowID(w) \
(DWORD_PTR) GetWindowLongPtr ((w), GWLP_ID)

#define SetWindowID(w,v) \
(DWORD_PTR) SetWindowLongPtr ((w), GWLP_ID, (LONG_PTR) (v))

#define GetWindowIDA(w) \
(DWORD_PTR) GetWindowLongPtrA ((w), GWLP_ID)

#define SetWindowIDA(w,v) \
(DWORD_PTR) SetWindowLongPtrA ((w), GWLP_ID, (LONG_PTR) (v))

#define GetWindowIDW(w) \
(DWORD_PTR) GetWindowLongPtrW ((w), GWLP_ID)

#define SetWindowIDW(w,v) \
(DWORD_PTR) SetWindowLongPtrW ((w), GWLP_ID, (LONG_PTR) (v))

#define GetWindowIDU(w) \
(DWORD_PTR) GetWindowLongPtrU ((w), GWLP_ID)

#define SetWindowIDU(w,v) \
(DWORD_PTR) SetWindowLongPtrU ((w), GWLP_ID, (LONG_PTR) (v))


/*
** Convenience macro to determine window thread ID
*/

#ifdef GetWindowThreadId
#undef GetWindowThreadId
#endif /* GetWindowThreadId */

#define GetWindowThreadId(w) \
GetWindowThreadProcessId ((w), (DWORD *) NULL)


/*
** Convenience macro to define top-level window ownership
*/

#ifdef GetWindowOwner
#undef GetWindowOwner
#endif /* GetWindowOwner */

#define GetWindowOwner(w) \
GetWindow ((w), GW_OWNER)

#ifdef SetWindowOwner
#undef SetWindowOwner
#endif /* SetWindowOwner */

#define SetWindowOwner(w,v) \
SetWindowParent ((w), (v))


/*
** Convenience macro to detect a transparent window
*/

#ifdef IsWindowTransparent
#undef IsWindowTransparent
#endif /* IsWindowTransparent */

#define IsWindowTransparent(w) \
((GetWindowExStyle (w) & WS_EX_TRANSPARENT) != 0)

#define IsWindowTransparentA(w) \
((GetWindowExStyleA (w) & WS_EX_TRANSPARENT) != 0)

#define IsWindowTransparentW(w) \
((GetWindowExStyleW (w) & WS_EX_TRANSPARENT) != 0)

#define IsWindowTransparentU(w) \
((GetWindowExStyleU (w) & WS_EX_TRANSPARENT) != 0)


/*
** Convenience macro to detect a layered window
*/

#ifdef IsWindowLayered
#undef IsWindowLayered
#endif /* IsWindowLayered */

#define IsWindowLayered(w) \
((GetWindowExStyle (w) & WS_EX_LAYERED) != 0)

#define IsWindowLayeredA(w) \
((GetWindowExStyleA (w) & WS_EX_LAYERED) != 0)

#define IsWindowLayeredW(w) \
((GetWindowExStyleW (w) & WS_EX_LAYERED) != 0)

#define IsWindowLayeredU(w) \
((GetWindowExStyleU (w) & WS_EX_LAYERED) != 0)


/*
** Convenience macro to detect a topmost window
*/

#ifdef IsWindowTopmost
#undef IsWindowTopmost
#endif /* IsWindowTopmost */

#define IsWindowTopmost(w) \
((GetWindowExStyle (w) & WS_EX_TOPMOST) != 0)

#define IsWindowTopmostA(w) \
((GetWindowExStyleA (w) & WS_EX_TOPMOST) != 0)

#define IsWindowTopmostW(w) \
((GetWindowExStyleW (w) & WS_EX_TOPMOST) != 0)

#define IsWindowTopmostU(w) \
((GetWindowExStyleU (w) & WS_EX_TOPMOST) != 0)


/*
** Convenience macro to detect a child window
*/

#ifdef IsChildWindow
#undef IsChildWindow
#endif /* IsChildWindow */

#define IsChildWindow(w) \
((GetWindowStyle (w) & (WS_POPUP | WS_CHILD)) == WS_CHILD)

#define IsChildWindowA(w) \
((GetWindowStyleA (w) & (WS_POPUP | WS_CHILD)) == WS_CHILD)

#define IsChildWindowW(w) \
((GetWindowStyleW (w) & (WS_POPUP | WS_CHILD)) == WS_CHILD)

#define IsChildWindowU(w) \
((GetWindowStyleU (w) & (WS_POPUP | WS_CHILD)) == WS_CHILD)


/*
** Convenience macro to detect a child window visibility without checking
** its parent hierarchy
*/

#ifdef IsChildWindowVisible
#undef IsChildWindowVisible
#endif /* IsChildWindowVisible */

#define IsChildWindowVisible(w) \
((GetWindowStyle (w) & WS_VISIBLE) != 0)

#define IsChildWindowVisibleA(w) \
((GetWindowStyleA (w) & WS_VISIBLE) != 0)

#define IsChildWindowVisibleW(w) \
((GetWindowStyleW (w) & WS_VISIBLE) != 0)

#define IsChildWindowVisibleU(w) \
((GetWindowStyleU (w) & WS_VISIBLE) != 0)


/*
** Convenience macros for children
*/

#ifndef GetFirstChild
#define GetFirstChild(w) \
GetWindow ((w), GW_CHILD)
#endif /* GetFirstChild */

#ifndef GetFirstSibling
#define GetFirstSibling(w) \
GetWindow ((w), GW_HWNDFIRST)
#endif /* GetFirstSibling */

#ifndef GetLastSibling
#define GetLastSibling(w) \
GetWindow ((w), GW_HWNDLAST)
#endif /* GetLastSibling */

#ifndef GetNextSibling
#define GetNextSibling(w) \
GetWindow ((w), GW_HWNDNEXT)
#endif /* GetNextSibling */

#ifndef GetPrevSibling
#define GetPrevSibling(w) \
GetWindow ((w), GW_HWNDPREV)
#endif /* GetPrevSibling */


/*
** Convenience macros to check the window state
*/

#ifndef IsMinimized
#define IsMinimized(w) \
IsIconic (w)
#endif /* IsMinimized */

#ifndef IsMaximized
#define IsMaximized(w) \
IsZoomed (w)
#endif /* IsMaximized */

#ifndef IsRestored
#define IsRestored(w) \
!(GetWindowStyle (w) & (WS_MINIMIZE | WS_MAXIMIZE))
#endif /* IsRestored */


/*
** Convenience macros for co-ordinate mapping
*/

#ifndef MapWindowPoint
#define MapWindowPoint(f,t,p) \
MapWindowPoints ((f), (t), (p), 1)
#endif /* MapWindowRect */

#ifndef MapWindowRect
#define MapWindowRect(f,t,r) \
MapWindowPoints ((f), (t), (POINT *) (r), 2)
#endif /* MapWindowRect */


/*
** Convenience macros for regions
*/

#ifdef DeleteRgn
#undef DeleteRgn
#endif /* DeleteRgn */

#define DeleteRgn(r) \
DeleteObject ((HGDIOBJ)(HRGN) (r))

#ifdef CopyRgn
#undef CopyRgn
#endif /* CopyRgn */

#define CopyRgn(r,s) \
CombineRgn ((r), (s), 0, RGN_COPY)

#ifdef IntersectRgn
#undef IntersectRgn
#endif /* IntersectRgn */

#define IntersectRgn(r,a,b) \
CombineRgn ((r), (a), (b), RGN_AND)

#ifdef UnionRgn
#undef UnionRgn
#endif /* UnionRgn */

#define UnionRgn(r,a,b) \
CombineRgn ((r), (a), (b), RGN_OR)

#ifdef SubtractRgn
#undef SubtractRgn
#endif /* SubtractRgn */

#define SubtractRgn(r,a,b) \
CombineRgn ((r), (a), (b), RGN_DIFF)

#ifdef XorRgn
#undef XorRgn
#endif /* XorRgn */

#define XorRgn(r,a,b) \
CombineRgn ((r), (a), (b), RGN_XOR)


/*
** Convenience macros for GDI objects
*/

#ifndef DeletePen
#define DeletePen(p) \
DeleteObject ((HGDIOBJ)(HPEN) (p))
#endif /* DeletePen */

#ifndef SelectPen
#define SelectPen(c,p) \
(HPEN) SelectObject ((c), (HGDIOBJ)(HPEN) (p))
#endif /* SelectPen */

#ifndef GetStockPen
#define GetStockPen(p) \
(HPEN) GetStockObject (p)
#endif /* GetStockPen */

#ifndef DeleteBrush
#define DeleteBrush(b) \
DeleteObject ((HGDIOBJ)(HBRUSH) (b))
#endif /* DeleteBrush */

#ifndef SelectBrush
#define SelectBrush(c,b) \
(HBRUSH) SelectObject ((c), (HGDIOBJ)(HBRUSH) (b))
#endif /* SelectBrush */

#ifndef GetStockBrush
#define GetStockBrush(b) \
(HBRUSH) GetStockObject (b)
#endif /* GetStockBrush */

#ifndef DeleteBitmap
#define DeleteBitmap(b) \
DeleteObject ((HGDIOBJ)(HBITMAP) (b))
#endif /* DeleteBitmap */

#ifndef SelectBitmap
#define SelectBitmap(c,b) \
(HBITMAP) SelectObject ((c), (HGDIOBJ)(HBITMAP) (b))
#endif /* SelectBitmap */

#ifndef DeletePalette
#define DeletePalette(p) \
DeleteObject ((HGDIOBJ)(HPALETTE) (p))
#endif /* DeletePalette */

#ifndef DeleteFont
#define DeleteFont(f) \
DeleteObject ((HGDIOBJ)(HFONT) (f))
#endif /* DeleteFont */

#ifndef SelectFont
#define SelectFont(c,f) \
(HFONT) SelectObject ((c), (HGDIOBJ)(HFONT) (f))
#endif /* SelectFont */

#ifndef GetStockFont
#define GetStockFont(f) \
(HFONT) GetStockObject (f)
#endif /* GetStockFont */


/*
** Convenience structure for version 4 bitmaps
*/

typedef struct
{
    BITMAPV4HEADER  bmiHeader;
    RGBQUAD         bmiColors [3];

} BITMAPV4INFO;


/*
** Convenience structure for version 5 bitmaps
*/

typedef struct
{
    BITMAPV5HEADER  bmiHeader;
    RGBQUAD         bmiColors [3];

} BITMAPV5INFO;


/*
** Logical Color Space types
*/

#ifndef LCS_CALIBRATED_RGB
#define LCS_CALIBRATED_RGB              0x00000000L
#endif /* LCS_CALIBRATED_RGB */

#ifndef LCS_DEVICE_RGB
#define LCS_DEVICE_RGB                  0x00000001L /* LCS_sRGB */
#endif /* LCS_DEVICE_RGB */

#ifndef LCS_DEVICE_CMYK
#define LCS_DEVICE_CMYK                 0x00000002L /* LCS_WINDOWS_COLOR_SPACE */
#endif /* LCS_DEVICE_CMYK */


/*
** Windows version identifiers
*/

#ifndef _WIN32_WINNT_WIN11
#define _WIN32_WINNT_WIN11              0x0B00
#endif /* _WIN32_WINNT_WIN11 */

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10              0x0A00
#endif /* _WIN32_WINNT_WIN10 */

#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE            0x0603
#endif /* _WIN32_WINNT_WINBLUE */

#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8               0x0602
#endif /* _WIN32_WINNT_WIN8 */

#ifndef _WIN32_WINNT_WIN7
#define _WIN32_WINNT_WIN7               0x0601
#endif /* _WIN32_WINNT_WIN7 */

#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA              0x0600
#endif /* _WIN32_WINNT_VISTA */

#ifndef _WIN32_WINNT_WINXP
#define _WIN32_WINNT_WINXP              0x0501
#endif /* _WIN32_WINNT_WINXP */

#ifndef _WIN32_WINNT_WIN2K
#define _WIN32_WINNT_WIN2K              0x0500
#endif /* _WIN32_WINNT_WIN2K */


/*
** Convenience macros for Windows version control
*/

#ifdef IsWindows11OrGreater
#undef IsWindows11OrGreater
#endif /* IsWindows11OrGreater */

#define IsWindows11OrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WIN11)

#ifdef IsWindows10OrGreater
#undef IsWindows10OrGreater
#endif /* IsWindows10OrGreater */

#define IsWindows10OrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WIN10)

#ifdef IsWindows8Point1OrGreater
#undef IsWindows8Point1OrGreater
#endif /* IsWindows8Point1OrGreater */

#define IsWindows8Point1OrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WINBLUE)

#ifdef IsWindows8OrGreater
#undef IsWindows8OrGreater
#endif /* IsWindows8OrGreater */

#define IsWindows8OrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WIN8)

#ifdef IsWindows7OrGreater
#undef IsWindows7OrGreater
#endif /* IsWindows7OrGreater */

#define IsWindows7OrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WIN7)

#ifdef IsWindowsVistaOrGreater
#undef IsWindowsVistaOrGreater
#endif /* IsWindowsVistaOrGreater */

#define IsWindowsVistaOrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_VISTA)

#ifdef IsWindowsXPOrGreater
#undef IsWindowsXPOrGreater
#endif /* IsWindowsXPOrGreater */

#define IsWindowsXPOrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WINXP)

#ifdef IsWindows2000OrGreater
#undef IsWindows2000OrGreater
#endif /* IsWindows2000OrGreater */

#define IsWindows2000OrGreater() \
_uint_is_windows_version_or_greater (_WIN32_WINNT_WIN2K)

#ifdef IsWindowsMEOrGreater
#undef IsWindowsMEOrGreater
#endif /* IsWindowsMEOrGreater */

#define IsWindowsMEOrGreater() \
_uint_is_windows_version_or_greater (0x0490)

#ifdef IsWindows98OrGreater
#undef IsWindows98OrGreater
#endif /* IsWindows98OrGreater */

#define IsWindows98OrGreater() \
_uint_is_windows_version_or_greater (0x0410)


/*
** Windows level global functions
*/

BOOL _uint_is_windows_version_or_greater (
    int version
);

char *_uint_exception_name (
    DWORD   exception_code
);

HINSTANCE _uint_get_current_instance (
    void
);

LRESULT _uint_send_message (
    HWND    window,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
);

int _uint_get_mousewheel_scroll_lines (
    void
);

int _uint_get_mousewheel_scroll_chars (
    void
);

int _uint_is_in_ui_window (
    HWND        window,
    ui_comp_t  *device
);

int _uint_is_ui_window (
    HWND    window
);

ui_comp_t _uint_comp_from_message (
    MSG    *message
);

int _uint_dlg_can_get_user_input (
    ui_comp_t   dialog
);

BOOL _uint_animate_window (
    HWND    window,
    DWORD   duration,
    DWORD   flags
);

int _uint_set_window_region (
    HWND    window,
    HRGN    region,
    BOOL    redraw
);

int _uint_get_window_region (
    HWND    window,
    HRGN    region
);

HWND _uint_get_app_window (
    void
);

int _uint_app_window_add_wndproc (
    UINT    message,
    WNDPROC wndproc
);

ATOM _uint_app_window_add_atom (
    TCHAR  *name
);

int _uint_app_window_atoms (
    ATOM  **atoms
);

int _uint_get_language_from_locale (
    HKL locale_id
);

HWND _uint_get_current_window (
    void
);

void _uint_register_wndproc (
    LONG_PTR    wndproc,
    ATOM        atom
);

WNDPROC _uint_get_wndproc_A (
    HWND    window
);

WNDPROC _uint_get_wndproc_W (
    HWND    window
);

#define _uint_get_wndproc_U(w) \
(IsWindowUnicode (w) ? \
 _uint_get_wndproc_W (w) : \
 _uint_get_wndproc_A (w))

void _uint_set_wndproc_A (
    HWND    window,
    WNDPROC wndproc
);

void _uint_set_wndproc_W (
    HWND    window,
    WNDPROC wndproc
);

#define _uint_set_wndproc_U(w,v) \
(IsWindowUnicode (w) ? \
 _uint_set_wndproc_W ((w), (v)) : \
 _uint_set_wndproc_A ((w), (v)))

TCHAR *_ui_wtounicode (
    TCHAR          *unicode,
    const wchar_t  *wstring,
    size_t          length,
    int             flags
);

wchar_t *_ui_unicodetow (
    wchar_t        *wstring,
    const TCHAR    *unicode
);

int _uint_d2d_initialize (
    void   *d2d_factory,
    void   *dwrite_factory
);

#define _uint_d2d_enabled() \
_uint_d2d_initialize (NULL, NULL)

#define _uint_d2d_factory(factory) \
_uint_d2d_initialize ((factory), NULL)

#define _uint_dwrite_factory(factory) \
_uint_d2d_initialize (NULL, (factory))

int _uint_d2d_renderer_create (
    HDC         device_context,
    const RECT *rect,
    int         add,
    void       *renderer
);

int _uint_d2d_renderer_create_nodraw (
    HDC         device_context,
    const RECT *rect,
    int         add,
    void       *renderer
);

int _uint_d2d_renderer_destroy (
    HDC device_context
);

void *_uint_d2d_renderer_region (
    HDC device_context
);

int _uint_d2d_renderer_clip (
    HDC     device_context,
    void   *region,
    int     mode
);

int _uint_d2d_renderer_unclip (
    HDC device_context,
    int clip
);

int _uint_d2d_renderer_set_origin (
    HDC device_context,
    int x,
    int y
);

#endif /* UI_SYSTEM_NT */

#endif /* UINT_H */
