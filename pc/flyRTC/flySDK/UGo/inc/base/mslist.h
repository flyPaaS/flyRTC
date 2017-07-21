#ifndef _MSLIST_H_
#define _MSLIST_H_
#include "mscommon.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct mslist_handle_t
{
	ms_mutex_t  ev_lock;
	MSList*		ev_list;
    int count; // yangjj add 2016-3-20
}mslist_handle_t;

int mslist_init(void);

void mslist_destroy(void);

int mslist_create(mslist_handle_t* lc);

void mslist_delete(mslist_handle_t* lc);

int mslist_append(mslist_handle_t* lc, void* ev_data);

int mslist_remove(mslist_handle_t* lc, void* ev_data);

MSList* mslist_find(mslist_handle_t* lc, int (*compare_func)(const void *, const void*), void *user_data);

int mslist_for_each(mslist_handle_t *lc, void (*func)(void *));

#ifdef __cplusplus
}
#endif

#endif


