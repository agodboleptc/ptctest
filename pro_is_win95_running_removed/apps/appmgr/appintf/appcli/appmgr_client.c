/*****************************************************************************/
/* File:     appmgr_client.c                                                 */
/* Desc.:    C Functions needed by an application manager client             */
/*                                                                           */
/* 19-Aug-96    H-01-05    GAA   $$1  Created.                               */
/* 19-Aug-96    H-01-05    Amin  $$2  added appmgr_get_rectangle for other   */
/*                                    platforms (!hp, !sgi...)               */
/* 27-Aug-96    H-01-07    GAA   $$3  Replaced named pipes with peer-to-peer */
/*                                    Systems Group IPC                      */
/* 29-Aug-96    H-01-07    PLS   $$4  tuned a bit above                      */
/*                                    init out data pointers to NULL         */
/* 04-Sep-96    H-01-07    GAA   $$5  Include appmgr_client.h for all platfs.*/
/* 05-Sep-96    H-01-08   swv/pls $$6 use make_sunrpc_add to construct       */
/*                                                                am_address */
/* 07-Sep-96    H-01-08    GAA   $$7  Added ibm_r6000 and alpha              */
/* 16-Sep-96    H-01-09    GAA   $$8  appmgr_get_rectangle() returns         */
/*                                    normalized (0.0/1.0) values            */
/* 20-Sep-96    H-01-10    GAA   $$9  More platforms                         */
/* 27-Sep-96    H-01-11    GAA   $$10 Connect only to user's appmgr (not to  */
/*                                    somebody else's appmgr) (disabled until*/
/*                                    supported by IPC Toolkit)              */
/* 10-Oct-96    H-01-13    GAA   $$11 Added appmgr_is_alive()                */
/* 17-Oct-96    H-01-14    GAA   $$12 Fixed appmgr_register_callback()       */
/*                                    parameters for non-supported platforms */
/* 23-Oct-96    H-01-15    GAA   $$13 Activated ##10                         */
/* 28-Oct-96    H-01-15    GAA   $$14 Added PDM - Pro/E client functions     */
/* 11-Nov-96    H-01-16    GAA   $$15 Added dummy functions for ##14         */
/* 15-Nov-96    H-01-17    GAA   $$16 Added workspace functions; added       */
/*                                    appmgr_start_and_wait()                */
/* 02 Dec 96    H-01-19    swv   $$17 added incarnation number logic         */
/* 02-Dec-96    H-01-19    GAA   $$18 Added appmgr_debug(); start appmgr on  */
/*                                    Windows using _spawnv()                */
/* 05-Dec-96    H-01-19+   GAA   $$19 Go back to pro_exec_command() to start */
/*                                    appmgr on Windows; added Hitachi       */
/* 24-Dec-96    H-01-21+   mvb   $$20 appmgr_start_and_wait now calls        */
/*                                    pro_start_background_process if        */
/* 				      WINDOWS_32 else pro_exec_command       */
/* 24-Jan-97    H-01-25    GAA   $$21 Use start_background_proc() for ##20   */
/* 27-Apr-97    H-01-31    GAA   $$22 Call remote_portmapper_responding()    */
/* 29-May-97    H-03-13    mvb   $$23 Use name service for contacting appmgr */
/* 11-Aug-97    H-03-18    PLS   $$24 Use app_service_peer_call              */
/* 28-Sep-97    H-03-24    amin  $$25 Fix cross versioning of appmgr         */
/* 02-Dec-97    H-03-31    jlc   $$26 H-03 appmgr client can't talk to       */
/*                                    H-01/H-02 appmgr                       */
/* 09-Dec-97    H-03-32    jlc   $$27 added appmgr_start_no_wait() and       */
/*                                    appmgr_contact_with_wait()             */
/* 22-DEC-97    H-03-32   Yuri   $$28 Removed PTCtoken                       */
/* 30-Dec-97    H-03-34+   jlc   $$29 added xdrs for setting icon            */
/* 10-Jan-98    H-03-36    jaw   $$30 use new fields in name service         */
/* 13-Jan-98    H-03-37    dmp   $$31 use pro_exec_cmd() to start appmgr     */
/* 26-Feb-98    H-03-39    amin  $$32 Change to use shell for pro_exec_cmd   */
/*                                    on windows 95                          */
/* 16-Jun-98    I-01-11    jlc   $$33 cleanups                               */
/* 16-Jun-98    I-01-13    amin  $$34 Fix send_request_to_appmgr_w_retry     */
/*                                    possible infinite loop                 */
/* 26-Aug-98    I-01-17    prf   $$35 call_script () instead of              */
/*                                    pro_exec_cmd () on Windows 95          */
/* 21-Sep-01    J-03-09    jas   $$36 Removed WINDOWS_95 macro               */
/* 17-Apr-02    J-03-23+   TWH   $$37 Support -laf PTC (new skin )           */
/* 24-Apr-02    J-03-23+   MBE   $$38 Fix ##37 by including bindcall.h.      */
/* 12-Jul-06    L-01-12    ksi   $$39 Unicode compliant changes              */
/* 27-Nov-11    P-10-14    Asaf  $$40 Add appmgr_supported_on_this_platform  */
/* 12-Mar-12    P-20-01    AC    $$41 Updated for Project 13028358           */
/* 17-Nov-20    P-80-30    TTU   $$42 Fixed contact_appmgr()                 */
/* 21-Dec-21    P-90-39    Ahmad $$43 scrambled literal env vars             */
/* 18-Aug-23    Q-11-26    AKA   $$44 Used global ext. call interface instead*/
/*                                    of pro_calling context-dependent labels*/
/*                                                                           */
/* IMPORTANT NOTE: Applications using this API are responsible for calling   */
/*                 comm_layer_initialize() somewhere before the first call   */
/*                 to any of the functions defined here, unless they call    */
/*                 contact_appmgr() with a TRUE argument, in which case      */
/*                 contact_appmgr() will call it itself. Note that it is not */
/*                 necessary to call contact_appmgr() at all. If you don't   */
/*                 call it, appmgr_register_application() will call it,      */
/*                 passing FALSE as argument (that is, it won't initialize   */
/*                 the communication layer). In addition, if applications    */
/*                 intend to service the application manager callbacks,      */
/*                 they must periodically (maybe 3 or 4 times per second)    */
/*                 call appmgr_service_callback(),                           */
/*                 unless they are directly calling app_service_peer_call()  */
/*                                                                           */
/*                 If you do call contact_appmgr(), you only need to do it   */
/*                 once.                                                     */
/*****************************************************************************/

