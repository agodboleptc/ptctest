/******************************************************************************
 Functions using PLP cpu_id instead of lmhostid.  

 Note: Do not add lots of stuff to this file.  In addition to being compiled
       with this directory, this file is also compiled separately and linked
       into the vendor daemon.  Thus we wish to keep it simple and minimize
       dependencies.

 11-Feb-97 H-03-01 EEB/MRC   $$1 Created.
 25-Feb-97 H-03-02 EEB/MRC   $$2 Fix def'n of PTCTYPE                     
 25-Mar-97 H-03-04 EEB/MRC   $$3 Make pli_GetHostId() output uppercase.
 08-Apr-97 H-03-05 EEB/MRC   $$4 Added comment.  
 15-Apr-97 H-03-06 EEB/MRC   $$5 Rename .c to .cxx
 06-May-97 H-03-09 EEB/MRC   $$6 Fix calling convention of pli_GetHostId().
 07-May-97 H-03-09+ EEB/MRC  $$7 Fix  #6 for win95
 28-May-97 H-03-12 EEB/MRC   $$8 Add extern "C" to pli_NewHostId().
 04-Jun-97 H-03-13  EEB/MRC  $$9 Don't include sysstdio.h.
 24-Jun-97 H-03-15  EEB/MRC $$10 Added conditionals for separate compilation.
 27-Jun-97 H-03-15+ EEB/MRC $$11 Use LM_CALLBACK_TYPE
 08-Jul-97 H-03-16  EEB/MRC $$12 Change lmclient.h to ptc_lmclient.h.
 12-Aug-97 H-03-97  EEB/MRC $$13 Use lmclient if OUT_OF_SYSTEM
 19-Sep-97 H-03-23  MRC/DJA $$14 return host_id like cpu_id does
 03-Oct-97 H-03-25  MRC/DJA $$15 include ProHardware.h
 09-Oct-97 H-03-25+ MRC     $$16 remove C++ style comment to allow c compile
                                 for daemon
 11-Feb-98 H-03-37+ DJA/MRC $$17 Removed check for RNTX86(MECHANICA FLAG)
 16-Mar-98 H-03-41  DJA/FBI $$18 Call geom_hdr_inita_str for WINDOWS_32.
 01-May-98 I-01-06  DJA/MRC $$19 Added security to PTC_HOSTID
 15-Jun-98 I-01-11  FBI     $$20 Included geomhdrinita.h.
 19-May-98 I-01-11  DJA/MRC $$21 Moved scrambled strings to separate file.
 29-Jun-98 I-01-14  DJA/MRC $$22 Enhanced allowable values for PTC_HOSTID
 29-Sep-98 I-01-21  DJA/MRC $$23 Updated for OUT_OF_SYSTEM.
 01-Aug-01 J-03-05  ACT/MRC $$24 Changed the lc_* calls to plc_*
 09-May-05 K-03-24  KSV     $$25 Conditional compilation for ##24
 17-May-05 K-03-24+ MRC     $$26 fix ptc_lmclient.h->lmclient.h
 10-Nov-05 K-03-36  KSV     $$27 Added string.h
 24-Apr-06 L-01-07  TWH     $$28 use windows unicode wrappers
 12-Jul-06 L-01-12  ksi     $$29 Unicode compliant changes
 28-Dec-07 L-01-42  MRC     $$30 saved_hostid;
                                 pli_HostidCreate()
                                 pli_HostidAppend()
                                 pli_HostidRemember()
                                 pli_GetHostId uses HostidListGet()
 07-Mar-08 L-03-04  MRC     $$31 fix ##30 - pli_GetHostId crashes appending
                                 ANY to hostid chain on Unix
 12-Mar-07 L-03-04  MRC     $$32 undo ##30
 21-May-08 L-03-09  KSV     $$33 Fix ported from L-01
 16-May-08 L-03-09  BI      $$34 Used syswindows.h instead of windows.h
 14-May-08 L-03-09  MRC     $$35 reinstate #31 by initializing unix hostid
 01-Jun-08 L-03-17  MRC     $$36 remove lint
 28-Mar-11 L-05-44  TWH     $$37 HostidListGet add second argument
 12-Mar-12 P-20-01  AC      $$38 Updated for Project 13028358
 19-Jul-16 P-30-36  SGL     $$39 VS2012 / VS2015 compatibility changes 
******************************************************************************/

/******* WARNING *************** WARNING ********** WARNING *******************
*                                                                             *
* Note: Do not add lots of stuff to this file.  In addition to being compiled *
*       with this directory, this file is also compiled separately and linked *
*       into the vendor daemon.  Thus we wish to keep it simple and minimize  *
*       dependencies.                                                         *
*                                                                             *
*       The vendor daemon uses a C compiler so don't use C++ specific stuff:  *
*       slash-slash comments, classes, etc.                                   *
*                                                                             *
******* WARNING *************** WARNING ********** WARNING *******************/

/******************************* Includes ************************************/

/* for NULL and sprintf */
#ifdef OUT_OF_SYSTEM

#  include <ptc_win32.h>
#  include <btkcstdio.h>
#  include <stdio.h>
#  include <plpf.h>
#  include <scramble.h>
#  include <plpf_a1scram_strings.cxx>

