/* 24-Aug-94 E-03-34 Anatoli $$1 Ported from Sun RPC 			*/
/* 22-May-95 E-06-39 MXS     $$2 Fixed NT compilation problem 		*/
/* 20-Nov-95 G-01-14 MXS     $$3 Removed proxy call support		*/
/* 06-Dec-95 G-01-16 cfk/PLS $$4 Win95 support                          */
/* 26 Mar 97 H-03-05 swv     $$5 Fix PROXY RPC error reporting          */
/* 30 Aug 00 J-01-16 prf     $$6 Added -noservice on NT                 */
/* 21 Sep 01 J-03-09 jas     $$7 Removed WINDOWS_95 macro               */
/* 22 Oct 01 J-03-11 KSV     $$8 pro_is_win95_running                   */
/* 24-Apr-06 L-01-07 TWH $$9 use windows unicode wrappers               */
/* 12-May-06 L-01-08 TWH $$10 fix typo                                  */
/* 12-Jul-06 L-01-12 ksi $$11 Unicode compliant changes
/* 16-May-08 L-03-09 BI  $$12 Used syswindows.h instead of windows.h
/* 03-Dec-08 L-03-23 BI  $$13 IPv6 code changes
/* 25-Dec-09 L-05-14 CHI $$14  Unicode compliance update (SPR 1925549)
/* 17-Jul-20 P-80-12 KSV $$15 Removed sandbox wrappers */

/* @(#)portmap.c	2.3 88/08/11 4.0 RPCSRC */
#ifndef lint
static	char sccsid[] = "@(#)portmap.c 1.32 87/08/06 Copyr 1984 Sun Micro";
#endif

/*
 * Copyright (c) 1984 by Sun Microsystems, Inc.
 */

/*
 * portmap.c, Implements the program,version to port number mapping for
 * rpc.
 */

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */


#include <btkcsocket.h>
#include <btkcstdio.h>
#include <ptc_win32.h>
#include <syswindows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#include "pm_sunrpc.h"
#include "pmap_prot.h"
#include <signal.h>


// this event is signalled when the
//  worker thread ends
//
HANDLE                  hServDoneEvent = NULL;
SERVICE_STATUS          ssStatus;       // current status of the service

SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwGlobalErr;
DWORD                   TID = 0;
HANDLE                  threadHandle = NULL;

BOOL	portmapper_noservice = FALSE;

//  declare the service threads:
//
VOID    service_main(DWORD dwArgc, LPTSTR *lpszArgv);
VOID    service_ctrl(DWORD dwCtrlCode);
BOOL    ReportStatusToSCMgr(DWORD dwCurrentState,
                            DWORD dwWin32ExitCode,
                            DWORD dwCheckPoint,
                            DWORD dwWaitHint);
VOID    StopSampleService(LPTSTR lpszMsg);
VOID    die(char *reason);
DWORD   pmap_main(VOID *notUsed);
VOID    StopPortmapperService(LPTSTR lpszMsg);



//  main() --
//      all main does is call StartServiceCtrlDispatcher
//      to register the main service thread.  When the
//      API returns, the service has stopped, so exit.
//
int WINAPI WinMain (HINSTANCE hi1, HINSTANCE hi2, LPSTR plParms, int li)
{
	portmapper_noservice = FALSE;

	btk_printf ("%s\n", plParms);

	if (strcmp (plParms, "-noservice") == 0)
	{
		portmapper_noservice = TRUE;
    	pmap_main((void *) 0);
		return (0);
	}
	else if (strcmp (plParms, "-V") == 0)
	{
		btk_printf ("Non-service portmapper. 09-Feb-2000\n");
		return (0);
	}

  {
    SERVICE_TABLE_ENTRY dispatchTable[] = {
        { TEXT("PortmapperService"), (LPSERVICE_MAIN_FUNCTION)service_main },
        { NULL, NULL }
    };


    if (!StartServiceCtrlDispatcher(dispatchTable)) {
        StopPortmapperService("StartServiceCtrlDispatcher failed.");
    }
  }

	return (0);
}

