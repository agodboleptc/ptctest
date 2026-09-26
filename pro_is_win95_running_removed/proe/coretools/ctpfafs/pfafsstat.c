/*
 pfafsstat.c: status-related methods for PfaStdFilesystem.
 ==============================================================================
 16-Mar-99  I-03-07  aap  $$1 Created from pfastat.c.
 14-JUN-99  I-03-12  KMG  $$2 Win32_pfa_access_work()
 09-Sep-99  I-03-18  aap  $$3 Use FILE_SHARE_READ for performance
 10-Jan-00  I-03-26  aap  $$4 Fix return val of pfafs_make_hidden_low().
 04-Jan-00  J-01-01  aap  $$5 Table display columns functions
 16-Mar-00  J-01-04+ HMR  $$6 Allow execute for dll in Win32_pfa_access_work().
 04-Apr-00  J-01-06  HMR  $$7 Move some functions to dlg_details.c.
 21-Jun-00  J-01-12  aap  $$8 Call seq of pfa_loc_method_register().
 21-Jul-00  J-01-13  aap  $$9 Fix NT heap corruption in pfafs_chown_low().
 26-Sep-00  J-01-19  hplab $$10  better check for local or remote filesystem type
 23-Oct-00  J-01-24  aap $$11 Reduce duplication.
 21-Sep-01  J-03-09  jas $$12 Removed WINDOWS_95 macro
 09-Apr-02  J-03-23  aap $$13 check for pfafs:drives 
 19-Apr-02  J-03-24  aap $$14 check for DRIVE_CDROM
 09-May-02  J-03-25  HMR $$15 Use CheckNTFSAccess for write as well as read.
 09-Aug-02  J-03-32  HMR $$16 Use ntlangwrap funcs.
 03-Dec-02  J-03-39  HMR $$17 Fix bug from ##16 in pfafs_get_file_size_low().
 06-JAN-03  J-03-39  MTP $$18 Linux port
 07-Apr-03  K-01-06  aap $$19 check from #15 applies only to local drives.
 06-May-03  K-01-06  aap $$20 pfa_access no longer trusts statvfs.
 20-MAY-03  K-01-07  MTP $$21 include sys/types.h for HP due to compile line
                              change for include path
 17-Jul-03  K-01-10  HMR $$22 Check for no location in Win32_pfa_access_work()
 17-Sep-03  K-01-15  aap $$23 Extracted pfafs_stat().
 03-Nov-03  K-01-17  HMR $$24 Use temp file for remote UNIX drive write access.
 28-Apr-04  K-03-01  HMR $$25 Added pfadc_[get/set]_nt_dir_writable().
 30-Jun-04  K-03-06  HMR $$26 Threading support.
 02-Sep-04  K-03-09  HMR $$27 Hack around redefinition of ACCESS_READ on Windows
 03-Nov-04  K-03-14  HMR $$28 Obsoleted ft_enums.h.
 10-Feb-06  L-01-05  aap $$29 Use pfalangwrap functions.
 24-Apr-06  L-01-07  TWH $$30 use windows unicode wrappers
 12-Jul-06  L-01-12  ksi $$31 Unicode compliant changes
 02-Aug-06  L-01-13  ksi $$32 More unicode changes
 06-Nov-06  L-01-20  aap $$33 Obsolete PFA_USE_UCNV
 10-NOV-06  L-01-20  KSV $$34 Adapted to interface changes
 27-NOV-06  L-01-21  KSV $$35 Undone ##34
 06-Mar-07  L-01-28  HMR $$36 Fix in unicoding of pfafs_stat(). 
 20-May-07  L-01-33  HMR $$37 Removed obsolete #define.
 22-May-08  L-03-09  KSV $$38 Ground work for SandBox project
 16-May-08  L-03-09  BI  $$39 Used syswindows.h instead of windows.h
 25-Aug-08  L-03-16  KSV $$40 Sandbox for Windows support
 12-Mar-12  P-20-01  AC  $$41 Updated for Project 13028358
 08-Jul-13  P-20-34  yura/ibl   $$42 Added pfafs_set_file_mtime ().
 27-Jun-14  P-20-56  jas $$43 Added support for apple_osx
 19-May-15  P-30-10  aap $$44 Win32_can_write_tmp_file vs loc_type
 02-Jul-15  P-30-12  HMR $$45 Switched PFA from char to wchar_t.
 14-Jul-15  P-30-12  HMR $$46 Pass widestring to pfalangwrap_fopen().
 29-Jul-15  P-30-13  HMR $$47 More wchar_t in Win32_pfa_access_work().
 21-Aug-15  P-30-15  YGK $$48 Fixes for THA.
 31-Mar-16  P-30-30  HMR $$49 Use wchar_t more, char less.
 15-Nov-18  P-60-25  HMR $$50 Update for Int64T file sizes
 13-Jul-20  P-80-12  KSV $$51 Removed sandbox leftovers
 09-Mar-26  Q-27-00  PROTO $$52 Automatic prototype creation
 ==============================================================================
*/

#include <btkcio.h>
#include <btkcstat.h>
#include <btkcstdio.h>
#include <pfa_internal.h>
#include <syserrno.h>
#include <sysstat.h>
#include <sysstatfs.h>
#include <sysstdlib.h>
#include <systime.h>
#include <dbg_crash.h>
#include <pro_string.h>
#include <ptime.h>
#include <proprintf.h>
#include <pro_sysinfo.h>
#include <pro_wstring.h>
#include <pfastdfs.h>
#include <thrctsd.h>
#include <cttime_proto.h>
#include <mkscpy.h>
#include <ctfileutil_proto.h>
#include <ct_win_syscall_proto.h>
#include <ctpfafs_proto.h>

