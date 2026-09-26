
/************************************************************************


 03-Sep-96 H-01-07 EEB/MRC $$1  Moved code here from nloserver.c
 17-Sep-96 H-01-09 EEB/MRC $$2  Sleep on startup if SERVER_TYPE_PUB
 24-Sep-96 H-01-10 EEB/MRC $$3  Allow -deere timeout value.
 01-Oct-96 H-01-11 EEB/MRC $$4  Allow -zzz timeout value.
 24-Jan-97 H-01-25 EEB/MRC $$5  Allow NT compiled code to run under WIN 95
 14-Jul-00 J-01-14 JPE     $$6  WIN64 Port
 11-May-04 K-03-03 ASRS    $$7  X86E_WIN64 support
 24-Apr-06 L-01-07 TWH     $$8  use windows unicode wrappers
 16-May-08 L-03-09 BI      $$9  Used syswindows.h instead of windows.h
 18-Jun-20 P-80-08 MVK     $$10 Build Fix for C4211 error

************************************************************************/

#include <hardware.h>

#if OPER_SYS == WINDOWS_32
#include <syswindows.h>
#include <stdlib.h>
#endif

#include "sysstdlib.h"
#include "sysstdio.h"
#include "systime.h"
#include "nlo.h"
#include "nloproc_iproto.h"
#include "nloproc_proto.h"
#include "proipc.h"
#include "pro_string.h"
#include "xarray.h"
#include "exit.h"      /* for EXIT_FATAL, EXIT_SUCCESS */
#include "proprintf.h"

#define SLEEPYTIME_OUT_MIN 20 /* minutes */

/************************************************************************/    
/************************************************************************/ 
/******************                            **************************/    
/*****************      shared static data      *************************/    
/******************                            **************************/    
/************************************************************************/ 
/************************************************************************/   
static char  itlicense_buffer[200];

static int nlo_server_type = SERVER_TYPE_NLO;

#if OPER_SYS == WINDOWS_32

static HANDLE                  hNLOServerQuit = NULL;
static HANDLE                  hNLOGlobalMutex = NULL;


#endif

void set_nlo_server_type(int t)
{
    nlo_server_type = t;
}

int get_nlo_server_type()
{
    return(nlo_server_type);
}



/************************************************************************/    
/************************************************************************/ 

static void proserver_process_args (int argc, char** argv, int *timeout)
{
    int   timeout_arg;
    int   sleepytime_out_requested = FALSE;
    
    nlo_set_server_exec_path(argv[0]);

    if (argc > 1)
    {
        if (strcmp(argv[1], "-verify") == 0)
        {
            printf("Pro/Server %s\n", get_internal_version());
            exit(0);
        }
        
        if (strcmp(argv[1], "-deere") == 0 || strcmp(argv[1], "-zzz") == 0)
        {
            /* Use SLEEPYTIME_OUT_MIN instead of MIN_TIMEOUT as the       */
            /* minimum timeout value.  The -deere or -zzz (sleepytime_out)*/
            /* argument has been provided as a special                    */
            /* (secret/undocumented) consideration for some customers that*/
            /* Marketing got pressure from after changing min timeout from*/
            /* 20 minutes to two hours.  This special value is the old    */
            /* minimum, 20 minutes.                                       */
            /* (get it? these customers fall asleep (zzz) a lot, and rely */
            /* on this timeout to release licenses.)                      */

            sleepytime_out_requested = TRUE;
            if (argc == 2)
            {
                /* use min as default value */
                timeout_arg = SLEEPYTIME_OUT_MIN;
            }
            else
            {
                timeout_arg = atoi(argv[2]);        /* expiration period in minutes */
            }
            if (timeout_arg >= SLEEPYTIME_OUT_MIN && timeout_arg <= MAX_TIMEOUT)
                *timeout = timeout_arg * 60;
            else if (timeout_arg < SLEEPYTIME_OUT_MIN)
                *timeout = SLEEPYTIME_OUT_MIN * 60;
            else if (timeout_arg > MAX_TIMEOUT)
                *timeout = MAX_TIMEOUT * 60;
        }
        else if (strcmp(argv[1], "-timeouttest") == 0)
        {
            *timeout = 2 * 60; /* two minutes */
        }
        else  /* normal user-configurable timeout value */
        {
            timeout_arg = atoi(argv[1]);        /* expiration period in minutes */
            if (timeout_arg >= MIN_TIMEOUT && timeout_arg <= MAX_TIMEOUT)
                *timeout = timeout_arg * 60;
            else if (timeout_arg < MIN_TIMEOUT)
                *timeout = MIN_TIMEOUT * 60;
            else if (timeout_arg > MAX_TIMEOUT)
                *timeout = MAX_TIMEOUT * 60;
        }
        
        if ( (  argc > 2 && ! sleepytime_out_requested  )
            ||( argc > 3 && sleepytime_out_requested))
        {
            toggle_nlo_debug_mode();            /* turn on printing of debug info */
            set_run_mode(atoi(argv[2]));
        }
    }
    
} /* proserver_process_args */

