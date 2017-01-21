#include "rbbuffer.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>



RBBuffer :: RBBuffer()
{
	m_pcArray = NULL;
	m_iWritePos = 0;
	m_iReadPos = 0;
	m_iCapacity = 0;
}

RBBuffer::RBBuffer(const RBBuffer& rhs)
{
	Reset(); // reset, no matter what will happen later. It can expose problem if latter code fails

	if (rhs.m_pcArray != NULL)
	{
		if (EnsureWriteSize(rhs.m_iCapacity) == 0)
		{
			memcpy(m_pcArray, rhs.m_pcArray, rhs.m_iWritePos);
			m_iWritePos = rhs.m_iWritePos;
			m_iReadPos = rhs.m_iReadPos;		
		}
	}
}

RBBuffer & RBBuffer::operator = (RBBuffer &rhs)
{
	Reset(); // reset, no matter what will happen later. It can expose problem if latter code fails

	if (rhs.m_pcArray != NULL)
	{
		if (EnsureWriteSize(rhs.m_iCapacity) == 0)
		{
			memcpy(m_pcArray, rhs.m_pcArray, rhs.m_iWritePos);
			m_iWritePos = rhs.m_iWritePos;
			m_iReadPos = rhs.m_iReadPos;				
		}
	}

	return *this;
}

RBBuffer :: ~RBBuffer()
{
	Reset();
}

int RBBuffer :: EnsureWriteSize( int iWriteSize )
{
	iWriteSize = iWriteSize > 0 ? iWriteSize : 1;

	if( m_iWritePos + iWriteSize > m_iCapacity )
	{
		if( NULL == m_pcArray )
		{
			m_iWritePos = 0;
			m_iCapacity = iWriteSize;
			m_pcArray = new unsigned char[ m_iCapacity + 1 ];
			if (NULL == m_pcArray)
				return -1;
		} 
		else 
		{
			unsigned char * tmp = new unsigned char [ m_iWritePos + iWriteSize + 1 ];
			if (NULL == tmp)
				return -1;
		
			m_iCapacity = m_iWritePos + iWriteSize;		
			memcpy( tmp, m_pcArray, m_iWritePos );
			delete [] m_pcArray;
			m_pcArray = tmp;
		}
		m_pcArray[ m_iCapacity ] = '\0';
	}

	//assert( NULL != m_pcArray );

	return 0;
}

unsigned char * RBBuffer :: GetWritePtr( int iWriteSize )
{
	if (EnsureWriteSize( iWriteSize ) != 0)
		return NULL;

	return m_pcArray + m_iWritePos;
}

void RBBuffer :: AddWritePos( int iPos )
{
	m_iWritePos += iPos;
}

void RBBuffer :: SetWritePos( int iPos )
{
	m_iWritePos = iPos;
}

int RBBuffer :: Write( const void * sBuffer, int iLen )
{
	unsigned char* p = GetWritePtr( iLen );
	if (!p) return 0;
	
	memcpy( p, sBuffer, iLen );
	AddWritePos( iLen );

	return iLen;
}

int RBBuffer :: GetWritePos()
{
	return m_iWritePos;
}

int RBBuffer::GetLen()
{
	return m_iWritePos;
}

int RBBuffer :: GetWriteCapacity()
{
	return m_iCapacity;
}

unsigned char * RBBuffer :: GetReadPtr()
{
	//assert( m_iReadPos <= m_iWritePos );
	return m_pcArray + m_iReadPos;
}

void RBBuffer :: AddReadPos( int iPos )
{
	m_iReadPos += iPos;
}

void RBBuffer :: SetReadPos( int iPos )
{
	m_iReadPos = iPos;
}

int RBBuffer :: GetReadPos()
{
	return m_iReadPos;
}

int RBBuffer :: Read( void * sBuffer, int iLen )
{
	//assert( ( m_iWritePos - m_iReadPos ) >= iLen );

	memcpy( sBuffer, GetReadPtr(), iLen );
	AddReadPos( iLen );

	return iLen;
}

unsigned char * RBBuffer :: GetArray()
{
	return m_pcArray;
}

void RBBuffer :: Reset()
{
	if( NULL != m_pcArray ) delete [] m_pcArray;
	m_pcArray = NULL;
	m_iWritePos = 0;
	m_iReadPos = 0;
	m_iCapacity = 0;
}

void RBBuffer :: Detach(unsigned char** pp, int &iLen)
{
	if (!pp) return;
	*pp = m_pcArray;
	iLen = m_iWritePos;
	m_pcArray = NULL;
	m_iWritePos = 0;
	m_iReadPos = 0;
}

void RBBuffer :: Attach(unsigned char* p, int iLen)
{
	Reset();
	m_pcArray = p;
	m_iWritePos = iLen;
	m_iReadPos = 0;
}

void RBBuffer::Attach(RBBuffer *apBuf)
{
	if (apBuf == this)
	{
		return;
	}

	Reset();

	if (NULL == apBuf)
	{
		return;
	}

	m_pcArray = apBuf->m_pcArray;
	m_iWritePos = apBuf->m_iWritePos;
	m_iReadPos = apBuf->m_iReadPos;
	m_iCapacity = apBuf->m_iCapacity;

	apBuf->m_pcArray = NULL;
	apBuf->m_iCapacity = 0;
	apBuf->m_iReadPos = 0;
	apBuf->m_iWritePos = 0;
}



