#include "typexp.h"




#ifdef android
int memcompare (const unsigned char* aLeft, int aLeftLen, const unsigned char* aRight, int aRightLen)
{
	assert(aLeftLen == aRightLen);
	return memcmp(aLeft,aRight,aLeftLen);
}
#endif

#ifdef __SYMBIAN32__
//int atoi(const char *aps)
//{
//	int n = 0;
//	TLex8 l;
//	l.Assign((unsigned char*)aps);
//	if (l.Val(n) != KErrNone) return 0;
//	return n;
//}
#endif