/************************************************************************/    
/************************************************************************/ 

static int  proserver_init (int argc, char** argv)
{
    int   timeout = DEF_TIMEOUT * 60;     /* seconds */
    char  buffer[MAX_NLOLOG_LINE];

    /* check input args -- set verify, timeout, and debug */

    proserver_process_args (argc, argv, &timeout);

    /* have nlo read in the license_tables */
    
    if ( !itnlo_init_licenses() ) 
        return FALSE;
    
    /* initialize data structures */
    
    server_tbl_init ();
    message_queues_init ();

    if (get_nlo_server_type() == SERVER_TYPE_PUB)
    {
        /* sleep so that devious users won't continually shutdown and */
        /* restart the server as a means of freeing licenses */
        sleep(PUB_SERVER_TIMEOUT); 

        initialize_timeouts(PUB_SERVER_TIMEOUT);
    }    
    else
    {
        initialize_timeouts(timeout);
    }

    /* now try to open communication channels */
    
    if ( !itrpc_open_communication_channels() )
        return FALSE;
    
    sprintf(buffer, "0 %s", SERVER_ON_STR); 
    nlo_write_usage((long int)0, buffer);
    
    return TRUE;
    
} /* initialize_server */

/******************************************************************************/
/******************************************************************************/

int  process_oldmsg_newprotocol ( request, response, oldaction )
    IPCmessage  *request, *response;
    char        oldaction[K_NAME_SIZE];
{
    int    status;
    
    
    response->question_id        = request->external_id;
    strcpy (response->caller_port, request->caller_port);
    
    /* strcpy (request->action, oldaction);        /* temporarily mark as old protocol */
    status = nlo_process_request (request, response);
    /* strcpy (response->action, IPC_VERSION);     /* reinstate  marker for new protocol */
    
    /* old protocol was to return "success" or "failure" in response.action ...
     * new protocol is to return status via response.command.
     */
    response->command = (status ? SERVER_AFFIRMATIVE_RESPONSE : SERVER_NEGATIVE_RESPONSE );
    
    /* send out the message */
    if (! itrpc_send_response_to_client ( response ))
    { /* msg never acknowledged! */
    }
    
    return (status);  /* && msg acknowledged ?? */
    
}  /* process_oldmsg_newprotocol */

/******************************************************************************/
/******************************************************************************/
/* process_command -- this is the main dispatcher for messages
 */

