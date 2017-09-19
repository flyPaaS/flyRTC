#ifndef __MMCOMPRESS__
#define __MMCOMPRESS__

#include "rbbuffer.h"

enum
{
	// MMCPR_ZLIB_COMPRESS & MMCPR_NO_COMPRESS supported
	MMCOMPRESS_PACK_VERSION_1=1001,
};

/// 当前最新的压缩包版本，Compress方法所设置的版本号
#define LATEST_MMCOMPRESS_PACK_VERSION MMCOMPRESS_PACK_VERSION_1

namespace MicroMsg
{
	enum emRetCode
	{
		MMCPR_OK = 0,
		
		MMCPR_ERR_GENERAL = 1,
		MMCPR_ERR_INVALID_ARG = 2,
		MMCPR_ERR_PACK_CORRUPTED = 3,
		MMCPR_ERR_UNSUPPORTED_VERSION = 4,
		MMCPR_ERR_COMPRESS_FAILED = 5,
		MMCPR_ERR_DECOMPRESS_FAILED = 6,
		MMCPR_ERR_UNSUPPORTED_ALGORITHM = 7,
		MMCPR_ERR_NOCOMPRESS= 8, // not need compress
	};

	enum emCompressAlg
	{
		MMCPR_ZLIB_COMPRESS = 1, 
		MMCPR_NO_COMPRESS = 2,		
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