#include <btkcstdio.h>
#include <btkcstdlib.h>
#include <stdlib.h>
#include <btkscale31.h>

#include "pro_hardware.h"
#include "proprintf.h"
#include "prodev_comm.h"
#include "appmgr_client.h"
#include "make_add_proto.h"
#include "sysexecmd.h"

#include <stdio.h>
#include <string.h>

#include "appmgr_ipc_types.h"

#include <appmgr_comm.h>
#include <appmgr_func_ids.h>
#include <const.h>
#include <ctsyscall_proto.h>
#include <sysstdio.h>
#include <debug.h>
#include <ctsyscall_mt_protos.h>
#include <cttime_proto.h>
#include <ctstrutil_proto.h>
#include <ctmisc_proto.h>
#include <ct_win_syscall_proto.h>
#include <global_extcall_interface.h>

static int     appmgr_contacted = 0;
static PeerIdx appmgr_handle;

static int number_applications_registered = 0;

static char auxbuffer[1024];

#define DEBUG 0
#if DEBUG
static FILE* amg_cli_log = btk_get_stdout();
#endif

extern bool_t   remote_portmapper_responding();

static void (*appmgr_callback_function)() = (void(*)())NULL;

static bool_t process_appmgr_callback();

static CommXdrFcn am_cli_xdr_function_gen_req(int function_number)
{
  CommXdrFcn the_func = (CommXdrFcn)NULL;

  if (function_number == APPMGR_SET_ICON_FILE)
  {
    the_func = xip_am_set_icon_file;
  }
  else if (function_number > 2)
  {
    the_func = xdr_Appmgr_IPC_Data;
  }

  return the_func;
}

static CommXdrFcn am_cli_xdr_function_gen_reply(int function_number)
{
  CommXdrFcn the_func = (CommXdrFcn)NULL;

  if (function_number == APPMGR_SET_ICON_FILE)
  {
    the_func = xop_am_set_icon_file;
  }
  else if (function_number > 2)
  {
    the_func = xdr_Appmgr_IPC_Data;
  }

  return the_func;
}

