#include      "const.h"
#include      <pro_memory.h>
#include      "pro_wstring.h"
#include      "linestyle.h"
#include      "mathcons.h"
#include      "sysmath.h"
#include      "errors.h"
#include      "utility.h"
#include      "bindcall.h"
#include      <dbg_crash.h>
#include      <proprintf.h>
#include      <windprims.h>
#include <ct_win_syscall_proto.h>
#include <ctfileutil_proto.h>
#include <cu_msgutil_proto.h>
#include <hardware.h>
#include <pro_widec.h>
#include <ptc_memory.h>
#include <runmode.h>
#include <sysstdlib.h>
#include <utility3_proto.h>
#include <windmgr_proto.h>
#include <cu_init_proto.h>

/*****************************************************************************

   Date       Version    Author        Description
  ----------------------------------------------------------------------------
  09-jan-90   A-18-04    lokay    $$1  Created
  26-mar-90   A-20-02    lokay    $$2  added scale_flag
  16-Jul-1990 A-21-08	 MIP	  $$3  removed unwanted definition
  15-aug-90   A-21-20    lokay    $$4  combined files to optimize performance
  15-sep-90   A-21-28    lokay    $$5  reset ref_id_num in pro_load_std_line..
                                       move loadlnstyl.c funcs here
  06-oct-90   A-23-01    lokay    $$6  fix setting of default line style
  08-Mar-90   A-25-18    ZMS      $$7  First check current directory in
                                       pro_load_linestyle()
  01-NOV-91   A-29-16    Chris    $$8  Added DASHFONT
  21-Nov-91   c-01-11    blm      $$9  Add window ptr to dep_check_line_style_support
  13-Dec-91 C-01-18	amidon	  $$10	Moved dep_ fn to window fn struct.
  12-Feb-92 C-01-30 Jahn $$11 Returned status in get_ref_id().
  04-Mar-92 C-02-01 Jahn $$12 Added linestyle_is_predefined().
  15-JUN-92 D-01-06 Chris $$13 Defined 2 new fonts; added adaptive_mask, end_offset
  23-JUL-92 D-01-13 KO    $$14 Added linestyle_ref_id_from_name().
  14-SEP-92 D-01-23 KO    $$15 Added get_font_user_id(),
                               is_system_defined_line_font().
  14-SEP-92 D-01-25 Chris $$16 Modification of linestyle lengths
  21-SEP-92 D-01-26 Chris $$17 copy_linestyle_info: copy default_length too
  24-sep-92 D-01-22 dmh   $$18 Windows 32 port - Split out file_name_table.
  22-MAR-93 D-02-18 Chris $$19 Display which linestyle file was not found
  29-MAR-93 D-02-19 Chris $$20 Corrected precision problems; corrected min stroke
			       fixed/varying pattern info
  30-JUN-93 E-01-02 Chris $$21 Added CTRLFONT_S_S
  21-MAR-95 E-07-07 Chris $$22 Added DASHFONT_S_S, PHANTONFONT_S_S, CTRLFONT_MID_L
  10-Oct-95 G-01-10 Amin  $$23 Force line font on Win 95
  12-Dec-95 G-01-17 Amin  $$24 Removed stroke for CTRLFONT and DASHFONT (Win95)
  02-Jan-96 G-03-01 hsu   $$25 0.0 dash segment not used to calculate min segment
22-Jan-96 G-03-01 jmichaud $$26 Remove (unused) second arg from rlsmem().
  12-Nov-96 H-01-17 hsu   $$27 Check line_style name before looking up
  04-Feb-97 H-03-01 Chris $$28 Private graphics includes
  31-Jan-97 H-03-02 hsu   $$29 Removed stroke for DOTFONT (Win95)
  18-May-98 I-01-09 dac   $$30 fix args to pro_restore_line_style_length
  31-Jan-00 J-01-01 dac   $$31 Pgl Port
  28-Feb-00 J-01-03 rmi   $$32 Pgl port
  16-Aug-00 J-03-01 jhk   $$33 Called PglLinestyleSetEquivProeLinestyle
  23-Jan-01 J-03-01 Chris      initialized pgl_linestyle field; added comments
  25-Jan-01 J-03-01 Chris      optimization
  10-Jun-01 J-03-02 Chris $$34 Changed syserr to crash
  21-Sep-01 J-03-09 jas   $$35 Removed WINDOWS_95 macro
  27-Nov-01 J-03-14 Chris $$36 Removed obsolete code
  09-Jun-03 K-01-09 Chris $$37 Used pro_memory.h
  01-Sep-04 K-03-10 TWH   $$38 Add INTMIT_LWW_HIDDEN linestyle
  01-Oct-04 K-03-11 rmi   $$39 No dash end offset for INTMIT_LWW_HIDDEN
  29-Nov-04 K-03-15 JJE   $$40 New linestyle PDFHIDDEN_LINESTYLE
  25-Jul-05 K-03-32 aamm  $$41 Change PglLinestyleSet... to PglLinestyleSetData
  16-Nov-05 K-03-36 HMR   $$42 Fix memory leak in pro_load_linestyle().
  27-Mar-06 L-01-05 TWH   $$43 Fix $40 PDFHIDDEN_LINESTYLE
  13-Jun-06 L-01-10 PROTO $$44 Automatic prototype creation
  12-Jul-06 L-01-13 npand $$45 Added get_max_line_style_id().
  08-Aug-06 L-01-14 npand $$46 Removed get_max_line_style_id().
  07-Nov-06 L-01-20 cm    $$47 some cleanups and comments
  08-Oct-08 L-03-18+ EYk  $$48 Added DOT_CTRL_FONT.
  05-Jan-09 L-03-23 TWH   $$49 pgl_define_linestyle skip non_public entries
  09-Feb-09 L-03-26 TWH   $$50 Undo #49
  31-Mar-09 L-03-29 EYk   $$51 Updated DOT_CTRL_FONT.
  20-Nov-11 P-10-14 PPB   $$52 Added get_pgl_linestyle
  09-Sep-13 P-20-41 aargade $$53 Made intmit_lww_hidden public under
                                 show_hidden_linestyle
  05-Sep-14 P-20-61 NERELLA $$54 Updated pro_load_linestyle
  16-Jul-15 P-30-13 NERELLA $$55 stored info of line fonts failed to load
  16-Dec-16 P-30-42 MBE   $$56 Restore the line style length of the right font.
  25-Jul-21 P-90-19 DevOps $$57 Changed return type of some functions to void
  09-Mar-26 Q-27-00 PROTO  $$58 Automatic prototype creation
  *****************************************************************************/



static wchar_t linestyle_names[MAX_LINE_STYLE][K_LINESIZE];
static Line_style linestyle_table[MAX_LINE_STYLE];
static int max_line_style_id = RESERVED_SLOTS;

static Linestyle_ids linestyle_id_table[MAX_LINE_STYLE];
static int ref_id_num = 0;


/*****************************************************************************/

int initialize_linestyle_names()
{
  int i;

  for (i = 0; i < MAX_LINE_STYLE; i++)
     linestyle_names[i][0] = NULL_WCHAR;

  return E_NO_ERROR;
}