#  ifndef Bool
#    define Bool int
#  endif

#  ifdef WIN32
#    include <syswindows.h>
#    define IS_WINDOWS 1
#  else
#    define IS_WINDOWS 0
#  endif /* WIN32 */

   int pli_GetAllowAnyInCallback(){return(0);}

#  if defined (SUNOS5) || defined (_HIUX_SOURCE)
#    define FOURDIGITPAIRHOSTID 1
#  endif

#else /* ! OUT_OF_SYSTEM */

#  include "ProHardware.h"

#  if OPER_SYS == WINDOWS_32
#     define IS_WINDOWS 1
#  else
#     define IS_WINDOWS 0
#  endif /* OPER_SYS */

#  if (PRO_MACHINE == SUN4) || (PRO_MACHINE == HITACHI)
#    define FOURDIGITPAIRHOSTID 1
#  endif /* PRO_MACHINE */

#endif /* ! OUT_OF_SYSTEM */

#include <lmclient.h>
#include <lm_attr.h>
#include <string.h>

#define PTCTYPE HOSTID_VENDOR

/* The following is from const.h: */
#ifndef K_NAME_SIZE
#define K_NAME_SIZE 32
#endif

/* End of stuff from const.h */


#include <geomhdrinita.h>
#include <const.h>
#include <ct_adapters.h>
#include <ctgeom_protos.h>
#include <llog.h>

/***************************** Statics ****************************************/
static char plp_ptc_hostid[UNSCRAM_BUF_SIZE];
#ifdef OUT_OF_SYSTEM
  static char *pending_saved_hostid; // moves to hostid if checkout is successful
  static char *saved_hostid;         // checked in HostidListGet:
#endif
/******************************************************************************
 FUNCTION:   pli_GetPTCHostidLabel
 
 PURPOSE:    Callback to get the PTC hostid type ie. PTC_HOSTID.
 
 INPUT:      None
 
 RETURN:     PTC_HOSTID
******************************************************************************/
char *pli_GetPTCHostidLabel()
{
     static int already_called = FALSE;

     if (! already_called)
     {
        already_called = TRUE;

        strcpy(plp_ptc_hostid, cu_Unscramble(plp_ptc_hostid_scram));
     }

     return(plp_ptc_hostid);
}

static HOSTID *pli_HostidCreate(const char *this_hostid)
{
  HOSTID *h = NULL;

#ifdef OUT_OF_SYSTEM
    h = l_new_hostid();
#else
    h = pl_new_hostid();
#endif
    memset(h, 0, sizeof(HOSTID));
    h->type = PTCTYPE;

    strcpy(h->id.vendor, this_hostid);

  return h;

} // pli_HostidCreate


static HOSTID *pli_HostidAppend(HOSTID *hostidlist, HOSTID *new_hostid)
{
  HOSTID *h = hostidlist;

#   ifdef PLPF_DEBUG
        llog_info ("pli_HostidAppend: appending %s", new_hostid->id.vendor);
#   endif

    if (hostidlist == NULL)
      return new_hostid; // first element in new list

      // advance to the end of the list
    while (h->next != NULL)
        h = h->next;

      // append to last element
    h->next = new_hostid;

  return hostidlist;

} // pli_HostidAppend


static void pli_HostidRemember(int ignore)
{
    if (!ignore && !saved_hostid)
    {  
        if (saved_hostid)
            relmem (&saved_hostid);

        saved_hostid = pending_saved_hostid;

#       ifdef PLPF_DEBUG
            llog_info ("   pli_HostidRemember: %s", saved_hostid);
#       endif
    }
    else
        if (pending_saved_hostid)
            relmem (&pending_saved_hostid);

    pending_saved_hostid = NULL;
}