//  service_main() --
//      this function takes care of actually starting the service,
//      informing the service controller at each step along the way.
//      After launching the worker thread, it waits on the event
//      that the worker thread will signal at its termination.
//
VOID
service_main(DWORD dwArgc, char **lpszArgv)
{
    DWORD                   dwWait, dummy;

    // register our service control handler:
    //
  {
    sshStatusHandle = uRegisterServiceCtrlHandler(
                                    "PortmapperService",
                                    (LPHANDLER_FUNCTION) service_ctrl);
    if (!sshStatusHandle)
        goto cleanup;

    // SERVICE_STATUS members that don't change in example
    //
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;


    // report the status to Service Control Manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        1,                     // checkpoint
        3000))                 // wait hint
        goto cleanup;

    // create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    //
    hServDoneEvent = uCreateEvent(
        NULL,    // no security attributes
        TRUE,    // manual reset event
        FALSE,   // not-signalled
        NULL);   // no name

    if (hServDoneEvent == (HANDLE)NULL)
        goto cleanup;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        2,                     // checkpoint
        3000))                 // wait hint
        goto cleanup;

    // create a security descriptor that allows anyone to write to
    //  the pipe...
    //


    // start the thread that performs the work of the service.
    //
    threadHandle = (HANDLE)CreateThread(NULL, 8192,
                    (LPTHREAD_START_ROUTINE)pmap_main, NULL, 0, &dummy);

    if (!threadHandle)
        goto cleanup;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_RUNNING, // service state
        NO_ERROR,        // exit code
        0,               // checkpoint
        0))              // wait hint
        goto cleanup;

    // wait indefinitely until hServDoneEvent is signaled.
    //
    dwWait = WaitForSingleObject(
        hServDoneEvent,  // event object
        INFINITE);       // wait indefinitely

cleanup:

    if (hServDoneEvent != NULL)
        CloseHandle(hServDoneEvent);


    // try to report the stopped status to the service control manager.
    //
    if (sshStatusHandle != 0)
        (VOID)ReportStatusToSCMgr(
                            SERVICE_STOPPED,
                            dwGlobalErr,
                            0,
                            0);

    // When SERVICE MAIN FUNCTION returns in a single service
    // process, the StartServiceCtrlDispatcher function in
    // the main thread returns, terminating the process.
    //
    return;
  }
}



//  service_ctrl() --
//      this function is called by the Service Controller whenever
//      someone calls ControlService in reference to our service.
//
VOID
service_ctrl(DWORD dwCtrlCode)
{
	if (portmapper_noservice) return;

    {
    DWORD  dwState = SERVICE_RUNNING;

    // Handle the requested control code.
    //
    switch(dwCtrlCode) {

        // Pause the service if it is running.
        //
        case SERVICE_CONTROL_PAUSE:

            if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
                SuspendThread(threadHandle);
                dwState = SERVICE_PAUSED;
            }
            break;

        // Resume the paused service.
        //
        case SERVICE_CONTROL_CONTINUE:

            if (ssStatus.dwCurrentState == SERVICE_PAUSED) {
                ResumeThread(threadHandle);
                dwState = SERVICE_RUNNING;
            }
            break;

        // Stop the service.
        //
        case SERVICE_CONTROL_STOP:

            dwState = SERVICE_STOP_PENDING;

            // Report the status, specifying the checkpoint and waithint,
            //  before setting the termination event.
            //
            ReportStatusToSCMgr(
                    SERVICE_STOP_PENDING, // current state
                    NO_ERROR,             // exit code
                    1,                    // checkpoint
                    3000);                // waithint

            SetEvent(hServDoneEvent);
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    // send a status response.
    //
    ReportStatusToSCMgr(dwState, NO_ERROR, 0, 0);
    }
}


// utility functions...



