/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    uint_init.c
|
|  Purpose: Windows NT level initialisation functions
|
|  History:
|
|  Date      Release Name  Ver.  Comments
|  --------- ------- ----- ----- --------------------------------------------
|  11-Apr-95 E-07-10 UK   $$1    Created
|  19-Jun-95         AW          Added necessary checks for UI_SYSTEM_NT
|  28-Jun-95         jas         Added hooks for mouse enter/leave messages
|  06-Jul-95         jas         Added no buffering to stdout and stderr
|  12-Jul-95         jas         Added preliminary 3D support
|  18-Jul-95         gcn         Option for external fn to return instances
|  20-Jul-95         jas         Changed implementation of CursorInWindow to
|                                use the PtInRect function
|  24-Jul-95         gcn         Conditional compilation of WinMain for Pro/E
|  25-Jul-95         AW          renamed DUI_INITIALIZE to UI_WINMAIN and
|                                reversed the sense of the tests
|  25-Jul-95 G-01-01 UK    $$2   Submit to new cut.
|  31-Oct-95         gcn         Added empty open display func
|  02-Nov-95 G-01-11 UK    $$3   Automatic Submission
|  23-Jan-96         jas         Added runtime 3D support
|  24-Jan-96         jas         Added atexit call to unhook the mouse hook
|  31-Jan-96         jas         Added Windows 95 runtime 3D support
|  07-Feb-96 G-03-02 UK    $$4   Automatic Submission
|  13-Feb-96         jas         Use new string code
|  19-Feb-96         pch         Remove the atexit() calls.
|  20-Feb-96         jas         Added mouse coordinates to enter / leave
|                                messages and removed CursorInWindow
|  21-Feb-96 G-03-03 UK    $$5   Automatic Submission
|  13-Mar-96         jas         3D behaviour on by default
|  19-Mar-96 G-03-06 UK    $$6   Automatic Submission
|  04-Jun-96         jas         Check for Windows NT with the new shell
|  05-Jun-96         jas         Added support for Windows NT 4.0
|  11-Jun-96 G-03-17 UK    $$7   Automatic Submission
|  09-Sep-96         jas         Added _uint_get_windows_system
|  11-Sep-96 H-01-08 UK    $$8   Automatic Submission
|  15-Jan-97         jas         Added CBT hook procedure
|  15-Jan-97         jas         Added exception handling code
|  16-Jan-97         jas         Check whether the class name is an atom
|                                before calling lstrcmp in _uint_cbt_hook_proc
|  16-Jan-97         jas         Fixed CBN_SETCOMBOLBOXWINDOW
|  21-Jan-97 H-01-24 UK    $$9   Automatic Submission
|  29-Apr-97         jas         Added Netscape support
|  06-May-97 H-03-09 UK    $$10  Automatic Submission
|  14-May-97 H-03-10 UK    $$11  Automatic Submission
|  31-Jul-97         gcn         Set cascade menu delay in open display
|  12-Aug-97 H-03-18 UK    $$12  Automatic Submission
|  16-Oct-97         jas         Moved mouse hook to uint_krn_act.c
|  16-Oct-97         jas         Moved cbt hook to uint_option.c
|  21-Oct-97 H-03-27 UK    $$13  Automatic Submission
|  16-Dec-97         jas         Added application window
|  17-Dec-97         jas         Create the app window in _uint_open_display
|  17-Dec-97         jas         Obsoleted UINT_3D
|  23-Dec-97 H-03-34 UK    $$14  Automatic Submission
|  31-Dec-97         pch         If we are in a stand-alone UI application,
|                                then check that we have a  valid
|                                current_instance when it is requested.
|   7-Jan-97         pch         Move the WinMain to another file
|  13-Jan-98 H-03-36 UK    $$15  Automatic Submission
|  21-Jan-98         AW    $$16  Only allow instance_func to be set once
|  28-Apr-98         pch         Incorporate above change into UK tree
|  05-May-98 I-01-06 UK    $$17  Automatic Submission
|  28-May-98         jas         Added _uint_unicode_enabled
|  01-Jun-98 I-01-10 UK    $$18  Automatic Submission
|  17-Sep-98         jas         Added sysdep_uninitialize
|  23-Sep-98 I-01-20 UK    $$19  Automatic Submission
|  08-Jan-99         jas         Use UINT_SYSTEM_NT instead of UINT_SYSTEM_95
|  20-Jan-99         jas         Updated against I-01-29
|  01-Feb-99 I-03-01 UK    $$20  Automatic Submission
|  08-Feb-99 I-03-02 UK    $$21  Automatic Submission
|  12-Feb-99         jas         Added OLE initialize and uninitialize calls
|  01-Mar-99 I-03-03 UK    $$22  Automatic Submission
|  19-May-99         jas         Fixed compilation warnings
|  20-May-99         jas         Added service pack detection
|  01-Jun-99 I-03-10 UK    $$23  Automatic Submission
|  09-Dec-99         jas         Added multiple monitor support
|  15-Dec-99 I-03-24 UK    $$24  Automatic Submission
|  17-Dec-99         jas         Added multiple monitor wrappers
|  17-Dec-99 I-03-24+UK    $$25  Patch Submission
|  06-Jan-00 I-03-26 UK    $$26  Automatic Submission
|  11-Jan-00         jas         Added Windows 2000 recognition
|  13-Jan-00 I-03-26+UK    $$27  Automatic Submission
|  22-Mar-00         jas         Added support for WM_INPUTLANGCHANGEREQUEST
|  24-Mar-00         jas         Restrict user language selection
|  29-Mar-00 J-01-05 UK    $$28  Automatic Submission
|  10-Jul-00         jas         Added Windows Me recognition
|  01-Aug-00 J-01-13 UK    $$29  Automatic Submission
|  17-Nov-00         jas         Removed UNICODE dependency
|  23-Jan-01         jas         Added Windows Whistler recognition
|  15-Feb-01         jas         Changed Whistler to Windows XP
|  12-Jun-01 J-03-01 UK    $$30  Automatic Submission
|  04-Jul-01         jas         Added GlobalFindAtom
|  12-Jul-01 J-03-03 UK    $$31  Automatic Submission
|  09-Oct-01         jas         Use pro_is_win95_running
|  18-Oct-01 J-03-10 UK    $$32  Automatic Submission
|  18-Oct-01         jas         Added RegisterClipboardFormat
|  19-Oct-01         jas         Removed _uint_get_windows_system
|  30-Oct-01 J-03-11 UK    $$33  Automatic Submission
|  11-Mar-02         jas         Allow unknown language input in USASCII
|  20-Mar-02 J-03-21 UK    $$34  Automatic Submission
|  02-Oct-02         jas         Added MapVirtualKeyEx
|  17-Oct-02 J-03-35 UK    $$35  Automatic Submission
|  27-Mar-03         jas         Return current instance in _uint_get_display
|  08-Apr-03 K-01-04 UK    $$36  Automatic Submission
|  26-Jun-03         jas         Obsoleted ui_memory.h
|  10-Jul-03 K-01-10 UK    $$37  Automatic Submission
|  28-Aug-03         jas         Added more European language support
|  10-Sep-03 K-01-14 UK    $$38  Automatic Submission
|  02-Oct-03         jas         Added Windows Server 2003 recognition
|  09-Oct-03 K-01-16 UK    $$39  Automatic Submission
|  06-Nov-03         jas         Fixed SGI compiler warnings
|  12-Nov-03         jas         Added UI_STATIC
|  18-Nov-03 K-01-18 UK    $$40  Automatic Submission
|  14-Jan-04         jas         Fixed return of DispatchMessage
|  20-Jan-04 K-01-22 UK    $$41  Automatic Submission
|  26-Feb-04         jas         Added _uint_app_window_add_atom
|  02-Mar-04 K-01-24 UK    $$42  Automatic Submission
|  12-May-04         jas         Added support for Slovak
|  25-May-04 K-03-02 UK    $$43  Automatic Submission
|  12-Jul-04         AW          Removed _uint_get_previous_instance
|  19-Jul-04         AW          Added _uint_set_instance_handle
|  20-Jul-04 K-03-06 UK    $$44  Automatic Submission
|  01-Sep-04         jas         Added GetWindowText and GetWindowTextLength
|  21-Sep-04 K-03-10 UK    $$45  Automatic Submission
|  22-Dec-04         jas         Include const.h and ctwcfun.h
|  11-Jan-05 K-03-17 UK    $$46  Automatic Submission
|  25-Jan-05         jas         Include ctwcfun_proto.h instead of ctwcfun.h
|  25-Jan-05 K-03-18 UK    $$47  Automatic Submission
|  21-Feb-05         jas         Moved lookup calls from uint_util.c
|  22-Feb-05         jas         Added _uint_get_proc_address
|  23-Feb-05         jas         Added SM_TABLETPC and SM_MEDIACENTER
|  01-Mar-05         jas         Added Windows XP x64 recognition
|  01-Mar-05 K-03-20 UK    $$48  Automatic Submission
|  09-Aug-05         jas         Added Windows Vista (TM) recognition
|  09-Aug-05 K-03-30 UK    $$49  Automatic Submission
|  11-Oct-05         jas         Removed _uint_unicode_enabled
|  11-Oct-05         jas         Removed pro_is_win95_running
|  11-Oct-05         jas         Removed pre-Windows 2000 code
|  27-Oct-05         jas         Added _uint_register_wndproc
|  31-Jan-06 L-01-01 UK    $$50  Automatic Submission
|  01-Feb-06         jas         Removed more pre-Windows 2000 code
|  14-Feb-06 L-01-02 UK    $$51  Automatic Submission
|  16-Feb-06         jas         Fixed use of NULL_CHAR
|  28-Feb-06 L-01-03 UK    $$52  Automatic Submission
|  06-Mar-06         jas         Added IsHungAppWindow
|  13-Mar-06         jas         Added _uint_IsWow64Process
|  13-Mar-06 L-01-04 UK    $$53  Automatic Submission
|  12-Jul-06 L-01-12 ksi   $$54  Unicode compliant changes
|  28-Jun-06         jas         Removed extern references
|  26-Jul-06 L-01-13 UK    $$55  Automatic Submission
|  13-Nov-06         jas         Added BufferedPaintInit
|  14-Nov-06 L-01-20 UK    $$56  Automatic Submission
|  30-Oct-07         jas         Added support for Unicode
|  13-Nov-07 L-01-41 UK    $$57  Automatic Submission
|  09-Feb-09         jas         Added Windows 7 recognition
|  17-Feb-09 L-03-26 UK    $$58  Automatic Submission
|  21-Apr-10         jas         Added DwmIsCompositionEnabled
|  27-Apr-10 L-05-21 UK    $$59  Automatic Submission
|  15-Jun-10         jas         Moved DwmIsCompositionEnabled to uint_theme.c
|  22-Jun-10 L-05-25 UK    $$60  Automatic Submission
|  25-Jan-11         jas         Added _ui_get_language
|  01-Feb-11 L-05-41 UK    $$61  Automatic Submission
|  14-Nov-11         jas         Added Windows 8 recognition
|  15-Nov-11 P-10-12 UK    $$62  Automatic Submission
|  25-Nov-11         jas         Wrap CreateCompatibleDC and DeleteDC
|  25-Nov-11         jas         Removed or replaced CS_OWNDC
|  29-Nov-11 P-10-13 UK    $$63  Automatic Submission
|  02-Mar-12         jas         Added ChangeWindowMessageFilterEx
|  06-Mar-12 P-10-17+UK    $$64  Automatic Submission
|  23-Feb-12         jas         Added UI_PRIVATE
|  20-Mar-12 P-20-01 UK    $$65  Automatic Submission
|  20-Jun-12         AW          Added _uint_wakeup_gui_thread
|  27-Jun-12 P-20-08 UK    $$66  Automatic Submission
|  14-Aug-12         jas         Use class atoms instead of names
|  14-Aug-12         jas         Added _uint_RegisterClass
|  21-Aug-12 P-20-12 UK    $$67  Automatic Submission
|  11-Jul-13         jas         Added SetProcessDPIAware
|  12-Jul-13         jas         Added _ui_display_scale
|  16-Jul-13 P-20-34 UK    $$68  Automatic Submission
|  01-Oct-13         jas         Added Windows 8.1 recognition
|  09-Oct-13 P-20-40 UK    $$69  Automatic Submission
|  17-Oct-13         jas         Added SetProcessDpiAwareness
|  22-Oct-13 P-20-41 UK    $$70  Automatic Submission
|  25-Oct-13         jas         Fixed GetProcessDpiAwareness
|  12-Nov-13 P-20-42 UK    $$71  Automatic Submission
|  13-Nov-13         jas         Added pro_get_win32_os_name
|  26-Nov-13 P-20-43 UK    $$72  Automatic Submission
|  11-Feb-14         jas         Added uGetVersionEx
|  19-Feb-14 P-20-48 UK    $$73  Automatic Submission
|  11-Jun-14         jas         Added atom to _uint_register_wndproc
|  26-Jun-14 P-20-55 UK    $$74  Automatic Submission
|  14-Jan-15         jas         Removed sysdep_wakeup_gui_thread
|  29-Jan-15         jas         Added UINT_VERSION_WINDOWS_... macros
|  03-Feb-15 P-30-01 UK    $$75  Automatic Submission
|  03-Feb-15         jas         Changed _uint_get_windows_version to an int
|  17-Feb-15 P-30-02 UK    $$76  Automatic Submission
|  02-Jul-15         jas         Added UINT_VERSION_WINDOWS_10
|  07-Jul-15 P-30-12 UK    $$77  Automatic Submission
|  16-Sep-15         jas         Changed version identifiers to match SDK
|  16-Sep-15         jas         Updated Windows 10 recognition post-release
|  30-Sep-15 P-30-17 UK    $$78  Automatic Submission
|  07-Jun-16         jas         Removed obsolete functions
|  13-Jun-16         jas         Added convenience macros
|  21-Jun-16 P-30-34 UK    $$79  Automatic Submission
|  26-Aug-16         jas         Added _ui_krn_global_modify
|  30-Aug-16 P-30-39 UK    $$80  Automatic Submission
|  27-Apr-17         jas         Removed BufferedPaintInit
|  03-May-17 P-50-07 UK    $$81  Automatic Submission
|  27-Sep-17         jas         Added _uint_is_windows_version_or_greater
|  10-Oct-17 P-50-31 UK    $$82  Automatic Submission
|  22-Mar-18         jas         Added _uint_app_window_atoms
|  24-Apr-18 P-60-01 UK    $$83  Automatic Submission
|  01-May-18         jas         Added nt_get_registry_value
|  01-May-18         jas         Added pro_get_system_info
|  02-May-18 P-60-02 UK    $$84  Automatic Submission
|  09-May-18         jas         Set the instance handle automatically
|  15-May-18 P-60-03 UK    $$85  Automatic Submission
|  24-Oct-18         jas         Removed uint_theme.h
|  29-Oct-18         jas         Added UI_system_dark_mode_Attr
|  30-Oct-18 P-60-23 UK    $$86  Automatic Submission
|  30-Oct-18         jas         Moved dark mode to color initialize
|  12-Nov-18 P-60-24 UK    $$87  Automatic Submission
|  12-Nov-18         jas         Removed _uint_set_instance_handle
|  19-Nov-18 P-60-25 UK    $$88  Automatic Submission
|  06-Dec-18         jas         Added sysdep_get_command_line_args
|  06-Dec-18         jas         Added sysdep_main_try_except
|  10-Dec-18 P-60-28 UK    $$89  Automatic Submission
|  25-Jan-19         jas         Added UI_allow_crash_Attr
|  25-Jan-19         jas         Added CommandLineToArgvW
|  19-Mar-19 P-70-01 UK    $$90  Automatic Submission
|  21-Mar-19         jas         NULL-terminate the array of command line args
|  25-Mar-19 P-70-02 UK    $$91  Automatic Submission
|  26-Jun-19         jas         Added SetProcessDpiAwarenessContext
|  02-Jul-19 P-70-16 UK    $$92  Automatic Submission
|  05-Jul-19         jas         Added DPI interfaces for scaling
|  08-Jul-19 P-70-17 UK    $$93  Automatic Submission
|  11-Sep-19         jas         Added SetWindowCompositionAttribute
|  18-Sep-19 P-70-27 UK    $$94  Automatic Submission
|  25-Jun-20         jas         Added const qualifiers
|  30-Jun-20 P-80-10 UK    $$95  Automatic Submission
|  01-Sep-20         jas         Fixed _ui_error_msg formatting errors
|  02-Sep-20 P-80-19 UK    $$96  Automatic Submission
|  26-Sep-22         jas         Added _uint_d2d_initialize
|  28-Sep-22 Q-10-29 UK    $$97  Automatic Submission
|  30-Sep-22         jas         Modified _uint_d2d_initialize
|  06-Oct-22 Q-10-30 UK    $$98  Automatic Submission
|  17-Nov-22         jas         Added UI_use_legacy_system_text_Attr
|  30-Nov-22 Q-10-37 UK    $$99  Automatic Submission
|  31-Mar-23         jas         Extended UI_use_legacy_system_text_Attr
|  12-Apr-23 Q-11-07 UK    $$100 Automatic Submission
|  19-May-23         jas         Added AdjustWindowRectExForDpi
|  23-May-23 Q-11-13 UK    $$101 Automatic Submission
|  26-Jun-23         jas         Added reset to _uint_d2d_initialize
|  27-Jun-23 Q-11-18 UK    $$102 Automatic Submission
|  01-Sep-23         jas         Added C++ throw exception handling
|  05-Sep-23 Q-11-28 UK    $$103 Automatic Submission
|  18-Sep-23         jas         Added GetDpiAwarenessContextForProcess
|  20-Sep-23 Q-11-30 UK    $$104 Automatic Submission
|  26-Oct-23         jas         Added PROCESSOR_ARCHITECTURE_ARM64
|  31-Oct-23 Q-11-36 UK    $$105 Automatic Submission
|  06-Nov-23         jas         Added _M_ARM64EC
|  08-Nov-23 Q-11-37 UK    $$106 Automatic Submission
|  22-Feb-24         jas         Changed _M_ARM64EC
|  27-Feb-24 Q-12-01 UK    $$107 Automatic Submission
|  20-May-24         jas         Added _uint_d2d_renderer_create/destroy
|  22-May-24 Q-12-13 UK    $$108 Automatic Submission
|  22-May-24         jas         Clip render target when possible
|  28-May-24 Q-12-14 UK    $$109 Automatic Submission
|  30-Apr-25         jas         Added _ui_string_qcr_enabled
|  07-May-25 Q-13-07 UK    $$110 Automatic Submission
|  10-Jul-25         jas         Fixed compilation warnings
|  15-Jul-25 Q-13-17 UK    $$111 Automatic Submission
|  26-Sep-25         jas         Added _uint_exception_name
|  30-Sep-25 Q-13-28 UK    $$112 Automatic Submission
|  10-Oct-25         jas         Added ID2D1BitmapRenderTarget
|  15-Oct-25 Q-13-30 UK    $$113 Automatic Submission
|  17-Oct-25         jas         Added _uint_d2d_renderer_set_origin
|  22-Oct-25 Q-13-31 UK    $$114 Automatic Submission
|  30-Oct-25         jas         Added _uint_d2d_renderer_region
|  04-Nov-25 Q-13-33 UK    $$115 Automatic Submission
|  12-Nov-25         jas         Added support for DWriteCore
|  12-Nov-25 Q-13-35 UK    $$116 Automatic Submission
|
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#if !defined (lint) && defined (SHOW_SCCS_ID)
static char uint_init_id [] = "@(#) uint_init.c 4135.1@(#)";
#endif

