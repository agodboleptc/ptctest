/*---------------------------------------------------------------------*/
/*

pm_query.c

 21-Jun-95 e-06-42 rsc  $$1  created
 28-Jul-95 g-01-02 rsc  $$2  copied into G-01
17-Aug-95 G-01-03 jmichaud $$3 Use XAR_COUNT() and BYTCPY() macros.
10-Feb-96 ?-??-?? anatoli  $$4 Use "real" portmap functions
17-Apr-96 G-03-11 swv/pls  $$5 Use INADDR_ANY on Windows 95 and not
                           the localhost name when attempting to bind
                           to PMAPPORT.
17-Apr-96 G-03-11 swv/pls  $$6 Force pm_is_our_portmapper_running to return
                              false on Windows 95
14-Jul-00 J-01-14 JPE      $$7 WIN64 Port
21-Sep-01 J-03-09 jas      $$8 Removed WINDOWS_95 macro
23-May-05 K-03-25 ksi      $$9 Added prog arg to real_pmap_getport call
12-Jul-06 L-01-12 ksi      $$10 Unicode compliant changes
08-Dec-08 L-03-23 BI       $$11 IPv6 code changes
12-Mar-12 P-20-01 AC       $$12 Updated for Project 13028358
11-Nov-13 P-20-42 seer     $$13 Fixed for windows portmapper
13-Jul-20 P-80-12 KSV      $$14 Removed sandbox leftovers
02-Sep-20 P-80-20 Ahmad    $$15 Added forced loopback.
*/
/*---------------------------------------------------------------------*/


#include <btkcstdio.h>
#include "hardware.h"
#include "sysstdio.h"
#include "systypes.h"
#include "rpc_tools.h"
#include <baselibapi.h>
#include <btknetaddr.h>
#include <loopback_funcs.h>
#if OPER_SYS == WINDOWS_32

#include "sunrpc.h"
#include "sr_pmap_prot.h"
#include "sr_pmap_clnt.h"
#include "pro_string.h"
#include <const.h>
#include <ctsyscall_proto.h>
#include <w32chksysparms.h>
#include <ct_win_syscall_proto.h>

#define GET_LAST_ERROR WSAGetLastError()
extern struct addrinfo *pro_gethostbyname();
extern bool_t local_portmapper_set_is_windows (bool_t);

/*****************************************************************
 * FUNCTION: pm_is_our_portmapper_running
 * DESCRIPTION:
 *	finds out if PTC portmapper is running (on NT)
 *
 * RETURNS:
 *	TRUE if portmapper is running, FALSE otherwise (on NT)
 *      FALSE on Windows 95
 ****************************************************************/

bool_t pm_is_our_portmapper_running()
{
    return (ginst_is_service_running("ProPortmap Service"));
}




/*****************************************************************
 * FUNCTION: pm_is_portmapper_port_busy
 * DESCRIPTION:
 *	finds out if portmapper port is busy, by attempting to bind to
 *	it
 *
 * RETURNS:
 *	TRUE if port is busy, FALSE otherwise
 *
 * NOTE:
 ****************************************************************/