// ReportStatusToSCMgr() --
//      This function is called by the ServMainFunc() and
//      ServCtrlHandler() functions to update the service's status
//      to the service control manager.
//
BOOL
ReportStatusToSCMgr(DWORD dwCurrentState,
                    DWORD dwWin32ExitCode,
                    DWORD dwCheckPoint,
                    DWORD dwWaitHint)
{
	if (portmapper_noservice) return (FALSE);

    {
    BOOL fResult;

    // Disable control requests until the service is started.
    //
    if (dwCurrentState == SERVICE_START_PENDING)
        ssStatus.dwControlsAccepted = 0;
    else
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
            SERVICE_ACCEPT_PAUSE_CONTINUE;

    // These SERVICE_STATUS members are set from parameters.
    //
    ssStatus.dwCurrentState = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwCheckPoint = dwCheckPoint;

    ssStatus.dwWaitHint = dwWaitHint;

    // Report the status of the service to the service control manager.
    //
    if (!(fResult = SetServiceStatus(
                sshStatusHandle,    // service reference handle
                &ssStatus))) {      // SERVICE_STATUS structure

        // If an error occurs, stop the service.
        //
        StopPortmapperService("SetServiceStatus");
    }
    return fResult;
    }

  return FALSE;
}



// The StopPortmapperService function can be used by any thread to report an
//  error, or stop the service.
//
VOID
StopPortmapperService(char * lpszMsg)
{
	if (portmapper_noservice)
	{
		/* printf ("%s\n", lpszMsg); */
    	exit(0);
	}

  {
    char    chMsg[256];
    HANDLE  hEventSource;
    char *  lpszStrings[2];

    dwGlobalErr = GetLastError();

    // Use event logging to log the error.
    //
    hEventSource = uRegisterEventSource(NULL,
                            "PortmapperService");

    btk_sprintf(chMsg, "PortmapperService error: %d", dwGlobalErr);
    lpszStrings[0] = chMsg;
    lpszStrings[1] = lpszMsg;

    if (hEventSource != NULL) {
        uReportEvent(hEventSource, // handle of event source
            EVENTLOG_ERROR_TYPE,  // event type
            0,                    // event category
            0,                    // event ID
            NULL,                 // current user's SID
            2,                    // strings in lpszStrings
            0,                    // no bytes of raw data
            lpszStrings,          // array of error strings
            NULL);                // no raw data

        (VOID) DeregisterEventSource(hEventSource);
    }

    // Set a termination event to stop SERVICE MAIN FUNCTION.
    //
    SetEvent(hServDoneEvent);
  }
}

int reg_service();
struct pmaplist *pmaplist;
static int debugging = 0;

static void callit();

