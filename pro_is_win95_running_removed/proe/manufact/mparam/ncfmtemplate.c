/*--------------------------------------------------------------------------*\
|
|  Module Details:
|
|  Name:    ncfmtemplate.c
|
|  Purpose: Functions related to template placement/creation in NCFM
|           Ref. ncfmtemplate.h
|
|  History:
|
|  Date      Release  Name  Ver.  Comments
|  --------- -------  ----- ----- --------------------------------------------
|  21-May-98 I-01-14  RR    $$1   Created
|  21-Aug-98 I-01-17  RR    $$2   New message MSG_TPL_VALUE_WARN included.
|  31-Aug-98 I-01-18  RR    $$3   Enabled comment lines and parameter-ignore.
|  12-Oct-98 I-01-22  RR    $$4   Feature selection based on template type.
|  20-Oct-98 I-01-24  RR    $$5   Config option for directory with .tpl files.
|  02-Nov-98 I-01-25  RR    $$6   select_mill_feat() called with assem_ptr.
|  10-Nov-98 I-01-26  RR    $$7   Environ var to debug - ncfm_template_home.
|  01-Dec-98 I-01-27  RR    $$8   Enabled holemaking through templates.
|  02-JAN-99 I-01-28  MB    $$9   Added OpenSlot & ThruPocket.
|  14-Jan-98 I-03-02  RR    $$10  is_holemaking initialized in
|                                 convert_file_to_input_table().
|  04-Feb-99 I-03-02  RR          Added Slab, Flange; Renamed OpenSlot->Channel
|  17-Feb-99 I-03-02  MB          Pass mill_feat to MfgTemplatePlace.
|  18-FEB-99 I-03-02  integ       Compile fix.
|  18-FEB-99 I-03-02  integ       more....
|  18-FEB-99 I-03-02  integ       Unstatic get_mfg_template_dir.
|  03-Mar-99 I-03-04  RR    $$11  Attribute MA_NCFM added to nc_seq.
|  31-MAR-99 I-03-08  adj   $$12  Changed value of PRO_NCSEQ_LOC_CORNER
|                                 to avoid conflict with NCFM_TRAJ
|  26-Apr-99 I-03-09  RR    $$13  Csys, Retract obtained from mill-feat
|                                 and not operation.
|  27-Apr-99 I-03-11 HMR    $$14  Changed Pro_file_path to Pro_split_path.
|  15-Jul-99 I-03-11  thlee $$15  Use wrapper function to get feat sectname.
|  15-JUL-99 I-03-11  SRI   $$16  Submitting to I-03.
|            I-01-35  SRI         Added support to read an XML format.
|                     EY          Cal. seq. at setup_default_mfg_params().
|                     SRI         Changed XML Format.
|                     SRI         Handled default negative (-1) params.
|                     SRI         called should_free_tp_in_memory().
|                     SRI         called find_and_replace().
|            I-03-11  EY          Cal. seq. at setup_default_mfg_params_low().
|  26-Jul-99 I-03-12  SRI   $$17  Added multiple template placement for
|                                 the same feature.
|                                 Fixed call to CreateMachining to turn
|                                 Traj false. Cleaned up some of the code.
| 08-Sep-99  I-03-13  RR    $$18 Added oring, thruslot.
| 14-Sep-99  I-03-15  SRI   $$19 Merged I-02 bugfixes to I-03
| 16-Sep-99  I-03-14  RR    19.1 ncfm_create_hole_ncseq() for holemaking.
| 17-SEP-99  I-03-14  EY    $$20 Updated oring.
| 24-SEP-99  I-03-16  YV    $$21 Added a parameter to MfgTemplatePlace()
|                                 altered the func to take a default filename.
|                                 Added parameters to create_template_features
|                                 and update_template_feature ().
| 19-OCT-99  I-03-19  YV    $$22 Changed the message for template placement.
|				 Killed looping for mimic.
| 20-Oct-99  I-03-19  RR    $$23  Extracted ncfmtemplate.h
| 21-OCT-99  I-03-19  BV    $$24 Changed calling seq. of
|                                pre_create_template_feature. Removed
|                                get_active_operation() and called
|                                get_nc_seq_operation() from
|                                pre_create_template_feature().
|                                Removed alloc_nc_seq() and called
|                                alloc_nc_seq_low()
|                                from pre_create_template_feature().
| 29-OCT-99  I-03-20  BV    $$25 Removed get_oper_trr_tool() and called
|                                ActiveTreeToolUpdate();
| 02-Nov-99  I-03-20  RR    $$26 Added ncfm_get_mfg_template_directory().
| 02-Nov-99  I-03-21  RR    $$27 Did my best to cleanup the messy formatting.
| 19-Nov-99  I-03-22  RR    $$28 Used ncfm_get_feat_type().
| 06-DEC-99  I-03-22  EY    $$29 Cal. seq. at setup_default_mfg_params().
| 30-Nov-99  I-03-23  SRI   $$30 Added TOP_ENTRY_STATE parameter loading.
| 13-Dec-99  I-03-23  RR    $$31 Used ncfm_delete_ncseq().
| 05-JAN-00  I-03-25  EY    $$32 Called assign_tool_to_seq().
| 15-Mar-00  J-01-05  RR    $$33 Added ncfm_get_mill_feat_type_wstr().
| 03-MAY-00  J-01-06  EY    $$34 Called CreateOrRedefineMachining().
| 20-JUN-00  J-01-11  SRI   $$35 Called set_mill_seq_round for round features.
| 07-JUL-00  J-01-12  SRI   $$36 Added placement for clcommand elements.
| 10-JUL-00  J-01-11  EY    $$37 Cal. seq. at find_tdia_and_tlen_from_tool().
| 01-Nov-00  J-01-21  JPE   $$38 WIN64 Port
| 08-NOV-00  J-01-21  SRI   $$39 Removed the special code for TOP_ENTRY_STATE.
| 07-MAR-01  J-01-26  EY    $$40 Passes nc_seq instead mill_feat to
|                                table_set_retract()(fixed crash placing pocket
|                                template).
| 15-JUL-01  J-01-35  mkh   $$41 Added update_seq_cutdata_for_template().
| 12-Sep-01  J-03-07  SAN   $$42 Modified HoleType Enum.
| 21-Sep-01  J-03-09  jas   $$43 Removed WINDOWS_95 macro
| 28-JAN-02  J-03-18  SNK   $$44 Adopted existing C++ XML parser.
| 12-APR-02  J-03-23  SNK   $$45 Modified template_com() for WINDOWS.
| 10-Oct-02  J-03-35  RSU   $$46 delete ncfm ncseq if workpiece gets vanished.
| 19-Nov-02  J-03-38  HMR   $$47 Simplified set_mfg_template_dir().
| 13-Jan-03  J-03-40  AKG   $$48 Called setup_clrnce_plane_info() #835405
| 24-MAR-03  K-01-03  SNK   $$49 Changed appl_func to old_appl_func.
| 26-SEP-03  K-01-16  SNK   $$50 Commented Template Manager call for Windows.
| 12-DEC-03  K-01-20  YVZ   $$51 Declared static int befor using:
|                                get_elemid_from_clcomm_param_type().
| 15-Apr-04  K-01-26  PJN   $$52 Purify SPRs 1077820,1078044 fix.
| 04-May-04  K-03-01 PMORK/CHI  $$53 Convert string handling calls to i18n_xxx()
| 13-May-04  K-03-01 PMORK/CHI  $$54 undoing #53
| 21-MAY-04  K-03-02  YVZ   $$55 Lint.
| 12-May-04  K-03-03  ASRS  $$56 X86E_WIN64 support
| 29-Dec-04  K-03-17  ssunder  $$57 Introduced get_tool_param_value.
| 05-FEB-05  K-03-19  sankulka $$58 Modified arguments.
| 07-Mar-05  K-03-21  SAN   $$59 Updated for drill group name.
| 12-Jul-05  K-03-28  ssabnis $$60 Prefix issue fixed.
| 12-Jul-06  L-01-12  ksi     $$61 Unicode compliant changes
| 24-Aug-06  L-01-16  gupatil $$62 Added access functions for static int 
|                                  cdplayer_shown. 
| 26-Feb-07  L-01-27  gupatil $$63 Purify related fix.
| 12-Mar-07  L-01-29  mkh     $$64 assgn_default_params_for_feat call seq.
| 20-Aug-07  L-01-37  aabramychev $$65 Fixed MLK in init_clcommand_tm_params().
| 26-FEB-08  L-03-03  YVZ     $$66 Added include.
| 20-Mar-08  L-03-05  PROTO       $$67 Automatic prototype creation
| 07-Jul-09  L-05-01  SAN     $$68 Updated.
| 15-Feb-10  L-05-16  rsurampu $$69 Fixed X86E_WIN64 issue.
| 14-Jan-12  P-10-16  SAN     $$70 Arg-list for assgn_default_params_for_feat().
| 24-Aug-13  P-20-37  mkh     $$71 Updated alloc_nc_seq_low call seq.
| 24-Jun-14  P-20-55  mkh     $$72 Called set_classic_mill_seq_vol().
| 20-Jul-17  P-50-21  PD	  $$73 Used get_feat_user_name_with_size
| 03-Sep-20  P-80-20  jas  $$74 Fixed printf-style formatting
| 01-Apr-21  P-90-04  jas  $$75 Fixed printf-style argument errors
| 02-Dec-21  P-90-37  Ahmad $$76 scrambled literal env vars
| 13-Mar-26  Q-27-01  jas   $$77 Use standard wide string functions
|  INSERT COMMENT ABOVE THIS LINE
|
\*--------------------------------------------------------------------------*/

#include <const.h>
#include <cu_msgutil_proto.h>
#include <tk_manufact_proto.h>
#include <proscanf.h>
#include <place.h>
#include <ProNcseq.h>
#include <ProNcseqElem.h>
#include <TreeApi.h>
#include <cr_junk.h>
#include <errors.h>
#include <feat_data.h>
#include <feature.h>
#include <mfg.h>
#include <MfgTree.h>
#include <mfgparam.h>
#include <modify.h>
#include <pfa.h>
#include <pro_sel_file.h>
#include <solid.h>
#include <subfeat.h>
#include <sysexecmd.h>
#include <utility.h>
#include <manufact_msg.h>
#include <xmlconst.h>
#include <ncfmtemplate.h>
#include <ncfmtplconst.h>
#include <tlprmutils.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include <btkcwchar.h>
#include <CxxWfunc.h>
#include <ProElement.h>
#include <ct_win_syscall_proto.h>
#include <ctfileutil_proto.h>
#include <ctstrutil_proto.h>
#include <ctsyscall_proto.h>
#include <cu_fileutils_proto.h>
#include <dbg_crash.h>
#include <featcheck_proto.h>
#include <featgen_proto.h>
#include <featlist_proto.h>
#include <featutil2_proto.h>
#include <featutil_proto.h>
#include <g_featlist_proto.h>
#include <g_featutil_proto.h>
#include <getputgl_proto.h>
#include <holeselem.h>
#include <machwind_pr.h>
#include <machwind_proto.h>
#include <mat_rem_proto.h>
#include <mctool_proto.h>
#include <mdltree_proto.h>
#include <menus2_proto.h>
#include <menusm_proto.h>
#include <mfg_proto.h>
#include <mfginit_proto.h>
#include <mfgutil_proto.h>
#include <millchn_proto.h>
#include <millpath.h>
#include <mkscpy.h>
#include <mparam_cpp_proto.h>
#include <mparam_proto.h>
#include <nc_seq_proto.h>
#include <nc_seq_ui_proto.h>
#include <ncsequtil_proto.h>
#include <new_mill_proto.h>
#include <oduikit_tk_solid_proto.h>
#include <param_proto.h>
#include <pro_string.h>
#include <pro_widec.h>
#include <promill_proto.h>
#include <proprintf.h>
#include <proselect_proto.h>
#include <ptc_xarray.h>
#include <runmode.h>
#include <runmode_proto.h>
#include <sub_feat_proto.h>
#include <sysstdlib.h>
#include <tool_proto.h>
#include <tparam_proto.h>
#include <update3d_proto.h>
#include <stdfeatname_defines.h>
#include <btkscale31.h>



#define TPL_HACKS    1
#define TPL_DEBUG    1

/*
  Template file identifiers
  -------- ---- -----------
*/
#define TPL_OPERAND_SEP    '='
#define TPL_KEY_SEP        '.'
#define TPL_COMMENT_START  '#'
#define TPL_TEMPLATE_DELIM "[template]"
#define TPL_STRATEGY_DELIM "[strategy]"
#define TPL_KEYWORD_IGNORE "__"  /* Keyword preceded by this is ignored */
#define TPL_KEYWORD_IGNORE_LEN 2 /* Length of previous string */
/*
  Template file keywords
  -------- ---- --------
*/
#define TPL_TEMPLATE_NAME  "name"
#define TPL_TEMPLATE_TYPE  "type"
#define TPL_STRATEGY_TYPE  "type"

/*
  Keywords for CLCommand Placed
*/
#define TPL_MF_YES "YES"
#define TPL_MF_NO "NO"

/*
  Template file format with the above identifiers
  -------- ---- ------ ---- --- ----- -----------

  #Lines beginning with '#' sign are comments,
  #and can be placed anywhere in file

  [template]
  #template block which needs to exist in the file. e.g.
  type = step
  #or face, profile, pocket, etc.

  [strategy]
  #one or more strategy blocks containing the following. e.g.
  type = milling
  #or drilling, etc., this line is ignored.

  #each line comprises of
  #<prefix1>.<prefix2>. . . .<param key word> = <param value>
  #a. <param key word> could be any parameter name e.g. STEP_DEPTH,
  #b. or one of the key words mentioned before e.g. FEAT_NAME, etc.
  #c. key word recognition is case insensitive, and
  #d. to ignore a <param key word> attach "__" at its start, e.g. __STEP_DEPTH
  #e.g. file with
  name.FEAT_NAME = Strategy0
  tool.TOOL_ID   = F1250-0
  MACH_NAME      = AIMILL
  MACH_ID        = M0001
  TOOL_ID        = F1250-0
  RETRACT_FEED   = -
  CUT_UNITS      = IPM
  __CUT_UNITS    = FPM
  #The previous line initialization is ignored because of __ prefix to key.
  RETRACT_UNITS  = IPM
  SCAN_TYPE      = SPIRAL_NOTCH

  #Note1. After start of [strategy] entire file assumed to be key-value pairs
  #    2. Preceeding the [strategy] block all other lines except [template]
  #       block are ignored.

  #Special cases and keywords (OPTIONAL)
  TOOL_OPTION = PROMPT
  #Prompts for the user to specify a tool. Overrides any TOOL_ID setting.
  sub_type = LOC_CORNER_MILL
  #appears after type, and is  specify that it's corner milling.


  General Implementation Notes
  ------- -------------- -----

  MfgTemplatePlace() -> Checks template compatibility with feature. Picks
  geometry features, and for each pick creates features by calling

  create_template_features() -> Starts reading the file, and loops over
  all the [strategy] blocks in the file. The loop inserts blank seq, and calls

  update_template_feature() -> Updates the empty feature with the definitions
  from input table, and some default params. The input table is created in

  convert_file_to_input_table() -> Loop reads each line of file, recognizes
  special Keywords (see prev comments under Keywords), and treates the rest
  as parameters. The information is stored in the input table.


  XML File Format
  ---------------

  The first line of an XML file should have a version and type(xml or not)
  identifier. Usually, it looks like this:-
  <?xml version="1.0" encoding="UTF-8"?>

  This is used to distinguish between the current TPL file format and XML.
  NOTE: Both the present TPL format and XML formats are saved with extension
  "tpl".
  Relevant funtions:
    static  int get_template_file_type(Pfa *pfa_file, int *file_type);
    static void set_ncfmtemplate_file_type(int type);
    static int get_ncfmtemplate_file_type();

  The global variable "file_type" can be accessed with the set/get functions
  above.
  The template file type is set in the function MfgTemplatePlace() and from
  there on the get_ncfmtemplate_file_type() is used to call the right code.

  The functions which distinguish between the two file types are:
     - create_template_features() - to find the template_type (STEP/FACING/etc)
     - convert_file_to_input_table() - as the name suggests!!
       This function has the max change. Except the variable declaration, the
       code is distinctly divided by the call to get_ncfmtemplate_file_type()


  The functions specific to xml parsing are:
   remove_xml_comments_in_line() - recursively removes xml comments
   get_xml_tag_value_from_string() - gets the value within the tag, right
    now gets the first occurence in the strin.
   get_xml_tag_value_from_file() - same as the previous, except from a file.
    The current position of the file is the start point. Also, expects  the
    file to be open when called.
   has_xml_head_tag_in_file() - Returns TRUE if the header tag was found in
    the rest of the file from the current position.
   has_xml_head_tag_in_string();
    - same as above, expect that this searches in a string.

*/

