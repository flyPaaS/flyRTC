#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tlvport.h"

#include "skmetainfo.h"
#include "skmetautils.h"
#include "skbuffer.h"

namespace Comm
{

void SKMetaUtils :: Dump( const SKMetaInfo_t * ptMetaInfo )
{
	char tmp[ 128 ] = { 0 };

	printf( "prefix %s, name %s, magic %d\n",
			ptMetaInfo->pcPrefix, ptMetaInfo->pcName, ptMetaInfo->iMagic );
	printf( "\n" );

	printf( "struct.count %d\n", ptMetaInfo->hStructCount );
	for( int i = 0; i < ptMetaInfo->hStructCount; i++ )
	{
		printf( "\n" );
		const SKMetaStruct_t * st = &(ptMetaInfo->ptStructList[i]);
		printf( "struct %s, id %d, size %d, field.count %d\n",
				st->pcName, st->hId, st->iSize, st->hFieldCount );

		for( int j = 0; j < st->hFieldCount; j++ )
		{
			const SKMetaField_t * field = &(st->ptFieldList[j]);

			snprintf( tmp, sizeof( tmp ), "%s,", field->pcName );

			printf( "field %-20s id %d, offset %d, size %d, type %d, isptr %d, "
					"array.size %d, referto [%s]\n",
					tmp, field->hId, field->hOffset, field->iItemSize,
					field->hType, field->cIsPtr, field->hArraySize, field->pcReferTo );
		}
	}

	printf( "\n" );

	printf( "func.count %d\n", ptMetaInfo->hFuncCount );
	for( int i = 0; i < ptMetaInfo->hFuncCount; i++ )
	{
		printf( "\n" );

		const SKMetaFunc_t * func = &(ptMetaInfo->ptFuncList[i]);

		snprintf( tmp, sizeof( tmp ), "%s,", func->pcName );

		printf( "func %-20s cmdid %d, req.type %d, resp.type %d\n",
				tmp, func->hCmdid, func->tReq.hType, func->tResp.hType );
	}

	printf( "\n" );
}

void SKMetaUtils :: Dump2Xml( const SKMetaInfo_t * ptMetaInfo, SKBuffer * poResult )
{
	poResult->Printf( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n" );

	poResult->Printf( "<metainfo prefix=\"%s\" filename=\"%s\" magic=\"%d\">\n",
			ptMetaInfo->pcPrefix, ptMetaInfo->pcName, ptMetaInfo->iMagic );

	for( int i = 0; i < ptMetaInfo->hStructCount; i++ )
	{
		poResult->Printf( "\n" );

		const SKMetaStruct_t * st = &(ptMetaInfo->ptStructList[i]);

		poResult->Printf( "\t<struct name=\"%s\" id=\"%d\">\n", st->pcName, st->hId );

		for( int j = 0; j < st->hFieldCount; j++ )
		{
			const SKMetaField_t * field = &(st->ptFieldList[j]);

			poResult->Printf( "\t\t<field name=\"%s\" typeid=\"%d\" id=\"%d\" ",
					field->pcName, field->hType, field->hId );
			
			if( NULL != field->pcReferTo && '\0' != field->pcReferTo[0] )
			{
				poResult->Printf( " referto=\"%s\" ", field->pcReferTo );
			}

			if( field->hArraySize > 0 )
			{
				poResult->Printf( " arraysize=\"%d\" ", field->hArraySize );
			}

			poResult->Printf( "/>\n" );
		}

		poResult->Printf( "\t</struct>\n" );
	}

	for( int i = 0; i < ptMetaInfo->hFuncCount; i++ )
	{
		poResult->Printf( "\n" );

		const SKMetaFunc_t * func = &(ptMetaInfo->ptFuncList[i]);

		poResult->Printf( "\t<function name=\"%s\" cmdid=\"%d\">\n",
				func->pcName, func->hCmdid );

		poResult->Printf( "\t\t<request  typeid=\"%d\" />\n", func->tReq.hType );
		poResult->Printf( "\t\t<response typeid=\"%d\" />\n", func->tResp.hType );

		poResult->Printf( "\t</function>\n" );
	}

	poResult->Printf( "\n" );

	poResult->Printf( "</metainfo>\n" );
}

const SKMetaFunc_t * SKMetaUtils :: FindFunc( const SKMetaInfo_t *ptMetaInfo, const int iCmdId )
{
	const SKMetaFunc_t *ptFunc = NULL ;

	for ( int i=0; i<ptMetaInfo->hFuncCount; ++i )
	{
		if ( iCmdId == ptMetaInfo->ptFuncList[i].hCmdid )
		{
			ptFunc = &(ptMetaInfo->ptFuncList[i]);
			break;
		}
	}
	return ptFunc ;
}

const SKMetaFunc_t * SKMetaUtils :: FindFunc( const SKMetaInfo_t *ptMetaInfo, const char * sFuncName )
{
	const SKMetaFunc_t *ptFunc = NULL ;

	for ( int i=0; i<ptMetaInfo->hFuncCount; ++i )
	{
		if ( 0 == strcasecmp( sFuncName, ptMetaInfo->ptFuncList[i].pcName ) )
		{
			ptFunc = &(ptMetaInfo->ptFuncList[i]);
			break;
		}
	}
	return ptFunc ;
}

const SKMetaStruct_t * SKMetaUtils :: FindStruct( const SKMetaInfo_t *ptMetaInfo, const int iTypeId )
{
	const SKMetaStruct_t *ptStruct = NULL ;
	for ( int i=0; i<ptMetaInfo->hStructCount; ++i )
	{
		if ( iTypeId == ptMetaInfo->ptStructList[i].hId )
		{
			ptStruct = &(ptMetaInfo->ptStructList[i]);
			break;
		}
	}
	return ptStruct ;
}

const SKMetaStruct_t * SKMetaUtils :: FindStruct( const SKMetaInfo_t *ptMetaInfo, const char *sStructName )
{
	const SKMetaStruct_t *ptStruct = NULL ;
	for ( int i=0; i<ptMetaInfo->hStructCount; ++i )
	{
		if ( 0 == strcasecmp ( sStructName, ptMetaInfo->ptStructList[i].pcName ) )
		{
			ptStruct = &(ptMetaInfo->ptStructList[i]);
			break;
		}
	}
	return ptStruct ;
}

int SKMetaUtils :: GetReferCount( const void * pvStruct,
		const SKMetaStruct_t * ptMetaStruct, const SKMetaField_t * ptField )
{
	int iValue = -1;
	char *pcBase = (char *)pvStruct ;

	if ( 0 == strcasecmp(ptField->pcReferTo, "strlen") )
	{
		/* append '\0' byte */
		if ( ptField->cIsPtr )
		{
			char * pcStr = *(char **)(pcBase + ptField->hOffset);
			iValue = strlen ( pcStr ? pcStr : "" ) + 1;
		}
		else
		{
			iValue = strlen ( (char *)(pcBase + ptField->hOffset) ) + 1;
		}
		return iValue;
	}

	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		if ( 0 == strcasecmp(ptField->pcReferTo, ptMetaStruct->ptFieldList[i].pcName) )
		{
			int iOffset = ptMetaStruct->ptFieldList[i].hOffset;

			/* only allowed char/uchar, int8/uint8, int32/uint32 */
			switch ( ptMetaStruct->ptFieldList[i].hType )
			{
				case eTypeSKChar:
		    	case eTypeSKInt8:
					iValue = *((char *)(pcBase + iOffset));
					break ;
				case eTypeSKUchar:
		    	case eTypeSKUint8:
					iValue = *((unsigned char *)(pcBase + iOffset));
					break ;
				case eTypeSKInt16:
					{
						short val=0;
						memcpy(&val, pcBase + iOffset, sizeof(short));
						iValue = val;
					}
					break;
				case eTypeSKUint16:
					{
						unsigned short val=0;
						memcpy(&val, pcBase + iOffset, sizeof(unsigned short));
						iValue = val;
					}
					break ;
				case eTypeSKInt32:
					memcpy(&iValue, pcBase + iOffset, sizeof(int));
					//iValue = *((int *)(pcBase + iOffset));
					break ;
				case eTypeSKUint32:
					memcpy(&iValue, pcBase + iOffset, sizeof(unsigned int));
					//iValue = *((unsigned int *)(pcBase + iOffset));
					break ;
				default:
					iValue=-1;
					break;
			}
			break;
		}
	}

	return iValue;
}

int SKMetaUtils :: GetFieldSize (
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	const void *pvStruct,
	const int iStructSize
)
{
	int iSize = -1 ; 
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]) ;

	//DumpField(ptMetaField);

	if ( ptMetaField->cIsPtr )
	{
		iSize = SKMetaUtils::GetReferCount ( pvStruct, (SKMetaStruct_t *)ptMetaStruct, ptMetaField ) * ptMetaField->iItemSize;
	}
	else if ( 0 < ptMetaField->hArraySize )
	{
		if ( '\0' != *(ptMetaField->pcReferTo) )
		{
			/* NOTE: 'arraysize' and 'referto' should not be used together */
			iSize = -1;
		}
		else
		{
			iSize = ptMetaField->hArraySize * ptMetaField->iItemSize ;
		}
	}
	else
	{
		iSize = ptMetaField->iItemSize;
	}

	return iSize;
}

