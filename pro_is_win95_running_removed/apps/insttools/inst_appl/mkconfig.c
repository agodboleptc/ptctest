/*
  02/09/94  E-03-10 Jimdipalma $$1 Created.
  02/15/94  E-03-11 James      $$2 Fixed arguments to pfa_copy_file().
   8/24/94  E-03-34 SWL        $$3  Added processing command line argument
				    "-verify"
  11/17/94  E-06-19 James      $$4 Added hmflib.
  31-Aug-95 G-01-05 amidon     $$5 Made pfa_get_device thread-safe.
  04-Oct-95 G-01-08 GAA        $$6 Changed calls to pfa_get_... (thread safe).
  09-OCT-95 G-01-08 integ      $$7 Fixed typo.
  09-NOV-95 G-01-11 Harry      $$8 Made it work on win95 and fixed a bug on r4k.
  22-Jan-96 G-03-01 jmichaud   $$9 Include header for pro_printf() varargs funcs.
  21-Sep-01 J-03-09 jas        $$10 Removed WINDOWS_95 macro
  12-Jul-06 L-01-12 ksi        $$11 Unicode compliant changes
  21-Dec-21 P-90-39 Ahmad      $$12 scrambled literal env vars
*/

#include <btkcdir.h>
#include <btkcstdio.h>
#include <btkcstdlib.h>
#include "string.h"
#include "hardware.h"
#include "exit.h"
#include "pro_widec.h"
#include "pfa.h"
#include "core_err.h"
#include "const.h"
#include "ent_type.h"
#include "utility.h"
#include "proprintf.h"
#include <btkscale31.h>


#define CONFIG_FILE_EXT  "pro"
#define CONFIG_FILE_NAME "config"
#define DEFAULT_LIB_LOAD_POINT  ""
#define OLD_FILE_NAME    "oldconfig"

#define NO_USER_OPTIONS	0x00
#define RENAME_OK	0x01
#define APPEND_OK	0x02
#define DELETE_OK	0x04
#define LIB_DIR_SET	0x10

#ifndef NO_ERROR
#define NO_ERROR	0
#endif
#ifndef IS_ERROR
#define IS_ERROR	111
#endif


typedef struct _name_options {
	char	name[2*K_NAME_SIZE];
	char	banner[2*K_LINESIZE];
	char	top_lib_dir[K_PATH_SIZE];
	char	exclude_list[5*K_NAME_SIZE];
} name_options;

extern void exit(), pro_cnvrt_to_upper();
extern char btk_getch();
extern int atoi(), btk_mkdir(), sleep();
extern char *btk_getenv();

wchar_t *wstr_ret();
int pro_is_directory();

int user_options = NO_USER_OPTIONS;
static int mydbg;


/************************************************************/

char *get_library_lp(path, name_opts)
char		*path;
name_options	name_opts;
{
 char	buf[K_PATH_SIZE];
 char	library_pathname[K_PATH_SIZE] ;
 Pfa	*tmp_dir;
 Pfa	*working_dir;
 int	path_ok = FALSE;
 char	fullname[K_PATH_SIZE];
 int    user_opts;

 user_opts = user_options;

 pfa_alloc_pro_file(&tmp_dir);
 pfa_alloc_pro_file(&working_dir);
 pfa_copy_pro_file(pfa_get_cur_dir(), working_dir);


 do {
     if ( (user_opts & LIB_DIR_SET) && (path != NULL) ) {
	 strcpy(library_pathname, path);
	 user_opts -= LIB_DIR_SET;
     } else {
         pro_printf("Enter library load point directory.\n");
         pro_printf("[%s] ? : ", DEFAULT_LIB_LOAD_POINT);
         btk_fgets(buf, K_PATH_SIZE, btk_get_stdin());
         if ((int)strlen(buf) > 1)
          {
           btk_sscanf(buf, "%s", library_pathname);
          }
         else
          {
           strcpy(library_pathname, DEFAULT_LIB_LOAD_POINT);
          }
     }
     pfa_parse_to_pro_file(tmp_dir, library_pathname);
     pfa_get_full_name(tmp_dir, fullname);
     if (pfa_open_dir(tmp_dir) == PFA_E_NO_ERROR)
        {
         pfa_close(tmp_dir);
         pfa_chdir(tmp_dir);
         pfa_get_full_name(tmp_dir, fullname);
         pfa_parse_to_pro_file(tmp_dir, name_opts.top_lib_dir);
         if (pfa_open_dir(tmp_dir) == PFA_E_NO_ERROR)
            {
             pfa_get_full_name(tmp_dir, fullname);
             path_ok = TRUE;
            }
         else
            {
             pro_printf("ERROR:  %s - invalid directory at %s\n", name_opts.top_lib_dir,
                         fullname);
             pfa_chdir(working_dir);
            }
        }
     else
        {
         pro_printf("ERROR: %s - invalid directory\n", library_pathname);
        }
     pfa_close(tmp_dir);
 } while (path_ok == FALSE);

 if (mydbg) pro_printf("found directory - %s\n", fullname);
 pfa_chdir(tmp_dir);
 pfa_get_full_name(pfa_get_cur_dir(), path);
 pfa_chdir(working_dir);

 pfa_free_pro_file(&tmp_dir);
 return(path);
}