/*******************************************************************/
/* Compute and set run time parameters for efficient processing of */
/* full and partial adaptive modes.                                */
/*******************************************************************/
int compute_linestyle_seg_info(Line_style *lnstyle)
{
 double head_off, tail_off, h_v, h_f, t_v, t_f, l_f, l_v;
 double tmp_len, seg_len1, seg_len2, min_stroke, head_len;
 double tail_len, *dashes, patt_length, ms_f, ms_v;
 double seg_len3, seg_len4, right_v, left_v, right_f, left_f;
 int i, fully_adaptive, head_dash_count, tail_dash_count;
 int start_index, end_index;
 struct seg_data *ptr;

 dashes = lnstyle->dash_list;
 fully_adaptive = (lnstyle->adaptive_mask == 0xFFFFFFFF);

 start_index = end_index = -1;
 patt_length = 0.0;
 for(i = 0; i < lnstyle->dash_count; i++)
  {
   if (start_index == -1 &&
       lnstyle->dash_offset + ALMOST_ZERO < patt_length + dashes[i])
     start_index = i;
   if (end_index == -1 &&
       lnstyle->dash_end_offset < patt_length + dashes[i])
     end_index = i;

   patt_length += dashes[i];
  }

 if (lnstyle->default_length == 0.0)  /* Initialize default_length */
   lnstyle->default_length = patt_length;

 if (lnstyle->dash_offset <= ALMOST_ZERO || lnstyle->dash_offset > patt_length ||
     fabs(lnstyle->dash_offset - patt_length) <= ALMOST_ZERO ||
     start_index > lnstyle->dash_count || start_index < 0)
   { lnstyle->dash_offset = 0.0;  start_index = 0; }
 if (lnstyle->dash_end_offset <= ALMOST_ZERO || lnstyle->dash_end_offset > patt_length ||
     fabs(lnstyle->dash_end_offset - patt_length) <= ALMOST_ZERO ||
     end_index > lnstyle->dash_count || end_index < 0)
   { lnstyle->dash_end_offset = 0.0; end_index = 0; }

 head_off = lnstyle->dash_offset;
 tail_off = lnstyle->dash_end_offset;

 h_v = h_f = t_v = t_f = l_f = l_v = right_v = right_f = left_v = left_f = 0.0;
 ms_f = ms_v = 0.0;
 head_dash_count = tail_dash_count = 0;
 for (tmp_len = 0.0, i=0; i<lnstyle->dash_count; tmp_len += dashes[i], i++)
  {
   seg_len1 = seg_len2 = seg_len3 = seg_len4 = min_stroke = 0.0;
   if (i>= start_index)
    {
     seg_len1 = (i == start_index) ?
                (tmp_len + dashes[i] - head_off) : dashes[i];
     head_dash_count++;  /* Number of segments from stroke start to font end */
    }

   if (i<= end_index)
    {
     seg_len2 = (i == end_index) ?
                (tail_off - tmp_len) : dashes[i];
     tail_dash_count++;  /* Number of segments from font start to stroke end */
    }

   if (fully_adaptive == FALSE && head_off < tail_off)
    {
     if (i >= end_index)
      {
       seg_len3 = (i == end_index) ?
                  (tmp_len + dashes[i] - tail_off) : dashes[i];
      }
     if (i <= start_index)
      {
       seg_len4 = (i == start_index) ?
                  (dashes[i] - seg_len1) : dashes[i];
      }
    }

   if (fabs(head_off - tail_off) <= ALMOST_ZERO)
     min_stroke = dashes[i];
   else
     if ( (head_off < tail_off && i>= start_index && i<= end_index) ||
          (head_off > tail_off && (i>= start_index || i<= end_index) ))
       min_stroke = (i == start_index) ? seg_len1 :
                    (i == end_index) ? seg_len2 : dashes[i];

   if (lnstyle->adaptive_mask & (1 << i))
    {
     h_v += seg_len1;  t_v += seg_len2;  right_v += seg_len3;
     left_v += seg_len4; l_v += dashes[i];
     ms_v += min_stroke;
    }
   else
    {
     h_f += seg_len1;  t_f += seg_len2;  right_f += seg_len3;
     left_f += seg_len4; l_f += dashes[i];
     ms_f += min_stroke;
    }
  }

 head_len = h_f + h_v;
 tail_len = t_f + t_v;

 /* Checksums */
 if (fabs(patt_length - head_off - head_len) > ALMOST_ZERO && get_run_mode())
   crash();
 if ((tail_off > 0.0 || tail_len > 0.0) &&
     fabs(tail_off - tail_len) > ALMOST_ZERO && get_run_mode())
   crash();
 /* Checksum min pattern length */
 if (fabs(head_off - tail_off) <= ALMOST_ZERO)
   min_stroke = patt_length;
 else
   min_stroke = (head_off < tail_off) ? (tail_off - head_off) :
                (patt_length - head_off + tail_off);
 if (fabs(min_stroke - ms_v - ms_f) > ALMOST_ZERO && get_run_mode())
   crash();

 if (head_off == 0.0)
  {  head_len = h_f = h_v = 0.0;  head_dash_count = 0;  }
 if (tail_off == 0.0)
  {  tail_len = t_f = t_v = 0.0;  tail_dash_count = 0;  }

 if (lnstyle->seg_info == NULL)
   lnstyle->seg_info = (struct seg_data *)getmem(sizeof(struct seg_data));
 ptr = lnstyle->seg_info;
 ptr->start_index = start_index;
 ptr->end_index = end_index;
 ptr->patt_length = patt_length;
 ptr->h_f = h_f;
 ptr->h_v = h_v;
 ptr->l_f = l_f;
 ptr->l_v = l_v;
 ptr->t_f = t_f;
 ptr->t_v = t_v;
 ptr->ms_f = ms_f;
 ptr->ms_v = ms_v;
 ptr->right_f = right_f;
 ptr->right_v = right_v;
 ptr->left_f = left_f;
 ptr->left_v = left_v;
 ptr->min_stroke = min_stroke;
 ptr->head_dash_count = head_dash_count;
 ptr->tail_dash_count = tail_dash_count;

 for (tmp_len=HUGE_DOUBLE,i=0; i<lnstyle->dash_count; i++)
   if ( (dashes[i] > ALMOST_ZERO) && (dashes[i] < tmp_len) )
     tmp_len = dashes[i];

 ptr->min_segment = tmp_len;

 return E_NO_ERROR;
}


/*****************************************************************************/
/* Define a linestyle in the PGL environment; create or update the linestyle */
/* as needed.                                                                */
/*****************************************************************************/
PRO_STATIC int pgl_define_linestyle(Line_style *linestyle_ptr)
{
 double dash_array[100], *dash_list;
 double factor_x = 1.0, factor_y = 1.0;
 PglName lsname;
 int i, predef = FALSE;
 PglError status = PGL_E_OK;

 pro_wsprintf(lsname, "proe_%ws", linestyle_ptr->name);

 if (linestyle_ptr->dash_count > 100)
   dash_list = (double *)getmem(sizeof(double) * linestyle_ptr->dash_count);
 else
   dash_list = dash_array;

 if (linestyle_ptr->pgl_linestyle == NULL)  /* i.e. never looked up in PGL yet */
 {
   if(wstrcmp(linestyle_ptr->name, L"SOLIDFONT") == 0)
   {
     predef = TRUE;
     status = PglFindLinestyle(PGL_SOLIDFONT, &linestyle_ptr->pgl_linestyle);
   }
   else if(wstrcmp(linestyle_ptr->name, L"DOTFONT") == 0)
   {
     predef = TRUE;
     status = PglFindLinestyle(PGL_DOTFONT, &linestyle_ptr->pgl_linestyle);
   }
   else if(wstrcmp(linestyle_ptr->name, L"CTRLFONT") == 0)
   {
     predef = TRUE;
     status = PglFindLinestyle(PGL_CTRLFONT, &linestyle_ptr->pgl_linestyle);
   }
   else if(wstrcmp(linestyle_ptr->name, L"PHANTOMFONT") == 0)
   {
     predef = TRUE;
     status = PglFindLinestyle(PGL_PHANTOMFONT, &linestyle_ptr->pgl_linestyle);
   }
   else if(wstrcmp(linestyle_ptr->name, L"DASHFONT") == 0)
   {
     predef = TRUE;
     status = PglFindLinestyle(PGL_DASHFONT, &linestyle_ptr->pgl_linestyle);
   }
  else
   {
    status = PglFindLinestyle(lsname, &linestyle_ptr->pgl_linestyle);
    if (status == PGL_E_OK)
      predef = TRUE;
   }

   if (predef && status != PGL_E_OK)
    {
     /* We were looking for one of the default PGL linefonts and did no find it! */
     dbg_err_crash("pgl_define_linestyle",
                   "Can't find default linestyle in PGL Database...");
    }

   if(linestyle_ptr->pgl_linestyle == NULL)  /* Not found; create it! */
    {
     /* Create a new PGL Linestyle and associate with Pro's linestyle structure */
     PglLinestyleCreate(lsname, &linestyle_ptr->pgl_linestyle);
    }
 }

 /* Linestyle segment lengths are in pixels; convert to SLUs */
 PglWindowGetPixelToSLUFactor(NULL, &factor_x, &factor_y);

 for (i=0; i<linestyle_ptr->dash_count; i++)
    dash_list[i] = linestyle_ptr->dash_list[i] * factor_x;

 PglLinestyleSetData(linestyle_ptr->pgl_linestyle,
                         linestyle_ptr->dash_count, dash_list,
                         linestyle_ptr->dash_offset * factor_x,
                         linestyle_ptr->dash_end_offset * factor_x,
                         linestyle_ptr->adaptive_mask,
                         linestyle_ptr->physical_id);

 if (linestyle_ptr->dash_count > 100)
   relmem(&dash_list);

 return 0;
}