static Service_func am_cli_service_function_gen( function_number )
    int function_number;
{
    if( function_number > 2 )
        return( process_appmgr_callback );
    else
        return( (Service_func)NULL );
}

int contact_appmgr( int initialize_comm_layer )
{
  static int               first_time = TRUE;
  static char              host_name[256];
  static char              display_name[256];
  static char              disp_kbd[256];
  static char              user_name[256];
  static int               peer_added = FALSE;
  int                      appmgr_rpc_port = K_NOT_USED;
  int                      kbd_num=0;

  if (first_time)
  {
    pro_get_hostname(host_name, 256);
    if (get_display_address() != NULL)
    {
      strcpy(display_name, get_display_address());
    }
    else
    {
      strcpy(display_name, "");
    }
    get_display_kbd_screen( &kbd_num, NULL);
    pro_sprintf(disp_kbd, "%d", kbd_num);

    pro_wstr_to_str(user_name, pro_get_user_name());
    first_time = FALSE;
  }

  if (appmgr_contacted)
  {
    return APP_MGR_CONTACTED;
  }

  if (!peer_added)
  {
    if (initialize_comm_layer)
    {
      if (!comm_layer_initialize())
      {
        return APP_MGR_CANT_INITIALIZE_COMM;
      }
    }

    register_xdr_gen_fcn_req(14, 0, am_cli_xdr_function_gen_req);
    register_xdr_gen_fcn_reply(14, 0, am_cli_xdr_function_gen_reply);
    register_svc_gen_fcn(14, 0, am_cli_service_function_gen);

    /* if PRO_NO_NAME_SERVICE is defined check the old way */
    if (BTK_GETENV_31_S("PRO_NO_NAME_SERVICE") == NULL
    /*
     * Temporarily (at least) suppress registering with/starting
     * name service in regression mode
     */
       && !regression_run_mode())
    {
      get_appmgr_rpc_ports_from_nm_svc(NULL, display_name, disp_kbd,
                                       user_name, &appmgr_rpc_port,
                                       &appmgr_handle);
    }
    else
    {
      get_appmgr_rpc_ports(NULL, display_name, user_name, &appmgr_rpc_port,
                           NULL, &appmgr_handle);

    }
  }

  if (appmgr_rpc_port >= 0)
  {
    peer_added = TRUE;
  }

  if (peer_added && app_is_peer_alive(appmgr_handle, KNOWN_INCARNATION))
  {
    appmgr_contacted = TRUE;
    return APP_MGR_CONTACTED;
  }
  else
  {
    return APP_MGR_IS_NOT_RUNNING;
  }
}

static int appmgr_goodbye()
{
    return( TRUE );
}

int appmgr_service_callback()
{
    Pro_Time_Val  wait_timer = {0, 0};
    Pro_Time_Val  service_timer = {300, 0}; /* make it longer than necessary
					       for servicing any call appmgr --> application */

    /* remark on the coding practice: optimizing compilers should figure out
       how that timer variables are actually constants. Compiler
       will make a best decision how to speed things up.
       Declaring timers "static" might be not best. PLS */

    app_service_peer_call(wait_timer, service_timer);
    return( TRUE );
}

Prodev_comm_status send_request_to_appmgr_w_retry(
                     int remote_function, void **in_data, void **out_data)
{
  static Pro_Time_Val wait_timer = {300, 0};
  Prodev_comm_status error_code;
  int retries = 0;

  do
  {
    call_peer(appmgr_handle, 14, remote_function, 0, in_data, out_data,
              NULL, wait_timer, NULL, &error_code);

    if (prodevcomm_certain_deadlock(error_code) ||
        prodevcomm_possible_deadlock(error_code))
    {
#if DEBUG
      btk_fprintf(amg_cli_log, "Possible deadlock - Retrying\n");
#endif

      retries++;
    }
    else /* success or other error other than deadlock */
    {
       if ( ! prodevcomm_success(error_code) )
          dbg_info_msg("send_request_to_appmgr_w_retry", "error code is (%d)\n",
                       error_code);
       break;
    }

  }
  while (retries <= 5);

  return error_code;
}