/************************************************************/

main(argc, argv)
int argc;
char *argv[];
{
 char		lib_dir[K_PATH_SIZE];
 char		banner_line[3*K_LINESIZE];
 Pfa		*config_file;
 name_options	name_opts;
 int		error = NO_ERROR;
 int		i;

 if (argc > 1 && strcmp(argv[1], "-verify") == 0)
 {
   btk_printf("mkconfig.exe %s\n", get_internal_version());
   exit (0);
 }


 mydbg = FALSE;

 if ( (error = parse_name(argc, argv, &name_opts)) != NO_ERROR) {
     exit(IS_ERROR);
    }
 if ( (error = parse_options(argc, argv, &user_options, lib_dir)) != NO_ERROR) {
     exit(IS_ERROR);
    }

 pro_sprintf(banner_line,
	 "    %s  -  the Pro/ENGINEER %s search path generator",
	 name_opts.name, name_opts.banner);
 pro_printf("%s\n", banner_line);
 pro_printf("    ");
 for (i = 0; i < (int)strlen(banner_line) - 4; i++)
  {
   pro_printf("-");
  }
 pro_printf("\n\n");

 get_library_lp(lib_dir, name_opts, user_options);

 pfa_alloc_pro_file(&config_file);
 open_config_file(config_file);
 recursedir(lib_dir, config_file, name_opts);
 pfa_close(config_file);
 pfa_free_pro_file(&config_file);
}

/************************************************************/

int parse_name(argc, argv, name_opts)
int		argc;
char		*argv[];
name_options	*name_opts;
{
 int	error = NO_ERROR;
 char	name[K_NAME_SIZE];
 Pfa	*name_pfa;
 char   *p;

 pfa_alloc_pro_file(&name_pfa);

 pfa_parse_to_pro_file(name_pfa, argv[0]);

 pfa_get_name(name_pfa, name);

 if ( (strcmp(name, "mklibconfig") == 0) ||
      (strcmp(name, "mklibconfig.exe") == 0) )
    {
     strcpy(name_opts->banner, "Basic Library");
     strcpy(name_opts->top_lib_dir, "objlib");
     strcpy(name_opts->exclude_list, "feat_lib");
     strcpy(name_opts->name, name);
    } else if ( (strcmp(name, "mktoolconfig") == 0) ||
                (strcmp(name, "mktoolconfig.exe") == 0) )
    {
     strcpy(name_opts->banner, "Tooling Library");
     strcpy(name_opts->top_lib_dir, "mfglib");
     strcpy(name_opts->exclude_list, "");
     strcpy(name_opts->name, name);
    } else if ( (strcmp(name, "mkmoldconfig") == 0) ||
                (strcmp(name, "mkmoldconfig.exe") == 0) )
    {
     strcpy(name_opts->banner, "Mold Base Library");
     strcpy(name_opts->top_lib_dir, "moldlib");
     strcpy(name_opts->exclude_list, "");
     strcpy(name_opts->name, name);
    } else if ( (strcmp(name, "mkconnconfig") == 0) ||
                (strcmp(name, "mkconnconfig.exe") == 0) )
    {
     strcpy(name_opts->banner, "Connector Library");
     strcpy(name_opts->top_lib_dir, "connlib");
     strcpy(name_opts->exclude_list, "");
     strcpy(name_opts->name, name);
    } else if ( (strcmp(name, "mkfitconfig") == 0) ||
                (strcmp(name, "mkfitconfig.exe") == 0) )
    {
     strcpy(name_opts->banner, "Pipe Fitting Library");
     strcpy(name_opts->top_lib_dir, "fittinglib");
     strcpy(name_opts->exclude_list, "");
     strcpy(name_opts->name, name);
    } else if ( (strcmp(name, "mkhmfconfig") == 0) ||
                (strcmp(name, "mkhmfconfig.exe") == 0) )
    {
     strcpy(name_opts->banner, "Human Factors Library");
     strcpy(name_opts->top_lib_dir, "hmflib");
     strcpy(name_opts->exclude_list, "");
     strcpy(name_opts->name, name);
    } else
    {
     pro_printf("\n\nERROR:  unrecognized script name : %s\n\n", name);
     error = IS_ERROR;
    }
 return (error);
}
/************************************************************/