/*****************************************************************************/
/* This function transfers linestyle data from incoming argument to the      */
/* linestyle table. Note that the pointer to dash_list is copied, not re-    */
/* allocated.  Therefore it is assumed that the calling routine has this     */
/* space static.                                                             */
/*****************************************************************************/
PRO_STATIC void load_linestyle(linestyle_ptr, physical_id)
  Line_style *linestyle_ptr;
  int physical_id;
{
  linestyle_table[physical_id].physical_id = physical_id;
  wstrcpy(linestyle_table[physical_id].name, linestyle_ptr->name);
  linestyle_table[physical_id].line_def = linestyle_ptr->line_def;
  linestyle_table[physical_id].cap_style = linestyle_ptr->cap_style;
  linestyle_table[physical_id].join_style = linestyle_ptr->join_style;
  linestyle_table[physical_id].dash_offset = linestyle_ptr->dash_offset;
  linestyle_table[physical_id].dash_end_offset = linestyle_ptr->dash_end_offset;
  linestyle_table[physical_id].dash_count = linestyle_ptr->dash_count;
  linestyle_table[physical_id].dash_list = linestyle_ptr->dash_list;
#if 0
  linestyle_table[physical_id].scale_flag = linestyle_ptr->scale_flag;
#endif
  linestyle_table[physical_id].fill_style = linestyle_ptr->fill_style;
  linestyle_table[physical_id].fill_rule = linestyle_ptr->fill_rule;
  linestyle_table[physical_id].tile = linestyle_ptr->tile;
  linestyle_table[physical_id].stipple = linestyle_ptr->stipple;

  {
    linestyle_table[physical_id].adaptive_mask = linestyle_ptr->adaptive_mask;
  }

  linestyle_table[physical_id].seg_info = NULL;
  linestyle_table[physical_id].default_length = linestyle_ptr->default_length;
  linestyle_table[physical_id].non_public = 0;
  wstrcpy (linestyle_names[physical_id], linestyle_ptr->name);
  pro_restore_line_style_length_ptr(&linestyle_table[physical_id]);
  compute_linestyle_seg_info(&linestyle_table[physical_id]);

  pgl_define_linestyle(&linestyle_table[physical_id]);
}

