#include <btkcstdio.h>
#include <sysstdio.h>
#include <ent_type.h>
#include <pfastruc.h>
#include <pfascdir.h>
#include <pro_dir.h>
#include <pro_string.h>
#include <pro_widec.h>
#include <rbtree.h>
#include <dbg_crash.h>
#include <utility.h>
#include <xarray.h>
#include <proprintf.h>
#include <ct_win_syscall_proto.h>
#include <nt_registry.h>

/*
22-Jul-93 E-01-09 amido $$1   Created.
24-Aug-93 E-01-12 CJL   $$2   Changed func args to match prototype
31-Aug-93 E-01-13 CJL   $$3   Fixed getting NEXT_VERSION.
09-Sep-93 E-01-14 amido $$4   Changed entry-file checks.
17-Sep-93 E-01-15 amido $$5   Changed strcasecmp to u_strcmp.
27-Sep-93 E-01-17 amido $$6   Cast versions to int for array.
30-Sep-93 E-01-18 amido $$7   Pfa_get_versions() returns -1 error.
19-Oct-93 E-01-23+amido $$8   Pfa_read_dir() update.
09-Dec-93 E-01-31 amido $$9   Get versions of very long foreign names.
04-May-94 E-03-25 amido $$10  Speed up version search.
10-Nov-94 E-06-19 sfc   $$11  Fixed getting HIGHEST_VERSION.
20-Dec-94 E-06-21 VEG   $$12  Added/used pfa_search_file_versions()
11-May-95 E-06-40 amido $$13  We don't know case of Import files.
17-Aug-95 G-01-03 jmich $$14  Use XAR_COUNT() and BYTCPY() macros.
12-Sep-95 G-01-07 GAA   $$15  Changed calls to pfa_get_... (thread-sf)
22-Jan-96 G-03-01 jmich $$16  Include header of pro_printf() varargs funcs.
19-Aug-96 H-01-05 amido $$17  Cleaned some lint.
11-Sep-96 H-01-09 amido $$18  Start using PRO_STATIC
04-Aug-97 H-03-18 SGV   $$19  Removed prototype for upper_strncmp
31-Oct-97 H-03-32 aap   $$20  Add arg to os_readdir().
12-May-98 I-01-08 MBE   $$21  Normalize for code massager.
04-Jun-98 I-01-11 MBE   $$22  Removed ProFile vs. Pfa casts, etc.
29-Oct-98 I-01-25 aap   $$23  Case insensitive on NT.
28-Dec-98 I-03-07 aap   $$24  Rename profile.h => pfastruc.h
07-Sep-99 I-03-14 aap   $$25  Add documentation.
29-Oct-99 I-03-22 HMR   $$26  extracted pfa_get_file_vers_low().
01-Feb-00 I-03-27 aap   $$27  call ntver_insure_version_registered().
18-Mar-00 J-01-05 HMR   $$28  Fixed bug in previous extraction.
19-Mar-00 J-01-07 aap   $$29  Use pfa_open_dir_subset().
16-Aug-00 J-01-16 HMR   $$30  Use PfaCaseSensitiveLoc in pfa_get_versions().
10-Oct-00 J-01-20 MBE   $$31  Foreign files have next versions too.
10-May-01 J-03-02 HMR   $$32  Call PfaGetFileVersion method.
21-Sep-01 J-03-09 jas   $$33  Removed WINDOWS_95 macro
06-Dec-01 J-03-14 HMR   $$34  Fix invocation of PfaGetFileVersion.
07-Dec-01 J-03-14 aap    ""   add arg to pfa_open_dir_subset().  
06-Dec-02 J-03-39 HMR   $$35  Added arg in pfa_get_file_vers_low().
17-Apr-03 K-01-05 HMR   $$36  Various changes to support string versions.
09-Sep-03 K-01-15 HMR   $$37  Added pfa_same_versions().
20-Oct-03 K-01-16+ HMR  $$38  Check string versions in pfa_same_versions().
05-Nov-03 K-01-17+ HMR  $$39  Another fix in pfa_same_versions().
23-Feb-04 K-01-25 HMR   $$40  Another fix in pfa_same_versions().
05-May-04 K-03-01 HMR   $$41  Appease IBM in vers_filt_data.
20-Aug-04 K-03-09 HMR   $$42  Created pfa_set_file_version(), cleanup.
 12-Feb-06  L-01-02 PROTO $$43  Automatic prototype creation
 02-Aug-06  L-01-13 ksi   $$44  More unicode changes
03-May-08 L-03-08 HMR   $$45  NULL check in pfa_set_to_version().
09-Aug-08 L-03-17 HMR   $$46  Added pfa_set_version_if_missing().
30-Oct-09 L-05-09 aap   $$47  Use pfaloc_is_case_sensitive().
08-Jul-10 L-05-27 HMR   $$48  Make better use of is_vers_string().
25-Jan-12 P-10-16 HMR   $$49  Added pfa_normalize_old_version().
12-Mar-12 P-20-01 AC    $$50  Updated for Project 13028358
13-Jun-12 P-20-07 aap   $$51  Added NEXT_VERSION_OR_ZERO.
2-Jul-12 P-20-09 YGK    $$52 corrected driver call - we need files to get version
21-Jul-12 P-20-10 HMR   $$53  Expanded previous.
25-Jan-13 P-20-23 HMR   $$54  Added pfa_get_user_version().
10-Sep-13 P-20-38 aap   $$55  Added pfa_unset_version().
10-Feb-15 P-30-02 HMR   $$56  Check pfaloc_allows_numeric_versions().
12-May-15 P-30-10 NKV   $$57  Pfa update logic while searching in pfa_get_file_vers_low.
11-Jun-15 P-30-10 NKV   $$58  Fixed compilation issue.
15-Jun-15 P-30-10 aap   $$59  Avoid losing T_UG_FILE in #57.
02-Jul-15 P-30-12 HMR   $$60  Switched PFA from char to wchar_t.
02-Nov-16 P-30-42 aap   $$61  Perf tweak.
04-Aug-20 P-80-16 nnn   $$62 Do not normalize version if exact_filename is set on pfa
01-Nov-24 Q-13-22 aap   $$63 Added pfavers_index_dir().
09-Mar-26 Q-27-00 PROTO $$64 Automatic prototype creation
20-Mar-26 Q-27-02 ABHAYG $$65 Replace the XAR macros with corresponding function calls.
*/

