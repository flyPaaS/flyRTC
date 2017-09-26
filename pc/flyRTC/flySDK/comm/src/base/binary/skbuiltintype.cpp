#include <stdio.h>
#include <string.h>

#include "skmetainfo.h"
#include "skbuiltintype.h"
#include "sktlvpickle.h"
#include "skbuffer.h"
#include "skmetautils.h"
#include "skalloc.h"
#include "skpbpickle.h"
#include "skmetainfofinder.h"

namespace Comm
{

static const SKMetaField_t g_tMetaSKBuiltinInt32 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinInt32_t, iVal), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinUint32 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinUint32_t, iVal), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinChar [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinChar_t, cVal), eTypeSKChar, 0, 1, -1,
		"", 1, sizeof(char), sizeof(char), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinUchar [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinUchar_t, cVal), eTypeSKUchar, 0, 1, -1,
		"", 1, sizeof(unsigned char), sizeof(unsigned char), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinInt8 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinInt8_t, cVal), eTypeSKChar, 0, 1, -1,
		"", 1, sizeof(char), sizeof(char), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinUint8 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinUint8_t, cVal), eTypeSKUchar, 0, 1, -1,
		"", 1, sizeof(unsigned char), sizeof(unsigned char), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinInt16 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinInt16_t, hVal), eTypeSKInt16, 0, 1, -1,
		"", 1, sizeof(short), sizeof(short), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinUint16 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinUint16_t, hVal), eTypeSKUint16, 0, 1, -1,
		"", 1, sizeof(unsigned short), sizeof(unsigned short), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinInt64 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinInt64_t, llVal), eTypeSKInt64, 0, 1, -1,
		"", 1, sizeof(long long), sizeof(long long), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinUint64 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinUint64_t, llVal), eTypeSKUint64, 0, 1, -1,
		"", 1, sizeof(unsigned long long), sizeof(unsigned long long), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinFloat32 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinFloat32_t, fVal), eTypeSKFloat32, 0, 1, -1,
		"", 1, sizeof(float), sizeof(float), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinDouble64 [] = {
	{ "Val", SK_FIELD_OFFSET(SKBuiltinDouble64_t, dVal), eTypeSKDouble64, 0, 1, -1,
		"", 1, sizeof(double), sizeof(double), 1, 0 }
};

static const SKMetaField_t g_tMetaQSUint [] = {
	{ "Val", SK_FIELD_OFFSET(qs_uin_t, uin), eTypeSKUint32, 0, 1, -1,
		"", 1, sizeof(unsigned int), sizeof(unsigned int), 1, 0 }
};
    
    
// =============================

static const SKMetaField_t g_tMetaSKBuiltinBuffer [] = {
	{ "Len", SK_FIELD_OFFSET(SKBuiltinBuffer_t, iLen), eTypeSKInt32, 0, 1, -1,
		"", 1, sizeof(int), sizeof(int), 1, 0 },
	{ "Buff", SK_FIELD_OFFSET(SKBuiltinBuffer_t, pcBuff), eTypeSKChar, 1, 0, -1,
		"Len", 2, sizeof(char *), sizeof(char), 1, 0 }
};

static const SKMetaField_t g_tMetaSKBuiltinString [] = {
	{ "Buff", SK_FIELD_OFFSET(SKBuiltinString_t, pcBuff), eTypeSKChar, 1, 0, -1,
		"strlen", 1, sizeof(char *), sizeof(char), 1, 0 }
};


// ************ function list *************** //

/*static const SKMetaFunc_t g_tMetaSKFuncList [] = {
	{ "SKEcho", eFuncSKBuiltinSKEcho,
		{ "Request", eTypeSKBuiltinEchoInfo, 1 }, { "Response", eTypeSKBuiltinEchoInfo, 1 } }
};
		*/


// *********** struct list  *************** //

