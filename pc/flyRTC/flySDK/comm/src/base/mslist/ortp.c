 /*
  The oRTP library is an RTP (Realtime Transport Protocol - rfc3550) stack.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ortp.h"
#include <time.h>
#ifdef ANDROID
#include <android/log.h>
#endif


//RtpScheduler *__ortp_scheduler;
unsigned int __ortp_log_mask = ORTP_WARNING | ORTP_ERROR | ORTP_FATAL;

OrtpLogFunc ortp_logv_out=NULL;


static void init_random_number_generator(){
	struct timeval t;
	gettimeofday(&t,NULL);
	srandom(t.tv_usec+t.tv_sec);
}


#ifdef WIN32
static bool_t win32_init_sockets(void){
	WORD wVersionRequested;
	WSADATA wsaData;
	int i;
	
	wVersionRequested = MAKEWORD(2,0);
	if( (i = WSAStartup(wVersionRequested,  &wsaData))!=0)
	{
		ortp_error("Unable to initialize windows socket api, reason: %d (%s)",i,getWinSocketError(i));
		return FALSE;
	}
	return TRUE;
}

static bool_t win32_clear_sockets(void){
	WSACleanup();
	return TRUE;
}

#endif

/**
 *	Initialize the oRTP library. You should call this function first before using
 *	oRTP API.
**/
void ortp_init()
{
	static bool_t initialized=FALSE;
	if (initialized) return;
	initialized=TRUE;

#ifdef WIN32
	win32_init_sockets();
#endif

}

void ortp_destroy(void)
{
#ifdef WIN32
	win32_clear_sockets();
#endif
}


static FILE *__log_file=0;
static size_t size_in_bytes_ = 0;
static size_t max_size_in_bytes = 0;
static size_t row_count_text = 0;

/**
 *@param file a FILE pointer where to output the ortp logs.
 *
**/
void ortp_set_log_file(FILE *file)
{
	__log_file=file;
}


/**
 *@param func: your logging function, compatible with the OrtpLogFunc prototype.
 *
**/
void ortp_set_log_handler(OrtpLogFunc func){
	ortp_logv_out=func;
}

/**
 * @ param levelmask a mask of ORTP_DEBUG, ORTP_MESSAGE, ORTP_WARNING, ORTP_ERROR
 * ORTP_FATAL .
**/
void ortp_set_log_level_mask(int levelmask){
	__ortp_log_mask=levelmask;
}

void ortp_set_log_max_file_sizes(size_t bytes)
{
    max_size_in_bytes = bytes;
}

char * ortp_strdup_vprintf(const char *fmt, va_list ap)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 200;
	char *p,*np;
#ifndef WIN32
	va_list cap;/*copy of our argument list: a va_list cannot be re-used (SIGSEGV on linux 64 bits)*/
#endif
	if ((p = (char *) ortp_malloc (size)) == NULL)
		return NULL;
	while (1)
	{
		/* Try to print in the allocated space. */
#ifndef WIN32
		va_copy(cap,ap);
		n = vsnprintf (p, size, fmt, cap);
		va_end(cap);
#else
		/*this works on 32 bits, luckily*/
		n = vsnprintf (p, size, fmt, ap);
#endif
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		//printf("Reallocing space.\n");
		/* Else try again with more space. */
		if (n > -1)	/* glibc 2.1 */
			size = n + 1;	/* precisely what is needed */
		else		/* glibc 2.0 */
			size *= 2;	/* twice the old size */
		if ((np = (char *) ortp_realloc (p, size)) == NULL)
		  {
		    free(p);
		    return NULL;
		  }
		else
		  {
		    p = np;
		  }
	}
}

char *ortp_strdup_printf(const char *fmt,...){
	char *ret;
	va_list args;
	va_start (args, fmt);
	ret=ortp_strdup_vprintf(fmt, args);
 	va_end (args);
	return ret;
}

#if	defined(WIN32) || defined(_WIN32_WCE)
#define ENDLINE "\n"
#else
#define ENDLINE "\n"
#endif

#if	defined(WIN32) || defined(_WIN32_WCE)
void ortp_logv(int level, const char *fmt, va_list args)
{
	if (ortp_logv_out!=NULL && ortp_log_level_enabled(level))
		ortp_logv_out(level,fmt,args);
#if !defined(_WIN32_WCE)
	if ((level)==ORTP_FATAL) abort();
#endif
}
#endif