/*****************************************************************************/

 load_std_linestyles()
{
 wchar_t name[K_NAME_SIZE];
 int force_stroke;

 {
  force_stroke = FALSE;
 }

 /* Note: Dash list lengths and offsets are in pixels */

 /* SOLIDFONT */
 linestyle_table[SOLIDFONT].physical_id = 0;
 strtows(name, "SOLIDFONT");
 wstrcpy (linestyle_names[SOLIDFONT], name);
 wstrcpy (linestyle_table[SOLIDFONT].name, name);
 linestyle_table[SOLIDFONT].line_def = 0;
 linestyle_table[SOLIDFONT].cap_style = 1;
 linestyle_table[SOLIDFONT].join_style = 2;
 linestyle_table[SOLIDFONT].dash_offset = 0.0;
 linestyle_table[SOLIDFONT].dash_count = 0;
 linestyle_table[SOLIDFONT].dash_list = NULL;
#if 0
 linestyle_table[SOLIDFONT].scale_flag = FALSE;
#endif
 linestyle_table[SOLIDFONT].fill_style = 0;
 linestyle_table[SOLIDFONT].fill_rule = 1;
 linestyle_table[SOLIDFONT].tile = NULL;
 linestyle_table[SOLIDFONT].stipple = NULL;
 linestyle_table[SOLIDFONT].adaptive_mask = 0x00;
 linestyle_table[SOLIDFONT].dash_end_offset = 0.0;
 linestyle_table[SOLIDFONT].seg_info = NULL;
 linestyle_table[SOLIDFONT].default_length = 0.0;
 linestyle_table[SOLIDFONT].non_public = 0;
 assign_line_style_ref_id(linestyle_table[SOLIDFONT].name, SOLIDFONT);
 compute_linestyle_seg_info(&linestyle_table[SOLIDFONT]);
 pgl_define_linestyle(&linestyle_table[SOLIDFONT]);

 /* DOTFONT */
 linestyle_table[DOTFONT].physical_id = 1;
 strtows(name, "DOTFONT");
 wstrcpy (linestyle_names[DOTFONT], name);
 wstrcpy (linestyle_table[DOTFONT].name, name);
 linestyle_table[DOTFONT].line_def = 1;
 linestyle_table[DOTFONT].cap_style = 1;
 linestyle_table[DOTFONT].join_style = 2;
 linestyle_table[DOTFONT].dash_offset = 0.0;
 linestyle_table[DOTFONT].dash_count = 2;
 linestyle_table[DOTFONT].dash_list = (double *)getmem(sizeof(double) * 2);
 linestyle_table[DOTFONT].dash_list[0] = 1.0;
 linestyle_table[DOTFONT].dash_list[1] = 3.0;
#if 0
 linestyle_table[DOTFONT].scale_flag = FALSE;
#endif
 linestyle_table[DOTFONT].fill_style = 0;
 linestyle_table[DOTFONT].fill_rule = 1;
 linestyle_table[DOTFONT].tile = NULL;
 linestyle_table[DOTFONT].stipple = NULL;
 linestyle_table[DOTFONT].adaptive_mask = 0x00;
 linestyle_table[DOTFONT].dash_end_offset = 0.0;
 linestyle_table[DOTFONT].seg_info = NULL;
 linestyle_table[DOTFONT].default_length = 0.0;
 linestyle_table[DOTFONT].non_public = 0;
 assign_line_style_ref_id(linestyle_table[DOTFONT].name, DOTFONT);
 compute_linestyle_seg_info(&linestyle_table[DOTFONT]);
 pgl_define_linestyle(&linestyle_table[DOTFONT]);

 /* CTRLFONT */
 linestyle_table[CTRLFONT].physical_id = 2;
 strtows(name, "CTRLFONT");
 wstrcpy (linestyle_names[CTRLFONT], name);
 wstrcpy (linestyle_table[CTRLFONT].name, name);
 linestyle_table[CTRLFONT].line_def = 1;
 linestyle_table[CTRLFONT].cap_style = 1;
 linestyle_table[CTRLFONT].join_style = 2;
 linestyle_table[CTRLFONT].dash_offset = 0.0;
 linestyle_table[CTRLFONT].dash_count = 4;
 linestyle_table[CTRLFONT].dash_list = (double *)getmem(sizeof(double) * 4);
 linestyle_table[CTRLFONT].dash_list[0] = 3.8;
 linestyle_table[CTRLFONT].dash_list[1] = 3.8;
 linestyle_table[CTRLFONT].dash_list[2] = 11.4;
 linestyle_table[CTRLFONT].dash_list[3] = 3.8;
#if 0
 linestyle_table[CTRLFONT].scale_flag = FALSE;
#endif
 linestyle_table[CTRLFONT].fill_style = 0;
 linestyle_table[CTRLFONT].fill_rule = 1;
 linestyle_table[CTRLFONT].tile = NULL;
 linestyle_table[CTRLFONT].stipple = NULL;
 linestyle_table[CTRLFONT].adaptive_mask = 0x00;
 linestyle_table[CTRLFONT].dash_end_offset = 0.0;
 linestyle_table[CTRLFONT].seg_info = NULL;
 linestyle_table[CTRLFONT].default_length = 0.0;
 linestyle_table[CTRLFONT].non_public = 0;
 assign_line_style_ref_id(linestyle_table[CTRLFONT].name, CTRLFONT);
 compute_linestyle_seg_info(&linestyle_table[CTRLFONT]);
 pgl_define_linestyle(&linestyle_table[CTRLFONT]);

 /* PHANTOMFONT */
 linestyle_table[PHANTOMFONT].physical_id = 3;
 strtows(name, "PHANTOMFONT");
 wstrcpy (linestyle_names[PHANTOMFONT], name);
 wstrcpy (linestyle_table[PHANTOMFONT].name, name);
 linestyle_table[PHANTOMFONT].line_def = 1;
 linestyle_table[PHANTOMFONT].cap_style = 1;
 linestyle_table[PHANTOMFONT].join_style = 2;
 linestyle_table[PHANTOMFONT].dash_offset = 0.0;
 linestyle_table[PHANTOMFONT].dash_count = 6;
 linestyle_table[PHANTOMFONT].dash_list = (double *)getmem(sizeof(double) * 6);
 linestyle_table[PHANTOMFONT].dash_list[0] = 10.0;
 linestyle_table[PHANTOMFONT].dash_list[1] = 3.0;
 linestyle_table[PHANTOMFONT].dash_list[2] = 1.0;
 linestyle_table[PHANTOMFONT].dash_list[3] = 2.0;
 linestyle_table[PHANTOMFONT].dash_list[4] = 1.0;
 linestyle_table[PHANTOMFONT].dash_list[5] = 3.0;
#if 0
 linestyle_table[PHANTOMFONT].scale_flag = FALSE;
#endif
 linestyle_table[PHANTOMFONT].fill_style = 0;
 linestyle_table[PHANTOMFONT].fill_rule = 1;
 linestyle_table[PHANTOMFONT].tile = NULL;
 linestyle_table[PHANTOMFONT].stipple = NULL;
 if ( force_stroke == TRUE )
    linestyle_table[PHANTOMFONT].adaptive_mask = 0xFFFFFFFF;
 else
    linestyle_table[PHANTOMFONT].adaptive_mask = 0x00;
 linestyle_table[PHANTOMFONT].dash_end_offset = 0.0;
 linestyle_table[PHANTOMFONT].seg_info = NULL;
 linestyle_table[PHANTOMFONT].default_length = 0.0;
 linestyle_table[PHANTOMFONT].non_public = 0;
 assign_line_style_ref_id(linestyle_table[PHANTOMFONT].name, PHANTOMFONT);
 compute_linestyle_seg_info(&linestyle_table[PHANTOMFONT]);
 pgl_define_linestyle(&linestyle_table[PHANTOMFONT]);

 /* DASHFONT */
 linestyle_table[DASHFONT].physical_id = 4;
 strtows(name, "DASHFONT");
 wstrcpy (linestyle_names[DASHFONT], name);
 wstrcpy (linestyle_table[DASHFONT].name, name);
 linestyle_table[DASHFONT].line_def = 1;
 linestyle_table[DASHFONT].cap_style = 1;
 linestyle_table[DASHFONT].join_style = 2;
 linestyle_table[DASHFONT].dash_offset = 0.0;
 linestyle_table[DASHFONT].dash_count = 2;
 linestyle_table[DASHFONT].dash_list = (double *)getmem(sizeof(double) * 2);
 linestyle_table[DASHFONT].dash_list[0] = 11.4;
 linestyle_table[DASHFONT].dash_list[1] = 11.4;
#if 0
 linestyle_table[DASHFONT].scale_flag = FALSE;
#endif
 linestyle_table[DASHFONT].fill_style = 0;
 linestyle_table[DASHFONT].fill_rule = 1;
 linestyle_table[DASHFONT].tile = NULL;
 linestyle_table[DASHFONT].stipple = NULL;
 linestyle_table[DASHFONT].adaptive_mask = 0x00;
 linestyle_table[DASHFONT].dash_end_offset = 0.0;
 linestyle_table[DASHFONT].seg_info = NULL;
 linestyle_table[DASHFONT].default_length = 0.0;
 linestyle_table[DASHFONT].non_public = 0;
 assign_line_style_ref_id(linestyle_table[DASHFONT].name, DASHFONT);
 compute_linestyle_seg_info(&linestyle_table[DASHFONT]);
 pgl_define_linestyle(&linestyle_table[DASHFONT]);

 /* CTRLFONT_S_L */
 linestyle_table[CTRLFONT_S_L].physical_id = 5;
 strtows(name, "CTRLFONT_S_L");
 wstrcpy (linestyle_names[CTRLFONT_S_L], name);
 wstrcpy (linestyle_table[CTRLFONT_S_L].name, name);
 linestyle_table[CTRLFONT_S_L].line_def = 1;
 linestyle_table[CTRLFONT_S_L].cap_style = 1;
 linestyle_table[CTRLFONT_S_L].join_style = 2;
 linestyle_table[CTRLFONT_S_L].dash_offset = 3.8 / 2.0;
 linestyle_table[CTRLFONT_S_L].dash_count = 4;
 linestyle_table[CTRLFONT_S_L].dash_list = (double *)getmem(sizeof(double) * 4);
 linestyle_table[CTRLFONT_S_L].dash_list[0] = 3.8;
 linestyle_table[CTRLFONT_S_L].dash_list[1] = 3.8;
 linestyle_table[CTRLFONT_S_L].dash_list[2] = 11.4;
 linestyle_table[CTRLFONT_S_L].dash_list[3] = 3.8;
#if 0
 linestyle_table[CTRLFONT_S_L].scale_flag = FALSE;
#endif
 linestyle_table[CTRLFONT_S_L].fill_style = 0;
 linestyle_table[CTRLFONT_S_L].fill_rule = 1;
 linestyle_table[CTRLFONT_S_L].tile = NULL;
 linestyle_table[CTRLFONT_S_L].stipple = NULL;
 linestyle_table[CTRLFONT_S_L].adaptive_mask = 0x04;
 linestyle_table[CTRLFONT_S_L].dash_end_offset = 3.8 + 3.8 + 11.4;
 linestyle_table[CTRLFONT_S_L].seg_info = NULL;
 linestyle_table[CTRLFONT_S_L].default_length = 0.0;
 linestyle_table[CTRLFONT_S_L].non_public = 0;
 assign_line_style_ref_id(linestyle_table[CTRLFONT_S_L].name, CTRLFONT_S_L);
 compute_linestyle_seg_info(&linestyle_table[CTRLFONT_S_L]);
 pgl_define_linestyle(&linestyle_table[CTRLFONT_S_L]);


 /* CTRLFONT_L_L */
 linestyle_table[CTRLFONT_L_L].physical_id = 6;
 strtows(name, "CTRLFONT_L_L");
 wstrcpy (linestyle_names[CTRLFONT_L_L], name);
 wstrcpy (linestyle_table[CTRLFONT_L_L].name, name);
 linestyle_table[CTRLFONT_L_L].line_def = 1;
 linestyle_table[CTRLFONT_L_L].cap_style = 1;
 linestyle_table[CTRLFONT_L_L].join_style = 2;
 linestyle_table[CTRLFONT_L_L].dash_offset = (3.8 + 3.8);
 linestyle_table[CTRLFONT_L_L].dash_count = 4;
 linestyle_table[CTRLFONT_L_L].dash_list = (double *)getmem(sizeof(double) * 4);
 linestyle_table[CTRLFONT_L_L].dash_list[0] = 3.8;
 linestyle_table[CTRLFONT_L_L].dash_list[1] = 3.8;
 linestyle_table[CTRLFONT_L_L].dash_list[2] = 11.4;
 linestyle_table[CTRLFONT_L_L].dash_list[3] = 3.8;
#if 0
 linestyle_table[CTRLFONT_L_L].scale_flag = FALSE;
#endif
 linestyle_table[CTRLFONT_L_L].fill_style = 0;
 linestyle_table[CTRLFONT_L_L].fill_rule = 1;
 linestyle_table[CTRLFONT_L_L].tile = NULL;
 linestyle_table[CTRLFONT_L_L].stipple = NULL;
 linestyle_table[CTRLFONT_L_L].adaptive_mask = 0x04;
 linestyle_table[CTRLFONT_L_L].dash_end_offset = (3.8 + 3.8 + 11.4);
 linestyle_table[CTRLFONT_L_L].seg_info = NULL;
 linestyle_table[CTRLFONT_L_L].default_length = 0.0;
 linestyle_table[CTRLFONT_L_L].non_public = 0;
 assign_line_style_ref_id(linestyle_table[CTRLFONT_L_L].name, CTRLFONT_L_L);
 compute_linestyle_seg_info(&linestyle_table[CTRLFONT_L_L]);
 pgl_define_linestyle(&linestyle_table[CTRLFONT_L_L]);


 /* CTRLFONT_S_S */
 linestyle_table[CTRLFONT_S_S].physical_id = 7;
 strtows(name, "CTRLFONT_S_S");
 wstrcpy (linestyle_names[CTRLFONT_S_S], name);
 wstrcpy (linestyle_table[CTRLFONT_S_S].name, name);
 linestyle_table[CTRLFONT_S_S].line_def = 1;
 linestyle_table[CTRLFONT_S_S].cap_style = 1;
 linestyle_table[CTRLFONT_S_S].join_style = 2;
 linestyle_table[CTRLFONT_S_S].dash_offset = 3.8 / 2.0;
 linestyle_table[CTRLFONT_S_S].dash_count = 4;
 linestyle_table[CTRLFONT_S_S].dash_list = (double *)getmem(sizeof(double) * 4);
 linestyle_table[CTRLFONT_S_S].dash_list[0] = 3.8;
 linestyle_table[CTRLFONT_S_S].dash_list[1] = 3.8;
 linestyle_table[CTRLFONT_S_S].dash_list[2] = 11.4;
 linestyle_table[CTRLFONT_S_S].dash_list[3] = 3.8;
#if 0
 linestyle_table[CTRLFONT_S_S].scale_flag = FALSE;
#endif
 linestyle_table[CTRLFONT_S_S].fill_style = 0;
 linestyle_table[CTRLFONT_S_S].fill_rule = 1;
 linestyle_table[CTRLFONT_S_S].tile = NULL;
 linestyle_table[CTRLFONT_S_S].stipple = NULL;
 linestyle_table[CTRLFONT_S_S].adaptive_mask = 0x04;
 linestyle_table[CTRLFONT_S_S].dash_end_offset = 3.8 / 2.0;
 linestyle_table[CTRLFONT_S_S].seg_info = NULL;
 linestyle_table[CTRLFONT_S_S].default_length = 0.0;
 linestyle_table[CTRLFONT_S_S].non_public = 0;
 assign_line_style_ref_id(linestyle_table[CTRLFONT_S_S].name, CTRLFONT_S_S);
 compute_linestyle_seg_info(&linestyle_table[CTRLFONT_S_S]);
 pgl_define_linestyle(&linestyle_table[CTRLFONT_S_S]);

/* DASHFONT_S_S  Starts and ends on mid of start segment */
 linestyle_table[DASHFONT_S_S].physical_id = 8;
 strtows(name, "DASHFONT_S_S");
 wstrcpy (linestyle_names[DASHFONT_S_S], name);
 wstrcpy (linestyle_table[DASHFONT_S_S].name, name);
 linestyle_table[DASHFONT_S_S].line_def = 1;
 linestyle_table[DASHFONT_S_S].cap_style = 1;
 linestyle_table[DASHFONT_S_S].join_style = 2;
 linestyle_table[DASHFONT_S_S].dash_offset = 11.4 / 2.0;
 linestyle_table[DASHFONT_S_S].dash_count = 2;
 linestyle_table[DASHFONT_S_S].dash_list = (double *)getmem(sizeof(double) * 2);
 linestyle_table[DASHFONT_S_S].dash_list[0] = 11.4;
 linestyle_table[DASHFONT_S_S].dash_list[1] = 11.4;
#if 0
 linestyle_table[DASHFONT_S_S].scale_flag = FALSE;
#endif
 linestyle_table[DASHFONT_S_S].fill_style = 0;
 linestyle_table[DASHFONT_S_S].fill_rule = 1;
 linestyle_table[DASHFONT_S_S].tile = NULL;
 linestyle_table[DASHFONT_S_S].stipple = NULL;
 linestyle_table[DASHFONT_S_S].adaptive_mask = 0xFFFFFFFF;
 linestyle_table[DASHFONT_S_S].dash_end_offset = 11.4 / 2.0;
 linestyle_table[DASHFONT_S_S].seg_info = NULL;
 linestyle_table[DASHFONT_S_S].default_length = 0.0;
 linestyle_table[DASHFONT_S_S].non_public = 0;
 assign_line_style_ref_id(linestyle_table[DASHFONT_S_S].name, DASHFONT_S_S);
 compute_linestyle_seg_info(&linestyle_table[DASHFONT_S_S]);
 pgl_define_linestyle(&linestyle_table[DASHFONT_S_S]);

 /* PHANTOMFONT_S_S  Starts and ends on mid of start segment */
 linestyle_table[PHANTOMFONT_S_S].physical_id = 9;
 strtows(name, "PHANTOMFONT_S_S");
 wstrcpy (linestyle_names[PHANTOMFONT_S_S], name);
 wstrcpy (linestyle_table[PHANTOMFONT_S_S].name, name);
 linestyle_table[PHANTOMFONT_S_S].line_def = 1;
 linestyle_table[PHANTOMFONT_S_S].cap_style = 1;
 linestyle_table[PHANTOMFONT_S_S].join_style = 2;
 linestyle_table[PHANTOMFONT_S_S].dash_offset = 10.0 / 2.0;
 linestyle_table[PHANTOMFONT_S_S].dash_count = 6;
 linestyle_table[PHANTOMFONT_S_S].dash_list = (double *)getmem(sizeof(double) * 6);
 linestyle_table[PHANTOMFONT_S_S].dash_list[0] = 10.0;
 linestyle_table[PHANTOMFONT_S_S].dash_list[1] = 3.0;
 linestyle_table[PHANTOMFONT_S_S].dash_list[2] = 1.0;
 linestyle_table[PHANTOMFONT_S_S].dash_list[3] = 2.0;
 linestyle_table[PHANTOMFONT_S_S].dash_list[4] = 1.0;
 linestyle_table[PHANTOMFONT_S_S].dash_list[5] = 3.0;
#if 0
 linestyle_table[PHANTOMFONT_S_S].scale_flag = FALSE;
#endif
 linestyle_table[PHANTOMFONT_S_S].fill_style = 0;
 linestyle_table[PHANTOMFONT_S_S].fill_rule = 1;
 linestyle_table[PHANTOMFONT_S_S].tile = NULL;
 linestyle_table[PHANTOMFONT_S_S].stipple = NULL;
 linestyle_table[PHANTOMFONT_S_S].adaptive_mask = 0xFFFFFFFF;
 linestyle_table[PHANTOMFONT_S_S].dash_end_offset = 10.0 / 2.0;
 linestyle_table[PHANTOMFONT_S_S].seg_info = NULL;
 linestyle_table[PHANTOMFONT_S_S].default_length = 0.0;
 linestyle_table[PHANTOMFONT_S_S].non_public = 0;
 assign_line_style_ref_id(linestyle_table[PHANTOMFONT_S_S].name, PHANTOMFONT_S_S);
 compute_linestyle_seg_info(&linestyle_table[PHANTOMFONT_S_S]);
 pgl_define_linestyle(&linestyle_table[PHANTOMFONT_S_S]);

 /* CTRLFONT_MID_L */
 linestyle_table[CTRLFONT_MID_L].physical_id = 10;
 strtows(name, "CTRLFONT_MID_L");
 wstrcpy (linestyle_names[CTRLFONT_MID_L], name);
 wstrcpy (linestyle_table[CTRLFONT_MID_L].name, name);
 linestyle_table[CTRLFONT_MID_L].line_def = 1;
 linestyle_table[CTRLFONT_MID_L].cap_style = 1;
 linestyle_table[CTRLFONT_MID_L].join_style = 2;
 linestyle_table[CTRLFONT_MID_L].dash_offset = 11.4 / 2.0;
 linestyle_table[CTRLFONT_MID_L].dash_count = 4;
 linestyle_table[CTRLFONT_MID_L].dash_list = (double *)getmem(sizeof(double) * 4);
 linestyle_table[CTRLFONT_MID_L].dash_list[0] = 11.4;
 linestyle_table[CTRLFONT_MID_L].dash_list[1] = 3.8;
 linestyle_table[CTRLFONT_MID_L].dash_list[2] = 3.8;
 linestyle_table[CTRLFONT_MID_L].dash_list[3] = 3.8;
#if 0
 linestyle_table[CTRLFONT_MID_L].scale_flag = FALSE;
#endif
 linestyle_table[CTRLFONT_MID_L].fill_style = 0;
 linestyle_table[CTRLFONT_MID_L].fill_rule = 1;
 linestyle_table[CTRLFONT_MID_L].tile = NULL;
 linestyle_table[CTRLFONT_MID_L].stipple = NULL;
 linestyle_table[CTRLFONT_MID_L].adaptive_mask = 0xFFFFFFFF;
 linestyle_table[CTRLFONT_MID_L].dash_end_offset = 11.4 / 2.0;
 linestyle_table[CTRLFONT_MID_L].seg_info = NULL;
 linestyle_table[CTRLFONT_MID_L].default_length = 0.0;
 linestyle_table[CTRLFONT_MID_L].non_public = 0;
 assign_line_style_ref_id(linestyle_table[CTRLFONT_MID_L].name, CTRLFONT_MID_L);
 compute_linestyle_seg_info(&linestyle_table[CTRLFONT_MID_L]);
 pgl_define_linestyle(&linestyle_table[CTRLFONT_MID_L]);

 /* INTMIT_LWW_HIDDEN */
 linestyle_table[INTMIT_LWW_HIDDEN].physical_id = 11;
 strtows(name, "INTMIT_LWW_HIDDEN");
 wstrcpy (linestyle_names[INTMIT_LWW_HIDDEN], name);
 wstrcpy (linestyle_table[INTMIT_LWW_HIDDEN].name, name);
 linestyle_table[INTMIT_LWW_HIDDEN].line_def = 1;
 linestyle_table[INTMIT_LWW_HIDDEN].cap_style = 1;
 linestyle_table[INTMIT_LWW_HIDDEN].join_style = 2;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_offset = 0.0;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_count = 6;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list = (double *)getmem(sizeof(double) * 6);
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list[0] = 1.0;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list[1] = 1.0;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list[2] = 1.0;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list[3] = 1.0;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list[4] = 1.0;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_list[5] = 3.0;
#if 0
 linestyle_table[INTMIT_LWW_HIDDEN].scale_flag = FALSE;
#endif
 linestyle_table[INTMIT_LWW_HIDDEN].fill_style = 0;
 linestyle_table[INTMIT_LWW_HIDDEN].fill_rule = 1;
 linestyle_table[INTMIT_LWW_HIDDEN].tile = NULL;
 linestyle_table[INTMIT_LWW_HIDDEN].stipple = NULL;
 linestyle_table[INTMIT_LWW_HIDDEN].adaptive_mask = 0x20;
 linestyle_table[INTMIT_LWW_HIDDEN].dash_end_offset = 0.0;
 linestyle_table[INTMIT_LWW_HIDDEN].seg_info = NULL;
 linestyle_table[INTMIT_LWW_HIDDEN].default_length = 0.0;
   
 if (get_show_hidden_linestyle())
 linestyle_table[INTMIT_LWW_HIDDEN].non_public = 0;
 else
  linestyle_table[INTMIT_LWW_HIDDEN].non_public = 1;

 assign_line_style_ref_id(linestyle_table[INTMIT_LWW_HIDDEN].name, INTMIT_LWW_HIDDEN);
 compute_linestyle_seg_info(&linestyle_table[INTMIT_LWW_HIDDEN]);
 pgl_define_linestyle(&linestyle_table[INTMIT_LWW_HIDDEN]);

 /* PDFHIDDEN_LINESTYLE  - this is used for 2d interface pdf hidden lines */
 linestyle_table[PDFHIDDEN_LINESTYLE].physical_id = 12;
 strtows(name, "PDFHIDDEN_LINESTYLE");
 wstrcpy (linestyle_names[PDFHIDDEN_LINESTYLE], name);
 wstrcpy (linestyle_table[PDFHIDDEN_LINESTYLE].name, name);
 linestyle_table[PDFHIDDEN_LINESTYLE].line_def = 1;
 linestyle_table[PDFHIDDEN_LINESTYLE].cap_style = 1;
 linestyle_table[PDFHIDDEN_LINESTYLE].join_style = 2;
 linestyle_table[PDFHIDDEN_LINESTYLE].dash_offset = 0.0;
 linestyle_table[PDFHIDDEN_LINESTYLE].dash_count = 2;
 linestyle_table[PDFHIDDEN_LINESTYLE].dash_list = (double *)getmem(sizeof(double) * 2);
 linestyle_table[PDFHIDDEN_LINESTYLE].dash_list[0] = 3.0;
 linestyle_table[PDFHIDDEN_LINESTYLE].dash_list[1] = 2.0;
#if 0
 linestyle_table[PDFHIDDEN_LINESTYLE].scale_flag = FALSE;
#endif
 linestyle_table[PDFHIDDEN_LINESTYLE].fill_style = 0;
 linestyle_table[PDFHIDDEN_LINESTYLE].fill_rule = 1;
 linestyle_table[PDFHIDDEN_LINESTYLE].tile = NULL;
 linestyle_table[PDFHIDDEN_LINESTYLE].stipple = NULL;
 linestyle_table[PDFHIDDEN_LINESTYLE].adaptive_mask = 0xFFFFFFFF;
 linestyle_table[PDFHIDDEN_LINESTYLE].dash_end_offset = 0.0;
 linestyle_table[PDFHIDDEN_LINESTYLE].seg_info = NULL;
 linestyle_table[PDFHIDDEN_LINESTYLE].default_length = 0.0;
 linestyle_table[PDFHIDDEN_LINESTYLE].non_public = 1;
 assign_line_style_ref_id(linestyle_table[PDFHIDDEN_LINESTYLE].name, PDFHIDDEN_LINESTYLE);
 compute_linestyle_seg_info(&linestyle_table[PDFHIDDEN_LINESTYLE]);
 pgl_define_linestyle(&linestyle_table[PDFHIDDEN_LINESTYLE]);

 /* DOT_CTRL_FONT */
 linestyle_table[DOT_CTRL_FONT].physical_id = 13;
 strtows(name, "DOT_CTRL_FONT");
 wstrcpy (linestyle_names[DOT_CTRL_FONT], name);
 wstrcpy (linestyle_table[DOT_CTRL_FONT].name, name);
 linestyle_table[DOT_CTRL_FONT].line_def = 1;
 linestyle_table[DOT_CTRL_FONT].cap_style = 1;
 linestyle_table[DOT_CTRL_FONT].join_style = 2;
 linestyle_table[DOT_CTRL_FONT].dash_offset = 0.0;
 linestyle_table[DOT_CTRL_FONT].dash_count = 4;
 linestyle_table[DOT_CTRL_FONT].dash_list = (double *)getmem(sizeof(double) * 4);
 linestyle_table[DOT_CTRL_FONT].dash_list[0] = 1.0;
 linestyle_table[DOT_CTRL_FONT].dash_list[1] = 1.0;
 linestyle_table[DOT_CTRL_FONT].dash_list[2] = 3.0;
 linestyle_table[DOT_CTRL_FONT].dash_list[3] = 1.0;
#if 0
 linestyle_table[DOT_CTRL_FONT].scale_flag = FALSE;
#endif
 linestyle_table[DOT_CTRL_FONT].fill_style = 0;
 linestyle_table[DOT_CTRL_FONT].fill_rule = 1;
 linestyle_table[DOT_CTRL_FONT].tile = NULL;
 linestyle_table[DOT_CTRL_FONT].stipple = NULL;
 linestyle_table[DOT_CTRL_FONT].adaptive_mask = 0x00;
 linestyle_table[DOT_CTRL_FONT].dash_end_offset = 0.0;
 linestyle_table[DOT_CTRL_FONT].seg_info = NULL;
 linestyle_table[DOT_CTRL_FONT].default_length = 0.0;
 linestyle_table[DOT_CTRL_FONT].non_public = 0;
 assign_line_style_ref_id(linestyle_table[DOT_CTRL_FONT].name, DOT_CTRL_FONT);
 compute_linestyle_seg_info(&linestyle_table[DOT_CTRL_FONT]);
 pgl_define_linestyle(&linestyle_table[DOT_CTRL_FONT]);
 
 /* now set up the default line style */
 assign_line_style_ref_id(linestyle_table[SOLIDFONT].name, DEFAULT_LINE_STYLE);
}