/*! Category = "Version" */

#include <pfa_public.h>
#include <ctpfa_mt_protos.h>
#include <const.h>
#include <pfa.h>

struct vers_filt_data
{
   int      name_len;
   wchar_t *filename;
   INT_FUNC funci;
   void    *funci_data;
};

/*****************************************************************************/
/*ARGSUSED*/
static int vers_filter(Pfa *dir, wchar_t *entry_name, Bool ignored_dir_status,
                       struct vers_filt_data *vf_data, Pfa *ignored_pfa)
{
   int case_sen_vers;
   int case_ins_vers;
   int name_len = vf_data->name_len;
   wchar_t *fixed_name = NULL;

   case_sen_vers = case_ins_vers = -1;

   if (!wstrncmp(vf_data->filename, entry_name, name_len))
    {
     if (entry_name[name_len] == L'\0')
       case_sen_vers = 0;
     else if (entry_name[name_len] == VERSION_DELIMITER)
      {
       case_sen_vers = is_vers_wstring(entry_name + name_len + 1);
       if (case_sen_vers == 0)
         case_sen_vers = -1;
      }
    }
   else if (!wu_strncmp(vf_data->filename, entry_name, name_len))
    {
     if (entry_name[name_len] == L'\0')
      {
       case_ins_vers = 0;
       fixed_name = entry_name;
      }
     else if (entry_name[name_len] == VERSION_DELIMITER)
      {
       case_ins_vers = is_vers_wstring(entry_name + name_len + 1);
       if (case_ins_vers == 0)
         case_ins_vers = -1;
      }
    }
   (*(vf_data->funci))(case_sen_vers, case_ins_vers, fixed_name, vf_data->funci_data);

   /* "Skip" this entry: no need to parse a pfa for it. */
   return TRUE;
}

/* argmument 'func' call. seq.: (*func)(int  case_sen_vers, int  case_ins_vers,
                                        void *data); */
