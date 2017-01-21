

#include "skinnerstruct.h"

SKString :: SKString()
{
	m_pcBuff = NULL;
	m_piBuffLen = NULL;
	m_pcTmpBuff = NULL;
	m_iTmpLen = 0;
	m_pcBuff = &m_pcTmpBuff;
	m_piBuffLen = &m_iTmpLen;
}

SKString :: SKString( char ** pcValue, int * iSize )
{
	m_pcBuff = pcValue;
	m_piBuffLen = iSize;
	m_pcTmpBuff = NULL;
	m_iTmpLen = 0;
}


SKString :: SKString( char * pcValue, int * iSize )
{
	int len = ( iSize == NULL? strlen( pcValue ) + 1 : *iSize );
	m_pcTmpBuff = ( char * )calloc( sizeof( char ) , len );
	memcpy( m_pcTmpBuff, pcValue, len );

	m_pcBuff = &m_pcTmpBuff;

	m_iTmpLen = len; 
	m_piBuffLen = &m_iTmpLen;
}

SKString :: ~SKString()
{
	m_pcBuff = NULL;
	m_piBuffLen = NULL;
	free( m_pcTmpBuff );
	m_pcTmpBuff = NULL;
	m_iTmpLen = 0;
}

void SKString :: ReSet()
{
	m_pcBuff = NULL;
	m_piBuffLen = NULL;
	m_pcTmpBuff = NULL;
	m_iTmpLen = 0;
	m_pcBuff = &m_pcTmpBuff;
	m_piBuffLen = &m_iTmpLen;

}

int SKString :: CopyFrom( const char * pcValue, const int &iSize )
{
	if( m_pcBuff != NULL && *m_pcBuff != NULL )
	{
		free( *m_pcBuff );
	}
	
	if( m_piBuffLen != NULL )
		*m_piBuffLen = iSize;

	char * tmp = ( char * ) calloc ( sizeof( char ) , iSize + 1 );

	memcpy( tmp, pcValue, iSize );

	if( m_pcBuff == NULL )
	{
		m_pcBuff = &tmp;
	}
	else
	{
		*m_pcBuff = tmp;
		tmp = NULL;
	}
	return 0;
}

SKString & SKString :: operator = ( const char * pcValue ) 
{
	CopyFrom ( pcValue, strlen( pcValue )  );
	return *this;
}

SKString &SKString :: operator = ( const SKString &sValue )
{
	CopyFrom ( sValue.GetString(), sValue.Size() );

	return *this;
}

char &SKString :: operator [] ( const int &iIndex )
{
	return (*m_pcBuff)[iIndex];
}

const char &SKString :: operator [] ( const int &iIndex )const
{
	return (*m_pcBuff)[iIndex];
}

void SKString :: Set( char ** pcValue, int *iSize )
{
	if( m_pcBuff != NULL && m_pcBuff == pcValue )
	{
		return;	
	}

	if( m_pcBuff != NULL && *m_pcBuff != NULL )
	{
		free( *m_pcBuff );
	}

	m_pcBuff = pcValue;
	m_piBuffLen = iSize;

	return;
}

void SKString :: Append( const char & cValue )
{
	Append( &cValue, 1);
}

void SKString :: Append( const char * pcValue, const int &iSize )
{
	if( m_piBuffLen == NULL )
	{
		int oldlen = strlen( *m_pcBuff );
		int newlen = strlen( pcValue );

        char * pTmpBuf = ( char * )realloc( *m_pcBuff, newlen + oldlen + 1 );
        if(NULL == pTmpBuf) {
            assert(false);
        } else {
            *m_pcBuff = pTmpBuf;
        }
		memcpy( *m_pcBuff + oldlen, pcValue, newlen );
		(*m_pcBuff)[newlen + oldlen] = 0;

	}
	else
	{
		int oldlen = *m_piBuffLen;
		int newlen = 0;
		if( iSize == -1 )
		{   
			newlen = strlen( pcValue );
		}
		else
		{   
			newlen = iSize;
		}

        char * pTmpBuf = ( char * )realloc( *m_pcBuff, newlen + oldlen + 1 );
        if(NULL == pTmpBuf) {
            assert(false);
        } else {
            *m_pcBuff = pTmpBuf;
        }
		memcpy( ( *m_pcBuff ) + oldlen, pcValue, newlen );

		*m_piBuffLen = oldlen + newlen;
	}
}

int SKString :: Size()const
{
	if( m_piBuffLen == NULL )
	{
		return strlen( *m_pcBuff );
	}
	return *m_piBuffLen;
}

const char *SKString :: GetString()const
{
	return *m_pcBuff;	
}

char *SKString :: GetString()
{
	return *m_pcBuff;	
}

int SKString :: cmp ( const SKString sString1, const SKString &sString2 )
{
	int len1 = sString1.Size();
	int len2 = sString2.Size();

	int min_len = len1 < len2? len1 : len2;

	for( int i=0; i < min_len; ++i )
	{
		if( sString1[i] < sString2[i] )
		{
			return -1;
		}
		else if( sString1[i] > sString2[i] ) 
		{
			return 1;
		}
	}
	if( len1 == len2 )
		return 0;
	if( len1 < len2 ) 
		return -1;
	if( len1 > len2 )
		return 1;
	return 0;
}

bool SKString :: operator == ( const SKString &sString )
{
	return cmp( (*this), sString ) == 0;
}

bool SKString :: operator < ( const SKString &sString )
{
	return cmp((*this),sString) < 0;
}

bool SKString :: operator > ( const SKString &sString )
{
	return cmp( (*this), sString ) > 0;
}

bool SKString :: operator <= ( const SKString &sString )
{
	return cmp( (*this), sString ) <= 0;
}

bool SKString :: operator >= ( const SKString &sString )
{
	return cmp( (*this), sString ) >= 0;
}
