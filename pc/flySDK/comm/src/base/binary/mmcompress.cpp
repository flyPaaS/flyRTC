#include "mmcompress.h"
#include "iLog.h"

#include <zlib.h>

using namespace MicroMsg;
using namespace Comm;

bool MicroMsg::IsAlgorithmSupported(short aiAlgorithm, short aiVersion)
{
	if (aiVersion == MMCOMPRESS_PACK_VERSION_1) // version 1 support 2 alg
	{
		if (aiAlgorithm != MMCPR_ZLIB_COMPRESS &&
			aiAlgorithm != MMCPR_NO_COMPRESS)
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

bool MicroMsg::IsVersionSupported(short aiVersion)
{
	if (aiVersion != MMCOMPRESS_PACK_VERSION_1 )
	{
		return false;
	}
	return true;
}

int MicroMsg::Compress(
	const unsigned char *apPlainBuf, 
	unsigned long adwPlainBufLen, 
	RBBuffer *apoCompressedBuf,
	short &aiAlgorithm,
	short &aiVersion)
{
#if 1
	// argument validation
	if(apPlainBuf == NULL) return MMCPR_ERR_INVALID_ARG;
	if(apoCompressedBuf == NULL) return MMCPR_ERR_INVALID_ARG;

	if (aiAlgorithm == MMCPR_ZLIB_COMPRESS  && IsVersionSupported(aiVersion) && 
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
			return MMCPR_OK;
		}
	}
	//  all other condition, use no compress to replace
	aiAlgorithm = MMCPR_NO_COMPRESS;
	aiVersion = MMCOMPRESS_PACK_VERSION_1;
	apoCompressedBuf->Reset();
	apoCompressedBuf->EnsureWriteSize(adwPlainBufLen);
	apoCompressedBuf->Write(apPlainBuf, adwPlainBufLen);
#endif
	return MMCPR_ERR_NOCOMPRESS;
}

int MicroMsg::Decompress(
	const unsigned char *apCompressedBuf, 
	unsigned long adwCompressedBufLen,
	RBBuffer *apoDecompressedBuf,
	unsigned long adwDecompressedSize, 
	short aiAlgorithm,
	short aiVersion)
{
#if 1
	if(apCompressedBuf == NULL) return MMCPR_ERR_INVALID_ARG;
	if(apoDecompressedBuf == NULL) return MMCPR_ERR_INVALID_ARG;
	if(adwCompressedBufLen <= 0 ) return MMCPR_ERR_INVALID_ARG;
	if(adwDecompressedSize <= 0 ) return MMCPR_ERR_INVALID_ARG;

	if (false == IsVersionSupported(aiVersion))
	{
		return MMCPR_ERR_UNSUPPORTED_VERSION;
	}

	if (false == IsAlgorithmSupported(aiAlgorithm, aiVersion))
	{
		return MMCPR_ERR_UNSUPPORTED_ALGORITHM;
	}

	if (aiAlgorithm == MMCPR_ZLIB_COMPRESS )  // need decompress
	{
		RBBuffer bufRetTmp;
		bufRetTmp.EnsureWriteSize(adwDecompressedSize);

		int ret = uncompress( bufRetTmp.GetArray(), (uLongf*)&adwDecompressedSize, (const Bytef*)apCompressedBuf, (uLong)adwCompressedBufLen);
		if( ret != Z_OK )
		{
			return MMCPR_ERR_DECOMPRESS_FAILED;
		}
		bufRetTmp.SetWritePos((int)adwDecompressedSize); // BUGBUG: rbbuffer only supports int size
		apoDecompressedBuf->Reset();
		apoDecompressedBuf->Attach(&bufRetTmp);
		return MMCPR_OK;
	}
	else if( aiAlgorithm == MMCPR_NO_COMPRESS ) // no compress, just copy to rbbuffer
	{
		apoDecompressedBuf->EnsureWriteSize(adwCompressedBufLen);
		apoDecompressedBuf->Write(apCompressedBuf, adwCompressedBufLen);
		return MMCPR_OK;
	}
	else
	{
		return MMCPR_ERR_DECOMPRESS_FAILED;
	}
	return MMCPR_ERR_DECOMPRESS_FAILED;
#endif
    return MMCPR_OK;
}



