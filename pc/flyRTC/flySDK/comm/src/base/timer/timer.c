/*
* timer.c
*
* Created on: 2013-9-13
*      Author: gonghuojin
*/
#include "timer.h"
#include <time.h>

typedef struct timer_handle_t
{
    long 			m_millisecond;
	int  			m_cnt;
	int 			m_fd;
	int				m_handleid;
	tm_bool 		m_isstart;
	tm_bool 		m_isonlyonce;
	tm_bool 		m_isset;
	tm_bool 		thread_exit;
    ms_thread_t 	thread_timer;
	ms_mutex_t  	timer_mutex;
	cb_time_func 	m_callback; 
}timer_handle_t;


#define TIMER_MTU_VALUE  	 10*1000     //10 ms
#define DEFAULT_PORT_VALUE   5000

#define MAX_TIMER_COUNT		20	
#define ASSERT_HANDLE_ID(x)	 if (((x) < 0) || ((x) >= MAX_TIMER_COUNT)) return -1;

static timer_handle_t* gTimerHandleList[MAX_TIMER_COUNT];
static ms_mutex_t gTimerHanlesMutex;
	
static int tm_createsocket(void);
static int tm_find_available_from_list(void);
static void tm_init_timer_handles(void);
static void tm_release_timer_handles(void);
static void tm_reset_handle(timer_handle_t* thandle);
static int tm_append_handle(timer_handle_t* thandle);
static void tm_release_handle(timer_handle_t* thandle);

static int tm_set_timer(timer_handle_t* th, tm_bool isonlyonce, long millisecond, cb_time_func func);
static tm_bool tm_is_full(void);
static void tm_stop(timer_handle_t* th);

static void* thread_proc(void* arg);

#ifdef WIN32
static int tm_win32_init_sockets(void);
static int tm_win32_clear_sockets(void);
#endif

#ifdef WIN32
int tm_win32_init_sockets(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int i;
	
	wVersionRequested = MAKEWORD(2,0);
	if( (i = WSAStartup(wVersionRequested,  &wsaData))!=0)
	{
		ms_error("Unable to initialize windows socket api, reason: %d",i);
		return -1;
	}
	return 0;
}

int tm_win32_clear_sockets(void)
{
	WSACleanup();
	return 0;
}

#endif

int tm_createsocket(void)
{
   struct sockaddr_in addr;
   int fd;
   int port = DEFAULT_PORT_VALUE;
    
   fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if ( fd < 0)
   {
	  ms_error("stun_udp: Could not create a UDP socket");
      return -1;
   }
   
   do {
	   memset((char*) &(addr),0, sizeof((addr)));
	   addr.sin_family = AF_INET;
	   addr.sin_addr.s_addr = htonl(INADDR_ANY);
	   addr.sin_port = htons(port);
	    
	   if ( bind( fd,(struct sockaddr*)&addr, sizeof(addr)) != 0 )
	   {
	      port++;
		  continue;
	   }

	   break;
   }while(1);

   ms_message("opened port %i with fd %i\n", port, fd);
   
   return fd;
}

int tm_find_available_from_list(void)
{
	int i = 0;

	for (i = 0; i < MAX_TIMER_COUNT; i++)
	{
		if (gTimerHandleList[i] == NULL)
			break;	
	}

	if (i == MAX_TIMER_COUNT)
		return -1;

	return i;
}

void tm_init_timer_handles(void)
{
	int i = 0;
	
	ms_mutex_init(&gTimerHanlesMutex, NULL);
	
	for (i = 0; i < MAX_TIMER_COUNT; i++)
	{
		gTimerHandleList[i] = NULL;
	}
}

void tm_release_timer_handles(void)
{
	int i = 0;

	for (i = 0; i < MAX_TIMER_COUNT; i++)
	{
		if (gTimerHandleList[i] != NULL)
		{
			tm_delete_timer(i);
		}
	}

	ms_mutex_destroy(&gTimerHanlesMutex);
}

void tm_reset_handle(timer_handle_t* thandle)
{
	thandle->thread_exit = tm_false;
	thandle->m_isstart = tm_false;
	thandle->m_isset = tm_false;
	thandle->m_cnt = 0;
	thandle->m_millisecond = 0;
	thandle->m_fd = -1;
	thandle->m_handleid = -1;
	thandle->m_callback = NULL;
	thandle->thread_timer = 0;
	
	ms_mutex_init(&thandle->timer_mutex, NULL);
}


int tm_set_timer(timer_handle_t* th, tm_bool isonlyonce, long millisecond, cb_time_func func)
{
	ms_mutex_lock(&th->timer_mutex);
	th->m_fd = tm_createsocket();
	th->m_isonlyonce = isonlyonce;
	th->m_millisecond = millisecond;
	th->m_cnt = (millisecond*1000)/(TIMER_MTU_VALUE);
	if (th->m_cnt == 0) th->m_cnt = 1;
	th->m_callback = func;
	th->m_isset = tm_true;
	ms_mutex_unlock(&th->timer_mutex);
	
	return 0;
}

int tm_append_handle(timer_handle_t* thandle)
{
	int handle_id = -1;

	handle_id = tm_find_available_from_list();
	if (handle_id < 0)
		return -1;
	
	thandle->m_handleid = handle_id;
	gTimerHandleList[handle_id] = thandle;

	return handle_id;
}


