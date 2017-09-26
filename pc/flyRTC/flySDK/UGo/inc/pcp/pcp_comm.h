/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_comm.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/10/12
  Last Modified :
  Description   : pcp comm file header
  Function List :
  History       :
  1.Date        : 2013/10/12
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_COMM_H
#define PCP_COMM_H

#include "mscommon.h"

#ifdef __cplusplus
#define PCP_EXTERN_C_START extern "C" {
#define PCP_EXTERN_C_END }
#else
#define PCP_EXTERN_C_START	
#define PCP_EXTERN_C_END	
#endif

PCP_EXTERN_C_START

void null_comm();

typedef int				pcp_bool;
typedef char			pcp_int8;
typedef unsigned char   pcp_uint8;
typedef short   		pcp_int16;
typedef int     		pcp_int32;
typedef unsigned int 	pcp_uint32;
typedef unsigned short 	pcp_uint16;

#ifndef bool
#define bool int
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef IN
#define IN	
#endif

#ifndef OUT
#define OUT
#endif

#ifndef PCP_OK
#define PCP_OK 		0
#endif

#ifndef PCP_ERR
#define PCP_ERR 	-1
#endif

void res_st(IN void* st, IN int size);

OUT void* pcp_malloc(IN int size);

void pcp_free(IN void* p);

PCP_EXTERN_C_END

#endif