#if OPER_SYS == UNIX_OS
#include <syspwd.h>
#include <sysunistd.h>
#endif

#if OPER_SYS == WINDOWS_32
#include <ptc_win32.h>
#include <syswindows.h>
#include <lm.h>
#include <sys/utime.h>
#undef ACCESSDOTH   /* lm.h changed ACCESS_READ to 1; change it back to 4 */
#include <access.h>

#define TOKEN_ALL_ACCESS_PTC (STANDARD_RIGHTS_REQUIRED  |\
                          TOKEN_ASSIGN_PRIMARY      |\
                          TOKEN_DUPLICATE           |\
                          TOKEN_IMPERSONATE         |\
                          TOKEN_QUERY               |\
                          TOKEN_QUERY_SOURCE        |\
                          TOKEN_ADJUST_PRIVILEGES   |\
                          TOKEN_ADJUST_GROUPS       |\
                          TOKEN_ADJUST_DEFAULT)

static SID_IDENTIFIER_AUTHORITY authority = SECURITY_WORLD_SID_AUTHORITY;
#endif /* Win32 */

#if OPER_SYS == UNIX_OS

/******************************************************************************/
static Bool can_write_in_dir_slow_(char *dir_name, Pfa* in_pfa)

{
 pfaStat64 buff;
 char sticky_name[CHAR_PATH_SIZE];
 wchar_t sticky_wname[K_PATH_SIZE];
 FILE *sticky_fp;
 
 if (pfafs_stat(dir_name, &buff) == -1)
   return (FALSE);

 (void) strcpy(sticky_name, dir_name);
 (void) strcat(sticky_name, ".pfa_can_write.acc");
 strtows(sticky_wname, sticky_name);
 sticky_fp = pfalangwrap_fopen(sticky_wname, L"w", in_pfa);
 if (sticky_fp == (FILE *) NULL)
   return (FALSE);

 btk_fclose(sticky_fp);
 btk_unlink(sticky_name);
 return (TRUE);

#if 0
 else
  {
   /*
    * Try to modify the last-modified date of the directory.  if
    * we succeed, we conclude that the directory has write access.
    */
   struct timeval tvp[2];
   struct timeval tvp_test[2];
   Bool successfully_set_time = FALSE;

   tvp[0].tv_sec = buff.st_atime;
   tvp[1].tv_sec = buff.st_mtime;
   tvp[0].tv_usec = 0L;
   tvp[1].tv_usec = 0L;

   tvp_test[0].tv_sec = buff.st_atime;
   tvp_test[1].tv_sec = buff.st_mtime - 1;
   tvp_test[0].tv_usec = 0L;
   tvp_test[1].tv_usec = 0L;

   successfully_set_time = (utimes(dir_name, tvp_test) == 0);

   /* Put the old times back! */
   (void) utimes(dir_name, tvp);

    return (successfully_set_time);
  }
#endif
}
#endif

#if OPER_SYS == WINDOWS_32
/******************************************************************************/
static Bool can_tweak_pfa_dir_time(char *dir_name)

{
 /* On Windows, we cannot set the time without opening the directory
  * with "Write" permission, but if we can do that, we have no need
  * to do anything else.
  */
 Bool successfully_set_time = FALSE;
 HANDLE handle;
 DWORD access = GENERIC_WRITE;
 DWORD mode = OPEN_EXISTING;
 DWORD attr = FILE_FLAG_BACKUP_SEMANTICS;
 DWORD share = FILE_SHARE_READ;
 wchar_t wbuff[K_PATH_SIZE];

 strtows(wbuff, dir_name);
 handle = ntlangwrap_CreateFile(wbuff, access, share, mode, attr);
 successfully_set_time = (handle != INVALID_HANDLE_VALUE);
 if (successfully_set_time)
   CloseHandle(handle);

 return (successfully_set_time);
}
#endif


#if OPER_SYS == WINDOWS_32
/* Pretty much verbatum the Win32 parts of pfa_bruteforce_write_access()
   from R19.  We only need it for Win32, so no need to compile it for Unix... */
static Bool Win32_can_write_tmp_file(const wchar_t *dir_wname,
                                     PfaLocationType loc_type)
{
 struct _utimbuf  utb;
 pfaStat64        buff;
 char             cbuff[CHAR_PATH_SIZE];
 wchar_t         *ptr;
 int              cached_writable;
 Pfa             *tmp;
 thrDeclareStaticInTSDAndLocalTSDValue(Pfa *, static_pfa, NULL);
 
 /* _stat() likes c:\ and c:\foo, but not c: or c:\foo\ */
 ptr = wstrrchr(dir_wname, DIR_DELIMITER);
 if ( ptr && (*(ptr+1)==NULL_WCHAR) && (*(ptr-1) != DEVICE_SEPARATOR) )
    *ptr = NULL_WCHAR;
 else
    ptr = NULL;

 wstrtos_buff(dir_wname, cbuff, CHAR_PATH_SIZE);
 if (pfafs_stat(cbuff, &buff) == -1)
    return(FALSE);

 if (ptr)
    *ptr = DIR_DELIMITER;   /* put it back for pfadc_?et_nt_dir_writable() */

 cached_writable = pfadc_get_nt_dir_writable(dir_wname);
 if (cached_writable != K_NOT_USED)
    return(cached_writable);
 
 utb.actime = buff.st_atime;
 utb.modtime = buff.st_mtime;

 pfa_recycle_pro_file(p_static_pfa);
 tmp = *p_static_pfa;
 (void)pfa_put_wlocation(tmp, dir_wname, loc_type);
 (void)pfa_put_wname(tmp, L"write");
 (void)pfa_put_wextension(tmp, L"acc");
 if (pfa_access(tmp, ACCESS_F_EXIST))
    (void)pfa_put_version(tmp, 937);    /* if write.acc.937 exists, error */
 if (pfa_fopen(tmp, THIS_VERSION, "w") == PFA_E_NO_ERROR)
 {
    char cbuff[CHAR_PATH_SIZE];
    
    (void)pfa_close(tmp);
    (void)pfa_delete_file(tmp, THIS_VERSION);
    wstrtos(cbuff, dir_wname);
    (void)btk_utime(cbuff, &utb);
    pfadc_set_nt_dir_writable(dir_wname, TRUE);
    return(TRUE);
 }
 else
 {
    pfadc_set_nt_dir_writable(dir_wname, FALSE);
    return(FALSE);
 }
}
#endif


