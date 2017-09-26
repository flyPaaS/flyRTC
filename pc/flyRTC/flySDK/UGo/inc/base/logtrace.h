/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : logtrace.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/26
  Last Modified :
  Description   : log trace file header
  Function List :
  History       :
  1.Date        : 2013/9/26
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef LOG_TRACE_H
#define LOG_TRACE_H
#include "mscommon.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct trace_event_node
{
	int  level;
	char something[1024];
}trace_event_node_t;
#define TRACE_EVENT_NODE_T_SIZE (sizeof(trace_event_node_t))

int blog_init(void);

void blog_destroy(void);

int blog_enable_logs(int level, const char *logpath);

#ifdef __cplusplus
}
#endif

#endif /* LOG_TRACE_H */

