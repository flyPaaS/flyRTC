/*
 * =====================================================================================
 *
 *       Filename:  skbuildinclass.h
 *
 *    Description:  eddy
 *
 *        Version:  1.0
 *        Created:  03/26/2012 03:09:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#pragma once

namespace Comm
{
class SKBuiltinBuff 
{
public:
	typedef struct tagSKBuiltinBuffer  SKBuiltinBuffer_t;

	~SKBuiltinBuff();
	SKBuiltinBuff & operator=(const SKBuiltinBuff & other);

	inline SKBuiltinBuffer_t * GetStruct() const ;
	static SKBuiltinBuff * Alloc();
	static size_t size();
	void Free();

	void SetBuff(const char * pcBuff, unsigned int iLen);

	char * GetBuff() const;
	unsigned int GetLen() const;

private:
	SKBuiltinBuff();
	SKBuiltinBuff(const SKBuiltinBuff & other);
};

class SKBuiltinStr 
{
public:
	typedef struct tagSKBuiltinString  SKBuiltinString_t;

	~SKBuiltinStr();
	SKBuiltinStr & operator=(const SKBuiltinStr & other);

	inline SKBuiltinString_t * GetStruct() const ;
	static SKBuiltinStr * Alloc();
	static size_t size();
	void Free();

	void SetStr(const char * pcBuff);
	char * GetStr() const;

private:
	SKBuiltinStr();
	SKBuiltinStr(const SKBuiltinStr & other);
};
}
