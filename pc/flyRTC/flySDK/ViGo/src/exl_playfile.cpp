#include "exl_playfile.h"

#ifdef EXTERNAL_PLAYFILE

CExlPlayFileAdapter::CExlPlayFileAdapter(void)
{
	m_mode = kME_FILE_STREAM;
	m_fd = NULL;
	m_loop = false;
	m_offset = 0;
	
	_critSect = CriticalSectionWrapper::CreateCriticalSection();
	memset(&m_stream, 0, sizeof(m_stream));
}

CExlPlayFileAdapter::~CExlPlayFileAdapter(void)
{
	if (m_stream.data)
		ms_free(m_stream.data);

	if (_critSect)
		delete _critSect;
}

int CExlPlayFileAdapter::SetPlayFile(playfile_pm_t* pm)
{
	if (pm == NULL)
		return -1;

	CriticalSectionScoped lock(_critSect);
	
	m_loop = pm->loop;
	m_mode = pm->mode;
	
	if (kME_FILE_HANDLE == pm->mode)
	{
		m_fd = pm->fd;
	}
	else if(kME_FILE_STREAM == pm->mode)
	{
		if (pm->fstream.size != 0 && pm->fstream.data != NULL)
		{
			if (m_stream.data)
				ms_free(m_stream.data);

			m_offset = 0;
			m_stream.size = pm->fstream.size;
			m_stream.data = (char*)ms_malloc0(pm->fstream.size+1);
			if (m_stream.data)
			{
				memcpy(m_stream.data, pm->fstream.data, pm->fstream.size);
			}
		}
	}
	
	return 0;
}

int CExlPlayFileAdapter::ResetPlayFile(void)
{
	CriticalSectionScoped lock(_critSect);

	if (m_stream.data)
		ms_free((void*)m_stream.data);
	m_stream.data = NULL;
	m_stream.size = 0;
	
	m_offset = 0;
	m_loop = false;
	m_fd = NULL;

	return 0;
}

int CExlPlayFileAdapter::Read(void *buf,size_t size)
{
	int rlen = 0;
    int len = (int)size;

	if (buf == NULL || len == 0)
		return -1;

	CriticalSectionScoped lock(_critSect);

	if (m_mode == kME_FILE_HANDLE)
	{
		if (m_fd != NULL)
		{
			rlen = fread(buf, sizeof(char), len, m_fd);
			if (rlen != len && m_loop)
			{
				fseek(m_fd, 0, 0);
				rlen = fread(buf, sizeof(char), len, m_fd);
			}
		}
	}
	else if (m_mode == kME_FILE_STREAM)
	{
		if (m_stream.data != NULL 
			&& m_stream.size != 0)
		{
			if (m_offset + len <= m_stream.size)
			{
				memcpy(buf, m_stream.data+m_offset,len);
				rlen = len;
				m_offset += len;
			}
			else
			{
				if (m_loop)
				{
					m_offset = 0;
					memcpy(buf, m_stream.data+m_offset,len);
					rlen = len;
					m_offset += len;
				}
			}
		}
	}

	return rlen;
}

#endif
