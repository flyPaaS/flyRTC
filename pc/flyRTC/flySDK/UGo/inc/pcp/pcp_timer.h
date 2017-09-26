/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_timer.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/27
  Last Modified :
  Description   : pcp timer file header
  Function List :
  History       :
  1.Date        : 2013/9/27
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_TIMER_H
#define PCP_TIMER_H
#include "pcp_comm.h"

#include "timer.h"

PCP_EXTERN_C_START

void null_timer();

#define TIMEOUT_T0_V  30*1000 	//default call req timeout 30s
#define TIMEOUT_T1_V  120*1000	//default call anwer timeout 120s
#define TIMEOUT_T2_V  15*1000 	//default call ack timeout 15s
#define TIMEOUT_T3_V  10*1000 	//default push call timeout 10s
#define TIMEOUT_T4_V  10*1000 	//default conference call rsp timeout 10s
#define TIMEOUT_T5_V  3 * 1000  // default in trans rsp timeout 3s

typedef enum timer_type_t
{
	TIMEOUT_T0 = 0,
	TIMEOUT_T1 = 1,
	TIMEOUT_T2 = 2,
	TIMEOUT_T3 = 3,
	TIMEOUT_T4 = 4,
	TIMEOUT_T5 = 5
} timer_type_t;

typedef enum timer_mode_t
{
	TP_PERIOD	= 0,
	TP_ONLYONCE = 1
}timer_mode_t;

typedef struct pcp_timer_t
{
	int  time_id;
	char timer_name[64];
	cb_time_func timer_cb;	
}pcp_timer_t;

typedef struct pcp_timer_vtable_t
{
	pcp_timer_t* t0; //max timer for wait call rsp 
	pcp_timer_t* t1; //max timer for wait call answer
	pcp_timer_t* t2; //max timer for wait call ack
	pcp_timer_t* t3; //max timer for wait call req(by push) 
	pcp_timer_t* t4; //max timer for wait conference call rsp
	pcp_timer_t* t5; // timer for in trans rsp
}pcp_timer_vtable_t;


int init_timer(IN cb_time_func tm_cb);

void destroy_timer();

int start_timer(IN timer_type_t time_type);

int stop_timer(IN timer_type_t time_type);

int stop_all_timers(void);

OUT int get_timer(IN int time_id);


PCP_EXTERN_C_END

#endif