static void pfa_search_file_versions(ProFile *dir, wchar_t *filename,
                                     void (*funci)(), void *data)
{
 struct vers_filt_data vf_data;

 vf_data.filename = filename;
 vf_data.name_len = wstrlen(filename);
 vf_data.funci = (INT_FUNC) funci;
 vf_data.funci_data = data;
 /* Cheat: Do all the work in the vers_filter function. */
 pfaloc_call_FLPPPI(dir, PfaDirReadNext, NULL,
                    (void *)vers_filter, &vf_data,
                    (PFA_SCDIR_LIST_FILES | PFA_SCDIR_HIGHEST_VERS));
}

struct max_versions
 {
  int max_case_sen_vers;
  int max_case_ins_vers;
  Pfa *pfa;
 };

/******************************************************************************/
static void take_max_vers(int case_sen_vers, int case_ins_vers,
                          wchar_t *fixed_name,
                          struct max_versions *p_max_versions)
{
 if (case_sen_vers > p_max_versions->max_case_sen_vers)
   p_max_versions->max_case_sen_vers = case_sen_vers;

 if (case_ins_vers > p_max_versions->max_case_ins_vers)
   p_max_versions->max_case_ins_vers = case_ins_vers;
 if (p_max_versions->pfa && (fixed_name != NULL))
  {
   ProMdlName tmp_name = EMPTY_WSTRING;
   ProMdlExtension tmp_ext = EMPTY_WSTRING;
   /* The caller provided us with Pfa and (as a result of search)
      we think it should be updated with the file name.
      It's done as an attempt to fix the case of the file name.
   */
   if ((pfa_parse_to_components(fixed_name,
                                NULL, NULL,
                                tmp_name, tmp_ext, NULL)) == PFA_E_NO_ERROR)
    {
     if (tmp_name[0] != NULL_WCHAR)
       pfa_put_wname(p_max_versions->pfa, tmp_name);
     if (tmp_ext[0] != NULL_WCHAR)
      {
       /* use the low-level call to avoid overwriting T_UG_FILE */
       pfa_put_component(p_max_versions->pfa, PfaExtension, tmp_ext);
      }
    }
  }
}

struct all_versions
 {
  int *case_sen_vers;
  int *case_ins_vers;
 };

/******************************************************************************/
static void collect_revs(int case_sen_vers, int case_ins_vers,
                         wchar_t *fixed_name,
                         struct all_versions *p_all_versions)
{
 if (case_sen_vers != -1)
   xar_append(&p_all_versions->case_sen_vers, 1, &case_sen_vers);

 if (case_ins_vers != -1)
   xar_append(&p_all_versions->case_ins_vers, 1, &case_ins_vers);
}

/*****************************************************************************/

struct build_cache_data {
   RBtree *p_tree;
   int sens;
};

struct vers_index_node {
   wchar_t name_ext[K_PATH_SIZE];
   int vers;
};

void pfavers_free_cache_data(RBtree *p_cache)
{
   Bnode *p_node = NULL;
   while (p_node = next_btree_node(p_cache, p_node, 0))
   {
      rlsmem(p_node->p_data);
   }
   release_rbtree(&p_cache);
}

aiAppId pfaverscache_appdata_id()
{
 APPINFO_REGISTER_FREE(pfavers_appdata_type, (AppinfoFreeFunc)pfavers_free_cache_data);
 return pfavers_appdata_type;
}

static int vers_cache_entry_compare(struct vers_index_node *n1, struct vers_index_node *n2)
{
   int diff = wstrcmp(n1->name_ext, n2->name_ext);
   if (diff == 0 && (n2->vers > n1->vers))
   {
      /* rbtree will decline to add node with matching name.ext .
         remember higher version in existing node */
      n1->vers = n2->vers;
   }
   return diff;
}

static struct vers_index_node *pfaver_index_cache_entry(Pfa *dir, wchar_t *entry_name, Bool sens)
{
   struct vers_index_node *entry = NULL;
   wchar_t *last_dot;
   
   if (entry_name == NULL)
      return NULL;
   
   entry = GETSTRUCT(struct vers_index_node);
   wstrcpy(entry->name_ext, entry_name);
   last_dot = wstrrchr(entry->name_ext, (wchar_t)VERSION_DELIMITER);
   if (last_dot != NULL && (entry->vers = is_vers_wstring(last_dot + 1)) != 0)
   {
      *last_dot = L'\0';
   }

