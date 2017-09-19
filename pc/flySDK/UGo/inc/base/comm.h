#ifndef _COMM_H_
#define _COMM_H_

#include "mscommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
#define bool int
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true  1
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef RES_OK
#define RES_OK 0
#endif

#ifndef RES_ERR
#define RES_ERR -1
#endif

typedef struct sv_addr_t
{
	int  sv_port;
	char sv_ip[64];
}sv_addr_t;

void cm_resst(IN void* st, IN int size);

int cm_paser_address(IN const char* saddr, OUT sv_addr_t* sv_addr);

int cm_sceq(void);

int cm_rand_time(void);

int cm_rand_alphabet5(OUT char alp_str[6]);

int cm_abs_i(IN int number);

OUT int64_t cm_stoi64(IN const char* int_str);

#ifdef __cplusplus
}
#endif


#endif  // _DEFAULTS_H_
