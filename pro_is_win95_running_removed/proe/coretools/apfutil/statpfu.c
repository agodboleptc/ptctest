#include <btkcstat.h>
# include <pfuint.h>
#include "debug.h"

# if OPER_SYS == WINDOWS_32
   #include <sys/utime.h>
# endif

/*
    23-AUG-94   E-06-08   Gene   $$1   Initial code.
    26-nov-94   e-06-20   yura   $$2   added file_size to PF_Stat
    22-NOV-94   E-06-20   Maya   $$3   Fixed pfu_add_permission(),
				       added pfu_del_permission(),
				       pfu_get_file_size(),
                                       pfu_get_def_dir_perm(),
                                       pfu_get_def_file_perm();
				       added file_size to PF_Stat structure.
    05-DEC-94   E-06-21   Maya   $$4   Fixed pfu_set_def_perm().
    06-DEC-94   E-06-21   Maya   $$5   NT fix.
    12-DEC-94   E-07-03   Maya   $$6   Added pfu_test_permission().
    06-DEC-94   E-07-04   Maya   $$7   Added pfu_utime().
    30-DEC-94   E-07-05   Maya   $$8   Changed pfu_chmod_ext().
				       Fixed compilation error on apollo.
    13-FEB-95   E-07-07   Maya   $$9   Fixed compilation error on apollo.
    28-MAR-95   E-07-09   Maya   $$10  Add PFU_Perm_Execute.
    14-APR-95   E-07-09   MTP    $$11  NT fixes/additions.
    15-Jun-95   E-07-18   cfk    $$12  fix for windows95 in pfu_utime()
    09-JUN-95   E-07-18   MTP    $$13  VMS compile fixes.
17-Aug-95 G-01-03 jmichaud $$14 Use XAR_COUNT() and BYTCPY() macros.
    30-oct-95   g-01-11   yura   $$15  added pfu_umask()
22-Jan-96 G-03-01 jmichaud $$16 Include header for dbg_xxx_msg() varargs funcs.
    14-OCT-97   H-03-26   Jean   $$17  pfu_utime use unix path for win95
    02-OCT-99   I-03-17   ksi    $$18  Declaration added
    21-Sep-01   J-03-09   jas    $$19  Removed WINDOWS_95 macro
    12-Feb-06  L-01-02 PROTO $$20  Automatic prototype creation
    12-Jul-06  L-01-12 ksi   $$21  Unicode compliant changes
    12-Mar-12  P-20-01 AC    $$22  Updated for Project 13028358
*/

#include <profutil.h>
#include <const.h>
#include <ct_win_syscall_proto.h>
#include <pro_string.h>
#include <windows_32_protos.h>
#include <strtool.h>
#include <xarptr.h>

/*****************************************************************************/
PFU_Status	pfu_stat (path, p_buf)

   PF_Path		path;
   PF_Stat		*p_buf;

/* Return: if successful */

{

# if OPER_SYS == UNIX

   PF_Int_Perm		*p_intp;
   struct stat          ststr;

   OS_Path_Decl (path, FALSE);

   if (btk_stat (os_path, &ststr))
      return (pfu_map_error ());

   if (p_buf != NULL)
      {
         ZERO_OUT_STRUCT (*p_buf);
	 p_buf -> mod_time = ststr.st_mtime;
	 p_buf -> file_size = ststr.st_size;

         p_buf -> is_directory = S_ISDIR (ststr.st_mode);
	 p_buf -> is_link = S_ISLNK (ststr.st_mode);

	 p_intp = (PF_Int_Perm *) p_buf -> permiss;

	 p_intp -> st_mode = ststr.st_mode;

	 if (ststr.st_uid == geteuid ())
	    p_buf -> relation = PFU_User;
	 else if (ststr.st_gid == getegid ())
	    p_buf -> relation = PFU_Group;
	 else
	    p_buf -> relation = PFU_Others;

	 p_buf -> can_execute =
	    (ststr.st_mode & PermMask (__Execute + p_buf -> relation));

	 p_buf -> can_open_for_read =
	    (ststr.st_mode & PermMask (__Read + p_buf -> relation));

	 p_buf -> can_open_for_write =
	    (ststr.st_mode & PermMask (__Write + p_buf -> relation));
      }


   return (PFU_Success);
# endif

# if OPER_SYS == WINDOWS_32

   struct stat          ststr;

   OS_Path_Decl (path, FALSE);

   if (btk_stat (os_path, &ststr))
      return (pfu_map_error ());

   if (p_buf != NULL)
      {
         ZERO_OUT_STRUCT (*p_buf);
	 p_buf -> mod_time = ststr.st_mtime;
	 p_buf -> file_size = ststr.st_size;

         p_buf -> is_directory = S_ISDIR (ststr.st_mode);
	 p_buf -> is_link = S_ISLNK (ststr.st_mode);

	 if (ststr.st_uid == geteuid ())
	    p_buf -> relation = PFU_User;
	 else if (ststr.st_gid == getegid ())
	    p_buf -> relation = PFU_Group;
	 else
	    p_buf -> relation = PFU_Others;

	 p_buf -> can_execute = TRUE;

	 p_buf -> can_open_for_read = TRUE;

	 p_buf -> can_open_for_write = TRUE;
      }


   return (PFU_Success);

# endif

}

