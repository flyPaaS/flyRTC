#include "pcp_comm.h"

void res_st(IN void* st, IN int size)
{
	memset(st, 0x00, size);
}


OUT void* pcp_malloc(IN int size)
{
	return ms_malloc0(size);
}

void pcp_free(IN void* p)
{
	if (p != NULL)
	{
		ms_free(p);
		p = NULL;
	}
}