#if OPER_SYS == WINDOWS_32

static int CheckNTFSAccess(char *path, DWORD dwDesiredAccess)
{
   DWORD                dwGrantedAccess;
   DWORD                dwPrivilegeLength;
   BOOL                 fStatus;
   GENERIC_MAPPING      gmMapping;
   HANDLE               hThread = 0;
   HANDLE               hThreadToken = 0;
   DWORD                lasterror;
   PRIVILEGE_SET        psPrivilege;
   Bool                 ret = FALSE;
   PSECURITY_DESCRIPTOR psdSD=(PSECURITY_DESCRIPTOR)NULL;
   SECURITY_INFORMATION info;
   wchar_t              wpath[K_PATH_SIZE];


   info = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
          DACL_SECURITY_INFORMATION;

   strtows(wpath, path);
   if ( ! ntlangwrap_GetFileSecurity(wpath, info, &psdSD))
   {
      lasterror = GetLastError();
      switch (lasterror)
      {
         case ERROR_NO_SECURITY_ON_OBJECT:
         case ERROR_NOT_SUPPORTED:
            /* No security/non-NTFS file. We should have access to write... */
            relmem(&psdSD);
            return (TRUE);

         case ERROR_FILE_NOT_FOUND:
         case ERROR_PATH_NOT_FOUND:
         default:
            relmem(&psdSD);
            return (FALSE);
      }
   }

   if (!ImpersonateSelf(SecurityImpersonation))
   {
      relmem(&psdSD);
      return (FALSE);
   }

   hThread = GetCurrentThread();

   if (!hThread ||
       !OpenThreadToken(hThread, TOKEN_ALL_ACCESS_PTC, FALSE, &hThreadToken))
   {
      RevertToSelf();
      relmem(&psdSD);
      return (FALSE);
   }

   dwPrivilegeLength = sizeof(psPrivilege);

   if (AccessCheck(psdSD,               /* file security descriptor */
                   hThreadToken,        /* thread access token */
                   dwDesiredAccess,     /* desired access rights */
                   &gmMapping,          /* general mapping structure */
                   &psPrivilege,        /* privilege set */
                   &dwPrivilegeLength,  /* privilege set length */
                   &dwGrantedAccess,    /* accesses granted */
                   &fStatus))           /* requested access allowed flag */
   {
      /* Since we're only interested in Read or Write access, we don't really
         need to look really hard at fStatus */
      ret = fStatus;
   }

   RevertToSelf();
   relmem(&psdSD);
   CloseHandle(hThread);
   CloseHandle(hThreadToken);

   return (ret);
}

#endif

    /* ===================================================================== */

#if OPER_SYS == WINDOWS_32
static Bool pfafs_has_win32_exec_ext(Pfa *pfa)
{
   /* On Windows, files can be executed if having the following ext. */
   if ( pfa_component_u_wstrcmp(pfa, PfaExtension, L"BAT", 3) == 0 ||
        pfa_component_u_wstrcmp(pfa, PfaExtension, L"COM", 3) == 0 ||
        pfa_component_u_wstrcmp(pfa, PfaExtension, L"EXE", 3) == 0 ||
        pfa_component_u_wstrcmp(pfa, PfaExtension, L"DLL", 3) == 0    )
      return(TRUE);
   return(FALSE);
}