#include <const.h>
#include <btkcstdio.h>

#include <ui.h>
#include <uip.h>
#include <ui_app_res.h>
#include <ui_message.h>
#include <ui_string.h>
#include <ui_utils.h>

#ifdef UI_SYSTEM_NT

#define UINT_NO_WIN32_WRAP

#include <uint.h>
#include <uint_d2d1.h>
#include <uint_dwrite.h>

#include <bindcall.h>
#include <btkutfdef.h>
#include <ctwcfun_proto.h>
#include <languages.h>
#include <nt_registry.h>
#include <pro_memory.h>
#include <pro_stack_walk.h>
#include <pro_sys_info.h>
#include <ptc_win32.h>
#include <xarray.h>
#include <sysmath.h>


/*
** Global Windows version
*/

static int                              uint_version = 0;


/*
** Current instance handle
*/

static HINSTANCE                        instance_handle = (HINSTANCE) NULL;


/*
** Direct2D DC and render target data
*/

typedef struct
{
    HDC                         dc;
    RECT                        dc_rect;

    ID2D1DCRenderTarget        *dc_renderer;
    int                         dc_renderer_bound;
    int                         dc_renderer_drawing;

    ID2D1BitmapRenderTarget    *bitmap_renderer;
    void                       *bitmap_renderer_clip_region;
    ID2D1GeometryGroup         *bitmap_renderer_clip_group;
    ui_rect_t                   bitmap_renderer_bounds;

} nt_dcrt_t;


/*
** Application window data
*/

static HWND                             application_window = (HWND) NULL;

typedef struct
{
    UINT    message;
    WNDPROC wndproc;

} nt_app_window_wndproc_t;

static nt_app_window_wndproc_t         *app_window_wndprocs = (nt_app_window_wndproc_t *) NULL;

static ATOM                            *app_window_atoms = (ATOM *) NULL;


/*
** Application window functions
*/

static int _uint_create_app_window (
    void
);

static int _uint_destroy_app_window (
    void
);

static LRESULT CALLBACK _uint_app_window_wnd_proc (
    HWND    window,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
);


/*
** Cached DC wrapper data
*/

typedef struct
{
    HDC     dc;
    HBITMAP sys_bitmap;
    BOOL    in_use;

} nt_hdc_t;

static nt_hdc_t                         *cached_dcs = (nt_hdc_t *) NULL;


/*
** Cached DC wrapper functions
*/

static nt_hdc_t *_uint_create_cached_dc (
    HDC dc
);

static BOOL _uint_dc_caching_enabled (
    void
);


/*
** Private functions
*/

static int _uint_get_windows_version (
    OSVERSIONINFOEXA   *version_info
);

static void _uint_print_windows_version (
    OSVERSIONINFOEXA   *version_info
);

static LRESULT CALLBACK _uint_input_lang_wnd_proc (
    HWND    window,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
);

static FARPROC _uint_get_proc_address (
    CONST TCHAR    *module,
    CONST CHAR     *proc,
    BOOL            load
);

#endif /* UI_SYSTEM_NT */


