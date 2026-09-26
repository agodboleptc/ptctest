/* qm_procs.c - functions used by the queue manager to start and stop
   		batch processes running in the background.
*/
/*
--------------------------------------------------------------------------
29-Feb-96   G-03-04	mxs	$$1	Created
11-Mar-96   G-03-05	mxs	$$2	Fixed changing of working directory
12-Mar-96   G-03-06	mxs	$$3	Fixed pro_format_execv_args()
23-Mar-96   G-03-07	mxs	$$4 	Fixed qm_get_qm_working_dir_1()
11-Apr-96   G-03-10     mxs	$$5	fixed qm_start_rbat_proc
09-Jul-96   G-03-18+    swv     $$6	Turn off DETACHED_PROCESS flag for
                                        Windows 95 when creating process.
21 Sep 01   J-03-09     jas     $$7     Removed WINDOWS_95 macro
24-Apr-06 L-01-07 TWH $$8 use windows unicode wrappers
12-Jul-06 L-01-12 ksi $$9 Unicode compliant changes
--------------------------------------------------------------------------
*/

#include <ptc_win32.h>
#include <btkcprocess.h>
#include <btkcstdio.h>
#include "pro_signal.h"
#include "queuem.h"
#include "pfa.h"
#include "prorembatch.h"


extern int qm_add_rembat_proc_handle();
extern Qm_Proc_Handle * qm_find_rembat_proc_handle();
extern int qm_delete_rembat_proc_handle();

static int pro_format_execv_args();



/*---------------------------------------------------------------*/
int qm_kill_proc_for_entry(entry_ptr)
Qm_Queue_Entry * entry_ptr;
{
   if (entry_ptr == NULL)
      return(FALSE);

   if(qm_kill_rbat_proc(&(entry_ptr->data)))
   {
      entry_ptr->data.curr_status = QE_BG_KILLED;
      qm_delete_rembat_proc_handle(entry_ptr->data.bg_proc.pid,
				   entry_ptr->data.txn.txn_id);
      entry_ptr->data.bg_proc.pid = -1;
      return(TRUE);
   }
   else
   {
      entry_ptr->data.curr_status = QE_BG_ERROR;
      return(FALSE);
   }
}

/*---------------------------------------------------------------*/
int qm_run_process_for_entry(entry_ptr)
Qm_Queue_Entry * entry_ptr;
{
   char exec_name[QM_CMD_LINE_LEN];
   char arg_string[QM_CMD_LINE_LEN];
   Pfa * job_directory;
   int stat = 0;

   if (entry_ptr == NULL)
      return(FALSE);

   BYTE_FILL(exec_name,  QM_CMD_LINE_LEN, '\0');
   BYTE_FILL(arg_string,  QM_CMD_LINE_LEN, '\0');

   stat = qm_get_rbat_exec_name(exec_name, QM_CMD_LINE_LEN);
   if(!stat)
   {
      entry_ptr->data.curr_status = QE_BG_ERROR;
      return(FALSE);
   }

   stat = qm_get_rbat_arg_string(arg_string, QM_CMD_LINE_LEN, &(entry_ptr->data));
   if(!stat)
   {
      entry_ptr->data.curr_status = QE_BG_ERROR;
      return(FALSE);
   }

   job_directory = entry_ptr->data.job_dir_pfa;

   stat = qm_start_rbat_proc(exec_name, arg_string, job_directory,
			     (entry_ptr->data.txn.txn_id), &(entry_ptr->data.bg_proc));
   if(!stat)
   {
      entry_ptr->data.curr_status = QE_BG_ERROR;
      return(FALSE);
   }
   else
   {
      entry_ptr->data.curr_status = QE_BG_RUNNING;
   }
   return(TRUE);
}
/*---------------------------------------------------------------------------*/

int qm_start_rbat_proc(cmd_string, arg_string, wrk_dir, txn_id, bg_proctok_p)
char * cmd_string;
char * arg_string;
Pfa * wrk_dir;
int txn_id;
Qm_Proc_Token * bg_proctok_p;
{
   int a_stat = TRUE;
   int stat;
   Qm_Proc_Handle p_handle;

   if((cmd_string == NULL)|| (wrk_dir == NULL))
      return(FALSE);

   stat = pfa_chdir(wrk_dir);
   if (stat != PFA_E_NO_ERROR)
   {
      btk_fprintf(btk_get_stderr(), "qm_start_rbat_proc() : Unable to change working dir\n");
      return(FALSE);
   }

   if ((a_stat) && (! qm_start_proc_(cmd_string, arg_string, &p_handle)))
   {
      btk_fprintf(btk_get_stderr(), "qm_start_rbat_proc() : Unable to spawn sub process\n");
      a_stat = FALSE;
   }

   if((a_stat) && (bg_proctok_p != NULL))
   {
      bg_proctok_p->pid = p_handle.pid;
      bg_proctok_p->qm_index = -1;
      bg_proctok_p->hostname[0] = '\0';
      bg_proctok_p->ip_address[0] = '\0';
   }

   if ((a_stat) && (!qm_add_rembat_proc_handle( &p_handle, txn_id)))
   {
      btk_fprintf(btk_get_stderr(), "qm_start_rbat_proc() : Unable to add handle\n");
      a_stat = FALSE;
   }

   if(!qm_change_to_qm_home_dir())
   {
      btk_fprintf(btk_get_stderr(), "qm_start_rbat_proc() : Unable to change to home dir\n");
      a_stat = FALSE;
   }
   return(a_stat);
}