/* Initialize the cl_command_param_array. This array is used
   in convert_file_to_input_table() function to identify if the
   given parameter is a cl command param and other details of the
   param.
*/
static Mfg_param *cl_command_tm_param_array;
static int CL_COMMAND_TM_PARAM_NUMBER = 24;
static int set_use_nclcommand_strvals(Strval **r_clcommand_strvals);
static int update_seq_cutdata_for_template(Feat *nc_seq);
static int get_elemid_from_clcomm_param_type(int param_type);

static int line_number = 0;
static wchar_t mfg_template_dir[K_PATH_SIZE] = {NULL_WCHAR};
             /* Static set based on the config.pro option */
static int cdplayer_shown = FALSE;

/* Hole type mapping */
typedef struct holetype
{
   char *str;
   int val;
} HoleType;
static HoleType hole_type[] =
{
   {"PRO_E_HOLEMAKING_TYPE", PRO_E_HOLEMAKING_TYPE},
   {"PRO_E_HOLE_CYCLE_TYPE", PRO_E_HOLE_CYCLE_TYPE},
   {"PRO_E_HOLESET_DEPTH_BY_TYPE", PRO_E_HOLESET_DEPTH_BY_TYPE},
   {"PRO_E_HOLESET_DEPTH_TYPE", PRO_E_HOLESET_DEPTH_TYPE},
   {"PRO_E_HOLESET_PLATE_DEPTH", PRO_E_HOLESET_PLATE_DEPTH},
   {"PRO_E_HOLESET_PLATE_CDIAM", PRO_E_HOLESET_PLATE_CDIAM},
   {"PRO_E_HOLESET_EDGE_BREAK", PRO_E_HOLESET_EDGE_BREAK},
   {"PRO_E_HOLESET_SEL_AUTO_CHAMFER", PRO_E_HOLESET_SEL_AUTO_CHAMFER}
};
static int hole_type_num = sizeof (hole_type) / sizeof (HoleType);
static HoleType hole_value[] =
{
   {"PRO_HOLE_MK_CSINK", PRO_HOLE_MK_CSINK},
   {"PRO_HOLE_MK_DRILL", PRO_HOLE_MK_DRILL},
   {"PRO_HOLE_MK_BORE", PRO_HOLE_MK_BORE},
   {"PRO_HOLE_MK_TAP", PRO_HOLE_MK_TAP},
   {"PRO_HOLE_MK_FACE", PRO_HOLE_MK_FACE},
   {"PRO_HOLE_MK_REAM", PRO_HOLE_MK_REAM},
   {"PRO_HOLE_MK_CYCLE_STD", PRO_HOLE_MK_CYCLE_STD},
   {"PRO_HOLE_MK_CYCLE_DEEP", PRO_HOLE_MK_CYCLE_DEEP},
   {"PRO_HOLE_MK_CYCLE_FIXED", PRO_HOLE_MK_CYCLE_FIXED},
   {"PRO_HOLE_MK_CYCLE_FLOATING", PRO_HOLE_MK_CYCLE_FLOATING },
   {"PRO_HOLE_MK_CYCLE_BREAK_CHIP", PRO_HOLE_MK_CYCLE_BREAK_CHIP},
   {"PRO_HOLE_MK_CYCLE_WEB", PRO_HOLE_MK_CYCLE_WEB},
   {"PRO_HOLE_MK_CYCLE_BACK", PRO_HOLE_MK_CYCLE_BACK},
   {"PRO_DRILL_BLIND", PRO_DRILL_AUTO},
   {"PRO_DRILL_THRU_ALL", PRO_DRILL_THRU_ALL},
   {"PRO_DRILL_AUTO", PRO_DRILL_AUTO},
   {"PRO_DRILL_BY_SHOULDER", PRO_DRILL_BY_SHOULDER},
   {"PRO_DRILL_BY_TIP", PRO_DRILL_BY_TIP},
   {"PRO_TRUE", TRUE},
   {"PRO_FALSE", FALSE}
};
static int hole_value_num = sizeof (hole_value) / sizeof (HoleType);


/* Declarations to support XML format */
#define XML_TEMPLATE_FILE 1
#define TPL_TEMPLATE_FILE 2
static int file_type = TPL_TEMPLATE_FILE;
static void set_ncfmtemplate_file_type(int type);
static int get_ncfmtemplate_file_type(void);


/* Structure that stores template manager specific parameters which have
   no use in placing a template and hence will be discarded in
  convert_file_to_input_table function.
*/
typedef struct unused_tpl_mgr_params
{
 char *param_name;
}unused_tpl_mgr_params_db;

/* static defining initializing all the unused parameters */
static unused_tpl_mgr_params_db UNUSED_TM_PARAMS [] =
{
 { "USE_FIXT_OFFSET" },
 { "USE_SPINDLE_SPEED" },
 { "USE_FINISH_SPINDLE_SPEED" },
 { "USE_STEP_DEPTH" },
 { "USE_STEP_OVER" },
 { "USE_TOOL_OVERLAP" }
};

static int NUM_OF_UNUSED_TM_PARAMS =
  sizeof (UNUSED_TM_PARAMS) / sizeof (unused_tpl_mgr_params_db);


/* External functions */
static void find_and_replace(char *str, char *find, char *replace);
static int convert_text_to_lines(wchar_t *area_text, wchar_t ***r_lines);
static int init_clcommand_tm_params(void);

/* External functions defined in this file */
static int split_template_wstring(wchar_t *input_wstr, wchar_t *i_wstr, wchar_t *p_wstr, wchar_t *v_wstr);
static void reset_line_number(void);
static void increase_line_number(void);
static int get_line_number(void);

/* Internal functions */
static int create_template_features(Mfg *mfg_ptr, Solid *part_ptr, Feat *operation, Feat *mill_feat, int mill_feat_type, Pfa *pfa_file, int mimic_flag, Feat **p_nc_seq);
static int update_template_feature(Mfg *mfg_ptr, Solid *part_ptr, Feat *operation, Feat **p_nc_seq, Feat *mill_feat, int mill_feat_type, Pfa *pfa_file, int *r_found_new_strat, int mimic_flag);
static int pre_create_template_feature(Mfg *mfg_ptr, Feat *mill_feat, Solid *part_ptr, Feat **r_nc_seq, int feat_type, Feat **r_old_feat, Mfg_param **r_old_param_arr);
static int post_create_template_feature(Mfg *mfg_ptr, Solid *part_ptr, Feat *operation, Feat **p_nc_seq, Feat *old_feat, Mfg_param *old_param_arr, int feat_redef_err, int mill_feat_type);
static int convert_file_to_input_table(Mfg *mfg_ptr, Solid *part_ptr, Feat *operation, Feat *mill_feat, Feat *nc_seq, ProElement **r_input_table, Pfa *pfa_file, Tool **r_tool_ptr, int *r_found_new_strat, int mimic_flag, Bool *r_is_entry_hole);
static int get_template_file(Pfa *pfa_file);
static int get_template_type(Pfa *pfa_file, int *p_tpl_type, wchar_t **p_msg_wstring);
static int get_template_type_from_wstring(wchar_t *type_wstring, wchar_t **r_msg_wstring);
static int read_next_tpl_line(Pfa *pfa_file, int max_out_size, wchar_t *out_wstr);
static wchar_t *prune_space_wstring(wchar_t *v_wstr);
static char *template_com(void);

/* Xml Utility functions */
static int get_template_file_type(Pfa *pfa_file, int *file_type);
static int get_template_type_from_xml(Pfa *pfa_file, int *tpl_type, wchar_t **r_msg_wstring);
static int remove_xml_comments_in_line(char *in_str, int *bcomm, int *ecomm, char *uncomm_str);
static int read_next_xml_line(Pfa *pfa_file, int max_out_size, char *dest_str, int *bcomm, int *ecomm);
static int has_xml_head_tag_in_string(char *in_str, int *bcomm, int *ecomm, char *in_head_tag);

/* Top level function for creating template */
extern int MfgTemplateCreate (void)
{
   int     ierror, status = E_NO_ERROR;
   char    *com_str, template_com_str[K_PATH_SIZE] = {NULL_WCHAR};

   /* Get the path and executable command with required arguments to
      run template manager */
   com_str = template_com();
   strcpy(template_com_str, com_str);
   dbg_print_info("MfgTemplateCreate", "Command line : %s", template_com_str);

   if (template_com_str[0] == NULL_WCHAR)
   {
      dbg_err_syserr("MfgTemplateCreate", "Can't access template manager");
      ierror = E_ERROR;
      return (ierror);
   }

   {
      status = pro_system_call_new (&status, DETACHED_MODE, template_com_str);
   }

   if (status == K_NOT_USED)
   {
      dbg_err_syserr("MfgTemplateCreate", "Error processing java function");
   }

   return (status);
}


/* Function : MfgTemplatePlace
   Purpose  : Top level function for placing a template
   Input    : in_mill_feat - If NULL keeps prompting user for mill feature
                             to create templates. If not NULL creates toolpaths
                             only for the given mill feature
              file_name    - Template file to use to create tool path.
              mimic_flag   - no idea, except that it mimics.
              p_nc_seq     - Returns last created nc sequence.
   Return   :
   E_NO_ERROR - If feature creation proceeds to completion, or if user
                intentionally quits.
   E_INVALID_INPUT - If invalid input, or invalid template file
   E_ERROR - If feature got created okay, but error with redefinition.
*/
extern int MfgTemplatePlace (in_mill_feat, file_name, mimic_flag, p_nc_seq)
Feat    *in_mill_feat;
wchar_t *file_name;
int      mimic_flag;
Feat   **p_nc_seq;
{
   Mfg      *mfg_ptr = get_cur_mfg();
   Solid    *part_ptr = NULL;
   Feat     *operation = NULL, *mill_feat = NULL;
   int       ierror = E_NO_ERROR, pfa_err;
   int       tpl_type = K_NOT_USED, mill_feat_type, ok, template_file_type;
   Pfa      *pfa_file = NULL;
   wchar_t  *msg_wstring, mill_feat_name[PRO_FEAT_NAME_SIZE];
   char      fname_str[K_PATH_SIZE];

   /* Obtain the part and operation */
   get_oper_tree_data_gen ( NULL, &part_ptr, &operation, NULL, NULL );
   if ( operation == NULL )
   {
      msgID_put ( msg_ID1088 );
      return ( E_INVALID_INPUT );
   }

   /* Obtain the template file */
   (void) pfa_alloc_pro_file (&pfa_file);

   if (file_name == NULL)
      ierror = get_template_file (pfa_file);
   else
   {
      wstrtos (fname_str, file_name);
      pfa_err = pfa_parse_to_pro_file (pfa_file, fname_str);
      if (pfa_err != PFA_E_NO_ERROR)
	 ierror = E_ERROR;
   }

   if (ierror != E_NO_ERROR)
   {
      /* "Error reading template file" */
      msgID_put (MSG_TPL_FILE_ERR);
      dbg_err_syserr ("MfgTemplatePlace",
                      "Error parsing file (PFA_error code %d)", ierror);
      pfa_free_pro_file (&pfa_file);
      return (E_INVALID_INPUT);
   }
   fname_str[0] = NULL_CHAR;
   pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);

   /* Get The File Type - XML or TPL */
   ierror = get_template_file_type(pfa_file, &template_file_type);
   if(ierror != E_NO_ERROR)
   {
      pfa_free_pro_file(&pfa_file);
      return (E_INVALID_INPUT);
   }

   /* Set it in a global variable declared in this file */
   set_ncfmtemplate_file_type(template_file_type);

   /* Obtain the type of the template */
   ierror = get_template_type (pfa_file, &tpl_type, &msg_wstring);
   if (ierror != E_NO_ERROR)
   {
      pfa_free_pro_file(&pfa_file);
      ierror = E_INVALID_INPUT;
      return (ierror);
   }
   /* Initialize selection structure */

   ok = TRUE;

   /*
    * Pick mill features, and apply template to each pick
    * Terminates if user doesn't select anything and clicks DoneReturn
    */
   while (ok)
   {
      if ( in_mill_feat == NULL )
      {
         ok = select_mill_feat (mfg_ptr->assem_ptr, part_ptr, tpl_type,
                                &mill_feat);

         if (!ok || mill_feat == NULL)
         {
            ierror = E_NO_ERROR;
            dbg_print_info("MfgTemplatePlace", "invalid or no selection");
            break;
         }
      }
      else /* If a mill feature is passed in from the model tree */
      {
        ok = FALSE; /* don't select another feat, just quit loop after this */
        mill_feat = in_mill_feat;
      }

      /* Check applicablity of template to the mill feature */
      mill_feat_type = ncfm_get_feat_type (part_ptr, mill_feat);

      mill_feat_name[0] = NULL_WCHAR;
	  get_feat_user_name_with_size(part_ptr, mill_feat, mill_feat_name, GET_NAME_SIZE2(PRO_FEAT_NAME_SIZE, K_LINESIZE));

      if (mill_feat_type != tpl_type)
      {
         ierror = E_INVALID_INPUT;
         /* "Template %0s and feature %0w are incompatible." */
         msgID_put(MSG_TPL_INCOMPATIBLE_ERR, fname_str, mill_feat_name);
         dbg_print_info("MfgTemplatePlace",
                        "template and feature are not compatible");
         continue;
     }

      /* Convert the geometry feature into mfg feature */
      ierror = create_template_features (mfg_ptr, part_ptr, operation,
                                         mill_feat, mill_feat_type, pfa_file,
					 mimic_flag, p_nc_seq);

      if (mimic_flag)
	 break;
   }

   /* Free allocated memory and return */
   pfa_free_pro_file (&pfa_file);

   return (ierror);
}

/*
  Function : create_template_features
  Purpose  : Top level function for creating the template feature
  Comments :
  This function has been adapted from machstrat_finalize and portions
  of it should be extracted and used in both the places in the future
*/
static int create_template_features (mfg_ptr, part_ptr, operation,
                                     mill_feat, mill_feat_type,
                                     pfa_file, mimic_flag, p_nc_seq)
