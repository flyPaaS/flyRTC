#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "skbuffer.h"

#include "tlvport.h"


#include "iFile.h"

namespace Comm
{

typedef struct tagSKBufferImpl {
	unsigned char *pcArray;
	int iReadPos;
	int iWritePos;
	int iCapacity;
	bool bMalloc;
} SKBufferImpl_t;
	
SKBuffer :: SKBuffer()
{
	m_ptBufferImpl = new SKBufferImpl_t;
	memset ( m_ptBufferImpl, 0x0, sizeof(SKBufferImpl_t) );
}

SKBuffer :: ~SKBuffer()
{
	Reset();
	delete m_ptBufferImpl, m_ptBufferImpl=NULL;
}

int SKBuffer :: EnsureExpandSize( int iWriteSize )
{
	iWriteSize = iWriteSize > 0 ? iWriteSize : 1;

	if( m_ptBufferImpl->iWritePos + iWriteSize > m_ptBufferImpl->iCapacity )
	{
		if( NULL == m_ptBufferImpl->pcArray )
		{
			m_ptBufferImpl->iWritePos = 0;
			m_ptBufferImpl->iCapacity = iWriteSize;
			m_ptBufferImpl->pcArray = new unsigned char[ m_ptBufferImpl->iCapacity + 1 ];
			if( !m_ptBufferImpl->pcArray)
				return -1;
		} else {
			TLV_LOG_IN ( "EnsureExpandSize %d to %d increase %d", 
					m_ptBufferImpl->iCapacity, m_ptBufferImpl->iWritePos + iWriteSize, 
					m_ptBufferImpl->iWritePos + iWriteSize - m_ptBufferImpl->iCapacity);

			m_ptBufferImpl->iCapacity = m_ptBufferImpl->iWritePos + iWriteSize;

			unsigned char * tmp = new unsigned char [ m_ptBufferImpl->iCapacity + 1 ];
			if(!tmp)
				return -2;

			memcpy( tmp, m_ptBufferImpl->pcArray, m_ptBufferImpl->iWritePos );
			if (m_ptBufferImpl->bMalloc)
			{
				free(m_ptBufferImpl->pcArray);
			}
			else
			{
				delete[] (m_ptBufferImpl->pcArray);
			}

			m_ptBufferImpl->pcArray = tmp;
			m_ptBufferImpl->bMalloc = false;
		}
		m_ptBufferImpl->pcArray[ m_ptBufferImpl->iCapacity ] = '\0';
	}

	assert( NULL != m_ptBufferImpl->pcArray );

	return 0;
}

unsigned char * SKBuffer :: GetWritePtr( int iWriteSize )
{
	EnsureExpandSize( iWriteSize );

	return m_ptBufferImpl->pcArray + m_ptBufferImpl->iWritePos;
}

void SKBuffer :: AddWritePos( int iWritePos )
{
	m_ptBufferImpl->iWritePos += iWritePos;
}

void SKBuffer :: SetWritePos( int iWritePos )
{
	if( iWritePos > m_ptBufferImpl->iWritePos )
	{
		EnsureExpandSize( iWritePos - m_ptBufferImpl->iWritePos );
	}
	m_ptBufferImpl->iWritePos = iWritePos;
}

int SKBuffer :: Write( const void * sBuffer, int iLen )
{
	memcpy( GetWritePtr( iLen ), sBuffer, iLen );
	AddWritePos( iLen );

	return iLen;
}

int SKBuffer :: Printf( const char * sFmt, ... )
{
	int len = 0;
	char tmp[ 4096 ] = { 0 };

	if( NULL != strchr( sFmt, '%' ) )
	{
		va_list vlist;
		va_start( vlist, sFmt );
		len = vsnprintf( tmp, sizeof( tmp ), sFmt, vlist );
		va_end( vlist );
		return Write( tmp, len );
	}

	return Write( sFmt, strlen( sFmt ) );
}

unsigned char * SKBuffer :: GetReadPtr( int iReadSize )
{
	if( iReadSize > m_ptBufferImpl->iWritePos )
	{
		EnsureExpandSize( iReadSize - m_ptBufferImpl->iWritePos );
	}

	return m_ptBufferImpl->pcArray + iReadSize;
}

void SKBuffer :: AddReadPos( int iReadPos )
{
	m_ptBufferImpl->iReadPos += iReadPos;
}

void SKBuffer :: SetReadPos( int iReadPos )
{
	if( iReadPos > m_ptBufferImpl->iWritePos )
	{
		EnsureExpandSize( iReadPos - m_ptBufferImpl->iWritePos );
	}
	m_ptBufferImpl->iReadPos = iReadPos;
}

int SKBuffer :: GetReadSize()
{
	return m_ptBufferImpl->iReadPos;
}

int SKBuffer::GetLen()
{
	return m_ptBufferImpl->iWritePos;
}

int SKBuffer::GetCapacity()
{
	return m_ptBufferImpl->iCapacity;
}

unsigned char * SKBuffer :: GetBuffer()
{
	return m_ptBufferImpl->pcArray;
}
//********************************************
int SKBuffer::GetLen()const
{
	return m_ptBufferImpl->iWritePos;
}

int SKBuffer::GetCapacity()const
{
	return m_ptBufferImpl->iCapacity;
}

unsigned char * SKBuffer :: GetBuffer()const
{
	return m_ptBufferImpl->pcArray;
}
//********************************************


void SKBuffer :: Reset()
{
	if( NULL != m_ptBufferImpl->pcArray ) 
	{
		if (m_ptBufferImpl->bMalloc)
		{
			free(m_ptBufferImpl->pcArray);
		}
		else
		{
			delete[] (m_ptBufferImpl->pcArray);
		}
	}
	memset ( m_ptBufferImpl, 0x0, sizeof(SKBufferImpl_t) );
}

unsigned char * SKBuffer ::  Detach( int * piLen )
{
	unsigned char * ret = m_ptBufferImpl->pcArray;
	if( piLen )
		*piLen = m_ptBufferImpl->iWritePos;

	memset ( m_ptBufferImpl, 0x0, sizeof(SKBufferImpl_t) );

	return ret;
}

void SKBuffer :: Attach( unsigned char * sBuffer, int iLen )
{
	Reset();

	m_ptBufferImpl->pcArray = sBuffer;
	m_ptBufferImpl->iWritePos = iLen;
	m_ptBufferImpl->iCapacity = iLen;
}

void SKBuffer :: AttachMalloc( unsigned char * sBuffer, int iLen )
{
	Reset();

	m_ptBufferImpl->pcArray = sBuffer;
	m_ptBufferImpl->iWritePos = iLen;
	m_ptBufferImpl->iCapacity = iLen;
	m_ptBufferImpl->bMalloc = true;
}

/* save buffer to file */
/* @ret 0 -- ok, <>0 -- failed */
int SKBuffer :: SaveToFile(const char *sFile)
{
	if ( (NULL == sFile) || ('\0' == *sFile) )
	{
		return COMM_ERR_PARA;
	}

	if ( (NULL == m_ptBufferImpl) || (NULL == m_ptBufferImpl->pcArray)
		|| (0 > m_ptBufferImpl->iWritePos) )
	{
		return COMM_ERR_GENERAL;
	}

	int ret = TLV_WRITE_TO_FILE ( sFile, (char *)m_ptBufferImpl->pcArray, m_ptBufferImpl->iWritePos );
	if ( 0 != ret )
	{
		TLV_LOG_ERR ("SKBuffer::WriteToFile(%s, %i) failed %i",
			sFile, m_ptBufferImpl->iWritePos, ret );
		ret = COMM_ERR_FILEIO;
	}

	return ret;
}

}