static const SKMetaStruct_t g_tSKMetaStructList [] = {
	{ eTypeSKBuiltinInt32, "SKBuiltinInt32_t", sizeof(SKBuiltinInt32_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinInt32), g_tMetaSKBuiltinInt32 },
	{ eTypeSKBuiltinUint32, "SKBuiltinUint32_t", sizeof(SKBuiltinUint32_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinUint32), g_tMetaSKBuiltinUint32 },
	{ eTypeSKBuiltinChar, "SKBuiltinChar_t", sizeof(SKBuiltinChar_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinChar), g_tMetaSKBuiltinChar },
	{ eTypeSKBuiltinUchar, "SKBuiltinUchar_t", sizeof(SKBuiltinUchar_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinUchar), g_tMetaSKBuiltinUchar },
	{ eTypeSKBuiltinInt8, "SKBuiltinInt8_t", sizeof(SKBuiltinInt8_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinInt8), g_tMetaSKBuiltinInt8 },
	{ eTypeSKBuiltinUint8, "SKBuiltinUint8_t", sizeof(SKBuiltinUint8_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinUint8), g_tMetaSKBuiltinUint8 },
	{ eTypeSKBuiltinInt16, "SKBuiltinInt16_t", sizeof(SKBuiltinInt16_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinInt16), g_tMetaSKBuiltinInt16 },
	{ eTypeSKBuiltinUint16, "SKBuiltinUint16_t", sizeof(SKBuiltinUint16_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinUint16), g_tMetaSKBuiltinUint16 },
	{ eTypeSKBuiltinInt64, "SKBuiltinInt64_t", sizeof(SKBuiltinInt64_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinInt64), g_tMetaSKBuiltinInt64 },
	{ eTypeSKBuiltinUint64, "SKBuiltinUint64_t", sizeof(SKBuiltinUint64_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinUint64), g_tMetaSKBuiltinUint64 },
	{ eTypeSKBuiltinFloat32, "SKBuiltinFloat32_t", sizeof(SKBuiltinFloat32_t),
		SK_ARRAY_SIZE(g_tMetaSKBuiltinFloat32), g_tMetaSKBuiltinFloat32 },
	{ eTypeSKBuiltinDouble64, "SKBuiltinDouble64_t", sizeof(SKBuiltinDouble64_t),
      SK_ARRAY_SIZE(g_tMetaSKBuiltinDouble64), g_tMetaSKBuiltinDouble64 },

    { eTypeSKBuiltinBuffer, "SKBuiltinBuffer_t", sizeof(SKBuiltinBuffer_t),
      SK_ARRAY_SIZE(g_tMetaSKBuiltinBuffer), g_tMetaSKBuiltinBuffer },
    { eTypeSKBuiltinString, "SKBuiltinString_t", sizeof(SKBuiltinString_t),
      SK_ARRAY_SIZE(g_tMetaSKBuiltinString), g_tMetaSKBuiltinString },

	{ eTypeSKBuiltinQSUint, "qs_uin_t", sizeof(qs_uin_t),
      SK_ARRAY_SIZE(g_tMetaQSUint), g_tMetaQSUint }
};

// ********** meta info **************** //

static const SKMetaInfo_t g_tSKMetaInfoBuiltinType = {
    "SK", "Builtintype", 10001,
    SK_ARRAY_SIZE(g_tSKMetaStructList), g_tSKMetaStructList,
    0, NULL
};

const SKMetaInfo_t * g_ptSKBuiltinMetaInfo = &g_tSKMetaInfoBuiltinType;
const static SKMetaInfoFinder g_objSKBuiltinMetaInfoFinder(g_ptSKBuiltinMetaInfo);

// ********* function impl ************ //

