#pragma once


namespace Comm
{

class SKBuffer;
class CTLVPack;
class SKTLVMergePack;

typedef struct tagSKTLVHelperImpl SKTLVHelperImpl_t;

class SKTLVHelper
{
public:
	SKTLVHelper();

	~SKTLVHelper();

	//old, new, dest
	int Merge(
		const SKBuffer * poSrcBuffer1,
		const SKBuffer * poSrcBuffer2,
		SKBuffer * poDestBuffer
		);
	static bool CheckIsMM();

private:
	int MergeBuffer(
		SKTLVMergePack * poMergePack,
		CTLVPack * poTlvBuffer
		);
	static void GetHostName();
};


}


