#ifndef __KCTCOMPRESS__
#define __KCTCOMPRESS__

#include "rbbuffer.h"

enum
{
	// KCTCPR_ZLIB_COMPRESS & KCTCPR_NO_COMPRESS supported
	KCTCOMPRESS_PACK_VERSION_1=1001,
};

/// 当前最新的压缩包版本，Compress方法所设置的版本号
#define LATEST_KCTCOMPRESS_PACK_VERSION KCTCOMPRESS_PACK_VERSION_1

namespace KCTMsg
{
	enum emRetCode
	{
		KCTCPR_OK = 0,
		
		KCTCPR_ERR_GENERAL = 1,
		KCTCPR_ERR_INVALID_ARG = 2,
		KCTCPR_ERR_PACK_CORRUPTED = 3,
		KCTCPR_ERR_UNSUPPORTED_VERSION = 4,
		KCTCPR_ERR_COMPRESS_FAILED = 5,
		KCTCPR_ERR_DECOMPRESS_FAILED = 6,
		KCTCPR_ERR_UNSUPPORTED_ALGORITHM = 7,
		KCTCPR_ERR_NOCOMPRESS= 8, // not need compress
	};

	enum emCompressAlg
	{
		KCTCPR_ZLIB_COMPRESS = 1, 
		KCTCPR_NO_COMPRESS = 2,		
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