/*****************************************************************************/

wchar_t *get_line_style_name(int reference_id)
{
  int physical_id;

  get_physical_id(reference_id, &physical_id);
  if (physical_id == -1)
     return (NULL);
  else
    return (linestyle_names[physical_id]);
}

/*****************************************************************************/

 PRO_STATIC int get_next_linestyle_table_slot()
{
  int i;

  for (i = RESERVED_SLOTS; i < MAX_LINE_STYLE; i++)
     if (linestyle_names[i][0] == NULL_WCHAR)
     {
        max_line_style_id = i;
        return (i);
     }

  return (NO_SPACE_IN_TABLE);
}

/*****************************************************************************/

 int linestyle_is_loaded(linestyle, physical_id)
  wchar_t linestyle[];
  int    *physical_id;
{
  int i;

  for (i = 0; i <= max_line_style_id; i++)
    if ( (linestyle_names[i][0] != NULL_WCHAR) &&
         (wu_strcmp(linestyle, linestyle_names[i]) == 0) )
    {
       if (physical_id != NULL)
          *physical_id = i;
       return (TRUE);
    }
  return (FALSE);
}


int linestyle_is_predefined(wchar_t *style_name, int *p_physical_id )
{
 int pre_defined;

 linestyle_is_loaded( style_name, p_physical_id );
 pre_defined = 0 <= *p_physical_id && *p_physical_id < RESERVED_SLOTS;

 return pre_defined;
}

