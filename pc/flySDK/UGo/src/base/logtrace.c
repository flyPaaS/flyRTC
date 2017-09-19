#include "logtrace.h"

static char 			g_logpath[512] = {0};
static FILE*			g_logfp	= NULL;

static int blog_open(int level, const char *logpath);
static void blog_close(void);


int blog_open(int level, const char *logpath)
{
	if (g_logfp != NULL)
		blog_close();

	if (logpath != NULL)
	{
		strcpy(g_logpath, logpath);
		g_logfp = fopen(g_logpath, "a+");		
	}
	
	if ( g_logfp == NULL) 
	{
	    g_logfp = stdout;
	    ortp_set_log_max_file_sizes(0);
	}
	else
	{
	    // set log file max size to 3M
	    ortp_set_log_max_file_sizes(3 * 1024 * 1024);
	}
	ortp_set_log_file(g_logfp);
	ortp_set_log_level_mask(level);
	ortp_set_log_handler(__ortp_logv_out);

	return 0;
}


void blog_close(void)
{
	if (g_logfp != NULL && g_logfp != stdout)
	{
		fprintf(g_logfp,"\n\n");
		fflush(g_logfp);
		fclose (g_logfp);
		g_logfp = NULL;
	}
}


int blog_init(void)
{
	static bool_t initialized=FALSE;
	if (initialized) return 0;
	initialized=TRUE;

	ms_init();
	
	return 0;
}

void blog_destroy(void)
{
	blog_close();
	ms_exit();
}


int blog_enable_logs(int level, const char *logpath)
{
	return blog_open(level, logpath);
}


