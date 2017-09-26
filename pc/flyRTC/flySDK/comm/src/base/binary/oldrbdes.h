#ifndef _OLD_DES_H_
#define _OLD_DES_H_

namespace olddes {

//#include <e32cmn.h>
#include "typexp_xyssl.h"

typedef char (*SUBKEY_P)[16][48];

enum {ENCRYPTION, DECRYPTION};

char Using_DES(char *Out, const char *In, 
			long textlen, int keylen,
			const char *Key, char Type);

char Using_DES(char *Out, const char *In, 
			long textlen, int keylen,
			const char *Key, char Type, char InitVec[9]);

}

#endif