   if (!sens)
      wcnvrt_to_lower(entry->name_ext, entry->name_ext);

   return entry;
}

/*ARGSUSED*/
static int build_vers_cache_filter(Pfa *dir, wchar_t *entry_name, Bool ignored_dir_status,
                                   struct build_cache_data *p_data, Pfa *ignored_pfa)
{
   struct vers_index_node *entry = NULL;
   
   if (entry_name == NULL)
      return TRUE;

   entry = pfaver_index_cache_entry(dir, entry_name, p_data->sens);

   if (entry != NULL &&
       !insert_data_into_rbtree(p_data->p_tree, (char *)entry, vers_cache_entry_compare))
   {
      /* existing item was updated if needed in vers_cache_entry_compare */
      rlsmem(entry);
   }

   /* "Skip" this entry: no need to parse a pfa for it. */
   return TRUE;
}

int pfavers_index_dir(Pfa *dir_pfa)
{
   Bool alloc = TRUE; /* get some idea from search code */

   int vers_flag = pfaloc_call_F(dir_pfa, PfaLocGetVersioning);
   if ( vers_flag != PFALOC_VERS_NUMERIC && vers_flag != K_NOT_USED )
      return FALSE;
   
   RBtree **pp_cache = (RBtree **) pfadc_find_appdata(dir_pfa, pfaverscache_appdata_id(), TRUE);
   if (pp_cache == NULL)
      return FALSE;
   if (*pp_cache == NULL)
   {
      struct build_cache_data data;
      ZERO_OUT_STRUCT(data);
      
      if (pfa_open_dir_subset(dir_pfa, NULL, NULL, PFA_SCDIR_LIST_FILES) != PFA_E_NO_ERROR)
         return (FALSE);
      *pp_cache = data.p_tree = new_rbtree(16);
      data.sens = pfaloc_is_case_sensitive(dir_pfa);
      pfaloc_call_FLPPPI(dir_pfa, PfaDirReadNext, NULL,
                         (void *)build_vers_cache_filter, &data,
                         (PFA_SCDIR_LIST_FILES | PFA_SCDIR_HIGHEST_VERS));
      (void) pfa_close(dir_pfa);
   }

   return (*pp_cache != NULL);
}

static Bool pfavers_cached_version_value(Pfa *dir_pfa, wchar_t *filewname, int *r_max_vers)
{
   RBtree **pp_cache = (RBtree **) pfadc_find_appdata(dir_pfa, pfaverscache_appdata_id(), FALSE);
   if (pp_cache == NULL || *pp_cache == NULL)
      return FALSE;

   struct vers_index_node *lookup = pfaver_index_cache_entry(dir_pfa, filewname, pfaloc_is_case_sensitive(dir_pfa));
   if (lookup != NULL)
   {
      RBnode *p_node = NULL; 
      if (btree_locate(*pp_cache, (void *) lookup, vers_cache_entry_compare,
                       (Bnode**)&p_node, NULL, NULL))
      {
         struct vers_index_node *entry = (struct vers_index_node *)p_node->p_data;
         INIT_ARG(r_max_vers, entry->vers);
      }
      rlsmem(lookup);
      return (p_node != NULL);
   }
   
   return FALSE;
}