void  process_command ( request )
    IPCmessage  *request;
{
    command_s   command;
    static      IPCmessage  response, saved_request;
    
    message_static_init (&saved_request);
    message_copy(request, &saved_request);

    message_static_init ( &response );
    ipc_init_response ( request, &response );
    
    command = command_of( request );
    
    if ( is_server_command ( command )) /* register partner statuses */
        set_partner_status ( request );
    
    if (   ( request->command != ITL_NO_COMMAND )
        && ( nlo_debug_mode() )
        )
    {
        sprintf(itlicense_buffer, "%s request received from user %s at host %s %s\n",
                command_decode (request),
                request->user,
                request->send_host,
                request->caller_port);
        nlo_write_log(itlicense_buffer);
    }
    
    switch (command)
    {
      case  ITL_NO_COMMAND : break; /* do nothing */
        
        /*************************** (oldmsg requests from a Client) ***************************/
      case  ITL_STATUS :
          process_oldmsg_newprotocol ( request, &response, STATUS );
        break;
        
      case  ITL_OPTIONS :
          process_oldmsg_newprotocol ( request, &response, OPTIONS );
        break;
        
      case  ITL_VERSION :
          process_oldmsg_newprotocol ( request, &response, VERSION );
        break;
        
      case  ITL_FREE_OPTIONS :
          process_oldmsg_newprotocol ( request, &response, FREE_OPTIONS );
        break;
        
      case  ITL_DEBUG :
          process_oldmsg_newprotocol ( request, &response, DEBUG );
        break;
        
      case  ITL_SHUTDOWN :
          process_oldmsg_newprotocol ( request, &response, SHUTDOWN );
        break;
        
      case  ITL_FLUSH_STAT :
          process_client_flush_table ( request );
        break;
        
      case  ITL_TRIM_LOG :
          process_oldmsg_newprotocol ( request, &response, TRIM_LOG );
        break;
        
        
        /******** ( requests from license monitor):  *********************/
        
      case MONITOR_CLEAR  :
          process_monitor_clear( request, &response );
        break;
        
      case MONITOR_COMPOSE  :
          process_client_send_usage(request);
        break;
#if 0        
      case MONITOR_SNAPSHOT  :
          process_monitor_snapshot( request, &response );
        break;
#endif        
      case MONITOR_USAGE_ON  :
          process_monitor_usage_on( request, &response );
        break;
        
      case MONITOR_USAGE_OFF  :
          process_monitor_usage_off( request, &response );
        break;
        
        /*************************** (newmsg requests from a Client) ***************************/
        
      case  CLIENT_RESERVE_LICENSE :
          process_client_add_license ( request, &response );          
        break;
        
      case  CLIENT_RELEASE_LICENSE :
          process_client_release_license ( request );          
        break;
        
      case  CLIENT_SEND_TABLE :
          process_client_send_table ( request );          
        break;
        
      case  CLIENT_SEND_PARTNER_STATUS :
          process_client_send_server_status ( request );          
        break;
        
      case  CLIENT_SEND_LOCATION :
          process_client_send_location ( request );          
        break;

      case  CLIENT_DEBUG :
          process_client_debug ();          
        break;
        /************************ (requests from another Server): ***********************/
        
      case  SERVER_ACKNOWLEDGES_MSG : /* ignore for now */
          break;
        
      case  SERVER_NEGATIVE_RESPONSE :
      case  SERVER_AFFIRMATIVE_RESPONSE :
            process_server_yesno_response ( request );          
        break;
        
      case  SERVER_RESERVE_LICENSE :
      case  SERVER_RESERVED_LICENSE :
            process_server_add_license ( request, &response );          
        break;
        
      case  SERVER_RELEASE_LICENSE :
      case  SERVER_RELEASED_LICENSE :
            process_server_release_license ( request );          
        break;
        
      case  SERVER_REFRESH_LICENSE :
      case  SERVER_REFRESHED_LICENSE :
            process_server_update_license ( request, &response );          
        break;
        
      case  SERVER_DEBUG :
          process_server_debug ();          
        break;
        
      case  SERVER_SEND_TABLE_RESPONSE :
          process_server_send_table_response ( request );          
        break;
        
      case  SERVER_SEND_TABLE_REQUEST :
      case  SERVER_SENT_TABLE :
            process_server_send_table_request ( request );          
        break;
        
      case  SERVER_FLUSH_TABLE :
      case  SERVER_FLUSHED_TABLE :
            process_server_flush_table ( request );          
        break;
        
      default :
          /* must be an old-protocol command */
          break;
        
    }  /* end of switch current request */
    
    
    /* if a license was taken or freed, update status file */
    nlo_update_status_file ();
    
    
} /* process_command */
  
/****************************************************************************/
/****************************************************************************/
/* ok_to_process -- returns TRUE if the given message can be processed now;
 * FALSE if it must wait (for some response to a pending question, perhaps)
 */
 
int  ok_to_process ( msg_q )
 void  *msg_q;   /* message_qhead_rec */
{
      
     /*******************************************
        this is experimental stuff
      *******************************************/
 
 /*  message = (IPCmessage *)peek_at_current_message ( msg_q );             */
 
 /*  return (     ( is_response ( message ) )   /* message is a response to a question */
 /*           ||  ( !questionq_walk_curr() )    /* no pending questions */
 /*           || !( /* here is the list of requests that must wait */
 /*                  (command_of (message) == SERVER_RESERVE_LICENSE)       */
 /*                ||(command_of (message) == SERVER_SEND_TABLE_REQUEST)    */
 /*             /* ||(command_of (message) == CLIENT_RESERVE_LICENSE) */
 /*               )                                                         */
 /*         );  
                                                             */
 return (TRUE); 
         
}  /* ok_to_process */
 
