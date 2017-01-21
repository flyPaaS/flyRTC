#include "iVByte.h"
#include <string.h>

namespace Comm
{

int EncodeVByte32(unsigned int auValue, unsigned char *apcBuffer)
{
	int pos = 0;

	while (auValue >= 128) 
	{
		if( apcBuffer != NULL )
			apcBuffer[pos++] = 128 + (unsigned char)(auValue & 127);
		else
			++pos;
		auValue >>= 7;
	}   
	
	apcBuffer[pos++] = (unsigned char)auValue;

	return pos;
}

int DecodeVByte32(unsigned int *apuValue, const unsigned char *apcBuffer)
{
	int pos = 0, shift = 0;
	unsigned int dummy, result = 0;
	unsigned char b = apcBuffer[pos++];

	while (b >= 128) 
	{
		dummy = (b & 127);
		result += (dummy << shift);
		shift += 7;
		b = apcBuffer[pos++];
	}

	dummy = b;
	result += (dummy << shift);
	memcpy((char*)apuValue, (char*)&result, sizeof(unsigned int));

	return pos;
}

}