static Bool Win32_pfa_access_work(Pfa *pfa, int mode, Bool ignore_host, PfaLocationType loc_type)
{
   wchar_t        full_wname[K_PATH_SIZE];
   char           full_name[CHAR_PATH_SIZE];
   int            result;
   DWORD          attr;
   Bool           readonly_flag_set;
   Bool           is_dir;
   char           dir_name[CHAR_PATH_SIZE];
   wchar_t        dir_wname[K_PATH_SIZE];
   DWORD          drive_type;
   char           letter;

   if (!pfa_component_wstrcmp(pfa, PfaStdDevice, L"/", 1))
     /* pfafs:drives won't work with GetFileAttributes */
     return (mode == ACCESS_F_EXIST || mode == ACCESS_READ);
   
   if (ignore_host)
     result = pfa_get_file_wname(pfa, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                                 full_wname);
   else
     result = pfa_get_full_wname(pfa, full_wname);

   if (result != PFA_E_NO_ERROR)
     return(FALSE);

   wstrtos_buff(full_wname, full_name, CHAR_PATH_SIZE);

   if( (attr = ntlangwrap_GetFileAttributes(full_wname)) == -1 )
      return(FALSE);
   
   if(mode == ACCESS_F_EXIST)
      return(TRUE);

   if(mode == ACCESS_EXECUTE)
      return pfafs_has_win32_exec_ext(pfa);

   if (mode == ACCESS_READ)
   {
      /* it would be polite to check the security attributes of the
         file to see whether we can read. However, (a) the API doesn't
         exist on win95, and (b) the API can give the wrong answer for
         remote drives. For those two cases we just assume reading is OK. */
      if (!ignore_host) /* UNC path... call it remote */
         return TRUE;
      if (pfa_path_is_absolute(pfa))
        letter = full_name[0];
      else
      {
         char drive[K_NAME_SIZE];
         Pfa *cwd = pfa_get_cur_dir();
         if (!pfa_is_location(cwd, PfaStdFilesystem) ||
             pfa_get_device(pfa_get_cur_dir(), drive) != PFA_E_NO_ERROR)
            return TRUE;
         letter = drive[0];
      }
      if (!isalpha(letter) || nt_is_remote_drive(letter))
         return TRUE;
      return(CheckNTFSAccess(full_name, FILE_GENERIC_READ) != FALSE);
   }
   
   if(mode != ACCESS_WRITE)
   {
      dbg_print_info("Win32_pfa_access_work", "bad mode %d", mode);
      return(FALSE);
   }

   /* Must be ACCESS_WRITE, which can be treacherous. */
   readonly_flag_set = (attr & FILE_ATTRIBUTE_READONLY);
   is_dir = (pfa_get_obj_type(pfa) == T_DIR);
         
   /* For files, we care about the read-only flag */
   if (!is_dir && readonly_flag_set)
      return (FALSE);

   pfa_get_file_wname(pfa, ignore_host ? PFA_DEVICE_PATH :
                      PFA_HOST_DEVICE_PATH, dir_wname);
   if (dir_wname[0] == L'\0')
     pfa_get_wlocation(pfa_get_cur_dir(), dir_wname, NULL);


   drive_type = ntlangwrap_GetDriveType(dir_wname);

   if (drive_type == DRIVE_FIXED)
   {
      /* for local disk, we know of only one reason why you couldn't
         write in a directory, namely security. We check security
         directly rather than writing a temp file in hopes that it's
         faster. */
      return (CheckNTFSAccess(full_name, FILE_GENERIC_WRITE) != FALSE);
   }

   wstrtos_buff(dir_wname, dir_name, CHAR_PATH_SIZE);
   if (   !nt_is_samba_drive(dir_name[0])
          /* can_tweak_pfa_dir_time() will succeed for read-only dirs
             on Samba! do it the hard way there. unfortunately the above
             check is not adequate for all versions of Samba. */
          && can_tweak_pfa_dir_time(dir_name))
      return(TRUE);

   /* Sometimes tweak fails even when we have write access...  Still
      dunno why...  Try the expensive way... */
   return (Win32_can_write_tmp_file(dir_wname, loc_type));
}

static Bool Win32_pfa_access(Pfa *pfa, int mode)
{
 return (Win32_pfa_access_work(pfa, mode, TRUE, PfaStdFilesystem));
}
#endif

#if OPER_SYS == WINDOWS_32
static Bool PfaUNC_access(Pfa *pfa, int mode)
{
   char    fullpath[CHAR_PATH_SIZE];
   wchar_t wloc[K_PATH_SIZE];

   if (pfa_has_component(pfa, PfaStdDevice))
     return (Win32_pfa_access_work(pfa, mode, FALSE, PfaUNC));
   /* At best, we have just a host. */
   if (mode != ACCESS_F_EXIST && mode != ACCESS_READ)
      return FALSE;

   pfa_get_wlocation(pfa, wloc, (PfaLocationType*) NULL);
   wstrtos_buff(wloc, fullpath, CHAR_PATH_SIZE);
   return nt_is_valid_host(fullpath);
}
#endif

#if OPER_SYS == UNIX_OS
static Bool Unix_pfa_access(Pfa *pfa, int mode, Bool conv_to_lower)
{
   char           full_name[K_PATH_SIZE];
   int            result;
   Bool           retval;
   pfaStat64      buff;

   result = pfa_get_file_name_mixed_case(pfa, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                                         full_name, conv_to_lower);
   if (result != PFA_E_NO_ERROR)
      return(FALSE);

   /*
    * A Read-Only mount would return TRUE for a directory which ostensibly
    * has write-access, but would not allow any new files to be opened.
    *
    * Even statvfs won't help if the mount is rw but the server exported
    * the drive as read-only.
    */
   if (mode == ACCESS_WRITE && pfa_is_dir(pfa) && pfa_is_local(pfa)
       && pfa_is_remote_mounted(pfa))
   {
      return(can_write_in_dir_slow_(full_name,pfa));
   }

   if (mode == ACCESS_F_EXIST)
      return(pfafs_stat(full_name, &buff) != -1);

   retval = (pfalangwrap_access(full_name, mode) == 0);

   return(retval);
}
#endif

int pfafs_stat( char *full_name, pfaStat64 *buff )
{
#if OPER_SYS == WINDOWS_32
 /* Windows chokes on "C:\users\" */
 char loc_buff[CHAR_PATH_SIZE];
 if (full_name != NULL)
  {
   strcpy(loc_buff, full_name);
   remove_dir_delim(loc_buff, TRUE);
   full_name = loc_buff;
  }
 return btk_stat64(full_name, buff);
#else
 return btk_stat(full_name, buff);
#endif
}

static int pfafs_wstat(const wchar_t *full_wname, pfaStat64 *buff )
{
 char cbuff[CHAR_PATH_SIZE];

 wstrtos_buff(full_wname, cbuff, CHAR_PATH_SIZE);
 return(pfafs_stat(cbuff, buff));
}

/* For gtm and other old code which doesn't use stat64. */
int pfafs_stat32(char *full_name, struct stat *buff)
{
#if OPER_SYS == WINDOWS_32
 /* Windows chokes on "C:\users\" */
 char loc_buff[CHAR_PATH_SIZE];
 if (full_name != NULL)
  {
   strcpy(loc_buff, full_name);
   remove_dir_delim(loc_buff, TRUE);
   full_name = loc_buff;
  }
#endif
 return btk_stat(full_name, buff);
}

    /* ===================================================================== */