/*****************************************************************************/
# define TEST_FUNC_CODE_GENERATOR(func_name, bool_field)		\
           Bool    func_name (path)					\
									\
              PF_Path              path;				\
								        \
           {								\
              PF_Stat              pfststr;				\
								        \
              return (pfu_stat (path, &pfststr) == PFU_Success &&	\
                      pfststr.bool_field);				\
           }

TEST_FUNC_CODE_GENERATOR (pfu_is_dir, is_directory)

TEST_FUNC_CODE_GENERATOR (pfu_is_link, is_link)

TEST_FUNC_CODE_GENERATOR (pfu_is_executable, can_execute)

TEST_FUNC_CODE_GENERATOR (pfu_is_readable, can_open_for_read)

TEST_FUNC_CODE_GENERATOR (pfu_is_writable, can_open_for_write)

/*****************************************************************************/
PFU_Status	pfu_get_file_size (path, p_size)

   PF_Path	path;
   long int	*p_size;

{
   PF_Stat	buf;
   PFU_Status	status;

   if ((status = pfu_stat (path, &buf)) != PFU_Success)
      return (status);

   *p_size = buf.file_size;

   return (PFU_Success);
}

/*****************************************************************************/

# if OPER_SYS == UNIX || OPER_SYS == WINDOWS_32
   static PF_Int_Perm	       	default_dir_permission = { 0755 };
   static PF_Int_Perm           default_file_permission = { 0644 };
# endif

/*****************************************************************************/
void 		pfu_set_def_perm (dir_perm, file_perm)

   PF_Permissions       dir_perm;
   PF_Permissions	file_perm;

{

# if OPER_SYS == UNIX || OPER_SYS == WINDOWS_32
   BYTCPY (&default_dir_permission, dir_perm, sizeof (PF_Int_Perm));
   BYTCPY (&default_file_permission, file_perm, sizeof (PF_Int_Perm));
# endif
}

/*****************************************************************************/
void		pfu_get_def_file_perm (file_perm)

   PF_Permissions	file_perm;

{

# if OPER_SYS == UNIX || OPER_SYS == WINDOWS_32
   BYTCPY (file_perm, &default_file_permission, sizeof (PF_Permissions));
# endif
}

/*****************************************************************************/
void            pfu_get_def_dir_perm (dir_perm)

   PF_Permissions       dir_perm;

{

# if OPER_SYS == UNIX || OPER_SYS == WINDOWS_32
   BYTCPY (dir_perm, &default_dir_permission, sizeof (PF_Permissions));
# endif
}

/*****************************************************************************/
Bool    pfu_umask (mask)
  Bitmask       mask;
{
# if OPER_SYS == UNIX

  return (umask(mask) != mask);

#else

  return (TRUE);

#endif
}

/*****************************************************************************/
PFU_Status	pfu_chmod (path, permiss)

   PF_Path		path;
   PF_Permissions	permiss;

{

# if OPER_SYS == UNIX

   PF_Int_Perm		*p_intp = (PF_Int_Perm *) permiss;

   OS_Path_Decl (path, FALSE);

   if (PFU_chmod (os_path, p_intp -> st_mode))
      return (pfu_map_error ());

   return (PFU_Success);

# else

   return (PFU_Success);

# endif

}

/*****************************************************************************/
PFU_Status 	pfu_chmod_ext (path, func, p_data)

   PF_Path              path;
   Func                 func;
   Pointer              p_data;

{
   PFU_Status           status;
   PF_EntryName		ename;
   PF_LocPath           file_path;
   PF_Permissions       permiss;
   PF_Stat              entry_ststr;
   Bool                 change_perm;
   int 			i_dir, n_dirs;
   XText		dir_list = NULL;

   status = pfu_stat (path, &entry_ststr);
   if (status != PFU_Success)
      return (status);

   pfu_copy_perm (entry_ststr.permiss, permiss);

   (*func) (p_data, path, &entry_ststr, permiss);

   change_perm = !pfu_cmp_perm (entry_ststr.permiss, permiss);

   if (!entry_ststr.is_directory)
      if (change_perm)
         return (pfu_chmod (path, permiss));
      else
         return (PFU_Success);

   status = pfu_add_perm_on_disk (path, PFU_Perm_Read_Dir, PFU_User);
   if (status != PFU_Success && status != PFU_No_Change)
      return (status);

   status = pfu_expand_pattern (path, NULL, &dir_list);
   if (status != PFU_Success)
      return (status);

    n_dirs = XAR_COUNT (&dir_list);
    for (i_dir = 0; i_dir < n_dirs; i_dir++)
       {
          pfu_cat_path (path, dir_list [i_dir], file_path);
          pfu_chmod_ext (file_path, func, p_data);
       }

   XTEXT_FREE (&dir_list);

   if (change_perm)
      status = pfu_chmod (path, permiss);
   else
      status = PFU_Success;

   return (status);
}

