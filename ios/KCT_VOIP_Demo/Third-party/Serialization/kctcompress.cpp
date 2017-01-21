#include "kctcompress.h"
#include "iLog.h"

#include <stdlib.h>
#include <unistd.h>

#if 0

#ifdef linux
	#include <zlib.h>
#else
	#include "zlib/zlib.h"
#endif

#endif


using namespace KCTMsg;
using namespace Comm;

bool KCTMsg::IsAlgorithmSupported(short aiAlgorithm, short aiVersion)
{
	if (aiVersion == KCTCOMPRESS_PACK_VERSION_1) // version 1 support 2 alg
	{
		if (aiAlgorithm != KCTCPR_ZLIB_COMPRESS &&
			aiAlgorithm != KCTCPR_NO_COMPRESS)
		{
			return false;
		}
	}
	else 
	{
		return false; // unknown version
	}

	return true;
}

bool KCTMsg::IsVersionSupported(short aiVersion)
{
	if (aiVersion != KCTCOMPRESS_PACK_VERSION_1 )
	{
		return false;
	}
	return true;
}

int KCTMsg::Compress(
	const unsigned char *apPlainBuf, 
	unsigned long adwPlainBufLen, 
	RBBuffer *apoCompressedBuf,
	short &aiAlgorithm,
	short &aiVersion)
{
    
#if 0
	// argument validation
	if(apPlainBuf == NULL) return KCTCPR_ERR_INVALID_ARG;
	if(apoCompressedBuf == NULL) return KCTCPR_ERR_INVALID_ARG;

	if (aiAlgorithm == KCTCPR_ZLIB_COMPRESS  && IsVersionSupported(aiVersion) && 
		IsAlgorithmSupported(aiAlgorithm, aiVersion)) 
	{
		unsigned long dwCompressedLen = compressBound(adwPlainBufLen);
		
		RBBuffer CompressedBuf;
		CompressedBuf.EnsureWriteSize(dwCompressedLen);

		int ret = compress(CompressedBuf.GetArray(), (uLongf*)&dwCompressedLen, (const unsigned char*)apPlainBuf, adwPlainBufLen);
		CompressedBuf.SetWritePos(dwCompressedLen);

		
		if( ret == Z_OK && dwCompressedLen < adwPlainBufLen ) //用压缩的结果
		{
			apoCompressedBuf->Reset();
			apoCompressedBuf->Attach(&CompressedBuf);
			return KCTCPR_OK;
		}
	}
	//  all other condition, use no compress to replace
	aiAlgorithm=KCTCPR_NO_COMPRESS;
	aiVersion=KCTCOMPRESS_PACK_VERSION_1;
	apoCompressedBuf->Reset();
	apoCompressedBuf->EnsureWriteSize(adwPlainBufLen);
	apoCompressedBuf->Write(apPlainBuf, adwPlainBufLen);
		
 #endif
    
	return KCTCPR_ERR_NOCOMPRESS;
}

int KCTMsg::Decompress(
	const unsigned char *apCompressedBuf, 
	unsigned long adwCompressedBufLen,
	RBBuffer *apoDecompressedBuf,
	unsigned long adwDecompressedSize, 
	short aiAlgorithm,
	short aiVersion)
{
#if 0
    
	if(apCompressedBuf == NULL) return KCTCPR_ERR_INVALID_ARG;
	if(apoDecompressedBuf == NULL) return KCTCPR_ERR_INVALID_ARG;
	if(adwCompressedBufLen <= 0 ) return KCTCPR_ERR_INVALID_ARG;
	if(adwDecompressedSize <= 0 ) return KCTCPR_ERR_INVALID_ARG;

	if (false == IsVersionSupported(aiVersion))
	{
		return KCTCPR_ERR_UNSUPPORTED_VERSION;
	}

	if (false == IsAlgorithmSupported(aiAlgorithm, aiVersion))
	{
		return KCTCPR_ERR_UNSUPPORTED_ALGORITHM;
	}

	if (aiAlgorithm == KCTCPR_ZLIB_COMPRESS )  // need decompress
	{
		RBBuffer bufRetTmp;
		bufRetTmp.EnsureWriteSize(adwDecompressedSize);

		int ret=uncompress( bufRetTmp.GetArray(), (uLongf*)&adwDecompressedSize, (const Bytef*)apCompressedBuf, (uLong)adwCompressedBufLen);
		if( ret != Z_OK )
		{
			return KCTCPR_ERR_DECOMPRESS_FAILED;
		}
		bufRetTmp.SetWritePos((int)adwDecompressedSize); // BUGBUG: rbbuffer only supports int size
		apoDecompressedBuf->Reset();
		apoDecompressedBuf->Attach(&bufRetTmp);
		return KCTCPR_OK;
	}
	else if( aiAlgorithm == KCTCPR_NO_COMPRESS ) // no compress, just copy to rbbuffer
	{
		apoDecompressedBuf->EnsureWriteSize(adwCompressedBufLen);
		apoDecompressedBuf->Write(apCompressedBuf, adwCompressedBufLen);
		return KCTCPR_OK;
	}
	else
	{
		return KCTCPR_ERR_DECOMPRESS_FAILED;
	}
#endif
	return KCTCPR_ERR_DECOMPRESS_FAILED;
}