static int pfafs_get_file_time_low(wchar_t *full_wname, Ptime *last_access,
                                   Ptime *last_modified, Ptime *last_status)
{
 pfaStat64 buff;
 if (pfafs_wstat(full_wname, &buff) == -1)
   return (PFA_E_ERROR);

 proe_gmtime(last_access, buff.st_atime);
 proe_gmtime(last_modified, buff.st_mtime);
 proe_gmtime(last_status, buff.st_ctime);

 return (PFA_E_NO_ERROR);
}

static int pfafs_get_file_time(Pfa *file, Ptime *last_access,
                               Ptime *last_modified, Ptime *last_status)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                             full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_get_file_time_low(full_wname, last_access, last_modified,
                                last_status);
}

static int PfaUNC_get_file_time(Pfa *file, Ptime *last_access,
                                Ptime *last_modified, Ptime *last_status)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_full_wname(file, full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_get_file_time_low(full_wname, last_access, last_modified,
                                last_status);
}

static int pfafs_utime(char *path, time_t t)
{
#if OPER_SYS == WINDOWS_32
  struct _utimbuf tb;
#else
  struct utimbuf tb;
#endif
  int error;
  
  tb.actime = tb.modtime = t;
  error = btk_utime(path, t == K_NOT_USED ? NULL : &tb);
  
  /* is there a better way to handle errors that are set in errno? */
  return error == -1 ? PFA_E_ERROR : PFA_E_NO_ERROR;
}

static int pfafs_set_file_mtime(Pfa *file, Ptime *last_modified)
{
 wchar_t full_wname[K_PATH_SIZE];
 char    full_name[CHAR_PATH_SIZE];
 int     result;

 result = pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                             full_wname);

 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 wstrtos_buff(full_wname, full_name, CHAR_PATH_SIZE);
 return pfafs_utime(full_name, ptime_to_time_t_(last_modified));
}

    /* ===================================================================== */
/*ARGSUSED*/
static int pfafs_get_mode_low(Pfa *file, wchar_t *full_wname,
                              unsigned long *mode)
{
 pfaStat64 buff;
#if OPER_SYS == WINDOWS_32
 int i;
 DWORD attr;

 i = wstrlen(full_wname);
 if (i >= 2 && full_wname[i-1] == L'\\' && full_wname[i-2] != L':')
   full_wname[i-1] = L'\0';

 if ((attr = ntlangwrap_GetFileAttributes(full_wname)) == -1)
   return (PFA_E_NO_ACCESS);
 *mode = 0444;                 /* set read access */
 if (attr & FILE_ATTRIBUTE_DIRECTORY)
   *mode |= S_IFDIR;
 if (!(attr & FILE_ATTRIBUTE_READONLY))    /* file is NOT readonly */
   *mode |= 0222;            /* write access */

 if (pfafs_has_win32_exec_ext(file))
   *mode |= 0111;

 if (!(attr & FILE_ATTRIBUTE_DIRECTORY) && !(attr & FILE_ATTRIBUTE_SYSTEM))
   /* it's more or less a regular file */
   *mode |= S_IFREG;
#else

 if (pfafs_wstat(full_wname, &buff) == -1)
   return (PFA_E_NO_ACCESS);

 *mode = (unsigned long) buff.st_mode;
#endif

 return (PFA_E_NO_ERROR);
}

static int pfafs_get_mode(Pfa *file, unsigned long *mode)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                             full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_get_mode_low(file, full_wname, mode);
}

static int PfaUNC_get_mode(Pfa *file, unsigned long *mode)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_full_wname(file, full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_get_mode_low(file, full_wname, mode);
}

    /* ===================================================================== */

static int pfafs_get_file_size_low(wchar_t *full_wname, uInt64T *size)
{
 pfaStat64 buff;

 if (pfafs_wstat(full_wname, &buff) == -1)
   return (PFA_E_NO_ACCESS);

 *size = (uInt64T)buff.st_size;

 return (PFA_E_NO_ERROR);
}

static int pfafs_get_file_size(Pfa *file, uInt64T *size)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                             full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);
 return pfafs_get_file_size_low(full_wname, size);
}

static int PfaUNC_get_file_size(Pfa *file, uInt64T *size)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_full_wname(file, full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);
 return pfafs_get_file_size_low(full_wname, size);
}

    /* ===================================================================== */