/************************************************************************/    
/************************************************************************/ 
/* something_in_pipe -- returns a message from the channel if there is one;
 *                      returns TRUE if message found, otherwise FALSE.
 */
 
int  something_in_pipe ( request, dont_wait, alarm_time )
    IPCmessage *request;
    int     dont_wait;
    long       alarm_time;
{
    static IPCmessage response; /* never needs disposing */
#if (PRO_MACHINE == IA64_NT) || (PRO_MACHINE == X86E_WIN64)
    SOCKET    msg_chan;
#else
    int       msg_chan;
#endif
    char (*readonly_supp_info)[MAX_SUBMSG_SIZE];
    int    msg_status;
    int    success;
    struct timeval timeout;
    int    alarm_to_go;
    static int    assurance_dots = 0; 
    
#if 0
    if (ipc_get_debug_mode())
    { 
        pro_printf ("*");
        if (assurance_dots++ == 80)
        {
            assurance_dots = 0;
            pro_printf ("\n");
        }
    }
#endif
    
    /* when should alarm go off? */
    
    if (dont_wait)
        timeout.tv_sec = 0;
    else
    {
        alarm_to_go = alarm_time - time(0);
        timeout.tv_sec = ((alarm_to_go < 1) ? 1 : alarm_to_go);
    }
    timeout.tv_usec  = 0;
    
    message_clear ( request );
    
    /* peek into the socket... */
    if ( !check_for_incoming_message ( &timeout, &msg_chan, &msg_status ) )    
        /* if nothing's there, return emptyhanded */
        return (FALSE);
    
    else
    {
        /* otherwise, something IS there --
         * if it's the old protocol, fill the old message_rec; execute it now.
         * if it's the new protocol, fill the new message rec, and return it.
         */
        
        if (msg_status == MSG_CONNECT)
        {
            if (nlo_debug_mode())
                nlo_write_log ("MSG_CONNECT -----------------------------------------------\n");
            
            success = connect_to_client(&msg_chan);
            if (!success)
            {
                if (nlo_debug_mode())
                    nlo_write_log ("COULDN'T CONNECT\n");
                return (FALSE);  /* couldn't connect */
            }
            
            
#if OPER_SYS == WINDOWS_32
            /*
             * If the quit event is set then we know that we will never get access to
             * hGlobalMutex, so disconnect from the client, and finish.
             */
            if ( WaitForSingleObject(hNLOServerQuit,0) == WAIT_OBJECT_0)
            {
                disconnect_from_client(msg_chan);
                return (FALSE);
            }
            
            /*
             * Wait for the Mutex, before proceding. dont bother checking the result, since
             * it won't help us to know if it was ABONDONED.
             */
            WaitForSingleObject(hNLOGlobalMutex,INFINITE);
#endif            
            
            success = ipc_get_message(msg_chan, request);
            if (!success)     /* wait a bit and try again */
            {
                sleep(1);
                success = ipc_get_message(msg_chan, request);
            }
            
            
            if (success)  /* got a message ! */
            {
                if (nlo_debug_mode())
                {
                    sprintf (itlicense_buffer, "Get COMMAND: %s", command_decode(request));
                    nlo_write_log(itlicense_buffer);
                    sprintf (itlicense_buffer, " from %s %s", request->send_host, request->caller_port);
                    nlo_write_log(itlicense_buffer);
                }
                
                readonly_supp_info          = request->supp_info;
                xar_cpy (&readonly_supp_info, &request->supp_info);
                
                ipc_init_response(request, &response);
                
                /* if (request->command == ITL_NO_COMMAND) */
                if (strcmp(request->action, IPC_VERSION) == 0) /* call-back protocol */
                {  /* acknowledge request receipt */
                    strcpy(response.action, IPC_VERSION);   /* mark message as new version */
                    response.command     = SERVER_ACKNOWLEDGES_MSG;  /* serves as acknowledgement */
                    response.question_id = request->message_id;
                    ipc_put_message(msg_chan, &response);
                }
                else /* single-connection protocol */
                {
                    /* map the old action string to the new command scalar */
                    request->command = command_map (request->action);
                    
                    if (nlo_debug_mode())
                    {
                        sprintf (itlicense_buffer, "... old_style message: %s command: %d\n",
                                 request->action, request->command);
                        nlo_write_log(itlicense_buffer);
                    }
                    
                    success = nlo_validate_request(request, &response);
                    
                    if (!success)
                    {
                        nlo_invalid_response(request, &response);
                        ipc_put_message(msg_chan, &response);
                        disconnect_from_client(msg_chan);
                    }
                    else /* got a valid client request */
                    {
                        if ( shutdown_now() )
                        {
                            disconnect_from_client(msg_chan);
                            sprintf(itlicense_buffer, "Pro/Server version %d shutdown", NLO_SERVER_VERSION);
                            nlo_write_log(itlicense_buffer);
                            sprintf(itlicense_buffer, "0 %s", SERVER_OFF_STR); 
                            nlo_write_usage((long int)0, itlicense_buffer);
                            exit(EXIT_SUCCESS);
                        }
                        
                    }
                    return(success);
                }
            }
            else if (nlo_debug_mode())
                nlo_write_log ("NEVER RECEIVED MESSAGE !!!!\n");
            
#if OPER_SYS != VMS
            success = (disconnect_from_client(msg_chan) && success);   /* disconnect VMS only when requested */
#endif
        }
        else /* connection failed */
        {
            if (nlo_debug_mode())
                nlo_write_log ("Connection failed");
            
            success = disconnect_from_client(msg_chan);
            return (FALSE);
        }
        
        if ( shutdown_now() )
        {
            sprintf(itlicense_buffer, "Pro/Server version %d shutdown", NLO_SERVER_VERSION);
            nlo_write_log(itlicense_buffer);
            sprintf(itlicense_buffer, "0 %s", SERVER_OFF_STR); 
            nlo_write_usage((long int)0, itlicense_buffer);
#if OPER_SYS == WINDOWS_32
            /* Grab the mutex, and signal the exit event. */
            WaitForSingleObject(hNLOGlobalMutex,INFINITE);
            SetEvent(hNLOServerQuit);
            return (FALSE);
#else
            exit(EXIT_SUCCESS);
#endif
        }
        
        return (success);
    }
}  /* something_in_pipe */

