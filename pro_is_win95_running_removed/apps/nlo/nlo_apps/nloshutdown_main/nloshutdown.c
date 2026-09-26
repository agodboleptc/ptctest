/***********************************************************************\
| PROSHUTDOWN - Shut down an Pro/Server. Requires superuser privilege.	|
| 1st argument (optional) : Pro/Server hostname (defaults to current).	|
|									|
| 12-Dec-89 A-17-08 df	$$1  Initial coding and bugs.			|
| 25-Jan-90 A-18-13 df  $$2  Added args to connect_to_server().         |
| 08-Feb-90 A-19-01 df  $$3  ipc_put_message(), ipc_get_message().	|
| 20-Jun-91 A-27-01 df	$$4  Renamed to 'proshutdown'.			|
| 16-Aug-91 A-27-09 df	$$5  Use WILDCARD_SERIAL in request.		|
| 06-Jan-92 C-01-26 df	$$6  Removed 2nd arg of connect_to_server().	|
| 24-Jun-94 E-03-30 SWL $$7  Added processing "-verify" on command line |
| 10-Oct-94 E-06-14 mrc $$8  IPCmessage changes for Triads              |
|                            calls ipc_exchange_messages()              |
|                            put error message if can't cinnect_to_server
| 03-Nov-94 E-06-17 MRC $$9  revert back to NON-TRIAD NLO               |
| 08-Feb-95 E-06-28 MRC $$10 leap forward to IRON-TRIAD NLO             |
| 13-Feb-95 E-06-29 MRC $$11 revert back to NON-TRIAD NLO               |
| 08-Feb-95 E-06-32 MRC $$12 leap forward to IRON-TRIAD NLO             |
| 22-May-95 E-07-14 MRC $$13 revert back to non-Triad NLO               |
| 05-Jun-95 E-07-17 MRC $$14 calls ipc_exchange_messages()              |
|                            nlo_set_timeout_multiplier(10)             |
  22-Aug-95 G-01-05 EEB/MRC $$15 leap forward to IRON-TRIAD NLO
| 18-Sep-95 G-01-07 MRC $$16 rename connect_to_server ipc_connect2server
| 09-Nov-95 G-01-13 MRC $$17 change -verify to use get_internal_version()
|                            ipc_exchange_messages interface change
22-Jan-96 G-03-01 jmichaud $$18 Include header for pro_printf() varargs funcs.
| 12-Feb-96 G-03-03 rsc $$19 change get_hostname to pro_get_hostname.
| 22-Apr-96 G-03-11 EEB/MRC $$20 Add call to set_host_in_list().
| 27-Aug-96 H-01-06 EEB/MRC $$21 Fix shutdown message for non-UNIX.
| 04-Sep-96 H-01-07 EEB/MRC $$22 Fix shutdown message for WIN95
| 24-Jan-97 H-01-25 EEB/MRC $$23 Allow NT compiled code to run under WIN 95
| 14-Jul-00 J-01-14 JPE     $$24 WIN64 Port
| 11-May-04 K-03-03 ASRS    $$25 X86E_WIN64 support
\***********************************************************************/


#include "pro_hardware.h"
#include "xarray.h"
#include "pro_widec.h"
#include "pro_string.h"
#include "wchar_t.h"
#include "sysmath.h"
#include "sysstdio.h"
#include	"nlo.h"
#include	"proipc.h"
#include        "exit.h"
#include "proprintf.h"

extern char	*nlo_get_server_port();
extern char	*get_internal_version();

main(argc, argv)
    int	argc;
    char	*argv[];
{
    char	server_host[K_NAME_SIZE];
#if (PRO_MACHINE == IA64_NT) || (PRO_MACHINE == X86E_WIN64)
    SOCKET	chan; 
#else
    int         chan;
#endif
    int conn_status, status;
    IPCmessage	request, response;
    
    set_host_in_list(TRUE); /* even if not in nloservers.txt */
    nlo_set_timeout_multiplier(10); /* for slow networks */
    if (argc > 1)		/* Pro/Server hostname specified */
    {
        if (strcmp(argv[1], "-verify") == 0)
        {
            printf("proshutdown %s\n", get_internal_version());
            exit (0);
        }
        strcpy(server_host, argv[1]);
    }
    else
        pro_get_hostname(server_host, K_NAME_SIZE);
    
    /* Connect to server */
    status = ipc_connect2server(server_host, nlo_get_server_port(server_host),
                                &chan, &conn_status);
    if (status != 1)
    {
        pro_printf("Cannot reach Pro/Server on host %ws.\n", wstr_ret(server_host));
        exit(EXIT_ERROR);
    }
    
    switch (conn_status)
    {
    case MSG_CONFIRM:
        /* Fill in request structure */
        ipc_init_request(&request);
        
        if (nlo_get_connected_server())
            strcpy(request.action, SHUTDOWN);
        else
            request.command  = ITL_SHUTDOWN;
        
        strcpy(request.recv_host, server_host);
        strcpy(request.serial_num, WILDCARD_SERIAL);
        
        
        /* Send the request and get a response and disconnect */
        status = ipc_exchange_messages ( chan, &request, &response, nlo_get_connected_server() );
        
        
        if (status == -1)
        {
            pro_printf("Cannot reach Pro/Server on host %ws.\n", wstr_ret(server_host));
            exit(EXIT_ERROR);
        }
        else if ( status == 1)
        {
            pro_printf("Pro/Server on %ws has been shut down",
                       wstr_ret(server_host));
#if OPER_SYS ==  UNIX_OS
            pro_printf("; restart with 'prostartserver'");
#endif
            pro_printf(".\n");                
        }
        else
            pro_printf("Insufficient privilege to shut down Pro/Server on host %ws.\n",
                       wstr_ret(server_host));
        break;
        
    default:
        break;
    }	/* switch (conn_status) */
    
    exit(EXIT_SUCCESS);
}