static int pfafs_get_owners_low(char *full_name, char *user_id_str,
                                char *group_id_str, char *owner_name)
{
#if OPER_SYS == UNIX_OS
 pfaStat64 buff;
 struct passwd *results;
#endif
#if OPER_SYS == WINDOWS_32
 char domain_name1[K_NAME_SIZE], domain_name2[K_NAME_SIZE];
 char user_name[K_NAME_SIZE];
 wchar_t full_wname[K_PATH_SIZE];
 DWORD lpcch_account1 = K_NAME_SIZE, lpcch_domain1 = K_NAME_SIZE,
       lpcch_account2 = K_NAME_SIZE, lpcch_domain2 = K_NAME_SIZE;
 PSECURITY_DESCRIPTOR psd;
 SECURITY_INFORMATION sec_info;
 PSID lp_psid_owner, lp_psid_group;
 BOOL lpf_owner_defaulted;
 SID_NAME_USE psnu1, psnu2;

 sec_info = (OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION);
 strtows(full_wname, full_name);
 if (ntlangwrap_GetFileSecurity(full_wname, sec_info, &psd) != TRUE)
  {
   dbg_print_info("pfa_get_owners", "GetFileSecurity failed: %d",
                  GetLastError());
   return (PFA_E_ERROR);
  }

 if (group_id_str != NULL)
  {
   if (GetSecurityDescriptorGroup((PSECURITY_DESCRIPTOR) psd, &lp_psid_group,
                                  &lpf_owner_defaulted) != TRUE)
    {
     dbg_print_info("pfa_get_owners", "GetSecurityDescriptorGroup failed: %d",
                    GetLastError());
     return (PFA_E_ERROR);
    }
   if (uLookupAccountSid(NULL, lp_psid_group, group_id_str, &lpcch_account1,
       domain_name1, &lpcch_domain1, (PSID_NAME_USE) & psnu1) !=
       TRUE)
    {
     dbg_print_info("pfa_get_owners", "LookupAccountSid for group failed: %d",
                    GetLastError());
     return (PFA_E_ERROR);
    }
  }

 if (user_id_str != NULL || owner_name != NULL)
  {
   if (GetSecurityDescriptorOwner((PSECURITY_DESCRIPTOR) psd, &lp_psid_owner,
                                  &lpf_owner_defaulted) != TRUE)
    {
     dbg_print_info("pfa_get_owners", "GetSecurityDescriptorOwner failed: %d",
                    GetLastError());
     return (PFA_E_ERROR);
    }

   if (uLookupAccountSid(NULL, lp_psid_owner, user_name, &lpcch_account2,
       domain_name2, &lpcch_domain2, (PSID_NAME_USE) & psnu2) !=
       TRUE)
    {
     dbg_print_info("pfa_get_owners", "LookupAccountSid for owner failed: %d",
                    GetLastError());
     return (PFA_E_ERROR);
    }

   if (user_id_str != NULL)
     (void) strcpy(user_id_str, user_name);

   if (owner_name != NULL)
     (void) strcpy(owner_name, user_name);
  }
#endif
#if OPER_SYS == UNIX_OS

 if (pfafs_stat(full_name, &buff) == -1)
   return (PFA_E_NO_ACCESS);

 if (user_id_str != NULL)
   (void) btk_sprintf(user_id_str, "%d", (int) buff.st_uid);

 if (group_id_str != NULL)
   (void) btk_sprintf(group_id_str, "%d", (int) buff.st_gid);

 if (owner_name != NULL)
  {
   if ((results = getpwuid(buff.st_uid)) == NULL)
     return (PFA_E_ERROR);

   if (btkUnicodeIsEnabled())
     btkStrNativeToUtf(owner_name, K_NAME_SIZE, NULL,
                       results->pw_name, (size_t)-1);
   else
     (void) strcpy(owner_name, results->pw_name);
  }
#endif
 return (PFA_E_NO_ERROR);

}

static int pfafs_get_owners(Pfa *file, char *user_id_str, char *group_id_str,
                            char *owner_name)
{
 char full_name[K_PATH_SIZE];
 int result;

 result = pfa_get_file_name(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                            full_name);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_get_owners_low(full_name, user_id_str, group_id_str, owner_name);
}

static int PfaUNC_get_owners(Pfa *file, char *user_id_str, char *group_id_str,
                             char *owner_name)
{
 char full_name[K_PATH_SIZE];
 int result;

 result = pfa_get_full_name(file, full_name);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_get_owners_low(full_name, user_id_str, group_id_str, owner_name);
}

    /* ===================================================================== */

static int pfafs_chmod_low(const wchar_t *full_wname, int mode)
{
#if OPER_SYS == UNIX_OS
 char cbuff[CHAR_PATH_SIZE];

 wstrtos_buff(full_wname, cbuff, CHAR_PATH_SIZE);
 return ((btk_chmod(cbuff, mode) == -1) ? PFA_E_ERROR : PFA_E_NO_ERROR);
#endif

#if OPER_SYS == WINDOWS_32
 Bool status;

 /* file will be set to read only when the write permission is off. */
 if (!(mode & 0222))
   status = ntlangwrap_SetFileAttributes(full_wname, FILE_ATTRIBUTE_READONLY);
 else
   status = ntlangwrap_SetFileAttributes(full_wname, FILE_ATTRIBUTE_NORMAL);
 if (status == TRUE)
   return(PFA_E_NO_ERROR);
 else
   return(PFA_E_ERROR);
#endif
}

static int pfafs_chmod(Pfa *file, int mode)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                             full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_chmod_low(full_wname, mode);
}

#if OPER_SYS == WINDOWS_32
static int PfaUNC_chmod(Pfa *file, int mode)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_full_wname(file, full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_chmod_low(full_wname, mode);
}
#endif

    /* ===================================================================== */

static int pfafs_chown_low(wchar_t *full_wname, char *user_id_str,
                           char *group_id_str)
{
 int user_id, group_id;
 char full_cname[CHAR_PATH_SIZE];
#if OPER_SYS == WINDOWS_32
 char user_domain_name[K_NAME_SIZE], group_domain_name[K_NAME_SIZE];
 DWORD owner_sid_size, group_sid_size, user_domain_size, group_domain_size;
 SECURITY_DESCRIPTOR sd;
 SECURITY_INFORMATION info;
 char owner_sid_buf[K_PATH_SIZE], group_sid_buf[K_PATH_SIZE];
 PSID owner_psid = (PSID) owner_sid_buf, group_psid = (PSID) group_sid_buf;
 SID_NAME_USE psnu;
#endif

 user_id = atoi(user_id_str);
 group_id = atoi(group_id_str);

#if OPER_SYS == WINDOWS_32
 if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) != TRUE)
  {
   dbg_print_info("pfa_chown", "InitializeSecurityDescriptor failed: %d",
                  GetLastError());
   return (PFA_E_ERROR);
  }

 owner_sid_size = K_PATH_SIZE;
 user_domain_size = K_NAME_SIZE;
 if (uLookupAccountName(NULL, user_id_str, owner_psid, &owner_sid_size,
     user_domain_name, &user_domain_size, (PSID_NAME_USE) & psnu) != TRUE)
  {
   dbg_print_info("pfa_chown", "LookupAccountName failed: %d", GetLastError());
   return (PFA_E_ERROR);
  }

 group_sid_size = K_PATH_SIZE;
 group_domain_size = K_NAME_SIZE;
 if (uLookupAccountName(NULL, group_id_str, group_psid, &group_sid_size,
     group_domain_name, &group_domain_size, (PSID_NAME_USE) & psnu) != TRUE)
  {
   dbg_print_info("pfa_chown", "LookupAccountName failed: %d", GetLastError());
   return (PFA_E_ERROR);
  }

 if (SetSecurityDescriptorOwner(&sd, owner_psid, FALSE) != TRUE)
  {
   dbg_print_info("pfa_chown", "SetSecurityDescriptorOwner failed: %d",
                  GetLastError());
   return (PFA_E_ERROR);
  }

 if (SetSecurityDescriptorGroup(&sd, group_psid, FALSE) != TRUE)
  {
   dbg_print_info("pfa_chown", "SetSecurityDescriptorGroup failed: %d",
                  GetLastError());
   return (PFA_E_ERROR);
  }

 info = (OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION);
 if (ntlangwrap_SetFileSecurity(full_wname, info, &sd) != TRUE)
  {
   dbg_print_info("pfa_chown", "SetFileSecurity failed: %d", GetLastError());
   return (PFA_E_ERROR);
  }
 return (PFA_E_NO_ERROR);
