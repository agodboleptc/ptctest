#include <const.h>
#include <featcons.h>
#include <mfgpath.h>
#include <mfgparam.h>
#include <menucons.h>
#include <errors.h>
#include <mfg.h>
#include <pro_wstring.h>
#include <mfgcltrf.h>
#include <order.h>
#include <mfgcons.h>
#include <mfgconst.h>
#include <trl.h>
#include <pfa.h>
#include <sysexecmd.h>
#include <access.h>
#include <ft_misc_ch.h>
#include <utility.h>
#include <xarray.h>
#include <ent_type.h>
#include <proprintf.h>
#include <dbg_crash.h>
#include <displisttype.h>
#include <runmode.h>
#include <genfileext.h>
#include <manufact_msg.h>
#include <manufact2_msg.h>
#include <srch.h>
#include <AncppFuncDecl.h>
#include <menumgr_proto.h>
#include <nc_proc.h>
#include <cu_msgutil_proto.h>
#include <msgutil.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include <drmproto.h>
#include <MdlTree.h>
#include <ancpp_ui_proto.h>
#include <appl_tools_proto.h>
#include <chain.h>
#include <chnutil_proto.h>
#include <cldisp_proto.h>
#include <ct_misc_proto.h>
#include <ct_win_syscall_proto.h>
#include <ctcmdlg_proto.h>
#include <ctfileutil_proto.h>
#include <ctmisc_proto.h>
#include <ctstrutil_proto.h>
#include <ctsyscall_proto.h>
#include <cttime_proto.h>
#include <cu_cutils_proto.h>
#include <cu_fileutils_proto.h>
#include <cu_matrix_mt_proto.h>
#include <customize_proto.h>
#include <db_model_proto.h>
#include <draft_proto.h>
#include <drawutils_proto.h>
#include <feature.h>
#include <featutil2_proto.h>
#include <featutil_proto.h>
#include <g_featutil_proto.h>
#include <getputgl_proto.h>
#include <initial_proto.h>
#include <jobman_proto.h>
#include <jobmaninterface.h>
#include <mctool_proto.h>
#include <menusm_proto.h>
#include <mfg_cl_proto.h>
#include <mfg_proto.h>
#include <mfgcmds_proto.h>
#include <mfgutil_proto.h>
#include <millchn_proto.h>
#include <millpath.h>
#include <millutil_proto.h>
#include <mparam_proto.h>
#include <mpattern_proto.h>
#include <nc_seq_proto.h>
#include <ncl_proto.h>
#include <ncsequtil_proto.h>
#include <oper_cl_proto.h>
#include <operui_proto.h>
#include <pro_widec.h>
#include <runmode_proto.h>
#include <shtmparam_proto.h>
#include <solidutil_proto.h>
#include <sub_feat_proto.h>
#include <sysstdlib.h>
#include <tk_manufact_proto.h>
#include <tparam_proto.h>
#include <trajgen_proto.h>
#include <turn_proto.h>
#include <urec_utils_proto.h>
#include <utility_proto.h>
#include <valgebra.h>
#include <dashboard_proto.h>
#include <depend_proto.h>
#include <ndcapi.h>
#include <mfg2pro_t1.h>
#include <pro2mfg_proto.h>
#include <mtc_proto.h>
#include <db_funcs.h>
#include <mfg_util_proto.h>
#include <nc_check_proto.h>
#include <xarptr.h>
#include <pro11_msg.h>
#include <btkscale31.h>
#include <mfgptm_integ.h>
#include <moldmachin_proto.h>
#include <tessent_proto.h> 
#include <tool_motions_proto.h>

#define TAP_FILE_EXT ".tap"


static int done_wrapper(Part *part_ptr, Feat *feat_ptr);
static int mwsim_wrapper (Mfg *mfg_ptr, Part *part_ptr, Feat *feat_ptr);
static int cut_path_step(Sld_prt *part_ptr);
static int menu_cutpath_file(Mfg *mfg_ptr, Solid *part_ptr, Feat *oper_feat, wchar_t *out_fname, int save_option);
static int submitToJobManWrapper(Feat *feat_ptr, Part *part_ptr, int flag);
static int done_output(void);
static int get_ppname_from_command(char ppname[K_NAME_SIZE], wchar_t *ncl_file_name, Sld_prt *part_ptr, Bool list_all_pp);
static int get_pid_string(char *pidstring);
static int get_dblist_descriptor(wchar_t *ncl_file_name, Sld_prt *part_ptr, char *descriptor);

static wchar_t *get_pro_mf_cl_dir_absolute(void);

static int icam_read_dbfile(Pfa *pfa, wchar_t ***file_buffer, int line_size);
static void menu_cutpath_inaccess(feature_record *feat_ptr);
static int get_slice_num_from_user(feature_record *feat_ptr, int *rslice_num);
static int process_pp_error(int status);
static int is_fatal_pprocess_error(int status);
static int no_file_attr(int output_post_attr);
static int ncfm_post_tree_node_with_attr(int cl_file_attr, int post_attr);
static int mfg_clfile_out_exclu_seq_warn(Mfg*, Sld_prt*, Feat*, int*);

static Unit_rec	*cur_unit_rec_ptr = NULL;

typedef struct pp_error
{
int     status;
char    err_mes[K_LINESIZE];
} PP_error;

static PP_error pp_errors[] =
{
 {
   100,
   "Error occured opening the CL file"
 },

 {
   101,
   "Post-processor not found"
 },

 {
   -1,
   "Can not execute Post Processor"
 }
};



