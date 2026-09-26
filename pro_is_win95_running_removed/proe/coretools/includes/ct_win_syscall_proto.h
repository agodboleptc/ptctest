#ifndef ct_win_syscall_proto_h
#define ct_win_syscall_proto_h

/*######################################################################*/
/* Date         Ver.    Author  Mod.    Description                     */
/* ====         ====    ======  ====    ===========                     */
/* 12-Feb-06    L-01-02 Asaf    $$01    Created for PROTO project       */
/* 24-Nov-06    L-01-21 PROTO   $$2     Automatic prototype creation    */
/* 16-May-08    L-03-09 BI      $$3     Used syswindows.h instead of windows*/
/* 25-Mar-09    L-03-29 aap     $$4     Prototype pro_is_vista_running(). */
/* 06-Jun-10    L-05-24 lli     $$5     Added pro_is_win7_running()     */
/* 12-Mar-12    P-20-01 AC      $$6     Updated for Project 13028358    */
/* 03-Oct-12    P-20-15 Shturm  $$7     Fixed compilation warning.      */
/* 01-May-18    P-60-02 jas     $$8     Added pro_is_win10_running()    */
/* 29-Jun-21    P-90-16 jas     $$9     Added pro_is_win11_running()    */
/* 18-Sep-23    Q-11-30 jas     $$10    Added WIN10 and WIN11 builds    */
/* 10-Nov-23    Q-11-39 jas     $$11    Added WIN11_23H2 build          */
/* 14-Feb-24    Q-12-01 jas     $$12    Added WIN11_24H2 build          */
/* 13-Oct-24    Q-13-31 jas     $$13    Added WIN11_25H2 build          */
/* 19-Aug-26    Q-27-22 jas     $$14    Added WIN11_26H2 build          */
/*######################################################################*/

#if OPER_SYS == WINDOWS_32
#include <syswindows.h>
extern BOOL CALLBACK console_hide(HWND mywindow, LPARAM ignore);
LIB_CORETOOLS_API  int play_batch_file(char *cmd_string);
LIB_CORETOOLS_API  int call_script(char *cmd_string, int *rc);
LIB_CORETOOLS_API  int pro_is_vista_running (void);
LIB_CORETOOLS_API  int pro_is_win7_running (void);
LIB_CORETOOLS_API  int pro_is_win10_running (void);
#define WIN10_1507 10240
#define WIN10_1511 10586
#define WIN10_1607 14393
#define WIN10_1703 15063
#define WIN10_1709 16299
#define WIN10_1803 17134
#define WIN10_1809 17763
#define WIN10_1903 18362
#define WIN10_1909 18363
#define WIN10_2004 19041
#define WIN10_20H2 19042
#define WIN10_21H1 19043
#define WIN10_21H2 19044
#define WIN10_22H2 19045
LIB_CORETOOLS_API  int pro_is_win11_running (void);
#define WIN11_21H2 22000
#define WIN11_22H2 22621
#define WIN11_23H2 22631
#define WIN11_24H2 26052
#define WIN11_25H2 26200
#define WIN11_26H2 26300
extern int pro_spawn_for_version_check_use_array(char *exec_name, char **exec_param);
extern void handle_nt_env_vars(char *command, char *newcmd);
extern HANDLE pro_exec_background_shell_ex(char *cmd_fmt, ...);
#endif

/* of ct_win_syscall_proto_h closure */
#endif