#ifdef ANDROID
#define print_tag "linphone"
void android_log_handler(OrtpLogLevel lev, const char *msg){
#define MAX_LOGCAT_OUTPUT_LEN 1024
	int prio;
	int msglen = 0;
	int index = 0;
	char tmpOut[MAX_LOGCAT_OUTPUT_LEN] = {0};
	
	if ( NULL == msg )
	{
	    return;
	}

	msglen = strlen(msg);
	
	switch(lev){
    	case ORTP_DEBUG:	prio = ANDROID_LOG_DEBUG;	break;
    	case ORTP_MESSAGE:	prio = ANDROID_LOG_INFO;	break;
    	case ORTP_REPORT:  	prio = ANDROID_LOG_INFO;	break;
    	case ORTP_WARNING:	prio = ANDROID_LOG_WARN;	break;
    	case ORTP_ERROR:	prio = ANDROID_LOG_ERROR;	break;
    	case ORTP_FATAL:	prio = ANDROID_LOG_FATAL;	break;
    	default:			prio = ANDROID_LOG_DEFAULT;	break;
	}

    while ( index < msglen )
    {
        memset(&tmpOut, 0x00, MAX_LOGCAT_OUTPUT_LEN);
        if ( msglen <= (index + MAX_LOGCAT_OUTPUT_LEN) )
        {
            strncpy(tmpOut, &msg[index], msglen - index );
        }
        else
        {
            strncpy(tmpOut, &msg[index], MAX_LOGCAT_OUTPUT_LEN);
        }

        index += MAX_LOGCAT_OUTPUT_LEN;
        __android_log_print(prio, print_tag, "%s", tmpOut);
    }
	
}
#endif

void __ortp_logv_out(OrtpLogLevel lev, const char *fmt, va_list args){
#define MAX_MSG_LEN 1500
	const char *lname="undef";
	char *msg = NULL;
	char msgBuf[MAX_MSG_LEN] = {0};
	int length = 0;	
	time_t timer;
	struct tm *l = NULL;
	
	if ( __log_file == NULL ) __log_file = stderr;
	switch(lev){
		case ORTP_DEBUG:
			lname="debug";
			break;
		case ORTP_MESSAGE:
			lname="message";
			break;
		case ORTP_WARNING:
			lname="warning";
			break;
		case ORTP_ERROR:
			lname="error";
			break;
		case ORTP_FATAL:
			lname="fatal";
			break;
		case ORTP_REPORT://jkh
			lname="report";
			break;			
		default:
			ortp_fatal("Bad level !");
	}
	
	msg = ortp_strdup_vprintf(fmt, args);

#ifdef ANDROID  //jkh
	android_log_handler(lev, msg);
#endif	

	timer = time(NULL);
	l = localtime(&timer);

    if ( row_count_text == 0 )
    {
        length = snprintf(msgBuf, MAX_MSG_LEN, ENDLINE "Local Date: %d-%d %d:%d:%d\n" ENDLINE, \
	                l->tm_mon+1, l->tm_mday, l->tm_hour, l->tm_min, l->tm_sec);
	    length = fwrite(msgBuf, 1, length, __log_file);
        size_in_bytes_ += length;
        row_count_text++;
    }
    
	length = snprintf(msgBuf, MAX_MSG_LEN, "%d-%d %d:%d:%d %s-%s" ENDLINE, \
	    l->tm_mon+1, l->tm_mday, l->tm_hour, l->tm_min, l->tm_sec, lname, msg);

#ifdef __APPLE__
	printf("%s", msgBuf);
#endif
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
	OutputDebugString(msgBuf);
	// added for win32 console print
	printf("%s", msgBuf);
#endif

	if ( max_size_in_bytes > 0 
        && (size_in_bytes_ + length) > max_size_in_bytes )
    {
        rewind(__log_file);
        size_in_bytes_ = 0;
        row_count_text = 0;
    }

    if ( length > 0 )
    {
        length = fwrite(msgBuf, 1, length, __log_file);
        size_in_bytes_ += length;
        row_count_text++;
        if ( !(row_count_text % 50 ))
        {
    	    fflush(__log_file);
    	}
	}
	ortp_free(msg);
}