int linestyle_is_public(int physical_id)
{
	if (physical_id >= 0 && physical_id <= max_line_style_id)
	{
		if (linestyle_table[physical_id].non_public == 1)
			return (FALSE);
	}
	return (TRUE);
}
/*****************************************************************************/

Line_style *get_linestyle_structure(int reference_id)
{
  int physical_id;

  get_physical_id(reference_id, &physical_id);
  if (physical_id == -1)
    return (NULL);
  else
    return(&(linestyle_table[physical_id]));
}
/*****************************************************************************/

int clear_linestyle_names()  /* clear ONLY the user defined names!! */
{
  int i, j;

  for (i = RESERVED_SLOTS; i <= max_line_style_id; i++)
  {
     linestyle_names[i][0] = NULL_WCHAR;
     if (linestyle_table[i].dash_list != NULL)
     {
      for (j = i+1; j <= max_line_style_id; j++)
       {
        if (linestyle_table[j].dash_list == linestyle_table[i].dash_list)
          linestyle_table[j].dash_list = NULL;
        if (linestyle_table[j].seg_info == linestyle_table[i].seg_info)
          linestyle_table[j].seg_info = NULL;
       }
      rlsmem((char *)linestyle_table[i].dash_list);
      linestyle_table[i].dash_list = NULL;
      rlsmem((char *)linestyle_table[i].seg_info);
      linestyle_table[i].seg_info = NULL;
      linestyle_table[i].pgl_linestyle = NULL;
     }
  }
  max_line_style_id = RESERVED_SLOTS;

  return E_NO_ERROR;
}