bool_t pm_is_portmapper_port_busy()
{
    SOCKET		our_socket;
    struct addrinfo	*addr = NULL;
    int			last_error, RetVal;
    bool_t		result;
    char		*hostname;
    static bool_t	startup_done = FALSE;
    struct addrinfo     *SAddrinfo, Hints;
    char                Port[PORTSIZE];
    struct sockaddr_in6 sa;
    size_t sa_size;


    memset(&Hints, 0, sizeof(Hints)); 
    Hints.ai_family = ptc_family;
    Hints.ai_socktype = SOCK_DGRAM;
    Hints.ai_flags = AI_PASSIVE;
    Hints.ai_protocol = IPPROTO_UDP;

    btk_sprintf(Port,"%d",PMAPPORT);
    /* make sure socket communications are initialized */
    init_communications_for_system();

    /* open a socket for testing */

    if (btkLoopBackForced())
    {
        btkSockAddrInitLoopback(&sa, AF_UNSPEC, PMAPPORT);

        if ((our_socket = socket(sa.sin6_family, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
        {
            btk_fprintf(btk_get_stderr(), "pm_is_portmapper_port_busy: socket() failed, error = %d\n",
                GET_LAST_ERROR);
            return (FALSE);
        }
        sa_size = SOADDR_ADDRLEN(&sa);
        if (loopback_bind(our_socket, SOADDR_ADDRESS(&sa), sa_size) < 0) {
            if ((last_error = GET_LAST_ERROR) == SOCKERROR(EADDRINUSE)) {
                /* it was busy */
                result = TRUE;
            }
            else {
                /* we can't tell, return true for now */
                btk_fprintf(btk_get_stderr(), "pm_is_portmapper_port_busy: bind failed, error = %d\n",
                    last_error);
                result = TRUE;
            }
        }
        else {
            /*
             * we were able to bind to it, so it wasn't busy. At some
             * point we might want to add some more sophisticated test
             * here to see if we can actually connect using that port
             */
            result = FALSE;
        }

    }
    else
    {
        hostname = rpc_host_name();

        RetVal = btk_get_nodeaddrinfo(hostname, Port, &Hints, &SAddrinfo);
    

        if((our_socket = socket (SAddrinfo->ai_family, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
        {
            btk_fprintf(btk_get_stderr(), "pm_is_portmapper_port_busy: socket() failed, error = %d\n",
		    GET_LAST_ERROR);

		    btk_free_nodeaddrinfo(SAddrinfo);
            return (FALSE);
        }

        if (loopback_bind(our_socket, SAddrinfo->ai_addr, SAddrinfo->ai_addrlen) < 0){
            if ((last_error = GET_LAST_ERROR) == SOCKERROR(EADDRINUSE)) {
            /* it was busy */
            result = TRUE;
            }
            else {
            /* we can't tell, return true for now */
                btk_fprintf(btk_get_stderr(), "pm_is_portmapper_port_busy: bind failed, error = %d\n",
	            last_error);
                result = TRUE;
            }
        }
        else {
	    /*
	     * we were able to bind to it, so it wasn't busy. At some
	     * point we might want to add some more sophisticated test
	     * here to see if we can actually connect using that port
	     */
	    result = FALSE;
        }


        btk_free_nodeaddrinfo(SAddrinfo);
    }
    /* close the socket, presumably releasing the port if we bound to it */

    (void) closesocket (our_socket);
    return (result);
}


/*****************************************************************
 * FUNCTION: pm_is_portmapper_a_portmapper
 * DESCRIPTION:
 *	Attempts to determine if server bound to PMAPPORT is in fact a
 *	portmapper, by using it to "register" a dummy client
 *	(pmap_set).
 *
 * RETURNS:
 *	TRUE if server behaves as a portmapper, FALSE otherwise
 *
 * NOTE:
 ****************************************************************/

bool_t pm_is_portmapper_a_portmapper()
{

    SOCKET our_socket;
    struct sockaddr_in6	*sock_in6;
    struct sockaddr_storage *addrStore;
    struct addrinfo	*res, hints;
    char		*hostname;
    short		our_port, ret_port;
    u_long		prog;
    bool_t		result = TRUE;
    bool_t		mapping_happened = FALSE;
    bool_t		bound;
    struct addrinfo *SAddrinfo = NULL;
    int RetVal;
    struct sockaddr_in6 sa;
    size_t sa_size;

	addrStore = NULL;
	res = NULL;
    

    /* make sure socket communications are initialized */
    init_communications_for_system();

    /* if nobody is using the port, it can't be a port mapper */
    if (! pm_is_portmapper_port_busy()) return (FALSE);

    hostname = rpc_host_name();

	bzero(&hints, sizeof (hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_family =ptc_family; 

    if (btkLoopBackForced())
    {
        btkSockAddrInitLoopback(&sa, AF_UNSPEC, 0);

        if ((our_socket = socket(sa.sin6_family, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: could not create socket\n",
                GET_LAST_ERROR);
            return (FALSE);
        }

        /* need to get a port on which to test it */
        our_port = bindresvport(our_socket, &hints);
        if (our_port < 0) {
            btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: bind() failed\n",
                GET_LAST_ERROR);
            (void)closesocket(our_socket);
            return (FALSE);
        }

        prog = PTC_RPCNUM_09;	/* for now, anyway */

        /* map our port to prog */
        /*
         * note: this call takes 60 seconds to time out if portmapper port
         * is held by an unresponsive server; if this is determined to be
         * too long, some kind of local timeout should be used
         */

        if (!real_pmap_set(prog, VERSION_ONE, IPPROTO_TCP, our_port)) {
            btk_fprintf(btk_get_stderr(), "pmap_set failed, error = %d\n", GET_LAST_ERROR);
            result = FALSE;
        }
        else
        {
            mapping_happened = TRUE;

            /* Try to map same program to different port without removing previous one
               This should not succeed. If it does, we are using windows server 2008's portmapper (or one with similar behavior).
               In that case we can't rely on return value of real_pmap_set to determine if prog is already registered,
               we would need to call pmap_getport each time to check */
            if (real_pmap_set(prog, VERSION_ONE, IPPROTO_TCP, our_port + 1)) {
                local_portmapper_set_is_windows(TRUE);
            }

            /* see if putative portmapper returns the same port */
            addrStore = (struct sockaddr_storage*)getmem(sizeof(struct sockaddr_storage));
            sa_size = SOADDR_ADDRLEN(&sa);
            BYTCPY(addrStore, (struct sockaddr_storage*)&sa, sa_size);


            if ((ret_port = real_pmap_getport(addrStore, prog, VERSION_ONE, IPPROTO_TCP)) == 0) {
                btk_fprintf(btk_get_stderr(), "pmap_getport returned 0, error = %d\n", GET_LAST_ERROR);
                result = FALSE;
            }
            else {
                if (ret_port != our_port) {
                    btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: pmap_getport returned %d, our_port = %d\n",
                        ret_port, our_port);
                    result = FALSE;
                }
            }
        }
    }
    else
    {
        RetVal = btk_get_nodeaddrinfo(hostname, NULL, &hints, &res);
   

        if((our_socket = socket (res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET) {
		    btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: could not create socket\n",
			    GET_LAST_ERROR);
		    btk_free_nodeaddrinfo(res);
		    return (FALSE);
        }

	    /* need to get a port on which to test it */
        our_port = bindresvport(our_socket, &hints);
        if( our_port < 0 ) {
		    btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: bind() failed\n",
			    GET_LAST_ERROR);
		    (void) closesocket (our_socket);
		    btk_free_nodeaddrinfo(res);
		    return (FALSE);
        }

        prog = PTC_RPCNUM_09;	/* for now, anyway */

        /* map our port to prog */
        /*
         * note: this call takes 60 seconds to time out if portmapper port
         * is held by an unresponsive server; if this is determined to be
         * too long, some kind of local timeout should be used
         */

        if (! real_pmap_set(prog, VERSION_ONE, IPPROTO_TCP, our_port)) {
		    btk_fprintf(btk_get_stderr(), "pmap_set failed, error = %d\n", GET_LAST_ERROR);
		    result = FALSE;
        }
	    else 
	    {
            mapping_happened = TRUE;

            /* Try to map same program to different port without removing previous one
               This should not succeed. If it does, we are using windows server 2008's portmapper (or one with similar behavior).
		       In that case we can't rely on return value of real_pmap_set to determine if prog is already registered,
               we would need to call pmap_getport each time to check */
            if (real_pmap_set(prog, VERSION_ONE, IPPROTO_TCP, our_port+1)) {
                local_portmapper_set_is_windows(TRUE);
            }
        
            /* see if putative portmapper returns the same port */
            addrStore = (struct sockaddr_storage *)getmem(sizeof(struct sockaddr_storage));
            BYTCPY(addrStore, (struct sockaddr_storage *)res->ai_addr, res->ai_addrlen);

            /*since we are done with res, so free it now*/
            btk_free_nodeaddrinfo(res);
        
	        if ((ret_port = real_pmap_getport(addrStore, prog, VERSION_ONE, IPPROTO_TCP)) == 0) {
		        btk_fprintf(btk_get_stderr(), "pmap_getport returned 0, error = %d\n", GET_LAST_ERROR);
		        result = FALSE;
	        }
	        else {
		        if (ret_port != our_port) {
			        btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: pmap_getport returned %d, our_port = %d\n",
				        ret_port, our_port);
			        result = FALSE;
		        }
	        }
        }
    }

    if (mapping_happened) {
	/* make sure it can unmap as well */
		if (! real_pmap_unset(prog, VERSION_ONE)) {
			btk_fprintf(btk_get_stderr(), "pm_is_portmapper_a_portmapper: pmap_unset failed, error = %d\n",
				GET_LAST_ERROR);
			result = FALSE;
		}
		else {
			if ((ret_port = real_pmap_getport(addrStore, prog, VERSION_ONE, IPPROTO_TCP)) != 0) {
				btk_fprintf(btk_get_stderr(), "last pmap_getport returned %d\n", ret_port);
				result = FALSE;
			}
		}
    }

    if(addrStore != NULL)relmem(&addrStore);
    (void) closesocket (our_socket);
    return (result);
}

#endif /* OPER_SYS == WINDOWS_32 */