/*
   Date    Version    Author		Desc.
   ----    -------    ------            -----
 19-SEP-89  A-16-04   JMS  $$1 Created.
 20-OCT-89  A-16-15   Integ $$2 Removed decl for get_cur_sec
 26-OCT-89  A-16-15   JMS  $$3 Changed call. seq to create_cutter_..().
 29-NOV-89  A-16-25   Lev  $$4 Added arguments of create_cutter_loc_low .
 08-DEC-89  A-17-03   ASDM $$5 Added FT_GROOVE
 30-JAN-90  A-18-14   JMS  $$6 Added "Cutter Step".
 01-FEB-90  A-18-15   JMS  $$7 Moved call to setup_tool_display() into
			   cl_data_input().
 07-FEB-90  A-18-17   JMS  $$8 Externed cut_path_step(). Also set range[0].
 13-FEB-90  A-19-01   JMS      Added feat_ptr to get_ncl_file_name().
 26-FEB-90  A-19-03   JMS  $$9 Prompted user with option to display CL path
			   with/without tool. "File" doesn't display CL path.
 02-MAR-90  A-19-05   JMS $$10 Accomodated above change for mill operation.
 07-JUN-90  A-21-03   JMS $$11 Removed special cases for mill & groove.
 26-JUN-90  A-21-04  Integ $$12  Added PRO_WSTRING.H
 02-JUL-90  A-21-04   GGR  $$13 Changed calling seq of menu_cl_disp_type().
 06-SEP-90  A-21-25   JMS  $$14 Introduced slice creation modes.
 15-SEP-90  A-21-27   GGR  $$15 Call get_slice_num_from_user() in loop.
 27-SEP-90  A-22-02   GGR  $$16 Init dummy_file inside loop.
 27-Sep-90  A-22-03   ZMS  $$17 Changed decl size of file_name[].
 06-OCT-90  A-23-02   GGR  $$18 Store slice_num not slice_index.
 28-OCT-90  A-23-15   IlG  $$19 Added "Rotate" and "Translate".
 16-OCT-90  A-23-06   GGR  $$20 Added messages.
 07-NOV-90  A-24-03   JMS  $$21 Reset tool display flag for single slice disp.
 30-NOV-90  A-24-11   JMS  $$22 Checked for operation patterns.
 23-JAN-91  A-25-07   JMS  $$23 Changed call. seq. to cut_path_display().
 11-FEB-91  A-25-11   Bob  $$24 Changed call. seq. to menu_cl_disp_type().
 08-apr-91  a-26-05   duk  $$25 modified to handle FT_SMT_MFG_CUT and
                                FT_SMT_PUNCH_PNT
 13-may-91  a-26-08   duk  $$26 changed feat_is_cut_punch =>feat_is_smt_mfg_feat
 21-may-91  a-26-09   duk  $$27 changed calling seq of menu_cl_disp_type
 12-sep-91  a-27-17   duk  $$28 added cut_path_step_with_feat()
 01-OCT-91  A-28-05   GGR  $$29 Allow rotate and translate for manual.
 26-OCT-01  C-01-10   BSC  $$30 Called get_mfg_cl_lead_feat().
 27-dec-91  c-01-23   duk  $$31 output additional error message in cutpath_file
 07-JAN-92  C-01-24   JMS  $$32 Called get/set_tool_disp_flags().
 17-JAN-92  C-01-27   JMS  $$33 Removed dummy_file and passed NULL to
                                create_cutter_location().
 15-Jan-92  C-01-27   RWA  $$34 Add scale, mirror, output units.
                                Make rotate/translate available to all oper's,
                                except those that are patterned.
 22-APR-92  D-01-01   GGR  $$35 Remember last set units in unit_ptr.
                                Synced calls to get(set)_tool_disp_flags.
 24-JUN-92  D-01-09   GGR  $$36 Fixed uninitialized variable.
 29-JUN-92  C-02-23  Lee   $$37 Moved the static function declaration to the h
 24-AUG-92  D-01-22   MTP  $$38 Updated call seq of
                                create_file_cutter_location().
 11-nov-92  d-01-47   duk  $$39 Disable "Scale" and "Units" for smt mfg feats
 01-DEC-92  D-02-02   ITS  $$40 Moved set_env_feature_id() down to
                                create_cutter_loc_low().
 18-JAN-93  D-02-08   ITS  $$41 Store user spec. units info.
 19-JAN-93  D-02-08   GGR  $$42 Removed call to get_mfg_cl_lead_feat.
 27-JAN-93  D-02-08   GGR  $$43 Declared switch_to_leader.
 01-FEB-93  D-02-09   MTP  $$44 Extracted cut_path_step_low().
 01-FEb-93  D-02-09   GGR  $$45 Reinstated call to get_mfg_cl_lead_feat.
 16-MAY-93  D-02-31   MTP  $$46 Put message if toolpath display fails.
 17-Jun-93  E-01-01   CJL  $$47 Changed msg_put and msg_read to msg_get
 10-AUG-93  E-01-09   MTP  $$48 Cleanup all loops at end of output.
 25-AUG-93  E-01-12   GGR  $$49 Called is_nc_seq_feat..
 31-AUG-93  E-01-13   GGR  $$50 Changed call to is_nc_seq_cl_disp_mode.
 06-OCT-93  E-01-20   GGR  $$51 Make cutter step inaccessible here.
 15-OCT-93  E-01-22   GGR  $$52 Added "Done Output".
 19-OCT-93  E-01-24   GGR  $$53 Called reset_rot_trfs.
 09-FEB-94  E-03-09 Roni   $$54 check_order: No output if ORDER_DEMO
 06-Feb-94  E-03-07   dan  $$55 If feat is smt mfg, remove ncllp from assoc
				objs only.
 08-MAR-94  E-03-15   ITS  $$56 Did #55 for smt operation.
 10-MAR-94  E-03-15   GGR  $$57 Multi window stuff.
 11-MAR-94  E-03-15  ILBS  $$58 Added featcons.h
 07-Apr-94  E-03-20  dan   $$59 Added filter to smt_add_or_rm_ncl_from_view().
 19-Apr-94  E-03-22  dan   $$60 menu_cutter_path() now always release the
                                chain loops.
 21-APR-94  E-03-23   GGR  $$61 Called set_part_max_cut_step.
 25-MAY-94  E-03-27   GGR  $$62 Called rem_nc_seq_final_tpath_loop
 21-JUN-94  E-06-02   yf   $$63 Added call to promenu_get_value.
 24-JUN-94  E-06-02   yf   $$64 Added cutpath_file_create().
 24-JUN-94  E-06-02   yf   $$65 resolved comments complaints.
 27-JUL-94  E-06-04   yf   $$66 New interface for Icam.
 02-AUG-94  E-06-05   GGR  $$67 Remove all raw loops.
 15-AUG-94  E-06-07   GGR  $$68 Called cleanup_all_feat_assoc_cloops
 28-AUG-94  E-06-09   MAX  $$69 Push Display command on stack.
 06-SEP-94  E-06-10   MAX  $$70 NCL_FILE name for OPERATION.
 31-AUG-94  E-06-09   LMh  $$71 Removed msg if ierror == E_INVALID_OPER in
                                cutpath_file_create().
 19-OCT-94  E-06-15   EY   $$72 Off kerf display after toolpath display.
 11-NOV-94  E-06-18   LMh  $$73 Modified menu_cutpath_inaccess().
 22-MAR-95  E-07-09   GYU  $$74 Fixed get_cur_units_ptr().
 21-FEB-95  E-07-09   LUK  $$75 Added remove_ncllp_after_cl_output.
 11-Apr-95  E-07-08 mrayko $$76 Removed msg if ierror == E_INVALID_OPER in
                                cutpath_display().
 05-APR-95  E-07-10   LUK  $$77 Fixed logic in should_free_tp_in_memory().
 05-MAY-95  E-07-13   PBO  $$78 Cleanup of transf params for all milling
                                operations not just sheet metal in
                                menu_cutter_path().
 10-MAY-95  E-07-15   LUK  $$79 Externed remove_ncllp_after_cl_output().
 12-JUN-95  E-07-18   yf   $$80 Campost should not depend on system.
 12-JUN-95  E-07-18   LUK  $$81 Changed menu_cl_disp_type() call. seq.
17-Aug-95 G-01-03 jmichaud $$82 Use XAR_COUNT() and BYTCPY() macros.
 12-Sep-95  G-01-07   GAA  $$83 Changed calls to pfa_get_... (thread safe).
 07-NOV-95  G-01-12   LUK  $$84 Handles ncpost license.
 11-NOV-95  G-01-13   LUK  $$85 Called allow_campost() in cutpath_file().
 25-NOV-95  G-01-15   LUK  $$86 Removed opt accessible check for icam.
 10-DEC-95  G-01-17 mrayko $$87 Modified remove_ncllp_after_cl_output() for
                                smt output.
 12-DEC-95  G-01-17   LUK  $$88 Setup "param" environment variable for icam.
                                Stopped calling remove_ncllp_after_cl_output().
 20-DEC-95  G-01-18   LUK  $$89 Replaced pro_read_file_to_buffer() with
                                pro_read_file_to_buf_low().
 15-JAN-96  G-01-19   LUK  $$90 Don't abort if (1 <= error type < 16)
 22-Jan-96  G-03-01 jmichaud $$91 Include header for pro_printf() varargs
                                  funcs. Include header for dbg_err_xxx().
 26-JAN-96  G-03-02   LUK  $$92 Append temp file extension and add delay().
 24-FEB-96  G-03-04   LUK  $$93 Skip mcd check mark menu for cmm.
 05-FEB-96  G-03-05   dpek $$94 Added remote cl computation
 17-MAR-96  G-03-06   LMh  $$95 Used smt_cleanup_after_cl_output().
 20-MAR-96  G-03-07   JMH  $$96 Extracted menu_cutter_path_pre_ui() and
                                menu_cutter_path_post_ui(); externed
                                cutpath_display().
 22-MAR-96  G-03-07   LMh  $$97 Used reset_toolpath_dl() for smt.
 08-APR-96  G-03-10   mrayko $$98 Removed ##97.
 18-APR-96  G-03-11   dpek $$99 Added no_file_attr()
 13-MAY-96  G-03-13   LUK  $$100 Updated get_dblist_descriptor() for cmm.
 13-JUN-96  G-03-16   ELB  $$101 changed usage of get_workcell_type for new param
 12-JUL-96  H-01-02   mkh  $$102 Changed menu_cutter_path_post_ui().
 26-JUL-96  H-01-03   LUK  $$103 Added pro_wait for 95. Added pt_ncpost.
 05-SEP-96  H-01-08   LUK  $$104 Added more debug code.
 13-SEP-96  H-01-08   LUK  $$105 Added set_icam_debug_runmode().
 02-OCT-96  H-01-12   LUK  $$106 Called icam_read_dbfile() in a loop if fails.
 15-Oct-96 H-01-13  EEB/MRC $$107 Allow icam debugging.
 04-DEC-96  H-01-19   mkh  $$108 Called get_outset_from_feat_misc_data().
 11-DEC-96  H-01-21   LUK  $$109 Try to open dblist file 3 times for unix.
 06-JAN-97  H-01-22   LUK  $$110 Removed pro_wait.  Replace dcl_com() with
                                 pro_system_call_new() for 95 case.
 10-JAN-97  H-01-24   mkh  $$111 Called get_output_set_name_from_oper().
 20-MAR-97  H-03-05   MYO  $$112 used switch_mz_data() in menu_cutter_path().
 27-MAR-97  H-03-05   LUK  $$113 Give msg if cannot read header.
 27-MAR-97  H-03-13   QBQ  $$114 added saveas ui.
  11-Jun-97 H-03-13+ DAJ    $$115 Replace API for messages with ID-base one
 12-JUN-97  H-03-15   QBQ  $$116 update saveas ui.
 12-JUL-97  H-03-17   QBQ  $$117 cmm ui update.
 12-JUL-97  H-03-20   QBQ  $$118 change msg output for cmm.
 16-SEP-97  H-03-21   mkh  $$119 Called rem_final_tpath_loops_in_set() and
                                 rem_nc_seq_final_tpath_loop() in
                                 cutpath_display().
 28-SEP-97  H-03-24   mkh  $$120 Undo #119.
 01-OCT-97  H-03-25   LUK  $$121 Don't convert to lower case of tmp file.
 20-OCT-97  H-03-27   MB   $$122 Called new_cutcom_info
 21-Oct-97  H-03-27   RR   $$123 Made get_ppname_from_command platform
                                 independent; post processing to be done
                                 even if cl file is invalid
 12-OCT-97  H-03-27   QBQ  $$124 added user define ext type for ncl saveas.
 24-Oct-97  H-03-28   RR   $$125 Added "C" option in get_dblist_descriptor;
                                 display all PP if Post Proc is chosen.
 25-Nov-97  H-03-32   RR   $$126 If no PP found print an error.
 15-Dec-97  H-03-33 Anatoli  $$127  Handling of SMT in get_dblist_descriptor().
 11-Jan-98  H-03-36  MUU   $$128 Changed cutpath_file() - warning msg for SMT.
 20-Jan-98  H-03-37   RR   $$129 Check creation of .tap file in exec_cl_post().
 20-JAN-98  H-03-37  integ $$130 Removed smtuid.h
 11-Feb-98  H-03-38+   RR   $$131 Used pro_exec_command instead of pro_system_
                                  call_new in Win '95.
 12-Feb-98  H-03-38++  RR   $$132 Fixed compilation problem in SGI
 27-JUL-98  I-01-15   MB   $$133 Calling seq of cut_path_display
 21-JUL-98  I-01-15 mrayko $$134 Removed  old smt mfg stuff.
 18-AUG-98  I-01-17     prf $$135 called play_batch_file for WINDOWS_95
 18-AUG-98  I-01-17     MB $$136 Calling sequence of cutpath_display.
 06-OCT-98  I-01-21   mkh  $$137 Called remove_loop_from_ncseq_misc_data()
                                 in remove_ncllp_after_cl_output().
 05-OCT-98  I-01-21    MB   $$138 Calling sequence of create_file_cutter_loc
 19-OCT-98  I-01-23    MB  $$139 Called cl_post from cdplayer.
 11-Nov-98  I-01-26    RR  $$140 get_dblist_descriptor() modified for SMT.
 01-FEB-99  I-03-03    MB  $$141 save, save_as and save_as_mcd.
 10-APR-99  I-03-07   mkh  $$142 Called clean_up_tool_disp_stat_loop_map().
 06-JUN-99  I-03-10    MB  $$143 Added Clmaps to cal. seq.
 20-JUN-99  I-03-10    MB  $$144 create_cutter_loc_w_clmaps.
 08-Jul-99  I-03-11    RR  $$145 Enabled GPOST.
 15-JUN-00  J-01-10   mkh  $$146 Added "Show File" in menu_cutter_path() for
                                 cl archive.
 10-JUL-00  J-01-12   mkh  $$147 Disallow "Compute CL" preselection in
                                 cutpath_display() and menu_cutpath_file().
 27-Jun-00  J-01-12   HMR  $$148 Explicit lowercase of file names.
 14-JUL-00  J-01-12+  mkh  $$149 Called prep_feat_for_recompute_cl().
 14-Dec-00  J-01-24   MBE  $$150 Consolidate spin num version access.
 17-MAY-01  J-01-32+  mkh  $$151 Added stat_final_loop.
 26-JUN-01  J-03-02   mkh  $$152 Added  release_stat_final_loop().
 20-Jun-01  J-03-02 jmichaud $$153 pro_get_mdate() uses 'time_t *'
                                   Fix compiler warnings.
 21-Sep-01  J-03-09   jas  $$154 Removed WINDOWS_95 macro
 07-Feb-02  J-03-19   SAN  $$155 Modified set_stat_final_loop().
 30-Apr-02  J-03-24   mkh  $$156 Modified set_stat_final_loop().
 01-Jul-02  J-03-28   MDA  $$157 Supported save in WS in cutpath_file_create()
 22-Nov-02  J-03-38   AKG  $$158 Added copy_cl_file_to_cwd_dir().
 20-Feb-03  K-01-01   VAD  $$159 Called update_ugc_tp_files() in cut_path_display(); 
 27-Feb-03  K-01-02   VAD  $$160 Added submitToJobManWrapper(), done_output(). 
 06-Feb-03  K-01-03   aap  $$161 Translate menu title.
 20-Mar-03  K-01-03   VAD  $$162 Modified nc_jobman_display_toolpath().
 07-Apr-03  K-01-04   AKG  $$163 Added cdplayer for operation
 19-May-03  K-01-07   MDA  $$164 Removed extra declarations
 20-Jun-03  K-01-09   AKG  $$165 Warning for ncl file saved, when Shadedtool on 
 03-Nov-03  K-01-17   AKG  $$166 Called unlink_shaded_memb() and Undo #165
 15-Dec-03  K-01-20   AKG  $$167 Donot launch cdplayer form cmm_feat
 02-Feb-04  K-01-23   AKG  $$168 Added reset_stat_final_loop().
 09-Mar-04  K-01-24+  VAJ  $$169 Regfail fix.'Collision' menu not shown for cmm.
 14-APR-04  K-01-26   MTP  $$170 Use NULL instead of zero
 19-Jul-04  K-03-06   tgupta  $$171 If pro_mf_cl_dir is defined, save 
                                    ncl file there.
 21-sep-04  K-03-10   jparakal  $$172 handling pro_mf_cl_dir config
                                      option corrected.
 23-Sep-04  K-03-11   gen       $$173 Updated for Process Manager.
 25-Nov-04  K-03-15   AW        $$174 Fixed UMR errors cause by missing func args
 06-Dec-04  K-03-15   agupta    $$175 Called get_cmm_clp..().
 19-Jan-05  K-03-18   agupta    $$176 Modified cutpath_display().
 22-Feb-05  K-03-20   agupta    $$177 Launch old ui for multiple head 
                                      simulation, not the cdplayer.
 05-May-05  K-03-24   agupta    $$178 Added is_file_from_workspace() #1127709.
 09-Jun-05  K-03-26   agupta    $$179 Modified is_file_from_work...().
 10-Jul-05  K-03-28   ksaraswa  $$180 Modified arguement in remote_create_file_cutter_location().
 05-Oct-05  K-03-33   gen  $$181 Stopped resetting a rotation transform when 
                                 cdplayer is on.
 13-Mar-06  L-01-04   tgupta    $$182 Added collision option for cmm.
 11-Jun-06  L-01-10   Asaf      $$183 Fixed Prototypes for get_rid_of_special_characters
 26-Jun-06  L-01-11   agupta    $$184 Updated for cmm file for mach. simul.
 12-Jul-06  L-01-12   ksi       $$185 Unicode compliant changes
 10-Jul-06  L-01-13   HMR       $$186 Called parse_filename_buffer().
 21-Jul-06  L-01-13   tgupta    $$187 Addition to remember collision option.
 05-AUG-06  L-01-14   agupta    $$188  Called get_tool_changed().
 20-Sep-06  L-01-16   tgupta    $$189  Add check for cmm mfg.
 25-Sep-06  L-01-17   tgupta    $$190  Use cmm specific message for cmm file.
 10-Oct-06  L-01-18   tgupta    $$191  Get cmm file name using
                                       get_feat_user_name for all cases.
 27-Oct-06  L-01-19   arawat    $$192  Enabled Collision checking for CMM
 				       operation.
 31-Jan-07  L-01-25  gganeriw   $$193  Added machine play menu.
 26-Feb-07  L-01-27  gupatil    $$194  Issue related to cl data output of mirror seq.
 26-Feb-07  L-01-27  ksamanta   $$195  Updated args.
 13-Apr-07  L-01-30  tgupta     $$196  New collision UI for cmm.
 11-May-07  L-01-31+ ksamanta   $$197  Removed "Machine Play" menu.
 29-May-07  L-01-32  agupta     $$198  Enabled cdplayer for mill/turn wcell.			       
 28-Jun-07  L-01-34  nprakash   $$199  Updated menu_cutter_path for CMM.
 02-Aug-07  L-01-35  nprakash   $$200  Updated menu_cutter_path for CMM.
 30-Aug-07  L-01-38  agupta     $$201  Added DRM check for Jobman functionality.
 09-Oct-07  L-01-41  aabramychev $$202 Modified cutpath_file_create().
 17-Dec-07  L-01-41  nprakash   $$203 Fixed cdplayer_cutpath_display(().
 20-Mar-08  L-03-05  PROTO      $$204 Automatic prototype creation
 08-Jul-08  L-03-12+ vkarakulin $$205 Updated for MTC.
 27-Aug-08  L-03-16+ vkarakulin $$206 Fixed SPR 1518962.
 03-Nov-08  L-03-19  aargade    $$207 Updated for copying tap file to wspace.
 17-Nov-08  L-03-20  aargade    $$208 Updated for mfg deliverables
 23-Dec-08  L-03-23  aargade    $$209 Updated for T_ASSEMBLY instead of T_MFG
 14-Jan-09  L-03-23  aargade    $$210 Updated for tap file extension and message name.
 19-Jan-09  L-03-24  aargade    $$211 Added argument to cutpath_file_create().
 08-Jan-09  L-03-24  aargade    $$212 Default post "uncx01.p"00 for Expe
 03-Feb-09  L-03-25  aargade    $$213 Called is_ncexpert().
 03-Feb-09  L-03-25  aargade    $$214 Updated for ncl/tape dep on mfg
 03-Mar-09  L-03-27  aargade    $$215 Fixed the only tape file creation
 17-Mar-09  L-03-28  aargade    $$216 Updated cutpath_file_create &
                                       is_file_from_windchill()etc
 01-Sep-10  L-05-30  magarwal   $$217 Updated for CMM file name in message.
 13-Dec-10  L-05-40  rbokil     $$218 Added create_ppname_for_machine_attr().
 28-JAN-11  L-05-42  nkhedkar  $$219 Mfg Modularization Work
 24-Mar-11  L-05-44  mkh       $$220 Modified cdplayer_cutpath_display().
 08-Dec-11  P-10-14  rbokil    $$221 Updated for jobman.
 07-Feb-12  P-10-17  rbokil    $$222 Modified create_ppname_for_machine_attr().
 28-Mar-12  P-20-02  rbokil    $$223 Updated for copying tap file to wspace.
 06-Nov-12  P-20-17  AC        $$224 Prototype compliance.
 18-Jun-13  P-20-32  rbokil    $$225 Removed ptncpost_dbf & ncseq_outbnd_curve.
 08-Oct-13  P-20-40  rbokil    $$226 Updated cutpath_file_create.
 13-Jan-14  P-20-55  rbokil    $$227 Updated exec_cl_post().
 11-Nov-14  P-20-64  rbokil    $$228 Updated write_to_wsp_process_name().
 12-Jan-15  P-20-64  NPR       $$229 Updated for excluded sequence warning.
 16-May-16  P-30-33  anath     $$230 Added isGenerateCorrectToolpath(), getFeat().
 20-Jul-17  P-50-21  PD	       $$231 Used get_feat_user_name_with_size
 17-Sep-19  P-70-27  ngarad    $$232 Updated for multibody solid tool/adapter
 27-Nov-20  P-80-33  nisaxena  $$233 Updated for static warnings.
 02-Dec-21  P-90-37  Ahmad     $$234 scrambled literal env vars
 07-Mar-22  Q-10-02  ksingh    $$235 Updated for MW CL player for synch
 23-Mar-22  Q-10-05  YKI       $$236 Updated write_to_wsp_process_name().
 19-Apr-22  Q-10-08 svrs       $$237 Added case for ProCmdMfgOpToolPath 
 01-Jun-22  Q-10-13  ksingh    $$238 Updated dev config rt_13601993 to mfg_new_cl_player_for_sync.
 27-Nov-22  Q-10-37  svrs   $$239  Updated for 4axis turning
 29-Oct-23  Q-11-36  ewa    $$240  Called get_full_model_name_for_display().
 27-Feb-24  Q-12-03  svrs     $$241  Updated for 4axis turning
 19-Mar-24  Q-12-07  svrs     $$242 Updated for auto compute stock model
 11-Apr-24  Q-12-08  svrs     $$243 corrected reset for stock model
 27-Feb-24  Q-12-03  svrs     $$244  Updated for 4axis turning

 19-Mar-24  Q-12-07  svrs     $$245 Updated for auto compute stock model
 16-Apr-24  Q-12-08  ngarad   $$246 Renamed mfg_mbody_is_supported
 24-Apr-24  Q-12-09  ngarad   $$247 Renamed nc_seq_has_mbody_tool_or_adapter
 25-Aug-24  Q-12-27  DevOps   $$248 Use standard wide string functions
 17-Dec-24  Q-12-44  ngarad   $$249 Updated mfg_has_unsupported_components argument
 07-Aug-25  Q-13-32  ibl      $$250 Used API to access solid outline
 24-Dec-25  Q-13-40  ksingh   $$251 Updated mfg_get_excluded_seq_from_list for missing stl files.

*/


/*---------------------------------------------------------------------------*/
int  menu_cutter_path_pre_ui( part_ptr, feat_ptr, p_slice_cr_mode,
                              trf, unit_rec_ptr, p_model_unit_ptr )