Prodev_comm_status send_request_to_appmgr(remote_function, in_data, out_data)
    int                remote_function;
    Appmgr_IPC_Data   *in_data;
    Appmgr_IPC_Data  **out_data;
{
  Prodev_comm_status error_code;

  if( !app_is_peer_alive(appmgr_handle, KNOWN_INCARNATION) )
  {
    return PDEV_COMM_STATUS_COMM_BROKEN;
  }

  error_code = send_request_to_appmgr_w_retry(
                 remote_function, (void **)&in_data, (void **)out_data);

  return error_code;
}

static bool_t process_appmgr_callback( in_data, in_free, out_data, out_free )
    void                  **in_data;
    CommGenericDestructor   in_free;
    void                  **out_data;
    CommGenericDestructor **out_free;
{
    Appmgr_IPC_Data   *in;
    int                start_index, end_index;
    int                appl_id, callback_id, callback_flags;

    if( in_free.destructor_function != NULL )
        (*in_free.destructor_function)( &in_free, NULL );

    in = (Appmgr_IPC_Data*)*in_data;
    if( in == NULL )
        return( FALSE );

    *out_data = NULL;
    *out_free = NULL;

    get_ipc_string( in->data, 0, in->n_bytes, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    appl_id = atoi( in->data );
    start_index = end_index;

    get_ipc_string( in->data, start_index, in->n_bytes, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    callback_id = atoi( (char*)&in->data[start_index] );
    start_index = end_index;

    get_ipc_string( in->data, start_index, in->n_bytes, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    callback_flags = atoi( (char*)&in->data[start_index] );
    start_index = end_index;

    if( appmgr_callback_function != (void(*)())NULL )
       (*appmgr_callback_function)( appl_id, callback_id, callback_flags );

    free_Appmgr_IPC_Data( in );
    return( TRUE );
}

int appmgr_register_application( registration_string )
    char *registration_string;
{
    int                status, end_index;
    Appmgr_IPC_Data   *in, *out;
    int                appl_id;
    Prodev_comm_status request_status;

    out = (Appmgr_IPC_Data *) NULL; /* IMPORTANT!!! Otherwise comm layer
                                       (rather app xdr) will assume that
                                       Appmgr_IPC_Data is already allocated */


    if( !appmgr_contacted )
    {
        status = contact_appmgr( FALSE );
        if( status != APP_MGR_CONTACTED )
            return( -1 );
    }

    in = get_Appmgr_IPC_Data_ds();

    if( registration_string != NULL )
        put_ipc_stream( in->data, 0, in->max_n_bytes,
                        registration_string, strlen(registration_string),
                        &end_index );
    else
        put_ipc_stream( in->data, 0, in->max_n_bytes, NULL, 0, &end_index );

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 3, in, &out );
    if( prodevcomm_success(request_status) )
    {
        get_ipc_string( out->data, 0, out->n_bytes, &end_index );
        if( end_index != -1 )
            appl_id = atoi( out->data );
        else
            appl_id = -1;

        free_Appmgr_IPC_Data( out );
    }
    else
        appl_id = -1;

#if DEBUG
        btk_fprintf( amg_cli_log, "* Application Registration Confirmation\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n\n", appl_id );
#endif

    if( appl_id != -1 )
        number_applications_registered++;

    free_Appmgr_IPC_Data( in );

    return( appl_id );
}

int appmgr_register_callback( application_id, callback_name, callback_image_tag,
                              callback_tooltip, callback_flags )
    int   application_id;
    char *callback_name;
    char *callback_image_tag;
    char *callback_tooltip;
    int   callback_flags;
{
    int                  start_index, end_index, callback_id;
    Appmgr_IPC_Data     *in, *out;
    Prodev_comm_status   request_status;
    out = (Appmgr_IPC_Data *) NULL; /* IMPORTANT!!! Otherwise comm layer
                                       (rather app xdr) will assume that
                                       Appmgr_IPC_Data is already allocated */

    if( !appmgr_contacted )
        return( -1 );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    if( callback_name != NULL )
        put_ipc_stream( in->data, start_index, in->max_n_bytes, callback_name,
                        strlen(callback_name), &end_index );
    else
        put_ipc_stream( in->data, start_index, in->max_n_bytes, NULL,
                        0, &end_index );
    if( end_index == -1 )
    {
       free_Appmgr_IPC_Data( in );
       return( -1 );
    }
    start_index = end_index;

    if( callback_image_tag != NULL )
        put_ipc_string( in->data, start_index, in->max_n_bytes,
                        callback_image_tag, &end_index );
    else
        put_ipc_string( in->data, start_index, in->max_n_bytes, "",
                        &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    if( callback_tooltip != NULL )
        put_ipc_stream( in->data, start_index, in->max_n_bytes,
                        callback_tooltip, strlen(callback_tooltip),
                        &end_index );
    else
        put_ipc_stream( in->data, start_index, in->max_n_bytes, NULL,
                        0, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    btk_sprintf( auxbuffer, "%d", callback_flags );
    put_ipc_string( in->data, start_index, in->max_n_bytes, auxbuffer,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 4, in, &out );
    if( prodevcomm_success(request_status) )
    {
        get_ipc_string( out->data, 0, out->n_bytes, &end_index );
        if( end_index == -1 )
            callback_id = -1;
        if( atoi(out->data) != application_id )
            callback_id = -1;
        start_index = end_index;

        get_ipc_string( out->data, start_index, out->n_bytes, &end_index );
        if( end_index == -1 )
            callback_id = -1;

        callback_id = atoi( (char*)&out->data[start_index] );

        free_Appmgr_IPC_Data( out );
    }
    else
        callback_id = -1;

#if DEBUG
        btk_fprintf( amg_cli_log, "* Callback Registration Confirmation\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Callback ID =    %d\n\n", callback_id );
#endif

    free_Appmgr_IPC_Data( in );

    return( callback_id );
}

int appmgr_update_callback( application_id, callback_id, callback_name,
                            callback_image_tag, callback_tooltip )
    int   application_id;
    int   callback_id;
    char *callback_name;
    char *callback_image_tag;
    char *callback_tooltip;
{
    int                  start_index, end_index, update_mask;
    Appmgr_IPC_Data     *in;
    Prodev_comm_status   request_status;

    if( !appmgr_contacted )
        return( -1 );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    btk_sprintf( auxbuffer, "%d", callback_id );
    put_ipc_string( in->data, start_index, in->max_n_bytes, auxbuffer,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    update_mask = 0;
    if( callback_name != NULL )
        update_mask |= 1;
    if( callback_image_tag != NULL )
        update_mask |= 2;
    if( callback_tooltip != NULL )
        update_mask |= 4;

    if( update_mask == 0 )
    {
        free_Appmgr_IPC_Data( in );
        return( callback_id );
    }

    btk_sprintf( auxbuffer, "%d", update_mask );
    put_ipc_string( in->data, start_index, in->max_n_bytes, auxbuffer,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    if( callback_name != NULL )
    {
        put_ipc_stream( in->data, start_index, in->max_n_bytes, callback_name,
                        strlen(callback_name), &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( in );
            return( -1 );
        }
        start_index = end_index;
    }

    if( callback_image_tag != NULL )
    {
        put_ipc_string( in->data, start_index, in->max_n_bytes,
                        callback_image_tag, &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( in );
            return( -1 );
        }
        start_index = end_index;
    }

    if( callback_tooltip != NULL )
    {
        put_ipc_stream( in->data, start_index, in->max_n_bytes,
                        callback_tooltip, strlen(callback_tooltip),
                        &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( in );
            return( -1 );
        }
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 5, in, (Appmgr_IPC_Data**)NULL );
    free_Appmgr_IPC_Data( in );

    if( prodevcomm_success(request_status) )
    {
#if DEBUG
        btk_fprintf( amg_cli_log, "* Callback Update\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Callback ID =    %d\n\n", callback_id );
#endif
        return( callback_id );
    }
    else
        return( -1 );
}

int appmgr_unregister_callback( application_id, callback_id )
    int application_id;
    int callback_id;
{
    int                  start_index, end_index;
    Appmgr_IPC_Data     *in;
    Prodev_comm_status   request_status;

    if( !appmgr_contacted )
        return( -1 );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    btk_sprintf( auxbuffer, "%d", callback_id );
    put_ipc_string( in->data, start_index, in->max_n_bytes, auxbuffer,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 6, in, (Appmgr_IPC_Data**)NULL );
    free_Appmgr_IPC_Data( in );

    if( prodevcomm_success(request_status) )
    {
#if DEBUG
        btk_fprintf( amg_cli_log, "* Callback Unregistration Confirmation\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Callback ID =    %d\n\n", callback_id );
#endif
        return( callback_id );
    }
    else
        return( -1 );
}

int appmgr_unregister_application( application_id )
    int application_id;
{
    int                  end_index;
    Appmgr_IPC_Data     *in;
    Prodev_comm_status   request_status;

    if( !appmgr_contacted || application_id < 0 )
        return( -1 );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 7, in, (Appmgr_IPC_Data**)NULL );
    free_Appmgr_IPC_Data( in );

    if( prodevcomm_success(request_status) )
    {
#if DEBUG
        btk_fprintf( amg_cli_log, "* Application Unregistration Confirmation\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n\n", application_id );
#endif

        if( --number_applications_registered == 0 )
            appmgr_goodbye();

        return( application_id );
    }
    else
        return( -1 );
}

int appmgr_get_rectangle( application_id, out_x, out_y, out_width, out_height,
                          out_placement )
    int     application_id;
    double *out_x;
    double *out_y;
    double *out_width;
    double *out_height;
    int    *out_placement;
{
    int                 start_index, end_index;
    double              x, y, width, height;
    int                 placement;
    Appmgr_IPC_Data    *out;
    Prodev_comm_status  request_status;

    if( !appmgr_contacted )
        return( -1 );

    out = (Appmgr_IPC_Data *) NULL; /* IMPORTANT!!! Otherwise comm layer
                                       (rather app xdr) will assume that
                                       Appmgr_IPC_Data is already allocated */

    request_status = send_request_to_appmgr( 8, (Appmgr_IPC_Data*)NULL, &out );

    if( prodevcomm_success(request_status) )
    {
        get_ipc_string( out->data, 0, out->n_bytes, &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( out );
            return( -1 );
        }
        x = atof( out->data );
        start_index = end_index;

        get_ipc_string( out->data, start_index, out->n_bytes, &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( out );
            return( -1 );
        }
        y = atof( (char*)&out->data[start_index] );
        start_index = end_index;

        get_ipc_string( out->data, start_index, out->n_bytes, &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( out );
            return( -1 );
        }
        width = atof( (char*)&out->data[start_index] );
        start_index = end_index;

        get_ipc_string( out->data, start_index, out->n_bytes, &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( out );
            return( -1 );
        }
        height = atof( (char*)&out->data[start_index] );
        start_index = end_index;

        get_ipc_string( out->data, start_index, out->n_bytes, &end_index );
        if( end_index == -1 )
        {
            free_Appmgr_IPC_Data( out );
            return( -1 );
        }
        placement = atoi( (char*)&out->data[start_index] );

        if( out_x != NULL )
            *out_x = x;

        if( out_y != NULL )
            *out_y = y;

        if( out_width != NULL )
            *out_width = width;

        if( out_height != NULL )
            *out_height = height;

        if( out_placement != NULL )
            *out_placement = placement;

#if DEBUG
        btk_fprintf( amg_cli_log, "* Application Manager Rect Request:\n" );
        btk_fprintf( amg_cli_log, "    Rectangle = (%2.3f,%2.3f,%2.3f,%2.3f)\n",
                 x, y, width, height );
        btk_fprintf( amg_cli_log, "    Placement = %d\n\n", placement );
#endif
        return( application_id );
    }
    else
        return( -1 );
}

int appmgr_get_callback_from_title( application_id, callback_name,
                                    requested_appl_id, requested_callback_id )
    int   application_id;
    char *callback_name;
    int  *requested_appl_id;
    int  *requested_callback_id;
{
    int                  start_index, end_index;
    int                  success, out_appl_id, out_callback_id;
    Appmgr_IPC_Data     *in, *out;
    Prodev_comm_status   request_status;
    out = (Appmgr_IPC_Data *) NULL; /* IMPORTANT!!! Otherwise comm layer
                                       (rather app xdr) will assume that
                                       Appmgr_IPC_Data is already allocated */

    if( !appmgr_contacted )
        return( FALSE );

    if( callback_name == NULL )
        return( FALSE );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    start_index = end_index;

    put_ipc_stream( in->data, start_index, in->max_n_bytes, callback_name,
                    strlen(callback_name), &end_index );

    if( end_index == -1 )
    {
       free_Appmgr_IPC_Data( in );
       return( FALSE );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 10, in, &out );
    if( prodevcomm_success(request_status) )
    {
        get_ipc_string( out->data, 0, out->n_bytes, &end_index );
        if( end_index == -1 )
            success = FALSE;
        else
            success = atoi( out->data );
        start_index = end_index;
        if( success )
        {
            get_ipc_string( out->data, start_index, out->n_bytes, &end_index );
            if( end_index == -1 )
            {
                success = FALSE;
            }
            else
            {
                out_appl_id = atoi( (char*)&out->data[start_index] );

                start_index = end_index;
                get_ipc_string( out->data, start_index, out->n_bytes,
                                &end_index );
                if( end_index != -1 )
                {
                    out_callback_id = atoi( (char*)&out->data[start_index] );
                }
                else
                {
                    success = FALSE;
                }
            }
        }

        free_Appmgr_IPC_Data( out );
    }
    else
        success = FALSE;

    if( success )
    {
        if( requested_appl_id != NULL )
            *requested_appl_id = out_appl_id;
        if( requested_callback_id != NULL )
            *requested_callback_id = out_callback_id;
    }

#if DEBUG
        btk_fprintf( amg_cli_log, "* Callback from Title String Requested\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Callback Title =    %s\n", callback_name );
        btk_fprintf( amg_cli_log, "    Result = %d\n", success );
        if( success )
        {
            btk_fprintf( amg_cli_log, "    Requested Appl ID = %d\n", out_appl_id);
            btk_fprintf( amg_cli_log, "    Requested CB ID   = %d\n\n",
                                       out_callback_id );
        }
#endif

    free_Appmgr_IPC_Data( in );

    return( success );
}

int appmgr_get_tmp_cb_ownership( application_id, callback_name, menu_item_id )
    int   application_id;
    char *callback_name;
    int   menu_item_id;
{
    int                  start_index, end_index;
    int                  new_callback_id;
    Appmgr_IPC_Data     *in, *out;
    Prodev_comm_status   request_status;

    out = (Appmgr_IPC_Data *) NULL; /* IMPORTANT!!! Otherwise comm layer
                                       (rather app xdr) will assume that
                                       Appmgr_IPC_Data is already allocated */

    if( !appmgr_contacted )
        return( -1 );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( -1 );
    }
    start_index = end_index;

    if( callback_name != NULL )
    {
        put_ipc_stream( in->data, start_index, in->max_n_bytes, callback_name,
                        strlen(callback_name), &end_index );
    }
    else
    {
        /*
         * A (char)1 in the first position of the stream tells the server side
         * that we are sending a menu_item_id instead of a callback_name
         */

        btk_sprintf( auxbuffer, "%c%d", (char)1, menu_item_id );
        put_ipc_stream( in->data, start_index, in->max_n_bytes, auxbuffer,
                        strlen(auxbuffer), &end_index );
    }

    if( end_index == -1 )
    {
       free_Appmgr_IPC_Data( in );
       return( -1 );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 11, in, &out );
    if( prodevcomm_success(request_status) )
    {
        get_ipc_string( out->data, 0, out->n_bytes, &end_index );
        if( end_index == -1 )
            new_callback_id = -1;
        else
            new_callback_id = atoi( out->data );
    }
    else
        new_callback_id = -1;

#if DEBUG
        btk_fprintf( amg_cli_log, "* Callback Temporary Ownership Requested\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Callback Title =    %s\n", callback_name );
        btk_fprintf( amg_cli_log, "    New Callback ID = %d\n", new_callback_id );
#endif

    free_Appmgr_IPC_Data( in );

    return( new_callback_id );
}

int appmgr_register_workspace( application_id, workspace_name, workspace_dir )
    int   application_id;
    char *workspace_name;
    char *workspace_dir;
{
    int                  start_index, end_index;
    Appmgr_IPC_Data     *in;
    Prodev_comm_status   request_status;

    if( !appmgr_contacted )
        return( FALSE );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    start_index = end_index;

    put_ipc_string( in->data, start_index, in->max_n_bytes, workspace_name,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    start_index = end_index;

    put_ipc_string( in->data, start_index, in->max_n_bytes, workspace_dir,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 12, in, (Appmgr_IPC_Data**)NULL );
    free_Appmgr_IPC_Data( in );

    if( prodevcomm_success(request_status) )
    {
#if DEBUG
        btk_fprintf( amg_cli_log, "* Workspace Registration Confirmation\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Workspace =      %s\n", workspace_name );
        btk_fprintf( amg_cli_log, "    Workspace Dir =  %s\n\n", workspace_dir );
#endif
        return( TRUE );
    }
    else
        return( FALSE );
}

int appmgr_unregister_workspace( application_id, workspace_name )
    int   application_id;
    char *workspace_name;
{
    int                  start_index, end_index;
    Appmgr_IPC_Data     *in;
    Prodev_comm_status   request_status;

    if( !appmgr_contacted )
        return( FALSE );

    in = get_Appmgr_IPC_Data_ds();

    btk_sprintf( auxbuffer, "%d", application_id );
    put_ipc_string( in->data, 0, in->max_n_bytes, auxbuffer, &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }
    start_index = end_index;

    put_ipc_string( in->data, start_index, in->max_n_bytes, workspace_name,
                    &end_index );
    if( end_index == -1 )
    {
        free_Appmgr_IPC_Data( in );
        return( FALSE );
    }

    in->n_bytes = end_index;

    request_status = send_request_to_appmgr( 13, in, (Appmgr_IPC_Data**)NULL );
    free_Appmgr_IPC_Data( in );

    if( prodevcomm_success(request_status) )
    {
#if DEBUG
        btk_fprintf( amg_cli_log, "* Workspace Unregistration Confirmation\n" );
        btk_fprintf( amg_cli_log, "    Application ID = %d\n", application_id );
        btk_fprintf( amg_cli_log, "    Workspace =      %s\n\n", workspace_name );
#endif
        return( TRUE );
    }
    else
        return( FALSE );
}

void set_appmgr_callback_function( function )
    void(*function)();
{
    appmgr_callback_function = function;
}

int appmgr_is_alive()
{
    if( appmgr_contacted && app_is_peer_alive(appmgr_handle, KNOWN_INCARNATION) )
        return( TRUE );
    else
        return( FALSE );
}

int appmgr_start_no_wait(char *path_to_executable)
{
  int status = TRUE, ret_val = FALSE;
  char *command = NULL;

  command = getmem(sizeof(char)*(strlen(path_to_executable)+32));
  if (IS_BOUND_TO_GLOB_INTF(get_new_skin_usage) &&
      EXT_CALL_VIA_GLOB_INTF(get_new_skin_usage))
  {
        btk_sprintf(command, "%s -laf PTC", path_to_executable);
  }
  else
  	btk_sprintf(command, "%s -laf motif", path_to_executable);

  /*
     for Windows 95 use shell as when running a appmgr batch file
     it runs out of env space, as when shell is the used to start appmgr
     this can increased with the option /E:nnnnn
  */
  status = pro_exec_cmd(FALSE, FALSE, FALSE, FALSE, &ret_val, command);

  rlsmem(command);

  return status;
}

int appmgr_contact_with_wait(int seconds_to_wait)
{
  int n_tries = 0;
  int status = APP_MGR_IS_NOT_RUNNING;
  int max_num_tries = seconds_to_wait / 2;

  /*
  ** Try to contact at least once.
  */

  status = contact_appmgr(FALSE);

  while ((status != APP_MGR_CONTACTED) && (n_tries < max_num_tries))
  {
    n_tries++;

    if (appmgr_debug())
    {
      btk_printf("Waiting for AppMgr; Try # %d\n", n_tries);
    }

    spg_delay_mic((long)2000000); /* wait 2 seconds */

    status = contact_appmgr(FALSE);
  }

  return status;
}

int appmgr_start_and_wait(char *path_to_executable, int seconds_to_wait)
{
  int status = FALSE;

  status = appmgr_start_no_wait(path_to_executable);

  if (status && (seconds_to_wait > 0))
  {
    spg_delay_mic((long)4000000);   /* wait 4 seconds */

    if (appmgr_contact_with_wait(seconds_to_wait - 4) != APP_MGR_CONTACTED)
    {
      status = FALSE;
    }
  }

  return status ;
}

int appmgr_debug()
{
    static  int appmgr_debug_status;
    static  int first_time = TRUE;

    if( first_time )
    {
        appmgr_debug_status = (BTK_GETENV_31_S("APPMGR_DEBUG") != NULL) &&
                                 (strcmp(BTK_GETENV_31_S("APPMGR_DEBUG"), "x782x") == 0);
        first_time = FALSE;
    }

    return( appmgr_debug_status );
}

/* 
 * The App-Manager application is not shipped on Windows.
 * There is no need to do some of its initialization
 */

int appmgr_supported_on_this_platform()
{
#ifdef WIN32
	return 0;
#else
	return 1;
#endif
}
