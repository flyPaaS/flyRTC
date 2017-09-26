/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : mq.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2014/5/29
  Last Modified :
  Description   : message queue head file
  Function List :
  History       :
  1.Date        : 2014/5/29
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "comm.h"
#include "mslist.h"

typedef enum mq_type_t
{
	MQ_ICE_TP = 0,
	MQ_TCP_TP = 1,
	MQ_PCP_TP = 2,
	MQ_MEG_TP = 3,
	MQ_TMT_TP = 4,
	MQ_LOG_TP = 5
}mq_type_t;


int mq_init(void);

int mq_destroy(void);

OUT mslist_handle_t* mq_get_mslist(IN mq_type_t mstype);

OUT void* mq_get_node(IN mq_type_t mstype);

int mq_remove_node(IN mq_type_t mstype, IN void* node);

#endif

