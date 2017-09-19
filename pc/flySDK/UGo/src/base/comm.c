#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "comm.h"

const char kAlphabetUp[] = "FGELMNOPQRWXHISTUVKABCDJYZ.";
const char kAlphabetLow[] = "uvwxmghijknopalrstbcdefqyz.";


void cm_resst(IN void* st, IN int size)
{
	memset(st, 0, size);
}

int cm_paser_address(IN const char* saddr, OUT sv_addr_t* sv_addr)
{
	char *p = NULL;

	if (saddr == NULL)
		return RES_ERR;

	p = strchr(saddr, ':');
	if (p == NULL)
	{
		strcpy(sv_addr->sv_ip, saddr);
		sv_addr->sv_port = 5060;
	}
    else
	{
		memcpy(sv_addr->sv_ip, saddr, p - saddr);
		sv_addr->sv_ip[p - saddr] = '\0';
		sv_addr->sv_port = atoi(p+1);
	}

	return RES_OK;
}

int cm_sceq(void)
{
	static int sceq_num = 0;
	
	if (sceq_num++ > 1000)
		sceq_num = 0;

	return sceq_num;
}

int cm_rand_time(void)
{
	int rand_num = 0;

	srand((unsigned)time(NULL));
	
	rand_num = rand();

	return rand_num;
}

int cm_abs_i(IN int number)
{
	int abs_value = number;

	if (number < 0)
		abs_value = -number;
	
	return abs_value;
}

OUT int64_t cm_stoi64(IN const char* int_str)
{
	int64_t i64=0;
#ifdef WEBRTC_LINUX
if (int_str)
	{	
		sscanf(int_str , "%ld" , &i64);
	}
#else
	if (int_str)
	{	
		sscanf(int_str , "%lld" , &i64);
	}
#endif
	
	return i64;
}

int cm_rand_alphabet5(OUT char alp_str[6])
{
	int i = 0;
	int j = 0;
	int rand_num = cm_rand_time();
	
	for (i = 0; i < 5; i++)
	{
		j = (rand_num+i+j) % 26;
		if (i%2)
		{
			alp_str[i] = kAlphabetUp[j];
		}
		else
		{
			alp_str[i] = kAlphabetLow[j];
		}
	}

	alp_str[i] = '\0';

	return RES_OK;
}

 