Mfg *mfg_ptr;
Solid *part_ptr;
Feat *operation;
Feat *mill_feat;
int mill_feat_type;
Pfa *pfa_file;
int mimic_flag;
Feat **p_nc_seq;
{
   int        ierror = E_NO_ERROR;
   int        feat_create_err = E_NO_ERROR, feat_redef_err = E_NO_ERROR;
   int        ii, strategy_num, total_startegies = 0;
   Feat      *nc_seq = NULL, *old_feat = NULL;
   wchar_t    str_delim_wstr[K_LINESIZE];
   wchar_t    buff_wstr[K_PATH_SIZE], param_wstr[K_PATH_SIZE];
   Bool       found_strategy_start;
   Mfg_param *old_param_arr = NULL;
   char       fname_str[K_LINESIZE];

   /* xml parsing specific variables */


   INIT_ARG(p_nc_seq, NULL);

   if(get_ncfmtemplate_file_type() == TPL_TEMPLATE_FILE)
   {
     /* Open the file */
     if (pfa_fopen(pfa_file, THIS_VERSION, "r") != PFA_E_NO_ERROR)
     {
        msgID_put(MSG_TPL_FILE_ERR); /* "Error reading template file" */
        dbg_err_syserr("get_template_type", "error reading template type");
        return (E_INVALID_INPUT);
     }
     /* get file name */
     fname_str[0] = NULL_CHAR;
     pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);
     /* reset the line number count */
     reset_line_number();

     /* Insert each [strategy] block in file as a new nc sequence */
     strtows(str_delim_wstr, TPL_STRATEGY_DELIM);
     /* find start of first nc sequence */
     found_strategy_start = FALSE;

     while (!found_strategy_start && read_next_tpl_line(pfa_file, K_PATH_SIZE,
                                                buff_wstr) == PFA_E_NO_ERROR)
     {
       split_template_wstring(buff_wstr, NULL, param_wstr, NULL);
       if (!wu_strcmp(param_wstr, str_delim_wstr))
         found_strategy_start = TRUE;
     }
     if (!found_strategy_start)
     {
       msgID_put(MSG_TPL_STRATBLK_ERR, fname_str);
       /* "[strategy] block not found in template file %0s."*/
        ierror = E_INVALID_INPUT;
     }

     /* insert all sequences identified by [strategy] blocks */
     while (found_strategy_start)
     {
       cdplayer_shown = FALSE;

        /* Inserts a blank nc_seq into the feature list */
       feat_create_err = pre_create_template_feature (mfg_ptr, mill_feat,
                                          part_ptr, &nc_seq, mill_feat_type,
                                          &old_feat, &old_param_arr);

       /* Updates the feature from the template file */
       /* found_strategy_start is TRUE if this [strategy] block is terminated
          by another [strategy] block, FALSE if terminated by end of file */
       feat_redef_err = update_template_feature (mfg_ptr, part_ptr, operation,
                                  &nc_seq, mill_feat, mill_feat_type, pfa_file,
                                  &found_strategy_start, mimic_flag);

       /* Unsets necessary static, etc. after feature creation */
       if (feat_create_err == E_NO_ERROR && nc_seq != NULL)
         post_create_template_feature(mfg_ptr, part_ptr, operation, &nc_seq,
                                      old_feat, old_param_arr, feat_redef_err,
                                      mill_feat_type);
     }

     /* Close the file */
     pfa_close(pfa_file);
   }

   else if( get_ncfmtemplate_file_type() == XML_TEMPLATE_FILE )
   {
     ierror  = get_no_of_strategies_from_xml( pfa_file, &total_startegies );
     if( ierror )
       return( ierror );

     for( ii = 0; ii<total_startegies; ii++ )
     {
       cdplayer_shown = FALSE;
 
        /* Inserts a blank nc_seq into the feature list */
       feat_create_err = pre_create_template_feature (mfg_ptr, mill_feat,
                                          part_ptr, &nc_seq, mill_feat_type,
                                          &old_feat, &old_param_arr);
 
       /* Updates the feature from the template file */
       strategy_num = ii;
       feat_redef_err = update_template_feature (mfg_ptr, part_ptr, operation,
                                  &nc_seq, mill_feat, mill_feat_type, pfa_file,
                                  &strategy_num, mimic_flag);
 
       /* Unsets necessary static, etc. after feature creation */
       if (feat_create_err == E_NO_ERROR && nc_seq != NULL)
         post_create_template_feature(mfg_ptr, part_ptr, operation, &nc_seq,
                                      old_feat, old_param_arr, feat_redef_err,
                                      mill_feat_type);
     }
   }

   /* Update the tree */
   ActiveTreeToolUpdate();


   if (feat_create_err != E_NO_ERROR)
      /* If feat creation itself has a problem return E_INVALID_INPUT */
      ierror = E_INVALID_INPUT;
   else if (feat_redef_err != E_NO_ERROR)
      /* If it's just a redefinition problem return E_ERROR */
      ierror = E_ERROR;

   INIT_ARG(p_nc_seq, nc_seq);

   return (ierror);
}

/* Creates an empty nc seq feature */
/* Returns E_NO_ERROR or E_ERROR */
static int pre_create_template_feature (Mfg *mfg_ptr, Feat *mill_feat,
		              Solid *part_ptr, Feat **r_nc_seq, int feat_type,
		              Feat **r_old_feat, Mfg_param **r_old_param_arr)
{
   int ierror = E_NO_ERROR, ind = K_NOT_USED, old_flag;
   Mfg_info *mfg_info_ptr;
   Feat *active_oper, *detach_feat, *nc_seq, *old_feat;
   int to_insert, sub_feat_id;
   Sub_feat *sub_feat;
   Mfg_param *old_param_arr;

   /*
    * Insert a new, empty nc_seq into the feature list
    */

   /* obtain active operation */
   if ( !get_nc_seq_operation ( part_ptr, mill_feat, &active_oper ) )
      return ( E_ERROR );

   /* find next operation if one exists, and set to insert nc_seq before it */
   detach_feat = active_oper;
   do
   {
      get_next_feat_quick (part_ptr, detach_feat, &detach_feat, &ind);
      to_insert = is_mfg_oper_feat(detach_feat);
   } while (detach_feat != NULL && !to_insert);
   if (to_insert)
      ierror = auto_activate_insert_mode_low (part_ptr, detach_feat, FALSE);

   /* create a blank feature */
   if (ierror == E_NO_ERROR)
   {
      if (feat_type == NCFM_HOLE)
      {

         /* alloc_nc_seq (mfg_ptr, part_ptr, PRO_FEAT_DRILL, &nc_seq); */
         ierror = ncfm_create_hole_ncseq (mfg_ptr, mfg_ptr->assem_ptr,
                                          active_oper, mill_feat,
                                          &nc_seq, NULL);
      }
      else
      {
         /* Mill case */
         alloc_nc_seq_low (mfg_ptr, part_ptr, active_oper, PRO_FEAT_MILL,
                           K_NOT_USED, &nc_seq);

         mark_feat_as_incomplete (nc_seq);
         put_mfg_attr( nc_seq, MA_NCFM );
         old_flag = set_feattop_ui_flag (FALSE);
         ierror = feattop_with_data (mfg_ptr->assem_ptr, part_ptr, K_NOT_USED,
                                     &nc_seq, NULL, NULL, 0);
         set_feattop_ui_flag (old_flag);
         if (ierror != E_NO_ERROR)
	    return (E_ERROR);
         sub_feat_id = add_sub_feat (nc_seq, SUBFEAT_MILL_FEAT_TYPE);
         get_sub_feat (nc_seq, sub_feat_id, &sub_feat);
         sub_feat->sub_type = feat_type;

         /* Set default attributes */
         switch (feat_type)
         {
         case FACING :
         case BOSSTOP :
            set_mill_seq_classic_face(part_ptr, nc_seq);
            break;
         case PROFILE :
            set_mill_seq_classic_prf(part_ptr, nc_seq);
            break;
         case TOPCHAMFER:
         case TOPROUND:
            set_mill_seq_round(part_ptr, nc_seq);
            break;
         case NCFMSTEP :
         case POCKET :
         case THRUPOCKET :
         case CHANNEL : /* Channel */
         case SLOT :
         case SLAB :
         case FLANGE :
         case ORING :
         case THRUSLOT :
         case UNDERCUT:
         case RIBTOP:
         default :
            set_classic_mill_seq_vol(part_ptr, nc_seq);
         }

         /* Assign default parameters to the feature*/
         get_mfg_info_ptr(nc_seq, &mfg_info_ptr);
         assgn_default_params_for_feat(nc_seq, &(mfg_info_ptr->param_arr),
                                       FALSE);
      }
   }

   /* unset the insert mode */
   if (to_insert)
      cancel_insert(part_ptr, FALSE);

   /* Static stuff for parameter update */
   if (ierror == E_NO_ERROR)
   {
      set_real_mfg_param_array(&old_param_arr);
      old_feat = (Feat *)set_cur_feat (nc_seq);
   }


   INIT_ARG(r_nc_seq, nc_seq);
   INIT_ARG(r_old_feat, old_feat);
   INIT_ARG(r_old_param_arr, old_param_arr);

   if (ierror != E_NO_ERROR)
      ierror = E_ERROR;

   return (ierror);
}

/*
 * Update the feature based on the template file - mostly adapted from
 * Elena's machstrat_finalize in MachStrat.cxx
 * The function returns E_ERROR if some error with feature redefinition,
 * E_NO_ERROR otherwise.
 * The flag *r_found_new_strat is set to TRUE if nc seq ([strategy]) definition
 * is terminated by a new [strategy], and is set to FALSE otherwise.
 */
static int update_template_feature (Mfg *mfg_ptr, Solid *part_ptr,
                                    Feat *operation, Feat **p_nc_seq,
                                    Feat *mill_feat, int mill_feat_type,
                                    Pfa *pfa_file, int *r_found_new_strat,
				    int mimic_flag)
{
   int         ierror = E_NO_ERROR;
   ProElement *input_table = NULL;
   Tool       *tool_ptr = NULL;
   double      tdia = 0.0, tlen = 0.0, twidth = 0.0;
   int         feat_redef_err; /* E_ERROR implies nc_seq needs
                                               to be redefined */
   Mfg_info    *mfg_info_ptr;
   Bool         is_holemaking = (mill_feat_type == NCFM_HOLE);
   Feat        *nc_seq;
   Bool         is_entry_hole = FALSE;

   if (p_nc_seq == NULL)
   {
      dbg_err_crash("update_template_features", "input nc_seq is NULL");
      return(PTC_E_ABORT);
   }
   nc_seq = *p_nc_seq;

   put_active_mill_window(mfg_ptr, mill_feat);


   /* File -> Input table conversion */
   feat_redef_err  = convert_file_to_input_table (mfg_ptr, part_ptr, operation,
                                      mill_feat, nc_seq, &input_table, pfa_file,
                                      &tool_ptr, r_found_new_strat,
                                      mimic_flag, &is_entry_hole);

   /* Error check 1 : Is tool specified?
      Error check 2 : Should user select EntryHole option in topentry?
      also find tdia, tlen to set the defaults to params */
   if(!is_entry_hole) 		/* Only check 1 */
   {
   if (tool_ptr == NULL)
   {
      dbg_print_info("update_template_feature",
                     "Please specify or define a tool");
      msgID_put(MSG_TPL_TOOL_SPECIFY);
      /* "Please specify or define a tool" */

      /* Set arbitrary tdia and tlen */
      tdia = tlen = 0.0;
   }
   else
     {
       find_tdia_and_tlen_from_tool (tool_ptr, &tdia, &tlen, &twidth);
       assign_tool_to_seq (nc_seq, FALSE, tool_ptr->tool_id, TRUE,
			   K_NOT_USED, FALSE);
     }
   setup_clrnce_plane_info (part_ptr, nc_seq);
   }
   else
   {
     if (tool_ptr == NULL)	 /* Check 1 and 2 */
     {
       dbg_print_info("update_template_feature",
                      "Please specify or define a tool");
       msgID_put(MSG_TPL_TOOL_SPECIFY);
       dbg_print_info("update_template_feature",
         "Please select Entry Hole option in the dialog");
       msgID_put(MSG_ENTRY_HOLE_SPECIFY);

       /* Set arbitrary tdia and tlen */
       tdia = tlen = 0.0;
     }
     else 			 /* check 2 only */
     {
       /* find the dia */
      find_tdia_and_tlen_from_tool (tool_ptr, &tdia, &tlen, &twidth);
      assign_tool_to_seq (nc_seq, FALSE, tool_ptr->tool_id, TRUE,
			  K_NOT_USED, FALSE);

       dbg_print_info("update_template_feature",
         "Please select Entry Hole option in the dialog");
       msgID_put(MSG_ENTRY_HOLE_SPECIFY);
   }
   }

   if (!is_holemaking)
   {
      /*
       * Setup EY's NCFM specific default parameters to avoid unspecified
       * parameter error
       */

      /*
       * calling EY's function - note that this is only a hack, and ideally
       * all these defaults should be set beforehand. However it has been
       * used here 'cos (1) error with parameters is not corrected properly,
       * (2) setting the correct default init_mparams would involve inhuman
       * work and (3) user group wants to see Feature Mill next week
       */
      setup_default_mfg_params_low(mill_feat,nc_seq, mill_feat_type, TRUE, tdia,
                                   tlen, twidth, &input_table);

      /*
       * Update feature with input table
       */

      /* Input table -> Feature conversion */
      ierror = convert_input_to_feat (nc_seq, input_table);

      if(ierror == UI_SUCCESS)
      {
         setup_clrnce_plane_info (part_ptr, nc_seq);
         set_parent_table (part_ptr, nc_seq);
      }

      /* Error check 2 : Are all params okay? */
      if (feat_redef_err == E_NO_ERROR)
      {
         if (get_mfg_info_ptr(nc_seq, &mfg_info_ptr))
         {
            /*
             * For now this check is included here - shoud call EY's redefine,
             * and her dialog should take care of this checking
             */
            feat_redef_err = check_mfg_params(nc_seq,
                                              &(mfg_info_ptr->param_arr));

            /* This check is called in auto_cut_for_feat_mill() */
            if (feat_redef_err == E_NO_ERROR &&
                !req_seq_refs_are_defined(nc_seq, CR_CREATE))
               feat_redef_err = E_ERROR;

         }
         else
            feat_redef_err = E_ERROR;
      }
   }
   else
   {
      /* Sort the input table */
      xar_sort(&input_table, hole_template_cmp_function);
      feat_redef_err = convert_hole_input_to_feat (&nc_seq, input_table);
   }

   update_seq_cutdata_for_template( nc_seq );

   if(input_table != NULL)
       ProElemElemArrValueFree(&input_table);
   
   /* Redefine the feature through machining dialog if there's an error */
   if (feat_redef_err != E_NO_ERROR || ierror != E_NO_ERROR || mimic_flag)
   {
      dbg_print_info("update_template_feature", "error in definition");

      if ( get_run_mode() == 590627 )
         /* NOTE: this is a reuse of a PPfM debug run mode! */
         feature_info( nc_seq->id, 0 );

      if (!is_holemaking)
         feat_redef_err = CreateOrRedefineMachining (mfg_ptr, operation,
                                 nc_seq, NULL, CR_REDEFINE, 2, 0, 0, 0, NULL);
      else
      {
         clear_feat_incomplete(nc_seq);
         feat_redef_err = menu_redefine( part_ptr, &nc_seq, E_NO_ERROR, NULL,
                                         CR_RECREATE, MOD_ACT_REDEF_REF );
      }
      cdplayer_shown = TRUE;
   }

   INIT_ARG(p_nc_seq, nc_seq);
   return (feat_redef_err);
}

/* Finish feature creation */
static int post_create_template_feature(Mfg *mfg_ptr, Solid *part_ptr,
                                      Feat *operation, Feat **p_nc_seq,
                                      Feat *old_feat, Mfg_param *old_param_arr,
                                      int feat_redef_err, int mill_feat_type)
{
   int ierror = E_NO_ERROR;
   (void)operation;

   if (p_nc_seq == NULL || *p_nc_seq == NULL)
   {
      dbg_err_crash("post_create_template_feature", "invalid input");
      return (PTC_E_ABORT);
   }

   /* Redefine feature in case of error with input - should ideally
      call with input table */
   if (feat_redef_err != E_NO_ERROR)
   {
      /* Delete if even redefine fails */
      ncfm_delete_ncseq(part_ptr, *p_nc_seq);
   }
   else
   {
      if (mill_feat_type != NCFM_HOLE)
         set_empty_nc_seq (*p_nc_seq, FALSE);

      ierror = auto_cut_for_feat_mill ( part_ptr, *p_nc_seq, CR_CREATE );

      /* Show the tool path */
      if ( !cdplayer_shown && ierror == E_NO_ERROR &&
           nc_seq_ready_for_tpath (*p_nc_seq) )
      {
         clear_feat_incomplete ( *p_nc_seq );
         cdplayer ( part_ptr, *p_nc_seq );
      }

    regen_from_specified_feat ( part_ptr, *p_nc_seq );
    if (should_free_tp_in_memory() && !is_feat_delsupp(part_ptr,
                                                       (*p_nc_seq)->id))
       _cleanup_all_nc_seq_ncllps (*p_nc_seq);

    if (!nc_seq_has_mat_rem (*p_nc_seq)) {
	ierror = create_matl_removal (mfg_ptr, part_ptr, *p_nc_seq, NULL);
	if(ierror == E_NO_MATERIAL) {
	    /* Workpiece has become null; Delete the feature. */
	    ncfm_delete_ncseq(part_ptr, *p_nc_seq);
	    msgID_put(msgWorkpieceDisappeared);
	    ierror = PTC_E_ABORT;
	}
    }
   }

   /* Unset static variables */
   set_cur_feat (old_feat);
   set_mfg_param_array(old_param_arr);

   return (ierror);
}