/*****************************************************************************/

int inquire_loaded_linestyles(wchar_t ***names, int **ref_ids)
{
  int i, index;
  static wchar_t *namearray[MAX_LINE_STYLE];
  static int id_array[MAX_LINE_STYLE];

  if (names == NULL) return 0;
  if (ref_ids == NULL) return 0;

  index = 0;
  /* the fact that the check below is "<=" means that the list of names
     will always be terminated by an entry with NULL in it */
  for (i = 0; i <= max_line_style_id; i++)
  {
    namearray[i] = NULL;
    id_array[i] = -1;
  }

  for (i = 0; i <= max_line_style_id; i++)
    if (linestyle_names[i][0] != NULL_WCHAR)
    {
       namearray[index] = linestyle_names[i];
       get_ref_id(i, &id_array[index]);
       index++;
    }

  *names = namearray;
  *ref_ids = id_array;
  return index; /* return count of items in the array */
}

/*****************************************************************************/

 initialize_linestyle_id_table()
{
  int i;

  for (i = 0; i < MAX_LINE_STYLE; i++)
  {
    linestyle_id_table[i].physical_id = -1;
    linestyle_id_table[i].reference_id = -1;
  }
  ref_id_num = 0;
  return E_NO_ERROR;
}

/*****************************************************************************/

void clear_all_ref_ids()
{
 int i;
 static wchar_t solidfont[]={'S','O','L','I','D','F','O','N','T',NULL_WCHAR};
 

  for (i = RESERVED_SLOTS; i <= max_line_style_id; i++)
  {
    linestyle_id_table[i].physical_id = -1;
    linestyle_id_table[i].reference_id = -1;
  }
  ref_id_num = SYSTEM_DEFINED_LINE_STYLES;
  clear_linestyle_file_names(max_line_style_id);
  clear_linestyle_names();
  assign_line_style_ref_id(solidfont, DEFAULT_LINE_STYLE);
}

/*****************************************************************************/

int get_ref_id(int phys_id, int *ref_id)
{
  int ii, found;

  found = FALSE;
  for (ii = 0; ii < ref_id_num && ! found; ii++)
     if (linestyle_id_table[ii].physical_id == phys_id)
     {
        *ref_id = linestyle_id_table[ii].reference_id;
        found = TRUE;
     }
  return found;
}

/*****************************************************************************/

void get_physical_id(int ref_id, int *phys_id)
{
  int i;

  *phys_id = -1;
  for (i = 0; i < ref_id_num; i++)
     if (linestyle_id_table[i].reference_id == ref_id)
     {
        *phys_id = linestyle_id_table[i].physical_id;
        return;
     }
  return;
}