static int pfa_get_file_vers_lower(Pfa *dir_pfa, wchar_t *filewname, int option,
                                   int *out_version, int obj_type,
                                   Bool check_driver, Pfa *full_pfa)
{
   struct max_versions max_vers;
   Pfa *file_pfa = NULL;
   char string_vers[K_PATH_SIZE];
   int loc_version;

   max_vers.max_case_sen_vers = max_vers.max_case_ins_vers = -1;
   if (full_pfa && direct_open_type(pfa_get_obj_type(full_pfa)))
     max_vers.pfa = full_pfa;
   else
     max_vers.pfa = NULL;
   if (dir_pfa == NULL)
      return (PFA_E_NO_ACCESS);

   if (option == NEXT_VERSION_OR_ZERO && use_creo_local_model_versioning())
     option = NEXT_VERSION;
   
   if (check_driver  &&  pfa_alloc_pro_file(&file_pfa) == PFA_E_NO_ERROR  &&
       pfa_w_parse_to_pro_file_private(file_pfa, dir_pfa, filewname,
                                       ENTRY_IS_NOT_DIR) == PFA_E_NO_ERROR  &&
       pfaloc_call_FIIPP(file_pfa, PfaGetFileVersion, option, FALSE,
                         &loc_version, string_vers) == PFA_E_NO_ERROR)
   {
      *out_version = loc_version;
   }
   else
   {
      if (pfavers_cached_version_value(dir_pfa, filewname, out_version))
         /* EMPTY */;
      else
      {
         if (pfa_open_dir_subset(dir_pfa, filewname, NULL,
                                 PFA_SCDIR_LIST_FILES) != PFA_E_NO_ERROR)
            return (PFA_E_NO_ACCESS);
         
         pfa_search_file_versions(dir_pfa, filewname, take_max_vers, &max_vers);
         (void) pfa_close(dir_pfa);
      
         *out_version = max_vers.max_case_sen_vers;

         /* We should not be stricter than the OS */
         if (max_vers.max_case_ins_vers > *out_version
             &&  ! pfaloc_is_case_sensitive(dir_pfa))
            *out_version = max_vers.max_case_ins_vers;
      }
   }

   pfa_free_pro_file(&file_pfa);
   
   if (*out_version == -1)
   {
      if (option == HIGHEST_VERSION)
         return (PFA_E_NOT_FOUND);

      if (option == NEXT_VERSION_OR_ZERO)
        *out_version = 0;
      else if (pfaloc_allows_numeric_versions(dir_pfa))
        *out_version = 1;
      else
      {
         dbg_print_info("pfa_get_file_vers_low",
                        "omitting next version for %ws", filewname);
         *out_version = 0;
      }
   }
   else if (*out_version == 0)
   {
      if (option != HIGHEST_VERSION && option != NEXT_VERSION_OR_ZERO)
      {
         if (pfaloc_allows_numeric_versions(dir_pfa))
            *out_version = (option == NEXT_VERSION) ? 2 : 1;
         else
            dbg_print_info("pfa_get_file_vers_low",
                           "omitting next version for %ws", filewname);
      }
   }
   else if (option == NEXT_VERSION || option == NEXT_VERSION_OR_ZERO)
      (*out_version)++;

#if OPER_SYS == WINDOWS_32
   ntver_insure_version_registered(*out_version);
#endif

   return(PFA_E_NO_ERROR);
}

/*ARGSUSED*/
int pfa_get_file_vers_low(Pfa *dir_pfa, wchar_t *filename, int option,
                          int *out_version, int obj_type, Bool check_driver)
{
 return pfa_get_file_vers_lower(dir_pfa, filename, option,
                                out_version, obj_type, check_driver, NULL);
}

/******************************************************************************/

/*: Return the requested version of the file. */
/*!Arguments: file           - Pfa describing a file
              version_option - THIS_VERSION HIGHEST_VERSION NEXT_VERSION
              out_version    - pointer to integer                             */
/*!Returns: PFA_E_NO_ERROR      - Successful.
            PFA_E_NOT_VALID     - file is not a valid ProFile structure.
            PFA_E_INVALID_INPUT - version option was ALL_VERSIONS or unrecognized,
                                  or out_version was NULL.
            PFA_E_NOT_FOUND     - version option was HIGHEST_VERSION and the file
                                  was not in the directory.
            PFA_E_NOT_ALLOWED   - version option was NEXT_VERSION and the file
                                  was versionless.
            PFA_E_NO_ACCESS     - Cannot read the Pfa's directory to find the
                                  highest or next version.                      */
/* If "version_option" is THIS_VERSION, the version in the given
   ProFile structure is returned in &quot;out_version&quot;.

   For HIGHEST_VERSION, the highest version of the file's name and
   extension in the directory is found and returned. If there is only
   one version of the file and it is versionless, 1 will be output for
   Pro objects and 0 will be output for files of type T_FOREIGN.

   If NEXT_VERSION, the highest version is found as described above,
   and if it is not 0, out_version will be the highest version plus
   1. Versionless (foreign) files will return an error. */
int pfa_get_file_vers(Pfa *file, int option, int *out_version)

