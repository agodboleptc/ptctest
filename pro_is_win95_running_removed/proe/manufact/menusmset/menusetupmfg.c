/* Peter Everett, Peter Liberman  change .inc and .hh to .h in includes */
/* 06-AUG-92  D-01-15 Integ  Removed include files. */
#include <const.h>
#include        "errors.h"
#include	"menucons.h"
#include	"mfg.h"
#include	"xarray.h"
#include        "proincludes.h"
#include        "mfgcons.h"
#include        "runmode.h"
#include        "genmfgtbl.h"
#include        "manufact_msg.h"
#include        "useroper.h"
#include        <pro_string.h>
#include        <uservol.h>
#include <cblflat_proto.h>
#include <ct_win_syscall_proto.h>
#include <ctsyscall_proto.h>
#include <cu_cutils_proto.h>
#include <cu_msgutil_proto.h>
#include <dashboard_proto.h>
#include <dbg_crash.h>
#include <featcmd_proto.h>
#include <featgen_proto.h>
#include <feattab_proto.h>
#include <featutil2_proto.h>
#include <featutil_proto.h>
#include <g_memory_proto.h>
#include <g_model_proto.h>
#include <getputgl_proto.h>
#include <mach_db_proto.h>
#include <machwind_proto.h>
#include <mass_proto.h>
#include <mctool_proto.h>
#include <menumgr_proto.h>
#include <menus1_proto.h>
#include <menus2_proto.h>
#include <menusf_proto.h>
#include <menusm_proto.h>
#include <menusmld_proto.h>
#include <menusmop_proto.h>
#include <menusmset_proto.h>
#include <mfg_cl_proto.h>
#include <mfgui_proto.h>
#include <mfgutil_proto.h>
#include <minfo_proto.h>
#include <nc_seq_proto.h>
#include <operui_proto.h>
#include <ptc_xarray.h>
#include <rel_proto.h>
#include <smt_zone_proto.h>
#include <turn_proto.h>
#include <useroper_proto.h>
#include <utility_proto.h>
#include <turnprof_const.h>
#include <pro2mfg_proto.h>
#include <vol_utils_proto.h>
#include <relparamui_proto.h>



static int mfgset_tool(Mfg *mfg_ptr);
static int mfgset_milvol(Mfg *mfg_ptr);
static int mfgset_milsrf(Mfg *mfg_ptr);
static int mfgset_mp(Mfg *mfg_ptr);
static int mfgset_dim_bnd(Mfg *mfg_ptr);
static int mfgset_units(Mfg *mfg_ptr);
static int mfgset_drl_grp(Mfg *mfg_ptr);
static int menu_mfgset_model_setup(Mfg *mfg_ptr);