/* Adds the template input from the file into the input table */
/* The function returns
   E_ERROR     : if the error wants a redefine.
   PTC_E_ABORT : crash error.
   E_NO_ERROR  : otherwise.
   The flag *r_found_new_strat is TRUE if the sequence definition is
   terminated by a new sequence ([strategy]) definition and FALSE otherwise.
*/
static int convert_file_to_input_table (Mfg *mfg_ptr, Solid *part_ptr,
                                        Feat *operation, Feat *mill_feat,
                                        Feat *nc_seq,
                                        ProElement **r_input_table,
                                        Pfa *pfa_file, Tool **r_tool_ptr,
                                        int *r_found_new_strat,
                                        int mimic_flag,
					Bool *r_is_entry_hole)
{
   ProElement *input_table;
   Tool *tool_ptr;
   Mfg_param  *mpar_ptr;
   Param **param_arr = NULL;
   Select3d *mach_csys;

   int line_num, ierror = E_NO_ERROR, found_new_strategy, strategy_num;
   int param_type, mpar_idx, mill_feat_type, mill_type, default_mill_type;
   int param_intval, value_intval, gen_feat_type;
   int feat_redef_err = E_NO_ERROR, *values;

   Bool is_holemaking = FALSE, prompt_for_tool = FALSE;
   Bool is_entry_hole = FALSE;

   double value_dval, tdia, tlen, twidth;

   wchar_t buff_wstr[K_PATH_SIZE], *mill_feat_name;
   wchar_t str_delim_wstr[K_NAME_SIZE], str_type_wstr[K_NAME_SIZE];
   wchar_t str_key_ignore_wstr[K_NAME_SIZE];
   wchar_t param_wstr[K_PATH_SIZE], val_wstr[K_PATH_SIZE];
   wchar_t feat_name_wstr[K_LINESIZE];

   char fname_str[K_LINESIZE];
   
   /* Obtain the file name */
   fname_str[0] = NULL_CHAR;
   pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);

   /* Obtain mill feature type e.g. FACING, NCFMSTEP, NCFM_HOLE etc. */
   mill_feat_type = ncfm_get_feat_type (part_ptr, mill_feat);
   if (is_feat_mill_drl_grp(mill_feat) || is_entry_hole_feat(mill_feat))
      is_holemaking = TRUE;

   /* initialize the local cl command parameter array */
   init_clcommand_tm_params();

   /* Mill type initialization - corner mill option taken care of later */
   switch (mill_feat_type)
   {
   case FACING :
   case BOSSTOP :
      mill_type = default_mill_type = PRO_NCSEQ_FACE_MILL;
      break;
   case NCFM_HOLE :
      {
         gen_feat_type = PRO_FEAT_DRILL;
         mill_type = default_mill_type = PRO_NCSEQ_HOLEMAKING;
      }
      break;
   default :
      mill_type = default_mill_type = PRO_NCSEQ_VOL_MILL;
   }

   /* Create Input table */
   input_table = (ProElement *) XAR_BEGIN (ProElement, 4);

   /* Mill window id */
   if (is_holemaking)
   {
      values = XAR_BEGIN (int, 1);
      add_hole_elem_to_input_table(&input_table,
                                   PRO_E_HOLESET_SEL_DRILL_GROUPS,
                                   (void *) &(mill_feat->id),
                                   PRO_VALUE_TYPE_INT, 1);
      xar_free(&values);
   }
   else
      add_to_input_table (&input_table, PRO_E_MACH_WINDOW,
                          (void *)&(mill_feat->id), 1);

   /* If default template placement, or mimic tool path, tool shouldn't be
      read in */
   if (mimic_flag)
       prompt_for_tool = TRUE;

   /* Parse file to update ...  */
   found_new_strategy = FALSE;
   feat_name_wstr[0] = NULL_WCHAR;
   tool_ptr = NULL;

   /* parse based on template file type - tpl or xml */
   if(get_ncfmtemplate_file_type() == TPL_TEMPLATE_FILE)
   {

    /* Various identifier strings in the template file */
    strtows(str_delim_wstr, TPL_STRATEGY_DELIM); /* Strategy "[strategy]" */
    strtows(str_type_wstr,  TPL_STRATEGY_TYPE);  /* Strategy "type" */
    strtows(str_key_ignore_wstr, TPL_KEYWORD_IGNORE); /* "__" ignore keyword */

    while (!found_new_strategy && read_next_tpl_line(pfa_file, K_PATH_SIZE,
                                                buff_wstr) == PFA_E_NO_ERROR)
    {
      line_num = get_line_number();

      split_template_wstring(buff_wstr, NULL, param_wstr, val_wstr);
      if (!wu_strcmp(param_wstr, str_delim_wstr))
      {
         /* ... nothing. Stop update since a new strategy is found  */
         found_new_strategy = TRUE;
         continue;
      }
      else if (!wu_strncmp(param_wstr, str_key_ignore_wstr,
                           TPL_KEYWORD_IGNORE_LEN))
      {
         /* ... nothing. Any param with prefix '__' is to be ignored */
         continue;
      }
      else if (is_holemaking && !wu_strncmp(param_wstr, str_hole_param_prefix,
                                             TPL_HOLE_PARAM_PREFIX_LEN))
      {

         ierror = get_hole_type_from_wstr (param_wstr, TRUE, &param_intval);
         /* Only two types of RHS allowed (a) int e.g. see "PRO_" strings
            and (b) double */
         if (ierror == E_NO_ERROR)
            if(!wu_strncmp(val_wstr, str_hole_param_prefix,
                           TPL_HOLE_PARAM_PREFIX_LEN))
            {
               ierror = get_hole_type_from_wstr(val_wstr, FALSE,
                                                &value_intval);
               add_hole_elem_to_input_table (&input_table, param_intval,
                                             (void *) &value_intval,
                                             PRO_VALUE_TYPE_INT, 0);
            }
            else
            {
               value_dval = atof(str_ret(val_wstr));
               add_hole_elem_to_input_table (&input_table, param_intval,
                                             (void *) &value_dval,
                                             PRO_VALUE_TYPE_DOUBLE, 0);
            }

         if (ierror != E_NO_ERROR)
         {
            dbg_print_info("convert_file_to_input_table",
                           "Keyword-value pair in line %d unrecognized",
                           line_num);
            msgID_put(MSG_TPL_VALUE_WARN, &line_num, fname_str);
            disable_replace_next_msg(); /* Covering up for the stupid msg
                                           system */
            continue;
         }
      }
      else if (!wu_strcmp(param_wstr, str_name_wstr))
      {
         /* ... Feature name  */

         /* get mill feat name */
         mill_feat_name = get_mill_window_name ((Model *)part_ptr,
                                                   mill_feat->id);

         /* create a new feature name  */
         /* for now this is based on mill_feat_name only, it can be changed
            to depend on val_wstr i.e. strategy name also */
         get_default_tp_name(part_ptr, mill_feat_name, feat_name_wstr);
         if (is_holemaking)
            add_hole_elem_to_input_table (&input_table, PRO_E_FEAT_NAME,
                                          (void *)feat_name_wstr,
                                          PRO_VALUE_TYPE_WSTRING, 0);
         else
            add_to_input_table (&input_table, PRO_E_FEAT_NAME,
                                (void *)feat_name_wstr, 1);
      }
      else if (!wu_strcmp(param_wstr, str_topt_wstr))
      {
         /* ... TOOL_OPTION */

         /* If PROMPT is marked for TOOL_OPTION this overrides any tool
            specified later or before in strategy block */
         if (!wu_strcmp(val_wstr, str_topt_prompt_wstr))
         {
            /* Prompt for the tool */
            prompt_for_tool = TRUE;
            tool_ptr = NULL;
         }

      }
      else if (!wu_strcmp(param_wstr, str_tid_wstr))
      {
         /* ... Tool ID */

         /* if prompt of tool has been requested already don't use the tool
            specified */
         if (prompt_for_tool)
            continue;

         /* check for blank tool string */
         if (blankstr(val_wstr))
         {
            dbg_print_info("convert_file_to_input_table",
                           "No tool specified in line %d", line_num);
            continue;
         }
         /* check for duplicate tool */
         if (tool_ptr != NULL)
         {
            dbg_print_info("convert_file_to_input_table",
                       "duplicate tool in line %d will be ignored", line_num);
            continue;
         }
         /* obtain the tool_ptr from the manufacturing model */
         ierror =  get_tool_from_mfg(mfg_ptr, val_wstr,
                                     (char **) &tool_ptr);
         /* check if tool is in mfg model */
         if (ierror == E_NOT_FOUND || tool_ptr == NULL)
         {
            dbg_print_info("convert_file_to_input_table",
                           "Tool ID %ws not found", val_wstr);
            msgID_put(MSG_TPL_TOOL_GIVEN_ERR, val_wstr, &line_num, fname_str);
            /* "Tool %0w specified in line %d (file %0s) is not available." */
            continue;
         }
         /* everything okay - add tool to input table */
         if (is_holemaking)
            add_hole_elem_to_input_table (&input_table, PRO_E_TOOL,
                                          (void *)val_wstr,
                                          PRO_VALUE_TYPE_WSTRING, 0);
         else
            add_to_input_table (&input_table, PRO_E_TOOL, (void *)val_wstr, 1);
      }
      else if (!wu_strcmp(param_wstr, str_type_wstr))
      {
         /* ... nothing. Strategy type/sub-type string to be ignored */
         continue;
      }
      else if (!wu_strcmp (param_wstr, str_subtype_wstr))
      {
         /* ... corner mill option. set sub_type */
         if (!wu_strcmp (val_wstr, str_subtype_corner_wstr))
            mill_type = PRO_NCSEQ_LOC_CORNER_MILL;
         else
            mill_type = default_mill_type;

         continue;
      }
      else
      {
         /* .... a parameter. By default input assumed to be a parameter */

         /* check for blank string */
         if (blankstr(val_wstr))
         {
            dbg_print_info("convert_file_to_input_table",
                           "Blank value in line %d", line_num);
            continue;
         }

         mpar_idx = get_mfg_prm_ptr_from_keyword(param_wstr, &param_type,
                                                 &mpar_ptr);
         /* check if parameter is found */
         if (mpar_idx == K_NOT_USED)
         {

            /* ignore if template_manager specific param */
            if(is_template_manager_only_param(param_wstr))
              continue;
            else
	    {
             msgID_put(MSG_TPL_PARAM_WARN, param_wstr, &line_num, fname_str);
             /* "Keyword %0w unrecognized in line %1d of template file %2s." */
             disable_replace_next_msg(); /* Covering up for the stupid msg
                                           system */
            dbg_print_info("convert_file_to_input_table",
                "Parameter %ws in line %d unrecognized", param_wstr, line_num);
            continue;
	    }
         }

         ierror = assign_mfg_param_value (val_wstr, param_wstr,
                                          mpar_ptr->data_type, &param_arr);
         if (ierror != E_NO_ERROR)
         {
            dbg_print_info("convert_file_to_input_table",
                           "Keyword-value pair in line %d unrecognized",
                           line_num);
            msgID_put(MSG_TPL_VALUE_WARN, &line_num, fname_str);
            disable_replace_next_msg(); /* Covering up for the stupid msg
                                           system */
         }
      }
    }
   } /* End TPL file parse */
   else if (get_ncfmtemplate_file_type() == XML_TEMPLATE_FILE)
   {
     strategy_num = *r_found_new_strat;
     ierror = create_input_table_from_xml( mfg_ptr, part_ptr, mill_feat, is_holemaking, &default_mill_type,
                                  	   strategy_num, pfa_file, &param_arr, &tool_ptr, &input_table );
     if( ierror )
       return ierror;
   }


   /* Complete check : Has tool been specified? */
   if (tool_ptr == NULL)
   {
      if (!prompt_for_tool)
      {
         dbg_print_info("convert_file_to_input_table",
                        "No valid tool specified in file %0s", fname_str);
         msgID_put(MSG_TPL_TOOL_NONE_ERR, fname_str);
         /* "No valid tool specified in file %0s." */
      }

      feat_redef_err = E_ERROR; /* No tool, but continue till end of fn */
      tdia = tlen = 0.0;
   }
   else
      find_tdia_and_tlen_from_tool (tool_ptr, &tdia, &tlen, &twidth);

   /* Complete update of all parameters */
   if (xar_count ((char **)&param_arr) > 0)
      if (is_holemaking)
         add_hole_params_to_input_table(&input_table, &param_arr, tdia, tlen,
                                        mfg_ptr);
      else
         add_to_input_table (&input_table, PRO_E_PARAMS,
                             (void *)&param_arr, 1);

   free_param_arr(&param_arr);


   /* Feature type and sub-type update */
   if (is_holemaking)
   {
      add_hole_elem_to_input_table (&input_table, PRO_E_FEATURE_TYPE,
                                    (void *)(&gen_feat_type),
                                    PRO_VALUE_TYPE_INT, 0);
      add_hole_elem_to_input_table (&input_table, PRO_E_NCSEQ_TYPE,
                                    (void *)(&mill_type),
                                    PRO_VALUE_TYPE_INT, 0);
   }
   else
      table_set_nc_seq_type ( NULL, NULL, NULL, mill_type, &input_table );

   /* Operation update */
   if (is_holemaking)
      add_hole_elem_to_input_table (&input_table, PRO_E_OPERATION,
                                    (void *)(&(operation->id)),
                                    PRO_VALUE_TYPE_INT, 0);
   else
      add_to_input_table (&input_table, PRO_E_OPERATION,
                          (void *)&operation->id, 1 );

   /* Csys update */
   if (is_holemaking)
   {
      mach_csys = (Select3d *) getmem (sizeof (Select3d));

      if (get_oper_csys(part_ptr, mill_feat, mach_csys))
         add_hole_elem_to_input_table(&input_table, PRO_E_CSYS,
                                      (void *)(mach_csys),
                                      PRO_VALUE_TYPE_SELECTION, 0);
      else
         dbg_print_info("convert_file_to_input_table", "no Csys found");

   }
   else
      ierror = table_set_csys_no_ui ( part_ptr, mill_feat, &input_table );
   if (ierror != E_NO_ERROR)
      return (ierror);

   /* Retract plane update */
   if (!is_holemaking)
      ierror = table_set_retract( part_ptr, nc_seq, &input_table );
   if (ierror != E_NO_ERROR)
      return (ierror);

   /* Initialize output arguments */
   INIT_ARG(r_input_table, input_table);
   INIT_ARG(r_found_new_strat, found_new_strategy);
   /* Another sequence needs to be processed */
   INIT_ARG(r_tool_ptr, tool_ptr);

   /* entry_hole for features with top entry only*/
   INIT_ARG(r_is_entry_hole, is_entry_hole);

   return (feat_redef_err);
}


