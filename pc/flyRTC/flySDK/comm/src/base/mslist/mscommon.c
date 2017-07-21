/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "mscommon.h"


#if !defined(_WIN32_WCE)
#include <sys/types.h>
#endif

#if defined(__APPLE__) && !defined(__GNUC__)
#import <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#endif

#ifdef __APPLE__
   #include "TargetConditionals.h"
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

MSList *ms_list_new(void *data){
	MSList *new_elem=(MSList *)ms_new(MSList,1);
	new_elem->prev=new_elem->next=NULL;
	new_elem->data=data;
	return new_elem;
}

MSList * ms_list_append(MSList *elem, void * data){
	MSList *new_elem=ms_list_new(data);
	MSList *it=elem;
	if (elem==NULL) return new_elem;
	while (it->next!=NULL) it=ms_list_next(it);
	it->next=new_elem;
	new_elem->prev=it;
	return elem;
}

MSList * ms_list_prepend(MSList *elem, void *data){
	MSList *new_elem=ms_list_new(data);
	if (elem!=NULL) {
		new_elem->next=elem;
		elem->prev=new_elem;
	}
	return new_elem;
}


MSList * ms_list_concat(MSList *first, MSList *second){
	MSList *it=first;
	if (it==NULL) return second;
	while(it->next!=NULL) it=ms_list_next(it);
	it->next=second;
	second->prev=it;
	return first;
}

MSList * ms_list_free_data(MSList *list){
	MSList *elem = list;
	void* evdata;
	while (elem != NULL)
	{
		evdata = elem->data;
		elem = elem->next;
		if(evdata!=NULL) 
		{
			ms_free(evdata);
		}
	}
	return NULL;
}

MSList * ms_list_free(MSList *list){
	MSList *elem = list;
	MSList *tmp;
	if (list==NULL) return NULL;
	while(elem->next!=NULL) {
		tmp = elem;
		elem = elem->next;
		ms_free(tmp);
	}
	ms_free(elem);
	return NULL;
}

MSList * ms_list_remove(MSList *first, void *data){
	MSList *it;
	it=ms_list_find(first,data);
	if (it) return ms_list_remove_link(first,it);
	else {
		ms_warning("ms_list_remove: no element with %p data was in the list", data);
		return first;
	}
}

int ms_list_size(const MSList *first){
	int n=0;
	while(first!=NULL){
		++n;
		first=first->next;
	}
	return n;
}

void ms_list_for_each(const MSList *list, void (*func)(void *)){
	for(;list!=NULL;list=list->next){
		func(list->data);
	}
}

void ms_list_for_each2(const MSList *list, void (*func)(void *, void *), void *user_data){
	for(;list!=NULL;list=list->next){
		func(list->data,user_data);
	}
}

MSList *ms_list_remove_link(MSList *list, MSList *elem){
	MSList *ret;
	if (elem==list){
		ret=elem->next;
		elem->prev=NULL;
		elem->next=NULL;
		if (ret!=NULL) ret->prev=NULL;
		ms_free(elem);
		return ret;
	}
	elem->prev->next=elem->next;
	if (elem->next!=NULL) elem->next->prev=elem->prev;
	elem->next=NULL;
	elem->prev=NULL;
	ms_free(elem);
	elem = NULL;
	return list;
}

MSList *ms_list_find(MSList *list, void *data){
	for(;list!=NULL;list=list->next){
		if (list->data==data) return list;
	}
	return NULL;
}

MSList *ms_list_find_custom(MSList *list, int (*compare_func)(const void *, const void*), const void *user_data){
	for(;list!=NULL;list=list->next){
		if (compare_func(list->data,user_data)==0) return list;
	}
	return NULL;
}

void * ms_list_nth_data(const MSList *list, int index){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (i==index) return list->data;
	}
	ms_error("ms_list_nth_data: no such index in list.");
	return NULL;
}

int ms_list_position(const MSList *list, MSList *elem){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (elem==list) return i;
	}
	ms_error("ms_list_position: no such element in list.");
	return -1;
}

int ms_list_index(const MSList *list, void *data){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (data==list->data) return i;
	}
	ms_error("ms_list_index: no such element in list.");
	return -1;
}

