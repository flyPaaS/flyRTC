#pragma once

namespace Comm
{

typedef struct _unsigned_uin_t {
	unsigned int uin;

	operator unsigned int() const {return uin;}

	bool operator < ( const _unsigned_uin_t & it ) const 
	{
		return uin < it.uin;
	}
	
	bool operator > ( const _unsigned_uin_t & it ) const 
	{
		return uin > it.uin;
	}
} qs_uin_t;

const static qs_uin_t QS_UIN_ZERO = { 0 };
const static qs_uin_t QS_UIN_MIN = { 10000 };

}