/***************************************************************************/
/***************************************************************************/
/* any_input -- returns TRUE (and the message) if there is any pending input
 */
 
int  any_input ( msg_q )
 void  **msg_q;  /* message_qhead_rec */
{
 IPCmessage  message;
 int      dont_wait = TRUE;
 
   /*  This routine pulls everything pending in the input pipe
    *  and adds it to one of two virtual queues,
    *  the "message" queue, and the "response" queue.
    *  This enables responses to get a higher priority over messages.
    *  
    *  Then "something_in_queue" returns the "next" thing, which is:
    *  (first priority) the first response, if there is one queued, or
    *  (second priority) the next message, if there is one queued.
    */
    
   dont_wait = something_in_queue ( msg_q );
      
   while (something_in_pipe ( &message, dont_wait, nlo_next_input_alarm() ))
   { /* move input to queue */
      if ( message.command == ITL_NO_COMMAND)
          continue;
          
      message.external_id = message.message_id;   /* save for replies */
      message.message_id  = new_message_id();     /* for my sanity */
   
      if ( is_response ( &message ) )
      {
         qpush_response ( &message );
         messageq_walk_init();        /* always start over, after processing a response */
      }
      else 
         qpush_message ( &message );
         
      dont_wait = TRUE; /* we KNOW there's something in the queue now */
      
   } /* move input to queue */
   
   return ( something_in_queue ( msg_q ));
}  /* any_input */ 
 
/******************************************************************************/
/******************************************************************************/
/* server_driver -- this is the main command loop
 */
 