{
 ProFile *dir;
 wchar_t  filename[K_PATH_SIZE], string_vers[K_PATH_SIZE];
 int      ret;
 int      loc_version;

 if (INVALID_FILE(file))
   return (PFA_E_NOT_VALID);

 if (out_version == NULL)
   return (PFA_E_INVALID_INPUT);

 string_vers[0] = L'\0';
 switch (option)
 {
 case THIS_VERSION:
    *out_version = file->version;
    break;
 case HIGHEST_VERSION:
 case NEXT_VERSION:
    /* For PDM locations that have their own ideas about versioning */
    ret = pfaloc_call_FIIPP(file, PfaGetFileVersion, option, FALSE,
                            &loc_version, string_vers);
    if (ret == PFA_E_NO_ERROR)
    {
       *out_version = loc_version;
       if (loc_version < 0)
          return(PFA_E_NOT_FOUND);   /* driver is certain no version exists */
    }
    else
    {
       dir = extract_pfa_dir(file);
       (void) pfa_get_file_wname(file, PFA_NAME_EXTENSION, filename);
       ret = pfa_get_file_vers_low(dir, filename, option, out_version,
                                   file->obj_type, FALSE);
       (void) pfa_free_pro_file(&dir);
       return(ret);
    }
    break;
 default:
    return (PFA_E_INVALID_INPUT);
 }
 return (PFA_E_NO_ERROR);
}

/******************************************************************************/
int pfa_get_versions(ProFile *file, int **p_version_array)

{
 ProFile *dir;
 wchar_t  file_name[K_PATH_SIZE];
 struct all_versions all_vers;

 if (is_run_mode(RMODE_PRO_FILE_DEBUG))
   dbg_print_info("pfa_get_versions", "Find all versions of this file.");

 if (INVALID_FILE(file) || p_version_array == NULL)
   return (-1);

 dir = extract_pfa_dir(file);
 if (dir == NULL)
   return (-1);

 if (pfa_open_dir_subset(dir, NULL, NULL,
                         PFA_SCDIR_LIST_FILES) != PFA_E_NO_ERROR)
  {
   (void) pfa_free_pro_file(&dir);
   return (-1);
  }

 (void) pfa_get_file_wname(file, PFA_NAME_EXTENSION, file_name);
 all_vers.case_sen_vers = XAR_BEGIN(int, 5);
 all_vers.case_ins_vers = XAR_BEGIN(int, 5);

 pfa_search_file_versions(dir, file_name, collect_revs, &all_vers);

 *p_version_array = all_vers.case_sen_vers;
 all_vers.case_sen_vers = NULL;

 if ( ! pfaloc_is_case_sensitive(dir))
   (void) xar_append(p_version_array, XAR_COUNT(&all_vers.case_ins_vers),
                     all_vers.case_ins_vers);

 xar_free(&all_vers.case_ins_vers);
 (void) pfa_dispose(&dir);

 return (XAR_COUNT(p_version_array));
}

static int pfa_set_to_version(Pfa *pfa, int in_option, int *out_vers)
{
   int      status, version;
   wchar_t  string_vers[K_PATH_SIZE], filename[K_PATH_SIZE];
   Pfa     *dir;
   int      option = in_option;

   if (INVALID_FILE(pfa))
      return(PFA_E_INVALID_INPUT);

   if (option == NEXT_VERSION_OR_ZERO)
     option = NEXT_VERSION;

   string_vers[0] = L'\0';
   status = pfaloc_call_FIIPP(pfa, PfaGetFileVersion, option,  FALSE,
                              &version, string_vers);
   if (status == PFA_E_NO_ERROR)
   {
      if (string_vers[0] != L'\0')
         pfa_put_component_len(pfa, PfaVersion,
                               string_vers, wstrlen(string_vers), FALSE);
      else if (version < 0)
         return(PFA_E_NOT_FOUND);   /* driver is certain no version exists */
   }
   else
   {
      dir = extract_pfa_dir(pfa);
      (void) pfa_get_file_wname(pfa, PFA_NAME_EXTENSION, filename);
      status = pfa_get_file_vers_lower(dir, filename, in_option, &version,
                                       pfa->obj_type, FALSE, pfa);
      (void) pfa_free_pro_file(&dir);
      if (status != PFA_E_NO_ERROR)
         return(status);
   }
   
   INIT_ARG(out_vers, version);
   status = pfa_put_version(pfa, version);

   return(status);
}