int SKMetaUtils :: GetItemCnt (
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	const void *pvStruct,
	const int iStructSize
)
{
	int iCount = -1 ; 
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]) ;

	//DumpField(ptMetaField);

	if ( ptMetaField->cIsPtr )
	{
		iCount = SKMetaUtils::GetReferCount ( pvStruct, (SKMetaStruct_t *)ptMetaStruct, ptMetaField );
	}
	else if ( 0 < ptMetaField->hArraySize )
	{
		if ( '\0' != *(ptMetaField->pcReferTo) )
		{
			iCount = SKMetaUtils::GetReferCount ( pvStruct, (SKMetaStruct_t *)ptMetaStruct, ptMetaField );
		}
		else
		{
			iCount = ptMetaField->hArraySize;
		}
	}
	else
	{
		/* default count 1 */
		iCount = 1; 
	}

	return iCount;
}

int SKMetaUtils :: FreeField(
	const SKMetaInfo_t *ptMetaInfo,
	const int iType,
	const void *pvStruct,
	const int iSize
)
{
	int ret = 0;

	if( NULL == pvStruct ) return -1;

	char * base = (char*)pvStruct;

	const SKMetaStruct_t * metaStruct = SKMetaUtils::FindStruct( ptMetaInfo, iType );

	if( NULL == metaStruct ) return -1;

	if( (int)(metaStruct->iSize) != iSize ) return -1;

	for( int i = 0; i < metaStruct->hFieldCount && 0 == ret; i++ ) {
		const SKMetaField_t * field = metaStruct->ptFieldList + i;

		if( field->hType > eTypeSKBuiltinType/*eTypeSKUserDefine*/ ) {
			if( field->cIsPtr ) {
				int referCount = SKMetaUtils::GetReferCount( pvStruct, metaStruct, field );
				const SKMetaStruct_t * referStruct = SKMetaUtils::FindStruct( ptMetaInfo, field->hType );
				char * referBase = (char*)*(void**)(base + field->hOffset);

				for( int j = 0; j < referCount && 0 == ret; j++ ) {
					ret = FreeField( ptMetaInfo, field->hType, referBase + ( j * referStruct->iSize ), field->iItemSize );
				}

				if( NULL != referBase ) ::free( referBase );
			} else {
				ret = FreeField( ptMetaInfo, field->hType, base + field->hOffset, field->iItemSize );
			}
		} else {
			if( field->cIsPtr ) {
				if( field->hArraySize <= 0 ) {
					void * ptr = *(void**)( base + field->hOffset );
					if( NULL != ptr ) ::free( ptr );
				}
			}
		}
	}

	memset( base, 0, metaStruct->iSize );

	return ret;
}

void SKMetaUtils :: DumpField(const SKMetaField_t *ptMetaField)
{
	TLV_LOG_ERR( "FIELD: fname '%s' offset %i type %i isptr %i arraysize %i referto '%s' id %i fsize %i itemsize %i",
		ptMetaField->pcName, ptMetaField->hOffset, ptMetaField->hType, ptMetaField->cIsPtr, ptMetaField->hArraySize,
		ptMetaField->pcReferTo, ptMetaField->hId, ptMetaField->iFieldSize, ptMetaField->iItemSize );
}

void SKMetaUtils :: DumpStruct(const SKMetaStruct_t *ptMetaStruct)
{
	TLV_LOG_ERR( "STRUCT: id %i name '%s' size %i fieldcnt %i",
		ptMetaStruct->hId, ptMetaStruct->pcName, ptMetaStruct->iSize, ptMetaStruct->hFieldCount );
	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		TLV_LOG_ERR( "STRUCT: field %i value ------->", i ) ;
		DumpField ( &(ptMetaStruct->ptFieldList[i]) );
	}
}

}