static int init_clcommand_tm_params()
{
 static Strval *clcommand_strvals = NULL;

 int index = 0;

 if (clcommand_strvals == NULL)
    set_use_nclcommand_strvals (&clcommand_strvals);

 xar_free (&cl_command_tm_param_array);

 cl_command_tm_param_array = (Mfg_param *)
         xar_alloc(CL_COMMAND_TM_PARAM_NUMBER, sizeof(Mfg_param), 1);

 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_BEFORE_LOADTL");
 cl_command_tm_param_array[index].param_type =TM_USE_BEFORE_LOADTL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_BEFORE_LOADTL");
 cl_command_tm_param_array[index].param_type = TM_TEXT_BEFORE_LOADTL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_BEFORE_LOADTL");
 cl_command_tm_param_array[index].param_type = TM_SIZE_BEFORE_LOADTL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_AFTER_LOADTL");
 cl_command_tm_param_array[index].param_type =TM_USE_AFTER_LOADTL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_AFTER_LOADTL");
 cl_command_tm_param_array[index].param_type = TM_TEXT_AFTER_LOADTL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_AFTER_LOADTL");
 cl_command_tm_param_array[index].param_type = TM_SIZE_AFTER_LOADTL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;



 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_AFTER_SPINDL");
 cl_command_tm_param_array[index].param_type =TM_USE_AFTER_SPINDL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_AFTER_SPINDL");
 cl_command_tm_param_array[index].param_type = TM_TEXT_AFTER_SPINDL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_AFTER_SPINDL");
 cl_command_tm_param_array[index].param_type = TM_SIZE_AFTER_SPINDL;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;



 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_AFTER_FROM");
 cl_command_tm_param_array[index].param_type =TM_USE_AFTER_FROM;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_AFTER_FROM");
 cl_command_tm_param_array[index].param_type = TM_TEXT_AFTER_FROM;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_AFTER_FROM");
 cl_command_tm_param_array[index].param_type = TM_SIZE_AFTER_FROM;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;




 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_BEFORE_EACH_PASS");
 cl_command_tm_param_array[index].param_type =TM_USE_BEFORE_EACH_PASS;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_BEFORE_EACH_PASS");
 cl_command_tm_param_array[index].param_type = TM_TEXT_BEFORE_EACH_PASS;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_BEFORE_EACH_PASS");
 cl_command_tm_param_array[index].param_type = TM_SIZE_BEFORE_EACH_PASS;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;




 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_AFTER_EACH_PASS");
 cl_command_tm_param_array[index].param_type =TM_USE_AFTER_EACH_PASS;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_AFTER_EACH_PASS");
 cl_command_tm_param_array[index].param_type = TM_TEXT_AFTER_EACH_PASS;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_AFTER_EACH_PASS");
 cl_command_tm_param_array[index].param_type = TM_SIZE_AFTER_EACH_PASS;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;



 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_AFTER_FINAL_RETRACT");
 cl_command_tm_param_array[index].param_type =TM_USE_AFTER_FINAL_RETRACT;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_AFTER_FINAL_RETRACT");
 cl_command_tm_param_array[index].param_type = TM_TEXT_AFTER_FINAL_RETRACT;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_AFTER_FINAL_RETRACT");
 cl_command_tm_param_array[index].param_type = TM_SIZE_AFTER_FINAL_RETRACT;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_USE_AFTER_GOHOME");
 cl_command_tm_param_array[index].param_type =TM_USE_AFTER_GOHOME;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%d", TM_MF_NO);
 cl_command_tm_param_array[index].string_values = clcommand_strvals;
 cl_command_tm_param_array[index].data_type = MPAR_STRVAL;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_TEXT_AFTER_GOHOME");
 cl_command_tm_param_array[index].param_type = TM_TEXT_AFTER_GOHOME;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 strcpy (cl_command_tm_param_array[index].def_val, "-");
 cl_command_tm_param_array[index].data_type = MPAR_STRING;
 cl_command_tm_param_array[index].aliases = NULL;


 index++;
 strcpy (cl_command_tm_param_array[index].keyword, "TM_SIZE_AFTER_GOHOME");
 cl_command_tm_param_array[index].param_type = TM_SIZE_AFTER_GOHOME;
 cl_command_tm_param_array[index].param_group = GROUP_NAMES;
 btk_sprintf (cl_command_tm_param_array[index].def_val,"%1f", 1.0);
 cl_command_tm_param_array[index].data_type = MPAR_DOUBLE;
 cl_command_tm_param_array[index].string_values = NULL;
 cl_command_tm_param_array[index].aliases = NULL;


 return 0;
}

/*-------------------------------------------------------------------------*/
static int set_use_nclcommand_strvals (Strval **r_clcommand_strvals)
/*-------------------------------------------------------------------------*/
{
 Strval      *clcommand_strvals;

 clcommand_strvals = (Strval *)xar_alloc(0, sizeof(Strval), 4);

 add_strval (&clcommand_strvals, "YES", TM_MF_YES);
 add_strval (&clcommand_strvals, "NO", TM_MF_NO);

 INIT_ARG (r_clcommand_strvals, clcommand_strvals);
 return(E_NO_ERROR);

}

int is_cl_command_param(wchar_t *keyword, Mfg_param **r_mpar_ptr)
{
 Mfg_param *mpar_ptr;
 Mfg_param *real_mfg_param_arr;
 int        index;

 mpar_ptr = NULL;
 if(keyword == NULL)
  return(K_NOT_USED);

 /* Get the param array */
 get_mfg_param_array(&real_mfg_param_arr);

 /* Set the array to the cl_command array temporarily */
 set_mfg_param_array(cl_command_tm_param_array);

 index = get_mpar_from_keyword_n_type (cl_command_tm_param_array, keyword,
                                       K_NOT_USED, &mpar_ptr);

 if ( (index != K_NOT_USED))
    index = mpar_ptr->param_type;

 if(mpar_ptr != NULL)
    *r_mpar_ptr = mpar_ptr;

 /* reset back to the real array */
 set_mfg_param_array(real_mfg_param_arr);

 return(index);

}

/*
 * Update the ProElement corresponding to the passed cl command parameter
 * and add it back to the input table
 */
int update_cl_command_param(wchar_t *param_wstr, wchar_t *val_wstr,
                            ProElement **input_table)
{
 int ierror = E_NO_ERROR;
 Mfg_param *prm_ptr;
 int data_type = K_NOT_USED;
 int param_type = K_NOT_USED;
 int pro_elem_id;
 double dbl = 0.0;
 int str_count = 1;
 wchar_t **lines = NULL;
 double  use[3];
 double  axis[3];
 char  *s;
 ProElement old_elem = NULL;

 /* stuff from element in input table */
 wchar_t *old_line = NULL;
 double  old_use[3];

 void *arg_arr[4];

 arg_arr[0] = NULL;
 arg_arr[1] = &str_count;
 arg_arr[2] = &use[0];
 arg_arr[3] = &axis[0];

 (void) is_cl_command_param(param_wstr, &prm_ptr);

 if(prm_ptr != NULL)
 {
   data_type = prm_ptr->data_type;
   param_type = prm_ptr->param_type;
   pro_elem_id = get_elemid_from_clcomm_param_type(param_type);

   /* If the element already exists read the data, first */
   if(find_input_table_elem(*input_table, pro_elem_id, &old_elem) != K_NOT_USED)
   {
     ierror = ProElemNclCommdValueGet (old_elem, &old_line);
     if(ierror == E_NO_ERROR && old_line != NULL)
     {
       arg_arr[0] = (void *) &old_line;
       str_count = XAR_COUNT(&old_line);
     }

     ierror = ProElemNclCommdLocGet (old_elem, old_use);
     if (ierror == E_NO_ERROR)
     {
       use[0] = old_use[0];
     }
   }


   switch(data_type)
   {
     case MPAR_DOUBLE:
        pro_wsscanf(val_wstr, "%lf", &dbl);
     break;

     case MPAR_STRVAL:
        s =  (char *) getmem(wstrlen(val_wstr)*sizeof(char) + 1);
        wstrtos(s, val_wstr);
        if(!strcmp(s, TPL_MF_YES))
          use[0] = 1.0;
        else
          use[0] = 0.0;
        relmem(&s);
   /* Add the element to the input table */
   add_to_input_table (input_table, pro_elem_id, arg_arr, 1);
     break;

     case MPAR_STRING:
        /* convert the value to lines */
        str_count = convert_text_to_lines(val_wstr, &lines);
        arg_arr[0] = (void *) lines;
   /* Add the element to the input table */
   add_to_input_table (input_table, pro_elem_id, arg_arr, 1);
     break;

     case MPAR_INT:
     break;

   }


 }

 return(ierror);
}

static int get_elemid_from_clcomm_param_type(int param_type)
{
 switch(param_type)
 {

  case TM_USE_BEFORE_LOADTL:
  case TM_TEXT_BEFORE_LOADTL:
  case TM_SIZE_BEFORE_LOADTL:
    return PRO_E_AUTO_NCL_COMMAND_BEFORE_LOADTL;

  case TM_USE_AFTER_LOADTL:
  case TM_TEXT_AFTER_LOADTL:
  case TM_SIZE_AFTER_LOADTL:
    return PRO_E_AUTO_NCL_COMMAND_AFTER_LOADTL;

  case TM_USE_AFTER_FROM:
  case TM_TEXT_AFTER_FROM:
  case TM_SIZE_AFTER_FROM:
    return PRO_E_AUTO_NCL_COMMAND_AFTER_FROM;

  case TM_USE_AFTER_SPINDL:
  case TM_TEXT_AFTER_SPINDL:
  case TM_SIZE_AFTER_SPINDL:
    return PRO_E_AUTO_NCL_COMMAND_AFTER_SPINDLE;

  case TM_USE_BEFORE_EACH_PASS:
  case TM_TEXT_BEFORE_EACH_PASS:
  case TM_SIZE_BEFORE_EACH_PASS:
    return PRO_E_AUTO_NCL_COMMAND_BEFORE_EACH_PASS;

  case TM_USE_AFTER_EACH_PASS:
  case TM_TEXT_AFTER_EACH_PASS:
  case TM_SIZE_AFTER_EACH_PASS:
    return PRO_E_AUTO_NCL_COMMAND_AFTER_EACH_PASS;

  case TM_USE_AFTER_FINAL_RETRACT:
  case TM_TEXT_AFTER_FINAL_RETRACT:
  case TM_SIZE_AFTER_FINAL_RETRACT:
    return PRO_E_AUTO_NCL_COMMAND_LAST_RETRACT;

  case TM_USE_AFTER_GOHOME:
  case TM_TEXT_AFTER_GOHOME:
  case TM_SIZE_AFTER_GOHOME:
    return PRO_E_AUTO_NCL_COMMAND_AFTER_GOHOME;


  default:
    return K_NOT_USED;
 }
}

static int convert_text_to_lines(wchar_t *area_text, wchar_t ***r_lines)
{
   wchar_t **lines = NULL;
   wchar_t   new_wchar, *line, *line_copy;
   int       num_lines;

   if (area_text != NULL)
   {
      lines = (wchar_t **) xar_alloc (0, sizeof(wchar_t *), 3);
      line = (wchar_t *) xar_alloc(0, sizeof (wchar_t), K_PATH_SIZE);
      while (*area_text != NULL_WCHAR)
      {
         if (*area_text == '%' && *(area_text + 1) == '%' )
         {
            new_wchar = NULL_WCHAR;
            xar_append ((char **)&line, 1, (char *)&new_wchar);
            line_copy = make_wscopy(line);
            xar_append ((char **)&lines, 1, (char *)&line_copy);
            xar_resize((char **)&line, 0);
            area_text++;
         }
         else
         {
            new_wchar = *area_text;
            xar_append ((char **)&line, 1, (char *)&new_wchar);
         }
         area_text++;
      }

      /* finish last line */
      new_wchar = NULL_WCHAR;
      xar_append((char **)&line, 1, (char *)&new_wchar);
      line_copy = make_wscopy(line);
      xar_append((char **)&lines, 1, (char *)&line_copy);

      xar_free((char **)&line);
   }

   num_lines = XAR_COUNT((char **)&lines);
   INIT_ARG (r_lines, lines);

   return num_lines;
}


/* TEMPLATE SPECIFIC FILE UTILITIES */

/* Obtains the template file in a Pfa structure */
/* Returns E_NO_ERROR or E_ERROR */
static int get_template_file (Pfa *pfa_file)
{
   wchar_t filter_str[K_LINESIZE], fname_wstr[K_PATH_SIZE];
   char fname_str[K_PATH_SIZE];
   Pro_split_path **selection = NULL;
   int num_sel, ierror = E_NO_ERROR, ok;
   Pfa_Error pfa_err;
   wchar_t *tpldir, tpldir_wstr[K_PATH_SIZE] = {NULL_WCHAR};

   /* create the filter string */
   pro_wsprintf(filter_str, "*%c%s", EXTENSION_DELIMITER, TPL_FILE_EXT);

   /* create the start default directory */
   tpldir = get_mfg_template_dir();
   if (tpldir != NULL)
      wstrcpy(tpldir_wstr, tpldir);

   msg_put("Select a template file");
   /* obtain selection */
   num_sel = promenu_select_files (tpldir_wstr, DIRECTORY_ONLY_FOLDER,
                                   NULL, 0,
                                   NULL, 1, &selection,
                                   filter_str, 0, NULL,
                                   TRUE, 1, FALSE);
   if (num_sel < 0)
   {
      ierror = PTC_E_ABORT;
      dbg_err_crash ("get_template_file", "Invalid file selection");
      return (ierror);
   }
   else if (num_sel == 0)
   {
      ierror = E_NO_SELECTION;
      dbg_print_info ("get_template_file", "No files selected");
      return (ierror);
   }
   /* convert selection to fname */
   fname_wstr[0] = NULL_WCHAR;
   fname_str[0]  = NULL_CHAR;
   ok = generate_file_spec (selection[0], NULL, FALSE, fname_wstr);
   pro_splitpath_xar_free(&selection);
   if (!ok)
   {
      ierror = E_ERROR;
      dbg_err_syserr ("get_template_file", "Error parsing file selection");
      return (ierror);
   }
   wstrtos (fname_str, fname_wstr);

   /* convert file_name to pfa structure */
   pfa_err = pfa_parse_to_pro_file (pfa_file, fname_str);
   if (pfa_err != PFA_E_NO_ERROR)
      ierror = E_ERROR;

   return (ierror);

}


/* Obtain the type of the template (same as mill feature type FACE, NCFMSTEP,
   POCKET, PROFILE, etc. */
/* Relies on file_type static, and to obtain type outside of this file
   use ncfm_get_template_type() */
static int get_template_type (Pfa *pfa_file, int *p_tpl_type,
                              wchar_t **p_msg_wstring)
{
   int     ierror = E_NO_ERROR;
   wchar_t tpl_delim_wstr[K_NAME_SIZE], str_delim_wstr[K_NAME_SIZE];
   wchar_t tpl_type_wstr[K_NAME_SIZE], buff_wstr[K_PATH_SIZE];
   wchar_t param_wstr[K_PATH_SIZE], val_wstr[K_PATH_SIZE];
   Bool    found_template_start, found_strategy_start, found_template_type;
   char    fname_str[K_LINESIZE];

   /* Error check */
   if (pfa_file == NULL)
   {
      dbg_err_crash ("get_template_type", "invalid input");
      ierror = PTC_E_ABORT;
      return (ierror);
   }

   /* Open the file */
   if (pfa_fopen(pfa_file, THIS_VERSION, "r") != PFA_E_NO_ERROR)
   {
      msgID_put(MSG_TPL_FILE_ERR); /* "Error reading template file." */
      dbg_err_syserr("get_template_type",
                     "error reading template type in file");
      ierror = E_ERROR;
      return (ierror);
   }
   fname_str[0] = NULL_CHAR;
   pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);

   reset_line_number(); /* reset the line number count */

   /*
    * Obtain the start of the template block
    * i.e. the first occurence of TPL_TEMPLATE_DELIM ([template])
    */
   strtows(tpl_delim_wstr, TPL_TEMPLATE_DELIM);
   found_template_start = FALSE;

   if(get_ncfmtemplate_file_type() == TPL_TEMPLATE_FILE)
   {
     while (!found_template_start && read_next_tpl_line(pfa_file, K_PATH_SIZE,
                                                  buff_wstr) == PFA_E_NO_ERROR)
        found_template_start = !wu_strcmp(buff_wstr, tpl_delim_wstr);

     if (!found_template_start)
     {
        msgID_put(MSG_TPL_TEMPBLK_ERR, fname_str);
        /* "[template] block not found in template file %0w." */
        dbg_err_syserr ("get_template_type", "%s not found",
                        TPL_TEMPLATE_DELIM);
        ierror = E_ERROR;
        pfa_close (pfa_file);
        return (ierror);
     }

     /* Obtain the template type */
     strtows(tpl_type_wstr,  TPL_TEMPLATE_TYPE); /* Template type "type" */
     strtows(str_delim_wstr, TPL_STRATEGY_DELIM); /* "[strategy]" */

     found_template_type = found_strategy_start = FALSE;

     /* Find "type" in template block */
     while (!found_strategy_start && !found_template_type &&
            read_next_tpl_line(pfa_file, K_PATH_SIZE, buff_wstr)
            == PFA_E_NO_ERROR)
     {
        split_template_wstring(buff_wstr, NULL, param_wstr, val_wstr);
        if (!wu_strcmp(param_wstr, tpl_type_wstr))
           found_template_type =  TRUE;
        else if (!wu_strcmp(param_wstr, str_delim_wstr))
           found_strategy_start = TRUE;
     }

     if (!found_template_type)
     {
        msgID_put(MSG_TPL_TYPE_ERR, fname_str);
        /* "Error in template file. Template type not found." */
        dbg_err_syserr("get_template_type", "template type not found");
        ierror = E_ERROR;
     }
     else
        INIT_ARG(p_tpl_type,
                 get_template_type_from_wstring (val_wstr, p_msg_wstring));
   }
   else if(get_ncfmtemplate_file_type() == XML_TEMPLATE_FILE)
   {
      found_template_type = get_template_type_from_xml(pfa_file, p_tpl_type,
                                                       p_msg_wstring);
      if (!found_template_type)
      {
         msgID_put(MSG_TPL_TYPE_ERR, fname_str);
         /* "Error in template file. Template type not found." */
         dbg_err_syserr("get_template_type", "template type not found");
         ierror = E_ERROR;
      }
   }

   pfa_close(pfa_file);
   return (ierror);
}