int pfa_set_to_highest_version(Pfa *pfa, int *out_vers)
{
   return(pfa_set_to_version(pfa, HIGHEST_VERSION, out_vers));
}

int pfa_set_to_next_version(Pfa *pfa)
{
   return(pfa_set_to_version(pfa, NEXT_VERSION, NULL));
}

int pfa_set_to_next_version_or_zero(Pfa *pfa)
{
   return(pfa_set_to_version(pfa, NEXT_VERSION_OR_ZERO, NULL));
}

Bool pfa_same_versions(Pfa *pfa1, Pfa *pfa2)
{
   int vers_flag1, vers_flag2;
   if (INVALID_FILE(pfa1)  ||  INVALID_FILE(pfa2))
      return(FALSE);

   vers_flag1 = pfaloc_call_F(pfa1, PfaLocGetVersioning);
   vers_flag2 = pfaloc_call_F(pfa2, PfaLocGetVersioning);

   /* locations which don't bind this method are treated like local disk */
   if (vers_flag1 == PFALOC_VERS_INVALID)
      vers_flag1 = PFALOC_VERS_NUMERIC_MULTI;   
   if (vers_flag2 == PFALOC_VERS_INVALID)
      vers_flag2 = PFALOC_VERS_NUMERIC_MULTI;   

   /* Compare WS (single string version) to CS (multiple string versions) */
   vers_flag1 &= ~PFALOC_VERS_MULTI;
   vers_flag2 &= ~PFALOC_VERS_MULTI;
   if (vers_flag1 != vers_flag2)
      return FALSE; /* locations' versions are not comparable */

   /* The hack can below go away once we clean up the non-standard locations
      which occasionally give a version of "1" for a file with no version.
      Affected tests include wf_ftp_web_dbclick1_240, wf_retrieve_zip_file. */
   if ((vers_flag1 & PFALOC_VERS_NUMERIC) && pfa1->version != pfa2->version
       && (pfa1->version < 0 || pfa1->version > 1
           || pfa2->version < 0 || pfa2->version > 1))
      return FALSE;

   /* string version mismatch only counts if both pfa's actually have
      the version set. Perhaps it would be better if Windchill gave us
      version names even when we were scanning a folder in
      not-all-versions mode. */
   if ((vers_flag1 & PFALOC_VERS_STRING) &&
       pfa_has_component(pfa1, PfaVersion) &&
       pfa_has_component(pfa2, PfaVersion) && 
       pfas_component_wstrcmp(pfa1, pfa2, PfaVersion))
      return(FALSE);

   return(TRUE);
}

/* Mimics behavior of pro_get_version_name_low().  If pfa version is:
   vers > 1  Do an access check, return PFA_E_NO_ERROR if exists,
             return PFA_E_NOT_FOUND if it doesn't.
   vers = 1  Do an access check, return PFA_E_NO_ERROR if exists, 
             if not, check if it exists woth no version. if it does,
             clear version and return PFA_E_NO_ERROR, return
             PFA_E_NOT_FOUND if it doesn't.
   vers = 0  Set to highest version.
   vers < 0  set version to vers from the highest (-2 will be third highest)
             and return PFA_E_NO_ERROR if enough versions exist,
             return PFA_E_NOT_FOUND if there aren't enough. */
int pfa_set_file_version(Pfa *pfa)
{
 if (INVALID_FILE(pfa))
   return(PFA_E_INVALID_INPUT);

 if (pfa->version == 0  &&  ! pfa_has_component(pfa, PfaVersion))
   return(pfa_set_to_version(pfa, HIGHEST_VERSION, NULL));
 
 if (pfa->version < 0)   /* Set relative version. */
  {
   int vers_count, *vers_arr, ret;

   vers_arr = XAR_BEGIN(int, 5);
   vers_count = pfa_get_versions(pfa, &vers_arr);

   if (vers_count <= -1 * pfa->version)
     ret = PFA_E_NOT_FOUND;
   else
    {
     sort_ints(vers_arr, vers_count);
     pfa->version = vers_arr[pfa->version + vers_count - 1];
     ret = PFA_E_NO_ERROR;
    }
   
   xar_free(&vers_arr);
   return(ret);
  }

 /* Do access check for specific version */
 if (pfa_access(pfa, ACCESS_F_EXIST))
   return(PFA_E_NO_ERROR);  /* OK */
 else if (pfa->version > 1)
   return(PFA_E_NOT_FOUND);

 /* Do special case handling for "1" == "0" */
 pfa->version = 0;
 if (pfa_access(pfa, ACCESS_F_EXIST))
   return(PFA_E_NO_ERROR);
 pfa->version = 1;
 return(PFA_E_NOT_FOUND);
}