int parse_options(argc, argv, options, lib_dir)
int	argc;
char	*argv[];
int	*options;
char	*lib_dir;
{
 char	opt_str[K_PATH_SIZE];
 int	error = NO_ERROR;
 int	i = 1;

 while (i < argc) {
     strcpy(opt_str, argv[i]);
     if (opt_str[0] == '-') {
	 int j = 1;
	 while (opt_str[j] != '\0') {
	     switch (opt_str[j]) {
		 case 'h':
		 case '?':
		     pro_printf("Usage:  %s [ lib_dir ]\n", argv[0]);
		     error = TRUE;
		     break;
		 default:
		     break;
             }
	     j++;
	 }
     } else {
	 strcpy(lib_dir, opt_str);
	 *options = *options | LIB_DIR_SET;
     }
 i++;
 }
 return (error);
}


/************************************************************/

int rename_config_file(config_file)
Pfa	*config_file;
{
 int error = NO_ERROR;
 Pfa *renamed_file;
 pfa_alloc_pro_file(&renamed_file);

 pfa_put_name(renamed_file, OLD_FILE_NAME);
 pfa_put_extension(renamed_file, CONFIG_FILE_EXT);
 error = pfa_copy_file(config_file, THIS_VERSION, renamed_file, THIS_VERSION);
 switch (error)
    {
     case PFA_E_NO_ERROR:
       break;

     case PFA_E_EXIST:
         error = pfa_delete_file(renamed_file, THIS_VERSION);
         switch (error)
            {
             case PFA_E_NO_ERROR:
               break;

             case PFA_E_NOT_FOUND:
               break;

             case PFA_E_NO_ACCESS:
                 pro_printf("Copy config file permission denied.\n");
                 pro_printf("Error (%d) : during deletion of old config file.", error);
                 return (error);
                 break;
             case PFA_E_NOT_VALID:
             case PFA_E_INVALID_INPUT:
             case PFA_E_ERROR:
             default:
                 pro_printf("Error (%d) : during deletion of old config file.", error);
                 return(error);
                 break;
            }
       break;

     case PFA_E_NOT_VALID:
     case PFA_E_NOT_FOUND:
     case PFA_E_NO_ACCESS:
     case PFA_E_ERROR:
     default:
         pro_printf("Error (%d) : occured while renaming config file\n");
         return(error);
         break;
    }
 pfa_free_pro_file(&renamed_file);
}

/************************************************************/