/*
  Function : ncfm_get_template_type
  Purpose  : Obtains the template type for the file. See also
             get_template_type().
  Input    : tpl_fname - Full path to template file.
  Output   : Message wstring identifying template type.
  Return   : Template type e.g. POCKET, NCFMSTEP, etc. or K_NOT_USED if error
  Comments :
  Function a necessity 'cos get_template_type unfortunately relies on statics.
*/
extern int ncfm_get_template_type(wchar_t *tpl_fname, wchar_t **p_type_wstr)
{
   Pfa *pfa_file = NULL;
   int  ierror;
   int  tpl_file_type = K_NOT_USED, r_tpl_feat_type = K_NOT_USED;
   char tpl_fname_str[K_PATH_SIZE];

   /* Error checks, create pfa on file */
   if (tpl_fname == NULL)
   {
      dbg_err_syserr("ncfm_get_template_type", "invalid input");
      return (K_NOT_USED);
   }
   wstrtos(tpl_fname_str, tpl_fname);
   if (pfa_alloc_pro_file (&pfa_file) != PFA_E_NO_ERROR)
   {
      dbg_err_syserr("ncfm_get_template_type", "pfa error");
      return (K_NOT_USED);
   }
   if (pfa_parse_to_pro_file (pfa_file, tpl_fname_str) != PFA_E_NO_ERROR)
   {
      dbg_err_syserr("ncfm_get_template_type", "pfa file error");
      pfa_free_pro_file(&pfa_file);
      return (K_NOT_USED);
   }

   /* Set up static */
   ierror = get_template_file_type(pfa_file, &tpl_file_type); /* XML? */
   if (ierror != E_NO_ERROR)
      return (K_NOT_USED);
   set_ncfmtemplate_file_type(tpl_file_type);

   /* Finds type */
   get_template_type (pfa_file, &r_tpl_feat_type, p_type_wstr);

   return (r_tpl_feat_type);
}

/*
 * Given the type string from tpl file returns the corresponding int feature
 * type. If not found returns K_NOT_USED. The message string returns the
 * corresponding translated string for messages e.g. Pocket, Slot etc. Message
 * string is a static variable and needs to be recopied
 */
static int get_template_type_from_wstring (wchar_t *type_wstring,
                                           wchar_t **r_msg_wstring)
{
   int ret_val, ii;
   wchar_t temp_wstr[K_LINESIZE];
   static wchar_t msg_wstring[K_LINESIZE];

   ret_val = K_NOT_USED;
   msg_wstring[0] = NULL_WCHAR;
   for (ii=0; ii <table_num && ret_val == K_NOT_USED; ii++)
   {
      strtows(temp_wstr, templ_type[ii].string);
      if (!wu_strcmp(temp_wstr, type_wstring))
      {
         ret_val = templ_type[ii].value;
         msgID_sput_buffer(msg_wstring, K_LINESIZE, templ_type[ii].msgid);
                  /* POCKET, STEP, FACE, etc. */
      }
   }

   INIT_ARG(r_msg_wstring, msg_wstring);

   return (ret_val);
}


/* GENERAL FILE UTILITIES */

/*
 * Reads the next uncommented file line, ignores leading white spaces,
 * and converts to wstring
 */
static int read_next_tpl_line(Pfa *pfa_file, int max_out_size,
                              wchar_t *out_wstr)
{
   Pfa_Error pfa_err;
   char *buff_str, *p_str;
   int buff_len;

   if (out_wstr == NULL || max_out_size <=0)
   {
      pfa_err = PFA_E_INVALID_INPUT;
      dbg_err_syserr("read_next_tpl_line", "invalid input");
      return (pfa_err);
   }

   p_str = buff_str = (char *) getmem(max_out_size * sizeof(char));

   /* Find the first non commented line */
   do
   {
      pfa_err = pfa_fgets(pfa_file, max_out_size, NULL, buff_str);
      increase_line_number(); /* increment line number count */
      if ((buff_len = strlen(buff_str)) > 1)
         buff_str[buff_len-1] = NULL_CHAR; /* Replace \n with \0 */

      /* Forward to the first non space character */
      while (btk_iswspace(*p_str++));
      p_str--;

   }while (*p_str == TPL_COMMENT_START);

   /* Copy to output wstring */
   strtows(out_wstr, p_str);

   relmem(&buff_str);
   return (pfa_err);
}

static void reset_line_number(void)
{
   line_number = 0;
}

static void increase_line_number(void)
{
   line_number++;
}
static int get_line_number(void)
{
   return (line_number);
}

/* GENERAL STRING UTILITIES */

/*
 * Completely decomposes input_wstring of the form " I . P = V ",
 * where I,P & V are wstrings (can be NULL), into the respective constituents
 * based on the following rules
 * 1. I, P do not contain any '=' (TPL_OPERAND_SEP) character.
 * 2. P does not contain any '.' (TPL_KEY_SEP) character.
 * 3. I, P & V do not contain leading or trailing spaces.
 * 4. If no '.' character I is empty, and if no '=' V is empty.
 * Assumes memory allocated for all output.
 * Returns E_ERROR if error with input and E_NO_ERROR otherwise.
 */
static int split_template_wstring (wchar_t *input_wstr, wchar_t *i_wstr,
                                   wchar_t *p_wstr, wchar_t *v_wstr)
{
   wchar_t *istart, *vstart, *pstart, *wptr;
   int vlen, plen, ilen, input_len;
   int ierror = E_NO_ERROR;

   if (input_wstr == NULL)
   {
      ierror = E_ERROR;
      dbg_err_syserr("split_template_wstring", "Empty input_wstr");
      return (ierror);
   }
   input_len = wstrlen (input_wstr);
   istart = NULL;
   pstart = wptr = input_wstr; /* first char */
   vstart = input_wstr + input_len+1; /* next after last char */

   /* The char after last TPL_KEY_SEP & before TPL_OPERAND_SEP is pstart */
   while (*wptr != NULL_WCHAR && *wptr != TPL_OPERAND_SEP)
   {
      if (*wptr == TPL_KEY_SEP) pstart = wptr + 1;
      wptr++;
   }

   /* The char after first TPL_OPERAND_SEP is vstart
      (default at the end of string) */
   if (*wptr == TPL_OPERAND_SEP)
      vstart = wptr + 1;

   /* If TPL_KEY_SEP has been found, istart is same as input_wstr start */
   if (pstart > input_wstr)
      istart = input_wstr;

   /* Copy portions of input_wstr to the output strings */
   if (i_wstr != NULL)
      if (istart != NULL && (ilen = (int) (pstart-istart-1)) > 0)
      {
         wstrncpy(i_wstr, istart, ilen);
         i_wstr[ilen] = NULL_WCHAR;
         prune_space_wstring(i_wstr);
      }
      else
         i_wstr[0] = NULL_WCHAR;
   if (p_wstr != NULL)
      if ((plen = (int) (vstart - pstart - 1)) > 0)
      {
         wstrncpy(p_wstr, pstart, plen);
         p_wstr[plen] = NULL_WCHAR;
         prune_space_wstring(p_wstr);
      }
      else
         p_wstr[0] = NULL_WCHAR;
   if (v_wstr != NULL)
      if ((vlen = (int) (input_wstr-vstart+input_len)) > 0)
      {
         wstrncpy(v_wstr, vstart, vlen);
         v_wstr[vlen] = NULL_WCHAR;
         prune_space_wstring(v_wstr);
      }
      else
         v_wstr[0] = NULL_WCHAR;

   return (ierror);
}

/* Prune the leading and trailing spaces in a wstring */
static wchar_t *prune_space_wstring(wchar_t *v_wstr)
{
   wchar_t *sptr = v_wstr; /* Start */
   wchar_t *eptr = v_wstr + wstrlen(v_wstr) - 1; /* End */

   while (btk_iswspace(*sptr++)); /* Locate start */
   sptr--;
   while (btk_iswspace(*eptr--)); /* Locate end */
   eptr++;

   *(eptr+1) = NULL_WCHAR; /* Modify end */
   if (sptr != v_wstr)
      wstrcpy(v_wstr, sptr); /* Modify beginning if necessary */

   return (v_wstr);
}

/* Prune the leading and trailing spaces in a string */
static char *prune_space_string(char *v_str)
{
   char *sptr = v_str; /* Start */
   char *eptr = v_str + strlen(v_str) - 1; /* End */

   while (btk_iswspace(*sptr++)); /* Locate start */
   sptr--;
   while (btk_iswspace(*eptr--)); /* Locate end */
   eptr++;

   *(eptr+1) = NULL_CHAR; /* Modify end */
   if (sptr != v_str)
      strcpy(v_str, sptr); /* Modify beginning if necessary */

   return (v_str);
}

/* Get the command to run the template manager/creator :
 * i.e. /path-to-template-exe/tpl-exe default-path-to-tpl-files
 * K_PATH_SIZE allocated to template_com
 */
static char *template_com (void)
{
   static char com_str[K_PATH_SIZE];
   char tpldir_str[K_PATH_SIZE];
   wchar_t *prodir, *tpldir, prodir_wstr[K_PATH_SIZE];
   char *prodir_str;

   /* Get the load point */
   if ((prodir_str = BTK_GETENV_31_S("NCFM_TEMPLATE_HOME"))== NULL) /* Only for debug */
      prodir = system_dir(); /* If no template dir specified get system dir */
   else
   {
      strtows(prodir_wstr, prodir_str);
      prodir = prodir_wstr;
   }

   if (prodir == NULL)
      return (NULL);

   /* Create the exe command */

#if ( (PRO_MACHINE_TYPE == I486_NT) || (PRO_MACHINE_TYPE == IA64_NT) || (PRO_MACHINE_TYPE == X86E_WIN64))
   pro_sprintf(com_str, "%s%ws%c%s%c%s%c%s%c%s%c%s%s ", "\'", prodir,
               DIR_DELIMITER, "apps", DIR_DELIMITER, "mfgapps",
               DIR_DELIMITER, "java", DIR_DELIMITER, "bin",
               DIR_DELIMITER, "template_run.bat", "\'" );
 
#else
   pro_sprintf(com_str, "%ws%c%s%c%s%c%s%c%s%c%s ", prodir, DIR_DELIMITER,
               "apps", DIR_DELIMITER, "mfgapps", DIR_DELIMITER, "java",
               DIR_DELIMITER, "bin", DIR_DELIMITER, "template_run");
               /* Crazy way of doing this I guess, but who cares? */
#endif

   /* Add as argument, path to where tpl files are stored */
   tpldir = get_mfg_template_dir();
   if (tpldir != NULL)
   {
      wstrtos(tpldir_str, tpldir);
      strcat (com_str, tpldir_str);
   }

   return(com_str);
}

/*
 * Get the config.pro option specifying the default dir to store/retrieve
 * tpl files - see also ncfm_get_mfg_template_directory()
 */
extern wchar_t *get_mfg_template_dir (void)
{
   if (mfg_template_dir[0] == NULL_WCHAR)
      return NULL;

   return(mfg_template_dir);
}

/*
 * Obtains the directory where template files are located - this is
 * either mfg_template_dir (if config specified), or the current directory.
 * See get_mfg_template_dir() also.
 * Assumes K_PATH_SIZE allocated to tpldir.
 * Return : E_NO_ERROR, PTC_E_ABORT
 */
extern int ncfm_get_mfg_template_directory(tpldir_wstr)
wchar_t tpldir_wstr[K_PATH_SIZE];
{
   wchar_t *tpldir;

   if ((tpldir = get_mfg_template_dir()) != NULL)
      wstrcpy(tpldir_wstr, tpldir); /* config mfg_template_dir directory */
   else
      dir_get(tpldir_wstr, K_PATH_SIZE); /* current directory */
   if (tpldir_wstr[0] == NULL_WCHAR)
   {
      dbg_err_crash("def_dlg_init_tpl_files",
                    "Unable to find template directory");
      return (PTC_E_ABORT);
   }
   return (E_NO_ERROR);
}

/*
 * Set static from the config.pro option specifying the default dir to
 * store/retrieve tpl files
 */
extern int set_mfg_template_dir (wchar_t *new_value)
{
   wstrcpy(mfg_template_dir, new_value);

   return(E_NO_ERROR);
}

/*
  Gets a translated string representing the mill feature type of the given
  mill feature. Returns a static wstring.
  Note that the function uses the templ_type table in ncfmtemplate.h
  and will work only for those types for which templates have
  been implemented
*/
extern wchar_t *ncfm_get_mill_feat_type_wstr(Solid *part_ptr, Feat *feat_ptr)
{
   int i, mill_feat_type;
   static wchar_t msg_wstr[K_NAME_SIZE] = {NULL_WCHAR};

   mill_feat_type = ncfm_get_feat_type (part_ptr, feat_ptr);

   if (mill_feat_type == K_NOT_USED)
      return (msg_wstr);

   for (i=0; i<table_num; i++)
   {
      if (templ_type[i].value == mill_feat_type)
         msgID_sput(msg_wstr, templ_type[i].msgid);
   }

   return (msg_wstr);
}

/*
 * If lhs = TRUE,
 * converts LHS e.g. "PRO_E_HOLEMAKING_TYPE" to PRO_E_HOLEMAKING_TYPE
 * if htype = FALSE,
 * converts RHS e.g. "PRO_HOLE_MK_CYCLE_STD" to PRO_HOLE_MK_CYCLE_STD
 * returns E_NO_ERROR/E_NOT_FOUND
 */
int get_hole_type_from_wstr (wchar_t *wstr, Bool lhs, int *p_rval)
{
   char str[K_NAME_SIZE];
   HoleType *search_arr;
   int i, num_arr;

   if (wstr == NULL || wstr[0] == NULL_WCHAR)
      return (E_NOT_FOUND);

   /* Search array is indexed based on string, not wstring */
   wstrtos(str, wstr);

   /* Convert LHS/RHS? */
   if (lhs)
   {
      search_arr = hole_type;
      num_arr = hole_type_num;
   }
   else
   {
      search_arr = hole_value;
      num_arr = hole_value_num;
   }

   /* Search for the element */
   for (i = 0 ; i < num_arr ; i++)
      if (!strcmp(str, search_arr[i].str))
      {
         INIT_ARG(p_rval, search_arr[i].val);
         return (E_NO_ERROR);
      }

   return (E_NOT_FOUND);
}

/* For xar_sort of input table : Ensures beginning elements and their order */
PRO_STATIC int hole_template_cmp_function (ProElement *elem1, ProElement *elem2)
{
   static ProElemId sort_order[] =
   {PRO_E_OPERATION, PRO_E_HOLEMAKING_TYPE, PRO_E_HOLE_CYCLE_TYPE,
    PRO_E_FEAT_NAME, PRO_E_COMMENTS, PRO_E_TOOL, PRO_E_MFG_PARAMS,
    PRO_E_HOLESETS, PRO_E_RETRACT, PRO_E_CSYS, PRO_E_FEATURE_TYPE,
    PRO_E_NCSEQ_TYPE};

   static int num = sizeof(sort_order) / sizeof(int);
   ProElemId id1 = K_NOT_USED, id2 = K_NOT_USED;
   int i;

   if (elem2 == NULL || ProElementIdGet(*elem2, &id2) != PRO_TK_NO_ERROR)
      /* elem2 not there, keep order */
      return (-1);
   else if (elem1 == NULL || ProElementIdGet(*elem1, &id1) != PRO_TK_NO_ERROR)
      /* elem1 not there, swap */
      return (1);
   else
      for (i = 0; i < num; i++)
         if (id1 == sort_order[i])
            /* keep order : elem1, elem2 */
            return (-1);
         else if (id2 == sort_order[i])
            /* invert order : elem2, elem1 */
            return (1);

   /* order doesn't matter, keep it */
   return (-1);
}


