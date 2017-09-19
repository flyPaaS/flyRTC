#pragma once

#include "iProtocolXP.h"
#include "iTLVPickle.h"
#include "iTypeXP.h"

namespace Comm
{

class CMsgBodyXP :
	public CTLVPack
{
public:
	CMsgBodyXP(void);
	~CMsgBodyXP(void);

	int  InitFromBuffer(const char *apBuf, int aiSize);
	int  PackToBuffer(char **appBuf, int *apiSize);
};

}