int SKBuiltinPickle :: ToBuffer( qs_uin_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinQSUint, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, qs_uin_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinQSUint, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( qs_uin_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinQSUint, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo);

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinQSUint, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, qs_uin_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinQSUint, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinQSUint, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int SKBuiltinPickle :: FreeStructField( qs_uin_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinQSUint, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( qs_uin_t * pDestStruct, const qs_uin_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinQSUint, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( qs_uin_t * ptStruct1, qs_uin_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinQSUint, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( qs_uin_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( qs_uin_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

    
int SKBuiltinPickle :: ToBuffer( SKBuiltinInt32_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinInt32, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt32_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinInt32, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinInt32_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinInt32, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinInt32, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt32_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinInt32, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinInt32, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int SKBuiltinPickle :: FreeStructField( SKBuiltinInt32_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinInt32, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinInt32_t * pDestStruct, const SKBuiltinInt32_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinInt32, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinInt32_t * ptStruct1, SKBuiltinInt32_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinInt32, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinInt32_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinInt32_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint32_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinUint32, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint32_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinUint32, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint32_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinUint32, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinUint32, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint32_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinUint32, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinUint32, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}



int SKBuiltinPickle :: FreeStructField( SKBuiltinUint32_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinUint32, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinUint32_t * pDestStruct, const SKBuiltinUint32_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinUint32, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinUint32_t * ptStruct1, SKBuiltinUint32_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinUint32, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinUint32_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinUint32_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinChar_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinChar, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinChar_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinChar, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinChar_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinChar, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinChar, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinChar_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinChar, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinChar, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int SKBuiltinPickle :: FreeStructField( SKBuiltinChar_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinChar, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinChar_t * pDestStruct, const SKBuiltinChar_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinChar, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinChar_t * ptStruct1, SKBuiltinChar_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinChar, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinChar_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinChar_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUchar_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinUchar, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUchar_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinUchar, poBuffer, ptStruct, sizeof( *ptStruct ) );
}


int SKBuiltinPickle :: ToBuffer( SKBuiltinUchar_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinUchar, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinUchar, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUchar_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinUchar, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinUchar, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int SKBuiltinPickle :: FreeStructField( SKBuiltinUchar_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinUchar, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinUchar_t * pDestStruct, const SKBuiltinUchar_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinUchar, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinUchar_t * ptStruct1, SKBuiltinUchar_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinUchar, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinUchar_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinUchar_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}


int SKBuiltinPickle :: ToBuffer( SKBuiltinInt8_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinInt8, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt8_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinInt8, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinInt8_t * ptStruct,SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinInt8, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinInt8, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt8_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinInt8, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinInt8, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int SKBuiltinPickle :: FreeStructField( SKBuiltinInt8_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinInt8, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinInt8_t * pDestStruct, const SKBuiltinInt8_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinInt8, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinInt8_t * ptStruct1, SKBuiltinInt8_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinInt8, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinInt8_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinInt8_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint8_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinUint8, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint8_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinUint8, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint8_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinUint8, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinUint8, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint8_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinUint8, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinUint8, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinUint8_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinUint8, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinUint8_t * pDestStruct, const SKBuiltinUint8_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinUint8, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinUint8_t * ptStruct1, SKBuiltinUint8_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinUint8, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinUint8_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinUint8_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinInt16_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinInt16, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt16_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinInt16, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinInt16_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinInt16, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinInt16, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt16_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinInt16, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinInt16, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinInt16_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinInt16, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinInt16_t * pDestStruct, const SKBuiltinInt16_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinInt16, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinInt16_t * ptStruct1, SKBuiltinInt16_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinInt16, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinInt16_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinInt16_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint16_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinUint16, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint16_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinUint16, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint16_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinUint16, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinUint16, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint16_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinUint16, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinUint16, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}


int SKBuiltinPickle :: FreeStructField( SKBuiltinUint16_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinUint16, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinUint16_t * pDestStruct, const SKBuiltinUint16_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinUint16, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinUint16_t * ptStruct1, SKBuiltinUint16_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinUint16, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinUint16_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinUint16_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinInt64_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinInt64, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt64_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinInt64, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinInt64_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinInt64, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinInt64, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinInt64_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinInt64, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinInt64, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinInt64_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinInt64, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinInt64_t * pDestStruct, const SKBuiltinInt64_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinInt64, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinInt64_t * ptStruct1, SKBuiltinInt64_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinInt64, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinInt64_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinInt64_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint64_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinUint64, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint64_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinUint64, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinUint64_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinUint64, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinUint64, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinUint64_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinUint64, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinUint64, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinUint64_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinUint64, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinUint64_t * pDestStruct, const SKBuiltinUint64_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinUint64, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinUint64_t * ptStruct1, SKBuiltinUint64_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinUint64, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinUint64_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinUint64_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinFloat32_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinFloat32, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinFloat32_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinFloat32, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinFloat32_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinFloat32, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinFloat32, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinFloat32_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinFloat32, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinFloat32, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}
int SKBuiltinPickle :: FreeStructField( SKBuiltinFloat32_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinFloat32, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinFloat32_t * pDestStruct, const SKBuiltinFloat32_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinFloat32, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinFloat32_t * ptStruct1, SKBuiltinFloat32_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinFloat32, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinFloat32_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinFloat32_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinDouble64_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinDouble64, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinDouble64_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinDouble64, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinDouble64_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinDouble64, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinDouble64, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinDouble64_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinDouble64, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinDouble64, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinDouble64_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinDouble64, &tStruct, sizeof( tStruct ) );
}


int SKBuiltinPickle :: DeepCopy( SKBuiltinDouble64_t * pDestStruct, const SKBuiltinDouble64_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinDouble64, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinDouble64_t * ptStruct1, SKBuiltinDouble64_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinDouble64, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinDouble64_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinDouble64_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}
    

////////

int SKBuiltinPickle :: ToBuffer( SKBuiltinBuffer_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinBuffer, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinBuffer_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinBuffer, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinBuffer_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinBuffer, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinBuffer, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinBuffer_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinBuffer, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinBuffer, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinBuffer_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinBuffer, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinBuffer_t * pDestStruct, const SKBuiltinBuffer_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinBuffer, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinBuffer_t * ptStruct1, SKBuiltinBuffer_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinBuffer, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinBuffer_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinBuffer_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinString_t * ptStruct, SKBuffer * poBuffer )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Struct2Buffer(
		eTypeSKBuiltinString, ptStruct, sizeof( *ptStruct ), poBuffer );
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinString_t * ptStruct )
{
	SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

	return tlvPickle.Buffer2Struct(
		eTypeSKBuiltinString, poBuffer, ptStruct, sizeof( *ptStruct ) );
}

int SKBuiltinPickle :: ToBuffer( SKBuiltinString_t * ptStruct, SKBuffer * poBuffer, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );
		pbPickle.SetStatus( iPickleStatus );

		return pbPickle.Struct2Buffer(
			eTypeSKBuiltinString, ptStruct,poBuffer );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Struct2Buffer(
			eTypeSKBuiltinString, ptStruct, sizeof( *ptStruct ), poBuffer );
	}

	return -1;
}

int SKBuiltinPickle :: FromBuffer( SKBuffer * poBuffer, SKBuiltinString_t * ptStruct, int iPickleType, int iPickleStatus )
{
	if( ePickleTypeProtoBuf == iPickleType )
	{
		SKPBPickle pbPickle( &g_objSKBuiltinMetaInfoFinder );

		return pbPickle.Buffer2Struct(
			eTypeSKBuiltinString, poBuffer, ptStruct );
	} else if( ePickleTypeTlv == iPickleType ) {
		SKTLVPickle tlvPickle( g_ptSKBuiltinMetaInfo );

		return tlvPickle.Buffer2Struct(
			eTypeSKBuiltinString, poBuffer, ptStruct, sizeof( *ptStruct ) );
	}

	return -1;
}

int SKBuiltinPickle :: FreeStructField( SKBuiltinString_t & tStruct )
{
	return SKMetaUtils::FreeField(
		g_ptSKBuiltinMetaInfo, eTypeSKBuiltinString, &tStruct, sizeof( tStruct ) );
}

int SKBuiltinPickle :: DeepCopy( SKBuiltinString_t * pDestStruct, const SKBuiltinString_t * pSourceStruct )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.DeepCopy(
		eTypeSKBuiltinString, pDestStruct, pSourceStruct, sizeof( *pDestStruct ) );
}

int SKBuiltinPickle :: Diff( SKBuiltinString_t * ptStruct1, SKBuiltinString_t * ptStruct2 )
{
	SKAllocator Allocator( g_ptSKBuiltinMetaInfo );

	return Allocator.Diff(
		eTypeSKBuiltinString, ptStruct1, ptStruct2, sizeof( *ptStruct1 ) );
}

int SKBuiltinPickle :: IsMatch( SKBuiltinString_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() == poBuffer->GetLen() )
	{
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}

int SKBuiltinPickle :: IsNewStruct( SKBuiltinString_t * ptStruct, Comm::SKBuffer * poBuffer )
{
	int ret = 0;
	Comm::SKBuffer tmp;
	SKBuiltinPickle::ToBuffer( ptStruct, &tmp );
	if( tmp.GetLen() > poBuffer->GetLen() )
	{
		ret = 1;
	} else if( tmp.GetLen() == poBuffer->GetLen() ) {
		if( 0 == memcmp( tmp.GetBuffer(), poBuffer->GetBuffer(), tmp.GetLen() ) )
		{
			ret = 1;
		}
	}
	return ret;
}


}

