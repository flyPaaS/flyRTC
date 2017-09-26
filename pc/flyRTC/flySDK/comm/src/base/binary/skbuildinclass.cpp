/*
 * =====================================================================================
 *
 *       Filename:  skbuildinclass.cpp
 *
 *    Description:  eddy
 *
 *        Version:  1.0
 *        Created:  03/26/2012 03:37:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "skbuiltintype.h"
#include "skbuildinclass.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


namespace Comm
{

SKBuiltinBuff :: SKBuiltinBuff()
{
}

SKBuiltinBuff :: SKBuiltinBuff( const SKBuiltinBuff & other )
{
}

SKBuiltinBuff :: ~SKBuiltinBuff()
{
	Free();
}

SKBuiltinBuff & SKBuiltinBuff :: operator=( const SKBuiltinBuff & other )
{
	if(this == & other)
	{
		return *this;
	}
	Free();
	SKBuiltinBuffer_t *ptStruct=other.GetStruct();
	if(ptStruct->iLen<=0 || ptStruct->pcBuff==NULL)
	{
		return *this;
	}
	SKBuiltinBuffer_t * tmp=this->GetStruct();
	tmp->iLen=ptStruct->iLen;
	tmp->pcBuff=(char*)malloc(tmp->iLen);
	assert(tmp->pcBuff != NULL);
	memcpy(tmp->pcBuff, ptStruct->pcBuff, tmp->iLen);
	return *this;
}

SKBuiltinBuffer_t * SKBuiltinBuff :: GetStruct() const
{
	return (SKBuiltinBuffer_t *)this;
}

SKBuiltinBuff * SKBuiltinBuff :: Alloc()
{
	SKBuiltinBuff * tmp=( SKBuiltinBuff *)calloc( sizeof(SKBuiltinBuffer_t),1);
	assert(tmp!=NULL);
	return tmp;
}

size_t SKBuiltinBuff :: size()
{
	return sizeof(SKBuiltinBuffer_t);
}

void SKBuiltinBuff :: Free()
{
	SKBuiltinBuffer_t * tmp=this->GetStruct();
	if(tmp->pcBuff!=NULL)
	{
		free(tmp->pcBuff);
	   	tmp->pcBuff=NULL;
	}
	tmp->iLen=0;
}

void SKBuiltinBuff :: SetBuff(const char * pcBuff, unsigned int iLen)
{
	Free();
	SKBuiltinBuffer_t * tmp=this->GetStruct();
	if(pcBuff==NULL || iLen <=0)
	{
		return;
	}
	tmp->iLen=iLen;
	tmp->pcBuff=(char*)malloc(tmp->iLen);
	assert(tmp->pcBuff != NULL);
	memcpy(tmp->pcBuff, pcBuff, tmp->iLen);
}

char * SKBuiltinBuff :: GetBuff() const
{
	SKBuiltinBuffer_t * tmp=this->GetStruct();
	return tmp->pcBuff;
	
}

unsigned int SKBuiltinBuff :: GetLen() const
{
	SKBuiltinBuffer_t * tmp=this->GetStruct();
	return tmp->iLen;
}
//==================================================

SKBuiltinStr :: SKBuiltinStr()
{
}

SKBuiltinStr :: SKBuiltinStr( const SKBuiltinStr & other )
{
}

SKBuiltinStr :: ~SKBuiltinStr()
{
	Free();
}

SKBuiltinStr & SKBuiltinStr :: operator=( const SKBuiltinStr & other )
{
	if(this == & other)
	{
		return *this;
	}
	Free();
	SKBuiltinString_t * ptStruct=other.GetStruct();
	if(ptStruct->pcBuff==NULL)
	{
		return *this; 
	}
	SKBuiltinString_t * tmp=this->GetStruct();
	tmp->pcBuff=strdup(ptStruct->pcBuff);
	assert(tmp->pcBuff != NULL);
	return *this;
}

SKBuiltinString_t * SKBuiltinStr :: GetStruct() const
{
	return (SKBuiltinString_t *)this;
}

SKBuiltinStr * SKBuiltinStr :: Alloc()
{
	SKBuiltinStr * tmp=(SKBuiltinStr *)calloc( sizeof(SKBuiltinString_t),1);
	assert(tmp!=NULL);
	return tmp;
}

size_t SKBuiltinStr :: size()
{
	return sizeof(SKBuiltinString_t);
}

void SKBuiltinStr :: Free()
{
	SKBuiltinString_t * tmp=this->GetStruct();
	if(tmp->pcBuff!=NULL)
	{
		free(tmp->pcBuff);
	   	tmp->pcBuff=NULL;
	}
}

void SKBuiltinStr :: SetStr(const char * pcStr)
{
	Free();
	SKBuiltinString_t * tmp=this->GetStruct();
	if(pcStr==NULL)
	{
		return;
	}
	tmp->pcBuff=strdup(pcStr);
	assert(tmp->pcBuff!=NULL);
}

char * SKBuiltinStr :: GetStr() const
{
	SKBuiltinString_t * tmp=this->GetStruct();
	return tmp->pcBuff;
}

}
