#ifndef __UCSCOMPRESS__
#define __UCSCOMPRESS__

#include "rbbuffer.h"

enum
{
	// UCSCPR_ZLIB_COMPRESS & UCSCPR_NO_COMPRESS supported
	UCSCOMPRESS_PACK_VERSION_1=1001,
};

/// 当前最新的压缩包版本，Compress方法所设置的版本号
#define LATEST_UCSCOMPRESS_PACK_VERSION UCSCOMPRESS_PACK_VERSION_1

namespace UcsMsg
{
	enum emRetCode
	{
		UCSCPR_OK = 0,
		
		UCSCPR_ERR_GENERAL = 1,
		UCSCPR_ERR_INVALID_ARG = 2,
		UCSCPR_ERR_PACK_CORRUPTED = 3,
		UCSCPR_ERR_UNSUPPORTED_VERSION = 4,
		UCSCPR_ERR_COMPRESS_FAILED = 5,
		UCSCPR_ERR_DECOMPRESS_FAILED = 6,
		UCSCPR_ERR_UNSUPPORTED_ALGORITHM = 7,
		UCSCPR_ERR_NOCOMPRESS= 8, // not need compress
	};

	enum emCompressAlg
	{
		UCSCPR_ZLIB_COMPRESS = 1, 
		UCSCPR_NO_COMPRESS = 2,		
	};

	int Compress(
		const unsigned char *apPlainBuf, 
		unsigned long adwPlainBufLen, 
		RBBuffer *apoCompressedBuf,
		short &aiAlgorithm, 
		short &aiVersion); 

	int Decompress(
		const unsigned char *apCompressedBuf, 
		unsigned long adwCompressedBufLen, 
		RBBuffer *apoDecompressedBuf,
		unsigned long adwDecompressedSize,
		short aiAlgorithm, 
		short aiVersion); 

	bool IsAlgorithmSupported(short aiAlgorithm, short aiVersion ); 

	bool IsVersionSupported(short aiVersion);
};

#endif

