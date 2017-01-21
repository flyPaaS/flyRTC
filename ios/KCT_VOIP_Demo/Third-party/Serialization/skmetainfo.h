#pragma once

#include <sys/types.h>

namespace Comm
{

enum {
	eTypeSKUnknown = 0,

	eTypeSKChar,
	eTypeSKUchar,

	eTypeSKInt8,
	eTypeSKUint8,

	eTypeSKInt16,
	eTypeSKUint16,

	eTypeSKInt32,
	eTypeSKUint32,

	eTypeSKInt64,
	eTypeSKUint64,

	eTypeSKFloat32,
	eTypeSKDouble64,

	eTypeSKQSUint,

	eTypeSKSInt32,

	eTypeSKSInt64,

	eTypeSKBuiltinType = 256,

	eTypeSKUserDefine = 512
};

enum
{
	ePickleTypeTlv = 1,
	ePickleTypeProtoBuf = 2
};

#define SK_FIELD_OFFSET(type,field) ((long)&(((type *)0)->field))
#define SK_ARRAY_SIZE(array)        ((int)(sizeof(array)/sizeof(array[0])))

typedef struct tagSKMetaField {
	const char *pcName;
	short hOffset;
	short hType;
	char cIsPtr;
	char cIsRequired;
	short hArraySize;
	const char *pcReferTo;
	short hId;
	short iFieldSize;
	short iItemSize;
	unsigned char  cStatus;
	char  cReserved;
} SKMetaField_t;

typedef struct tagSKMetaStruct {
	short hId;
	const char * pcName;
	unsigned int iSize;
	short hFieldCount;
	const SKMetaField_t *ptFieldList;
} SKMetaStruct_t;

typedef struct tagSKMetaParam {
	const char * pcName;
	short hType;
	char cExistence;
} SKMetaParam_t;

typedef struct tagSKMetaFunc {
	const char * pcName;
	short hCmdid;
	SKMetaParam_t tReq;
	SKMetaParam_t tResp;
} SKMetaFunc_t;

typedef struct tagSKMetaInfo {
	const char * pcPrefix;
	const char * pcName;
	int iMagic;
	short hStructCount;
	const SKMetaStruct_t * ptStructList;
	short hFuncCount;
	const SKMetaFunc_t * ptFuncList;

} SKMetaInfo_t;


}

