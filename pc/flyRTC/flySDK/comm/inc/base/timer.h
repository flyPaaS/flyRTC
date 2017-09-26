/*
* timer.h
*
* Created on: 2013-9-13
*      Author: gonghuojin
*/
#ifndef CTIMER_H_
#define CTIMER_H_
#include "mscommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef tm_bool 
#define tm_bool 	int
#endif

#ifndef tm_true
#define tm_true		1
#endif

#ifndef tm_false
#define tm_false	0
#endif

typedef void (*cb_time_func)(int handleid);

int  tm_init(void);

void tm_destroy(void);

int tm_create_timer(tm_bool isonlyonce, long m_millisecond, cb_time_func func);

int tm_delete_timer(int tHandleid);

int tm_start_timer(int tHandleid);

int tm_stop_timer(int tHandleid);

#ifdef __cplusplus
}
#endif

#endif /* CTIMER_H_ */


