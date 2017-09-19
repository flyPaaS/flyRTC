/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_callpm.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/12/3
  Last Modified :
  Description   : pcp call param file header
  Function List :
  History       :
  1.Date        : 2013/12/3
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_CALL_PM_H
#define PCP_CALL_PM_H

#include "pcp_cfg.h"

PCP_EXTERN_C_START

void null_pm();

int pm_init(void);

void pm_destroy(void);

int pm_update(IN void* pm);

void pm_param_copy(void* pm);

OUT void* pm_get(void);

int pm_clean(void);

int pm_update_call_type(IN int call_type);

OUT char* pm_callid(void);

PCP_EXTERN_C_END

#endif