int config_file_exists(config_file)
Pfa	*config_file;
{
 int	error = NO_ERROR;
 int	valid_choice = FALSE;
 char	buf[K_LINESIZE];
 char   tmp_buff[K_PATH_SIZE];

 pfa_get_full_name(config_file, tmp_buff);
 pro_printf("WARNING:  %s already exists\n", tmp_buff);
 do {
     pro_printf("          Rename it or Append to it (r/a)? [%s] ", "r");
     btk_fgets(buf, K_LINESIZE, btk_get_stdin());
if (mydbg) pro_printf("%d : %s", strlen(buf), buf);
     if ( ((int)strlen(buf) == 1) ||
          (buf[0] == 'r') || (buf[0] == 'R') ||
          (buf[0] == 'o') || (buf[0] == 'O') )
        {
         user_options = user_options | RENAME_OK;
         valid_choice = TRUE;
        }
     else if ( (buf[0] == 'a') || (buf[0] == 'A') )
        {
         user_options = user_options | APPEND_OK;
         valid_choice = TRUE;
        }
} while (valid_choice == FALSE);

 if (user_options & RENAME_OK)
     if (error = rename_config_file(config_file))  return (error);

 return (error);
}

/************************************************************/

int open_config_file(config_file)
Pfa *config_file;
{
 char	pfa_buff[K_PATH_SIZE];
 int	error = NO_ERROR;
 int	file_vers;

 error += pfa_put_name(config_file, CONFIG_FILE_NAME);
 error += pfa_put_extension(config_file, CONFIG_FILE_EXT);
 pfa_get_path(pfa_get_cur_dir(), pfa_buff);
 error += pfa_put_path(config_file, pfa_buff);
 if (pfa_get_device(pfa_get_cur_dir(), pfa_buff) != PFA_E_NO_ERROR ||
     pfa_put_device(config_file, pfa_buff) != PFA_E_NO_ERROR)
  {
   error++;
  }
 if (error)
    {
     pro_printf("Error while constructing config.pro's pfa_file");
     exit(error);
    }
if (mydbg)
    {
     pfa_get_full_name(config_file, pfa_buff);
     btk_printf("%s\n", pfa_buff);
    }

 error = pfa_get_file_vers(config_file, HIGHEST_VERSION, &file_vers);
 switch (error)
    {
     case PFA_E_NO_ERROR:
        if (error = config_file_exists(config_file)) return (error);
	break;
     case PFA_E_NOT_FOUND:    break;
     case PFA_E_NOT_VALID:
     case PFA_E_INVALID_INPUT:
     case PFA_E_NOT_ALLOWED:
     case PFA_E_NO_ACCESS:
     default:
	pro_printf("Error (%d) : Occured while creating config file\n", error);
    }
 if (user_options & APPEND_OK) {
     error = pfa_fopen(config_file, THIS_VERSION, "a");
     pfa_fprintf(config_file, "\n");
 } else {
     error = pfa_fopen(config_file, THIS_VERSION, "w");
 }
 switch (error)
    {
     case PFA_E_NO_ERROR:	break;
     case PFA_E_NOT_VALID:
     case PFA_E_INVALID_INPUT:
	 pro_printf("Unabel to open config file\n");
	 exit(1);
	 break;
     case PFA_E_CANT_OPEN:
     case PFA_E_ERROR:
     default:
	 pro_printf("Unable to open config file\n");
	 exit(1);
	 break;
    }
 return(NO_ERROR);
}

/************************************************************/

