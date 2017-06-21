
#include "buffertools.h"
#include <string.h>
#include <stdlib.h>
#include "skbuffer.h"

#include "tlvport.h"

using namespace Comm;
int BufferTools :: Buffer2String( const unsigned char * pcBuffer, const int &iLen, char ** pcStr )
{
	int strlen = iLen * 2;
	*pcStr = (char *)calloc( sizeof(char), strlen );
	if( *pcStr == NULL )
	{
		LogErr("%s calloc error",__func__);
		return -1;
	}

	int ret = 0;
	char * pcPos = *pcStr;
	for( int i = 0; i < iLen; ++i )
	{
		if( Encode(pcBuffer[i],&pcPos) )
		{
			LogErr("%s encode error pos (%d) char(%c %u)",__func__, i, pcBuffer[i], pcBuffer[i]);
			ret = -1;
			break;
		}
	}
	if( ret )
	{
		free( (*pcStr ) );
		*pcStr = NULL;
	}
	return ret;
}

int BufferTools :: String2Buffer( const char * pcStr, unsigned char ** pcBuffer, int * iLen )
{
	int len = strlen( pcStr );
	if( len & 0x1 )
	{
		LogErr("%s string len error len(%d)",__func__, len);
		return -1;
	}

	*iLen = len/2;
	(*pcBuffer) = ( unsigned char * ) calloc( sizeof(unsigned char), *iLen );
	if( *pcBuffer == NULL)
	{
		LogErr("%s calloc error",__func__);
		return -1;
	}

	int ret = 0;
	const char * pcStrPos = pcStr;
	for( int i = 0; i < *iLen; ++i )
	{
		if( Decode(&pcStrPos ,(*pcBuffer)[i]) )
		{
			LogErr("%s decode error pos (%d) char(%c %u)",__func__, i, *pcStrPos, *pcStrPos);
			ret = -1;
		}
	}
	if( ret )
	{
		free( *pcBuffer );
		*pcBuffer = NULL;
	}
	return ret;
}

int BufferTools :: Buffer2String( const Comm::SKBuffer * poBuffer, char ** pcStr )
{
	return Buffer2String( poBuffer->GetBuffer(), poBuffer->GetLen(), pcStr );
}

int BufferTools :: String2Buffer( const char * pcStr, Comm::SKBuffer *poBuffer )
{
	unsigned char * pcBuffer = NULL;
	int iLen = 0;

	int ret = String2Buffer( pcStr, &pcBuffer, &iLen );
	if( ret == 0 )
	{
		poBuffer->Write( pcBuffer, iLen );
	}
	return ret;
}

int BufferTools :: Encode(unsigned char val, char ** pcStr)
{
	if( pcStr == NULL )
	{
		return -1;
	}
	**pcStr = val & 0xf;
	**pcStr += 'a';
	(*pcStr)++;
	**pcStr = val >> 4;
	**pcStr += 'a';
	(*pcStr)++;
	return 0;
}

int BufferTools :: Decode(const char ** pcStr, unsigned char &val)
{
	if( pcStr == NULL )
	{
		return -1;
	}
	val = (**pcStr)-'a';
	(*pcStr)++;
	if( *pcStr == NULL )
	{
		return -1;
	}
	val |= ((**pcStr)-'a')<<4;
	(*pcStr)++;
	return 0;
}