void tm_release_handle(timer_handle_t* thandle)
{
	if (thandle != NULL)
	{
		tm_stop(thandle);

		ms_mutex_lock(&thandle->timer_mutex);
		if (thandle->m_fd > 0)
		{
			close_socket(thandle->m_fd);
			thandle->m_fd = -1;
		}
		ms_mutex_unlock(&thandle->timer_mutex);
		ms_mutex_destroy(&thandle->timer_mutex);
	}
}


tm_bool tm_is_full(void)
{
	return (tm_find_available_from_list() < 0 ? tm_true : tm_false);
}


void tm_stop(timer_handle_t* th)
{
	if (th == NULL)
		return ;
	
	if (th->m_isstart)
	{
		int waiting_time = 2000;
		int *ptime = &waiting_time;
		
		ms_mutex_lock(&th->timer_mutex);
		th->thread_exit = tm_true;
 #ifndef ANDROID
		ms_thread_join(th->thread_timer, (void**)&ptime);
 #endif
		th->thread_timer = 0;
		th->m_isstart = tm_false;
		ms_mutex_unlock(&th->timer_mutex);
	}
}


int tm_start(timer_handle_t* th)
{
	if (NULL == th)
		return -1;
	
	if (!th->m_isstart && th->m_isset)
	{
		ms_mutex_lock(&th->timer_mutex);
		if (th->m_fd < 0)
		{
			th->m_fd = tm_createsocket();
			if (th->m_fd < 0)
			{
				ms_error("SetTimer: tm_createsocket faild!!!");
				ms_mutex_unlock(&th->timer_mutex);
				return -1;
			}
		}
		th->m_isstart = tm_true;
		th->thread_exit = tm_false;
		ms_thread_create(&th->thread_timer, NULL, thread_proc, (void*)th);
		ms_mutex_unlock(&th->timer_mutex);
	}

	return 0;
}

void* thread_proc(void* arg)
{
	int cnt = 0;
	fd_set fs_read;
	struct timeval tempval;
	timer_handle_t* th = (timer_handle_t*)arg;

	if (th == NULL) 
		return NULL;
	
	cnt = th->m_cnt ? th->m_cnt : 1;

    while (!th->thread_exit)
    {
		if (cnt-- != 0)
		{
			tempval.tv_sec = 0;
			tempval.tv_usec = TIMER_MTU_VALUE;

			FD_ZERO(&fs_read);
            FD_SET(th->m_fd, &fs_read);

			if (select(0, &fs_read, NULL, NULL, &tempval) < 0)
			{
				ms_mutex_lock(&th->timer_mutex);
				if (th->m_fd > 0)
					close_socket(th->m_fd);
				th->m_fd = -1;
				ms_mutex_unlock(&th->timer_mutex);
				ms_error("thread_proc: Select error!! reCreate Socket faild!!");
				break;
			}
		}
		else
		{
			if (th->m_callback)
				th->m_callback(th->m_handleid);
			
			if (th->m_isonlyonce)
			{
				ms_mutex_lock(&th->timer_mutex);
				//thread_timer = NULL;
				th->m_isstart = tm_false;
				th->thread_exit = tm_true;
				ms_mutex_unlock(&th->timer_mutex);
			}
			else
			{
				cnt = th->m_cnt ? th->m_cnt : 1;
			}
		}
    }

	return NULL;
}


int tm_init(void)
{
	int i = 0;
	
#ifdef WIN32
	tm_win32_init_sockets();
#endif
	/*init timer handle list*/
	tm_init_timer_handles();

	return 0;
}

void tm_destroy(void)
{
#ifdef WIN32
	tm_win32_clear_sockets();
#endif

	/*release timer handle list*/
	tm_release_timer_handles();
}


int tm_create_timer(tm_bool isonlyonce, long m_millisecond, cb_time_func func)
{
	timer_handle_t* timerHD = NULL;

	timerHD = ms_malloc0(sizeof(timer_handle_t));
	
	if (timerHD == NULL)
	{
		ms_error("tm_create_timer: new timer handle faild!!");
		return -1;
	}
	
	if (tm_is_full())
	{
		ms_error("tm_create_timer: timer list is full, can`t create!!");
		ms_free(timerHD);
		return -1;
	}

	tm_reset_handle(timerHD);
	tm_set_timer(timerHD, isonlyonce, m_millisecond, func);
	
	return tm_append_handle(timerHD);
}

int tm_delete_timer(int tHandleid)
{
	ASSERT_HANDLE_ID(tHandleid);

	ms_mutex_lock(&gTimerHanlesMutex);
	if (gTimerHandleList[tHandleid] != NULL)
	{
		tm_release_handle(gTimerHandleList[tHandleid]);
		ms_free(gTimerHandleList[tHandleid]);
		gTimerHandleList[tHandleid] = NULL;
	}
	ms_mutex_unlock(&gTimerHanlesMutex);
	
	return 0;
}

int tm_start_timer(int tHandleid)
{
	ASSERT_HANDLE_ID(tHandleid);
	
	ms_mutex_lock(&gTimerHanlesMutex);
	if (gTimerHandleList[tHandleid] != NULL)
	{
		tm_start(gTimerHandleList[tHandleid]);
	}
	ms_mutex_unlock(&gTimerHanlesMutex);

	return 0;
}

int tm_stop_timer(int tHandleid)
{
	ASSERT_HANDLE_ID(tHandleid);
		
	ms_mutex_lock(&gTimerHanlesMutex);
	if (gTimerHandleList[tHandleid] != NULL)
	{
		tm_stop(gTimerHandleList[tHandleid]);
	}
	ms_mutex_unlock(&gTimerHanlesMutex);

	return 0;

}