/*--------------------------------------------------------------------------*\
| Function: _uint_get_command_line_args
| Purpose:  Get the command line arguments of the application
| Input:    argc        - number of command line arguments
|           argv        - command line arguments
| Output:   argc        - number of UTF-8 command line arguments
|           argv        - UTF-8 command line arguments
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_get_command_line_args (int *argc, char ***argv)
{
#ifdef UI_SYSTEM_NT

    int         i;
    LPWSTR     *wargv = CommandLineToArgvW (GetCommandLineW (), &i);
    char      **utf8_argv;

    INIT_ARG (argc, i);

    /*
    ** The C Standard guarantees that argv[argc] will be NULL
    */

    utf8_argv = GET_ARRAY (char *, (i + 1));

    for (i--; i >= 0; i--)
    {
        utf8_argv [i] = make_wstrtos ((wchar_t *) wargv [i]);
    }

    INIT_ARG (argv, utf8_argv);

    LocalFree (wargv);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_main_try_except
| Purpose:  Call the main function of the application with exceptin handling
| Input:    function    - the main function of the application
|           argc        - number of command line arguments
|           argv        - command line arguments
| Output:
| Return:   The result of the main function of the application
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_main_try_except (int (*function) (int, char **),
                                     int argc, char **argv)
{
#ifdef UI_SYSTEM_NT

    DWORD   code;
    int     status = 0;

    __try
    {
        status = (*function) (argc, argv);
    }
    __except ((VoidToInt) _ui_get_named_default (UI_allow_crash_Attr) ?
              EXCEPTION_CONTINUE_SEARCH :
              EXCEPTION_EXECUTE_HANDLER)
    {
        code = GetExceptionCode ();

        btk_fprintf (
            btk_get_stderr (),
            "Exception - code was %%x%x (%s)\n",
            code,
            _uint_exception_name (code));

        status = -1;
    }

    return (status);

#else

    return (-1);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_open_display
| Purpose:  Open the display
| Input:    argc            - pointer to the number of command line arguments
|           argv            - array of the command line arguments
|           appClassName    - the application class
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
UI_STATIC int _uint_open_display (int *argc, char *argv [],
                                  const char *appClassName)
{
#ifdef UI_SYSTEM_NT

    OSVERSIONINFOEXA    version_info = { sizeof (OSVERSIONINFOEXA) };
    int                 delay, language, sys_language;

    instance_handle = (HINSTANCE) GetModuleHandle ((TCHAR *) NULL);

    uint_version = _uint_get_windows_version (&version_info);

#if 0

    /*
    ** Do not run on anything prior to Windows 10
    */

    if (!(IsWindows10OrGreater ()))
    {
        fprintf (stderr, "This software is not supported on this version of Microsoft Windows\n");
        exit (1);
    }
    else

#endif

    {
        _uint_print_windows_version (&version_info);
    }

    _ui_dpi_scale (UI_DPI_DEFAULT, USER_DEFAULT_SCREEN_DPI);

    OleInitialize (NULL);

    _uint_create_app_window ();

    if ((delay = _ui_get_menu_show_delay ()) != 0)
    {
        _ui_krn_global_modify (UI_cascade_timer_Attr, delay, NULL);
    }

    /*
    ** Setup the input-locale
    */

    if (!(btkUnicodeIsEnabled ()))
    {
        language = _ui_get_language ();

        if ((sys_language = _ui_get_input_language ()) == UNKNOWN_LANG)
        {
            sys_language = _ui_get_system_input_language ();
        }

        if (!(is_generic_language (language)) &&
            (is_righttoleft_language (language) ||
             is_multibyte_language (sys_language)) &&
            sys_language != language)
        {
            sys_language = USASCII;
        }

        if (sys_language != _ui_get_system_input_language ())
        {
            _ui_set_system_input_language (sys_language);
        }
    }

    _ui_set_input_language (_ui_get_system_input_language ());

    _uint_app_window_add_wndproc (
        WM_INPUTLANGCHANGEREQUEST,
        _uint_input_lang_wnd_proc);

    _uint_app_window_add_wndproc (
        WM_INPUTLANGCHANGE,
        _uint_input_lang_wnd_proc);

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_initialize
| Purpose:  Initialize the application
| Input:    appClassName    - the application class
|           appName         - the application name
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
/*ARGSUSED*/
UI_STATIC int _uint_initialize (const char *appClassName, const char *appName)
{
#ifdef UI_SYSTEM_NT

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


/*--------------------------------------------------------------------------*\
| Function: _uint_uninitialize
| Purpose:  Uninitialize the application
| Input:
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
UI_STATIC int _uint_uninitialize (void)
{
#ifdef UI_SYSTEM_NT

    OleUninitialize ();

    _uint_destroy_app_window ();

    return (UI_SUCCESS);

#else

    return (UI_ERROR);

#endif /* UI_SYSTEM_NT */
}


#ifdef UI_SYSTEM_NT

/*--------------------------------------------------------------------------*\
| Function: _uint_get_windows_version
| Purpose:  Get the current Windows version number
| Input:
| Output:
| Return:   The Windows version number (e.g. 0x0610)
\*--------------------------------------------------------------------------*/
static int _uint_get_windows_version (OSVERSIONINFOEXA *version_info)
{
    /*
    ** Get the OS version
    */

    uGetVersionEx ((OSVERSIONINFOA *) version_info);

    /*
    ** Encode the OS version as a binary coded decimal to match the macros
    */

    return (
        ((version_info->dwMajorVersion << 8) |
         ((version_info->dwMinorVersion / 10) << 4) |
         (version_info->dwMinorVersion % 10)));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_print_windows_version
| Purpose:  Print the current Windows version
| Input:
| Output:
| Return:
\*--------------------------------------------------------------------------*/
static void _uint_print_windows_version (OSVERSIONINFOEXA *version_info)
{
    ProSystemInfo  *pro_info = pro_get_system_info ();
    const char     *text = pro_info->os_name;
    char            suffix [32] = { NULL_CHAR };
    char            text64 [32] = { NULL_CHAR };
    char            sp_text [32] = { NULL_CHAR };
    DWORD           csd_version = 0;
    int             sp_version = 0;
    double          win_version = 0.0;
    BOOL            is_wow64 = FALSE;
    SYSTEM_INFO     info;

    if (version_info->dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        /*
        ** Get the Version or the Service Pack number
        */

        if (IsWindows10OrGreater ())
        {
            btk_sprintf (sp_text, " (Version %s)", pro_info->os_version);
        }
        else if (nt_get_registry_value (
                     HKEY_LOCAL_MACHINE,
                     "System\\CurrentControlSet\\Control\\Windows",
                     "CSDVersion", &csd_version) &&
                 (sp_version = (LOWORD (csd_version) >> 8)) > 0)
        {
            btk_sprintf (sp_text, " (Service Pack %d)", sp_version);
        }

        if (IsWindowsXPOrGreater ())
        {
            /*
            ** Windows XP, Windows Vista, Windows 7, Windows 8, Windows 8.1,
            ** Windows 10, or Windows 11
            */

            if (IsWow64Process (GetCurrentProcess (), &is_wow64) &&
                is_wow64)
            {
                strcpy (text64, " 32-bit WOW64");
            }
            else
            {
                GetSystemInfo (&info);

                switch (info.wProcessorArchitecture)
                {
                    case PROCESSOR_ARCHITECTURE_AMD64:
                    case PROCESSOR_ARCHITECTURE_ARM64:
                    case PROCESSOR_ARCHITECTURE_IA64:
                    {
                        btk_sprintf (
                            text64,
                            " %s Edition",
                            ((info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ?
#if defined(_M_ARM64EC)
                             "ARM64 (x64 compatible)" :
#else
                             "x64" :
#endif /* _M_ARM64EC */
                             ((info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64) ?
                              "ARM64" :
                              "Itanium")));

                        break;
                    }
                }
            }

            if (!(IsWindowsVistaOrGreater ()))
            {
                /*
                ** Windows XP
                */

                if (version_info->wProductType == VER_NT_WORKSTATION)
                {
                    strcpy (suffix, " Professional");
                }

                if (text64 [0] == NULL_CHAR)
                {
                    if (GetSystemMetrics (SM_TABLETPC))
                    {
                        strcpy (text64, " Tablet PC Edition");
                    }
                    else if (GetSystemMetrics (SM_MEDIACENTER))
                    {
                        strcpy (text64, " Media Center Edition");
                    }
                    else if (GetSystemMetrics (SM_STARTER))
                    {
                        strcpy (text64, " Starter Edition");
                    }
                }
            }
        }
        else if (!(IsWindows2000OrGreater ()))
        {
            /*
            ** Windows NT 3.1, Windows NT 3.5, Windows NT 3.51
            ** or Windows NT 4.0
            */

            text = "Windows NT";

            win_version =
                (((version_info->dwMajorVersion * 100) +
                  version_info->dwMinorVersion) /
                 100.0);

            btk_sprintf (suffix, " Version %.2f", win_version);
        }
    }
    else
    {
        /*
        ** Windows 95, Windows 98 or Windows ME
        */

        text = "Windows ";

        if (IsWindowsMEOrGreater ())
        {
            /*
            ** Windows ME
            */

            strcpy (suffix, "Millennium Edition");
        }
        else if (IsWindows98OrGreater ())
        {
            /*
            ** Windows 98
            */

            strcpy (suffix, "98");
        }
        else
        {
            /*
            ** Windows 95
            */

            strcpy (suffix, "95");
        }
    }

    _ui_info_msg (UIMSG_UI, "Using %s%s%s%s", text, suffix, text64, sp_text);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_is_windows_version_or_greater
| Purpose:  Test the current Windows version number
| Input:    version     - the version to test
| Output:
| Return:   TRUE if Windows is at least the given version
\*--------------------------------------------------------------------------*/
BOOL _uint_is_windows_version_or_greater (int version)
{
    return (uint_version >= version);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_exception_name
| Purpose:  Get the name of an exception
| Input:    exception_code  - the exception code
| Output:
| Return:   The name of the exception
\*--------------------------------------------------------------------------*/
char *_uint_exception_name (DWORD exception_code)
{

#ifndef STATUS_POSSIBLE_DEADLOCK
#define STATUS_POSSIBLE_DEADLOCK        ((DWORD)0xC0000194L)
#endif /* STATUS_POSSIBLE_DEADLOCK */

#ifndef STATUS_PORT_DISCONNECTED
#define STATUS_PORT_DISCONNECTED        ((DWORD)0xC0000037L)
#endif /* STATUS_PORT_DISCONNECTED */

    typedef struct
    {
        DWORD   code;
        char   *name;

    } nt_exception_t;

    static nt_exception_t exceptions [] =
    {
        { EXCEPTION_ACCESS_VIOLATION,           "Access Violation" },
        { EXCEPTION_DATATYPE_MISALIGNMENT,      "Data Misalignment" },
//        { EXCEPTION_BREAKPOINT,                 "Breakpoint" },
//        { EXCEPTION_SINGLE_STEP,                "Single Step" },
        { EXCEPTION_ARRAY_BOUNDS_EXCEEDED,      "Array Bounds Exceeded" },
        { EXCEPTION_FLT_DENORMAL_OPERAND,       "FP Denormal Operand" },
        { EXCEPTION_FLT_DIVIDE_BY_ZERO,         "FP Divide by Zero" },
        { EXCEPTION_FLT_INEXACT_RESULT,         "FP Inexact Result" },
        { EXCEPTION_FLT_INVALID_OPERATION,      "FP Invalid Operation" },
        { EXCEPTION_FLT_OVERFLOW,               "FP Overflow" },
        { EXCEPTION_FLT_STACK_CHECK,            "FP Stack Check" },
        { EXCEPTION_FLT_UNDERFLOW,              "FP Underflow" },
        { EXCEPTION_INT_DIVIDE_BY_ZERO,         "Int Divide by Zero" },
        { EXCEPTION_INT_OVERFLOW,               "Int Overflow" },
        { EXCEPTION_PRIV_INSTRUCTION,           "Insufficient Privilege" },
        { EXCEPTION_IN_PAGE_ERROR,              "I/O Error in Paging" },
        { EXCEPTION_ILLEGAL_INSTRUCTION,        "Illegal Instruction" },
        { EXCEPTION_NONCONTINUABLE_EXCEPTION,   "Noncontinuable Exception" },
        { EXCEPTION_STACK_OVERFLOW,             "Stack Overflow" },
        { EXCEPTION_INVALID_DISPOSITION,        "Invalid Disposition" },
        { EXCEPTION_GUARD_PAGE,                 "Guard Page Violation" },
        { EXCEPTION_INVALID_HANDLE,             "Invalid Handle" },
        { EXCEPTION_POSSIBLE_DEADLOCK,          "Possible Deadlock" },
        { CONTROL_C_EXIT,                       "Control-C Exit" },
        { STATUS_NO_MEMORY,                     "Out of Memory" },
        { STATUS_INVALID_DISPOSITION,           "Invalid Disposition" },
        { STATUS_PORT_DISCONNECTED,             "Infinite Loop" },
        { 0xe06d7363,                           "C++ throw" },
        { 0,                                    "Unknown" }
    };

    int i;

    for (i = 0;
         i < NUM_ELEM_IN_ARR (exceptions) &&
         exceptions [i].code != exception_code &&
         exceptions [i].code != 0;
         i++);

    return (exceptions [i].name);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_current_instance
| Purpose:  Retrieves the handle of the current instance
| Input:
| Output:
| Return:   The handle of the current instance
\*--------------------------------------------------------------------------*/
HINSTANCE _uint_get_current_instance (void)
{
    return (instance_handle);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_app_window
| Purpose:  Get the handle of the application window
| Input:
| Output:
| Return:   The handle of the application window
\*--------------------------------------------------------------------------*/
HWND _uint_get_app_window (void)
{
    return (application_window);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_app_window_add_wndproc
| Purpose:  Add a window procedure to the application window
| Input:    message     - the message on which to call the window-procedure
|           wndproc     - the window-procedure to be called
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
int _uint_app_window_add_wndproc (UINT message, WNDPROC wndproc)
{
    nt_app_window_wndproc_t data;

    if (app_window_wndprocs == (nt_app_window_wndproc_t *) NULL)
    {
        app_window_wndprocs = XAR_BEGIN (nt_app_window_wndproc_t, 1);
    }

    data.message = message;
    data.wndproc = wndproc;

    XAR_APPEND (&app_window_wndprocs, 1, &data);

    return (UI_SUCCESS);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_app_window_add_atom
| Purpose:  Add an atom to the local atom table
| Input:    name        - the name of the atom to be added
| Output:
| Return:   The newly created atom
\*--------------------------------------------------------------------------*/
ATOM _uint_app_window_add_atom (TCHAR *name)
{
    TCHAR   new_name [256];
    char    suffix [16];
    ATOM    new_atom;
    int     length, i, count;

    if (app_window_atoms == (ATOM *) NULL)
    {
        app_window_atoms = XAR_BEGIN (ATOM, 1);
    }

    if ((new_atom = FindAtom (name)) != (ATOM) 0)
    {
        length = (NUM_ELEM_IN_ARR (new_name) - NUM_ELEM_IN_ARR (suffix));

        BYTCPY (new_name, name, (sizeof (TCHAR) * length));

        length = (int) lstrlen (new_name);

        for (count = 1; new_atom != (ATOM) 0; count++)
        {
            btk_sprintf (suffix, ":%d", count);

            for (i = 0; i < NUM_ELEM_IN_ARR (suffix); i++)
            {
                new_name [length + i] = (TCHAR) suffix [i];
            }

            new_atom = FindAtom (new_name);
        }

        name = new_name;
    }

    new_atom = AddAtom (name);

    XAR_APPEND (&app_window_atoms, 1, &new_atom);

    return (new_atom);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_app_window_add_atom
| Purpose:  Add an atom to the local atom table
| Input:    name        - the name of the atom to be added
| Output:
| Return:   The newly created atom
\*--------------------------------------------------------------------------*/
int _uint_app_window_atoms (ATOM **atoms)
{
    INIT_ARG (atoms, app_window_atoms);

    return (XAR_COUNT (&app_window_atoms));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_create_app_window
| Purpose:  Create the application window
| Input:
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_create_app_window (void)
{
    static ATOM app_window_class = 0;
    WNDCLASS    window_class;

    window_class.hInstance = _uint_get_current_instance ();

    if (app_window_class == (ATOM) 0)
    {
        window_class.style = CS_PARENTDC;
        window_class.lpfnWndProc = (WNDPROC) _uint_app_window_wnd_proc;
        window_class.cbClsExtra = 0;
        window_class.cbWndExtra  = 0;
        window_class.hIcon = (HICON) NULL;
        window_class.hCursor = (HCURSOR) NULL;
        window_class.hbrBackground = (HBRUSH) NULL;
        window_class.lpszMenuName = (TCHAR *) NULL;
        window_class.lpszClassName = TEXT ("Application");

        if ((app_window_class = RegisterClass (&window_class)) == 0)
        {
            _ui_error_msg (
                UIMSG_UI,
                "%s - failed to register Application class",
                "_uint_create_app_window");

            return (UI_ERROR);
        }
        else
        {
            _uint_register_wndproc (
                (LONG_PTR) _uint_app_window_wnd_proc,
                app_window_class);
        }
    }

    if (application_window == (HWND) NULL)
    {
        application_window =
            CreateWindow (
                MAKEINTATOM (app_window_class),
                (TCHAR *) NULL,
                WS_POPUP,
                0,
                0,
                0,
                0,
                (HWND) NULL,
                (HMENU) NULL,
                window_class.hInstance,
                NULL);

        if (application_window == (HWND) NULL)
        {
            _ui_error_msg (
                UIMSG_UI,
                "%s - failed to create application window",
                "_uint_create_app_window");

            return (UI_ERROR);
        }
    }

    return (UI_SUCCESS);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_destroy_app_window
| Purpose:  Destroy the application window
| Input:
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
static int _uint_destroy_app_window (void)
{
    int i;

    if (application_window != (HWND) NULL)
    {
        DestroyWindow (application_window);
    }

    if (app_window_atoms != (ATOM *) NULL)
    {
        for (i = (XAR_COUNT (&app_window_atoms) - 1); i >= 0; i--)
        {
            DeleteAtom (app_window_atoms [i]);
        }

        XAR_FREE (&app_window_atoms);
    }

    if (app_window_wndprocs != (nt_app_window_wndproc_t *) NULL)
    {
        XAR_FREE (&app_window_wndprocs);
    }

    return (UI_SUCCESS);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_app_window_wnd_proc
| Purpose:  Window Procedure for the application window
| Input:    window      - window handle of the application window
|           message     - message sent
|           wParam      - first parameter
|           lParam      - second parameter
| Output:
| Return:   The result of the window-procedure for the message
\*--------------------------------------------------------------------------*/
static LRESULT CALLBACK _uint_app_window_wnd_proc (HWND window, UINT message,
                                                   WPARAM wParam,
                                                   LPARAM lParam)
{
    int count, i;

    if (app_window_wndprocs != (nt_app_window_wndproc_t *) NULL)
    {
        count = XAR_COUNT (&app_window_wndprocs);

        for (i = 0; i < count; i++)
        {
            if (app_window_wndprocs [i].message == message)
            {
                return (
                    CallWindowProc (
                        app_window_wndprocs [i].wndproc,
                        window,
                        message,
                        wParam,
                        lParam));
            }
        }
    }

    return (DefWindowProc (window, message, wParam, lParam));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_input_lang_wnd_proc
| Purpose:  Window Procedure for the input language
| Input:    window      - window handle of the top-level window
|           message     - message sent
|           wParam      - first parameter
|           lParam      - second parameter
| Output:
| Return:   0 if the message was processed
\*--------------------------------------------------------------------------*/
static LRESULT CALLBACK _uint_input_lang_wnd_proc (HWND window, UINT message,
                                                   WPARAM wParam,
                                                   LPARAM lParam)
{
    int     language, new_language;
    LRESULT result;

    if (!(btkUnicodeIsEnabled ()))
    {
        new_language = _uint_get_language_from_locale ((HKL) lParam);

        switch (new_language)
        {
            case UNKNOWN_LANG:
            {
                if ((language = _ui_get_language ()) != USASCII &&
                    !(is_generic_language (language)))
                {
                    return (0);
                }

                break;
            }

            case USASCII:
            {
                break;
            }

            case RUSSIAN:
            case GREEK:
            case TURKISH:
            case CZECH:
            case POLISH:
            case HUNGARIAN:
            case SLOVENIAN:
            case SLOVAK:
            {
                if ((language = _ui_get_language ()) != new_language &&
                    !(is_generic_language (language)))
                {
                    return (0);
                }

                break;
            }

            default:
            {
                language = _ui_get_language ();

                if (!(is_generic_language (language)) &&
                    (is_righttoleft_language (new_language) ||
                     is_multibyte_language (new_language)) &&
                    new_language != language)
                {
                    return (0);
                }
            }
        }
    }

    result = DefWindowProc (window, message, wParam, lParam);

    _ui_set_input_language (_ui_get_system_input_language ());

    return (result);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_get_proc_address
| Purpose:  Get the address of the given procedure in the given module
| Input:    module      - the module containing the procedure
|           proc        - the name of the procedure
|           load        - flag indicating whether to attempt to load the
|                         module if it is not already loaded
| Output:
| Return:   The address of the procedure, or NULL if the procedure could not
|           be located
\*--------------------------------------------------------------------------*/
static FARPROC _uint_get_proc_address (CONST TCHAR *module, CONST CHAR *proc,
                                       BOOL load)
{
    HMODULE handle;
    FARPROC function;

    if (((handle = GetModuleHandle (module)) == (HMODULE) NULL &&
         (!load ||
          (handle = (HMODULE) LoadLibrary (module)) == (HMODULE) NULL)) ||
        handle == INVALID_HANDLE_VALUE ||
        (function = GetProcAddress (handle, proc)) == (FARPROC) NULL)
    {
        function = (FARPROC) NULL;
    }

    return (function);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_GetProcessDpiAwareness
| Purpose:  Wrapper for GetProcessDpiAwareness
| Input:    The arguments for GetProcessDpiAwareness
| Output:
| Return:   The result from GetProcessDpiAwareness
\*--------------------------------------------------------------------------*/
HRESULT WINAPI _uint_GetProcessDpiAwareness (HANDLE process,
                                             PROCESS_DPI_AWARENESS *state)
{
    static HRESULT (WINAPI *function) (HANDLE, PROCESS_DPI_AWARENESS *);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (HRESULT (WINAPI *) (HANDLE, PROCESS_DPI_AWARENESS *))
                _uint_get_proc_address (
                    TEXT ("SHCORE.DLL"),
                    "GetProcessDpiAwareness",
                    TRUE);
    }

    return (function ? (function) (process, state) : E_NOTIMPL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_SetProcessDpiAwareness
| Purpose:  Wrapper for SetProcessDpiAwareness
| Input:    The arguments for SetProcessDpiAwareness
| Output:
| Return:   The result from SetProcessDpiAwareness
\*--------------------------------------------------------------------------*/
HRESULT WINAPI _uint_SetProcessDpiAwareness (PROCESS_DPI_AWARENESS state)
{
    static HRESULT (WINAPI *function) (PROCESS_DPI_AWARENESS);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (HRESULT (WINAPI *) (PROCESS_DPI_AWARENESS))
                _uint_get_proc_address (
                    TEXT ("SHCORE.DLL"),
                    "SetProcessDpiAwareness",
                    FALSE);
    }

    return (function ? (function) (state) : E_NOTIMPL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_GetDpiAwarenessContextForProcess
| Purpose:  Wrapper for GetDpiAwarenessContextForProcess
| Input:    The arguments for GetDpiAwarenessContextForProcess
| Output:
| Return:   The result from GetDpiAwarenessContextForProcess
\*--------------------------------------------------------------------------*/
DPI_AWARENESS_CONTEXT WINAPI _uint_GetDpiAwarenessContextForProcess (HANDLE process)
{
    static DPI_AWARENESS_CONTEXT (WINAPI *function) (HANDLE);
    static BOOL             initialized = FALSE;
    PROCESS_DPI_AWARENESS   state;
    DPI_AWARENESS_CONTEXT   result = DPI_AWARENESS_CONTEXT_UNAWARE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (DPI_AWARENESS_CONTEXT (WINAPI *) (HANDLE))
                _uint_get_proc_address (
                    TEXT ("USER32.DLL"),
                    "GetDpiAwarenessContextForProcess",
                    FALSE);
    }

    if (function != (DPI_AWARENESS_CONTEXT (WINAPI *) (HANDLE)) NULL)
    {
        result = (function) (process);
    }
    else if (GetProcessDpiAwareness (process, &state) == S_OK)
    {
        switch (state)
        {
            case PROCESS_SYSTEM_DPI_AWARE:
            {
                result = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
                break;
            }

            case PROCESS_PER_MONITOR_DPI_AWARE:
            {
                result = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
                break;
            }
        }
    }

    return (result);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_SetProcessDpiAwarenessContext
| Purpose:  Wrapper for SetProcessDpiAwarenessContext
| Input:    The arguments for SetProcessDpiAwarenessContext
| Output:
| Return:   The result from SetProcessDpiAwarenessContext
\*--------------------------------------------------------------------------*/
BOOL WINAPI _uint_SetProcessDpiAwarenessContext (DPI_AWARENESS_CONTEXT value)
{
    static BOOL (WINAPI *function) (DPI_AWARENESS_CONTEXT);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (BOOL (WINAPI *) (DPI_AWARENESS_CONTEXT))
                _uint_get_proc_address (
                    TEXT ("USER32.DLL"),
                    "SetProcessDpiAwarenessContext",
                    FALSE);
    }

    return (function ? (function) (value) : FALSE);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_AreDpiAwarenessContextsEqual
| Purpose:  Wrapper for AreDpiAwarenessContextsEqual
| Input:    The arguments for AreDpiAwarenessContextsEqual
| Output:
| Return:   The result from AreDpiAwarenessContextsEqual
\*--------------------------------------------------------------------------*/
BOOL WINAPI _uint_AreDpiAwarenessContextsEqual (DPI_AWARENESS_CONTEXT v1, DPI_AWARENESS_CONTEXT v2)
{
    static BOOL (WINAPI *function) (DPI_AWARENESS_CONTEXT, DPI_AWARENESS_CONTEXT);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (BOOL (WINAPI *) (DPI_AWARENESS_CONTEXT, DPI_AWARENESS_CONTEXT))
                _uint_get_proc_address (
                    TEXT ("USER32.DLL"),
                    "AreDpiAwarenessContextsEqual",
                    FALSE);
    }

    return (function ? (function) (v1, v2) : (v1 == v2));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_GetDpiForMonitor
| Purpose:  Wrapper for GetDpiForMonitor
| Input:    The arguments for GetDpiForMonitor
| Output:
| Return:   The result from GetDpiForMonitor
\*--------------------------------------------------------------------------*/
HRESULT WINAPI _uint_GetDpiForMonitor (HMONITOR monitor,
                                       MONITOR_DPI_TYPE type, UINT *x,
                                       UINT *y)
{
    static HRESULT (WINAPI *function) (HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (HRESULT (WINAPI *) (HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *))
                _uint_get_proc_address (
                    TEXT ("SHCORE.DLL"),
                    "GetDpiForMonitor",
                    FALSE);
    }

    return (function ? (function) (monitor, type, x, y) : E_NOTIMPL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_GetScaleFactorForMonitor
| Purpose:  Wrapper for GetScaleFactorForMonitor
| Input:    The arguments for GetScaleFactorForMonitor
| Output:
| Return:   The result from GetScaleFactorForMonitor
\*--------------------------------------------------------------------------*/
HRESULT WINAPI _uint_GetScaleFactorForMonitor (HMONITOR monitor,
                                               DEVICE_SCALE_FACTOR *scale)
{
    static HRESULT (WINAPI *function) (HMONITOR, DEVICE_SCALE_FACTOR *);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (HRESULT (WINAPI *) (HMONITOR, DEVICE_SCALE_FACTOR *))
                _uint_get_proc_address (
                    TEXT ("SHCORE.DLL"),
                    "GetScaleFactorForMonitor",
                    FALSE);
    }

    return (function ? (function) (monitor, scale) : E_NOTIMPL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_AdjustWindowRectExForDpi
| Purpose:  Wrapper for AdjustWindowRectExForDpi
| Input:    The arguments for AdjustWindowRectExForDpi
| Output:
| Return:   The result from AdjustWindowRectExForDpi
\*--------------------------------------------------------------------------*/
BOOL WINAPI _uint_AdjustWindowRectExForDpi (RECT *rect, DWORD style,
                                            BOOL menu, DWORD ex_style,
                                            UINT dpi)
{
    static BOOL (WINAPI *function) (RECT *, DWORD, BOOL, DWORD, UINT);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (BOOL (WINAPI *) (RECT *, DWORD, BOOL, DWORD, UINT))
                _uint_get_proc_address (
                    TEXT ("USER32.DLL"),
                    "AdjustWindowRectExForDpi",
                    FALSE);
    }

    return (function ? (function) (rect, style, menu, ex_style, dpi) : FALSE);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_SetWindowCompositionAttribute
| Purpose:  Wrapper for SetWindowCompositionAttribute
| Input:    The arguments for SetWindowCompositionAttribute
| Output:
| Return:   The result from SetWindowCompositionAttribute
\*--------------------------------------------------------------------------*/
BOOL WINAPI _uint_SetWindowCompositionAttribute (HWND window,
                                                 WINDOWCOMPOSITIONATTRIBUTE *attribute)
{
    static BOOL (WINAPI *function) (HWND, WINDOWCOMPOSITIONATTRIBUTE *);
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        initialized = TRUE;

        function =
            (BOOL (WINAPI *) (HWND, WINDOWCOMPOSITIONATTRIBUTE *))
                _uint_get_proc_address (
                    TEXT ("USER32.DLL"),
                    "SetWindowCompositionAttribute",
                    FALSE);
    }

    return (function ?
            ((window != (HWND) NULL &&
              attribute != (WINDOWCOMPOSITIONATTRIBUTE *) NULL) ?
             (function) (window, attribute) :
             TRUE) :
            FALSE);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_CreateCompatibleDC
| Purpose:  Wrapper for CreateCompatibleDC
| Input:    The arguments for CreateCompatibleDC
| Output:
| Return:   The result from CreateCompatibleDC
\*--------------------------------------------------------------------------*/
HDC WINAPI _uint_CreateCompatibleDC (HDC dc)
{
    int i;

    if (_uint_dc_caching_enabled ())
    {
        if (cached_dcs == (nt_hdc_t *) NULL)
        {
            cached_dcs = XAR_BEGIN (nt_hdc_t, 16);
        }

        for (i = (XAR_COUNT (&cached_dcs) - 1);
             i >= 0 && cached_dcs [i].in_use;
             i--);

        if (i < 0)
        {
            i = XAR_COUNT (&cached_dcs);

            XAR_APPEND (&cached_dcs, 1, _uint_create_cached_dc (dc));

/*
            _ui_info_msg (UIMSG_UI, "Caching HDC %08x", cached_dcs [i].dc);
*/
        }

        cached_dcs [i].in_use = TRUE;

        dc = cached_dcs [i].dc;
    }
    else
    {
        dc = CreateCompatibleDC (dc);
    }

    return (dc);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_DeleteDC
| Purpose:  Wrapper for DeleteDC
| Input:    The arguments for DeleteDC
| Output:
| Return:   The result from DeleteDC
\*--------------------------------------------------------------------------*/
BOOL WINAPI _uint_DeleteDC (HDC dc)
{
    int     i = -1;
    BOOL    status;

    if (_uint_dc_caching_enabled ())
    {
        for (i = (XAR_COUNT (&cached_dcs) - 1);
             i >= 0 && cached_dcs [i].dc != dc;
             i--);
    }

    if (i >= 0)
    {
        SelectBitmap (dc, cached_dcs [i].sys_bitmap);

        cached_dcs [i].in_use = FALSE;

        status = TRUE;
    }
    else
    {
        status = DeleteDC (dc);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_create_cached_dc
| Purpose:  Create a new cached DC compatible with the given device context
| Input:    dc          - the device context
| Output:
| Return:   A pointer to the new cached DC
\*--------------------------------------------------------------------------*/
static nt_hdc_t *_uint_create_cached_dc (HDC dc)
{
    static nt_hdc_t new_dc = { 0 };
    HBITMAP         bitmap;

    new_dc.dc = CreateCompatibleDC (dc);

    bitmap = CreateCompatibleBitmap (dc, 16, 16);
    new_dc.sys_bitmap = SelectBitmap (new_dc.dc, bitmap);
    SelectBitmap (new_dc.dc, new_dc.sys_bitmap);
    DeleteBitmap (bitmap);

    return (&new_dc);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_dc_caching_enabled
| Purpose:  Determine whether DC caching is enabled
| Input:
| Output:
| Return:   TRUE if DC caching is enabled, otherwise FALSE
\*--------------------------------------------------------------------------*/
static BOOL _uint_dc_caching_enabled (void)
{
    static int      enabled = UI_ERROR;
    char           *env;

    if (enabled == UI_ERROR)
    {
        /*
        ** Always use DC caching unless the user specifies otherwise
        */

        enabled =
            ((env = _ui_getenv ("UINT_DC_CACHING")) == (char *) NULL ||
             (*env != 'f' &&
              *env != 'F'));

        if (enabled)
        {
            _ui_info_msg (UIMSG_UI, "Using DC caching for compatible contexts");
        }
    }

    return (enabled);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_factory_create
| Purpose:  Get the factory for Direct2D
| Input:
| Output:   factory     - the ID2D1Factory
| Return:   TRUE on success
\*--------------------------------------------------------------------------*/
static int _uint_d2d_factory_create (ID2D1Factory **factory)
{
    typedef HRESULT (WINAPI *PD2D1CREATEFACTORY) (
        D2D1_FACTORY_TYPE,
        REFIID,
        const D2D1_FACTORY_OPTIONS *,
        void **
    );

    static PD2D1CREATEFACTORY   pD2D1CreateFactory =
        (PD2D1CREATEFACTORY) NULL;

    return ((pD2D1CreateFactory =
             (PD2D1CREATEFACTORY)
                 _uint_get_proc_address (
                     TEXT ("d2d1.dll"), "D2D1CreateFactory", TRUE))
                != (PD2D1CREATEFACTORY) NULL &&
            (*pD2D1CreateFactory) (
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &IID_ID2D1Factory,
                (D2D1_FACTORY_OPTIONS *) NULL, (void **) factory) == S_OK &&
            *factory != (ID2D1Factory *) NULL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_dwrite_factory_create
| Purpose:  Get the factory for DirectWrite
| Input:
| Output:   factory     - the IDWriteFactory
| Return:   TRUE on success
\*--------------------------------------------------------------------------*/
static int _uint_dwrite_factory_create (IDWriteFactory **factory)
{
    typedef HRESULT (WINAPI *PDWRITECREATEFACTORY) (
        DWRITE_FACTORY_TYPE,
        REFIID,
        void **
    );

    static PDWRITECREATEFACTORY pDWriteCreateFactory =
        (PDWRITECREATEFACTORY) NULL;

    static CONST IID            IID_IDWriteFactory =
        { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };

    return ((pDWriteCreateFactory =
             (PDWRITECREATEFACTORY)
                 _uint_get_proc_address (
                     TEXT ("dwrite.dll"), "DWriteCreateFactory", TRUE))
                != (PDWRITECREATEFACTORY) NULL &&
            (*pDWriteCreateFactory) (
                DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
                (void **) factory) == S_OK &&
            *factory != (IDWriteFactory *) NULL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_dwrite_core_factory_create
| Purpose:  Get the factory for DirectWrite using DWriteCore
| Input:
| Output:   factory     - the IDWriteFactory using DWriteCore
| Return:   TRUE on success
\*--------------------------------------------------------------------------*/
static int _uint_dwrite_core_factory_create (IDWriteFactory **factory)
{
    typedef HRESULT (WINAPI *PDWRITECORECREATEFACTORY) (
        DWRITE_FACTORY_TYPE,
        REFIID,
        IUnknown **
    );

    static PDWRITECORECREATEFACTORY     pDWriteCoreCreateFactory =
        (PDWRITECORECREATEFACTORY) NULL;

    static CONST IID                    IID_IDWriteFactory =
        { 0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 } };

    return ((pDWriteCoreCreateFactory =
             (PDWRITECORECREATEFACTORY)
                 _uint_get_proc_address (
                     TEXT ("DWriteCore.dll"), "DWriteCoreCreateFactory",
                     TRUE)) != (PDWRITECORECREATEFACTORY) NULL &&
            (*pDWriteCoreCreateFactory) (
                DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
                (IUnknown **) factory) == S_OK &&
            *factory != (IDWriteFactory *) NULL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_initialize
| Purpose:  Initialize support for Direct2D and DirectWrite
| Input:
| Output:   d2d         - the ID2D1Factory
|           dwrite      - the IDWriteFactory
| Return:   The rendering mode for Direct2D and DirectWrite
\*--------------------------------------------------------------------------*/
int _uint_d2d_initialize (void *v_d2d_factory, void *v_dwrite_factory)
{
    static ID2D1Factory    *d2d_factory = (ID2D1Factory *) NULL;
    static IDWriteFactory  *dwrite_factory = (IDWriteFactory *) NULL;
    static int              default_mode = DWRITE_RENDERING_MODE_GDI_CLASSIC;
    static int              enabled = UI_ERROR;
    char                   *env;
    int                     status;

    if (enabled == UI_ERROR)
    {
        if (_ui_string_qcr_enabled ())
        {
            enabled = FALSE;
        }
        else if ((env = _ui_getenv ("UINT_DIRECT2D_ENABLED")) == (char *) NULL)
        {
            enabled =
                (VoidToInt)
                    _ui_get_named_default (UI_use_legacy_system_text_Attr);

            if (enabled > 0)
            {
                enabled = FALSE;
            }
            else if (enabled == 0)
            {
                enabled = default_mode;
            }
            else
            {
                enabled = -enabled;
            }
        }
        else if (!u_strcmp (env, "natural"))
        {
            enabled = DWRITE_RENDERING_MODE_NATURAL;
        }
        else if (!u_strcmp (env, "gdinatural"))
        {
            enabled = DWRITE_RENDERING_MODE_GDI_NATURAL;
        }
        else if (!u_strcmp (env, "gdiclassic"))
        {
            enabled = DWRITE_RENDERING_MODE_GDI_CLASSIC;
        }
        else if (*env == 'f' ||
                 *env == 'F')
        {
            enabled = FALSE;
        }
        else
        {
            enabled = default_mode;
        }

        if (!enabled)
        {
            enabled = FALSE;
        }
        else if (_uint_dwrite_core_factory_create (&dwrite_factory))
        {
            /*
            ** Use DWriteCore in preference when available
            */

            d2d_factory = (ID2D1Factory *) NULL;
        }
        else if (!(_uint_d2d_factory_create (&d2d_factory)))
        {
            d2d_factory = (ID2D1Factory *) NULL;

            enabled = FALSE;
        }
        else if (!(_uint_dwrite_factory_create (&dwrite_factory)))
        {
            dwrite_factory = (IDWriteFactory *) NULL;

            ID2D1Factory_Release (d2d_factory);

            d2d_factory = (ID2D1Factory *) NULL;

            enabled = FALSE;
        }

        _ui_info_msg (
            UIMSG_UI,
            "Using %s for %s text rendering",
            (enabled ?
             ((d2d_factory == (ID2D1Factory *) NULL) ?
              "DWriteCore" :
              "Direct2D and DirectWrite") :
             "Windows GDI"),
            ((enabled == DWRITE_RENDERING_MODE_NATURAL ||
              enabled == DWRITE_RENDERING_MODE_GDI_NATURAL) ?
             "natural" :
             "classic"));
    }

    if ((status = enabled) != FALSE)
    {
        if (v_d2d_factory != NULL &&
            (*((ID2D1Factory **) v_d2d_factory) =
             d2d_factory) == (ID2D1Factory *) NULL)
        {
            status = FALSE;
        }

        if (v_dwrite_factory != NULL)
        {
            *((IDWriteFactory **) v_dwrite_factory) = dwrite_factory;
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_DC_renderer_create
| Purpose:  Create a DC render target for a device context
| Input:    device_context  - the device context
|           rect            - the rectangle of the renderer
|           bind            - TRUE to bind the renderer to the device context
| Output:   renderer        - the DC render target
| Return:   TRUE if a render target was created
\*--------------------------------------------------------------------------*/
static int _uint_d2d_DC_renderer_create (ID2D1DCRenderTarget **renderer,
                                         HDC device_context, const RECT *rect,
                                         int bind)
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
    HBITMAP                                 bitmap;
    BITMAP                                  bitmap_data;
    RECT                                    bitmap_rect;
    POINT                                   bitmap_origin;
    ID2D1DCRenderTarget                    *dc_renderer;
    int                                     status = FALSE;

    if (device_context == (HDC) NULL ||
        !(_uint_d2d_factory (&factory)) ||
        ID2D1Factory_CreateDCRenderTarget (
            factory, &properties, &dc_renderer) != S_OK ||
        dc_renderer == (ID2D1DCRenderTarget *) NULL)
    {
        dc_renderer = (ID2D1DCRenderTarget *) NULL;
    }
    else
    {
        if (bind &&
            (bitmap =
             GetCurrentObject (device_context, OBJ_BITMAP))
                != (HBITMAP) NULL &&
            GetObject (bitmap, sizeof (BITMAP), &bitmap_data) > 0 &&
            bitmap_data.bmWidth > 0L &&
            bitmap_data.bmHeight > 0L &&
            GetWindowOrgEx (device_context, &bitmap_origin) &&
            SetRect (
                &bitmap_rect, bitmap_origin.x, bitmap_origin.y,
                (bitmap_origin.x + (int) bitmap_data.bmWidth),
                (bitmap_origin.y + (int) bitmap_data.bmHeight)) &&
            (bitmap_rect.right < rect->right ||
             bitmap_rect.bottom < rect->bottom))
        {
            if ((bitmap_rect.left = rect->left) > bitmap_rect.right)
            {
                bitmap_rect.right = bitmap_rect.left;
            }
            else if (bitmap_rect.right >= rect->right)
            {
                bitmap_rect.right = rect->right;
            }

            if ((bitmap_rect.top = rect->top) > bitmap_rect.bottom)
            {
                bitmap_rect.bottom = bitmap_rect.top;
            }
            else if (bitmap_rect.bottom >= rect->bottom)
            {
                bitmap_rect.bottom = rect->bottom;
            }

            rect = &bitmap_rect;
        }

        if (bind &&
            ID2D1DCRenderTarget_BindDC (
                dc_renderer, device_context, rect) != S_OK)
        {
            ID2D1DCRenderTarget_Release (dc_renderer);

            dc_renderer = (ID2D1DCRenderTarget *) NULL;
        }
        else
        {
            ID2D1DCRenderTarget_SetAntialiasMode (
                dc_renderer,
                D2D1_ANTIALIAS_MODE_ALIASED);

            *renderer = dc_renderer;

            status = TRUE;
        }
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_DC_renderer_destroy
| Purpose:  Destroy a DC render target
| Input:    renderer        - the DC render target
|           device_context  - the device context
| Output:
| Return:   TRUE if the render target was destroyed
\*--------------------------------------------------------------------------*/
static int _uint_d2d_DC_renderer_destroy (ID2D1DCRenderTarget *renderer)
{
    int status = FALSE;

    if (renderer != (ID2D1DCRenderTarget *) NULL)
    {
        ID2D1DCRenderTarget_Release (renderer);

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_bitmap_renderer_create
| Purpose:  Create a bitmap render target for a device context
| Input:    dc_renderer     - the DC render target
|           rect            - the rectangle of the renderer
|           draw            - TRUE to copy the bitmap into the device context
|                             on destroy
| Output:   renderer        - the bitmap render target
| Return:   TRUE if a render target was created
\*--------------------------------------------------------------------------*/
static int _uint_d2d_bitmap_renderer_create (ID2D1BitmapRenderTarget **renderer,
                                             ID2D1DCRenderTarget *dc_renderer,
                                             const RECT *rect, int draw)
{
    D2D1_SIZE_F                 size;
    D2D1_SIZE_U                 pixel_size;
    D2D1_PIXEL_FORMAT           pixel_format =
    {
        DXGI_FORMAT_B8G8R8A8_UNORM,
        D2D1_ALPHA_MODE_PREMULTIPLIED
    };
    D2D1_COLOR_F                transparent_black =
        { 0.0f, 0.0f, 0.0f, 0.0f };
    ID2D1BitmapRenderTarget    *bitmap_renderer =
        (ID2D1BitmapRenderTarget *) NULL;
    int                         status = FALSE;

    if (draw)
    {
        ID2D1DCRenderTarget_GetSize (dc_renderer, &size);
        ID2D1DCRenderTarget_GetPixelSize (dc_renderer, &pixel_size);
        ID2D1DCRenderTarget_GetPixelFormat (dc_renderer, &pixel_format);
    }
    else
    {
        pixel_size.width = (rect->right - rect->left);
        pixel_size.height = (rect->bottom - rect->top);

        size.width = (FLOAT) pixel_size.width;
        size.height = (FLOAT) pixel_size.height;
    }

    if (ID2D1DCRenderTarget_CreateCompatibleRenderTarget (
            dc_renderer, &size, &pixel_size, &pixel_format,
            D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE,
            &bitmap_renderer) == S_OK)
    {
        ID2D1BitmapRenderTarget_SetAntialiasMode (
            bitmap_renderer,
            D2D1_ANTIALIAS_MODE_ALIASED);

        ID2D1BitmapRenderTarget_BeginDraw (bitmap_renderer);

        ID2D1BitmapRenderTarget_Clear (bitmap_renderer, &transparent_black);

        *renderer = bitmap_renderer;

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_bitmap_renderer_destroy
| Purpose:  Destroy a bitmap render target
| Input:    renderer        - the bitmap render target
|           dc_renderer     - the DC render target
|           rect            - the rectangle of the renderer
|           draw            - TRUE to copy the bitmap into the device context
|                             on destroy
| Output:
| Return:   TRUE if the render target was destroyed
\*--------------------------------------------------------------------------*/
static int _uint_d2d_bitmap_renderer_destroy (ID2D1BitmapRenderTarget *renderer,
                                              ID2D1DCRenderTarget *dc_renderer,
                                              const RECT *rect, int draw)
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
    HRESULT                         result;
    ID2D1Bitmap                    *bitmap = (ID2D1Bitmap *) NULL;
    ID2D1Bitmap                    *shared_bitmap = (ID2D1Bitmap *) NULL;
    D2D1_RECT_F                     bitmap_rect;
    int                             status = FALSE;

    if (renderer != (ID2D1BitmapRenderTarget *) NULL &&
        dc_renderer != (ID2D1DCRenderTarget *) NULL)
    {
        result =
            ID2D1BitmapRenderTarget_EndDraw (
                renderer,
                (D2D1_TAG *) NULL,
                (D2D1_TAG *) NULL);

        if (draw &&
            ID2D1BitmapRenderTarget_GetBitmap (renderer, &bitmap)
                == S_OK &&
            bitmap != (ID2D1Bitmap *) NULL)
        {
            if (ID2D1DCRenderTarget_CreateSharedBitmap (
                    dc_renderer, &IID_ID2D1Bitmap, bitmap, &bitmap_properties,
                    &shared_bitmap) == S_OK &&
                shared_bitmap != (ID2D1Bitmap *) NULL)
            {
                bitmap_rect.left = (FLOAT) rect->left;
                bitmap_rect.top = (FLOAT) rect->top;
                bitmap_rect.right = (FLOAT) rect->right;
                bitmap_rect.bottom = (FLOAT) rect->bottom;

                ID2D1DCRenderTarget_BeginDraw (dc_renderer);

                ID2D1DCRenderTarget_DrawBitmap (
                    dc_renderer,
                    shared_bitmap,
                    &bitmap_rect,
                    1.0f,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                    (D2D1_RECT_F *) NULL);

                result =
                    ID2D1DCRenderTarget_EndDraw (
                        dc_renderer,
                        (D2D1_TAG *) NULL,
                        (D2D1_TAG *) NULL);

                if (result == S_OK)
                {
                }
                else if (result == D2DERR_RECREATE_TARGET)
                {
                }

                ID2D1Bitmap_Release (shared_bitmap);
            }

            ID2D1Bitmap_Release (bitmap);
        }

        ID2D1BitmapRenderTarget_Release (renderer);

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_add_remove
| Purpose:  Add or remove the renderer for a device context
| Input:    dc              - the device context
|           rect            - the rectangle
|           add             - TRUE to add a new renderer if one does not exist
|           remove          - TRUE to remove a renderer if one exists
|           draw            - TRUE to allow drawing in the DC render target
|           bitmap          - TRUE to use an extra bitmap render target
| Output:   dcrt            - the corresponding renderer
| Return:   TRUE if a renderer was found
\*--------------------------------------------------------------------------*/
static int _uint_d2d_renderer_add_remove (HDC dc, const RECT *rect,
                                          int add, int remove, int draw,
                                          int bitmap, nt_dcrt_t **dcrt)
{
    static nt_dcrt_t   *dcrts = (nt_dcrt_t *) NULL;
    static nt_dcrt_t    new_dcrt;
    int                 i;
    HRESULT             result;
    int                 status = FALSE;

    /*
    ** Look for for the device context
    */

    for (i = (XAR_COUNT (&dcrts) - 1);
         i >= 0 && dcrts [i].dc != dc;
         i--);

    if (i < 0)
    {
        /*
        ** The device context could not be found, so add a new one if required
        */

        if (add &&
            _uint_d2d_DC_renderer_create (
                &(new_dcrt.dc_renderer), dc, rect, draw))
        {
            new_dcrt.dc = dc;
            new_dcrt.dc_rect = *rect;

            new_dcrt.dc_renderer_bound = draw;

            if (bitmap &&
                _uint_d2d_bitmap_renderer_create (
                    &(new_dcrt.bitmap_renderer), new_dcrt.dc_renderer,
                    &(new_dcrt.dc_rect), new_dcrt.dc_renderer_bound))
            {
                new_dcrt.dc_renderer_drawing = FALSE;
            }
            else
            {
                new_dcrt.bitmap_renderer = (ID2D1BitmapRenderTarget *) NULL;

                ID2D1DCRenderTarget_BeginDraw (new_dcrt.dc_renderer);

                new_dcrt.dc_renderer_drawing = TRUE;
            }

            new_dcrt.bitmap_renderer_bounds.x =
                new_dcrt.bitmap_renderer_bounds.y = 0;

            new_dcrt.bitmap_renderer_bounds.width =
                (new_dcrt.dc_rect.right - new_dcrt.dc_rect.left);

            new_dcrt.bitmap_renderer_bounds.height =
                (new_dcrt.dc_rect.bottom - new_dcrt.dc_rect.top);

            new_dcrt.bitmap_renderer_clip_region = NULL;
            new_dcrt.bitmap_renderer_clip_group = (ID2D1GeometryGroup *) NULL;

            xar_alloc_or_append (
                &dcrts,
                1,
                sizeof (nt_dcrt_t),
                16,
                &new_dcrt);

            INIT_ARG (dcrt, (nt_dcrt_t *) xar_last (&dcrts));

            status = TRUE;
        }
    }
    else if (remove)
    {
        /*
        ** Remove the renderer from the device context
        */

        if (dcrts [i].bitmap_renderer != (ID2D1BitmapRenderTarget *) NULL)
        {
            _uint_d2d_bitmap_renderer_destroy (
                dcrts [i].bitmap_renderer,
                dcrts [i].dc_renderer,
                &(dcrts [i].dc_rect),
                dcrts [i].dc_renderer_bound);
        }
        else if (dcrts [i].dc_renderer_drawing)
        {
            result =
                ID2D1DCRenderTarget_EndDraw (
                    dcrts [i].dc_renderer,
                    (D2D1_TAG *) NULL,
                    (D2D1_TAG *) NULL);

            if (result == S_OK)
            {
            }
            else if (result == D2DERR_RECREATE_TARGET)
            {
            }
        }

        _uint_d2d_DC_renderer_destroy (dcrts [i].dc_renderer);

        XAR_REMOVE (&dcrts, i, 1);

        status = TRUE;
    }
    else
    {
        /*
        ** Return the renderer
        */

        INIT_ARG (dcrt, &(dcrts [i]));

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_create
| Purpose:  Get the renderer for a device context
| Input:    dc              - the device context, origin and rectangle
|           add             - TRUE to add a new renderer if one does not exist
| Output:   renderer        - the corresponding renderer, origin and rectangle
| Return:   TRUE if a renderer was found or created
\*--------------------------------------------------------------------------*/
int _uint_d2d_renderer_create (HDC device_context, const RECT *rect,
                               int add, void *renderer)
{
    nt_dcrt_t  *dcrt;
    int         status =
        _uint_d2d_renderer_add_remove (
            device_context, rect, add, FALSE, TRUE, FALSE, &dcrt);

    if (status &&
        renderer != NULL)
    {
        *((ID2D1RenderTarget **) renderer) =
            ((dcrt->bitmap_renderer != (ID2D1BitmapRenderTarget *) NULL) ?
             (ID2D1RenderTarget *) dcrt->bitmap_renderer :
             (ID2D1RenderTarget *) dcrt->dc_renderer);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_create_nodraw
| Purpose:  Get the renderer for a device context
| Input:    dc              - the device context, origin and rectangle
|           add             - TRUE to add a new renderer if one does not exist
| Output:   renderer        - the corresponding renderer, origin and rectangle
| Return:   TRUE if a renderer was found or created
\*--------------------------------------------------------------------------*/
int _uint_d2d_renderer_create_nodraw (HDC device_context, const RECT *rect,
                                      int add, void *renderer)
{
    nt_dcrt_t  *dcrt;
    int         status =
        _uint_d2d_renderer_add_remove (
            device_context, rect, add, FALSE, FALSE, TRUE, &dcrt);

    if (status &&
        renderer != NULL)
    {
        *((ID2D1RenderTarget **) renderer) =
            ((dcrt->bitmap_renderer != (ID2D1BitmapRenderTarget *) NULL) ?
             (ID2D1RenderTarget *) dcrt->bitmap_renderer :
             (ID2D1RenderTarget *) dcrt->dc_renderer);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_destroy
| Purpose:  Remove the renderer for a device context
| Input:    device_context  - the device context
| Output:
| Return:   TRUE if a renderer was destroyed
\*--------------------------------------------------------------------------*/
int _uint_d2d_renderer_destroy (HDC device_context)
{
    return (_uint_d2d_renderer_add_remove (
                device_context, (RECT *) NULL, FALSE, TRUE, FALSE, TRUE,
                (nt_dcrt_t **) NULL));
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_region_create
| Purpose:  Create a region for Direct2D
| Input:    region          - the region
| Output:   d2d_region      - the Direct2D region as a ID2D1GeometryGroup
| Return:   TRUE if a region was created
\*--------------------------------------------------------------------------*/
static int _uint_d2d_region_create (void *region,
                                    ID2D1GeometryGroup **d2d_region)
{
    ID2D1Factory               *factory;
    DWORD                       region_size;
    unsigned char              *buffer;
    RGNDATA                    *region_data;
    UINT                        count;
    RECT                       *region_rect;
    ID2D1RectangleGeometry    **clip_rects = (ID2D1RectangleGeometry **) NULL;
    ID2D1RectangleGeometry    **clip_rect;
    D2D1_RECT_F                 rect;
    ID2D1GeometryGroup         *clip_group = (ID2D1GeometryGroup *) NULL;
    int                         status = FALSE;

    /*
    ** Only create the geometry for complex regions, i.e. those comprised of
    ** more than a single rectangle
    */

    if (region != NULL &&
        _uint_d2d_factory (&factory) &&
        (region_size =
         GetRegionData ((HRGN) region, 0, (RGNDATA *) NULL)) > 0 &&
        (count =
         ((region_size - sizeof (RGNDATAHEADER)) / sizeof (RECT))) > 1)
    {
        PRO_CREATE_AND_LOCK_STATIC_BUFFER (buffer, 1024, region_size);
        {
            if ((region_data = (RGNDATA *) buffer) != (RGNDATA *) NULL &&
                GetRegionData ((HRGN) region, region_size, region_data) &&
                region_data->rdh.iType == RDH_RECTANGLES &&
                (count = (UINT) region_data->rdh.nCount) > 0 &&
                (region_rect = (RECT *) region_data->Buffer) != (RECT *) NULL)
            {
                PRO_CREATE_AND_LOCK_STATIC_BUFFER (clip_rects, 64, count);
                {
                    for (clip_rect = clip_rects;
                         region_data->rdh.nCount > 0;
                         region_rect++,
                         clip_rect++,
                         region_data->rdh.nCount--)
                    {
                        rect.left = (FLOAT) region_rect->left;
                        rect.top = (FLOAT) region_rect->top;
                        rect.right = (FLOAT) region_rect->right;
                        rect.bottom = (FLOAT) region_rect->bottom;

                        ID2D1Factory_CreateRectangleGeometry (
                            factory,
                            &rect,
                            clip_rect);
                    }

                    ID2D1Factory_CreateGeometryGroup (
                        factory,
                        D2D1_FILL_MODE_ALTERNATE,
                        (ID2D1Geometry **) clip_rects,
                        count,
                        &clip_group);

                    status = TRUE;
                }
                PRO_UNLOCK_STATIC_BUFFER (clip_rects);
            }
        }
        PRO_UNLOCK_STATIC_BUFFER (buffer);
    }

    *d2d_region = clip_group;

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_region_destroy
| Purpose:  Destroy a region for Direct2D
| Input:    d2d_region      - the region
| Output:   d2d_region      - the released Direct2D region
| Return:   TRUE if the region was destroyed
\*--------------------------------------------------------------------------*/
static int _uint_d2d_region_destroy (ID2D1GeometryGroup **d2d_region)
{
    ID2D1GeometryGroup     *clip_group = *d2d_region;
    UINT                    count, i;
    ID2D1Geometry         **clip_rects = (ID2D1Geometry **) NULL;
    int                     status = FALSE;

    if (clip_group != (ID2D1GeometryGroup *) NULL)
    {
        count = ID2D1GeometryGroup_GetSourceGeometryCount (clip_group);

        PRO_CREATE_AND_LOCK_STATIC_BUFFER (clip_rects, 64, count);
        {
            ID2D1GeometryGroup_GetSourceGeometries (
                clip_group,
                clip_rects,
                count);

            for (i = 0; i < count; i++)
            {
                ID2D1Geometry_Release (clip_rects [i]);
            }

            ID2D1GeometryGroup_Release (clip_group);
        }
        PRO_UNLOCK_STATIC_BUFFER (clip_rects);

        *d2d_region = (ID2D1GeometryGroup *) NULL;

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_region_set
| Purpose:  Set a clipping region onto a Direct2D render target
| Input:    renderer        - the render target
|           d2d_region      - the Direct2D region
|           region          - the region
|           set             - TRUE to set the region, FALSE to clear
| Output:
| Return:   TRUE if the region was set onto the render target
\*--------------------------------------------------------------------------*/
static int _uint_d2d_region_set (ID2D1RenderTarget *renderer,
                                 ID2D1GeometryGroup *d2d_region, void *region,
                                 int set)
{
    static D2D1_LAYER_PARAMETERS    layer =
    {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        (ID2D1Geometry *) NULL,
        D2D1_ANTIALIAS_MODE_ALIASED,
        { { { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f } } },
        1.0f,
        (ID2D1Brush *) NULL,
        D2D1_LAYER_OPTIONS_NONE
    };

    RECT                            box;
    int                             status = TRUE;

    if (region == NULL)
    {
        status = FALSE;
    }
    else if (set)
    {
        /*
        ** Get the bounds of the region
        */

        GetRgnBox ((HRGN) region, &box);

        layer.contentBounds.left = (FLOAT) box.left;
        layer.contentBounds.top = (FLOAT) box.top;
        layer.contentBounds.right = (FLOAT) box.right;
        layer.contentBounds.bottom = (FLOAT) box.bottom;

        if (d2d_region != (ID2D1GeometryGroup *) NULL)
        {
            /*
            ** For complex regions, use a layer masked by the geometry
            */

            layer.geometricMask = (ID2D1Geometry *) d2d_region;

            ID2D1RenderTarget_PushLayer (
                renderer,
                &layer,
                (ID2D1Layer *) NULL);
        }
        else
        {
            /*
            ** For simple regions, use an axis-aligned clip
            */

            ID2D1RenderTarget_PushAxisAlignedClip (
                renderer,
                &(layer.contentBounds),
                D2D1_ANTIALIAS_MODE_ALIASED);
        }
    }
    else if (d2d_region != (ID2D1GeometryGroup *) NULL)
    {
        /*
        ** For complex regions, use a layer masked by the geometry
        */

        ID2D1RenderTarget_PopLayer (renderer);
    }
    else
    {
        /*
        ** For simple regions, use an axis-aligned clip
        */

        ID2D1RenderTarget_PopAxisAlignedClip (renderer);
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_region
| Purpose:  Get the clipping region for Direct2D
| Input:    device_context  - the device context
| Output:
| Return:   The clipping region
\*--------------------------------------------------------------------------*/
void *_uint_d2d_renderer_region (HDC device_context)
{
    nt_dcrt_t  *dcrt;

    return ((_uint_d2d_renderer_add_remove (
                 device_context, (RECT *) NULL, FALSE, FALSE, FALSE, FALSE,
                 &dcrt) &&
             dcrt->bitmap_renderer != (ID2D1BitmapRenderTarget *) NULL) ?
            dcrt->bitmap_renderer_clip_region :
            NULL);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_clip
| Purpose:  Set the clipping region for Direct2D
| Input:    device_context  - the device context
|           region          - the region to use for clipping
|           mode            - the combination mode
| Output:
| Return:   TRUE if the clipping was applied
\*--------------------------------------------------------------------------*/
int _uint_d2d_renderer_clip (HDC device_context, void *region, int mode)
{
    nt_dcrt_t  *dcrt;
    int         status = FALSE;

    if (_uint_d2d_renderer_add_remove (
            device_context, (RECT *) NULL, FALSE, FALSE, FALSE, FALSE,
            &dcrt) &&
        dcrt->bitmap_renderer != (ID2D1BitmapRenderTarget *) NULL)
    {
        /*
        ** Remove any clipping and destroy the layer
        */

        if (dcrt->bitmap_renderer_clip_region != NULL)
        {
            _uint_d2d_region_set (
                (ID2D1RenderTarget *) dcrt->bitmap_renderer,
                dcrt->bitmap_renderer_clip_group,
                dcrt->bitmap_renderer_clip_region,
                FALSE);

            _uint_d2d_region_destroy (&(dcrt->bitmap_renderer_clip_group));
        }

        /*
        ** Define the new clipping region
        */

        if (region == NULL)
        {
            if (mode == UI_COPY ||
                mode == UI_AND)
            {
                _ui_gfx_region_destroy (&(dcrt->bitmap_renderer_clip_region));
            }
        }
        else if (mode == UI_COPY ||
                 dcrt->bitmap_renderer_clip_region == NULL)
        {
            _ui_gfx_region_copy (dcrt->bitmap_renderer_clip_region, region);
        }
        else
        {
            _ui_gfx_region_combine (
                &(dcrt->bitmap_renderer_clip_region),
                dcrt->bitmap_renderer_clip_region,
                region,
                mode);
        }

        /*
        ** Avoid using a simple region which maps to entire context
        */

        if (_ui_gfx_region_is_rect (
                dcrt->bitmap_renderer_clip_region,
                &(dcrt->bitmap_renderer_bounds)))
        {
            _ui_gfx_region_destroy (&(dcrt->bitmap_renderer_clip_region));
        }

        /*
        ** Create the layer (for complex regions) and apply the clipping
        */

        if (dcrt->bitmap_renderer_clip_region != NULL)
        {
            _uint_d2d_region_create (
                dcrt->bitmap_renderer_clip_region,
                &(dcrt->bitmap_renderer_clip_group));

            _uint_d2d_region_set (
                (ID2D1RenderTarget *) dcrt->bitmap_renderer,
                dcrt->bitmap_renderer_clip_group,
                dcrt->bitmap_renderer_clip_region,
                TRUE);
        }

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_unclip
| Purpose:  Temporarily un-clip or re-clip the clipping region for Direct2D
| Input:    context         - the context
|           clip            - TRUE to re-clip
| Output:
| Return:   TRUE if the clipping was temporarily changed
\*--------------------------------------------------------------------------*/
int _uint_d2d_renderer_unclip (HDC device_context, int clip)
{
    nt_dcrt_t  *dcrt;
    int         status = FALSE;

    if (_uint_d2d_renderer_add_remove (
            device_context, (RECT *) NULL, FALSE, FALSE, FALSE, FALSE,
            &dcrt) &&
        dcrt->bitmap_renderer_clip_group != (ID2D1GeometryGroup *) NULL)
    {
        _uint_d2d_region_set (
            (ID2D1RenderTarget *) dcrt->bitmap_renderer,
            dcrt->bitmap_renderer_clip_group,
            dcrt->bitmap_renderer_clip_region,
            clip);

        status = TRUE;
    }

    return (status);
}


/*--------------------------------------------------------------------------*\
| Function: _uint_d2d_renderer_set_origin
| Purpose:  Set the drawing origin for Direct2D
| Input:    device_context  - the device context
|           x               - the x co-ordinate of the origin
|           y               - the y co-ordinate of the origin
| Output:
| Return:   UI_SUCCESS if everything went well, otherwise UI_ERROR
\*--------------------------------------------------------------------------*/
int _uint_d2d_renderer_set_origin (HDC device_context, int x, int y)
{
    nt_dcrt_t          *dcrt;
    D2D1_MATRIX_3X2_F   transform;
    int                 status = FALSE;

    if (_uint_d2d_renderer_add_remove (
            device_context, (RECT *) NULL, FALSE, FALSE, FALSE, FALSE,
            &dcrt) &&
        dcrt->bitmap_renderer != (ID2D1BitmapRenderTarget *) NULL)
    {
        if (dcrt->bitmap_renderer_bounds.x != x ||
            dcrt->bitmap_renderer_bounds.y != y)
        {
            if (dcrt->bitmap_renderer_clip_region != NULL)
            {
                OffsetRgn (
                    (HRGN) dcrt->bitmap_renderer_clip_region,
                    (x - dcrt->bitmap_renderer_bounds.x),
                    (y - dcrt->bitmap_renderer_bounds.y));
            }

            ID2D1BitmapRenderTarget_GetTransform (
                dcrt->bitmap_renderer,
                &transform);

            transform.dx = (FLOAT) -x;
            transform.dy = (FLOAT) -y;

            ID2D1BitmapRenderTarget_SetTransform (
                dcrt->bitmap_renderer,
                &transform);

            dcrt->bitmap_renderer_bounds.x = x;
            dcrt->bitmap_renderer_bounds.y = y;

            if (dcrt->bitmap_renderer_clip_region != NULL)
            {
                _uint_d2d_renderer_clip (device_context, NULL, UI_OR);
            }
        }

        status = TRUE;
    }

    return (status);
}

#endif /* UI_SYSTEM_NT */
