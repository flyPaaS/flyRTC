#pragma once

#include "tlvport.h"
#include "skmetainfo.h"

namespace Comm
{

enum { 
    eTypeSKBuiltinInt32 = eTypeSKBuiltinType + 1,
    eTypeSKBuiltinUint32,
    eTypeSKBuiltinChar,
    eTypeSKBuiltinUchar,
    eTypeSKBuiltinInt8,
    eTypeSKBuiltinUint8,
    eTypeSKBuiltinInt16,
    eTypeSKBuiltinUint16,
    eTypeSKBuiltinInt64,
    eTypeSKBuiltinUint64,
    eTypeSKBuiltinFloat32,
    eTypeSKBuiltinDouble64,
    eTypeSKBuiltinQSUint,

    // ================

    eTypeSKBuiltinBuffer,
    eTypeSKBuiltinString
};

#pragma pack(1)

typedef struct tagSKBuiltinInt32 {
	int iVal;
} SKBuiltinInt32_t;

typedef struct tagSKBuiltinUint32 {
	unsigned int iVal;
} SKBuiltinUint32_t;

typedef struct tagSKBuiltinChar {
	char cVal;
} SKBuiltinChar_t;

typedef struct tagSKBuiltinUchar {
	unsigned char cVal;
} SKBuiltinUchar_t;

typedef struct tagSKBuiltinInt8 {
	char cVal;
} SKBuiltinInt8_t;

typedef struct tagSKBuiltinUint8 {
	unsigned char cVal;
} SKBuiltinUint8_t;

typedef struct tagSKBuiltinInt16 {
	short hVal;
} SKBuiltinInt16_t;

typedef struct tagSKBuiltinUint16 {
	unsigned short hVal;
} SKBuiltinUint16_t;

typedef struct tagSKBuiltinInt64 {
	long long llVal;
} SKBuiltinInt64_t;

typedef struct tagSKBuiltinUint64 {
	unsigned long long llVal;
} SKBuiltinUint64_t;

typedef struct tagSKBuiltinFloat32 {
	float fVal;
} SKBuiltinFloat32_t;

typedef struct tagSKBuiltinDouble64 {
	double dVal;
} SKBuiltinDouble64_t;

////////
typedef struct tagSKBuiltinBuffer{
    int iLen;
    char * pcBuff;
} SKBuiltinBuffer_t;

typedef struct tagSKBuiltinString {
	char * pcBuff;
} SKBuiltinString_t;

#pragma pack()

typedef struct tagSKMetaInfo SKMetaInfo_t;
extern const SKMetaInfo_t * g_ptSKBuiltinMetaInfo;

class SKBuffer;

class SKBuiltinPickle
{
public:

	static int ToBuffer( qs_uin_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, qs_uin_t * ptStruct );
	static int FreeStructField( qs_uin_t & tStruct );

