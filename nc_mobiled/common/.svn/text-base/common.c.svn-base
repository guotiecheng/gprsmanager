
#include  <common/common.h>

#if 0
void gblSetState(int holder)
{
    pthread_mutex_lock(&g_stGprsStatus.f_lock);
    g_stGprsStatus.ucGprsStatus= holder;
    pthread_mutex_unlock(&g_stGprsStatus.f_lock);
}
#endif

void  usage(char *name)
{
 char             *progname = NULL; 
 char             *ptr = NULL;

 ptr = strdup(name);
 progname = basename(ptr);

 printf("Usage: %s [OPTION]...\n", progname);
 printf(" %s is a daemon program running on GHL L-Serials device, which used to ", progname);
 printf("auto start ppp connection.\n");

 printf("Mandatory arguments to long options are mandatory for short options too:\n");
 printf("  -f, --foreground       Run this program on foreground.\n");
 printf("  -d, --debug=[1,2,3,4]  Display debug messages, log level specified by the argument:\n"); 
 printf("                          0[Disable] 1[Critcal] 2[Error] 3[debug] 4[Info]\n");
 printf("  -v, --version          Display the program version number\n");
 printf("  -h, --help             Display this usage information\n");

 print_version(progname);

 free(ptr);

 exit(0);
}

void print_version (char *name)
{
     char             *progname = NULL; 
     char             *ptr = NULL;
        
     ptr = strdup(name);  
     progname = basename(ptr); 

//     printf("%s version: %d.%d.%d Build %04d\n",  progname, MAJOR, MINOR, REVER, SVNVER);
     printf("Copyright (C) 2010 by GHL System Behard <supports@ghlsystems.com>\n");

     free(ptr);
     exit(0);
}
/* For linux*/
void daemonforLinux(unsigned char ucDebug)
{
   int         iRet = 0;
   int         fd   = -1; 

   if (1 == getppid())/* already a daemon */
       return;
   
   iRet = fork();
   if (0 > iRet)/* fork error*/ 
       _exit(1);    

   if (0 < iRet)/* parent exits*/
       _exit(0);

   /* child daemon continues*/
   setsid();/*obtain a new group */ 
   
   if (0x00 == ucDebug)
   {   
       /* Close all the file description.*/     
       for (iRet = getdtablesize(); 0 < iRet; --iRet)
       {
           fd = iRet -1;
           close(fd);
       }

       fd = open("dev/null", O_RDWR);/* Redirect Standard input [0] to /dev/null*/
       dup(fd);/* Redirect Standard output [1] to /dev/null*/
       dup(fd);/*Redirect  Standard output [2] to /dev/null*/
   }
   umask(0);

   chdir("/");
}


/*For uClinux doesn't support daemon() function, so we need implement it by ourself.*/
void daemonforUCLinux (unsigned char ucDebug, int argc, char **argv)
{
   int         iRet = 0;
   int         fd = -1;

   if (1 == getppid ()) /* already a daemon */
   {
        setsid (); /* set new process group */

        if (0x00 == ucDebug)
        {
            /*Close all the file description.*/
            for (iRet = getdtablesize (); 0 <= iRet; --iRet) 
            {
                 fd = iRet;  
                 close(fd);
            }

            fd = open("/dev/null", O_RDWR); /*Redirect Standard input [0] to /dev/null */
            dup (fd); /*Redirect Standard output [1] to /dev/null */
            dup (fd); /*Redirect Standard error [2] to /dev/null */
        }

        umask (027); /* set newly created file mode mask */
        chdir("/"); // change running directory

        return ;
   }
   else
   {
       iRet = vfork (); 

       if (0 > iRet) _exit (1); /* fork error */
       if (0 < iRet) _exit (0); /* parent exists */

       /*Child process(daemon) continue and fork again.*/

       iRet = vfork ();
       if (0 > iRet) _exit (1); /* fork error */
       if (0 < iRet) _exit (0); /* parent exists */

       /* Parent exit, so child process parent pid should be init process(pid is 1) */
       execv (argv[0], argv); /*Run the pograme again.*/
       _exit (1); /*If execv() returned, then something error happend.*/
   }
}