#endif

#if OPER_SYS == UNIX_OS
 wstrtos_buff(full_wname, full_cname, CHAR_PATH_SIZE);
 return ((btk_chown(full_cname, (uid_t) user_id, (gid_t) group_id) == -1) ?
         PFA_E_ERROR : PFA_E_NO_ERROR);
#endif
}

static int pfafs_chown(Pfa *file, char *user_id_str, char *group_id_str)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                             full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_chown_low(full_wname, user_id_str, group_id_str);
}

static int PfaUNC_chown(Pfa *file, char *user_id_str, char *group_id_str)
{
 wchar_t full_wname[K_PATH_SIZE];
 int     result;

 result = pfa_get_full_wname(file, full_wname);
 if (result != PFA_E_NO_ERROR)
   return (PFA_E_NOT_VALID);

 return pfafs_chown_low(full_wname, user_id_str, group_id_str);
}

    /* ===================================================================== */

int pfafs_get_avail_blocks(Pfa *file, unsigned long *out_blocks,
                           unsigned long *bytes_per_block)
{
   /* This function is straight out of pfastatfs.c. What a mess. Do we still
      support all these platforms? 17-Mar-99 aap
   */

#if OPER_SYS == UNIX_OS
 wchar_t wpath[K_PATH_SIZE];
 char path_buf[CHAR_PATH_SIZE];
 char *path = path_buf;
 int ierror;
 pfaStat64 dumbuf;
# if PRO_MACHINE == HP700
# include <sys/types.h>
# include <sys/statvfs.h>
 struct statvfs vfsbuf;
# endif
# if PRO_MACHINE == X86E_LINUX64 || PRO_MACHINE == APPLE_OSX
 struct statvfs vfsbuf;
# endif
# if PRO_MACHINE == SUN4 || PRO_MACHINE == IBM_RIOS || PRO_MACHINE == LINUX
#  ifdef SOLARIS
 struct statvfs vfsbuf;
#  else
 struct statfs buf;
#  endif
# endif
#endif
#if OPER_SYS == WINDOWS_32
 DWORD sectors_per_cluster, bytes_per_sector, free_clusters, dummy;
 char target_device[K_NAME_SIZE];
#endif


 if (!pfa_is_local(file))
   return (PFA_E_NOT_ALLOWED);

#if OPER_SYS == UNIX_OS
 if (pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION, wpath) !=
     PFA_E_NO_ERROR)
   return (PFA_E_INVALID_INPUT);

 if (pfafs_wstat(wpath, &dumbuf) == -1)         /* Input pfa does not exist	*/
  {
   Pfa *parent;
   
   if (pfa_alloc_pro_file(&parent) != PFA_E_NO_ERROR)
     return(PFA_E_NO_SPACE);
   
   for (ierror = pfa_make_parent(file, parent); ierror == PFA_E_NO_ERROR;
        ierror = pfa_make_parent(parent, parent))
    {
     if (pfa_get_file_wname(parent, PFA_DEVICE_PATH, wpath) != PFA_E_NO_ERROR)
      {
       ierror = PFA_E_INVALID_INPUT;
       break;
      }

     if (pfafs_wstat(wpath, &dumbuf) != -1)
      {
       ierror = PFA_E_NO_ERROR;
       break;
      }
    }
   pfa_free_pro_file(&parent); 
   if (ierror != PFA_E_NO_ERROR)
     return (PFA_E_INVALID_INPUT);
  }
 
# if PRO_MACHINE == SUN4 || PRO_MACHINE == HP700 || PRO_MACHINE == IBM_RIOS || PRO_MACHINE == LINUX || PRO_MACHINE == X86E_LINUX64 || PRO_MACHINE == APPLE_OSX

#  undef USE_STATVFS

#  ifdef SOLARIS
#   define USE_STATVFS
#  endif
#  if PRO_MACHINE == HP700 || PRO_MACHINE == X86E_LINUX64 || PRO_MACHINE == APPLE_OSX
#   define USE_STATVFS
#  endif

 wstrtos_buff(wpath, path, CHAR_PATH_SIZE);
#  ifdef USE_STATVFS
 if (btk_statvfs(path, &vfsbuf) == -1)
   return (PFA_E_ERROR);

 *out_blocks = (unsigned long) vfsbuf.f_bavail;
 *bytes_per_block = (unsigned long) vfsbuf.f_frsize;