/*
  Menu Interface for Setup of Tool & Manufacturing Assembly.
  Date		Version   Author		Desc.
  ----		-------   ------		-----
 30-MAY-89	 A-12-03   IGO	$$1 Created.
 06-DEC-89       A-17-02   JMS  $$2 Modified interface. Added "Tool"
 07-SEP-90 A-21-26  JMS  $$3 Removed "Machine", added "Mass Props", "Units" and
			     "Dim Bound".
 13-FEB-91 A-25-12  LUH  $$4 Added menu_setup_smfg();
 04-APR-91 A-26-04  LUH  $$5 Added ierror == E_NO_ERROR check in cr_smtmfg_csys.
 03-APR-91 A-26-02  SWL  $$6 Called csys_ptr_to_name.
 25-APR-91 A-26-05  GGR  $$7 Declared get_mfg_workpiece.
 02-MAY-91 A-26-08  LUH  $$8 Put all mfg setup button into smtmfg setup menu.
 10-MAY-91 A-26-10  JMS  $$9 Added "Mill Volume".
 13-may-91 a-26-09  duk $$10 changed find_smtmfg_frst_oper =>
                                                        get_first_smt_mfg_feat
 14-jul-91 a-27-03  duk $$11 updated ref csys prompt
 29-AUG-91 A-27-10  LUK $$12 Added Tool Table menu.
 03-SEP-91 A-27-13  JMN $$13 Call menu_get_csys_() instead of menu_get_csys().
 18-SEP-91 A-27-18 ILBS $$14 Added Setup Name.
 02-OCT-91 A-28-04  BCS $$15 Made inaccesible "Tool Table" in "SMFG SETUP".
 14-NOV-91 C-01-08  GGR $$16 Added Mill Surface button.
 10-JAN-91 C-01-24  BCS $$17 Added button PPRINT.
 17-JAN-92 C-01-25  LUK $$18 Added Mfg Site menu item.
 30-dec-91 c-01-23  duk      added machinability data
 22-JAN-92 C-01-26  LUK $$19 Pass MF_TOOL_TABLE to setup_mfg_table().
 05-feb-92 c-01-30  duk $$20 user can setup station table for smt
 30-APR-92 D-01-02  JMS $$21 Disabled "Mill Volume", "Mill Surface" if workpiece
                             doesn't have any features. Fixed calling sequence
                             to regen3d_from_feat() in cr_smtmfg_csys().
 12-may-92 d-01-02  duk $$22 added machine zones and clamps
 30-JUN-92 C-02-23  Lee $$23 Moved the static function declaration to the top
 15-JUL-92 D-01-13  MTP $$24 Added drill groups.
 05-oct-92 d-01-31  duk $$25 fixed message
  8-OCT-92 D-01-34 Yuri $$26 Fixed setup_mfg_acc for harness mfg
 15-OCT-92 D-01-36 Yuri $$27 Fixed again.
 09-nov-92 d-01-46 duk  $$28 fixed bug in cr_smtmfg_csys;
                             disallow selecting Machine Csys if zone exists.
 12-jan-92 d-02-07 EY   $$29 Changes in mfgset_milvol().
 12-JAN-93 D-02-06 JMS  $$30 Updated for assembly machining.
 01-FEB-93 D-02-10 GGR  $$31 Removed call to set_csys_create_pre_func
 26-JAN-93 D-02-09 ITS  $$32 Moved "Site" button to upper level.
 12-FEB-93 D-02-11 GGR  $$33 Called set_part_mod_mfg_wp_stats
 15-MAR-93 D-02-18 MTP  $$34 Added Drill Group to setup_mfg_acc().
 06-Apl-93 D-02-22 XDH  $$35 Changed can_add_feats_to_wrkpc.
			     if no feature in the assembly, blank out
			     button for both part and assembly maching.
 27-apr-93 d-02-28 duk  $$36 Fixed calling sequence to regen3d_from_feat()
                             in cr_smtmfg_csys().
 26-Apl-93 D-02-28 XDH  $$37 Called menu_mold_parting_srf in mfgset_milsrf.
 13-MAY-93 D-02-30 Nick $$38 Moved Site button to Mfg Setup from Mfg menu.
 09-jun-93 d-03-05 EY   $$39 Added "Proc Setup" button.
 14-jun-93 d-03-06 EY        Changed menu name.
 22-Jul-93 E-01-07 CJL  $$40 Changed get_yes_no to accept prompt
 12-jul-93 d-03-11 duk  $$41 added "Feed Colors"
 19-AUG-93 E-01-12 XDH  $$42 Added setup_workcell.
 31-AUG-93 E-01-13 Anatoli $$43 Added setup_model_parameters.
 08-SEP-93 E-01-14 ITS  $$44 Rearranged setup menu.
 15-SEP-93 E-01-15 ITS  $$45 Fixed bug, removed Operation button.
 20-SEP-93 E-01-15 XDH  $$46 blank out setup workcell if no workpiece.
 21-sep-93 e-01-16 duk  $$47 disabled "Feed Colors" for Cabling
 11-OCT-93 E-01-20 Yuri $$48 Fixed menu for Cable Mfg
 18-OCT-93 E-01-22 ITS  $$49 Push all menus for Mfg Geometry buttons.
 15-DEC-93 E-03-02 dan  $$50 Change menu_setup_mfg to include smt setup as well.
 27-DEC-93 E-03-05 AN   $$51 Added Peck Depth Table.
 07-FEB-94 E-03-10 GGR  $$52 Added NC Aliases
 09-MAR-94 E-03-15 GGR  $$53 Beta fix.  Make Alias unavailable in user_run_mode.
 21-MAR-94 E-03-18 GGR  $$54 Removed above.
 24-Mar-94 E-03-17 dan  $$55 Disallow selection of invalid csys
                            (X-Y should be on green(face) or white (offset) and
                             Z out of workpiece).
 02-juN-94 E-03-27 leo  $$56 Replaced get_yes_no() with mfg_confirm_request().
 24-MAY-94 E-06-02 ITS  $$57 Added PProcessor button.
 21-Jun-94 E-03-29 dan  $$58 Replaced menu_setup_mach_zones() by
                              smt_menu_setup_mach_zones().
 10-Jul-94 E-06-02 dan  $$59 Removed Clamps from "Mfg Setup" menu to
                             "Machine Zone Setup" menu.
 27-JUL-94 E-06-04 yf   $$60 called setup_post_create().
 17-AUG-94 E-06-08 yf   $$61 Added "Register".
 06-OCT-94 E-06-14 yf   $$62 Changed calling sequence of menu_setup_register().
 15-DEC-94 E-07-03 GYU  $$63 Added is_post_14_mfg_ui() and changed static
                             functions to extern.
 02-JAN-95 E-07-04 GYU  $$64 Changed menu_mfgset_geom() to extern.
 10-JAN-95 E-07-05 LUK  $$65 Added Tool Table button for workcell setup.
 22-JAN-95 E-07-05 LUK  $$66 Extern setup_workcell().
 08-FEB-95 E-07-06 JMH  $$67 Changed is_post_14_mfg_ui() to use run mode again.
 09-FEB-95 E-07-06 JMH  $$68 is_post_14_mfg_ui() return false now for smt mfg.
 24-JAN-95 E-07-04 PBO  $$69 Changed PPRINT so that another menu called
                             instead of going direclty to PPRINT table.
                             Made function menumfg_pprint().
 09-FEB-95 E-07-07 JMH  $$70 Removed menu pop/push in setup_workcell(), changed
                             menu_mfgset_geom(), menumfg_register(), MFG_SETUP.
 05-Mar-95 E-07-08 dan  $$71 is_post_14_smt_mfg_ui().
 09-Feb-95 E-07-06 LMh  $$72 Removed option "StationTable" from mfg setup menu.
 09-FEB-95 E-07-09 JMH  $$73 Added and called menu_mfg_datum_feat().
 02-Apr-95 E-07-09 dan  $$74 is_post_14_smt_mfg_ui(): Opened for regular rmode.
 11-Apr-95 E-07-08 nker $$75 Added is_post_14_manufact_ui()
 04-APR-95 E-07-10 JMH  $$76 Shortened "Datum Features" to "Datum Feats", fixed
                             menudatum() / feattop() usage.
 18-APR-95 E-07-10 nker $$77 Replaced is_post_14_mfg_ui() call by
                             is_post_14_manufact_ui() in menumfg_register().
 24-APR-95 E-07-11 JMH  $$78 Fixed menu_mfg_datum_feat() for part mfg.
 06-AUG-95 G-01-02 nker $$79 Allowed PPRINT command for smt mfg.
 14-SEP-95 G-01-07 GYU  $$80 Added menu_post_proc_opts().
 27-SEP-95 G-01-07+ GYU $$81 Changed menu file name.
 28-OCT-95 G-01-11 GYU  $$82 Removed "General" in menu_post_proc_opts().
 07-NOV-95 G-01-12 LUK  $$83 Handled ncpost license.
 11-NOV-95 G-01-13 LUK  $$84 Called allow_campost().
 25-NOV-95 G-01-15 LUK  $$85 Called use_proncpost_ui().
 21-DEC-95 G-01-18 LMh  $$86 Replaced cr_smtmfg_csys with assign_smt_mach_csys()
 26-JAN-95 G-03-02 PBO  $$87 Upadated call for PPRINT to menumfg_table_setup().
 20-FEB-96 G-03-03 LMh  $$88 Added "Mach Zone" for new MZ functionality.
 13-FEB-95 G-01-21 PBO  $$89 Assigned cut_motion field of menu_data.
 02-MAR-96 G-03-04 LMh  $$90 Removed 88.
 16-MAR-96 G-03-06 YF   $$91 Added mfg_setup_window.
 25-MAR-96 G-03-07 GYU  $$92 Added Turn Profile.
 25-MAR-96 G-03-07 LMh  $$93 Removed old mach zone code.
 10-JUN-96 G-03-17 LUK  $$94 Removed menu_post_proc_opts().
 18-JUL-96 G-03-20 LUK  $$95 Setup param env for pt/ncpost.
 08-SEP-96 H-01-08 Tim  $$96 Call set_assel_cur_part(NULL) in menu_mfg_datum_feat
 13-SEP-96 H-01-09 LUK  $$97 Added debug code to icam.
 15-Oct-96 H-01-13  EEB/MRC $$98 Allow icam debugging.
 20-NOV-96 H-01-18 JMH  $$99 Called set_creating_3_dtms() from
                             menu_mfg_datum_feat().
 07-FEB-97 H-01-22 dpek $$100      Mfg cleanup
 5-Mar-97  H-03-03 LPE  $$101      Removed unused functions.
 07-MAR-97 H-01-27+ PLS $$102 Just so I can be the hundreth second person to
                             correct this file!
			     Change dcl_com call to pro_exec_command_new
			     for Win95. dcl_com stopped working for batch
			     files with no .bat extension. No clue why,
			     but this is an obsolete function anyway.
 10-MAR-97 H-03-05 MYO  $$103 Removed button "Machine Zone" from MFG_SETUP
 15-APR-97 H-03-06 YF   $$104 Adopted calls to menu functions so that they can
>                             be called from new mfg dialog.
 28-APR-97 H-03-07 YF   $$105 Calling sequence to mfg_setup_window().
 08-MAY-97 H-03-10 YF   $$106 Unstatic setup_mfg_acc.
 07-MAY-97 H-03-10 mrayko $$107 Closed register set up for smt mfg.
 03-MAR-98 H-03-40 lmp  $$108 Put pivot curve to misc_data in
			      menu_mfg_datum_feat_n_type().
 17-Apr-98 I-01-05 RR   $$109 Hooked up new workcell dialog.
 14-APR-98 I-01-05 mrayko $$110 Removed old smt stuff.
 27-JUL-98 I-01-16 prf  $$111 called play_batch_script for WINDOWS_95
 25-Sep-98 I-01-21 RR   $$112 Workcell init to NULL in setup_workcell().
 08-Jul-99 I-03-11 RR   $$113 Enabled GPOST.
                        113.1 Modified args of ncfm_mach_dlg_main().
 04-FEB-00 I-03-26 EY   $$114 Made "Fixture" invisible for new fixtures.
 28-FEB-00 J-01-02 EY   $$115 Undone 114 for cmm.
 07-Mar-00 J-01-04 Menn $$116 Added parameter to menu_mold_parting_surf ().
 17-Apr-01 J-01-33 RR   $$117 Changed args of ncfm_mach_dlg_main().
 21-Sep-01 J-03-09 jas  $$118 Removed WINDOWS_95 macro
 14-May-02 J-03-25 mkh  $$119 Called set_cancel_dashb_ui().
 16-Jul-02 J-03-29 SVRS $$120 Called pre_create_check_feat (#961547)
 14-Apr-04 K-01-26 TRG  $$121 Purify fixes.
 21-JUN-04 K-03-04 LMh  $$122 Removed "Datum Feats" button from 
                              "Mfg Geometry" menu.
 20-JUN-04 K-03-04 akap $$123 added user operation registration
 24-JUN-04 K-03-05 LMh  $$124 Removed mill window, turn profile and
                              drill group from mfg geometry menu.
 23-JUL-04 K-03-07 LMh  $$125 Fixed c.s. for mfg_setup_window in
                              mfg_geom_feat_prewf_act()
 19-SEP-04 K-03-10 LIM  $$126 Supported Mill Volume new UI.
 03-OCT-04 K-03-11 LIM  $$127 Supported Mill Surface new UI.
                              In mfg_geom_feat_prewf_act() used u_strcmp().
 25-OCT-04 K-03-13 LIM  $$128 Called mfgproc_milvol(), mfgproc_milsrf().
 22-DEC-04 K-03-17 LMh  $$129 Added set/restore statics
 10-FEB-05 K-03-19 LMh  $$130 Modified prev (exclude part mode for mill wind)
 23-MAR-05 K-03-21 LMh  $$131 Used mill_window_create() instead of 
                              mfg_setup_window() in mfg_geom_feat_prewf_act()
 18-APR-05 K-03-23 sankulka $$132  Corrected return type.
 26-APR-05 K-03-23 LMh  $$133 Removed switch to assem mode for Mill Window 
 06-SEP-05 K-03-31 SOL  $$134 Added new_ui check in mfg_geom_feat_prewf_act() 
 25-JUN-06 L-01-11 akap $$135 fixed spell: registrate->register
 20-SEP-06 L-01-17 LMh  $$136 Used turn_profile_create().
 20-Mar-08 L-03-05 PROTO $$137 Automatic prototype creation
 12-Jul-08 L-03-13  LMh  $$138 Added turn envelope command.
 07-Aug-08 L-03-15  LMh  $$139 Added turn boundary command.
 18-Jan-09 L-03-24  LMh  $$140 Supported asynchronous Mill Volume/Surface
 22-Nov-08 L-05-01  SAN  $$141 Modified mfgset_drl_grp().
 17-AUG-09 L-05-03  SOL  $$142 included vol_utils_proto.h
 18-Jun-13 P-20-32  rbokil $$143 Removed get_pt_ncpost_dbf.
 21-Jun-14 P-20-55  rbokil $$144 Updated setup_post_create.
 25-Jul-19 P-70-20  OMI    $$145 Include added
 25-Jul-21 P-90-19  DevOps $$146 Changed return type of some functions to void
*/

 static char MFG_SETUP[] = "SET UP MFG";