/*****************************************************************************/

 assign_line_style_ref_id(linestyle_name, ref_id)
  wchar_t linestyle_name[];
  int ref_id;
{
  int physical_id, i, found;

  if (!linestyle_is_loaded(linestyle_name, &physical_id))
    return (E_NOT_FOUND);

  if (ref_id_num == MAX_LINE_STYLE)
    return (E_NO_SPACE);

  found = FALSE;
  for (i = 0; i < ref_id_num; i++)
    if (linestyle_id_table[i].reference_id == ref_id)
    {
      linestyle_id_table[i].physical_id = physical_id;
      found = TRUE;
    }

  if (!found)
  {
     linestyle_id_table[ref_id_num].reference_id = ref_id;
     linestyle_id_table[ref_id_num].physical_id = physical_id;
     ref_id_num++;
  }

  return (E_NO_ERROR);
}

/*****************************************************************************/

void copy_linestyle_info(from_struct, to_struct)
  Line_style *from_struct;
  Line_style *to_struct;
{
  to_struct->physical_id = from_struct->physical_id;
  wstrcpy (to_struct->name, from_struct->name);
  to_struct->line_def = from_struct->line_def;
  to_struct->cap_style = from_struct->cap_style;
  to_struct->join_style = from_struct->join_style;
  to_struct->dash_offset = from_struct->dash_offset;
  to_struct->dash_end_offset = from_struct->dash_end_offset;
  to_struct->dash_count = from_struct->dash_count;
  to_struct->dash_list = from_struct->dash_list;
#if 0
  to_struct->scale_flag = from_struct->scale_flag;
#endif
  to_struct->fill_style = from_struct->fill_style;
  to_struct->fill_rule = from_struct->fill_rule;
  to_struct->tile = from_struct->tile;
  to_struct->stipple = from_struct->stipple;
  to_struct->adaptive_mask = from_struct->adaptive_mask;
  to_struct->seg_info = from_struct->seg_info;
  to_struct->default_length = from_struct->default_length;
  to_struct->non_public = from_struct->non_public;
  to_struct->pgl_linestyle = from_struct->pgl_linestyle;
}

/*****************************************************************************

     wchar_t *pro_load_linestyle(path, linestyle_file_name)
       INPUT:
         wchar_t *path                    - the full directory path in
                                            system specific format
         wchar_t *linestyle_file_name     - a linestyle file containing the
                                             full line style description
       OUTPUT:
          TRUE if successful, FALSE otherwise

       - if the file is already loaded, return
       - if the file has not been loaded and there is space int the table
         - if path is not NULL, get the linestyle from the path specified
         - if path is NULL
           - get the file from the current working directory.
           - if the file does not exist in the current working directory,
             get the file from SPG_DIRECTORY/text/linestyle
         - validate all the information read in from the file
         - verify that the platform can support the specified line style

 *****************************************************************************/


Bool pro_load_linestyle(wchar_t path[], wchar_t file_name[])
{
  Linestyle_file *linestyle_file_ptr;
  Line_style local_linestyle;
  int linestyle_id, ugc_ret, delim;
  wchar_t linestyle_path[STR_LEN], local_obj_name[STR_LEN];
  wchar_t current_directory[STR_LEN];
  static wchar_t text_dir[] = {'%','t','e','x','t',
                    '%','l','i','n','e','s','t','y','l','e',NULL_WCHAR};
  static wchar_t extension[] = {'.','l','s','l',NULL_WCHAR};
  struct window *w_ptr;
if (is_linestyle_file_failed_to_load(file_name))
     return (FALSE);
     
  linestyle_file_ptr = NULL;

  /* check if the line style file name has already been loaded */
  if (is_linestyle_file_loaded(file_name, &linestyle_id))
     return (get_line_style_name(linestyle_id) != NULL);

  /* get the next free slot to load the line style */
  linestyle_id = get_next_linestyle_table_slot();
  if (linestyle_id == NO_SPACE_IN_TABLE)
     return (FALSE);

  /* open the file  and read it in */
  wstrcpy (local_obj_name, file_name);
  wstrcat (local_obj_name, extension);

  /* check in cwd for linestyle file */
  dir_get(current_directory, STR_LEN);
  delim = wstrlen(current_directory);
  if (current_directory[delim - 1] == DIR_DELIMITER)
    current_directory[delim - 1] = NULL_WCHAR;
  make_filepath(local_obj_name, current_directory, NULL, linestyle_path);
  ugc_ret = ugc_load_line_style(file_name, linestyle_path,
                                                   &linestyle_file_ptr);
  if (ugc_ret != E_NO_ERROR)
   {
    if (path != NULL)
     {
      delim = wstrlen(path);
      if (path[delim - 1] == DIR_DELIMITER)
       path[delim - 1] = NULL_WCHAR;
      make_filepath(local_obj_name, path, NULL, linestyle_path);
      ugc_ret = ugc_load_line_style(file_name, linestyle_path,
                                           &linestyle_file_ptr);
     }
    if (ugc_ret != E_NO_ERROR)
     {
      /* check in $SPG_DIRECTORY/text/linestyle for file  */
      make_filepath(local_obj_name, SYSTEM_DIR, text_dir, linestyle_path);
      ugc_ret = ugc_load_line_style(file_name, linestyle_path,
                                                &linestyle_file_ptr);
    }
  }

  if (ugc_ret != E_NO_ERROR)  /* if something went wrong, get out */
  {
    mark_linestyle_file_failed_to_load(file_name);
    msg_put ("unable to load line style file %ws", file_name);
    return (FALSE);
  }

  /* validate the info read in and generate run time structure */
  if (build_linestyle_info(linestyle_file_ptr, &local_linestyle) == E_NO_ERROR)
   {
    /* verify that the platform can handle it */
     w_ptr = get_current_window_address();

     load_linestyle(&local_linestyle, linestyle_id);
     set_linestyle_file_name(linestyle_id, file_name);
     /* dash_list was copied by reference in build and load, so don't free it */
     relmem(&linestyle_file_ptr);
     return (TRUE);
   }

  msg_put ("invalid values in line style file %s", file_name);
  if (linestyle_file_ptr->dash_list != NULL)
     relmem(&linestyle_file_ptr->dash_list);
  relmem(&linestyle_file_ptr);
  return (FALSE);
}
 /*****************************************************************************/
/*  Load the standard Pro/Engineer line styles (SOLIDFONT, PHANTOMFONT, etc  */
/*****************************************************************************/

void pro_load_std_linestyles()
{

  reset_linestyle_file_names();
  initialize_linestyle_id_table();
  initialize_linestyle_names();
  load_std_linestyles();
}


/*****************************************************************************/
int linestyle_ref_id_from_name(linestyle_name)
wchar_t linestyle_name[];
{
  int  ref_id;
  int phys_id;

  if (linestyle_is_loaded(linestyle_name, &phys_id))
     get_ref_id(phys_id, &ref_id);
  else
     ref_id = K_NOT_USED;
  return (ref_id);
}

/*****************************************************************************/
int is_system_defined_line_font(ref_id)
int ref_id;
{
 if (ref_id < SYSTEM_DEFINED_LINE_STYLES && ref_id >= 0)
    return(TRUE);
 else
    return(FALSE);
}

/*****************************************************************************/
int get_font_user_id(ref_id)
int ref_id;
{
 if (is_system_defined_line_font(ref_id))
    return(K_NOT_USED);
 else
    return(ref_id - RESERVED_SLOTS);
}

/*****************************************************************************/
void *get_pgl_linestyle(ref_id, p_physical_id)
int ref_id;
int *p_physical_id;
{
  Line_style *linestyle = get_linestyle_structure(ref_id);
  void *pglloc_linestyle = NULL;

  if (linestyle)
  {
    pglloc_linestyle = (void *)linestyle->pgl_linestyle;
    if (p_physical_id)
      *p_physical_id = linestyle->physical_id;
  }
  else if (p_physical_id)
      *p_physical_id = -1;

  return pglloc_linestyle;
}
