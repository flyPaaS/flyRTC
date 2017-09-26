/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : event_queue.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/11/6
  Last Modified :
  Description   : media engine event queue header filer
  Function List :
  History       :
  1.Date        : 2013/11/6
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "mscommon.h"

typedef void (*evt_cb_f)(int ev_type, int ev_reason, 
                            const char* something, void* param, int size);

int  evt_init();
void evt_destroy();
void evt_appand(int ev_type, int ev_reason, const char* something, void* param, int size);
void evt_remove(void* ev_data);
void evt_callback(evt_cb_f pevfunc);

#endif /* EVENT_QUEUE_H */