	static int ToBuffer( qs_uin_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus );
	static int FromBuffer( SKBuffer * poBuffer, qs_uin_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( qs_uin_t * pDestStruct, const qs_uin_t * pSourceStruct );
	static int Diff( qs_uin_t * ptStruct1, qs_uin_t * ptStruct2 );
	static int IsMatch( qs_uin_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( qs_uin_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinInt32_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt32_t * ptStruct );
	static int FreeStructField( SKBuiltinInt32_t & tStruct );

	static int ToBuffer( SKBuiltinInt32_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt32_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinInt32_t * pDestStruct, const SKBuiltinInt32_t * pSourceStruct );
	static int Diff( SKBuiltinInt32_t * ptStruct1, SKBuiltinInt32_t * ptStruct2 );
	static int IsMatch( SKBuiltinInt32_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinInt32_t * ptStruct, Comm::SKBuffer * poBuffer );


	static int ToBuffer( SKBuiltinUint32_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint32_t * ptStruct );
	static int FreeStructField( SKBuiltinUint32_t & tStruct );

	static int ToBuffer( SKBuiltinUint32_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint32_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinUint32_t * pDestStruct, const SKBuiltinUint32_t * pSourceStruct );
	static int Diff( SKBuiltinUint32_t * ptStruct1, SKBuiltinUint32_t * ptStruct2 );
	static int IsMatch( SKBuiltinUint32_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinUint32_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinChar_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinChar_t * ptStruct );
	static int FreeStructField( SKBuiltinChar_t & tStruct );

	static int ToBuffer( SKBuiltinChar_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinChar_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinChar_t * pDestStruct, const SKBuiltinChar_t * pSourceStruct );
	static int Diff( SKBuiltinChar_t * ptStruct1, SKBuiltinChar_t * ptStruct2 );
	static int IsMatch( SKBuiltinChar_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinChar_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinUchar_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUchar_t * ptStruct );
	static int FreeStructField( SKBuiltinUchar_t & tStruct );

	static int ToBuffer( SKBuiltinUchar_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUchar_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinUchar_t * pDestStruct, const SKBuiltinUchar_t * pSourceStruct );
	static int Diff( SKBuiltinUchar_t * ptStruct1, SKBuiltinUchar_t * ptStruct2 );
	static int IsMatch( SKBuiltinUchar_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinUchar_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinInt8_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt8_t * ptStruct );
	static int FreeStructField( SKBuiltinInt8_t & tStruct );

	static int ToBuffer( SKBuiltinInt8_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt8_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinInt8_t * pDestStruct, const SKBuiltinInt8_t * pSourceStruct );
	static int Diff( SKBuiltinInt8_t * ptStruct1, SKBuiltinInt8_t * ptStruct2 );
	static int IsMatch( SKBuiltinInt8_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinInt8_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinUint8_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint8_t * ptStruct );
	static int FreeStructField( SKBuiltinUint8_t & tStruct );

	static int ToBuffer( SKBuiltinUint8_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint8_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinUint8_t * pDestStruct, const SKBuiltinUint8_t * pSourceStruct );
	static int Diff( SKBuiltinUint8_t * ptStruct1, SKBuiltinUint8_t * ptStruct2 );
	static int IsMatch( SKBuiltinUint8_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinUint8_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinInt16_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt16_t * ptStruct );
	static int FreeStructField( SKBuiltinInt16_t & tStruct );

	static int ToBuffer( SKBuiltinInt16_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt16_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinInt16_t * pDestStruct, const SKBuiltinInt16_t * pSourceStruct );
	static int Diff( SKBuiltinInt16_t * ptStruct1, SKBuiltinInt16_t * ptStruct2 );
	static int IsMatch( SKBuiltinInt16_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinInt16_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinUint16_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint16_t * ptStruct );
	static int FreeStructField( SKBuiltinUint16_t & tStruct );

	static int ToBuffer( SKBuiltinUint16_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint16_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinUint16_t * pDestStruct, const SKBuiltinUint16_t * pSourceStruct );
	static int Diff( SKBuiltinUint16_t * ptStruct1, SKBuiltinUint16_t * ptStruct2 );
	static int IsMatch( SKBuiltinUint16_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinUint16_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinInt64_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt64_t * ptStruct );
	static int FreeStructField( SKBuiltinInt64_t & tStruct );

	static int ToBuffer( SKBuiltinInt64_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinInt64_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinInt64_t * pDestStruct, const SKBuiltinInt64_t * pSourceStruct );
	static int Diff( SKBuiltinInt64_t * ptStruct1, SKBuiltinInt64_t * ptStruct2 );
	static int IsMatch( SKBuiltinInt64_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinInt64_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinUint64_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint64_t * ptStruct );
	static int FreeStructField( SKBuiltinUint64_t & tStruct );

	static int ToBuffer( SKBuiltinUint64_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinUint64_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinUint64_t * pDestStruct, const SKBuiltinUint64_t * pSourceStruct );
	static int Diff( SKBuiltinUint64_t * ptStruct1, SKBuiltinUint64_t * ptStruct2 );
	static int IsMatch( SKBuiltinUint64_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinUint64_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinFloat32_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinFloat32_t * ptStruct );
	static int FreeStructField( SKBuiltinFloat32_t & tStruct );

	static int ToBuffer( SKBuiltinFloat32_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinFloat32_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinFloat32_t * pDestStruct, const SKBuiltinFloat32_t * pSourceStruct );
	static int Diff( SKBuiltinFloat32_t * ptStruct1, SKBuiltinFloat32_t * ptStruct2 );
	static int IsMatch( SKBuiltinFloat32_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinFloat32_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinDouble64_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinDouble64_t * ptStruct );
	static int FreeStructField( SKBuiltinDouble64_t & tStruct );

	static int ToBuffer( SKBuiltinDouble64_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinDouble64_t * ptStruct , int iPickleType, int iPickleStatus);
	
	static int DeepCopy( SKBuiltinDouble64_t * pDestStruct, const SKBuiltinDouble64_t * pSourceStruct );
	static int Diff( SKBuiltinDouble64_t * ptStruct1, SKBuiltinDouble64_t * ptStruct2 );
	static int IsMatch( SKBuiltinDouble64_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinDouble64_t * ptStruct, Comm::SKBuffer * poBuffer );

    /////////////////////

	static int ToBuffer( SKBuiltinBuffer_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinBuffer_t * ptStruct );
	static int FreeStructField( SKBuiltinBuffer_t & tStruct );

	static int ToBuffer( SKBuiltinBuffer_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinBuffer_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinBuffer_t * pDestStruct, const SKBuiltinBuffer_t * pSourceStruct );
	static int Diff( SKBuiltinBuffer_t * ptStruct1, SKBuiltinBuffer_t * ptStruct2 );
	static int IsMatch( SKBuiltinBuffer_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinBuffer_t * ptStruct, Comm::SKBuffer * poBuffer );

	static int ToBuffer( SKBuiltinString_t * ptStruct, SKBuffer * poBuffer );
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinString_t * ptStruct );
	static int FreeStructField( SKBuiltinString_t & tStruct );

	static int ToBuffer( SKBuiltinString_t * ptStruct, SKBuffer * poBuffer , int iPickleType, int iPickleStatus);
	static int FromBuffer( SKBuffer * poBuffer, SKBuiltinString_t * ptStruct , int iPickleType, int iPickleStatus);

	static int DeepCopy( SKBuiltinString_t * pDestStruct, const SKBuiltinString_t * pSourceStruct );
	static int Diff( SKBuiltinString_t * ptStruct1, SKBuiltinString_t * ptStruct2 );
	static int IsMatch( SKBuiltinString_t * ptStruct, Comm::SKBuffer * poBuffer );
	static int IsNewStruct( SKBuiltinString_t * ptStruct, Comm::SKBuffer * poBuffer );
};

}