void  server_driver ( void )
{
    void        *msg_q;  /* message_qhead_ptr */
    IPCmessage  message;
    
    for (;;)  /* loop forever */
    {
        if ( any_input ( &msg_q )   /* may set shutdown_now */
            && (!shutdown_now())
            )
        {        
            if ( ok_to_process ( msg_q ) )
            {
                qpop ( msg_q, &message );       /* grab the next message */
                
                /* the primary command dispatcher */
                process_command ( &message );   /* may set shutdown_now */
                
                message_dispose ( &message );
            }
            else  /* look past this message, for a no-brainer */
                
                messageq_walk_next ();
        }
#if OPER_SYS == WINDOWS_32
        else if ( WaitForSingleObject(hNLOServerQuit,0) == WAIT_OBJECT_0)
            /*
             * If the quit event is set then we know that we will never get access to
             * hGlobalMutex, so disconnect from the client, and finish.
             */
            
            return;
#endif
        else if (!shutdown_now())
            process_timeouts ( );
        
    
        if ( shutdown_now() )
        {
            sprintf(itlicense_buffer, "Pro/Server version %d shutdown", NLO_SERVER_VERSION);
            nlo_write_log(itlicense_buffer);
            sprintf(itlicense_buffer, "0 %s", SERVER_OFF_STR); 
            nlo_write_usage((long int)0, itlicense_buffer);
#if OPER_SYS == WINDOWS_32
            /* Grab the mutex, and signal the exit event. */
            WaitForSingleObject(hNLOGlobalMutex,INFINITE);
            SetEvent(hNLOServerQuit);
#else
            exit(EXIT_SUCCESS);
#endif
        }
    }  /* loop forever */
}  /* server_driver */
 
/******************************************************************************/
/******************************************************************************/

#if OPER_SYS == WINDOWS_32

void proserver_main_loop (void)
{
   
    send_request_for_license_table ();
    
    /* response will come back thru driver loop as SERVER_SEND_TABLE_RESPONSE */
    
    server_driver ();
}

#else

void nlo_server_main (int argc, char *argv[])
{
    
    if (!proserver_init(argc, argv))
        return;
    
    send_request_for_license_table ();
    
    /* response will come back thru driver loop as SERVER_SEND_TABLE_RESPONSE */
    
    server_driver ();

}

#endif 

/******************************************************************************/
/******************************************************************************/


#if OPER_SYS == WINDOWS_32


/*
 * End of "original service code, the rest is the specific "service" code which runs the show under
 * Windows NT.
 */

typedef struct {
    int argc;
    char **argv;
} ArgStruct;


static VOID worker_thread(ArgStruct *args);

SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwGlobalErr;
DWORD                   TID = 0;
HANDLE                  threadHandle = NULL;
HANDLE                  pipeHandle;


//  declare the service threads:
//



//  nlo_server_main() --
//      all nlo_server_main does is call StartServiceCtrlDispatcher
//      to register the main service thread.  When the
//      API returns, the service has stopped, so exit.
//

VOID nlo_server_main(int argc, char *argv[])
{
    SERVICE_TABLE_ENTRY dispatchTable[] = {
        { TEXT("Pro/SERVER"), (LPSERVICE_MAIN_FUNCTION)pro_server_startup },
        { NULL, NULL }
    };


    if (!StartServiceCtrlDispatcher(dispatchTable)) {
        ProServerServiceStop();/*("StartServiceCtrlDispatcher failed.");*/
    }
}



//  service_main() --
//      this function takes care of actually starting the service,
//      informing the service controller at each step along the way.
//      After launching the worker thread, it waits on the event
//      that the worker thread will signal at its termination.
//
VOID
pro_server_startup(DWORD dwArgc, char **lpszArgv)
{
    DWORD                   dwWait;
    PSECURITY_DESCRIPTOR    pSD;
    SECURITY_ATTRIBUTES     sa;
    ArgStruct args;
    
    // register our service control handler:
    //
        
    sshStatusHandle = uRegisterServiceCtrlHandler(
                                                 "Pro/SERVER",
                                                 service_ctrl);
    if (!sshStatusHandle)
        goto cleanup;

    // SERVICE_STATUS members that do not change in example
    //
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;


    // report the status to Service Control Manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING,
        NO_ERROR,
        1,
        3000))
        goto cleanup;

/* create the event object. The control handler function signals
 * this event when it receives the "stop" control code.
*/
    hNLOServerQuit = uCreateEvent(
        NULL,    // no security attributes
        TRUE,    // manual reset event
        FALSE,   // not-signalled
        NULL);   // no name

    if (hNLOServerQuit == (HANDLE)NULL)
        goto cleanup;

    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING,
        NO_ERROR,
        2,
        3000))
        goto cleanup;


    args.argc = dwArgc;
    args.argv = lpszArgv;

    // start the thread that performs the work of the service.
    //
    threadHandle = CreateThread(
                    NULL,       // security attributes
                    0,          // stack size (0 means inherit parents stack size)
                    (LPTHREAD_START_ROUTINE)worker_thread,
                    &args,       // argument to thread
                    0,          // thread creation flags
                    &TID);      // pointer to thread ID

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
        hNLOServerQuit,
        INFINITE);