/*---------------------------------------------------------------------------*/
void menu_setup_mfg (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg *mfg_ptr;
{
Gen_menu_data menu_data;

 menu_data.mfg_ptr = mfg_ptr;
 menu_data.feat_ptr = NULL;
 menu_data.cut_motion = NULL;

 promenu_create (MFG_SETUP, "mfgsetup.mnu");
   
 user_operation_register (ModelSetupOper);

 /* Both SMT MFG and non-SMT MFG */
 promenu_on_button (MFG_SETUP, "Workcell", setup_workcell, mfg_ptr, 0);
 promenu_on_button (MFG_SETUP, "Tool", mfgset_tool, mfg_ptr,0);
 promenu_on_button (MFG_SETUP, "Site", menumfg_table_setup, &menu_data,
                    MF_SITE_TABLE );
 promenu_on_button (MFG_SETUP, "Register", menumfg_table_setup, &menu_data,
                    MF_REGISTER_TABLE );
 promenu_on_button (MFG_SETUP, "Name", menu_setup_names, mfg_ptr, 0);
 promenu_on_button (MFG_SETUP, "Model Setup", menu_mfgset_model_setup,
                                mfg_ptr, 0);
 promenu_on_button (MFG_SETUP, "NC Aliases", menu_mfgset_nc_aliases,
                                mfg_ptr, 0);

 /* Only non-SMT MFG */
 promenu_on_button (MFG_SETUP, "Mfg Geometry", menu_mfgset_geom, mfg_ptr, 0);
 promenu_on_button (MFG_SETUP, "Fixture", menu_fixt_ui, mfg_ptr, 0);
 promenu_on_button (MFG_SETUP, "Tool Table", setup_tool_table, mfg_ptr, 0);
 promenu_on_button (MFG_SETUP, "Peck Table", menumfg_peck,
                                  mfg_ptr, 0 );
 promenu_on_button (MFG_SETUP, "PPRINT", menumfg_table_setup,
                                  &menu_data, MF_PPRINT_TABLE);
 promenu_on_button (MFG_SETUP, "Mach DB", mfgset_mach_db, mfg_ptr,0);
 promenu_on_button (MFG_SETUP, "Feed Colors", menu_feed_colors_setup,
                                mfg_ptr, 0);


 /* Only SMT MFG */
 promenu_on_button (MFG_SETUP, "Machine Csys", assign_smt_mach_csys, mfg_ptr, 0);
 /*
 promenu_on_button (MFG_SETUP, "Machine Zone", smt_mach_zone_setup, mfg_ptr,0);
 */

 promenu_on_button (MFG_SETUP, "PProcessor", setup_post_create, NULL,0);

 promenu_on_button (MFG_SETUP, "Done/Return", promenu_exit_up, 0,0);
 promenu_on_button (MFG_SETUP, MFG_SETUP, promenu_exit_up, 0,0);

 promenu_set_item_visible (MFG_SETUP, "Machine Csys", FALSE);
 promenu_set_item_visible (MFG_SETUP, "Machine Zone", FALSE);

 if (use_wf_ui_for_mfg_geom())
   promenu_set_item_visible (MFG_SETUP, "Mfg Geometry", FALSE);

 if (is_cable_mfg (mfg_ptr))
    {
     promenu_set_item_visible (MFG_SETUP, "Workcell", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Fixture", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Mfg Geometry", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Peck Table", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Tool Table", FALSE);
     promenu_set_item_visible (MFG_SETUP, "PPRINT", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Mach DB", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Tool", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Site", FALSE);
     promenu_set_item_visible (MFG_SETUP, "Feed Colors", FALSE);
    }

 /* leave "Fixture" button only for old fixtures */
 if (!is_cmm_mfg (mfg_ptr) && !model_has_old_fixt_setup (mfg_ptr->assem_ptr))
    promenu_set_item_visible (MFG_SETUP, "Fixture", FALSE);

 promenu_make (MAIN_MENU, MFG_SETUP);
 setup_mfg_acc (mfg_ptr);

 if ( promenu_inquire_item_visible (MFG_SETUP, "Mfg Geometry") &&
      promenu_inquire_item_access (MFG_SETUP, "Mfg Geometry") )
    push_command_on_stack ("Mfg Geometry", FALSE);

 promenu_action(0);
 
 user_operation_end (ModelSetupOper);
}

/*---------------------------------------------------------------------------*/
static menu_mfg_datum_feat( mfg_ptr )
 Mfg  *mfg_ptr;
{
 return ( menu_mfg_datum_feat_n_type ( mfg_ptr, K_NOT_USED ) );
}

/*---------------------------------------------------------------------------*/
menu_mfg_datum_feat_n_type ( mfg_ptr, feat_type )
/*---------------------------------------------------------------------------*/
 Mfg  *mfg_ptr;
 int   feat_type;
{
 int        ierror, *save_memb_id_tab, *memb_id_tab, ft_type;
 Solid      *saved_part_ptr, *saved_sel_part, *the_part;
 Feat_type  *feat_type_ptr;
 Assem      *assem_ptr;

 if ( ( mfg_ptr == NULL ) || ( (assem_ptr = mfg_ptr->assem_ptr) == NULL ) )
   return ( PTC_E_ABORT );

 /* set necessary statics, modes, etc, before feattop */
 get_part_mod_statics( &saved_part_ptr, &saved_sel_part, &save_memb_id_tab,
                       NULL );
 set_mfg_comp_statics( mfg_ptr, MF_WORKPIECE, &memb_id_tab );
 set_assel_cur_part(NULL);

 the_part = (Solid*) get_mfg_workpiece( mfg_ptr );

 /* Get feature information from user.  Note that this code is based on that
    found in menufttop.c.  Therefore if problems arise here, first look for
    changes there as this code is unfortunately not kept synchronized by those
    changing menudatum().  --JMH
 */
 feat_type_ptr = alloc_feat_type();
 set_creating_3_dtms( FALSE );

 if ( feat_type == K_NOT_USED )
   ierror = menudatum( the_part, feat_type_ptr );
 else
   ierror = assign_feat_type (get_cur_part(), feat_type_ptr, feat_type);

 ft_type = feat_type_ptr->type;
 if ( ( ierror != E_NO_ERROR ) || ( ft_type != FT_DATUM ) )
   release_ftype( feat_type_ptr );

 if ( ierror == E_NO_ERROR )  {
   int  n = push_all_menus();

 if ( pre_create_check_feat(the_part) != E_NO_ERROR )
    return PTC_E_ABORT;

   if ( ft_type != FT_DATUM )
   {
     /* for all but FT_DATUM, the user only gives the type of feature;
        must let feattop fill in other options */
     Feat  *p_feat = NULL;

     ierror = feattop( assem_ptr, the_part, ft_type, &p_feat );
     put_misc_data( (Model*)mfg_ptr, MFG_MISC_DATUM_FEAT, (char*)p_feat );
   }
   else  {
     /* for FT_DATUM, the user may have given necessary options already;
        if not feattop_w_preset_options still works out okay */
     Feat  *datum_feat;
     feat_alloc( (Model*)the_part, &datum_feat );
     release_ftype( datum_feat->feat_type_ptr );
     datum_feat->feat_type_ptr = feat_type_ptr;
     ierror = feattop_w_preset_options( assem_ptr, the_part, datum_feat, NULL,
                                        NULL, K_NOT_USED );
     put_misc_data( (Model*)mfg_ptr, MFG_MISC_DATUM_FEAT, (char*)datum_feat );
   }
   post_crfeat( assem_ptr, the_part, ierror );
   pop__menus( n );
 }

 /* reset necessary statics, modes, etc, after feattop */
 set_part_mod_statics( saved_part_ptr, saved_sel_part, save_memb_id_tab );
 xar_free( &memb_id_tab );

 setup_mfg_acc( mfg_ptr );
 return ( ierror );
}

/*==========================================================================*/
int mfg_geom_feat_prewf_act (const char* name)
/*--------------------------------------------------------------------------*/
{
  Mfg*  p_mfg    = get_cur_mfg();
  Part* cur_part = NULL;
  Part* sel_part = NULL;
  Part* inf_part = NULL;
  Feat* inf_feat = NULL;
  int*  id_tab   = NULL;
  int   show_mib = info_box_exist ((Model*)p_mfg, NULL);
  int   error    = PTC_E_ABORT;
  int   use_new_ui = use_mfg_vol_and_surf_new_ui ();
  int   asynch_cmd = ProCmdMfgGeom_AsyncCheck (name, K_NOT_USED, TRUE);
  
  if (u_strcmp (name, MILL_VOL_TOP_MENU) == 0)
  {
    if( use_new_ui )
    {
      set_visible_cmds_for_mfg_volume (TRUE);
      set_access_cmds_for_mfg_volume (TRUE);
    }
    if (!inside_tool_route_menu() || asynch_cmd)
      return (mfgset_milvol (p_mfg));
    else
      return (mfgproc_milvol ());
  }

  if (u_strcmp (name, MILL_SRF_TOP_MENU) == 0)
  {
    if ( use_new_ui )
    {
      set_visible_cmds_for_mfg_volume (TRUE);
      set_access_cmds_for_mfg_volume (TRUE);
    }
    if (!inside_tool_route_menu() || asynch_cmd)
      return (mfgset_milsrf (p_mfg));
    else
      return (mfgproc_milsrf ());
  }
  
  if (show_mib)
  {
    get_info_box_envr (&inf_part, &inf_feat);
    remove_mfg_info_box (p_mfg);
  }

  get_part_mod_statics (&cur_part, &sel_part, &id_tab, NULL);
  set_part_mod_mfg_wp_stats (p_mfg->assem_ptr);
  
  if (strcmp (name, "Mill Window") == 0)
  {
    error = mill_window_create (p_mfg);
  }

  else
  if (strcmp (name, "Turn Profile") == 0)
    error = turn_profile_create (p_mfg, TOOL_TURN_PROFILE);

  else
  if (strcmp (name, "Turn Envelope") == 0)
    error = turn_profile_create (p_mfg, TOOL_TURN_ENVELOPE);

  else
  if (strcmp (name, "Turn Boundary") == 0)
    error = turn_profile_create (p_mfg, TOOL_TURN_BOUNDARY);

  else
  if (strcmp (name, "Drill Group") == 0)
    error = mfgset_drl_grp (p_mfg);

  else
    dbg_err_crash ("mfg_geometry_feature", "unknown feature");

  set_part_mod_statics (cur_part, sel_part, id_tab);

  if (show_mib)
    init_mfg_info_box (p_mfg, inf_part, inf_feat);

  return (error);
}

/*---------------------------------------------------------------------------*/
menu_mfgset_geom (mfg_ptr)
Mfg *mfg_ptr;
{
 static char  *menus[] = { "MFG GEOM SET", "MFG GEOMETRY" };
 static char  *files[] = { "mfg_gm_stp.mnu", "mfg_gm_stp2.mnu" };
 int  is_post = is_post_14_mfg_ui();

 promenu_create ( menus[is_post], files[is_post] );

 promenu_on_button ( menus[is_post], "Mill Volume", mfgset_milvol, mfg_ptr, 0);
 promenu_on_button ( menus[is_post], "Mill Surface", mfgset_milsrf, mfg_ptr, 0);
 promenu_on_button_2 ( menus[is_post], "Mill Window", mfg_setup_window, mfg_ptr, NULL);
 promenu_on_button ( menus[is_post], "Turn Profile", setup_turn_profile,
                     mfg_ptr, 0);
 promenu_on_button ( menus[is_post], "Drill Group", mfgset_drl_grp, mfg_ptr, 0);

 if ( is_post )  {
   promenu_on_button( menus[is_post], "Datum Feats", menu_mfg_datum_feat,
                      mfg_ptr, 0 );
   if ( ( mfg_ptr == NULL ) || ( mfg_ptr->assem_ptr == NULL ) )
     promenu_make_item_inaccessible( menus[is_post], "Datum Feats" );
 }

 promenu_on_button ( menus[is_post], menus[is_post], promenu_exit_up, 0,0);

 promenu_make( MAIN_MENU, menus[is_post] );

 promenu_action( menus[is_post] );
}

/*---------------------------------------------------------------------------*/
static int menu_mfgset_model_setup (mfg_ptr)
Mfg *mfg_ptr;
{
 char     *MOD_SETUP = "MFG MODEL SET";

 promenu_create (MOD_SETUP, "mfg_mdl_stp.mnu");

 promenu_on_button (MOD_SETUP, "Mass Props", mfgset_mp, mfg_ptr,0);
 promenu_on_button (MOD_SETUP, "Units", mfgset_units, mfg_ptr,0);
 promenu_on_button (MOD_SETUP, "Dim Bound", mfgset_dim_bnd, mfg_ptr,0);
 promenu_on_button (MOD_SETUP, "Parameters", menu_setup_model_params,
                                             mfg_ptr->assem_ptr, 0);

 promenu_on_button (MOD_SETUP, MOD_SETUP, promenu_exit_up, 0,0);

 if (is_cable_mfg (mfg_ptr))
    promenu_set_item_visible (MOD_SETUP, "Dim Bound", FALSE);

 promenu_make (MAIN_MENU, MOD_SETUP);

 promenu_action(0);

 return E_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
 setup_workcell (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 Solid	*part_ptr;
 Assem	*assem_ptr;
 int	ierror = E_NO_ERROR;
 Solid  *prev_cur_part, *prev_sel_part;
 int    *prev_memb_id_tab;
 Feat   *workcell = NULL;


 if (is_new_mach_dlg())
 {
    ncfm_mach_dlg_main(mfg_ptr, &workcell, FALSE, NULL);
 }
 else
 {
    /* Set current global statics */
    assem_ptr = mfg_ptr->assem_ptr;
    part_ptr = (Solid *) get_mfg_workpiece ( mfg_ptr );
    get_part_mod_statics ( &prev_cur_part, &prev_sel_part, &prev_memb_id_tab,
                           NULL );
    set_part_mod_mfg_wp_stats ( assem_ptr );
    /* push_menu(); */
    ierror = menu_workcell(assem_ptr, part_ptr);
    /* pop_menu(); */
    set_part_mod_statics ( prev_cur_part, prev_sel_part, prev_memb_id_tab );
 }

    return (ierror);
}

/*---------------------------------------------------------------------------*/
void setup_mfg_acc (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 if ( can_add_feats_to_wrkpc(mfg_ptr) )
 {
    promenu_make_item_accessible (MFG_SETUP, "Mfg Geometry");
    promenu_make_item_accessible (MFG_SETUP, "Workcell");
 }
 else
 {
    promenu_make_item_inaccessible (MFG_SETUP, "Mfg Geometry");
    promenu_make_item_inaccessible (MFG_SETUP, "Workcell");
 }
}

/*---------------------------------------------------------------------------*/
can_add_feats_to_wrkpc (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 Assem	*assem_ptr;
 Part	*part_ptr;

 part_ptr = (Part *)get_mfg_workpiece (mfg_ptr);
 assem_ptr = mfg_ptr->assem_ptr;

 if ( part_ptr == NULL )
    return (FALSE);
 else if ((part_ptr->first_feat_ptr == NULL))
    return (FALSE);
 else
    return (TRUE);
}

/*---------------------------------------------------------------------------*/
static int mfgset_tool (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 push_menu ();
 menu_setup_tool (mfg_ptr, NULL);
 pop_menu ();
 return E_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
static int mfgset_milvol (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 _mfgset_milvol (mfg_ptr);
 setup_mfg_acc (mfg_ptr);
 return( E_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
int _mfgset_milvol (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg     *mfg_ptr;
{
 Sld_prt        *cur_part_ptr, *sel_part_ptr;
 int            *cur_memb_id_tab, *memb_id_tab;
 int            n_menus;

 get_part_mod_statics (&cur_part_ptr, &sel_part_ptr, &cur_memb_id_tab, NULL);

 set_mfg_comp_statics (mfg_ptr, MF_WORKPIECE, &memb_id_tab);

 n_menus = push_all_menus();
 menu_setup_mill_vol (mfg_ptr);
 pop__menus(n_menus);

 set_part_mod_statics (cur_part_ptr, sel_part_ptr, cur_memb_id_tab);
 xar_free ( (char **) &memb_id_tab);
 return E_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
static int mfgset_milsrf ( mfg_ptr )
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 Bool   new_dashb_flag = (use_mfg_vol_and_surf_new_ui ())?
                         FALSE : TRUE;
 Bool  old_dashb_flag = set_cancel_dashb_ui (new_dashb_flag);

 _mfgset_milsrf ( mfg_ptr );

 set_cancel_dashb_ui( old_dashb_flag );

 setup_mfg_acc (mfg_ptr);
 return( E_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
int _mfgset_milsrf ( mfg_ptr )
/*---------------------------------------------------------------------------*/
Mfg     *mfg_ptr;
{
 int            n_menus;

 n_menus = push_all_menus();
 menu_mold_parting_surf ( mfg_ptr->assem_ptr, NULL );
 pop__menus(n_menus);
 return E_NO_ERROR;
}


/*---------------------------------------------------------------------------*/
static int mfgset_drl_grp(mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 menu_setup_drl_grp (mfg_ptr);
 return( E_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
int  mfgset_mach_db ( mfg_ptr )
/*---------------------------------------------------------------------------*/
Mfg     *mfg_ptr;
{
 setup_mach_data ( mfg_ptr );
 return( E_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
static int mfgset_mp (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 set_assign_mp ((Model*)mfg_ptr->assem_ptr);
 return E_NO_ERROR;
}

/*---------------------------------------------------------------------------*/
static mfgset_dim_bnd (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 menu_dim_bound ((Model*)mfg_ptr->assem_ptr);
}

/*---------------------------------------------------------------------------*/
static int mfgset_units (mfg_ptr)
/*---------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 change_model_units ((Model*)mfg_ptr->assem_ptr);
 return E_NO_ERROR;
}

/*-------------------------------------------------------------------------*/
int menumfg_peck (mfg_ptr)
/*-------------------------------------------------------------------------*/
Mfg     *mfg_ptr;
{
  push_menu ();
  menu_setup_peck_table (mfg_ptr, FALSE);
  pop_menu ();
  return E_NO_ERROR;
}

/*-------------------------------------------------------------------------*/
static menumfg_site (mfg_ptr)
/*-------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
  push_menu ();
  pop_menu ();
}

/*-------------------------------------------------------------------------*/
 int menu_feed_colors_setup (mfg_ptr)
/*-------------------------------------------------------------------------*/
Mfg     *mfg_ptr;
{
  int	ierror;

  ierror = E_NO_ERROR;
  while (ierror == E_NO_ERROR)
    ierror = menu_feed_colors (mfg_ptr);
  return ierror;
}

/*-------------------------------------------------------------------------*/
int menu_mfgset_nc_aliases ( mfg_ptr )
/*-------------------------------------------------------------------------*/
Mfg	*mfg_ptr;
{
 setup_mfg_table ( mfg_ptr, MF_NC_ALIASES_TBL );
 return E_NO_ERROR;
}


/*-------------------------------------------------------------------------*/
setup_post_create()
{
 char     icam_quest[ICAM_COM_LEN];
 char     qualifier[ICAM_COM_LEN], dbf_file[K_PATH_SIZE];
 char     options[2*K_PATH_SIZE]; /* max size for NT, 1K for unix though */
 char     passwd[K_PATH_SIZE];
 int      ierror, status = TRUE, can_delete = FALSE;
 Pfa      *pfa, *tmp_pfa;

 set_icam_debug_runmode ();  /* make sure run mode is set */

 ierror = setup_post_env();
 if (ierror != E_NO_ERROR)
 {
    dbg_err_syserr ("setup_post_create", "setup_post_env() fails");
    return (FALSE);
 }

 if (get_icam_quest_path_n_name(icam_quest))
 {
    /*-----------------------------------------------------------------------*/
    /* See if pro_ncpost_dbf config option is set, if so, append to command. */
    /* This is not for campost_dir cases, it is for ncpost license cases.    */
    /*-----------------------------------------------------------------------*/
    if (use_ptncpost_ui ()) /* PT/NCPOST license */
    {
       options[0] = NULL_CHAR;

       /*--------------------------------------------------------------------*/
       /* get encrypted passward string, PT/NCPOST must pass it to ICAM      */
       /*--------------------------------------------------------------------*/
       if (icam_encrypt_opts (passwd, K_PATH_SIZE, 0) &&
           build_icam_qualifier (ICAM_PASSWD, qualifier))
       {
          append_icam_qualifier_arg (qualifier, passwd, qualifier);
          cat_icam_qualifier_arg (options, qualifier);
       }
       else
       {
          dbg_err_syserr ("setup_post_create", "fail to get ptncpost passwd");
          ierror = PTC_E_ABORT;
       }

       if (ierror == E_NO_ERROR && options[0] != NULL_CHAR)
          setup_param_env_for_icam (options);
       else
       {
          dbg_err_syserr ("setup_post_create", "fail to set ptncpost option");
          ierror = PTC_E_ABORT;
       }
    }
    else if (is_ncpost() && !is_campost())
    {
       if (get_pro_ncpost_dbf (dbf_file))
       {
          if (build_icam_qualifier (ICAM_DB, qualifier))
          {
             append_icam_qualifier_arg (qualifier, dbf_file, qualifier);
             strcat (icam_quest, qualifier);
          }
       }
    }
    else if (is_gpost() && !is_campost())
    {
       if (get_gpostpp_dir_absolute (dbf_file, &can_delete) &&
	   pro_ncpost_dbf_exists (dbf_file))
       {
          if (build_icam_qualifier (ICAM_DB, qualifier))
          {
             append_icam_qualifier_arg (qualifier, dbf_file, qualifier);
             strcat (icam_quest, qualifier);
          }
       }


    }

    if (is_gpost())
      {
	if (build_icam_qualifier(GPOST_SY, qualifier))
	  {
	    cat_icam_qualifier_arg(options, qualifier);
	    strcat(icam_quest, qualifier);
	  }
      }

    if (ierror == E_NO_ERROR)
    {
       dbg_print_info ("setup_post_create", "Calling %s", icam_quest);
       {
          status = dcl_com(icam_quest);
       }
       /* blocks until finished */

       dbg_print_info("setup_post_create", "Post return status= %d", status );
       if (is_gpost() && status == 101)
          msgID_put (msg_ID767);
       /* Cannot access Pro/NCPOST executables,
          please check for proper setup. */

    }
    else
       status = FALSE;
 }
 else
 {
    dbg_err_syserr ("setup_post_create", "get_icam_quest_path_n_name() fails");
    status = FALSE;
 }

 if (is_gpost() && can_delete)
 {
   tmp_pfa = pfa_alloc_dir(dbf_file);
   pfa_delete_dir(tmp_pfa, TRUE, TRUE);
   pfa_free_pro_file (&tmp_pfa);
 }
 
 unset_post_env();
 return (status);
}

/*--------------------------------------------------------------------------*/
is_post_14_mfg_ui()
/*--------------------------------------------------------------------------*/
{
  Mfg  *mfg_ptr = get_cur_mfg();

 /* DISABLE new interface if 4321 run mode */
 if ( get_run_mode() == POST_14_MFG_UI_RUN_MODE )
   return ( FALSE );

 /* DISABLE new interface for all sheet metail processes */
 if ( ( mfg_ptr != NULL ) && is_smt_mfg( mfg_ptr ) &&
      ( get_run_mode() != POST_14_SMT_UI_RUN_MODE ) )
   return ( FALSE );

 /* ENABLE unless trail file older than E-07-07 */
 return ( !trl_vers_is_less( 804 ) );
}

/*--------------------------------------------------------------------------*/
is_post_14_smt_mfg_ui()
/*--------------------------------------------------------------------------*/
{
  Mfg  *mfg_ptr = get_cur_mfg();
  int   ret;

  if ( get_run_mode() == POST_14_MFG_UI_RUN_MODE ||
       trl_vers_is_less( 804 ) )
     ret = FALSE ;
  else
     ret = is_smt_mfg( mfg_ptr ) ;


  return (  ret  ) ;

#if 0
  return ( ( get_run_mode() == POST_14_MFG_UI_RUN_MODE ) &&
            is_smt_mfg( mfg_ptr ) ) ;
#endif
}


/* ======================================================================== */
is_post_14_manufact_ui ()
/* ------------------------------------------------------------------------ */
{
/*  if (get_run_mode() == POST_14_SMT_UI_RUN_MODE) */
    return (!trl_vers_is_less (804));

 /* return (is_post_14_mfg_ui()); */
}