int recursedir(dir, config_file, name_opts)
char		*dir;
Pfa		*config_file;
name_options	name_opts;
{
 Pfa	*pfa_file;
 Pfa	*pfadir;
 char	ext[K_EXTENSION_SIZE];
 char	name[K_NAME_SIZE];
 char	path[K_PATH_SIZE];
 char	dev[50];
 char	fullname[K_PATH_SIZE];
 char	tmp_buff[K_PATH_SIZE];
 int	error = NO_ERROR;
 int	got_dir = FALSE;
 char	*fullname_ptr;
 char	*name_ptr;
 char	*ext_ptr;
 int	i;
 char	*char_ptr;
 char	*ex_name;
 int    do_exclude_dir = FALSE;

 pfa_alloc_pro_file(&pfadir);
 pfa_parse_to_pro_file(pfadir, dir);

 error = pfa_open_dir((Pfa *) pfadir);
 switch (error)
    {
     case PFA_E_NO_ERROR:	break;
     case PFA_E_NOT_VALID:
         pro_printf("WARNING:  skipping - %s - directory is not a valid\n", dir);
         return (error);
         break;
     case PFA_E_INVALID_INPUT:
         pro_printf("WARNING:  skipping - %s\n", dir);
         return (error);
         break;
     case PFA_E_NO_ACCESS:
         pro_printf("WARNING:  skipping - %s - directory is not accessable\n", dir);
         return (error);
         break;
     case PFA_E_CANT_OPEN:
         pro_printf("WARNING:  skipping - s - directory could not be opened\n", dir);
         return (error);
         break;
     case PFA_E_ERROR:
     default:
         pro_printf("WARNING %d:  skipping - %s\n", error, dir);
         return (error);
    }

 pfa_alloc_pro_file(&pfa_file);
 while(pfa_read_dir(pfadir, pfa_file) == PFA_E_NO_ERROR)
    {
     (void)pfa_get_device(pfa_file, dev);
     (void)pfa_get_path(pfa_file, path);
     (void)pfa_get_name(pfa_file, name);
     (void)pfa_get_extension(pfa_file,ext);

     if (strchr(name, EXTENSION_DELIMITER) != NULL)
	{
         if (mydbg) btk_printf("%s%c%s|%s  BADEXT\n", dir, DIR_DELIMITER, name, ext);
	 continue;
	}

     if ( (strcmp(ext, "prt") == 0) || (strcmp(ext, "PRT") == 0) ||
          (strcmp(ext, "asm") == 0) || (strcmp(ext, "ASM") == 0) )
        {
	 if (got_dir == FALSE)
	    {
	     if (mydbg)
             {
                 pfa_get_full_name(pfadir, tmp_buff);
                 pro_printf("search_path %s\n", tmp_buff);
             }
	     pfa_get_full_name(pfadir, tmp_buff);
	     pfa_fprintf(config_file, "search_path %s\n", tmp_buff);
	     got_dir = TRUE;
	    }
        }
     else
	{
         pfa_get_path(pfadir, tmp_buff);
         error+=pfa_put_path(pfa_file, tmp_buff);
         if (pfa_get_device(pfadir, tmp_buff) != PFA_E_NO_ERROR ||
             pfa_put_device(pfa_file, tmp_buff) != PFA_E_NO_ERROR)
          {
           error++;
          }
         if (error)
            {
             btk_printf("Errors while construction pfa_file\n");
	     error = NO_ERROR;
             continue;
            }

         pfa_get_full_name(pfa_file, fullname);

	 fullname_ptr = fullname;
	 name_ptr = name;
	 ext_ptr = ext;
	 while ( (char_ptr = strchr(fullname_ptr, PATH_SEPARATOR)) != NULL)
	    {
	     fullname_ptr = char_ptr + 1;
	    }
         for (i = 0; i < (int)strlen(name); i++)
	    {
             *fullname_ptr++ = *name_ptr++;
	    }
         if ((char_ptr = strchr(fullname_ptr, EXTENSION_DELIMITER)) != NULL)
	    {
	     fullname_ptr = char_ptr + 1;
	     for (i = 0; i < (int)strlen(ext); i++)
	        {
                 *fullname_ptr++ = *ext_ptr++;
	        }
            }
         if (mydbg) btk_printf("%s\n", fullname);
	 if ( pro_is_directory(wstr_ret(fullname)) )
	    {
              char	tok_list[5*K_NAME_SIZE];

              strcpy(tok_list, name_opts.exclude_list);
	      if (mydbg) btk_printf("\nDIR...  %s      |%s|%s|%s|\n",
				 fullname, path, name, ext);
              ex_name = strtok(tok_list, " ");
              if (ex_name != NULL) do
                 {
                  if (strcmp(ex_name, name) == 0) do_exclude_dir = TRUE;
                 }
                 while( (ex_name = strtok(NULL, " ")) != NULL);
              if (do_exclude_dir)
                 {
                  do_exclude_dir = FALSE;
                 } else {
                  recursedir(fullname, config_file, name_opts);
                 }
            }
         pfa_free_pro_file(&pfa_file);
        }
     pfa_free_pro_file(&pfa_file);
     pfa_alloc_pro_file(&pfa_file);
    }
 pfa_close(pfadir);
 pfa_free_pro_file(&pfadir);

 return(TRUE);
}
