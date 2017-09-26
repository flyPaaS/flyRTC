#include "typexp_xyssl.h"

#ifdef __SYMBIAN32__
int rand()
{
	return Math::Random();	
}
#endif