PRO_STATIC  int find_tdia_and_tlen_from_tool (Tool *tool_ptr, double *p_tdia,
                                          double *p_tlen, double *p_twidth)
{
   double tdia = 0.0, tlen = 0.0, twidth = 0.0;
   int ierror = E_NO_ERROR;
   Param ***p_param_arr = NULL;

   if (tool_ptr != NULL)
   {
      /* Find tdia and tlen from tool */
      if ( get_mfg_tool_params( tool_ptr, &p_param_arr ) )
      {
         get_tool_param_value ( p_param_arr, MT_CUTTER_DIA, &tdia );
         get_tool_param_value ( p_param_arr, MT_LENGTH, &tlen);
         get_tool_param_value ( p_param_arr, MT_CUTTER_WIDTH, &twidth);
      }
      if (tdia < ALMOST_ZERO)
         tdia = 0.0;
      if (tlen < ALMOST_ZERO)
         tlen = 0.0;
      if (twidth < ALMOST_ZERO)
         twidth = tlen;
   }
   else
      ierror = E_ERROR;

   INIT_ARG(p_tdia, tdia);
   INIT_ARG(p_tlen, tlen);
   INIT_ARG(p_twidth, twidth);

   return (ierror);
}

/* XML TEMPLATE FILE UTILITIES */

static int get_template_file_type (Pfa *pfa_file, int *file_type)
{
   Pfa_Error pfa_err;
   int     ierror;
   char    fname_str[K_LINESIZE];
   char    *buff_str, *p_str;
   int buff_len;
   Bool    found_file_type = FALSE;
   Bool    found_tpl_type, found_xml_type;
   char    open_xml_ver_id[K_NAME_SIZE];
   wchar_t out_wstr[K_PATH_SIZE];
   int     b_comm, e_comm;

   /* Error check */
   if (pfa_file == NULL)
   {
      dbg_err_crash ("get_template_type", "invalid input");
      ierror = PTC_E_ABORT;
      return (ierror);
   }

   /* Open the file */
   if (pfa_fopen(pfa_file, THIS_VERSION, "r") != PFA_E_NO_ERROR)
   {
      msgID_put(MSG_TPL_FILE_ERR); /* "Error reading template file." */
      dbg_err_syserr("get_template_type",
                     "error reading template type in file");
      ierror = E_ERROR;
      return (ierror);
   }
   fname_str[0] = NULL_CHAR;
   pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);

   reset_line_number(); /* reset the line number count */

   /* Read the first line of the file. */
   p_str = buff_str = (char *) getmem(K_PATH_SIZE* sizeof(char));

   /* Get the xml comment begin and end */
   strcpy (open_xml_ver_id, XML_HEADER_VERSION_ID);

   /* initialize flags */
   found_tpl_type = FALSE;
   b_comm = e_comm = 0;

   /* do while it is an xml comment or the xml type is found, if it is neither
      set TPL type to true */
   do
   {
      pfa_err = pfa_fgets(pfa_file, K_PATH_SIZE, NULL, buff_str);
      increase_line_number(); /* increment line number count */
      if ((buff_len = strlen(buff_str)) > 1)
         buff_str[buff_len-1] = NULL_CHAR; /* Replace \n with \0 */

      /* see if the line has an xml version header or does the line
         begin with a comment */
      found_xml_type =  has_xml_head_tag_in_string(p_str, &b_comm,
                                                   &e_comm, open_xml_ver_id);

      if(found_xml_type)
         found_file_type = TRUE;
      else if(b_comm == 0)
      {
         found_file_type = TRUE;
         found_tpl_type = TRUE;
      }
   } while (!found_file_type && pfa_err == PFA_E_NO_ERROR);

   if(found_tpl_type)
   {  INIT_ARG(file_type, TPL_TEMPLATE_FILE); }
   else if(found_xml_type)
   {   INIT_ARG(file_type, XML_TEMPLATE_FILE); }

   /* Copy to output wstring */
   strtows(out_wstr, p_str);

   /* free the buffer */
   relmem(&buff_str);

   /* Close the file */
   pfa_close(pfa_file);

   return(E_NO_ERROR);
}

/*
 * Get/Set the template file type here, i.e. if the tpl file is using the
 * TPL or XML format
 */
static void set_ncfmtemplate_file_type(int type)
{
   file_type = type;
}

static int get_ncfmtemplate_file_type()
{
   return(file_type);
}


/*
 * Returns true, if it finds a valid template type in the file, else false.
 * the template type and an appropriate message are also returned
 */
static int get_template_type_from_xml(Pfa *pfa_file, int *tpl_type,
                                      wchar_t **r_msg_wstring)

{
   Pfa_Error pfa_err = PFA_E_NO_ERROR;
   int     ierror;
   char    fname_str[K_LINESIZE];
   char    *buff_str, *p_str;
   int buff_len;
   wchar_t val_wstr[K_PATH_SIZE];

   Bool    found_template_type;
   char    open_xml_comment[K_NAME_SIZE], close_xml_comment[K_NAME_SIZE];
   char    open_head[8];
   char    close_footer[8];
   char    template_type_header[K_NAME_SIZE];
   char    template_type_footer[K_NAME_SIZE];
   char    template_type_tag[K_NAME_SIZE];
   char    template_type_value[K_NAME_SIZE];
   char    template_tag[K_NAME_SIZE];
   char    tag_value[K_PATH_SIZE], parse_tag_value[K_PATH_SIZE];
   char    null_string[4];
   int     BEGIN_TAG, END_TAG, BEGIN_COMM, END_COMM;

   /* Error check */
   if (pfa_file == NULL)
   {
      dbg_err_crash ("get_template_type_from_xml", "invalid input");
      ierror = PTC_E_ABORT;
      return (ierror);
   }

   /* An open file is being passed to this function, so do not open */
   fname_str[0] = NULL_CHAR;
   pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);

   p_str = buff_str = (char *) getmem(K_PATH_SIZE* sizeof(char));

   strcpy(template_type_tag, TM_XML_TEMPLATE_TYPE);
   strcpy(open_head, XML_HEADER_TAG_BEGIN);
   strcpy(template_tag, TM_XML_TEMPLATE);
   strcpy(close_footer, XML_FOOTER_TAG_END);

   /* REM: This should work but is not a thorough check, especially
      for escape characters */
   pro_sprintf(template_type_header, "%s%s ", open_head,
                           template_tag);
   pro_sprintf(template_type_footer, "%s", close_footer);


   /* read in the comment headers */
   strcpy (open_xml_comment, XML_COMMENT_HEADER);
   strcpy (close_xml_comment, XML_COMMENT_FOOTER);

   BEGIN_TAG = END_TAG = 0;
   BEGIN_COMM = END_COMM = 0;
   found_template_type = FALSE;
   null_string[0] = NULL_CHAR;
   strcpy(tag_value, null_string);

   while( (found_template_type == FALSE) &&
          (pfa_err = pfa_fgets(pfa_file, K_PATH_SIZE, NULL, buff_str)
                     == PFA_E_NO_ERROR))
   {
      if ((buff_len = strlen(buff_str)) >= 1)
         buff_str[buff_len-1] = NULL_CHAR; /* Replace \n with \0 */

     parse_tag_value[0] = NULL_CHAR;
     ierror = get_xml_tag_value_from_string (p_str,
                               template_type_header, template_type_footer,
                               &BEGIN_TAG, &END_TAG,
                               &BEGIN_COMM, &END_COMM,
                               parse_tag_value );

     if(ierror == E_NO_ERROR && BEGIN_TAG == 1 && END_TAG == 1)
       found_template_type = TRUE;

     if(strncmp(parse_tag_value, null_string, 1) != 0)
     {
       if(BEGIN_TAG != 0)
         strcat(tag_value, parse_tag_value);
     }
    }

/* REM: ADD MESSAGE */
   if(found_template_type == FALSE)
   {
      if(pfa_err != PFA_E_NO_ERROR)
      {
#if TPL_DEBUG
         dbg_print_info("get_template_type_from_xml",
                        "Error reading file or reached the end.");
#endif
      }
   }
   else
   {

      /* Get the template type attribute's value from the tag*/
      ierror = get_xml_attribute_value(tag_value, template_type_tag,
                                       template_type_value);

      if(ierror == E_NO_ERROR)
      {
         /* May be it needs to be pruned */
         prune_space_string(template_type_value);
         strtows(val_wstr, template_type_value);
         INIT_ARG(tpl_type,
                  get_template_type_from_wstring (val_wstr, r_msg_wstring));
      }
      else
         found_template_type = FALSE;
   }

   relmem(&buff_str);

   /* Close the file */
   pfa_close(pfa_file);

   return(found_template_type);
}

static int has_xml_head_tag_in_string(char *in_str, int *bcomm,
                                      int *ecomm, char *in_head_tag)
{
   Bool FOUND_HEADER_TAG = FALSE;
   char *uncomm_str, *buff_str;
   int b_comm, e_comm;
   char null_string[4];
   int max_out_size = strlen(in_str) ;
   char head_tag[K_NAME_SIZE];

   null_string[0] = NULL_CHAR;

   if(max_out_size <= 0)
      return(FALSE);

   b_comm = *bcomm;
   e_comm = *ecomm;

   strcpy(head_tag, in_head_tag);
   uncomm_str = buff_str = (char *) getmem(sizeof(char)*(max_out_size +
                                                         K_NAME_SIZE));
   remove_xml_comments_in_line(in_str, &b_comm, &e_comm, uncomm_str);

   /* Not an empty line */
   if(strncmp(uncomm_str, null_string, 1) != 0)
   {
      /* header found */
      if( strstr(uncomm_str, head_tag) != NULL)
      {
         FOUND_HEADER_TAG = TRUE;
      }
   }

   relmem(&buff_str);
   INIT_ARG(bcomm, b_comm);
   INIT_ARG(ecomm, e_comm);

   return(FOUND_HEADER_TAG);
}


/* Returns true, if header tag was found in the file, else false */
extern int has_xml_head_tag_in_file(Pfa *pfa_file,
                                    char *in_head_tag)
{
   Bool FOUND_HEADER_TAG = FALSE;
   int b_comm, e_comm;
   char null_string[4];
   char cur_line[K_PATH_SIZE];
   char head_tag[K_NAME_SIZE];
   char c[K_PATH_SIZE];

   /* Init tag status */
   null_string[0] = NULL_CHAR;
   b_comm = e_comm = 0;

   /* set the output to empty string */
   strcpy(cur_line, null_string);
   strcpy(c, null_string);

   /*  copy the tags to local strings */
   if(in_head_tag != NULL)
      strcpy(head_tag, in_head_tag);

   /* REM: Error handling if head_tag or foot_tag not initialized */

   while (!FOUND_HEADER_TAG && read_next_xml_line(pfa_file, K_PATH_SIZE,
                                 cur_line, &b_comm, &e_comm) == PFA_E_NO_ERROR)
   {
      if(b_comm == 1 && e_comm == 0)
         continue;

      /* Not an empty line */
      if(strncmp(cur_line, null_string, 1) != 0)
      {
         /* header found */
         if(strstr(cur_line, head_tag) != NULL)
         {
            FOUND_HEADER_TAG = TRUE;
            break;
         }
      }
   }

   return(FOUND_HEADER_TAG);
}

extern int get_xml_tag_value_from_string(char *in_str,
                  char *in_head_tag, char *in_foot_tag,
                  int *btag, int *etag,
                  int *bcomm, int *ecomm,
                  char *dest_str)
{
   int len;
   int b_tag, e_tag, b_comm, e_comm;
   int ierror;
   Bool b_found_tag = FALSE;
   char null_string[4];
   char head_tag[K_NAME_SIZE], foot_tag[K_NAME_SIZE];
   char *cur_line, *buff_cur;
   char *a, *b, *c = NULL;
   int max_tag_size = strlen(in_str) + K_NAME_SIZE;
   char xml_amp[8], xml_esc_amp[8], xml_gt[8], xml_esc_gt[8];
   char xml_lt[8], xml_esc_lt[8];

   /* Init tag status */
   null_string[0] = NULL_CHAR;
   b_tag = *btag;
   e_tag = *etag;
   b_comm = *bcomm;
   e_comm = *ecomm;

   /*  copy the tags to local strings */
   if(in_head_tag != NULL)
      strcpy(head_tag, in_head_tag);

   if(in_foot_tag != NULL)
      strcpy(foot_tag, in_foot_tag);

   /* REM: Error handling if head_tag or foot_tag not initialized */

   /* allocate memory */
   if(max_tag_size <= 0)
      return(E_ERROR);

   cur_line = buff_cur = (char *) getmem(max_tag_size*sizeof(char));

   /* set the output to empty string */
   strcpy(dest_str, null_string);

   /* The calling function to handle this case appropriately */
   ierror = remove_xml_comments_in_line(in_str, &b_comm, &e_comm, cur_line);
   if(b_comm == 1 && e_comm == 0)
      pro_printf("Could not remove all the comments\n");

   /* This should not occur */
   if(b_comm == 1 && e_comm == 1)
      pro_printf("Error in remove_xml_comments_in_line\n");

   if(ierror != E_NO_ERROR)
      pro_printf("ERROR READING FILE\n");

   /* Not an empty string */
   if(strncmp(cur_line, null_string, 1) != 0)
   {
      if(b_tag == 1)
         a = cur_line;

      if(b_tag == 0)
      {
         /* header found */
         if((a = strstr(cur_line, head_tag)) != NULL)
         {
            b_tag = 1;

            /* copy everthing that's there after the header tag */
            len = strlen(head_tag);
            a = a + len;
         }
      }

      if(b_tag == 1)
      {
         /* footer found */
         if((b = strstr(a, foot_tag)) != NULL)
         {
            e_tag = 1;
            /* copy everything before the footer */
            if( a < b)
            {
               len = strlen(a) - strlen(b);
               c = (char *) getmem(len * sizeof(char) + K_NAME_SIZE);
               strncpy(c,a,len);
               c[len] = NULL_CHAR;
            }
         }
      }

      if(b_tag == 1 && e_tag == 1)
         b_found_tag = TRUE;
   }

   if(b_found_tag)
   {
      if(c != NULL)
        strcpy(dest_str, c);

      /* Before the value is used, the special xml characters (&<>) should be
         converted back to normal vlaues */
      strcpy(xml_amp, XML_AMP);
      strcpy(xml_esc_amp, XML_ESC_AMP);
      strcpy(xml_gt, XML_GT);
      strcpy(xml_esc_gt, XML_ESC_GT);
      strcpy(xml_lt, XML_LT);
      strcpy(xml_esc_lt, XML_ESC_LT);

      /* Convert the escape chars to reserved chars */
      if((strstr(dest_str, xml_esc_amp)) != NULL)
         find_and_replace(dest_str, xml_esc_amp, xml_amp);

      if((strstr(dest_str, xml_esc_gt)) != NULL)
         find_and_replace(dest_str, xml_esc_gt, xml_gt);

      if((strstr(dest_str, xml_esc_lt)) != NULL)
         find_and_replace(dest_str, xml_esc_lt, xml_lt);

      relmem(&c);
   }

   INIT_ARG(btag, b_tag);
   INIT_ARG(etag, e_tag);
   INIT_ARG(bcomm, b_comm);
   INIT_ARG(ecomm, e_comm);

   relmem(&buff_cur);

   return(ierror);
}

