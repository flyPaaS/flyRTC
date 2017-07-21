/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ugo_test.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/10/17
  Last Modified :
  Description   : ugo test file header
  Function List :
  History       :
  1.Date        : 2013/10/17
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef UGO_TEST_H
#define UGO_TEST_H
#include "../../comm/inc/cfg.h"

int ugo_test_login(const char *ssid, int netmode);

int ugo_test_logout(void);

int ugo_test_set_codecs(void* pcodeclist);

int ugo_test_amr_record(const char* filepath, const char* filename);


#endif


