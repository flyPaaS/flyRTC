/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : exl_file.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2014/6/10
  Last Modified :
  Description   : external play file
  Function List :
  History       :
  1.Date        : 2014/6/10
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef EXL_PLAY_FILE_H
#define EXL_PLAY_FILE_H
#include "cfg.h"
#include "MediaEngineInterface.h"

#ifdef EXTERNAL_PLAYFILE
#include <stdio.h>
#include "mscommon.h"
#include "common_types.h"
#include "webrtc/system_wrappers/interface/critical_section_wrapper.h"

using namespace webrtc;

typedef struct file_stream_t
{
	int size;
	char* data;
}file_stream_t;

typedef struct playfile_pm_t
{
	int   mode;
	FILE* fd;
	bool  loop;
	file_stream_t fstream;
}playfile_pm_t;

class CExlPlayFileAdapter : public InStream
{
public:

	CExlPlayFileAdapter(void);

	virtual ~CExlPlayFileAdapter(void);

	int SetPlayFile(playfile_pm_t* pm);

	int ResetPlayFile(void);

	virtual int Read(void *buf,size_t size);
	
private:

	CriticalSectionWrapper* _critSect;
	
	int   m_mode;
	FILE* m_fd;
	bool  m_loop;
	int   m_offset;
	file_stream_t m_stream;
};


#endif

#endif /* EXL_PLAY_FILE_H */

