#include "mslist.h"


int mslist_init(void)
{
	ms_init();
	
	return 0;	
}

void mslist_destroy(void)
{
	ms_exit();
}

int mslist_create(mslist_handle_t* lc)
{
	ms_mutex_init(&lc->ev_lock,NULL);
	lc->ev_list = NULL;
    lc->count = 0; //yangjj add 2016-3-20
	return 0;
}

void mslist_delete(mslist_handle_t* lc)
{
	ms_mutex_lock(&lc->ev_lock);
	ms_list_free_data(lc->ev_list);
	lc->ev_list = ms_list_free(lc->ev_list);
	ms_mutex_unlock(&lc->ev_lock);
	ms_mutex_destroy(&lc->ev_lock);
}

int mslist_append(mslist_handle_t* lc, void* ev_data)
{
	if (ev_data == NULL)
		return -1;

	ms_mutex_lock(&lc->ev_lock);
	lc->ev_list = ms_list_append(lc->ev_list,ev_data);
    lc->count++; //yangjj add
	ms_mutex_unlock(&lc->ev_lock);
	
	return 0;
}


int mslist_remove(mslist_handle_t* lc, void* ev_data)
{
    if (lc == NULL || ev_data == NULL ||lc->ev_list ==NULL)
        return -1;
    ms_mutex_lock(&lc->ev_lock);
    lc->ev_list = ms_list_remove(lc->ev_list, ev_data);
    if ( ev_data != NULL )
    {
        ms_free(ev_data);
        ev_data = NULL;
    }
    lc->count--; //yangjj add
    ms_mutex_unlock(&lc->ev_lock);

    return 0;
}

MSList* mslist_find(mslist_handle_t* lc, int (*compare_func)(const void *, const void*), void *user_data)
{
	MSList* ls = NULL;
	
	if (lc == NULL ||lc->ev_list ==NULL)
        return NULL;
	
    ms_mutex_lock(&lc->ev_lock);
	ls = ms_list_find_custom(lc->ev_list, compare_func, user_data);
    ms_mutex_unlock(&lc->ev_lock);

	return ls;
}

int mslist_for_each(mslist_handle_t *lc, void (*func)(void *))
{
	if (lc == NULL ||lc->ev_list ==NULL)
        return -1;
	
    ms_mutex_lock(&lc->ev_lock);
	ms_list_for_each(lc->ev_list, func);
    ms_mutex_unlock(&lc->ev_lock);

	return 0;
}