/*---------------------------------------------------------------------------*/
int qm_start_proc_(cmd_string, arg_string, p_handle)
char * cmd_string;
char * arg_string;
Qm_Proc_Handle * p_handle;
{
#if OPER_SYS == WINDOWS_32

   STARTUPINFOA          sui;
   PROCESS_INFORMATION  pi;
   DWORD                ret;
   char * cmdline;

   if((cmd_string == NULL) || (p_handle == NULL))
      return(FALSE);

   cmdline = cmd_string;
   cmdline[strlen(cmd_string)] = ' ';
   cmdline[strlen(cmd_string) + 1] = '\0 ';
   strcat(cmdline, arg_string);

   BYTE_FILL(&sui, sizeof(STARTUPINFOA), '\0');
   sui.cb = sizeof (STARTUPINFOA);

   ret = uCreateProcess (NULL, cmdline, NULL, NULL,
			FALSE, CREATE_NEW_PROCESS_GROUP|DETACHED_PROCESS,
			NULL, NULL, &sui, &pi );
   if(!ret)
   {
      ret=GetLastError();
      return(FALSE);
   }

   p_handle->pid = pi.dwProcessId;
   p_handle->type = 0;
   p_handle->handle = pi.hProcess;

#elif OPER_SYS == UNIX_OS

   int child_pid = 0;
   char ** new_args;


   if((cmd_string == NULL) || (p_handle == NULL))
      return(FALSE);

   child_pid = fork();
   if(child_pid == 0)
   {
      /* Child process */
      pro_format_execv_args(cmd_string, arg_string, &(new_args));

      /*  Run the given command  */
      btk_execv (cmd_string, new_args);

      /* If we get this far, its an error */
      if (errno == ENOENT)
      {
	 btk_fprintf (btk_get_stderr(), "Cannot exec %s - No such file or directory\n",
		  cmd_string);
      }
      else
      {
	 btk_fprintf (btk_get_stderr(), "Child: execv failed, errno = %d\n", errno);
      }
      exit (1);
   }

   if(child_pid < 0)
   {
      btk_printf("ERROR : FAILED TO CREATE PROCESS\n");
      return(FALSE);
   }
   else
   {
      /* Parent process */

      p_handle->pid = child_pid;
      p_handle->type = 0;
   }

#endif

   return(TRUE);
}
/*---------------------------------------------------------------------------*/
int qm_kill_rbat_proc(entry_data)
Qm_Queue_Entry_Data * entry_data;
{
Qm_Proc_Handle * proc_handle;

   proc_handle = qm_find_rembat_proc_handle(entry_data->bg_proc.pid,
					    entry_data->txn.txn_id);
   if(proc_handle == NULL)
      return(TRUE);

   if (qm_kill_rbat_proc_(proc_handle))
   {
      return(TRUE);
   }
   else
   {
      return(FALSE);
   }
}
/*---------------------------------------------------------------------------*/

int qm_kill_rbat_proc_(proc_handle)
Qm_Proc_Handle * proc_handle;
{
#if OPER_SYS == UNIX_OS

   return (kill (((Qm_Proc_Handle *) proc_handle) -> pid, PRO_SIGTERM) == 0);

#elif OPER_SYS == WINDOWS_32
   return(TerminateProcess (((Qm_Proc_Handle *) proc_handle)->handle, 0));
#else
   return(FALSE);
#endif
}

/*---------------------------------------------------------------------------*/
static int pro_format_execv_args(exec_str, arg_str, ret_args)
   char * exec_str;
   char * arg_str;
   char *** ret_args;
{
   int i, j;
   int n_chars;
   int n_args = 0;
   int tot_args = 0;
   char ** new_args;

   /* exec needs the first arg[0] to be the name of the executable */
   if(exec_str == NULL)
      return(FALSE);

   n_chars = strlen(arg_str);
   for(i = 0; i < n_chars ; i++)
   {
      if (arg_str[i] == ' ')
      {
	 arg_str[i] = '\0';
	 n_args ++;
      }
   }
   n_args ++;
   tot_args = n_args +1;

   if(tot_args > 0)
   {
      new_args = (char **) getmem ((tot_args+1) * sizeof(char *));

      new_args[0] = exec_str;

      j = 0;
      for (i=1 ; i < tot_args ; i++)
      {
	 new_args[i] = &(arg_str[j]);
	 while (j < n_chars && arg_str[j] != '\0')
	 {
	    j++;
	 }
	 j++;
      }
      new_args[tot_args] = (char *)NULL;
   }
   *ret_args = new_args;
   return(TRUE);
}