DWORD pmap_main(void *unused)
{
	SVCXPRT *xprt;
	int pid, t;
	SOCKET sock;
	struct sockaddr_in addr;
	struct addrinfo *SAddrinfo, Hints;
	int len = sizeof(struct sockaddr_in);
	register struct pmaplist *pml;
	char Port[PORTSIZE];

	memset(&Hints, 0, sizeof(Hints)); 
        Hints.ai_family = AF_UNSPEC;
        Hints.ai_socktype = SOCK_DGRAM;
        Hints.ai_flags = AI_PASSIVE;
	Hints.ai_protocol = IPPROTO_UDP;

        btk_sprintf(Port,"%d",PMAPPORT);

	btk_get_nodeaddrinfo(NULL,Port,&Hints,&SAddrinfo);

	if ((sock = CreateSocket(SAddrinfo->ai_family, SAddrinfo->ai_socktype, SAddrinfo->ai_protocol)) == INVALID_SOCKET) {
		StopPortmapperService ("Portmapper cannot create socket.");
	}

	if (bind (sock,SAddrinfo->ai_addr,SAddrinfo->ai_addrlen) != 0) {
		StopPortmapperService ("Portmapper cannot bind.");
	}

	if ((xprt = svcudp_create(sock)) == (SVCXPRT *)NULL) {
		StopPortmapperService ("Portmapper cannot create UDP service.");
	}
	/* make an entry for ourself */
	pml = (struct pmaplist *)malloc((u_int)sizeof(struct pmaplist));
	pml->pml_next = 0;
	pml->pml_map.pm_prog = PMAPPROG;
	pml->pml_map.pm_vers = PMAPVERS;
	pml->pml_map.pm_prot = IPPROTO_UDP;
	pml->pml_map.pm_port = PMAPPORT;
	pmaplist = pml;

	if ((sock = CreateSocket(SAddrinfo->ai_family, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		StopPortmapperService ("Portmapper cannot create socket.");
	}
	if (bind(sock, SAddrinfo->ai_addr,SAddrinfo->ai_addrlen) != 0) {
		StopPortmapperService ("Portmapper cannot bind.");
	}
	if ((xprt = svctcp_create(sock, RPCSMALLMSGSIZE, RPCSMALLMSGSIZE))
	    == (SVCXPRT *)NULL) {
		StopPortmapperService ("Portmapper cannot create TCP service.");
	}
	/* make an entry for ourself */
	pml = (struct pmaplist *)malloc((u_int)sizeof(struct pmaplist));
	pml->pml_map.pm_prog = PMAPPROG;
	pml->pml_map.pm_vers = PMAPVERS;
	pml->pml_map.pm_prot = IPPROTO_TCP;
	pml->pml_map.pm_port = PMAPPORT;
	pml->pml_next = pmaplist;
	pmaplist = pml;

	(void)svc_register(xprt, PMAPPROG, PMAPVERS, reg_service, FALSE);

	svc_run();

	StopPortmapperService ("Portmapper: svc_run returned unexpectedly.");
	
	btk_free_nodeaddrinfo(SAddrinfo);
        return 0;
}

static struct pmaplist *
find_service(prog, vers, prot)
	u_long prog;
	u_long vers;
{
	register struct pmaplist *hit = NULL;
	register struct pmaplist *pml;

	for (pml = pmaplist; pml != NULL; pml = pml->pml_next) {
		if ((pml->pml_map.pm_prog != prog) ||
			(pml->pml_map.pm_prot != prot))
			continue;
		hit = pml;
		if (pml->pml_map.pm_vers == vers)
		    break;
	}
	return (hit);
}

/*
 * 1 OK, 0 not
 */
reg_service(rqstp, xprt)
	struct svc_req *rqstp;
	SVCXPRT *xprt;
{
	struct pmap reg;
	struct pmaplist *pml, *prevpml, *fnd;
	int ans, port;
	caddr_t t;


#ifdef DEBUG
	static int seq = 0;
	/* fprintf(stderr, "server: about do a switch: seq = %d\n", seq++); */
#endif
	switch (rqstp->rq_proc) {

	case PMAPPROC_NULL:
		/*
		 * Null proc call
		 */
		if ((!svc_sendreply(xprt, xdr_void, NULL)) && debugging) {
			StopPortmapperService ("Portmapper cannot svc_sendreply().");
		}
		break;

	case PMAPPROC_SET:
		/*
		 * Set a program,version to port mapping
		 */
		if (!svc_getargs(xprt, xdr_pmap, &reg))
			svcerr_decode(xprt);
		else {
			/*
			 * check to see if already used
			 * find_service returns a hit even if
			 * the versions don't match, so check for it
			 */
			fnd = find_service(reg.pm_prog, reg.pm_vers, reg.pm_prot);
			if (fnd && fnd->pml_map.pm_vers == reg.pm_vers) {
				if (fnd->pml_map.pm_port == reg.pm_port) {
					ans = 1;
					goto done;
				}
				else {
					ans = 0;
					goto done;
				}
			} else {
				/*
				 * add to END of list
				 */
				pml = (struct pmaplist *)
				    malloc((u_int)sizeof(struct pmaplist));
				pml->pml_map = reg;
				pml->pml_next = 0;
				if (pmaplist == 0) {
					pmaplist = pml;
				} else {
					for (fnd= pmaplist; fnd->pml_next != 0;
					    fnd = fnd->pml_next);
					fnd->pml_next = pml;
				}
				ans = 1;
			}
		done:
#ifdef DEBUG
			/* fprintf(stderr, "svc_sendreply : ans = %d\n", ans); */
#endif
			if ((!svc_sendreply(xprt, xdr_long, (caddr_t)&ans)) &&
			    debugging) {
				/* fprintf(stderr, "svc_sendreply\n"); */
				StopPortmapperService ("Portmapper cannot svc_sendreply().");
			}
		}
		break;

	case PMAPPROC_UNSET:
		/*
		 * Remove a program,version to port mapping.
		 */
		if (!svc_getargs(xprt, xdr_pmap, &reg))
			svcerr_decode(xprt);
		else {
			ans = 0;
			for (prevpml = NULL, pml = pmaplist; pml != NULL; ) {
				if ((pml->pml_map.pm_prog != reg.pm_prog) ||
					(pml->pml_map.pm_vers != reg.pm_vers)) {
					/* both pml & prevpml move forwards */
					prevpml = pml;
					pml = pml->pml_next;
					continue;
				}
				/* found it; pml moves forward, prevpml stays */
				ans = 1;
				t = (caddr_t)pml;
				pml = pml->pml_next;
				if (prevpml == NULL)
					pmaplist = pml;
				else
					prevpml->pml_next = pml;
				free(t);
			}
			if ((!svc_sendreply(xprt, xdr_long, (caddr_t)&ans)) &&
			    debugging) {
				/* fprintf(stderr, "svc_sendreply\n"); */
				StopPortmapperService ("Portmapper cannot svc_sendreply().");
			}
		}
		break;

	case PMAPPROC_GETPORT:
		/*
		 * Lookup the mapping for a program,version and return its port
		 */
		if (!svc_getargs(xprt, xdr_pmap, &reg))
			svcerr_decode(xprt);
		else {
			fnd = find_service(reg.pm_prog, reg.pm_vers, reg.pm_prot);
			if (fnd)
				port = fnd->pml_map.pm_port;
			else
				port = 0;
			if ((!svc_sendreply(xprt, xdr_long, (caddr_t)&port)) &&
			    debugging) {
				StopPortmapperService ("Portmapper cannot svc_sendreply().");
			}
		}
		break;

	case PMAPPROC_DUMP:
		/*
		 * Return the current set of mapped program,version
		 */
		if (!svc_getargs(xprt, xdr_void, NULL))
			svcerr_decode(xprt);
		else {
			if ((!svc_sendreply(xprt, xdr_pmaplist,
			    (caddr_t)&pmaplist)) && debugging) {
				StopPortmapperService ("Portmapper cannot svc_sendreply().");
			}
		}
		break;

	case PMAPPROC_CALLIT:
		/*
		 * Calls a procedure on the local machine.  If the requested
		 * procedure is not registered this procedure does not return
		 * error information!!
		 * This procedure is only supported on rpc/udp and calls via
		 * rpc/udp.  It passes null authentication parameters.
		 */
	   /*************************************************************/
	   /* PTC's portmapper service does not support Poxy RPC Calls	*/
	   /* This functionality is excluded from PTC's portmapper 	*/
	   /* to prevent a bug that was seen in the case of proxy calls */
	   /* on NT machines - at a customer site. We could             */
           /* return "SVCERR_NOPROC", but RFC 1833 points out that      */
           /* if "callit" fails, then we just keep quiet and do not     */
           /* errors.                                                   */
	   /*************************************************************/
# if 0
	   	callit(rqstp, xprt);
# endif

		/* svcerr_noproc(xprt); */
		break;

	default:
		svcerr_noproc(xprt);
		break;
	}
}


/*
 * Stuff for the rmtcall service
 */
#define ARGSIZE 9000

typedef struct encap_parms {
	u_long arglen;
	char *args;
};

static bool_t
xdr_encap_parms(xdrs, epp)
	XDR *xdrs;
	struct encap_parms *epp;
{

	return (xdr_bytes(xdrs, &(epp->args), &(epp->arglen), ARGSIZE));
}

typedef struct rmtcallargs {
	u_long	rmt_prog;
	u_long	rmt_vers;
	u_long	rmt_port;
	u_long	rmt_proc;
	struct encap_parms rmt_args;
};

static bool_t
xdr_rmtcall_args(xdrs, cap)
	register XDR *xdrs;
	register struct rmtcallargs *cap;
{

	/* does not get a port number */
	if (xdr_u_long(xdrs, &(cap->rmt_prog)) &&
	    xdr_u_long(xdrs, &(cap->rmt_vers)) &&
	    xdr_u_long(xdrs, &(cap->rmt_proc))) {
		return (xdr_encap_parms(xdrs, &(cap->rmt_args)));
	}
	return (FALSE);
}

static bool_t
xdr_rmtcall_result(xdrs, cap)
	register XDR *xdrs;
	register struct rmtcallargs *cap;
{
	if (xdr_u_long(xdrs, &(cap->rmt_port)))
		return (xdr_encap_parms(xdrs, &(cap->rmt_args)));
	return (FALSE);
}

/*
 * only worries about the struct encap_parms part of struct rmtcallargs.
 * The arglen must already be set!!
 */
static bool_t
xdr_opaque_parms(xdrs, cap)
	XDR *xdrs;
	struct rmtcallargs *cap;
{

	return (xdr_opaque(xdrs, cap->rmt_args.args, cap->rmt_args.arglen));
}

/*
 * This routine finds and sets the length of incoming opaque paraters
 * and then calls xdr_opaque_parms.
 */
static bool_t
xdr_len_opaque_parms(xdrs, cap)
	register XDR *xdrs;
	struct rmtcallargs *cap;
{
	register u_int beginpos, lowpos, highpos, currpos, pos;

	beginpos = lowpos = pos = xdr_getpos(xdrs);
	highpos = lowpos + ARGSIZE;
	while ((int)(highpos - lowpos) >= 0) {
		currpos = (lowpos + highpos) / 2;
		if (xdr_setpos(xdrs, currpos)) {
			pos = currpos;
			lowpos = currpos + 1;
		} else {
			highpos = currpos - 1;
		}
	}
	xdr_setpos(xdrs, beginpos);
	cap->rmt_args.arglen = pos - beginpos;
	return (xdr_opaque_parms(xdrs, cap));
}

/*
 * Call a remote procedure service
 * This procedure is very quiet when things go wrong.
 * The proc is written to support broadcast rpc.  In the broadcast case,
 * a machine should shut-up instead of complain, less the requestor be
 * overrun with complaints at the expense of not hearing a valid reply ...
 *
 * This now forks so that the program & process that it calls can call
 * back to the portmapper.
 */

typedef struct {
	struct svc_req *rqstp;
	SVCXPRT *xprt;
} CALLITARGS;


static int callit_do();

static void
callit(rqstp, xprt)
	struct svc_req *rqstp;
	SVCXPRT *xprt;
{
	HANDLE thread = NULL;
	CALLITARGS *args = malloc(sizeof(CALLITARGS));
	long thid;

	args->rqstp = rqstp;
	args->xprt = xprt;

	if (args != NULL)
            thread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)
        					callit_do, args, 0, &thid);
        if (thread == NULL)
        {
  		StopPortmapperService ("Portmapper cannot create thread.");
        }


}