/*
void signal_handler (int signo)
{
   logs(LOG_INFO, "Receive signal %d\n", signo);
   if (SIGTERM==signo || SIGINT==signo)
   {
       g_ucStop = 0x01;      
   }
   else if (SIGSEGV == signo)
   {
       _exit (1);
   }
   else if (SIGUSR1 == signo)
   {
       printf("Catch the signo SIGUSR1!\n");
                
   
   
   }
}
*/
pid_t get_daemon_pid(const char *pid_path)
{
     FILE *f;
     pid_t pid;


     /* pids are only 15 bits but 4294967296 
      * (32 bits in case of a new system use it) is on 10 bytes 
      */

     if ((f = fopen(pid_path, "rb")) != NULL)
     {
             char pid_ascii[PID_ASCII_SIZE];
             (void)fgets(pid_ascii, PID_ASCII_SIZE, f);
             (void)fclose(f);
             pid = atoi(pid_ascii);
     }
     else
     {
//             logs(LOG_CRIT, "Can't open %s: %s\n", pid_path, strerror(errno));
	     nc_log (&logger, NC_MOBILED_NAME, NA_LOG_WARNING, "@%04d Can't open %s: %s", __LINE__, pid_path,strerror(errno));
             return -1;
     }

     return pid;
} /* GetDaemonPid */


int is_myself_running(const char *pid_path)
{
     int             retVal = -1;
     struct          stat fStatBuf;

     retVal = stat(pid_path, &fStatBuf);
     if(0 == retVal)
     {
         pid_t       pid = -1;

         pid = get_daemon_pid(pid_path);

         if (pid > 0) /*Process pid exist*/
         {
             if (kill(pid, 0) == 0)
             {
//                  logs(LOG_CRIT, "file \"%s\" already exists.\n"
  //                     "\t\tAnother program (pid: %d) seems to be running.\n", pid_path, pid);
	     nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d file \"%s\" already exists.", __LINE__,pid_path);	
                  return RUNNING;
             }
             else /*Send signal to the old process get no reply.*/
             {
//                  logs(LOG_DEBG, "Program (pid: %d) seems exit. Do some clean work now.\n", pid);
	   	  nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Program (pid: %d) seems exit. Do some clean work now.", __LINE__, pid);	
                  remove(pid_path);
                  return NOT_RUNNING;
             }
         }
         else if(0 == pid)
         {
//		 logs(LOG_INFO, "Last Running PID file is empty, clean it now.\n");
	         nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Last Running PID file is empty, clean it now.", __LINE__);	
                  remove(pid_path);
                  return NOT_RUNNING;
         } 
         else /*Read pid from file "pid_path" failure*/
         {
//                  logs(LOG_CRIT, "File \"%s\" already exists, but I can not read the process pid."
//                                 "Maybe another program is running?\n"
  //                               "If the program is not running, remove it and try again.\n" , 
  //                               pid_path);
                  return RUNNING;
         }
     }

     return NOT_RUNNING;
}


int record_runpid_file(const char *pid_path)
{
    struct stat     fStatBuf;
    int             fd = -1;
    int             mode = S_IROTH | S_IXOTH | S_IRGRP | S_IXGRP | S_IRWXU;
    char            ipc_dir[64]={0};

    strncpy(ipc_dir, pid_path, 64);
    dirname(ipc_dir); /*dirname() will modify ipc_dir and save the result*/

    /* If folder "/tmp/mobiled doesnot exist, then we will create it"*/
    if(stat(ipc_dir, &fStatBuf) < 0)
    {
        if(mkdir(ipc_dir, mode) < 0)
        {
//             logs(LOG_CRIT, "cannot create %s: %s\n", ipc_dir, strerror(errno));
	     nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d cannot create %s: %s", __LINE__, ipc_dir, strerror(errno));	
             return -1;
        }
        (void)chmod(ipc_dir, mode);
    }
//    logs(LOG_INFO, "Create shared folder %s successfully.\n", ipc_dir);
    nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Create shared folder %s successfully.", __LINE__,ipc_dir);	

    /*Create the process running PID file*/
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if( (fd=open(pid_path, O_RDWR|O_CREAT|O_TRUNC, mode)) >= 0)
    {
         char pid[PID_ASCII_SIZE];
         snprintf(pid, sizeof(pid), "%u\n", (unsigned) getpid());
         write(fd, pid, strlen(pid));
         close(fd);

//         logs(LOG_INFO, "Create PID file %s and write process pid[%u]\n", pid_path, getpid());
	 nc_log (&logger, NC_MOBILED_NAME, NA_LOG_NORMAL, "@%04d Create PID file %s and write process pid[%u]", __LINE__, pid_path, getpid());	
    }
    else
    {
//         logs(LOG_CRIT, "cannot create %s: %s\n", pid_path, strerror(errno));
	 nc_log (&logger, NC_MOBILED_NAME, NA_LOG_ERROR, "@%04d cannot create %s: %s", __LINE__, pid_path, strerror(errno));	
         return -1;
    }

    return 0;
}