/******************************************************************************
 FUNCTION:   pli_GetHostId

 PURPOSE:    Callback to get the PTC hostid.
             A list of hostid's is returned. The first
             is the actual hostid, the second may be
             "ANY". We put "ANY" in the list only when:
             1. (zapped) AE license
             2. (zapped) DEMO license
             3. (zapped) SUPERMODULE definition
             4. (un-zapped) inhouse backup license - must have 3-day expiration.
                It is this expiration date check that forces us to structure the
                code this way.
 
 INPUT:      idtype

 RETURN:     *HOSTID, pointer to host id information
******************************************************************************/
HOSTID *
LM_CALLBACK_TYPE
pli_GetHostId(short idtype)
{
    HOSTID *h = NULL;
    unsigned char hardware[6];
    char *hostid_list = NULL;
    int i, status;
    char *s;
    
    if (idtype == PTCTYPE)
    {
        
#if IS_WINDOWS
        status = HostidListGet (&hostid_list, NULL);
        
        if (hostid_list != NULL)
        {
              /* if first time, save a copy of this hostidlist for use in Refresh */
            if (pending_saved_hostid == NULL)
            {
                pending_saved_hostid = (char *)getmem (strlen(hostid_list)+1);
                strcpy (pending_saved_hostid, hostid_list);
            }

              /* now put each hostid in list onto separate flex struct */
            s = strtok (hostid_list, ",");
    
            if (s == NULL)
            {
                h = pli_HostidCreate ("");
              return h;
            }
    
            do
            {
                h = pli_HostidAppend (h, pli_HostidCreate (s));
                s = strtok (NULL, ",");
            } while (s);
    
        } // hostid_list != NULL
		else
		{
            char net_addr[K_NAME_SIZE];

			// we really don't expect to get here.. just a backup to use old logic
			// we are not setting "pending_saved_hostid" test condition won't occur 
			// due to failure in call it's call to HostidListGet

			status = geom_hdr_inita_str(net_addr);
			h = pli_HostidCreate (net_addr);
#          ifdef PLPF_DEBUG
               llog_info ("pli_GetHostId: fallback to ghia_str callback: %s", net_addr);
#          endif
		}

#else
        status = geom_hdr_inita(hardware);
        h = pli_HostidCreate(""); // initialize
#endif                          /* IS_WINDOWS */

        if ( pli_GetAllowAnyInCallback() ) /* allow HOSTID=PTC_HOSTID=ANY? */
        {
          HOSTID *h_any = pli_HostidCreate(cu_Unscramble(plp_any_string));

           h = pli_HostidAppend(h, h_any);
        }

        if (status != 1)
        {  /* couldn't get host-id */
           h->id.vendor[0] = '\0';
        }
        else  /* format like cpu_id utility */
        {
           s = h->id.vendor;
#if IS_WINDOWS
#          ifdef PLPF_DEBUG
               llog_info ("pli_GetHostId: hostid added to callback: %s", hostid_list);
#          endif

           relmem(&hostid_list);
#else
          
#ifdef  FOURDIGITPAIRHOSTID
# define HOSTIDSTARTINDEX 2
#else
# define HOSTIDSTARTINDEX 0
#endif
           for (i=HOSTIDSTARTINDEX;  i<6; i++,s+=3)
           {
              btk_sprintf(s, "%02X-", hardware[i]);
           }
           s[-1] = '\0'; /* clear the trailing dash */
#undef HOSTIDSTARTINDEX

#endif      /* IS_WINDOWS */

#undef FOURDIGITPAIRHOSTID
        }

    }

    return(h);
}



/******************************************************************************
 Function:  pli_NewHostId 

 PURPOSE:  call this function just after lc_init()
           from your application, and also in your
           license generator (e.g., lmcrypt.c), and
           your vendor daemon.  To do this in the vendor
           daemon, add the following to lsvendor.c:
      
              void pli_NewHostId();
              void (*ls_user_init1)() = pli_NewHostId;

******************************************************************************/

#ifdef __cplusplus
extern "C" 
#endif
void pli_NewHostId(void *v_lm_job)
{
    LM_VENDOR_HOSTID h;
    LM_HANDLE *lm_job = (LM_HANDLE *) v_lm_job;
    
    if (lm_job != NULL)
    {
        /* define the hostid type */
        memset(&h, 0, sizeof (h));
        h.label = pli_GetPTCHostidLabel();
        h.hostid_num = PTCTYPE;
        h.case_sensitive = 0; 
        h.get_vendor_id = pli_GetHostId;

        /* register this definition: */
#ifdef OUT_OF_SYSTEM
        lc_set_attr(lm_job, LM_A_VENDOR_ID_DECLARE, (LM_A_VAL_TYPE) &h);
#else
        plc_set_attr(lm_job, LM_A_VENDOR_ID_DECLARE, (LM_A_VAL_TYPE) &h);
#endif
    }
}

#ifdef OUT_OF_SYSTEM
void ptc_exit(int v){ }

#ifdef WIN32



/***************************************************************************/
/* Initialize the version information structure */

static OSVERSIONINFOA    m_osversion;

static int
init_version_info (void)
{
    static BOOL         already_called = FALSE;

    if (already_called)
        return 1;

    /* Query version information */
    m_osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA); 
    if (!uGetVersionEx(&m_osversion))        
       return 0;
    
    already_called = TRUE;
    return 1;
}

/******************************************************************************

    Function:   MSWIN_is_win95_running()

    Purpose:    To provide a function that returns if Windows 95 is the
                operating system currently running.

     Author:	Eddy Pittman

     Input:     none

     Output:	TRUE (1) or FALSE (0)
****************************************************************************/
int 
MSWIN_is_win95_running (void) 
{
    if (!init_version_info())
        return 0;
    
    return (m_osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS); 
}

/******************************************************************************

    Function:   MSWIN_is_winnt_running()

    Purpose:    To provide a function that returns if Windows NT is the
                operating system currently running.

     Author:	Eddy Pittman

     Input:     none

     Output:	TRUE (1) or FALSE (0)
****************************************************************************/
int 
MSWIN_is_winnt_running (void) 
{
    if (!init_version_info())
        return 0;
    
    return (m_osversion.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

#endif
#endif



#undef PTCTYPE
#undef PTCSTRING
#ifdef K_NAME_SIZE
#  undef K_NAME_SIZE
#endif
#ifdef IS_WINDOWS
#  undef IS_WINDOWS
#endif