/* If the location supports string versions, but the pfa doesn't have one,
   set it to the default version. */
int pfa_set_version_if_missing(Pfa *pfa)
{
 int     vers_flag, loc_vers, status;
 wchar_t string_vers[K_PATH_SIZE];
 
 if (INVALID_FILE(pfa))
   return(PFA_E_INVALID_INPUT);

 vers_flag = pfaloc_call_F(pfa, PfaLocGetVersioning);
 if ( ! (vers_flag & PFALOC_VERS_STRING))
   return(PFA_E_NOT_VALID);
 else if (pfa_has_component(pfa, PfaVersion))
   return(PFA_E_EXIST);

 string_vers[0] = L'\0';
 status = pfaloc_call_FIIPP(pfa, PfaGetFileVersion, HIGHEST_VERSION, FALSE,
                            &loc_vers, string_vers);
 if (status != PFA_E_NO_ERROR)
   return(status);
 else if (loc_vers < 0)
   return(PFA_E_NOT_FOUND);   /* driver is certain no version exists */
 
 if (string_vers[0] != L'\0')
   pfa_put_component_len(pfa, PfaVersion,
                         string_vers, wstrlen(string_vers), FALSE);
 else
   dbg_print_info("pfa_set_version_if_missing", "no string version");

 return(PFA_E_NO_ERROR);
}

/* Old Pro/E code would sometimes use a version of "1" to refer to a file
   with no version, and no version to refer to the highest version.
   If this is the case, update to the true version and return PFA_E_NO_ERROR. */
int pfa_normalize_old_version(Pfa *pfa)
{
 PfaVersSupportType versioning;
 
 if (INVALID_FILE(pfa))
   return PFA_E_INVALID_INPUT;

 if (pfa_is_exact_filename(pfa))
     return PFA_E_CONTINUE;

 versioning = pfaloc_call_F(pfa, PfaLocGetVersioning);
 if (versioning != PFALOC_VERS_INVALID  &&
     (versioning & PFALOC_VERS_NUMERIC) == 0)
   return PFA_E_NOT_ALLOWED;

 if (pfa->version == 1)
  {
   pfa->version = 0;
   if (pfa_access(pfa, ACCESS_READ))
     return PFA_E_NO_ERROR;
   else
    {
     pfa->version = 1;
     return PFA_E_CONTINUE;
    }
  }
 else if (pfa->version == 0)
  {
   if (pfa_set_to_highest_version(pfa, NULL) == PFA_E_NO_ERROR
       &&  pfa_access(pfa, ACCESS_READ))
     return PFA_E_NO_ERROR;
   else
    {
     pfa->version = 0;
     return PFA_E_CONTINUE;
    }
  }
 else
   return PFA_E_CONTINUE;
}

/* Returns the user-visble string version of the file. */
int pfa_get_user_version(Pfa *pfa, int option, wchar_t *out_vers_wstr)
{
 wchar_t string_vers[K_PATH_SIZE];
 int     ret, local_vers;

 if (INVALID_FILE(pfa)  ||  out_vers_wstr == NULL)
   return PFA_E_INVALID_INPUT;
 else if (option != THIS_VERSION  &&  option != NEXT_VERSION  &&
          option != HIGHEST_VERSION)
   return PFA_E_NOT_ALLOWED;

 string_vers[0] = L'\0';
 ret = pfaloc_call_FIIPP(pfa, PfaGetFileVersion, option, TRUE,
                         &local_vers, string_vers);
 wstrcpy(out_vers_wstr, string_vers);
 return ret;
}

int pfa_unset_version(Pfa *pfa)
{
 if (INVALID_FILE(pfa))
   return PFA_E_INVALID_INPUT;

 pfa->version = 0;
 pfa_put_component_len(pfa, PfaVersion, L"", 0, FALSE);
 return PFA_E_NO_ERROR;
}