MSList *ms_list_insert_sorted(MSList *list, void *data, int (*compare_func)(const void *, const void*)){
	MSList *it,*previt=NULL;
	MSList *nelem;
	MSList *ret=list;
	if (list==NULL) return ms_list_append(list,data);
	else{
		nelem=ms_list_new(data);
		for(it=list;it!=NULL;it=it->next){
			previt=it;
			if (compare_func(data,it->data)<=0){
				nelem->prev=it->prev;
				nelem->next=it;
				if (it->prev!=NULL)
					it->prev->next=nelem;
				else{
					ret=nelem;
				}
				it->prev=nelem;
				return ret;
			}
		}
		previt->next=nelem;
		nelem->prev=previt;
	}
	return ret;
}

MSList *ms_list_insert(MSList *list, MSList *before, void *data){
	MSList *elem;
	if (list==NULL || before==NULL) return ms_list_append(list,data);
	for(elem=list;elem!=NULL;elem=ms_list_next(elem)){
		if (elem==before){
			if (elem->prev==NULL)
				return ms_list_prepend(list,data);
			else{
				MSList *nelem=ms_list_new(data);
				nelem->prev=elem->prev;
				nelem->next=elem;
				elem->prev->next=nelem;
				elem->prev=nelem;
			}
		}
	}
	return list;
}

MSList *ms_list_copy(const MSList *list){
	MSList *copy=NULL;
	const MSList *iter;
	for(iter=list;iter!=NULL;iter=ms_list_next(iter)){
		copy=ms_list_append(copy,iter->data);
	}
	return copy;
}

void ms_init(){
	ortp_init();
	ms_message("ms_init() done");
}

void ms_exit(){
	ortp_destroy();
	ms_message("ms_exit() done");
}

void ms_sleep(int seconds){
#ifdef WIN32
	Sleep(seconds*1000);
#else
	struct timespec ts,rem;
	int err;
	ts.tv_sec=seconds;
	ts.tv_nsec=0;
	do {
		err=nanosleep(&ts,&rem);
		ts=rem;
	}while(err==-1 && errno==EINTR);
#endif
}

void ms_usleep(uint64_t usec){
#ifdef WIN32
	Sleep((DWORD)(usec/1000));
#else
	struct timespec ts,rem;
	int err;
	ts.tv_sec=usec/1000000LL;
	ts.tv_nsec=(usec%1000000LL)*1000;
	do {
		err=nanosleep(&ts,&rem);
		ts=rem;
	}while(err==-1 && errno==EINTR);
#endif
}

extern void _android_key_cleanup(void*);

void ms_thread_exit(void* ref_val) {
#ifdef ANDROID
	// due to a bug in old Bionic version
	// cleanup of jni manually
	// works directly with Android 2.2
	_android_key_cleanup(NULL);
#endif
	ortp_thread_exit(ref_val);
}


#ifdef __MACH__
#include <sys/types.h>
#include <sys/timeb.h>
#endif

void ms_get_cur_time(MSTimeSpec *ret){
#if defined(_WIN32_WCE) || defined(WIN32)
	DWORD timemillis;
#	if defined(_WIN32_WCE)
	timemillis=GetTickCount();
#	else
	timemillis=timeGetTime();
#	endif
	ret->tv_sec=timemillis/1000;
	ret->tv_nsec=(timemillis%1000)*1000000LL;
#elif defined(__MACH__) && defined(__GNUC__) && (__GNUC__ >= 3)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ret->tv_sec=tv.tv_sec;
	ret->tv_nsec=tv.tv_usec*1000LL;
#elif defined(__MACH__)
	struct timeb time_val;

	ftime (&time_val);
	ret->tv_sec = time_val.time;
	ret->tv_nsec = time_val.millitm * 1000000LL;
#else
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC,&ts)<0){
		ms_fatal("clock_gettime() doesn't work: %s",strerror(errno));
	}
	ret->tv_sec=ts.tv_sec;
	ret->tv_nsec=ts.tv_nsec;
#endif
}




/*** plc context end***/