/*****************************************************************************/
void	pfu_copy_perm (from, to)

   PF_Permissions	from;
   PF_Permissions	to;

{
   BYTCPY (to, from, sizeof (PF_Permissions));
}

/*****************************************************************************/
Bool    pfu_cmp_perm (perm1, perm2)

   PF_Permissions       perm1;
   PF_Permissions       perm2;

{
# if OPER_SYS == UNIX

   int			mask1 = ((PF_Int_Perm *) perm1) -> st_mode;
   int		        mask2 = ((PF_Int_Perm *) perm2) -> st_mode;

   return ((mask1 & 0777) == (mask2 & 0777));

# else

   return (TRUE);

# endif
}

/*****************************************************************************/
static Bitmask pfu_change_perm_bits (mode, relation)

   PFU_Permission_Type          mode;
   PFU_Relation_Type		relation;

{
# if OPER_SYS == UNIX

   Bitmask                      bits;
   int				int_rel = (int) relation;

   switch (mode)
      {
         case PFU_Perm_Write:
            bits = PermMask (int_rel + __Write);
            break;

         case PFU_Perm_Read:
            bits = PermMask (int_rel + __Read);
            break;

         case PFU_Perm_Execute:
            bits = PermMask (int_rel + __Execute);
            break;

         case PFU_Perm_Delete_From:
         case PFU_Add_Dir_Entry:
            bits = PermMask (int_rel + __Write) |
                   PermMask (int_rel + __Execute);
            break;

         case PFU_Perm_Read_Dir:
	    bits = PermMask (int_rel + __Read) |
		   PermMask (int_rel + __Execute);
            break;

         case PFU_Delete_Self:
            bits = 0;
            break;

         default:
            dbg_crash_msg ("pfu_change_perm_bits", "Unknown permission type");
            break;
      }

   return (bits);

# else

   return (0);

# endif
}

/*****************************************************************************/
Bool	pfu_add_permission (perm, mode, relation)

   PF_Permissions		perm;
   PFU_Permission_Type		mode;
   PFU_Relation_Type 		relation;

{

# if OPER_SYS == UNIX

   Bitmask			bits;
   PF_Int_Perm			*p_intp = (PF_Int_Perm *) perm;


   bits = pfu_change_perm_bits (mode, relation);

   if ((p_intp -> st_mode & bits) == bits)
      return (FALSE);

   p_intp -> st_mode |= bits;
   return (TRUE);

# else

   return (FALSE);

# endif
}

/*****************************************************************************/
Bool    pfu_del_permission (perm, mode, relation)

   PF_Permissions               perm;
   PFU_Permission_Type          mode;
   PFU_Relation_Type		relation;

{
# if OPER_SYS == UNIX

   Bitmask                      bits;
   PF_Int_Perm                  *p_intp = (PF_Int_Perm *) perm;


   bits = pfu_change_perm_bits (mode, relation);

   if (((~(p_intp -> st_mode)) & bits) == bits)
      return (FALSE);

   p_intp -> st_mode &= ~bits;
   return (TRUE);

# else

   return (FALSE);

# endif
}

/*****************************************************************************/
Bool	pfu_test_permission (perm, mode, relation)

   PF_Permissions		perm;
   PFU_Permission_Type          mode;
   PFU_Relation_Type            relation;

{
# if OPER_SYS == UNIX

   Bitmask                      bits;
   int                          int_rel = (int) relation;
   PF_Int_Perm                  *p_intp = (PF_Int_Perm *) perm;

   bits = PermMask ((int) relation + (int) mode);

   if ((p_intp -> st_mode & bits) == bits)
      return (TRUE);
   else
      return (FALSE);

# else

   return (FALSE);

# endif
}

/*****************************************************************************/
PFU_Status pfu_utime (path, mod_time)

   PF_Path                      path;
   long int                     mod_time;

{
   PF_LocPath                   os_path;

# if OPER_SYS == UNIX

#   if PRO_MACHINE == APOLLO

      time_t                    p_times [2];

      p_times [0] = p_times [1] = (time_t) mod_time;

#   else

      struct utimbuf            times_buf;
      struct utimbuf            *p_times = &times_buf;

      times_buf.actime = times_buf.modtime = (time_t) mod_time;

#   endif

   STRCOPY_X (path, os_path);
   pfu_path_to_os (os_path, OPER_SYS, FALSE);

   if (btk_utime (os_path, p_times))
      return (pfu_map_error ());

   return (PFU_Success);

# elif  OPER_SYS == WINDOWS_32

   struct _utimbuf            times_buf;
   struct _utimbuf            *p_times = &times_buf;

   times_buf.actime = times_buf.modtime = (time_t) mod_time;

   STRCOPY_X (path, os_path);

   pfu_path_to_os (os_path, OPER_SYS, FALSE);

   if (btk_utime (os_path, p_times))
      return (pfu_map_error ());

   return (PFU_Success);

# endif
}

/*****************************************************************************/