/*---------------------------------------------------------------------------*/
 Part      *part_ptr;
 Feat      *feat_ptr;
 int       *p_slice_cr_mode;
 Trf       trf;
 Unit_rec  *unit_rec_ptr;
 Unit_rec  **p_model_unit_ptr;
{
 *p_slice_cr_mode = get_slice_cr_mode();
 set_part_max_cut_step( part_ptr );

 crl_transf_params( feat_ptr->id );

 if ( is_cdplayer_on() )
   set_ncl_mirror_transforms( NULL, NULL );
 else
   reset_rot_trfs();
 copy_matrix( NULL, trf );

 init_unit_rec( unit_rec_ptr );
 if ( !get_model_unit_ptr( part_ptr, p_model_unit_ptr ) )  {
   *p_model_unit_ptr = NULL;
   dbg_err_crash( "menu_cutter_path", "unable to locate model units." );
 }
 else
   copy_unit_rec( *p_model_unit_ptr, unit_rec_ptr );

 cur_unit_rec_ptr = unit_rec_ptr;

 return ( E_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
int  menu_cutter_path_post_ui( mfg_ptr, part_ptr, feat_ptr, slice_cr_mode )
/*---------------------------------------------------------------------------*/
 Mfg		*mfg_ptr;
 Sld_prt	*part_ptr;
 Feat  *feat_ptr;
 int   slice_cr_mode;
{
 wchar_t    *set_name;

 if( is_cdplayer_on() ) 
     set_rotation_transform( NULL );
 else
     reset_rot_trfs();
 
 cur_unit_rec_ptr = NULL;
 put_slice_cr_mode( slice_cr_mode );
 set_cutter_step( -1.0 );

 if( get_output_set_name_from_oper ( feat_ptr, &set_name) )
 {
   if ( !is_cdplayer_on() )
      clean_up_after_output_by_set ( mfg_ptr, part_ptr, set_name);
 }
 else
   remove_ncllp_after_cl_output ( feat_ptr);

 crl_transf_params( feat_ptr->id );

 return ( E_NO_ERROR );
}

/**********************************************************************\
* stat_final_loop created by output_ncs_oper_cl_data() can be set      *
* only if two conditions are met:                                      *
* 1. tph mechanism is not used - !is_store_tp_file_opt_set();          *
* 2. cd player is not used - !is_cdplayer_on().                        *
*
\**********************************************************************/
static int stat_loop_storage_allowed = FALSE;
static Ncloop *stat_final_loop = NULL;

/*=========================================================================*/
 static int set_compute_cl()
/*-------------------------------------------------------------------------*/
{
 int   flag = is_disp_force_compute_cl ();

 set_disp_force_compute_cl (!flag);
 return ( flag );
}

/*=========================================================================*/
 static int set_detect_collision()
/*-------------------------------------------------------------------------*/
{
 int   flag = is_disp_detect_collision ();

 set_disp_detect_collision (!flag);
 upd_cdp_coll_check_stop_btn( !flag );
 return ( flag );
}

/*=========================================================================*/
static int machine_cutpath_display( mfg_ptr, part_ptr, feat_ptr )
/*-------------------------------------------------------------------------*/
Mfg             *mfg_ptr;
Sld_prt         *part_ptr;
feature_record  *feat_ptr;
{
  int val;
  val = set_mfg_machine_sim_on( TRUE );
  cdplayer_cutpath_display( mfg_ptr, part_ptr, feat_ptr );
  set_mfg_machine_sim_on( val );
}

/*=========================================================================*/
int cdplayer_cutpath_display( mfg_ptr, part_ptr, feat_ptr )
/*-------------------------------------------------------------------------*/
Mfg		*mfg_ptr;
Sld_prt         *part_ptr;
feature_record  *feat_ptr;
{
  static char  *cl_menus[] = { "PLAY PATH", "DONE SUB", "" };
  static char  *cl_menus_mtc[] = { "PLAY PATH", "EXIT", "" };
  int	       display_cdp, is_cmm;
  int          show_comp_cl_option = TRUE, turn_4ax_exist=0;

  /*no need to check for cmm sequence or opertion, this check is good enough*/
  is_cmm = is_cmm_mfg(get_cur_mfg());

  display_cdp = display_cdp_for_oper ( mfg_ptr, part_ptr, feat_ptr, &turn_4ax_exist);
	
  if ( display_cdp )
  {
   if ( !is_cmm && use_mfg_top_ui_cmd() && in_ribbon_ui() )
   {
     /******************************************************************/
     /*  New logic introduced with ribbon UI !!!!                      */
     /*  There are two separate buttons: "Play", "Recompute and play". */
     /******************************************************************/
     if ( current_mtc_is( "ProCmdMfgPlayPath" ) ||
          current_mtc_is( "ProCmdMfgRecomputePlay" ) )
     {
       if ( current_mtc_is( "ProCmdMfgRecomputePlay" ) )
         set_disp_force_compute_cl( TRUE );
       
       cdplayer_( part_ptr, feat_ptr, K_NOT_USED );

       set_disp_force_compute_cl( FALSE );

       return ( 0 );
     }
   }

   promenu_create ( "PLAY PATH", "comp_cl.mnu" );
 
   promenu_on_button ( "PLAY PATH", "Compute CL", set_compute_cl, NULL, 0);
   promenu_on_button ("PLAY PATH", "Collision", set_detect_collision, NULL, 0);
   promenu_on_button ("PLAY PATH", "PLAY PATH", promenu_exit_up, NULL, 0);
   promenu_set_data_mode ( "PLAY PATH", TRUE );
 
   /*if it is not cmm feat(sequence and operation) disable collision*/
   if( !is_cmm )
   	promenu_set_item_visible ("PLAY PATH", "Collision", FALSE);

   if ( use_mfg_top_ui_cmd() && current_mtc_is ("ProCmdMfgPlayPath") )
   {
      promenu_create ( "EXIT", "exit.mnu" );
      promenu_load_action ( "EXIT", "Done", done_wrapper, part_ptr, feat_ptr,
                            NULL, NULL, NULL, NULL );
      promenu_load_action ( "EXIT", "Quit", promenu_exit_action_up2, NULL, PTC_E_ABORT,
                            NULL, NULL, NULL, NULL );
      promenu_on_button ( "EXIT", "EXIT", promenu_exit_up, NULL, 0 );

      promenu_make_compound(cl_menus_mtc);
   }
   else
   {
      promenu_create ( "DONE SUB", "done_sub.mnu" );
      promenu_load_action ( "DONE SUB", "Done", done_wrapper, part_ptr, feat_ptr,
                            NULL, NULL, NULL, NULL );
      promenu_on_button ( "DONE SUB", "DONE SUB", promenu_exit_up, NULL, 0 );

      promenu_make_compound(cl_menus);
   }

   if( is_cmm && is_disp_detect_collision() )
      promenu_set_item ( "PLAY PATH", "Collision" );

  promenu_action(cl_menus[0]);
  return ( 0 );
  }
  else
  {
    if (get_mfg_new_cl_player_for_sync() &&
         ( current_mtc_is( "ProCmdMfgClOutput" ) ||
         current_mtc_is( "ProCmdMfgOpToolPath" ) ) &&
        ( feat_oper_has_bldoper_synch_seqs(part_ptr, feat_ptr) || turn_4ax_exist) )
    {
       promenu_create ( "PLAY PATH", "comp_cl.mnu" );
 
       promenu_on_button ( "PLAY PATH", "Compute CL", set_compute_cl, NULL, 0);
       promenu_on_button ("PLAY PATH", "Collision", set_detect_collision, NULL, 0);
       promenu_on_button ("PLAY PATH", "PLAY PATH", promenu_exit_up, NULL, 0);
       promenu_set_data_mode ( "PLAY PATH", TRUE );

      promenu_create ( "DONE SUB", "done_sub.mnu" );
      promenu_load_action ( "DONE SUB", "Done", mwsim_wrapper, mfg_ptr, part_ptr, feat_ptr,
                             NULL, NULL, NULL );
      promenu_on_button ( "DONE SUB", "DONE SUB", promenu_exit_up, NULL, 0 );

      promenu_set_item_visible ("PLAY PATH", "Collision", FALSE);

      promenu_make_compound(cl_menus);
      return ( 0 );
    }

    if ( get_mfg_new_cl_player_for_sync() &&
         ( current_mtc_is( "ProCmdMfgPlayPath" ) ||
          current_mtc_is( "ProCmdMfgRecomputePlay" ) ) &&
        (feat_oper_has_bldoper_synch_seqs(part_ptr, feat_ptr) || turn_4ax_exist))
    {
       int one_seq = FALSE,sel_num =0;
       int *feat_ids = NULL;
    	
       if(get_selected_NCFeat_Id(&feat_ids))
       
         sel_num = XAR_COUNT (&feat_ids);
         if( (sel_num == 1) && (!check_mfg_oper_feat ( part_ptr, feat_ids[0] )))
            one_seq =  TRUE;
     	nc_simulate_tool_path ( mfg_ptr, part_ptr,
         	                      feat_ids, one_seq, NC_MW_SYNC_PLAYPATH);
     }
     else           
    cutpath_display( mfg_ptr, part_ptr, feat_ptr, NULL );
    return ( 0 );
  }
}

/*---------------------------------------------------------------------------*/
int cdplayer_cutpath_file (mfg_ptr, part_ptr, feat_ptr, filename, save_option)
/*---------------------------------------------------------------------------*/
Mfg		*mfg_ptr;
Sld_prt		*part_ptr;
feature_record	*feat_ptr;
wchar_t   filename[K_PATH_SIZE];
int             save_option;
{
  int		ierror;
  
  if( get_mfg_clfile_excl_seq_warn() )
  {
     int	user_resp = 0;
     (void)mfg_clfile_out_exclu_seq_warn (mfg_ptr, part_ptr, feat_ptr, &user_resp);
     if( user_resp == 2 )
     {
	return PTC_E_ABORT;
     }
  }

  if ( get_shaded_tool_disp_flag() )
    unlink_shaded_memb();

  ierror = cutpath_file( mfg_ptr, part_ptr, feat_ptr, filename, save_option );

  if ( get_shaded_tool_disp_flag() && !get_tool_changed() )
    link_shaded_memb();

  if(  !is_mfg_machine_sim_on() && is_cmm_feat (feat_ptr) 
        && !is_cmm_meas_from_scan(feat_ptr))
 /*  Avoiding crash while saving ncl file of cmm meas scan seq.*/
  {
	/*Patch for displaying the probe properly which gets reset by the
	  call of cutpath_file*/

   	Sensor_data		*sensor_data;
	Tool			*tool_ptr;

   	get_env_sensor_data (&sensor_data);
	get_tool_from_mfg (get_cur_mfg(), sensor_data->tool_id, &tool_ptr);
	set_tool_tip_pitch_roll (tool_ptr, sensor_data->tip_num,
		sensor_data->pitch, sensor_data->roll);
	setup_n_regen_probe (part_ptr, tool_ptr);
	unset_tool_tip_pitch_roll (tool_ptr);
	cdp_repaint ();
  }

  return ierror;
}

/*---------------------------------------------------------------------------*/
int menu_cutter_path (mfg_ptr, part_ptr, feat_ptr, filename, save_option)
/*---------------------------------------------------------------------------*/
/*
  this is the menu interface routine for the creation of cutter location.
*/
Mfg		*mfg_ptr;
Sld_prt		*part_ptr;
feature_record	*feat_ptr;
wchar_t   filename[K_PATH_SIZE];
int             save_option;
{
 static char  *menus[] = { "PATH", "JOBMAN", "", "" };

 static int cl_trf_rotate    = CL_TRF_ROTATE;
 static int cl_trf_translate = CL_TRF_TRANSLATE;
 static int cl_trf_scale     = CL_TRF_SCALE;
 static int cl_trf_mirror    = CL_TRF_MIRROR;
 static int cl_trf_selunits  = CL_TRF_SELUNITS;
 static int any_axis         = 0;

 int		p_slice_cr_mode = K_NOT_USED;
 Unit_rec	unit_rec_str, *model_unit_ptr = NULL;
 double 	trf[4][3], *p_trf;
 int		ierror;
 double         mir_part_to_assem[4][3], mir_plane_lcl_sys[4][3];
 int            use_mirror_trf = get_ncl_use_mirror_trf();

 set_stat_final_loop( NULL );
 allow_stat_final_loop( FALSE );
 if ( is_cdplayer_on() && use_mirror_trf )
     get_ncl_mirror_transforms( mir_part_to_assem, mir_plane_lcl_sys );

 menu_cutter_path_pre_ui( part_ptr, feat_ptr, &p_slice_cr_mode,
                          trf, &unit_rec_str, &model_unit_ptr );

 p_trf = &trf[0][0];

 if ( is_cdplayer_on() )
 {
   ierror = cdplayer_cutpath_file (mfg_ptr, part_ptr, feat_ptr, filename, save_option);

   if( use_mirror_trf )
       set_ncl_mirror_transforms( mir_part_to_assem, mir_plane_lcl_sys );

   goto END;
 }

 if ( !is_store_tp_file_opt_set() )
   allow_stat_final_loop( TRUE );

 promenu_create ("PATH", "cut_path.mnu");

 if ( get_clplayer_visible() && !is_cmm_mfg(mfg_ptr) )
    promenu_load_action ("PATH", "Display", cdplayer_cutpath_display,
                          mfg_ptr, part_ptr, feat_ptr,NULL,NULL,NULL);
 else if ( get_ncfm_ui_for_cmm() && is_cmm_mfg(mfg_ptr) )
    promenu_load_action ("PATH", "Display", cdplayer_cutpath_display,
                          mfg_ptr, part_ptr, feat_ptr,NULL,NULL,NULL);
 else
    promenu_load_action ("PATH", "Display", cutpath_display,
                         mfg_ptr, part_ptr, feat_ptr,NULL,NULL,NULL);

 promenu_load_action ("PATH", "Show File", screenui_show_file,
                      part_ptr, feat_ptr, FALSE,NULL,NULL,NULL);

 promenu_load_action ("PATH", "File", menu_cutpath_file,
                      mfg_ptr, part_ptr, feat_ptr, NULL,NULL,NULL);

 promenu_load_action ("PATH", "Rotate", trf_cl_data,
                      part_ptr, feat_ptr, &unit_rec_str, p_trf,
                      &cl_trf_rotate, NULL);

 promenu_load_action ("PATH", "Translate", trf_cl_data,
                      part_ptr, feat_ptr, &unit_rec_str, p_trf,
                      &cl_trf_translate, NULL);

 promenu_load_action ("PATH", "Scale", trf_cl_data_axis,
                      part_ptr, feat_ptr, &unit_rec_str, &any_axis, p_trf,
                      &cl_trf_scale);

 promenu_load_action ("PATH", "Mirror", trf_cl_data_axis,
                      part_ptr, feat_ptr, &unit_rec_str, &any_axis, p_trf,
                      &cl_trf_mirror);

 promenu_load_action ("PATH", "Units", trf_cl_data_axis,
                      part_ptr, feat_ptr, &unit_rec_str, &any_axis, p_trf,
                      &cl_trf_selunits);

 promenu_on_button ("PATH", "Cutter Step", cut_path_step, part_ptr,0);
 /* promenu_on_button ("PATH", "Done Output", promenu_exit_up, NULL,0); */
 promenu_on_button ("PATH", "PATH", promenu_exit_up, NULL,0);

 if ( model_unit_ptr == NULL )
    promenu_set_item_visible ( "PATH", "Units", FALSE );
 if ( is_nc_seq_cl_disp_mode () )
    promenu_set_item_visible ( "PATH", "Cutter Step", FALSE );

 if (( is_cmm_feat(feat_ptr) && should_bringup_newcmm() ) ||
     is_cl_archive_static_set(NULL) )
  {
    promenu_set_item_visible ("PATH", "Rotate", FALSE);
    promenu_set_item_visible ("PATH", "Translate", FALSE);
    promenu_set_item_visible ("PATH", "Scale", FALSE);
    promenu_set_item_visible ("PATH", "Mirror", FALSE);
    promenu_set_item_visible ("PATH", "Units", FALSE);
  }

 if ( is_cl_archive_static_set(NULL) )
   promenu_set_item_visible ("PATH", "Display", FALSE );
 else
   promenu_set_item_visible ("PATH", "Show File", FALSE );

 if ( !is_cl_archive_static_set(NULL) )
   push_command_on_stack ("Display", 0);


 menu_cutpath_inaccess (feat_ptr);

 promenu_create ("JOBMAN", "jobman.mnu");

 promenu_load_action ("JOBMAN", "Submit Now",submitToJobManWrapper, feat_ptr, part_ptr,NULL,NULL,NULL,NULL);
 promenu_load_action ("JOBMAN", "Submit Later", submitToJobManWrapper, feat_ptr, part_ptr,(void *)1,NULL,NULL,NULL);

 promenu_on_button ("JOBMAN", "Done Output", done_output, NULL, 0);
 promenu_on_button ("JOBMAN", "JOBMAN", promenu_exit_up, NULL,0);

 if(!get_nc_jobman_visible())
 {
    promenu_set_item_visible ("JOBMAN", "Submit Now", FALSE);
    promenu_set_item_visible ("JOBMAN", "Submit Later", FALSE);
 }

  promenu_make_compound(menus);
 /* promenu_make (MAIN_MENU, "PATH"); */
 /* menu_cutpath_inaccess (feat_ptr); */

 if ( !check_drm(DRM_COPY, FALSE) || !check_drm(DRM_SAVE, FALSE) )
 {
   promenu_make_item_inaccessible ("JOBMAN", "Submit Now" );
   promenu_make_item_inaccessible ("JOBMAN", "Submit Later" );
 }

 ierror = promenu_action(menus[0]);

 END:
 menu_cutter_path_post_ui( mfg_ptr, part_ptr, feat_ptr, p_slice_cr_mode );

 release_stat_final_loop();

 return (ierror);
}

/*---------------------------------------------------------------------------*/
int  cutpath_display (mfg_ptr, part_ptr, feat_ptr, cl_maps)
/*---------------------------------------------------------------------------*/
Mfg		*mfg_ptr;
Sld_prt		*part_ptr;
feature_record	*feat_ptr;
Clmaps          *cl_maps;
{
 int		prev_slice_mode, ierror;
 int		old_disp_flag, old_disp_opt;
 int* stk_mdl_ids = NULL;
 int  outset_found = 0, num_stk = 0;
 wchar_t *set_name;

 if ( !is_cdp_activate() )
 set_disp_force_compute_cl( FALSE );
 prev_slice_mode = put_slice_cr_mode (0);
 get_tool_disp_flags ( &old_disp_flag, &old_disp_opt );
 if ( !trl_vers_is_less (252) )
 {
    ierror = menu_cl_disp_type (part_ptr, feat_ptr, TRUE, NULL);
    if ( ierror != E_NO_ERROR )
       goto cdisp_end;
 }

 if ( ShouldCheckForStockAutoUpdate(part_ptr)  && 
      ( valid_mtc_operations_for_stock_update() 
       || get_mtc_index() == 0 ) )
 {
    outset_found = get_outset_from_feat_misc_data(feat_ptr, &set_name);
   ierror =  MessageAndUpdateStocksforFeat(mfg_ptr, part_ptr, feat_ptr, &stk_mdl_ids );
   if (ierror == PTC_E_ABORT)
     goto cdisp_end;
if ( outset_found )
  set_misc_data_for_outset_opers ( mfg_ptr, part_ptr, set_name );
 }


 ierror = cut_path_display (mfg_ptr, part_ptr, feat_ptr, K_NOT_USED, cl_maps);

 if ( (ierror != E_NO_ERROR) && (ierror != E_USER_ABORT) &&
      (ierror != E_INVALID_OPER) )
    msgID_put ( msg_ID86 );

cdisp_end:
 set_tool_disp_flags ( old_disp_flag, old_disp_opt );
 put_slice_cr_mode (prev_slice_mode);
 set_tool_kerf_opt (FALSE);
 clean_up_tool_disp_stat_loop_map( TRUE );
 set_disp_force_compute_cl( FALSE );

 if (get_mfg_auto_compute_stk_model() !=2 && stk_mdl_ids != NULL)
 {
   reset_stk_model_compute_flag(part_ptr, stk_mdl_ids);
   xar_free(&stk_mdl_ids);
 }
 return (ierror);
}

/*---------------------------------------------------------------------------*/
int cut_path_display (mfg_ptr, part_ptr, feat_ptr, create_mode, cl_maps)
/*---------------------------------------------------------------------------*/
Mfg		*mfg_ptr;
Sld_prt		*part_ptr;
feature_record	*feat_ptr;
int		create_mode;
Clmaps          *cl_maps;
{
 int		flag1, flag2;
 int		cr_slice_mode, done, current_slice_num;
 int		ierror, old_mult_wind_stat;

 cr_slice_mode = get_slice_cr_mode();
 current_slice_num = K_NOT_USED;
 done = FALSE;
 ierror = E_NO_ERROR;
 get_tool_disp_flags (&flag1, &flag2);
 old_mult_wind_stat = set_upd_mult_tool_wind ( TRUE );
 
 if(mfg_has_unsupported_components(feat_ptr))
   return (PTC_E_ABORT);

 ierror=update_ugc_tp_files(feat_ptr);

 while ( ! done )
 {
    if ( cr_slice_mode & CR_SLICE_DISP_DISC )
       done = get_slice_num_from_user ( feat_ptr, &current_slice_num );

    put_mill_slice_num ( current_slice_num );

    if ( ! done )
    {
       ierror = create_cutter_loc_w_clmaps (mfg_ptr, part_ptr, feat_ptr, NULL,
                                       create_mode, 0,0,0,0,0,0,cl_maps);
       set_tool_disp_flags (flag1, flag2);
    }

    if ( !(cr_slice_mode & CR_SLICE_DISP_DISC) )
       done = TRUE;
 }
 set_upd_mult_tool_wind ( old_mult_wind_stat );
 set_tool_disp_flags (flag1, flag2);
 put_mill_slice_num ( K_NOT_USED );

 return (ierror);
}
/*---------------------------------------------------------------------------*/
 static int  menu_cutpath_file (mfg_ptr, part_ptr, oper_feat, out_fname, save_option )
/*---------------------------------------------------------------------------*/
Mfg         *mfg_ptr;
Solid       *part_ptr;
Feat        *oper_feat;
wchar_t     *out_fname;
int         save_option;

{
 int  ierror = E_NO_ERROR;

 if( get_mfg_clfile_excl_seq_warn() )
 {
    int	user_resp = 0;
    (void)mfg_clfile_out_exclu_seq_warn (mfg_ptr, part_ptr, oper_feat, &user_resp);

    if( user_resp == 2 )
    {
       return PTC_E_ABORT;
    }
 }



 set_disp_force_compute_cl( FALSE );

 ierror = cutpath_file( mfg_ptr, part_ptr, oper_feat, out_fname, save_option );

 set_disp_force_compute_cl( FALSE );


 return ( ierror );
}

/*---------------------------------------------------------------------------*/
 PRO_STATIC int  cutpath_file (mfg_ptr, part_ptr, oper_feat, out_fname, save_option )
/*---------------------------------------------------------------------------*/
Mfg             *mfg_ptr;
Solid		*part_ptr;
Feat		*oper_feat;
wchar_t         *out_fname;
int             save_option;
{
 int             post_attr, post_flag, is_remote, output_post_attr, ierror;
 int             attr, no_menu;
 wchar_t         file_name[K_PATH_SIZE];
 int  mtc_ind = get_mtc_index();
 int* stk_mdl_ids = NULL;

 output_post_attr = 0;
 post_attr = 0;
 post_flag = FALSE;
 is_remote = FALSE;
 ierror = E_NO_ERROR;
 file_name[0] = NULL_WCHAR;
 no_menu = (save_option!=0);/*change to attr*/

 new_cutcom_info ( oper_feat );

 /* CMM doesn't use MCD file ...,  at least for now (g0302) */

 if ( !is_cdplayer_on() )
 {
   if ( no_menu )
     output_post_attr = 1;
   else if (!trl_vers_is_less (874) && !is_cmm_mfg (mfg_ptr))
   {
     ierror = output_type_post_menu ( &output_post_attr );
     if ( ierror != E_NO_ERROR || no_file_attr(output_post_attr)  )
        goto END;

     if ( is_disp_force_compute_cl() )
       prep_feat_for_recompute_cl( oper_feat );
   }
   is_remote = is_attribute(output_post_attr, REMOTE_ATTR);
 }
 else if ( cdp_action_save_as_mcd (save_option) )
 {
   ierror = post_proc_opt_dlg ( &post_attr, &output_post_attr);
   if ( ierror != E_NO_ERROR  )
     goto END;
 }
 else
   output_post_attr = CL_FILE_ATTR; /*==1*/

 post_flag = is_attribute(output_post_attr, MCD_FILE_ATTR );

 attr = post_flag;
 if ( cdp_action_save (save_option) )
   attr |= SAVE_CL_DFLT_NAME;

  if ( ShouldCheckForStockAutoUpdate(part_ptr) &&
       ( mtc_ind == ProCmdMfgClOutput ||
         mtc_ind == ProCmdMfgOpToolPath)
   )
   {
       ierror = MessageAndUpdateStocksforFeat(mfg_ptr, part_ptr, oper_feat, &stk_mdl_ids);
       if (ierror == PTC_E_ABORT )
         return ierror;
   }

 ierror = cutpath_file_create (mfg_ptr, part_ptr, oper_feat, attr,
                               is_remote, file_name, output_post_attr);
 if ( ierror != E_NO_ERROR )
   goto END;

 if ( post_flag && !is_cdplayer_on() )
   ierror = cl_post_optons_menu ( &post_attr );

 if ( ierror == E_NO_ERROR && post_flag )
   exec_cl_post (file_name, post_attr, output_post_attr, part_ptr, FALSE);

 END:
 if (get_mfg_auto_compute_stk_model() != 2 && stk_mdl_ids != NULL)
 {
   reset_stk_model_compute_flag(part_ptr, stk_mdl_ids);
   xar_free(&stk_mdl_ids);
 }

   if ( out_fname != NULL )
     wstrcpy ( out_fname, file_name );
   clean_up_tool_disp_stat_loop_map( TRUE );
   return (ierror);
}

/* Does the selected node in the tree have a workcell with a PP specified? */
extern int MfgPostTreeFeatHasMachinePP()
{
   Solid *part_ptr = NULL;
   Feat  *feat_ptr = NULL;
   Feat  *mach_ptr = NULL;

   wchar_t mach_name[K_NAME_SIZE] = {'\0'};
   wchar_t mach_id[K_NAME_SIZE] = {'\0'};

   /* Default dash */
   static wchar_t wstr_def_dash[2];
   strtows(wstr_def_dash, MPAR_DEF_DASH);

   /* Obtain the tree node */
   get_selected_node_a_data(NULL, NULL, &part_ptr, &feat_ptr);
   if (feat_ptr == NULL || part_ptr == NULL)
      return (FALSE);

   /* Check if it's a operation/nc seq */
   if (!is_operation_feat(feat_ptr) && !is_nc_seq_feat (feat_ptr))
       return (FALSE);

   /* Obtain the workcell of the feature */
   if (get_nc_seq_workcell(part_ptr, feat_ptr, &mach_ptr))
   {
      if (mach_ptr == NULL)
	  return (FALSE);

      if (!has_nc_expert_license(FALSE) && is_ncexpert())
         return (TRUE);

      /* Check if the workcell has a machine pp specified */
      get_workcell_mach_name(mach_ptr, mach_name);
      get_workcell_mach_id(mach_ptr, mach_id);

      /* Check if the machine pp specified is not null or '-' */
      if (mach_name[0] == '\0' || mach_id[0] == '\0' ||
          !wstrcmp(mach_name, wstr_def_dash) ||
          !wstrcmp(mach_id, wstr_def_dash))
	  return (FALSE);
   }

   return (TRUE);
}


/* Post Process with PP specified in the machine/CL file */
extern int MfgPostTreeFeatWithMachinePP()
{
   int ierror = E_NO_ERROR;
   int cl_file_attr = 0;
   int post_attr = 0;

   if (!MfgPostTreeFeatHasMachinePP())
      return (E_ERROR);

   /* Set post attributes */
    if (has_nc_expert_license(FALSE)) 
   {
	put_attribute(&post_attr, CL_PP_VERBOSE_ATTR);
   	put_attribute(&post_attr, CL_PP_MACHINE_ATTR);
   }

   /* Set CL file attributes */
   cl_file_attr = SAVE_CL_NO_MSG;

   /* Post Process */
   ierror = ncfm_post_tree_node_with_attr (cl_file_attr, post_attr);

   return (ierror);
}

/* Select a post processor and PP with it */
extern int MfgPostTreeFeatWithSelectedPP()
{
   int ierror = E_NO_ERROR;
   int cl_file_attr = 0;
   int post_attr = 0;

   /* Set post attributes */
   put_attribute(&post_attr, CL_PP_VERBOSE_ATTR);

   /* Set CL file attributes */
   cl_file_attr = SAVE_CL_NO_MSG;

   /* Post Process */
   ierror = ncfm_post_tree_node_with_attr (cl_file_attr, post_attr);

   return (ierror);
}

/* Post Process selected node in the tree with the given attributes */
static int ncfm_post_tree_node_with_attr (int cl_file_attr, int post_attr)
{
   int      ierror = E_NO_ERROR;
   Mfg     *mfg_ptr = get_cur_win_mfg();
   Solid   *part_ptr = NULL;
   Feat    *feat_ptr = NULL;
   wchar_t  file_name[K_PATH_SIZE] = {'\0'};
   int      post_file_attr = 0;

   /* Obtain the tree node */
   get_selected_node_a_data(NULL, NULL, &part_ptr, &feat_ptr);
   if (feat_ptr == NULL || part_ptr == NULL)
      return (E_ERROR);

   /* Create the NCL file */
   ierror = cutpath_file_create (mfg_ptr, part_ptr, feat_ptr, cl_file_attr,
                                 FALSE, file_name,FALSE);

   /* Post Process the NCL file */
   if (ierror == E_NO_ERROR && file_name[0] != '\0')
   {
      put_attribute(&post_file_attr, MCD_FILE_ATTR);
      exec_cl_post(file_name, post_attr, post_file_attr, part_ptr, TRUE);
   }
   else
   {
      dbg_err_syserr("ncfm_post_tree_node_with_attr",
                     "Error with creation of NCL file");
      return (ierror);
   }

   return (ierror);
}

/*---------------------------------------------------------------------------*/
static int no_file_attr(output_post_attr)
/*---------------------------------------------------------------------------*/
int output_post_attr;
{
    if (!is_attribute(output_post_attr, CL_FILE_ATTR)
           && !is_attribute(output_post_attr, MCD_FILE_ATTR ))
       return (TRUE);
    else
       return (FALSE);
}


/*---------------------------------------------------------------------------*/
int cutpath_file_create (mfg_ptr, part_ptr, oper_feat, save_opt, remote_flag,
                         file_name, output_post_attr)
/*---------------------------------------------------------------------------*/
Mfg             *mfg_ptr;
Solid           *part_ptr;
Feat            *oper_feat;
int             save_opt;
int              remote_flag;
/* O U T P U T */
wchar_t         *file_name;
int             output_post_attr;
{
 wchar_t        loc_file_name[K_PATH_SIZE];
 int            ierror=E_NO_ERROR;
 Feat           *feat_ptr;
 wchar_t        ext[K_PATH_SIZE];
 char           ext_char[K_PATH_SIZE];
 int            no_msg = save_opt & SAVE_CL_NO_MSG;
 int            dflt_name = save_opt & SAVE_CL_DFLT_NAME;
 wchar_t       *r_path_p;
 char          local_file_name[K_PATH_SIZE];
 wchar_t       tmp_loc_file_name[K_PATH_SIZE];
 PfaFileDepType dep_type = PFDT_BASED_ON_MODEL;
  int         check_flag = PFUH_CHECKOUT_ALL;
  Pfa         *pfa;
  Pfa         *file;
   int        dm_nc_ref = 1;
   char       part_name[K_LINESIZE];
   char       tmp_str[K_PATH_SIZE];
  wchar_t    f_name[K_PATH_SIZE];
 loc_file_name[0]=NULL_WCHAR;

 if (check_order_(ORDER_DEMO, NULL))
 {
   msgID_put(msg_ID87);
   return ( PTC_E_ABORT );
 }

 get_mfg_cl_lead_feat ( mfg_ptr, part_ptr, oper_feat, &feat_ptr );
 if ( feat_ptr == NULL )
     return ( PTC_E_ABORT );
 if (is_feat_w_type (oper_feat, FT_OPERATION))
    feat_ptr = oper_feat;

 if (!trl_should_bringup_saveui())
     ierror = get_ncl_file_name (feat_ptr, loc_file_name, FALSE);
 else
 {
   if (feat_ptr != NULL)
   {
      AncppModel  *ancpp_model = NULL;
      AncppRow    *step_row    = NULL;
      
      if( IsStepTableFeat( feat_ptr, &ancpp_model ) && 
          find_ancpp_row_by_temp_id( ancpp_model->steps, 
                                     feat_ptr->id, &step_row ) ) 
      {
         if( get_ancpp_ncl_file_name( step_row, loc_file_name ) )
            wcnvrt_to_lower( loc_file_name, loc_file_name );
      }
      else	 
      	 get_mfg_nc_fname( NULL, feat_ptr, loc_file_name );
   }
   
  if (feat_ptr != NULL && is_cmm_feat(feat_ptr) && get_ncfm_ui_for_cmm() )
	  get_feat_user_name_with_size( part_ptr, feat_ptr, loc_file_name, K_PATH_SIZE);

   get_ncl_file_extension(ext);
   
   if(!( (r_path_p = pro_search_get_path (L"PDM_WORKSPACE")) == NULL))
   write_to_wsp_process_name(loc_file_name);
     
   if ( !dflt_name )
   {
     wstrtos ( ext_char, ext);
     if (!strcmp(ext_char,"ncl"))
       ierror = gen_file_saveas_ui (MFG_CL_FILE, loc_file_name, NULL);
     else
       ierror = gen_file_saveas_ui (MFG_CL_FILE, loc_file_name, ext);
     
     if(ierror == E_NO_ERROR ) 
     {
       parse_filename_buffer( loc_file_name, NULL, f_name, NULL, NULL );	
       if( invalid_symbol( f_name ))
       {
         msgID_put(MFG_INVALID_SYMBOL);
         ierror = E_ERROR; 
       }
     }
   }
   else
   {
     wchar_t *ws_path = NULL;
     wchar_t buff[K_PATH_SIZE];
     wchar_t *r_path_p=NULL;
     int     user_dir_flag;
     char  pfa_buff[K_PATH_SIZE];
     wchar_t ws_pfa_buff[K_PATH_SIZE];


     assign_user_dir_flag_from_type(MFG_CL_FILE, &user_dir_flag);

     get_path_from_user_dir_flag(user_dir_flag,&r_path_p);

     
     if (r_path_p != NULL && !is_file_from_windchill(r_path_p) )
     {
       wstrcpy (buff, loc_file_name);
       pro_cat_location_name_ext (r_path_p, AnyLocation, buff, ext, 0,
                                  loc_file_name);
     }
     else if ((ws_path = pro_search_get_path (L"PDM_WORKSPACE")) != NULL)
     { 
       wstrcpy (buff, loc_file_name);
       pro_cat_location_name_ext (ws_path, AnyLocation, buff, ext, 0,
			          loc_file_name);
     }
    /* else 
     {
              wstrcpy (buff, loc_file_name);
              pfa_get_path ( pfa_get_cur_dir(), pfa_buff );
              strtows(ws_pfa_buff,pfa_buff);
       pro_cat_location_name_ext (ws_pfa_buff, AnyLocation, buff, ext, 0,
			          loc_file_name);

     }
     */
   }
 }

 if (ierror != E_NO_ERROR)
    return (ierror);

 ierror = update_ugc_tp_files(oper_feat);

  if ((!((r_path_p = pro_search_get_path (L"PDM_WORKSPACE")) == NULL)) &&
                                        (!is_attribute(output_post_attr, CL_FILE_ATTR)))
      get_cwd_clfname_from_ws_clfname(loc_file_name,loc_file_name);
 /*only in this specific case of a temp ncl file being created  for generation of tape file 
  changing the location of ncl file to cwd */ 
      
                            
 put_slice_cr_mode (CR_SLICE_FILE);
 cnvrt_filenm_only_to_lower(loc_file_name, loc_file_name);
 if (remote_flag)
    return remote_create_file_cutter_location ( mfg_ptr, part_ptr, oper_feat,
                                        loc_file_name);
 ierror = create_file_cutter_location ( mfg_ptr, part_ptr, oper_feat,
                                        loc_file_name, K_NOT_USED, NULL, NULL);

 
  
 if ( ierror == E_NO_ERROR )
 {
    if (!((r_path_p = pro_search_get_path (L"PDM_WORKSPACE")) == NULL))
    {
    file = pfa_alloc_wfile (loc_file_name);
     get_full_model_name_for_display ((Model*)part_ptr, FALSE, f_name, K_PATH_SIZE);
     wstrtos (part_name, f_name);
     convert_to_lower (part_name, part_name);
    dm_nc_ref = get_dm_nc_references();
    if (dm_nc_ref &&  (is_attribute(output_post_attr, CL_FILE_ATTR)))
  	{
 	pfa_set_dep_on_model(file,part_name,T_ASSEMBLY,DEP_T_MFG_DELIVERABLE);
  	}
    if (file != NULL)
    pfa_free_pro_file(&file);
    }

/* Change for SPR 2012326*/
    if ( !no_msg  )
    {
     if ( is_cmm_mfg(mfg_ptr))
         {
             wstrtos ( tmp_str,loc_file_name);
             msgID_put( CMM_FILEOPEN, tmp_str);
         }
     else
      msg_put ("Pro/NC file %0s has been created successfully.", loc_file_name);
    }
 }
 else if ( ierror == E_CANT_OPEN )
    msgID_put (msg_ID88);
 else if ( ierror != E_INVALID_OPER )
    if(is_cmm_mfg(mfg_ptr))
      msgID_put (MSG_CMM_FILE_CREATION_FAILED);
    else
      msgID_put (msg_ID85);
 if ( file_name != NULL )
    wstrcpy (file_name, loc_file_name);

    
 return (ierror);
}
/*---------------------------------------------------------------------------*/
int is_file_from_workspace (fname)
/*---------------------------------------------------------------------------*/
wchar_t *fname;
{
  wchar_t *r_path_p = NULL;
  
 if (fname == NULL)
 return(false);
  
 r_path_p = pro_search_get_path (L"PDM_WORKSPACE");

  if ( r_path_p != NULL && r_path_p[0] != L'\0' )
     if ( !(wstrncmp ( fname , r_path_p , 7 )) ) 
     return ( TRUE );

  return ( FALSE ); 
}

/*---------------------------------------------------------------------------*/
int is_file_from_windchill (fname)
/* Remember this may return true even if there is no primary server 	
	only checks if the path has wchill /ilink path  specifiers */
/*---------------------------------------------------------------------------*/
wchar_t *fname;
{
  wchar_t *r_path_p = NULL;
 
  if (fname == NULL)
 return(FALSE);

      r_path_p = pro_search_get_path (L"PDM_WORKSPACE");
     if ( r_path_p != NULL && r_path_p[0] != L'\0' )
     if ( !(wstrncmp ( fname , r_path_p , 7 )) ) 
     return ( TRUE );

     if ( !(wstrncmp ( fname , L"wtws:"  , 5 )) ||  !(wstrncmp ( fname , L"wtpub:" , 6 ))) 
     return ( TRUE );

  return ( FALSE ); 
}

/*---------------------------------------------------------------------------*/
static int create_ppname_for_machine_attr(cl_file_name, file_name)
/*---------------------------------------------------------------------------*/
wchar_t *cl_file_name;
wchar_t *file_name;
{
   Solid *part_ptr = NULL;
   Feat  *feat_ptr = NULL;
   Feat  *mach_ptr = NULL;
   int    finish;
   wchar_t mach_name[K_NAME_SIZE] = {'\0'};
   wchar_t mach_id[K_NAME_SIZE] = {'\0'};
   wchar_t *machine_id, *machine_name;
   
   /* Obtain the tree node */
   get_selected_node_a_data(NULL, NULL, &part_ptr, &feat_ptr);
   if (feat_ptr == NULL || part_ptr == NULL)
   {
      start_ncl_data_output(NULL, NULL);

      get_ncl_header_info (cl_file_name, NULL, NULL, NULL,
                             mach_name , mach_id, NULL, TRUE, &finish);
      ncl_close_in_file();
    
      end_ncl_data_output(FALSE);
   }
   /* Obtain the workcell of the feature */
   else if (get_nc_seq_workcell(part_ptr, feat_ptr, &mach_ptr))
   {
      get_feat_machines_params (feat_ptr, &machine_name, &machine_id);
      ProWstringCopy(machine_name,mach_name,PRO_VALUE_UNUSED);
      ProWstringCopy(machine_id,mach_id,PRO_VALUE_UNUSED);
   }
   if( file_name != NULL )
   {
      ProWstringConcatenate (mach_name, file_name, PRO_VALUE_UNUSED);
      ProWstringConcatenate (L".p", file_name, PRO_VALUE_UNUSED);
      ProWstringConcatenate (mach_id, file_name, PRO_VALUE_UNUSED);
      
	 return (TRUE);
   }
  return (FALSE);
}

/* Executes gener : The main PP routine */
/*---------------------------------------------------------------------------*/
int exec_cl_post (file_name, post_pr_attr, output_post_attr, 
                  part_ptr, list_all_pp)
/*---------------------------------------------------------------------------*/
wchar_t *file_name;
int      post_pr_attr;
int      output_post_attr;
Sld_prt *part_ptr;
Bool     list_all_pp; /* TRUE if all PP are to be listed */
{
 char    gener_com[4*K_PATH_SIZE];
 char    ppname[K_NAME_SIZE];
 char    pidstring[K_NAME_SIZE];
 wchar_t *mf_cl_dir, nclpname_to_get[K_PATH_SIZE];
 wchar_t *mf_tape_dir = NULL;
 wchar_t nclfname_to_get[K_FAMILY_NAME_SIZE];
 char    tap_name[K_FAMILY_NAME_SIZE];
 char    mf_cl_dir_[K_PATH_SIZE];
 char    mf_tape_dir_ [K_PATH_SIZE];
 wchar_t nclfname[K_PATH_SIZE], w_err_file[K_PATH_SIZE];
 char    file_to_camgener[K_PATH_SIZE], filename2[K_PATH_SIZE];
 char    out_name[K_PATH_SIZE], path_n_name[K_PATH_SIZE];
 char    qualifier[ICAM_COM_LEN]; /* 512, longer enough to hold password */
 char    dbf_file[K_PATH_SIZE], passwd[K_PATH_SIZE], err_file[K_PATH_SIZE];
 char    options[2*K_PATH_SIZE]; /* max size for NT, 1K for unix though */
 Pfa     *pfa, *tmp_pfa;
 int     vers, ierror, access_status, status = 0;
 time_t  tap_old_time = -1; /* Time of creation of .tap file */
 time_t  tap_new_time = -1; /* Time of modification of .tap file */
 wchar_t  tap_file[K_PATH_SIZE];
 wchar_t cwd_file_name[K_PATH_SIZE];
 wchar_t wsp_tape_file_name[K_PATH_SIZE];
 wchar_t *r_path_p;
 int      tap_to_wspace = FALSE, can_delete = FALSE;
 wchar_t ppname_machine[K_PATH_SIZE];



 if (file_name == NULL)
    return E_ERROR;

 set_icam_debug_runmode ();

 ierror = setup_post_env();
 if (ierror != E_NO_ERROR)
 {
    dbg_err_syserr ("exec_cl_post", "setup_post_env() fails");
    return ierror;
 }

 mf_cl_dir = get_pro_mf_cl_dir_absolute();
 mf_tape_dir = get_pro_mf_tape_dir_absolute();


 parse_filename_buffer ( file_name, nclpname_to_get, nclfname_to_get,
                         NULL, NULL );
 wcnvrt_to_lower (nclfname_to_get, nclfname);
 wstrtos (filename2, nclfname);

 if ( is_file_from_workspace( file_name ) )
  {
   ierror = copy_cl_file_to_cwd_dir(file_name, filename2, cwd_file_name);
   if ( !ierror )
      parse_filename_buffer ( cwd_file_name, nclpname_to_get, nclfname_to_get, 
                              NULL, NULL );
  }
 /* Create the tap file name */
 
 if (mf_cl_dir != NULL && (mf_cl_dir[0] != L'\0') && !is_file_from_windchill(mf_cl_dir))
   ProWstringCopy(mf_cl_dir,nclpname_to_get,PRO_VALUE_UNUSED);  
 pro_wsprintf(tap_file, "%ws%ws%s", nclpname_to_get,nclfname_to_get,
              TAP_FILE_EXT);

 /*----------------------------------------------------------------*/
 /* Get CL file_name version                                       */
 /*----------------------------------------------------------------*/
 pfa_alloc_pro_file (&pfa);
 if ( is_file_from_workspace( file_name ) && ierror == 0)
  pfa_parse_to_pro_file (pfa, str_ret (cwd_file_name));
 else
  pfa_parse_to_pro_file (pfa, str_ret (file_name));
 pfa_get_file_vers (pfa, THIS_VERSION, &vers);
 access_status = pfa_access (pfa,ACCESS_F_EXIST);

 /* If file_name version is 1 check if NCL file really has version
    or it was added by get_ncl_file_name() (Damned thing does it
    all the time ) */

 if ( vers == 1 && !access_status )
 	pfa_put_version ( pfa, 0);
 pfa_get_full_name ( pfa, file_to_camgener );
 pfa_free_pro_file (&pfa);

 /*----------------------------------------------------------------*/
 /* Prepare gener name                                             */
 /*----------------------------------------------------------------*/
 if (!get_icam_gener_path_n_name (path_n_name))
    return E_ERROR;
 pfa_alloc_pro_file (&pfa);
 pfa_parse_to_pro_file (pfa, path_n_name);
 pfa_get_full_name ( pfa, gener_com );
 pfa_free_pro_file (&pfa);


 /*----------------------------------------------------------------*/
 /* build the camgener command line options                        */
 /*----------------------------------------------------------------*/
 options[0] = NULL_CHAR;
 cat_icam_qualifier_arg (options, file_to_camgener);

 /*----------------------------------------------------------------*/
 /* append -po=ppname option                                       */
 /*----------------------------------------------------------------*/
 if (is_attribute (post_pr_attr, CL_PP_MACHINE_ATTR))
     strcpy (ppname, "1");
 else
 {
    if (!has_nc_expert_license(FALSE) && is_ncexpert())
    {
      strcpy (ppname, "\"uncx01.p\"00"); 
    }
    else
     ierror = get_ppname_from_command (ppname, file_name, part_ptr,
				       list_all_pp);
     if (ierror != E_NO_ERROR)
     {
        dbg_err_syserr ("exec_cl_post", "cannot get -po option");
        return ierror;
     }
 }
 if (build_icam_qualifier (ICAM_POST, qualifier))
 {
    append_icam_qualifier_arg (qualifier, ppname, qualifier);
    cat_icam_qualifier_arg (options, qualifier);
 }

 /*----------------------------------------------------------------*/
 /* Append debug option                                            */
 /*----------------------------------------------------------------*/
 if (!is_gpost() && is_attribute (post_pr_attr, CL_PP_DEBUG_ATTR) &&
     build_icam_qualifier (ICAM_DEBUG, qualifier))
    cat_icam_qualifier_arg (options, qualifier);

 /*----------------------------------------------------------------*/
 /* append -v , -tra  options                                      */
 /*----------------------------------------------------------------*/
 if (is_attribute(post_pr_attr, CL_PP_VERBOSE_ATTR) &&
     build_icam_qualifier (ICAM_VERBOSE, qualifier))
    cat_icam_qualifier_arg (options, qualifier);
 if (is_attribute(post_pr_attr, CL_PP_TRACE_ATTR) &&
     build_icam_qualifier (ICAM_TRACE, qualifier))
    cat_icam_qualifier_arg (options, qualifier);

 /*----------------------------------------------------------------*/
 /* append  -pid= option                                           */
 /*----------------------------------------------------------------*/
 if (!is_gpost() && is_attribute(post_pr_attr, CL_PP_PID_ATTR))
 {
    ierror = get_pid_string ( pidstring );
    if (ierror == E_NO_ERROR && build_icam_qualifier (ICAM_PID, qualifier))
    {
       append_icam_qualifier_arg (qualifier, pidstring, qualifier);
       cat_icam_qualifier_arg (options, qualifier);
    }
 }

 /*----------------------------------------------------------------*/
 /* Append input file format option, for pre-16.0.                 */
 /*----------------------------------------------------------------*/
 if (get_campost_dir_config() != NULL &&
     build_icam_qualifier (ICAM_FORMAT, qualifier))
 {
    append_icam_qualifier_arg (qualifier, "ptc", qualifier);
    cat_icam_qualifier_arg (options, qualifier);
 }

 /*----------------------------------------------------------------*/
 /* Append list and tap option                                     */
 /*----------------------------------------------------------------*/
 if (mf_cl_dir != NULL && (mf_cl_dir[0] != L'\0')  && !is_file_from_windchill(mf_cl_dir) )
 {
     wstrtos (mf_cl_dir_, mf_cl_dir);
     if (build_icam_qualifier (ICAM_TAP, qualifier))
     {
        append_icam_qualifier_arg (qualifier, mf_cl_dir_, qualifier);
        cat_icam_qualifier_arg (options, qualifier);
        cat_icam_qualifier_arg (options, filename2);
        cat_icam_qualifier_arg (options, TAP_FILE_EXT);
     }
     if (build_icam_qualifier (ICAM_LIST, qualifier))
     {
        append_icam_qualifier_arg (qualifier, mf_cl_dir_, qualifier);
        cat_icam_qualifier_arg (options, qualifier);
        cat_icam_qualifier_arg (options, filename2);
        cat_icam_qualifier_arg (options, ".lst");
     }
 }

 /*----------------------------------------------------------------*/
 /* Append db file option IF DBF FILE EXISTS.                      */
 /*----------------------------------------------------------------*/
 dbf_file[0] = NULL_CHAR;
 if (is_gpost())
    get_gpostpp_dir_absolute(dbf_file, &can_delete);
 else if (use_proncpost_ui())
    get_pro_ncpost_dbf(dbf_file);

 if (dbf_file[0] != NULL_CHAR && pro_ncpost_dbf_exists (dbf_file))
    if (build_icam_qualifier (ICAM_DB, qualifier))
    {
       append_icam_qualifier_arg (qualifier, dbf_file, qualifier);
       cat_icam_qualifier_arg (options, qualifier);
    }

 /*----------------------------------------------------------------*/
 /* Append error file option                                       */
 /*----------------------------------------------------------------*/
 if (use_ncpost_ui())
 {
    get_temp_fname (w_err_file, NULL, NULL);
    if (build_icam_qualifier (ICAM_ERFILE, qualifier))
    {
       wstrtos (err_file, w_err_file);
       append_icam_qualifier_arg (qualifier, err_file, qualifier);
       cat_icam_qualifier_arg (options, qualifier);
    }
 }

 /*----------------------------------------------------------------*/
 /* Passes -dbg_ef to trigger ICAM's debugging code which will dump*/
 /* some more debug information to the output file.                */
 /*----------------------------------------------------------------*/
 if (!is_gpost() && is_mfg_debug_runmode ())
 {
    if (build_icam_qualifier (ICAM_INTERNAL_DBG, qualifier))
       cat_icam_qualifier_arg (options, qualifier);
 }

 /*----------------------------------------------------------------*/
 /* Record the time of creation of .tap file if one exists. This   */
 /* is to check if the file will be updated in the following       */
 /* routine. Necessary in the absence of proper error codes in     */
 /* case of failure.                                               */
 /*----------------------------------------------------------------*/
 pro_get_mdate(tap_file, &tap_old_time);

 /*----------------------------------------------------------------*/
 /* Append password option                                         */
 /*----------------------------------------------------------------*/
 if (is_gpost())
 {
    if (gpost_encrypt_opts (passwd, K_NAME_SIZE, is_mfg_debug_runmode()) &&
        build_icam_qualifier (GPOST_PASSWD, qualifier))
    {
       append_icam_qualifier_arg (qualifier, passwd, qualifier);
       cat_icam_qualifier_arg (options, qualifier);
    }
 }
 else if (use_ncpost_ui ())
 {
    if (icam_encrypt_opts (passwd, K_PATH_SIZE, 0) &&
        build_icam_qualifier (ICAM_PASSWD, qualifier))
    {
       append_icam_qualifier_arg (qualifier, passwd, qualifier);
       cat_icam_qualifier_arg (options, qualifier);
    }
 }

 /*----------------------------------------------------------------*/
 /* If we are using campost_dir, then compose the entire command   */
 /* by simply appending options[] to genen_com; otherwise we need  */
 /* to setup an environment variable named "param" to hold the     */
 /* entire comamnd line options since Pro/E cannot handle command  */
 /* line longer than around 300 characters.                        */
 /*----------------------------------------------------------------*/
 if (use_ncpost_ui())
 {
    setup_param_env_for_icam (options);

 /* For Windows '95 in h0338 there was a problem with lack of environment
    space. Systems group suggested using pro_exec_command() instead of
    pro_system_call_new() to overcome this problem. (changed to pro_system_
    call_new() as per their suggestion in rev 18.) */
 /* In the long term we should just be using pro_exec_command() on all
       platforms */

    {
      pro_system_call_new (&status, WAIT_MODE, gener_com);
    }
 }
 else
 {
    strcat (gener_com, " ");
    strcat (gener_com, options);
    pro_system_call (&status, WAIT_MODE, gener_com);
    if (is_mfg_debug_runmode ()) /* 1234 */
       dbg_print_info("exec_cl_post", "Calling %s", gener_com);
 }

 /*----------------------------------------------------------------*/
 /* Handle icam returned error.                                    */
 /*----------------------------------------------------------------*/
 dbg_print_info ("exec_cl_post", "Post Processor return status :%d", status);
 process_pp_error ( status );
 ierror = is_fatal_pprocess_error (status) ? PTC_E_ABORT : E_NO_ERROR;
 /* Check if tap file has been updated */
 pro_get_mdate(tap_file, &tap_new_time);
 if ( ierror == E_NO_ERROR)
 {
  
   {
         if(!( (r_path_p = pro_search_get_path (L"PDM_WORKSPACE")) == NULL)
                                 || (mf_tape_dir != NULL && mf_tape_dir[0] != L'\0'))
          {
	   wstrtos (tap_name,nclfname_to_get);
	   if (is_attribute (post_pr_attr, CL_PP_MACHINE_ATTR))
	   {
	      ppname_machine[0] = NULL_WCHAR;
	      create_ppname_for_machine_attr(file_name, ppname_machine);
	      if ( wstrcmp(ppname_machine, L"") != 0)
		 wstrtos(ppname, ppname_machine);
	   }
	      if (!copy_tape_file_to_wspace (tap_file,tap_name,ppname, out_name))
	   {
           
             msg_put ("Post processed file %s was created sucsessfully", out_name);
	      tap_to_wspace = TRUE;
           	      	      
	   }
           else 
               {
                msg_put ("Post processed file %s could not be copied to wspace", out_name);    
               }
	    
	  }

      if (!tap_to_wspace)
          {
           if (tap_new_time > tap_old_time)
           {
           pro_sprintf ( out_name," %ws%s", nclfname, TAP_FILE_EXT);
           msg_put ("Post processed file %0s was created sucsessfully", out_name);
           }
          }
      
      
   }
 }
 else if (use_ncpost_ui())
    msgID_put (msg_ID89);

 if (is_gpost() && can_delete)
 {
   tmp_pfa = pfa_alloc_dir(dbf_file);
   pfa_delete_dir(tmp_pfa, TRUE, TRUE);
   pfa_free_pro_file (&tmp_pfa);
 }

 /*----------------------------------------------------------------*/
 /* set enviroment back                                            */
 /*----------------------------------------------------------------*/
    if (use_ncpost_ui())
    {
       if (w_err_file[0] != NULL_WCHAR)
       {
          typefile (w_err_file);
          if (!is_mfg_debug_runmode()) /* keep it for debug purpose */
             delete_file (w_err_file);
       }
    }

    if ( !is_attribute ( output_post_attr, CL_FILE_ATTR ) )
      {
	 delete_file ( file_name );
    if ( is_file_from_workspace( file_name ) && ierror == 0 )
        delete_file(cwd_file_name);
      }
    if ( tap_to_wspace)
       delete_file (tap_file);

    unset_post_env();
    return E_NO_ERROR;
}

/*==========================================================================*/
int write_to_wsp_process_name( 
/*--------------------------------------------------------------------------*/
    wchar_t        *file_name
   )
/*--------------------------------------------------------------------------*/
{
int     user_dir_flag;
wchar_t *r_path_p =NULL;
wchar_t *ws_path;
Mfg     *mfg_ptr;
wchar_t loc_file_name[K_PATH_SIZE];
wchar_t user_vis_model_name[PRO_MDLNAME_SIZE] = { NULL_WCHAR };
int     str_length;

   if(get_rem_mfgname_from_ncl())
     return(TRUE);
   ProWstringCopy(file_name,loc_file_name,PRO_VALUE_UNUSED);
   ws_path = pro_search_get_path (L"PDM_WORKSPACE");
   if(ws_path == NULL) 
     return FALSE;
   assign_user_dir_flag_from_type(MFG_CL_FILE, &user_dir_flag);
   get_path_from_user_dir_flag(user_dir_flag,&r_path_p);
   
   if (r_path_p == NULL )
       r_path_p = pro_search_get_path (L"PDM_WORKSPACE");

  mfg_ptr = get_cur_mfg();
  get_model_user_name_buffer((Model*)mfg_ptr, user_vis_model_name,
                             NUM_ELEM_IN_ARR(user_vis_model_name));
  ProWstringConcatenate (L"_", loc_file_name, PRO_VALUE_UNUSED);
  ProWstringConcatenate (user_vis_model_name, loc_file_name, PRO_VALUE_UNUSED);
  ProWstringLengthGet(loc_file_name,&str_length);
  if (str_length<32)
       ProWstringCopy(loc_file_name,file_name,PRO_VALUE_UNUSED); 
  return(TRUE);
   
}

/*---------------------------------------------------------------------------*/
static int 
 process_pp_error ( status )
/*---------------------------------------------------------------------------*/
int      status;
{
 int     ierror;
 int     ii;
 int     num_of_errors;

 ierror = E_NO_ERROR;

 num_of_errors = sizeof (pp_errors) / sizeof (PP_error);

/* errors from 1 to 16 are not in the table pp_errors !!! */

 if ( status >= 1 && status <= 16 )
 {
     msg_put ("Error type %0d detected", &status);
     ierror = PTC_E_ABORT;
          goto END;
 }

 for ( ii = 0; ii < num_of_errors; ii++ )
 {
     if ( status == pp_errors[ii].status )
     {
        msg_put (pp_errors[ii].err_mes);
        ierror = PTC_E_ABORT;
        break;
     }
 }

 END :
     return ( ierror );
}

/*=========================================================================*/
 static int is_fatal_pprocess_error (status)
/*-------------------------------------------------------------------------*/
/*
     Meanings of error values:

        -1      Command line argument error(s)
        0:3     Processing successfully completed
        4:7     Processing completed with warnings
        8:15    Processing completed with errors
        16      Processing aborted due to fatal error
        100     Error reading ncl file
        101     Error loading or locating post processor
*/

 int    status;
{
 /* Error status from 1 to 15 are not fatal error, pp output still useable */
 if (status >= 0 && status < 16)
    return (FALSE);
 else
    return (TRUE);
}

/*---------------------------------------------------------------------------*/
static int get_ppname_from_command (ppname, ncl_file_name, part_ptr, list_all_pp)
/*---------------------------------------------------------------------------*/
char     ppname[K_NAME_SIZE];
wchar_t  *ncl_file_name;
Sld_prt  *part_ptr;
Bool      list_all_pp; /* TRUE if all PP are to be  listed */
{
 int     num, index;
 wchar_t **names;
 wchar_t **menu_list;
 wchar_t **help_line_strs;
 wchar_t *menu_item;
 wchar_t *help_line;
 wchar_t tmpfilename[K_PATH_SIZE];
 char    dblist_path[K_PATH_SIZE];
 char    dblist_com[K_PATH_SIZE];
 char    descriptor[K_PATH_SIZE];
 char    dblist_exe[K_PATH_SIZE];
 char    tempname[K_PATH_SIZE];
 int     ierror;
 wchar_t *ptr;
 wchar_t *wp;
 int      nn, status;
 int      ii;
 static wchar_t space = ' ';
 Pfa     *pfa;
 char     ext[K_EXTENSION_SIZE];
 wchar_t  alt_title[K_NAME_SIZE];

 ierror = E_NO_ERROR;
 ptr = NULL_WCHAR;
 descriptor[0] = NULL_CHAR;

 ierror = create_path_to_post_com (dblist_path);
 if ( ierror != E_NO_ERROR )
 {
    msgID_put (msg_ID90);
    dbg_err_syserr ("get_ppname_from_command", "create_path_to_post_com fail");
    return ( PTC_E_ABORT );
 }

 pfa_alloc_pro_file (&pfa);
 if (pfa_make_temp_file (pfa, (int) T_FOREIGN) != PFA_E_NO_ERROR)
 {
    dbg_err_syserr ("get_ppname_from_command", "cannot generate tmpfile.");
    msgID_put (msg_ID90);
    pfa_free_pro_file (&pfa);
    return (PTC_E_ABORT);
 }

 /* Include an extension, forcing ICAM not to add any of its defaults */
 pfa_get_extension(pfa, ext);
 if (ext[0] == NULL_CHAR)
   pfa_put_extension(pfa, "tmp");

 if (pfa_get_full_name (pfa, tempname) != PFA_E_NO_ERROR)
 {
    dbg_err_syserr ("get_ppname_from_command", "cannot get full name");
    msgID_put (msg_ID90);
    pfa_free_pro_file (&pfa);
    return ( PTC_E_ABORT );
 }
 strtows (tmpfilename, tempname);
 dbg_print_info ("get_ppname_from_command", "tmpfile for dblist %ws",
                 tmpfilename);

 /*----------------------------------------------------------------------*/
 /* Build a dblist or propostl command and call it                       */
 /*----------------------------------------------------------------------*/
 get_icam_dblist_exe_name (dblist_exe);

 /* If all PP are to be listed there is no need to get any descriptor */
 if (!list_all_pp && !is_gpost())
   ierror = get_dblist_descriptor (ncl_file_name, part_ptr, descriptor);
 else
   ierror = E_NO_ERROR;

 if (ierror != E_NO_ERROR && ierror != E_INVALID_INPUT)
 {
   /* otherwise msg has been given */
   msgID_put (msg_ID90);
   dbg_err_syserr ("get_ppname_from_command", "get_dblist_descriptor fails");
   return (PTC_E_ABORT);
 }

 /*----------------------------------------------------------------------*/
 /* Call propostl ...                                                    */
 /*----------------------------------------------------------------------*/
 build_icam_dblist_command (dblist_path, dblist_exe, tmpfilename, descriptor,
                            dblist_com);
 if (is_mfg_debug_runmode ())
    dbg_print_info("get_ppname_from_command","Calling %s", dblist_com);

 /* For Windows '95 in h0338 there was a problem with lack of environment
    space. Systems group suggested using pro_exec_command() instead of
    pro_system_call_new() to overcome this problem. (changed to pro_system_
    call_new() as per their suggestion in rev 18.) */
 /* In the long term we should just be using pro_exec_command() on all
       platforms */
  {
    pro_system_call_new (&status, WAIT_MODE, dblist_com);
  }

 /*----------------------------------------------------------------------*/
 /* A list of database will be generated already in tmpfilename          */
 /*----------------------------------------------------------------------*/
 names = XAR_BEGIN (wchar_t *, 4);
 menu_list = XAR_BEGIN (wchar_t *, 4);
 help_line_strs = XAR_BEGIN (wchar_t *, 4);

 for (ii = 0, ierror = PTC_E_ABORT; ii < 3 && ierror != E_NO_ERROR; ii ++)
 {
    dbg_print_info ("get_ppname_from_command", "ii = %d", ii);
    spg_delay (30); /* delay for 3 seconds to wait for os */
    ierror = icam_read_dbfile (pfa, &names, K_PATH_SIZE);
 }

 if (!is_mfg_debug_runmode ())  /* keep it for debug purpose */
    pfa_delete_file (pfa, THIS_VERSION);

 pfa_free_pro_file (&pfa);

 if ( ierror == E_CANT_OPEN )
 {
    msgID_put (msg_ID90);
    dbg_err_syserr ("get_ppname_from_command", "pro_read_file_to_buf_low fail");
    ierror = PTC_E_ABORT;
    goto CLEANUP;
 }

 num = XAR_COUNT ( &names );
 if (num < 1)
 {
    msgID_put (msg_ID91);
    ierror = PTC_E_ABORT;
    goto CLEANUP;
 }

 /*----------------------------------------------------------------------*/
 /* Prepare menu item from tmpfilename                                   */
 /*----------------------------------------------------------------------*/
 for ( ii = 0; ii < num; ii++ )
 {
    menu_item = (wchar_t *) getmem ( sizeof (wchar_t) * K_LINESIZE );
    help_line = (wchar_t *) getmem ( sizeof (wchar_t) * K_LINESIZE );
    wp = wstrchr (names[ii], space);
    nn = wp-names[ii];
    wstrncpy (menu_item, names[ii], nn);
    menu_item[nn] = NULL_WCHAR;
    wstrcpy (help_line, wp);
    xar_append ( &menu_list, 1, &menu_item);
    xar_append ( &help_line_strs, 1, &help_line);
 }


 /*----------------------------------------------------------------------*/
 /* If the menu is empty print an error message                          */
 /*----------------------------------------------------------------------*/
 if (menu_list[0][0] == NULL_WCHAR)
 {
    msgID_put (msg_ID91);
    ierror = PTC_E_ABORT;
    goto CLEANUP;
 }

 xar_append ( &menu_list, 1, &ptr );
 xar_append ( &help_line_strs, 1, &ptr );

 /*----------------------------------------------------------------------*/
 /* Let user select one post processor                                   */
 /*----------------------------------------------------------------------*/
 msgID_put (msg_ID92);
 msgID_sput_buffer(alt_title, K_NAME_SIZE, MSG_PP_LIST_MENU_TITLE);
 index =  usr_sel_from_strs_wprefunc_hlp ("PP List", menu_list, 0, NULL, NULL,
                                          help_line_strs, NULL, alt_title);
 if ( index < 0 )
 {
    ierror = PTC_E_ABORT;
    goto CLEANUP;
 }

 ierror = E_NO_ERROR;
 wstrtos (ppname, menu_list[index]);


 CLEANUP:
    free_char_array ((char **)&names);
    free_char_array ((char **)&menu_list);
    free_char_array ((char **)&help_line_strs);

    return (ierror);
}


/*---------------------------------------------------------------------------*/
static int get_pid_string ( pidstring )
/*---------------------------------------------------------------------------*/
char     *pidstring;
{
 static wchar_t space = ' ';
 wchar_t *ws;
 int     stat;
 wchar_t wpidstring[K_NAME_SIZE];
 int     ierror;

 ierror = E_NO_ERROR;

 stat = msg_get (F_S, wpidstring, K_NAME_SIZE,
              "Enter the process identification string");
 if ( stat != 0 )
 {
    ierror = PTC_E_ABORT;
    goto END;
 }

 ws = wstrchr(wpidstring, space);

 if ( ws != NULL )
 {
    *ws = NULL_WCHAR;
    if (wstrlen(wpidstring) == 0)
    {
        ierror = PTC_E_ABORT;
        goto END;
    }
    else
        msgID_put (msg_ID93);
 }
 wstrtos (pidstring, wpidstring);

 END:
    return ( ierror );

}





/*---------------------------------------------------------------------------*/
static int cut_path_step (part_ptr)
/*---------------------------------------------------------------------------*/
Sld_prt	*part_ptr;
{
 double	default_step, step;

 get_cutter_step (&default_step);
 step = default_step;

 /* prompt user */
 cut_path_step_low ( part_ptr, default_step, &step );

 set_cutter_step (step);
 return (E_NO_ERROR);
}

/*---------------------------------------------------------------------------*/
int cut_path_step_low ( part_ptr, default_step, r_step )
/*---------------------------------------------------------------------------*/
Sld_prt	*part_ptr;
double	default_step;
double	*r_step;
{
 double	part_size, step, range[2];
 double         *def_vals;


 /* set the range */
 part_size = sld_regen_outline_get_part_size (part_ptr);
 range[0] = 0.005 * part_size;
 range[1] = 1e5;

 /* set the default value */
 if ( default_step <= 0.0)
    step = 0.05 * part_size;
 else
    step = default_step;

 if (trl_vers_is_less (TRL_VER_PRO_VAL))
     msg_get (F_LE, &step, range, "Enter step size [%0f]", &step);
 else
  {
     msgID_put (msg_ID94);

     def_vals = XAR_BEGIN ( double, 4);
     xar_append (&def_vals, 1, &step);

     /* prompt user */
     promenu_get_value (F_LE, &step, range, def_vals, 0,               "Enter step size [%0f]", &step);
     xar_free (&def_vals);
  }
 *r_step = step;
 return ( 0 );
}

/*---------------------------------------------------------------------------*/
/*ARGSUSED*/
int cut_path_step_with_feat (part_ptr, feat_ptr)
/*---------------------------------------------------------------------------*/
Sld_prt		*part_ptr;
feature_record	*feat_ptr;
{
  cut_path_step (part_ptr);
  return ( 0 );
}

/*---------------------------------------------------------------------------*/
static int get_slice_num_from_user ( feat_ptr, rslice_num )
/*---------------------------------------------------------------------------*/
feature_record	*feat_ptr;
int		*rslice_num;
{
 Mfg_info	*mfg_info_ptr;
 int		range[2];
 int		not_found = FALSE, slice_num;

 *rslice_num = -1;
 slice_num = 1;
 mfg_info_ptr = (Mfg_info *) feat_ptr->dat_ptr;

 range[0] = 1;
 range[1] = XAR_COUNT ( & (mfg_info_ptr->cl_info_ptr->slice_info ) );

 not_found = msg_get(F_D, &slice_num, range,
		     "Enter slice number [QUIT] :", &range[0], &range[1]);

 if ( ! not_found )
    *rslice_num = slice_num;

 return ( not_found );
}


/*========================================================================*/
 static void menu_cutpath_inaccess (feat_ptr)
/*------------------------------------------------------------------------*/
 feature_record *feat_ptr;
{
 if (oper_has_patterns (feat_ptr))
 {
     promenu_make_item_inaccessible ("PATH", "Rotate");
     promenu_make_item_inaccessible ("PATH", "Translate");
     promenu_make_item_inaccessible ("PATH", "Scale");
     promenu_make_item_inaccessible ("PATH", "Mirror");
     promenu_make_item_inaccessible ("PATH", "Units");
 }
}

/*========================================================================*/
int get_cur_units_ptr (r_unit_ptr)
/*------------------------------------------------------------------------*/
Unit_rec	**r_unit_ptr;
{
 INIT_ARG (r_unit_ptr, cur_unit_rec_ptr);
 return ( 0 );
}

/*========================================================================*/
static int get_dblist_descriptor( ncl_file_name, part_ptr, descriptor )
/*------------------------------------------------------------------------*/
wchar_t         *ncl_file_name;
Sld_prt         *part_ptr;
char            *descriptor;
{
 int             status;
 int             feat_id;
 int             dummy;
 Feat           *feat_ptr;
 Feat           *workcell;
 int             ierror = E_NO_ERROR;
 char           *loc_descriptor;
 Misc_choice_rec wkcell_type;

 /*
   if (is_smt_workpiece (part_ptr))
   {
     strcat (descriptor, "P");
     return E_NO_ERROR;
   }
 */

 start_ncl_data_output(NULL, NULL);

 status = get_ncl_header_info (ncl_file_name, NULL, NULL, &feat_id,
                               NULL, NULL, NULL, TRUE, &dummy);

 ncl_close_in_file();

 end_ncl_data_output(FALSE);


 if (!status)
 {
   msgID_put (WRONG_HEADER);
   ierror = E_INVALID_INPUT;
   goto END;
 }

 if ( (feat_id < 0)  || (feat_id >= dbkey_count(part_ptr)) ||
      ((feat_ptr = (feature_record *)dbkey_get(part_ptr, feat_id)) == NULL)
      || (feat_ptr->type != T_FEATURE) )
 {
    msgID_put (INVALID_FEATURE_ID);
    ierror = E_INVALID_INPUT;
    goto END;
 }

 if (!get_nc_seq_workcell ( part_ptr, feat_ptr, &workcell ))
 {
    msg_put ("Cannot get workcell information from feature (id = %d).",feat_id);
    ierror = E_INVALID_INPUT;
    goto END;
 }

 ierror = get_workcell_type(workcell, &wkcell_type);
 if (ierror == E_NO_ERROR &&
     ( is_rec_misc_choice ( &wkcell_type, FM_WRKCELL_MILL ) ||
       is_rec_misc_choice ( &wkcell_type, FM_WRKCELL_CMM  ) ) )
	   /* ICAM used same macro as milling */
 {
      if (use_ncpost_ui ())
         loc_descriptor = "M";
      else
      {
         if (is_5axis_oper (workcell->dat_ptr))
            loc_descriptor = "M5";
         else if (is_4axis_oper (workcell->dat_ptr))
            loc_descriptor = "M4";
         else
            loc_descriptor = "M3";
      }

      strcpy (descriptor, loc_descriptor);
 }
 else if (ierror == E_NO_ERROR &&
	  is_rec_misc_choice ( &wkcell_type, FM_WRKCELL_MIL_N_TRN ) )
 {
      if (use_ncpost_ui ())
         strcpy (descriptor, "L");
      else
      {
        if ( is_5axis_oper( workcell->dat_ptr ) )
           strcpy ( descriptor, "M5");
        else if ( is_4axis_oper(workcell->dat_ptr) )
           strcpy ( descriptor, "M4");
        else
           strcpy ( descriptor, "M3");
      }
 }
 else if (ierror == E_NO_ERROR &&
	  is_rec_misc_choice ( &wkcell_type, FM_WRKCELL_LATHE ) )
 {

      if (use_ncpost_ui ())
         strcpy (descriptor, "L");
      else
      {
        if ( is_4axis_oper(workcell->dat_ptr) )
           strcpy (descriptor, "L4");
        else
           strcpy (descriptor, "L2");
      }
 }
 else if (ierror == E_NO_ERROR &&
	  is_rec_misc_choice ( &wkcell_type, FM_WRKCELL_WEDM ) )
 {
       if (is_sheet_metal_part (part_ptr)) /* Laser or Flame */
	 strcpy (descriptor, "C");
       else
	 strcpy (descriptor,"E");
 }
 else if (ierror == E_NO_ERROR &&
	  is_rec_misc_choice ( &wkcell_type, FM_WRKCELL_PUNCH ))
 {
        strcpy (descriptor,"P");
 }
 else if (ierror == E_NO_ERROR &&
          is_rec_misc_choice (&wkcell_type, FM_WRKCELL_HYBRID))
 {
    strcpy(descriptor, "[PC]");
 }
 else
        ierror = PTC_E_ABORT;

 END:

   return (ierror);
}

/*==========================================================================*/
 int remove_ncllp_after_cl_output (feat_ptr)
/*--------------------------------------------------------------------------*/
 Feat   *feat_ptr;
{
 Feat   *bld_oper;
 Part   *part_ptr;

 /* Clean up chain loop in nc sequence, cut motions and build path */
 if (should_free_tp_in_memory())
    cleanup_all_nc_seq_ncllps (feat_ptr);
 else  /* 3344 run mode is set */
    cleanup_ncseq_bp_ncllps (feat_ptr); /* keep ncseq, cutmtn, but remove bp */

 get_part_from_feat ( feat_ptr, &part_ptr );
 remove_loop_from_ncseq_misc_data ( part_ptr, feat_ptr );

 /* Clean up final tool path created from cl output */
 rem_nc_seq_final_tpath_loop (feat_ptr);

 /* Clean up head loop, tail loop, pattern loop etc */
 cleanup_all_nc_seq_raw_loops (feat_ptr);

 /* Clean up Build Operation feature chain loop */
 if (operation_has_bld_oper (feat_ptr, &bld_oper))
    cleanup_feat_ncllps (bld_oper);
 return ( 0 );
}

/*=========================================================================*/
 static int icam_read_dbfile (pfa, file_buffer, line_size)
/*-------------------------------------------------------------------------*/
 Pfa           *pfa;
 wchar_t     ***file_buffer;
 int            line_size;
{
 wchar_t        *new_line, wbuffer[512];
 char           buffer[512];
 int            out_size, get_something;

 if (pfa_fopen (pfa, THIS_VERSION, "r") != PFA_E_NO_ERROR)
 {
    dbg_err_syserr ("icam_read_dbfile", "cannot open file");
    return(E_CANT_OPEN);
 }

 get_something = FALSE;
 if ( *file_buffer == NULL )
    *file_buffer = (wchar_t **) xar_alloc (0, sizeof(wchar_t *), 30 );

 while (pfa_fgets (pfa, 511, &out_size, buffer)==PFA_E_NO_ERROR)
 {
    strtows (wbuffer, buffer);
    get_rid_of_special_characters (wbuffer);
    new_line = (wchar_t *) getmem ( sizeof (wchar_t) * line_size );
    wstrncpy (new_line, wbuffer, line_size );
    xar_append (file_buffer, 1, &new_line);
    get_something = TRUE;
 }
 pfa_close (pfa);

 return (get_something ? E_NO_ERROR : E_CANT_OPEN);
}


/*=========================================================================*/
 int set_icam_debug_runmode ()
/*-------------------------------------------------------------------------*/
/*
   1234 is the icam debug runmode, but environment variable "DEBUG_ICAM"
   can also be to trigger debug code.  To be consistent, we set runmode
   to 1234 if "DEBUG_ICAM" is set, so that debug code only need to check
   one thing.
*/
{
 if (is_mfg_debug_runmode ())
    return 0;   /* already set, nothing to do */

 if (BTK_GETENV_31_S ("DEBUG_NCPOST") != NULL)
    set_run_mode (RMODE_MFG_DBG_UI);
 return 0;
}

/*=========================================================================*/
static wchar_t *get_pro_mf_cl_dir_absolute()
/*-------------------------------------------------------------------------*/
/* Get the pro_mf_cl_dir config option after converting relative path to
   absolute (if possible) */
{
   static wchar_t  _mf_cl_dir[K_PATH_SIZE] = {'\0'};
   wchar_t *rel_mf_cl_dir = NULL;

   rel_mf_cl_dir = get_pro_mf_cl_dir_config();
   /* May be relative path as specified in config.pro e.g. ./posts */

   /* Convert relative path to absolute path */
   if (rel_mf_cl_dir != NULL &&
       !pro_get_full_directory_spec(rel_mf_cl_dir, _mf_cl_dir))
   {
      /* If path not found simply use that specified in config.pro */
      wstrcpy(_mf_cl_dir, rel_mf_cl_dir);
   }

   return (_mf_cl_dir);
}

/*=========================================================================*/
wchar_t *get_pro_mf_tape_dir_absolute()
/*-------------------------------------------------------------------------*/
/* Get the pro_mf_tape_dir config option after converting relative path to
   absolute (if possible) */
{
   static wchar_t  _mf_tape_dir[K_PATH_SIZE] = {'\0'};
   wchar_t *rel_mf_tape_dir = NULL;

   rel_mf_tape_dir = get_pro_mf_tape_dir_config();
   /* May be relative path as specified in config.pro e.g. ./posts */

   /* Convert relative path to absolute path */
   if (rel_mf_tape_dir != NULL &&
       !pro_get_full_directory_spec(rel_mf_tape_dir, _mf_tape_dir))
   {
      /* If path not found simply use that specified in config.pro */
      wstrcpy(_mf_tape_dir, rel_mf_tape_dir);
   }

   return (_mf_tape_dir);
}


/*=========================================================================*/
int reset_stat_final_loop()
/*-------------------------------------------------------------------------*/
{
 return ( allow_stat_final_loop( FALSE ) );
}

/*=========================================================================*/
int allow_stat_final_loop ( int allow_flag )
/*-------------------------------------------------------------------------*/
{
 stat_loop_storage_allowed = allow_flag;

 return ( E_NO_ERROR );
}

/*=========================================================================*/
int menu_cutter_path_storage_allowed ()
/*-------------------------------------------------------------------------*/
{
 return ( stat_loop_storage_allowed && !in_customize_dialog() );
}

/*=========================================================================*/
int set_stat_final_loop( Ncloop *chain_loop )
/*-------------------------------------------------------------------------*/
{
 if ( chain_loop != stat_final_loop )
 {
   release_stat_final_loop();
   stat_final_loop = chain_loop;
 }
 return ( 0 );
}

/*=========================================================================*/
int get_stat_final_loop( Ncloop **p_chain_loop )
/*-------------------------------------------------------------------------*/
{
 INIT_ARG( p_chain_loop, stat_final_loop );

 return ( stat_final_loop != NULL );
}

/*=========================================================================*/
int release_stat_final_loop()
/*-------------------------------------------------------------------------*/
{
 release_chain_loop( &stat_final_loop );

 return ( E_NO_ERROR );
}

static int submitToJobManWrapper(Feat *feat_ptr, Part* part_ptr,int flag)
{

 promenu_exit_up();
 promenu_exit_up(); 

 submitToJobMan(feat_ptr, part_ptr,0,flag);

 return(0);
}

static int done_output()
{
 if ( get_nc_jobman_visible() )
 nc_jobman_store_setname(NULL); 

 promenu_exit_up();
 promenu_exit_up();

 return(0);
}

int nc_jobman_display_tool_path( Mfg *mfg_ptr, Part *part_ptr, Feat *feat_ptr, wchar_t *set_name)
{
 int ierror;

 if(set_name)
 { 
   ierror = get_feat_w_outset_attr_low( mfg_ptr,part_ptr,
					set_name, &feat_ptr );
   if(ierror != E_NO_ERROR) return (ierror);

 }

 if ( get_clplayer_visible() )
    ierror = cdplayer (part_ptr, feat_ptr);
 else
    ierror = cutpath_display(mfg_ptr,part_ptr,feat_ptr,NULL);
 if(set_name)
   clean_up_after_output_by_set ( mfg_ptr, part_ptr, set_name);

 return( ierror );
}

static int mwsim_wrapper( Mfg *mfg_ptr, Part *part_ptr, Feat *feat_ptr)
{
 int *seq_ids = NULL, one_seq =1;

 promenu_exit_up();
 promenu_exit_up();

 seq_ids = XAR_BEGIN(int, 3);
 xar_append( &seq_ids, 1, &feat_ptr->id );

 nc_simulate_tool_path ( mfg_ptr, part_ptr,
     	                      seq_ids, one_seq, NC_MW_SYNC_PLAYPATH);
 
 return(0);
}

static int done_wrapper(Part *part_ptr, Feat *feat_ptr)
{
 
 promenu_exit_up();
 promenu_exit_up();
 
 cdplayer_(part_ptr, feat_ptr,K_NOT_USED);
 set_disp_force_compute_cl( FALSE );
 return(0);
}
int feat_oper_has_bldoper_synch_seqs ( Sld_prt *part_ptr,  Feat *oper_ptr )
{
  int *sfeat_ids = NULL;
  int  n_sfeats = 0;
  Feat  *bld_oper = NULL;

 if( get_operation_bld_oper( oper_ptr, &bld_oper ) )
 {
    sfeat_ids = XAR_BEGIN( int, 1 );
    n_sfeats = collect_sub_feats_of_type(part_ptr, bld_oper,
                                          SUBFEAT_BLD_OPER_SYNC, &sfeat_ids);        
     if( sfeat_ids != NULL )
           xar_free( (char **)&sfeat_ids );
     if( n_sfeats )
            return( TRUE );
}
            
      return FALSE;
}

/*=========================================================================*/
int display_cdp_for_oper ( Mfg *mfg_ptr, Sld_prt *part_ptr, 
				  Feat *oper_ptr , int *turn_4ax_exist )
/*-------------------------------------------------------------------------*/
{
 int            ierror = TRUE, output_by_set = FALSE, turn_4ax_flag = 0;
 int            *seq_ids_arr = NULL, ii = 0, jj = 0, seq_id = 0;
 int            *temp_seq_ids_arr = NULL, num_seq = 0;
 int            number_of_opers = 0, *feat_ids, n_sfeats = 0;
 wchar_t        *set_name = NULL;
 Feat           *cur_oper = NULL, *cur_feat = NULL;
 Feat           **oper_arr = XAR_BEGIN ( Feat*, 4 );

 if ( is_feat_type (oper_ptr, FT_OPERATION) )
 {
 	
 	if ( feat_oper_has_bldoper_synch_seqs ( part_ptr, oper_ptr ) )
 	   return FALSE;
 
    
   seq_ids_arr = XAR_BEGIN ( int, 4);
   if ( get_output_set_name_from_oper( oper_ptr, &set_name ) )
   {
     get_opers_by_set_name ( mfg_ptr, part_ptr, set_name, NULL, &oper_arr );
     number_of_opers = XAR_COUNT ( &oper_arr );
     output_by_set = TRUE;
   }
   if ( output_by_set )
   {
     for ( ii = 0; ii < number_of_opers; ii++ )
     {
       cur_oper = oper_arr[ii];
       if( oper_has_outset_name_assigned( cur_oper ) )
       {
          get_outset_seq_ids_by_oper( mfg_ptr, part_ptr, cur_oper,
                                      &temp_seq_ids_arr ) ;
          num_seq = XAR_COUNT ( &temp_seq_ids_arr );

          for ( jj = 0; jj<num_seq; jj++ )
          {
             seq_id = temp_seq_ids_arr[jj];
             xar_append (&seq_ids_arr, 1, &seq_id);
          }
          if ( num_seq )
           xar_free (&temp_seq_ids_arr);
       }
     }
     if ( number_of_opers )
       xar_free ( &oper_arr );
   }
   else
   {
     feat_ids = XAR_BEGIN(int, 4);
     collect_oper_nc_seqs ( oper_ptr, &feat_ids, COL_NCSEQ_CHECK_TP );
     num_seq = XAR_COUNT( (char **) &feat_ids );
     for ( jj = 0; jj<num_seq; jj++ )
     {
       seq_id = feat_ids[jj];
       xar_append (&seq_ids_arr, 1, &seq_id);
     }
     if ( num_seq )
        xar_free (&feat_ids);
   }
   num_seq = XAR_COUNT (&seq_ids_arr);
   if ( num_seq )
   {
     for ( jj = 0; jj<num_seq; jj++ )
     {
       cur_feat = (Feat *)AncppGetFeat( (Model *)part_ptr, seq_ids_arr[jj] );
       if ( cur_feat != NULL && 
            ( is_turn_4x_area( cur_feat ) || 
              is_4axis_step_turn_odui( cur_feat ) || 
             is_4axis_wedm_uv_type ( cur_feat ) ) )
       {   
					ierror  = FALSE;
			    if ( is_4axis_step_turn_odui ( cur_feat))
					{
						turn_4ax_flag = 1;
            if (turn_4ax_exist != NULL)
              *turn_4ax_exist = turn_4ax_flag;
					   break;
			    }
					 else 
						  continue;
       }
     }
     xar_free (&seq_ids_arr);
   }
 }
 else if ( is_turn_4x_area( oper_ptr ) || is_4axis_step_turn_odui ( oper_ptr) )
 {
  if ( is_4axis_step_turn_odui ( oper_ptr) && turn_4ax_exist != NULL )
    *turn_4ax_exist = TRUE;

  ierror = FALSE;
 }

 return ( ierror );
}

/*============================================================================*/
static void mfg_clfile_out_exclu_seq_show_warn (
      			Sld_prt		*part_ptr,
			int		nids,
			int		*ftids,
			int		*user_resp/*0 => close, 1 => ok, 2=> cancel*/
      			)
/*----------------------------------------------------------------------------*/
{
   int		i, nnames;
   Feat		*feat_ptr;
   wchar_t	name[K_LINESIZE];
   wchar_t	**names = (wchar_t **)xar_alloc(0, sizeof(wchar_t*), nids);

   INIT_ARG(user_resp, 0);


   for( i = 0; i < nids; ++i )
   {
      if( (feat_ptr = AncppGetFeat( (Model *) part_ptr, ftids[i] )) == NULL )
      {
	 continue;
      }

      if( !get_ncseq_name(part_ptr, feat_ptr, name, TRUE) )
      {
	 continue;
      }

      *((wchar_t**)xar_extend(&names,1)) = make_wscopy(name);
   }

   if( (nnames = xar_count(&names)) > 0 )
   {
      (void)mfg_msg_list_with_msgIDS(
	    		msg_ID1701, MFG_MSG_CLFILE_OUT_SEQUENCE_SKIP,
			nnames, names, MFG_BTN_LBL_CONTINUE, MFG_BTN_LBL_ABORT,
			user_resp, NULL);
   }


   xar_ptr_free((Pointer**)&names, relmem);
}

/*============================================================================*/
static int mfg_get_excluded_seq_from_list (
      				Sld_prt		*part_ptr,
				int		nfeats,
				Feat		**fts,
				int		**exftids
      				)
/*----------------------------------------------------------------------------*/
{
   int		i;
   int		*ids = XAR_BEGIN(int, 5), nids = 0;

   INIT_ARG(exftids, NULL);

   for( i = 0; i < nfeats; ++i )
   {
      if( feat_type(fts[i]) == FT_OPERATION )
      {
	 collect_oper_nc_seqs(fts[i], &ids, 0);
      }
      else
      {
	 xar_append(&ids, 1, &fts[i]->id);
      }
   }
   nids = xar_count(&ids);

   for( i = nids-1; i > -1 ; --i )
   {
      Feat	*feat_ptr = AncppGetFeat((Model*)part_ptr, ids[i]);
      if( !feat_ptr )
      {
	 continue;
      }
      /*if( allow_feat_for_cl_data_low(part_ptr, ids[i], TRUE, TRUE) )*/
      if( !is_empty_nc_seq(feat_ptr) && is_HSM_seq_STL_file_valid ( feat_ptr ) )
      {
	 /*valid seq remove from list*/
	 xar_remove(&ids, i, 1);
      }
   }

   if( (nids = xar_count(&ids)) < 1 )
   {
      xar_free(&ids);
   }
   INIT_ARG(exftids, ids);


   return nids;
}

/*============================================================================*/
int mfg_get_seqs_by_set_name (
      			Mfg		*mfg_ptr,
			Sld_prt		*part_ptr,
			wchar_t		*set_name,
			Feat		***fts, /*pre allocated xar*/
			Feat		**ids   /*pre allocated xar*/
      			)
/*----------------------------------------------------------------------------*/
{
   Feat		**opers = (Feat**)xar_alloc(0, sizeof(Feat*), 2);
   int		i, nopers, count = 0;
   
   get_opers_by_set_name ( mfg_ptr, part_ptr, set_name, NULL, &opers );
   nopers = xar_count(&opers);
   
   for( i = 0; i < nopers; ++i )
   {
      int	j, *seq_ids = NULL, nseqs;

      if( !oper_has_outset_name_assigned(opers[i]) )
	 continue;
      
      if( !get_outset_seq_ids_by_oper(mfg_ptr, part_ptr, opers[i], &seq_ids) )
	 continue;

      nseqs = xar_count(&seq_ids);

      for( j = 0; j < nseqs; ++j )
      {
	 Feat	*feat_ptr = AncppGetFeat((Model*)part_ptr, seq_ids[j]);
	 if( !feat_ptr )
	    continue;

	 if( fts )
	 {
	    xar_append(fts, 1, &feat_ptr);
	 }
	 if( ids )
	 {
	    xar_append(ids, 1, &feat_ptr->id);
	 }
	 ++count;
      }
      xar_free(&seq_ids);
   }

   xar_free(&opers);

   return count;

}

/*============================================================================*/
static int mfg_clfile_out_exclu_seq_warn (
      				Mfg		*mfg_ptr,
      				Sld_prt		*part_ptr,
				Feat		*feat_ptr,
				int		*user_resp/*0 => close, 1 => ok, 2=> cancel*/
      				)
/*----------------------------------------------------------------------------*/
{
   wchar_t	*set_name = NULL;
   Feat		**fts = xar_alloc(0, sizeof(Feat*), 2);
   int		num = 0, ierror = E_NO_ERROR, *exftids = NULL;
   
   if( get_output_set_name_from_oper( feat_ptr, &set_name ) )
   {
      num = mfg_get_seqs_by_set_name(mfg_ptr, part_ptr, set_name, &fts, NULL);
   }
   else
   {
      xar_append(&fts,1,&feat_ptr);
      num = 1;
   }
   
   num = mfg_get_excluded_seq_from_list(part_ptr, num, fts, &exftids);
   mfg_clfile_out_exclu_seq_show_warn(part_ptr, num, exftids, user_resp);

   xar_free(&fts);
   xar_free(&exftids);

   return ierror;
}

/** @brief check whether passing Mfg feat id generate proper toolpath or not.
*
*  This function check whether passing feat_id generate proper toolpath or not.
*  It check wheather passing feat_id has any set_name or not. If so, then collect
*  all feat* (ncseq/operation) from the set_name. After then it check for wrong
*  nc seq.
*
*  @param[in]   mfg_ptr the Mfg handle of the passing feat_id.
*  @param[in]   feat_id the feat id to check.
*
*  @return int : TRUE   -> if generate correct toolpath.
*                FALSE  -> if not.
*/
int isGenerateCorrectToolpath(
                                Mfg *mfg_ptr,
                                int feat_id
                             )
{
   Sld_prt  *part_ptr;
   Feat     *feat_ptr;
   wchar_t  *set_name = NULL;
   int      num = 0;
   int      ierror, status = FALSE;
   
   if(mfg_ptr != NULL)
   {
     /// get part ptr from Mfg *.
     ierror = ProMfgSolidGet(mfg_ptr, &part_ptr);
     if(ierror == PRO_TK_NO_ERROR)
     {
       feat_ptr = getFeat(part_ptr, feat_id);
       if(feat_ptr != NULL)
       {
         Feat   **fts = xar_alloc(0, sizeof(Feat*), 2);
         int    *exftids = NULL;

         /// 
         if( get_output_set_name_from_oper( feat_ptr, &set_name ) )
         {
            num = mfg_get_seqs_by_set_name(mfg_ptr, part_ptr, set_name, &fts, NULL);
         }
         else
         {
            xar_append(&fts,1,&feat_ptr);
            num = 1;
         }

         /// check seqs are valid or not, if not then collect them in 'exftids'.
         num = mfg_get_excluded_seq_from_list(part_ptr, num, fts, &exftids);
         if(num == 0)
           status = TRUE;

         xar_free(&fts);
         xar_free(&exftids);
       }
     }
   }

   return(status);
}

/** @brief Get Feat handle corresponding to the passing feat_id.
*
*  @param[in]   part_ptr the Sld_prt handle of the passing feat_id.
*  @param[in]   feat_id the feat id to get Feat*.
*
*  @return int : Feat*  -> corresponding Feat*.
*                NULL   -> if any error.
*/
Feat* getFeat(Sld_prt *part_ptr, int feat_id)
{
  Feat *feat_ptr = NULL;

  if(part_ptr != NULL)
  {
    if ((feat_id >= 0) && (feat_id < dbkey_count(part_ptr)))
    {
      feat_ptr = (feature_record *)dbkey_get((Model*)part_ptr, feat_id);
      if((feat_ptr != NULL) && (feat_ptr->type != T_FEATURE))
        feat_ptr = NULL;
    }
  }

  return(feat_ptr);
}

#undef TAP_FILE_EXT