#  else
 if (btk_stati64(path, &buf) == -1)
   return ((errno == EACCES) ? PFA_E_NO_ACCESS : PFA_E_ERROR);

 *out_blocks = (unsigned long) buf.f_bavail;
 *bytes_per_block = (unsigned long) buf.f_bsize;

#  endif	/* USE_STATVFS	*/
#  undef USE_STATVFS
# endif	/* SUN4, HP700, IBM_RIOS */

#endif	/* UNIX_OS	*/
#if OPER_SYS == WINDOWS_32
 if (pfa_get_device(file, target_device) != PFA_E_NO_ERROR ||
     target_device[0] == '\0')
   (void) pfa_get_device(pfa_get_cur_dir(), target_device);

 strcat(target_device, ":\\");
 if (uGetDiskFreeSpace(target_device, &sectors_per_cluster,
                      &bytes_per_sector, &free_clusters, &dummy) != TRUE)
   return (PFA_E_ERROR);

 *bytes_per_block = (unsigned long) bytes_per_sector *
                    (unsigned long) sectors_per_cluster;
 *out_blocks = (unsigned long) free_clusters;
#endif

 return (PFA_E_NO_ERROR);
}

    /* ===================================================================== */

#if OPER_SYS == WINDOWS_32
static int pfafs_make_hidden_low(const wchar_t *file_wname)
{
 DWORD file_atts;

 file_atts = ntlangwrap_GetFileAttributes(file_wname);
 if (file_atts == (DWORD)(-1))
  {
   dbg_print_info("pfa_make_file_hidden", "Could not find attributes, %d",
                  GetLastError());
   return (PFA_E_ERROR);
  }
 else if (file_atts & FILE_ATTRIBUTE_HIDDEN)
   return (PFA_E_NO_ERROR);

 file_atts |= FILE_ATTRIBUTE_HIDDEN;
 if (!ntlangwrap_SetFileAttributes(file_wname, file_atts))
   return (PFA_E_ERROR);

 return (PFA_E_NO_ERROR);
}
#endif

static int pfafs_make_hidden(Pfa *file)
{
#if OPER_SYS == WINDOWS_32
 wchar_t file_wname[K_PATH_SIZE];
#endif

 if (!pfa_is_local(file))
   return (PFA_E_NOT_ALLOWED);

#if OPER_SYS == WINDOWS_32
 if (pfa_get_file_wname(file, PFA_DEV_PATH_NAME_EXTENSION_VERSION,
                        file_wname) != PFA_E_NO_ERROR)
   return (PFA_E_INVALID_INPUT);

 return pfafs_make_hidden_low(file_wname);
#else
 return PFA_E_NO_ERROR;
#endif
}

#if OPER_SYS == WINDOWS_32
static int PfaUNC_make_hidden(Pfa *file)
{
 wchar_t file_wname[K_PATH_SIZE];

 if (pfa_get_full_wname(file, file_wname) != PFA_E_NO_ERROR)
   return (PFA_E_INVALID_INPUT);

 return pfafs_make_hidden_low(file_wname);
}
#endif
    /* ===================================================================== */

#if OPER_SYS == WINDOWS_32
PfaUNC_register_access_functions()
{
   pfa_loc_method_register(PfaUNC, PfaAccess, PLM_CALL_FII, PfaUNC_access);
   pfa_loc_method_register(PfaUNC, PfaGetFileTime, PLM_CALL_FPPP, PfaUNC_get_file_time);
   pfa_loc_method_register(PfaUNC, PfaGetMode, PLM_CALL_FP, PfaUNC_get_mode);
   pfa_loc_method_register(PfaUNC, PfaGetFileSize, PLM_CALL_FP, PfaUNC_get_file_size);
   pfa_loc_method_register(PfaUNC, PfaGetOwners, PLM_CALL_FPPP, PfaUNC_get_owners);
   pfa_loc_method_register(PfaUNC, PfaChmod, PLM_CALL_FI, PfaUNC_chmod);
   pfa_loc_method_register(PfaUNC, PfaChown, PLM_CALL_FPP, PfaUNC_chown);
   return 0;
}
#endif

int pfafs_register_access_functions()
{
#if OPER_SYS == WINDOWS_32
   pfa_loc_method_register(PfaStdFilesystem, PfaAccess, PLM_CALL_FII, Win32_pfa_access);
#endif
#if OPER_SYS == UNIX
   pfa_loc_method_register(PfaStdFilesystem, PfaAccess, PLM_CALL_FII, Unix_pfa_access);
#endif
   pfa_loc_method_register(PfaStdFilesystem, PfaGetFileTime, PLM_CALL_FPPP,
                           pfafs_get_file_time);
   pfa_loc_method_register(PfaStdFilesystem, PfaSetFileMTime, PLM_CALL_FP,
                           pfafs_set_file_mtime);
   pfa_loc_method_register(PfaStdFilesystem, PfaGetMode, PLM_CALL_FP, pfafs_get_mode);
   pfa_loc_method_register(PfaStdFilesystem, PfaGetFileSize, PLM_CALL_FP,
                           pfafs_get_file_size);
   pfa_loc_method_register(PfaStdFilesystem, PfaGetOwners, PLM_CALL_FPPP, pfafs_get_owners);
   pfa_loc_method_register(PfaStdFilesystem, PfaChmod, PLM_CALL_FI, pfafs_chmod);
   pfa_loc_method_register(PfaStdFilesystem, PfaChown, PLM_CALL_FPP, pfafs_chown);
   pfa_loc_method_register(PfaStdFilesystem, PfaGetAvailBlocks, PLM_CALL_FPP,
                           pfafs_get_avail_blocks);
   pfa_loc_method_register(PfaStdFilesystem, PfaMakeHidden, PLM_CALL_F,
                           pfafs_make_hidden);
   return 0;
}
