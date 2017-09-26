/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_session.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/23
  Last Modified :
  Description   : pcp session header
  Function List :
  History       :
  1.Date        : 2013/9/23
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_SESSION_H
#define PCP_SESSION_H

#include "pcp_cfg.h"

PCP_EXTERN_C_START

void null_session();

int init_session(void);

void destroy_session(void);

OUT pcp_session_t* create_session(IN const char* callid);

OUT pcp_session_t* get_session(IN const char* callid);

void delete_session(IN pcp_session_t* psession);

int create_callid(IN const char* uid, OUT char* callid);

int update_session_state(IN const char* callid, IN pcp_state_t state);

int session_state(IN const char* callid);

int get_best_mediastream(IN int role, IN int type, OUT pcp_session_t* psession);

/* mod by VintonLiu on 20150511 for lock debug, easy to find died lock */
void lock_session_d(const char *pfunc);

void ulock_session_d(const char *pfunc);
#define lock_session() lock_session_d(__FUNCTION__)
#define ulock_session() ulock_session_d(__FUNCTION__)
/* VintonLiu mod end */

int check_session(IN pcp_session_t* psession);

PCP_EXTERN_C_END

#endif


