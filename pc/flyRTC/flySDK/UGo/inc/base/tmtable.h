/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : tm.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2014/5/28
  Last Modified :
  Description   : uc timer head file
  Function List :
  History       :
  1.Date        : 2014/5/28
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef UC_TMTABLE_H
#define UC_TMTABLE_H

#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

#ifndef TM_OK
#define TM_OK 0
#endif

#ifndef TM_ERR
#define TM_ERR -1
#endif

#define TM_P2P_DETECTION_T0_V  8*1000 	/*ice detection timeout tm value 8s*/
#define TM_P2P_CHECKLINE_T1_V  30*1000  /*ice check line state tm value 30s*/
#define TM_P2P_SENDPING_T2_V  	1*1000  /*ice send ping msg tm value 1s*/
#define TM_SEND_IN_TRANS_REQ_T3_V   1*1000   /* send in trans req in 1s period  */

typedef enum tm_mode_t
{
	TM_PERIOD_TP	= 0,
	TM_ONLYONCE_TP  = 1
}tm_mode_t;

typedef struct tm_t
{
	int  tm_id;
	char tm_name[64];
	cb_time_func tm_cb;	
}tm_t;

typedef struct tm_vtable_t
{
	tm_t* t0; //max timer for wait ice callback 
	tm_t* t1; //max timer for wait check line state
	tm_t* t2; //max timer for wait send ping msg
	tm_t* t3; // timer for send in trans req
}tm_vtable_t;

typedef enum tm_type_t
{
	TM_P2P_DETECTION_T0 = 0,
	TM_P2P_CHECKLINE_T1 = 1,
	TM_P2P_SENDPING_T2  = 2,
	TM_SEND_IN_TRANS_REQ_T3 = 3
}tm_type_t;

typedef struct tm_event_node_t
{
	int  tm_type;
	char tm_name[64];
}tm_event_node_t;
#define TM_EVENT_NODE_T_SIZE 	(sizeof(tm_event_node_t))


/**p2p timeout  callback prototype*/
typedef void (*tm_cb_t)(int timeid);

int tmt_init(IN tm_cb_t tm_cb);

void tmt_destroy(void);

int tmt_start_timer(IN tm_type_t time_type);

int tmt_stop_timer(IN tm_type_t time_type);

int tmt_stop_all_timers(void);

OUT int tmt_get_timer_type(IN int time_id);

int tmt_get_timer_name(IN int time_id, OUT char* time_name);

#ifdef __cplusplus
}
#endif

#endif