extern int get_xml_tag_value_from_file(Pfa *pfa_file, int max_tag_size,
                                       char *in_head_tag, char *in_foot_tag,
                                       int *btag, int *etag, char *dest_str)
{
   int len;
   int b_tag, e_tag, b_comm, e_comm;
   int ierror = E_NO_ERROR;

   Pfa_Error pfa_err = PFA_E_NO_ERROR;
   char    fname_str[K_LINESIZE];

   Bool b_found_tag = FALSE;
   char null_string[4];
   char cur_line[K_PATH_SIZE];
   char head_tag[K_NAME_SIZE], foot_tag[K_NAME_SIZE];
   char *tag_str, *buff_str;
   char *a, *b, c[K_PATH_SIZE];

   find_and_replace(in_head_tag, in_foot_tag, dest_str);

   /* Init tag status */
   null_string[0] = NULL_CHAR;
   b_comm = e_comm = 0;
   b_tag = *btag;
   e_tag = *etag;

   /* Error check */
   if (pfa_file == NULL)
   {
      dbg_err_crash ("get_xml_tag_value_from_file", "invalid input");
      ierror = PTC_E_ABORT;
      return (ierror);
   }

   /* An open file is being passed to this function, so do not open */
   fname_str[0] = NULL_CHAR;
   pfa_get_file_name(pfa_file, PFA_NAME_EXTENSION, fname_str);


   /* set the output to empty string */
   strcpy(dest_str, null_string);
   strcpy(cur_line, null_string);
   strcpy(c, null_string);

   /*  copy the tags to local strings */
   if(in_head_tag != NULL)
      strcpy(head_tag, in_head_tag);

   if(in_foot_tag != NULL)
      strcpy(foot_tag, in_foot_tag);

   /* REM: Error handling if head_tag or foot_tag not initialized */

   /* allocate memory */
   tag_str = buff_str = (char *) getmem(max_tag_size*sizeof(char) + K_NAME_SIZE);

   while (!b_found_tag &&  read_next_xml_line(pfa_file, K_PATH_SIZE,
                                              cur_line, &b_comm, &e_comm) == PFA_E_NO_ERROR)
   {
      if(b_comm == 1 && e_comm == 0)
         continue;

      if(b_comm == 1 && e_comm == 1)
      {
         pro_printf("Error in read_next_xml_line\n");
         break;
      }

      /* Not an empty string */
      if(strncmp(cur_line, null_string, 1) != 0)
      {
         if(b_tag == 0)
         {
            /* header found */
            if((a = strstr(cur_line, head_tag)) != NULL)
            {
               b_tag = 1;

               /* copy everthing that's there after the header tag */
               len = strlen(head_tag);
               a = a + len;
            }
         }
         else
            a = cur_line;

         if(b_tag == 1)
         {
            /* footer found */
            if((b = strstr(a, foot_tag)) != NULL)
            {
               e_tag = 1;
               /* copy everything before the footer */
               if( a < b)
               {
                  len = strlen(a) - strlen(b);
                  strncpy(c,a,len);
                  c[len] = NULL_CHAR;

                  len = strlen(tag_str) + strlen(c);
                  if(len > max_tag_size)
                  {
                     pro_printf("ERROR: Not enough space to copy the tag\n");
                     ierror = -1;
                     break;
                  }
                  else
                     strcat(tag_str, c);
               }
            }
            else
            {
               len = strlen(tag_str) + strlen(a);
               if(len > max_tag_size)
               {
                  pro_printf("ERROR: Not enough space to copy the tag\n");
                  ierror = -1;
                  break;
               }
               else
                  strcat(tag_str, a);
            }
         }

         if(b_tag == 1 && e_tag == 1)
            b_found_tag = TRUE;
      }
   }

   if(pfa_err != PFA_E_NO_ERROR)
   {
      ierror = pfa_err;
   }

   if(b_found_tag)
   {
      strcpy(dest_str, tag_str);
   }
   else
   {
      if(b_tag == 1 && e_tag != 1)
         pro_printf("Could not find the tag %s in the file\n", foot_tag);
      else if(b_tag != 1)
         pro_printf("Could not find the tag %s in the file\n", head_tag);
      else
         pro_printf("Unknown Error.\n");
   }

   INIT_ARG(btag, b_tag);
   INIT_ARG(etag, e_tag);

   relmem(&buff_str);

   return(ierror);
}

static int read_next_xml_line(Pfa *pfa_file, int max_out_size, char *dest_str,
                              int *bcomm, int *ecomm)
{
   Pfa_Error pfa_err;
   char *p_str, *buff_str;
   int buff_len;
   char uncomm_str[K_PATH_SIZE];
   char null_string[4];
   int b_comm = *bcomm;
   int e_comm = *ecomm;

   null_string[0]='\0';
   if (dest_str == NULL || max_out_size <= 0)
   {
      pfa_err = PFA_E_INVALID_INPUT;
      dbg_err_syserr("read_next_xml_line", "invalid input");
      return (pfa_err);
   }

   /* allocate memory */
   p_str = buff_str = (char *) getmem(max_out_size* sizeof(char) +
                                      K_NAME_SIZE);

   /* init constants */
   strcpy (dest_str, null_string);

   /* read a line */
   pfa_err =  pfa_fgets(pfa_file, max_out_size, NULL, buff_str);
   if( pfa_err == PFA_E_NO_ERROR)
   {
      if ((buff_len = strlen(buff_str)) > 0)
         buff_str[buff_len - 1] = NULL_CHAR;

      remove_xml_comments_in_line(p_str, &b_comm, &e_comm, uncomm_str);

      /* Copy the uncommented string to the output */
      strcpy(dest_str, uncomm_str);

   }

   relmem(&buff_str);

   INIT_ARG(bcomm, b_comm);
   INIT_ARG(ecomm, e_comm);

   return(pfa_err);
}

static int remove_xml_comments_in_line(char *in_str, int *bcomm, int *ecomm,
                                       char *uncomm_str)
{
   char open_comment[K_NAME_SIZE];
   char close_comment[K_NAME_SIZE];
   char null_str[4];
   char *a, *b, *c, *d;
   Bool END_OF_LINE = FALSE;
   int max_tag_size, len;
   int b_comm = *bcomm;
   int e_comm = *ecomm;

   /* init comments */
   strcpy (open_comment, XML_COMMENT_HEADER);
   strcpy (close_comment, XML_COMMENT_FOOTER);

   /* init strings */
   null_str[0] = NULL_CHAR;

   max_tag_size = strlen(in_str);
   if(max_tag_size <= 0)
   {
      strcpy(uncomm_str, null_str);
/*
   #if TPL_DEBUG
   dbg_print_info("remove_xml_comments_in_line", "Zero length string passed");
   #endif
*/
      return(E_NO_ERROR);
   }

   if(b_comm == 1)
      a = in_str;

   if(b_comm == 0)
   {
      if( (a = strstr(in_str, open_comment)) != NULL)
      {
         b_comm = 1;

         /* append the portion before the string */
         if(in_str < a)
         {
            len = strlen(in_str) - strlen(a);
            strncat(uncomm_str, in_str, len);
         }

      }
      else /* NO COMMENTS */
      {
         /* strcpy(tag_str, in_str); */
         strcpy(uncomm_str, in_str);
         goto END;
      }
   }

   /* 'a' is the latest postion, at this point */
   if(b_comm == 1) /* comment already started earlier */
   {
      while(e_comm == 0 && !END_OF_LINE)
      {
         if ( (b = strstr(a, close_comment)) != NULL) /* Found one end of string */
         {
            e_comm = 1;
            len = strlen(a) - strlen(b);
            a = a + len + 3;

            /* see if there is another comment */
            if( (c = strstr(a, open_comment))  != NULL)
            {
               /* copy str between a and c to tag */
               len = strlen(a) - strlen(c);
               d = (char *) getmem(len*sizeof(char) + K_NAME_SIZE);
               strncat(d, a, len);
               strcat(uncomm_str, d);
               relmem(&d);
               a = c;
               e_comm = 0;
            }
         }
         else
            END_OF_LINE = TRUE;
      }

      if(b_comm == 1 && e_comm == 1)
      {
         if(strlen(a) > 0)
            strcat(uncomm_str, a);

         b_comm = e_comm = 0;
         goto END;
      }
      else if(b_comm == 1 && END_OF_LINE)
      {
         goto END;
      }
      else
      {
         pro_printf("Incorrect Parsing in remove_xml_comments_in_line\n");
         goto END;
      }
   }

  END:
   INIT_ARG(bcomm, b_comm);
   INIT_ARG(ecomm, e_comm);

   return(E_NO_ERROR);
}

/* Find ALL the occurences of string "find"  and replaces it with
   "replace" in the input string "str" */
static void find_and_replace(char *str, char *find, char *replace)
{
   char *post, *pre, *out, *a, *loop_out, *loop_in, *b;
   int len1, len2;
   int SEARCH = TRUE;

   /* initialize out */
   len2 = strlen(str);
   out     = (char *) getmem(sizeof(char)*len2 + K_NAME_SIZE);
   loop_in = (char *) getmem(sizeof(char) * len2 + K_NAME_SIZE);
   pre     = (char *) getmem(sizeof(char) * len2 + K_NAME_SIZE);
   post    = (char *) getmem(sizeof(char) * len2 + K_NAME_SIZE);
   loop_out= (char *)getmem(sizeof(char) * len2 + K_NAME_SIZE);

   /* initialize out */
   strcpy(out, "");

   /* copy the input */
   strcpy(loop_in, str);

   while (SEARCH == TRUE)
   {
      len2 = strlen(loop_in);

      a = strstr(loop_in, find);
      if(a != NULL)
      {
         /* reset the strings */
         strcpy(pre, "");
         strcpy(loop_out, "");
         strcpy(post, "");

         /* find post */
         len1 = strlen(a);
         b = a + strlen(find);
         strcpy(post, b);

         /* find pre */
         strncpy(pre, loop_in, len2 - len1);
         pre[len2 - len1] = '\0';

         /* copy pre and replace, ignore out */
         strcpy(loop_out, pre);
         strcat(loop_out, replace);

         /* finally, copy the string back to the input string */
         strcat(out, loop_out);

         /* reset the loop_in to the post string */
         strcpy(loop_in,post);
      }
      else
      {
         /* append the remaining part of the string */
         if(post != NULL)
            strcat(out, post);
         SEARCH = FALSE;
      }

   }

   /* finally, copy the output back to the input string */
   if(strcmp(out, ""))
      strcpy(str, out);

   relmem(&loop_in);
   relmem(&loop_out);
   relmem(&pre);
   relmem(&out);
   relmem(&post);
}


/*
 * Given an input string and the attribute name, the value
 * of the attribute is returned
 */
int get_xml_attribute_value(char *str, char *name, char *out)
{
   char *in_str, *local_out;
   int FLAG, status = TRUE;
   int length;
   char *a, *b, *c;
   char end_char;
   char xml_amp[8], xml_esc_amp[8], xml_gt[8], xml_esc_gt[8];
   char xml_lt[8], xml_esc_lt[8];

   if(str == NULL)
      return(E_ERROR);

   length =  strlen(str);
   in_str = (char *) getmem(sizeof(char)*length + 1);
   local_out = (char *) getmem(sizeof(char)*length + 1);

   strcpy(in_str, str);

   a = b = strstr(in_str, name);
   if (a != NULL)
   {
      b = b + (int)strlen(name);
      /* find '=' */
      FLAG = FALSE;
      while(!FLAG && (*b != '\0'))
      {
         if(*b == ' ')
         {
            b++;
            continue;
         }

         /* is the first non-whitespace char "=" */
         if(*b == '=')
         {
            /*push it to the next char*/
            *b++; FLAG = TRUE;
         }
         else
         {
            /* may be there is another occurence. */
            b = strstr(b , name);

            /* move pointer to the next occurence */
            if(b != NULL)
            {
               b = b + (int)strlen(name);
               continue;
            }
            else
               break;
         }
      }
      /* Get the value, it could be one of the following formats:
         " A B C " - in this case the spaces in between are read
         ABC - spaces ignored
      */
      if(FLAG == TRUE)
      {
         FLAG = FALSE;
         while(!FLAG && (*b != '\0'))
         {
            if(*b == ' ')
            { b++; continue; }

            if(*b == '"')
               end_char = '"';
            else
               end_char = ' ';

            b++;
            c = b ; /* set the start point */

            while(!(*b++ == end_char));
            b--;

            FLAG = TRUE;
            status = 1;
         }
      }
      else
      {
         status = -1;
      }

      if(FLAG == TRUE)
      {
         length = strlen(c) - strlen(b);
         strncpy(local_out,c, length);
         local_out[length] = '\0';
      }
      else
         pro_printf("Value Not Found.\n");

   }
   else
   {
      status = -1;
      pro_printf("Value Not Found.\n");
   }

   if((status == 1) && local_out != NULL)
   {
      /* Before the value is used, the special xml characters (&<>) should be
         converted back to normal vlaues */
      strcpy(xml_amp, XML_AMP);
      strcpy(xml_esc_amp, XML_ESC_AMP);
      strcpy(xml_gt, XML_GT);
      strcpy(xml_esc_gt, XML_ESC_GT);
      strcpy(xml_lt, XML_LT);
      strcpy(xml_esc_lt, XML_ESC_LT);

      /* Convert the escape chars to reserved chars */
      if((strstr(local_out, xml_esc_amp)) != NULL)
         find_and_replace(local_out, xml_esc_amp, xml_amp);

      if((strstr(local_out, xml_esc_gt)) != NULL)
         find_and_replace(local_out, xml_esc_gt, xml_gt);

      if((strstr(local_out, xml_esc_lt)) != NULL)
         find_and_replace(local_out, xml_esc_lt, xml_lt);
   }

   if((status == 1) && local_out != NULL)
      strcpy(out, local_out);


   relmem(&local_out);
   relmem(&in_str);

   if(status == 1)
      return(E_NO_ERROR);
   else
      return(E_ERROR);
}


/*
 * Function returns true, if the passed param is used only
 * by template manager
 */
int is_template_manager_only_param(wchar_t *param_wstr)
{
   int i;
   char param_str[K_LINESIZE];

   /* convert the wchar_t to char */
   wstrtos(param_str, param_wstr);


   /* Check if this is a template manager specific parameter */
   for(i= 0; i< NUM_OF_UNUSED_TM_PARAMS; i++)
   {
      if(!strcmp(param_str, UNUSED_TM_PARAMS[i].param_name))
         return (TRUE);
   }
   return(FALSE);
}

/*--------------------------------------------------------------------*/
static int update_seq_cutdata_for_template( Feat *nc_seq )
/*--------------------------------------------------------------------*/
{
 double  dbl = 0.0;
 int     speed_input;
 int     feed_input;

 if ( get_mfg_param( nc_seq, MF_SPEED_INPUT_STATUS, &dbl) == E_NO_ERROR )
 {
   speed_input = (int)dbl;

   if ( is_speed_from_tool( speed_input ) )
     update_seq_cut_data_from_tool( nc_seq, MF_APPLICATION_TYPE_ROUGHING,
                                    SPINDLE_CUTDATA_PARAM );

   if ( is_fin_speed_from_tool( speed_input ) )
     update_seq_cut_data_from_tool( nc_seq, MF_APPLICATION_TYPE_FINISHING,
                                    SPINDLE_CUTDATA_PARAM );
 }

 if ( get_mfg_param( nc_seq, MF_FEED_INPUT_STATUS, &dbl) == E_NO_ERROR )
 {
   feed_input = (int)dbl;

   if ( is_cut_feed_input_from_tool( feed_input ) )
     update_seq_cut_data_from_tool( nc_seq, MF_APPLICATION_TYPE_ROUGHING,
                                    FEED_CUTDATA_PARAM );

   if ( is_fin_feed_input_from_tool( feed_input ) )
     update_seq_cut_data_from_tool( nc_seq, MF_APPLICATION_TYPE_FINISHING,
                                    FEED_CUTDATA_PARAM );
 }

 return ( E_NO_ERROR );
}

/*--------------------------------------------------------------------*/
PRO_STATIC int is_cdplayer_shown()
/*--------------------------------------------------------------------*/
{
    return cdplayer_shown;
}

/*--------------------------------------------------------------------*/
PRO_STATIC void set_cdplayer_shown(int flag)
/*--------------------------------------------------------------------*/
{
    cdplayer_shown = flag;
}