cleanup:

    if (hNLOServerQuit != NULL)
        CloseHandle(hNLOServerQuit);
    if (hNLOGlobalMutex != NULL)
        CloseHandle(hNLOServerQuit);
    sprintf(itlicense_buffer, "Pro/Server version %d shutdown", NLO_SERVER_VERSION);
    nlo_write_log(itlicense_buffer);
    ProServerServiceMsg(itlicense_buffer);


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



/*
 *  service_ctrl() --
 *      this function is called by the Service Controller whenever
 *      someone calls ControlService in reference to our service.
 */
VOID WINAPI service_ctrl(DWORD dwCtrlCode)
{
    DWORD  dwState = SERVICE_RUNNING;
    DWORD  mut_state;


/*
 * Handle the requested control code.
 */

    //
    switch(dwCtrlCode) {

/*
 * Leave the code for SERVICE_CONTROL_PAUSE, and SERVICE_CONTROL_CONTINUE in,
 * in case we need it in the future - It will never be called, because we dont
 * specify to the service control manager that we will accept these controls.
 */        case SERVICE_CONTROL_PAUSE:

            if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
                SuspendThread(threadHandle);
                dwState = SERVICE_PAUSED;
            }
            break;

        case SERVICE_CONTROL_CONTINUE:

            if (ssStatus.dwCurrentState == SERVICE_PAUSED) {
                ResumeThread(threadHandle);
                dwState = SERVICE_RUNNING;
            }
            break;

        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:

            dwState = SERVICE_STOP_PENDING;

            // Report the status, specifying the checkpoint and waithint,
            //  before setting the termination event.
            //
            ReportStatusToSCMgr(
                    SERVICE_STOP_PENDING, // current state
                    NO_ERROR,             // exit code
                    1,                    // checkpoint
                    3000);                // waithint

/*
 * Wait for the current request to finish (if there is one.)
 */
            mut_state = WaitForSingleObject(hNLOGlobalMutex,INFINITE);

/*
 * Find out if we really got access to the mutex, or if in fact the nlo thread just
 * died. ( EG somebody ran proshutdown. )
 */
            switch (mut_state) {
            case WAIT_OBJECT_0:
            case WAIT_ABANDONED:
              break;
            default:
              break;
              /* should never get here */
            }

            /* NOW procede to shutdown our application.
               Note that we never release the mutex, since from now on, we dont want any
               processing*/
            SetEvent(hNLOServerQuit);

        // Update the service status.
        //      104
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







//  worker_thread() --
//      this function does the actual nuts and bolts work that
//      the service requires.  It will also Pause or Stop when
//      asked by the service_ctrl function.
//
static VOID
worker_thread(ArgStruct *args)
{
  if (!proserver_init(args->argc, args->argv))
    ProServerServiceStop();
  else
    proserver_main_loop();
}



// utility functions...



// ReportStatusToSCMgr() --
//      This function is called by the ServMainFunc() and
//      ServCtrlHandler() functions to update the services status
//      to the service control manager.
//
BOOL
ReportStatusToSCMgr(DWORD dwCurrentState,
                    DWORD dwWin32ExitCode,
                    DWORD dwCheckPoint,
                    DWORD dwWaitHint)
{
    BOOL fResult;


    // Disable control requests until the service is started.
    //
    if (dwCurrentState == SERVICE_START_PENDING)
        ssStatus.dwControlsAccepted = 0;
    else
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;

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
        ProServerServiceStop();/*("Could not set service status");*/
    }

    return fResult;
}



void ProServerServiceStop(void)
{
    SetEvent(hNLOServerQuit);
}

void ProServerServiceMsg(char * lpszMsg)
{
  HANDLE  hEventSource;
  char *  lpszStrings[2];


/*
 * Log an error because this is the NT was to do things.
 */
  hEventSource = uRegisterEventSource(NULL,
                            "Pro/SERVER");

  lpszStrings[0] = lpszMsg;

  if (hEventSource != NULL)
  {
    uReportEvent(hEventSource,EVENTLOG_ERROR_TYPE,0,0,NULL,1,0,lpszStrings,NULL);
    (void) DeregisterEventSource(hEventSource);
  }
}

#endif /* OPER_SYS == WINDOWS_32 */


#undef  SLEEPYTIME_OUT_MIN