static int callit_do (args)
CALLITARGS *args;
{
	struct svc_req *rqstp = args->rqstp;
	SVCXPRT *xprt = args->xprt;
	struct sockaddr_storage *addrStore;
	struct rmtcallargs a;
	struct pmaplist *pml;
	u_short port;
	struct sockaddr_in6 me;
	int pid;
	SOCKET socket = INVALID_SOCKET;
	CLIENT *client;
	struct authunix_parms *au = (struct authunix_parms *)rqstp->rq_clntcred;
	struct timeval timeout;
	char buf[ARGSIZE];

	free(args);

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	a.rmt_args.args = buf;
	if (!svc_getargs(xprt, xdr_rmtcall_args, &a))
	    return -1;
	if ((pml = find_service(a.rmt_prog, a.rmt_vers, IPPROTO_UDP)) == NULL)
	    return -1;
	port = pml->pml_map.pm_port;
	get_myaddress(&me);
	me.sin6_port = htons(port);

	addrStore = (struct sockaddr_storage *)&me;
	client = clntudp_create(addrStore, a.rmt_prog, a.rmt_vers, timeout, &socket);
	if (client != (CLIENT *)NULL) {
		if (rqstp->rq_cred.oa_flavor == AUTH_UNIX) {
			client->cl_auth = authunix_create(au->aup_machname,
			   au->aup_uid, au->aup_gid, au->aup_len, au->aup_gids);
		}
		a.rmt_port = (u_long)port;
		if (clnt_call(client, a.rmt_proc, xdr_opaque_parms, &a,
		    xdr_len_opaque_parms, &a, timeout) == RPC_SUCCESS) {
			svc_sendreply(xprt, xdr_rmtcall_result, &a);
		}
		AUTH_DESTROY(client->cl_auth);
		clnt_destroy(client);
	}
	(void)CloseSocket(socket);
	return 0;
}


