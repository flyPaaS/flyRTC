#include <stdlib.h>

#include "skmetainfo.h"
#include "skmetautils.h"
#include "skmetainfofinder.h"

#include "skpbpickle.h"

#include "skbuffer.h"

#include "skpbhelper.h"
#include "skpbencoder.h"
#include "skpbdecoder.h"

#include "tlvport.h"

#include "iHash.h"

#include <algorithm>
#include <iostream>

namespace Comm
{

#define MODULE "ProtoBuf"

typedef struct tagSKPBPickleImpl {

	const SKMetaInfoFinder * poFinder;

	CHashTable< int, int > KeyHash;

	unsigned char cStatus;

	static unsigned int HFunc(const int &key)
	{
		return (unsigned int )key;
	}

	tagSKPBPickleImpl():KeyHash(tagSKPBPickleImpl :: HFunc,1){}

} SKPBPickleImpl_t;

SKPBPickle :: SKPBPickle( const SKMetaInfoFinder * poFinder )
{
	m_ptImpl = new SKPBPickleImpl_t;
	m_ptImpl -> poFinder = poFinder;
	m_ptImpl -> cStatus = 0;
}

SKPBPickle :: ~SKPBPickle()
{
	delete m_ptImpl;
	m_ptImpl = NULL;
}

void SKPBPickle :: SetStatus( unsigned char cStatus )
{
	m_ptImpl->cStatus = cStatus;
	//TLV_LOG_DEBUG(" %s::Debug setstatus(%d)", MODULE, cStatus );
}

// public ================

int SKPBPickle :: Struct2Buffer(
	const int iTypeId,
	const void *pvStruct,
	SKBuffer *poSKBuffer
)
{
	int iRet = 0;

	int iLen = 0,iLev = 1;

	iRet = m_ptImpl->KeyHash.Init();
	if( iRet != 0 )
	{
		TLV_LOG_ERR ( "%s::ERROR: KeyHash table init err ret(%d)", MODULE, iRet);
		return iRet;
	}

	iRet = CountLen(iTypeId,pvStruct,iLev,iLen);
	
	SKPBEncoder poBuffer(iLen);

	iLev = 1;	

	if( 0 == iRet )
	{
		iRet = PackBuffer( iTypeId, pvStruct,  1, &poBuffer, iLev );
		if ( 0 == iRet )
		{
			char *pcBuffer = NULL;
			int iUsedSize=0;
			iRet = poBuffer.Detach(&pcBuffer, &iUsedSize);
			if ( 0 == iRet )
			{
				iRet = poSKBuffer->Write(pcBuffer, iUsedSize);
				if ( iRet == iUsedSize )
				{
					/* final ok */
					iRet = 0 ;
				}
			}
			//printf(" countlen(%d) reallen(%d)\n", iLen, iUsedSize );
			
			if( iLen != iUsedSize )
			{
				iRet = -1;
				TLV_LOG_ERR("%s::ERROR countlen(%d) readlen(%d)", MODULE, iLen, iUsedSize );
			}
			if( pcBuffer )
			{
				free( pcBuffer );
				pcBuffer = NULL;
			}
		}
	}

	if( iRet )
		TLV_LOG_ERR ( "%s::ERROR: struct2buffer ret %d len %i ", MODULE, iRet, poSKBuffer->GetLen() );

	return iRet ;
}

int SKPBPickle :: Buffer2Struct(
	const int iTypeId,
	SKBuffer *poSKBuffer,
	void *pvStruct
)
{
	int iRet = 0;

	SKPBDecoder poBuffer;

	iRet = poBuffer.Attach((char *)poSKBuffer->GetBuffer(), poSKBuffer->GetLen());

	if ( 0 != iRet )
	{
		iRet = -1;
		TLV_LOG_ERR ( "%s::ERROR: SvrKit tlv buffer2struct ret %d len %i ", MODULE, iRet, poSKBuffer->GetLen() );
	}
	else
	{

		const SKMetaInfoFinder :: SKStructInfo_t * ptMetaStructInfo = m_ptImpl -> poFinder -> GetStructInfo( iTypeId );

		const SKMetaStruct_t * ptMetaStruct = ptMetaStructInfo->ptStruct;
		
		memset( (char *)pvStruct, 0, ptMetaStruct->iSize );

		iRet = UnpackBuffer( iTypeId, &poBuffer, pvStruct );
		if ( 0 != iRet )
		{
			/* failed */
		}
	}

	poBuffer.Detach();

	return iRet ;
}


// private ================
int SKPBPickle :: CountLen(
	const int iTypeId,
	const void *pvStruct,
	int &iLev,
	int &iLen
)
{
	int iRet = 0;

	int iCurLev = iLev++;

	iLen = 0;

	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;

	const SKMetaInfoFinder :: SKStructInfo_t * ptMetaStructInfo = m_ptImpl -> poFinder -> GetStructInfo( iTypeId );

	ptMetaStruct = ptMetaStructInfo->ptStruct;

	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "%s::ERROR: SvrKit PackBuffer FindStruct null id %i", MODULE, iTypeId );
		return -1;
	}

	for ( int index=0; index<ptMetaStruct->hFieldCount; ++index )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[index]) ;
		if( m_ptImpl -> cStatus && !( ptMetaField -> cStatus & 0x1 ) )
		{
			TLV_LOG_ERR("%s::Debug field(%s) skip due to status",MODULE, ptMetaField -> pcName );
			continue;
		}

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType < ptMetaField->hType )
		{
			/* user defined type or built-in type, recursive */
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, index, pvStruct, ptMetaStruct->iSize );
			char * pcPos = NULL;
			
			if( ptMetaField->cIsPtr )
			{
				pcPos = (char*)*(void**)(pcBase + ptMetaField->hOffset);
			} else {
				pcPos = pcBase + ptMetaField->hOffset;
			}

			int iItemlen = 0;
			for ( int itemIndex = 0; itemIndex < iItemCnt; ++itemIndex )
			{
				int iLen = 0;
				iRet = CountLen( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex),
					iLev, iLen);
				if( 0 != iRet )
				{
					break;
				}
					
				iItemlen += iLen;
				iItemlen += SKPBHelper :: GetTagAndLenSize( ptMetaField->hId, iLen );
			}
			iLen += iItemlen;
		}
		else
		{
			
			iLen += GetBaseTypeSize ( ptMetaStruct, index, pvStruct ) ;
		}

		if( iRet )
			break;
	}

	iRet = m_ptImpl->KeyHash.Insert(iCurLev,iLen);
	if( iRet < 0 )
	{
		TLV_LOG_ERR ( "%s::ERROR: SvrKit PackBuffer KeyHash insert error", MODULE );
	}
	
	return iRet ;
}

int SKPBPickle :: PackBuffer(
	const int iTypeId,
	const void *pvStruct,
	const int hId,
	SKPBEncoder *poBuffer,
	int &iLev
)
{
	int iCurLev = iLev++;	
	int iRet = 0 ;
	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;
	
	const SKMetaInfoFinder :: SKStructInfo_t * ptMetaStructInfo = m_ptImpl -> poFinder -> GetStructInfo( iTypeId );

	ptMetaStruct = ptMetaStructInfo->ptStruct;


	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "%s::ERROR: SvrKit PackBuffer FindStruct null id %i", MODULE, iTypeId );
		return -1;
	}

	if( iCurLev != 1 )
	{
		int iLen = 0;
		iRet = m_ptImpl -> KeyHash.Find(iCurLev,iLen);
		if( iRet )
		{
			iRet = CountLen( iTypeId, pvStruct, iCurLev, iLen);
		}
		if( iRet )
		{
			TLV_LOG_ERR( "%s::ERROR countlen error", MODULE );
			return -1;
		}
		poBuffer->AddTagAndLen( hId, iLen );//add struct len header except the root struct
	}

	for ( int index=0; index<ptMetaStruct->hFieldCount; ++index )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[ index ]) ;
			
		if( m_ptImpl -> cStatus && !(ptMetaField -> cStatus & 0x1) )
		{
			TLV_LOG_DEBUG("%s::Debug field(%s) skip due to status",MODULE, ptMetaField -> pcName );
			continue;
		}

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType < ptMetaField->hType )
		{
			/* user defined type or built-in type, recursive */
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, index, pvStruct, ptMetaStruct->iSize );
			if( iItemCnt < 0 )
			{
				iRet = -1;
				TLV_LOG_ERR("%s::ERROR field hasn't cnt name(%s)", MODULE, ptMetaField -> pcName );
				break;
			}

			char * pcPos = NULL;
			
			if( ptMetaField->cIsPtr )
			{
				pcPos = (char*)*(void**)(pcBase + ptMetaField->hOffset);
			} else {
				pcPos = pcBase + ptMetaField->hOffset;
			}
			
			for ( int itemIndex = 0; itemIndex < iItemCnt; ++itemIndex  )
			{
				iRet = PackBuffer( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex),
					ptMetaField->hId, poBuffer,iLev );

				if ( 0 != iRet )
				{
					break;
				}
			}
		}
		else if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType >= ptMetaField->hType )
		{
			/* base type */
			iRet = PackBaseType ( ptMetaStruct, index, pvStruct, poBuffer ) ;
			if ( 0 != iRet )
			{
				TLV_LOG_ERR ( "%s::ERROR: SvrKit PackBuffer basetype failed %i", MODULE,index );
				break;	
			}
		}
	}
	
	return iRet ;
}


void SKPBPickle :: ResetPointor(
	const int iTypeId,
	void *pvStruct
	)
{
	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;
	
	const SKMetaInfoFinder :: SKStructInfo_t * ptMetaStructInfo = m_ptImpl -> poFinder -> GetStructInfo( iTypeId );

	ptMetaStruct = ptMetaStructInfo->ptStruct;
	
	for ( int index=0; index<ptMetaStruct->hFieldCount; ++index )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[ index ]) ;
			
		if( m_ptImpl -> cStatus && !(ptMetaField -> cStatus & 0x1) )
		{
			TLV_LOG_DEBUG("%s::Debug field(%s) skip due to status",MODULE, ptMetaField -> pcName );
			continue;
		}

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType < ptMetaField->hType )
		{
			/* user defined type or built-in type, recursive */
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, index, pvStruct, ptMetaStruct->iSize );
			if( iItemCnt < 0 )
			{
				TLV_LOG_ERR("%s::ERROR field hasn't cnt name(%s)", MODULE, ptMetaField -> pcName );
				continue;
			}

			char * pcPos = NULL;
			
			if( ptMetaField->cIsPtr )
			{
				pcPos = NULL;
				continue;
			} else {
				pcPos = pcBase + ptMetaField->hOffset;
			}
			
			for ( int itemIndex = 0; itemIndex < iItemCnt; ++itemIndex  )
			{
				ResetPointor( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex) );
			}
		}
		else if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType >= ptMetaField->hType )
		{
			if( ptMetaField->cIsPtr )
			{
				char *pcValue = (char *)calloc( sizeof(char) , 1 );
				if( pcValue == NULL )
				{
					TLV_LOG_ERR( "%s(%d)::ERROR MEM ERROR",MODULE,__LINE__);
					continue;
				}
				*(void **)(pcBase+ptMetaField->hOffset) = pcValue;
			}
		}
	}
}

int SKPBPickle :: UnpackBuffer(
	const int iTypeId,
	SKPBDecoder *poBuffer,
	void *pvStruct
)
{
	if( NULL == pvStruct )
	{
		TLV_LOG_ERR( "%s :: ERROR struct null",MODULE);
		return -1;
	}

	int iRet = 0 ;
	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;

	const SKMetaInfoFinder :: SKStructInfo_t * ptMetaStructInfo = m_ptImpl -> poFinder -> GetStructInfo( iTypeId );
	
	ptMetaStruct = ptMetaStructInfo->ptStruct;

	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "%s::ERROR: SvrKit UnpackBuffer FindStruct null id %i", MODULE, iTypeId );
		return -1;
	}


	int * iFieldCnt = (int *)calloc( sizeof( int ) * ptMetaStruct -> hFieldCount,1 );
	if( iFieldCnt == NULL )
	{
		TLV_LOG_ERR( "%s(%d)::ERROR MEM ERROR",MODULE,__LINE__);
		return -1;
	}
	while( 1 )
	{
		int iObjectTag = poBuffer -> GetTag();	

		if( iObjectTag < 0 )// empty buff
			break;

		int index = ptMetaStructInfo -> FindFieldIndex( iObjectTag );
		
		if( index != -1 )
		{
			ptMetaField = &(ptMetaStruct->ptFieldList[ index ]) ;

			if ( eTypeSKBuiltinType >= ptMetaField->hType )
			{
				int iSize = 0;
				iRet = UnpackBaseType ( ptMetaStruct, index, poBuffer, pvStruct, iSize );
				if( iRet == 0 )
				{
					if( iFieldCnt[index] )
					{
						TLV_LOG_DEBUG("%s basefiled %s repeat",__FUNCTION__, ptMetaField->pcName );
					}
					iFieldCnt[index] = iSize/ptMetaField->iItemSize ? iSize/ptMetaField->iItemSize : -1;
				}

				if ( 0 != iRet )
				{
					if ( 0 == ptMetaField->cIsRequired )
					{
						iRet = 0;
					} else {
						TLV_LOG_ERR ( "%s::ERROR: SvrKit UnpackBaseType failed %i %i", MODULE, iRet, index );
						break;
					}
				}
			}
		}
		if( poBuffer->GetNextObject() )break;
	}

	if( iRet == 0 )
	{
		poBuffer->Reset();

		while( 1 )
		{
			int iObjectTag = poBuffer -> GetTag();	
			if( iObjectTag < 0 )
				break;

			int index = ptMetaStructInfo -> FindFieldIndex( iObjectTag );

			if( index != -1 )
			{
				ptMetaField = &(ptMetaStruct->ptFieldList[ index ]) ;
				if ( eTypeSKBuiltinType < ptMetaField->hType )
				{

					char * pcValue = NULL;

					int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, index, pvStruct, ptMetaStruct->iSize );

					pcValue = pcBase+ptMetaField->hOffset; 

					if( !iFieldCnt[index] )
					{
						if ( ptMetaField->cIsPtr )
						{
							char * tmp = pcValue;
							pcValue = (char *)calloc(ptMetaField->iItemSize, iItemCnt);
							if( pcValue == NULL )
							{
								TLV_LOG_ERR( "%s(%d)::ERROR MEM ERROR",MODULE,__LINE__);
								iRet = -1;
								break;
							}
							*(void**)( tmp ) = pcValue;
						}
					}
					else
					{
						if ( ptMetaField->cIsPtr )
						{
							pcValue = (char *)(( *( ( char ** )pcValue)  ) + ptMetaField -> iItemSize * iFieldCnt[index]);
						}
						else
						{
							pcValue = pcValue + ptMetaField -> iItemSize * iFieldCnt[index];
						}
					}


					if( iItemCnt <= iFieldCnt[index] ) 
					{
						iRet = -1;
						LogErr("%s field [%s] cnt not match too many referto (%d) real get (%d)",
								__FUNCTION__, ptMetaField->pcName, iItemCnt, iFieldCnt[index] );
						break;
					}

					SKPBDecoder poSubBuffer;

					iRet = poBuffer->GetSubObject( &poSubBuffer );

					if ( SKPB_EMPTYSTRUCT == iRet )
					{
						iRet = UnpackBuffer( ptMetaField->hType, &poSubBuffer, (void *)(pcValue ) );
						iRet = 0;
						++iFieldCnt[index];
					}
					else if( iRet == 0 )
					{
						iRet = UnpackBuffer( ptMetaField->hType, &poSubBuffer,
								(void *)(pcValue ) );
						++iFieldCnt[index];
					}
					poSubBuffer.Detach( );
				}
			}
			if( poBuffer->GetNextObject() )break;
		}

	}

	//check basetype referto
	for( int i = 0; iRet == 0 && i < ptMetaStruct -> hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[ i ]) ;

		if ( ptMetaField->cIsRequired && !iFieldCnt[i] )
		{
			iRet = -1;
			TLV_LOG_ERR("file error name(%s) require(%d)",ptMetaField->pcName, ptMetaField->cIsRequired );
			break;
		}
		if( iFieldCnt[i] == 0  )
		{
			if( ptMetaField->cIsPtr && eTypeSKBuiltinType >= ptMetaField->hType )
			{
				char *pcValue = (char *)calloc( sizeof(char) , 1 );
				if( pcValue == NULL )
				{
					TLV_LOG_ERR( "%s(%d)::ERROR MEM ERROR",MODULE,__LINE__);
					iRet = -1;
					break;
				}
				*(void **)(pcBase+ptMetaField->hOffset) = pcValue;
			}
			else if( eTypeSKBuiltinType < ptMetaField->hType )
			{
				/* reset each substruct */
				
				int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, i, pvStruct, ptMetaStruct->iSize );
				if( iItemCnt < 0 )
				{
					TLV_LOG_ERR("%s::ERROR field hasn't cnt name(%s)", MODULE, ptMetaField -> pcName );
					continue;
				}

				char * pcPos = NULL;
				if( ptMetaField->cIsPtr )
				{
					continue;
				} 
				else 
				{
					pcPos = pcBase + ptMetaField->hOffset;
				}

				for ( int itemIndex = 0; itemIndex < iItemCnt; ++itemIndex  )
				{
					ResetPointor( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex) );
				}
			}
		}
	}


	//check item cnt
	for( int i = 0; iRet == 0 && i < ptMetaStruct -> hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[ i ]) ;
		if ( 0 != strlen(ptMetaField->pcReferTo) && strcasecmp( ptMetaField->pcReferTo, "strlen" ) != 0 )
		{
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, i, pvStruct, ptMetaStruct->iSize );
			/*
			if( iFieldCnt[i] == 0 && ptMetaField -> cIsRequired == 0 )
			{
				continue;
			}
			*/

			//filedcnt = -1 means empty but has
			if( !( iItemCnt == 0 && iFieldCnt[i] == -1 ) && iItemCnt != iFieldCnt[i] )
			{
				iRet = -1;
				LogErr("%s field %s cnt not match referto (%d) real get (%d)",
						__FUNCTION__, ptMetaField->pcName, iItemCnt, iFieldCnt[i] );
				break;
			}
		}
	}

	free( iFieldCnt );
	return iRet ;
}

int SKPBPickle :: GetBaseTypeSize( 
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	const void *pvStruct
)
{
	int iRet = 0 ;
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]);
	char *pcBase = (char *)pvStruct;

	char *pcValue = NULL;

	if ( ptMetaField->cIsPtr )
	{
		pcValue = *(char **)(pcBase+ptMetaField->hOffset);
		if( NULL == pcValue ) pcValue = "";
	}
	else
	{
		pcValue = pcBase+ptMetaField->hOffset;
	}

	int iSize = 0;

	if ( 0 == strcasecmp(ptMetaField->pcReferTo, "strlen") )
	{
		if ( ptMetaField->cIsPtr )
		{
			char * pcStr = *(char **)(pcBase + ptMetaField->hOffset);
			iSize = strlen ( pcStr ? pcStr : "" );
		}
		else
		{
			iSize = strlen ( (char *)(pcBase + ptMetaField->hOffset) );
		}
	}
	else
	{
		iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct, ptMetaStruct->iSize ); 
	}

	if( ptMetaField->cIsPtr || ptMetaField->hArraySize > 0 )
	{
		iRet = GetBaseTypePtrSize( ptMetaField, pcValue, iSize );
	} 
	else {
		iRet = GetBaseTypeValSize( ptMetaField, pcValue );
	}

	return iRet;
}

int SKPBPickle :: PackBaseType( 
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	const void *pvStruct,
	SKPBEncoder *poBuffer
)
{
	int iRet = 0 ;
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]);
	char *pcBase = (char *)pvStruct;

	int iSize = 0;

	char *pcValue = NULL;

	if ( ptMetaField->cIsPtr )
	{
		pcValue = *(char **)(pcBase+ptMetaField->hOffset);
		if( NULL == pcValue ) pcValue = "";
	}
	else
	{
		pcValue = pcBase+ptMetaField->hOffset;
	}

	if ( 0 == strcasecmp(ptMetaField->pcReferTo, "strlen") )
	{
		/* append '\0' byte */
		if ( ptMetaField->cIsPtr )
		{
			char * pcStr = *(char **)(pcBase + ptMetaField->hOffset);
			iSize = strlen ( pcStr ? pcStr : "" );
		}
		else
		{
			iSize = strlen ( (char *)(pcBase + ptMetaField->hOffset) );
		}
	}
	else
	{
		iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct,ptMetaStruct->iSize ); 
	}

	if( ptMetaField->cIsPtr || ptMetaField->hArraySize > 0 )
	{
		iRet = PackBaseTypePtr( ptMetaField, pcValue, iSize, poBuffer );
	} 
	else {
		iRet = PackBaseTypeVal( ptMetaField, pcValue, poBuffer );
	}

	return iRet;
}

int SKPBPickle :: GetBaseTypePtrSize (
	const SKMetaField_t * ptMetaField,
	const char * pcPtr,
	int iSize
)
{
	int len = 0;
	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(char *)pcPtr, iSize/sizeof(char), ptMetaField->cIsRequired );
			break;
		case eTypeSKInt16:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(short *)pcPtr, iSize/sizeof(short), ptMetaField->cIsRequired );
			break;
		case eTypeSKInt32:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(int *)pcPtr, iSize/sizeof(int), ptMetaField->cIsRequired );
			break;
		case eTypeSKUchar:
		case eTypeSKUint8:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(unsigned char *)pcPtr, iSize/sizeof(unsigned char), ptMetaField->cIsRequired );
			break;
		case eTypeSKUint16:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(unsigned short *)pcPtr, iSize/sizeof(unsigned short), ptMetaField->cIsRequired );
			break;
		case eTypeSKUint32:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(unsigned int *)pcPtr, iSize/sizeof(unsigned int), ptMetaField->cIsRequired );
			break;
		case eTypeSKFloat32:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(float *)pcPtr, iSize/sizeof(float), ptMetaField->cIsRequired );
			break;
		case eTypeSKInt64:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(long long *)pcPtr, iSize/sizeof(long long), ptMetaField->cIsRequired );
			break;
		case eTypeSKUint64:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(unsigned long long *)pcPtr, iSize/sizeof(unsigned long long), ptMetaField->cIsRequired );
			break;
		case eTypeSKDouble64:
			len = SKPBHelper :: GetArraySize( ptMetaField->hId,
					(double *)pcPtr, iSize/sizeof(double), ptMetaField->cIsRequired );
			break;
		case eTypeSKSInt32:
			len = SKPBHelper :: GetSInt32ArraySize( ptMetaField->hId,
					(int *)pcPtr, iSize/sizeof(int), ptMetaField->cIsRequired );
			break;
		case eTypeSKSInt64:
			len = SKPBHelper :: GetSInt64ArraySize( ptMetaField->hId,
					(long long*)pcPtr, iSize/sizeof(long long), ptMetaField->cIsRequired );
			break;
		default:
			TLV_LOG_ERR("%s::ERROR unknown type(%d)", MODULE, ptMetaField->hType );
			len = -1;
			break;
	}
	return len;
}

int SKPBPickle :: PackBaseTypePtr (
	const SKMetaField_t * ptMetaField,
	const char * pcPtr,
	int iSize,
	SKPBEncoder *poBuffer
)
{
	int iRet = 0;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
			iRet = poBuffer->AddArray( ptMetaField->hId, (char *)pcPtr, iSize/sizeof(char), ptMetaField->cIsRequired );
			break;
		case eTypeSKInt16:
			iRet = poBuffer->AddArray( ptMetaField->hId, (short *)pcPtr, iSize/sizeof(short), ptMetaField->cIsRequired );
			break;
		case eTypeSKInt32:
			iRet = poBuffer->AddArray( ptMetaField->hId, (int*)pcPtr, iSize/sizeof(int), ptMetaField->cIsRequired );
			break;
		case eTypeSKUchar:
		case eTypeSKUint8:
			iRet = poBuffer->AddArray( ptMetaField->hId, (unsigned char*)pcPtr, iSize/sizeof(unsigned char), ptMetaField->cIsRequired );
			break;
		case eTypeSKUint16:
			iRet = poBuffer->AddArray( ptMetaField->hId, (unsigned short *)pcPtr, iSize/sizeof(unsigned short), ptMetaField->cIsRequired );
			break;
		case eTypeSKUint32:
			iRet = poBuffer->AddArray( ptMetaField->hId, (unsigned int*)pcPtr, iSize/sizeof(unsigned int), ptMetaField->cIsRequired );
			break;
		case eTypeSKInt64:
			iRet = poBuffer->AddArray( ptMetaField->hId, (long long*)pcPtr, iSize/sizeof(long long), ptMetaField->cIsRequired );
			break;
		case eTypeSKUint64:
			iRet = poBuffer->AddArray( ptMetaField->hId, (unsigned long long*)pcPtr, iSize/sizeof(unsigned long long), ptMetaField->cIsRequired );
			break;

		case eTypeSKFloat32:
			iRet = poBuffer->AddArray( ptMetaField->hId, (float*)pcPtr, iSize/sizeof(float), ptMetaField->cIsRequired );
			break;
		case eTypeSKDouble64:
			iRet = poBuffer->AddArray( ptMetaField->hId, (double*)pcPtr, iSize/sizeof(double), ptMetaField->cIsRequired );
			break;
		case eTypeSKSInt32:
			iRet = poBuffer->AddSInt32Array( ptMetaField->hId, (int*)pcPtr, iSize/sizeof(int), ptMetaField->cIsRequired );
			break;
		case eTypeSKSInt64:
			iRet = poBuffer->AddSInt64Array( ptMetaField->hId, (long long*)pcPtr, iSize/sizeof(long long), ptMetaField->cIsRequired );
			break;
		default:
			TLV_LOG_ERR("%s::ERROR unknown type(%d)", MODULE, ptMetaField->hType );
			iRet = -1;
			break;
	}

	return iRet ;
}

int SKPBPickle :: GetBaseTypeValSize (
	const SKMetaField_t * ptMetaField,
	const char * pcValue
)
{
	if( NULL == pcValue )
	{
		TLV_LOG_ERR( "%s :: ERROR struct null",MODULE);
		return -1;
	}

	int len = 0;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
			len = SKPBHelper :: GetSize( ptMetaField->hId, (long long)*(signed char*)pcValue );
			break;
		case eTypeSKUchar:
		case eTypeSKUint8:
			len = SKPBHelper :: GetSize( ptMetaField->hId, (unsigned long long)*( unsigned char *)pcValue );
			break;
		case eTypeSKInt16:
			len = SKPBHelper :: GetSize( ptMetaField->hId, (long long)*( signed short *)pcValue );
			break;
		case eTypeSKUint16:
			len = SKPBHelper :: GetSize( ptMetaField->hId, (unsigned long long)*( unsigned short*)pcValue );
			break;
		case eTypeSKInt32:
			len = SKPBHelper :: GetSize( ptMetaField->hId, (long long)*( signed int *)pcValue );
			break;
		case eTypeSKUint32:
			len = SKPBHelper :: GetSize( ptMetaField->hId, (unsigned long long)*( unsigned int*)pcValue );
			break;
		case eTypeSKFloat32:
			len = SKPBHelper :: GetSize( ptMetaField->hId, *( float *)pcValue );
			break;
		case eTypeSKInt64:
			len = SKPBHelper :: GetSize( ptMetaField->hId, *( long long *)pcValue );
			break;
		case eTypeSKUint64:
			len = SKPBHelper :: GetSize( ptMetaField->hId, *( unsigned long long *)pcValue );
			break;
		case eTypeSKDouble64:
			len = SKPBHelper :: GetSize( ptMetaField->hId, *( double *)pcValue );
			break;
		case eTypeSKSInt32:
			len = SKPBHelper :: GetSInt32Size( ptMetaField->hId, *( int *)pcValue );
			break;
		case eTypeSKSInt64:
			len = SKPBHelper :: GetSInt64Size( ptMetaField->hId, *( long long *)pcValue );
			break;
		default:
			TLV_LOG_ERR("%s::ERROR unknown type(%d)", MODULE, ptMetaField->hType );
			len = -1;
			break;
	}

	return len;
}
int SKPBPickle :: PackBaseTypeVal (
	const SKMetaField_t * ptMetaField,
	const char * pcValue,
	SKPBEncoder *poBuffer
)
{
	int iRet = 0;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
			iRet = poBuffer->AddInt64( ptMetaField->hId, *( signed char *)pcValue );
			break;
		case eTypeSKUchar:
		case eTypeSKUint8:
			iRet = poBuffer->AddUInt64( ptMetaField->hId, *( unsigned char *)pcValue );
			break;
		case eTypeSKInt16:
			iRet = poBuffer->AddInt64( ptMetaField->hId, *( signed short *)pcValue );
			break;
		case eTypeSKUint16:
			iRet = poBuffer->AddUInt64( ptMetaField->hId, *( unsigned short*)pcValue );
			break;
		case eTypeSKInt32:
			iRet = poBuffer->AddInt64( ptMetaField->hId, *( signed int *)pcValue );
			break;
		case eTypeSKUint32:
			iRet = poBuffer->AddUInt64( ptMetaField->hId, *( unsigned int*)pcValue );
			break;
		case eTypeSKFloat32:
			iRet = poBuffer->AddFloat( ptMetaField->hId, *( float *)pcValue );
			break;
		case eTypeSKInt64:
			iRet = poBuffer->AddInt64( ptMetaField->hId, *( long long *)pcValue );
			break;
		case eTypeSKUint64:
			iRet = poBuffer->AddUInt64( ptMetaField->hId, *( unsigned long long*)pcValue );
			break;
		case eTypeSKDouble64:
			iRet = poBuffer->AddDouble( ptMetaField->hId, *( double *)pcValue );
			break;
		case eTypeSKSInt32:
			iRet = poBuffer->AddSInt32( ptMetaField->hId, *( int *)pcValue );
			break;
		case eTypeSKSInt64:
			iRet = poBuffer->AddSInt64( ptMetaField->hId, *( long long *)pcValue );
			break;
		default:
			TLV_LOG_ERR("%s::ERROR unknown type(%d)", MODULE, ptMetaField->hType );
			iRet = -1;
			break;
	}

	return iRet ;
}

int SKPBPickle :: UnpackBaseType (
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	SKPBDecoder *poBuffer,
	const void *pvStruct,
	int &iSize
)
{
	
	if( NULL == pvStruct )
	{
		TLV_LOG_ERR( "%s :: ERROR struct null",MODULE);
		return -1;
	}
	int iRet = 0 ;
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]) ;
	char *pcBase = (char *)pvStruct;
	char *pcValue = NULL;
	iSize = 0;

	int iExSize = 0;	
	if ( ptMetaField->cIsPtr )
	{
		if ( 0 == strcasecmp(ptMetaField->pcReferTo, "strlen") )
		{
			iRet = poBuffer->GetStringArrayLen(ptMetaField->hId, iSize);
			if ( 0 > iSize )
			{
				//TLV_LOG_ERR ( "%s::ERROR: UnpackBaseType GetBuf() %i failed -->", MODULE, iRet );
				//SKMetaUtils::DumpField(ptMetaField);
			}
			iExSize = 1;
		}
		else
		{
			iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct,ptMetaStruct->iSize ); 
		}

		if ( iSize == 0 )
		{
			pcValue = strdup("");

			*(void**)(pcBase + ptMetaField->hOffset) = pcValue;

			return 0;
		}

		if ( 0 > iSize )
		{
			return -1;
		}

		pcValue = (char *)calloc( 1, iSize + iExSize );
		if( pcValue == NULL )
		{
			TLV_LOG_ERR( "%s(%d)::ERROR MEM ERROR",MODULE,__LINE__);
			return -1;
		}

		*(void**)(pcBase + ptMetaField->hOffset) = pcValue;
	}
	else if( ptMetaField->hArraySize > 0 )
	{
		int buffsize = 0;
		iRet = poBuffer->GetStringArrayLen(ptMetaField->hId, buffsize );
		if( iRet != 0 )
		{
			return -1;
		}
		int iFieldSize;
		iFieldSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct, ptMetaStruct->iSize ); 
		iSize = std::min( iFieldSize, buffsize );
		pcValue = pcBase+ptMetaField->hOffset;
	}
	else
	{
		iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct, ptMetaStruct->iSize ); 
		pcValue = pcBase+ptMetaField->hOffset;
	}

	if( ptMetaField->cIsPtr || ptMetaField->hArraySize > 0 )
	{
		iRet = UnpackBaseTypePtr( ptMetaField, poBuffer, pcValue, iSize );
	} else {
		iRet = UnpackBaseTypeVal( ptMetaField, poBuffer, pcValue );
	}
	return iRet ;

}

int SKPBPickle :: UnpackBaseTypeVal (
	const SKMetaField_t * ptMetaField,
	SKPBDecoder *poBuffer,
	char * pcValue
)
{
	
	if( NULL == pcValue )
	{
		TLV_LOG_ERR( "%s :: ERROR struct null",MODULE);
		return -1;
	}

	int iRet = 0;

	switch ( ptMetaField ->hType )
	{
		case eTypeSKFloat32:
			iRet = poBuffer->GetFloat( *(float*)pcValue );
			return iRet;
		case eTypeSKDouble64:
			iRet = poBuffer->GetDouble( *(double*)pcValue );
			return iRet;
		case eTypeSKSInt32:
			iRet = poBuffer->GetSInt32( *(int*)pcValue );
			return iRet;
		case eTypeSKSInt64:
			iRet = poBuffer->GetSInt64( *(long long*)pcValue );
			return iRet;
	}

	long long lValue;
	iRet = poBuffer -> GetInt64( lValue );
	if( iRet )
	{
		return iRet;
	}

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
			*(char *)pcValue = (char)lValue;
			break;
		case eTypeSKInt16:
			*(short*)pcValue = (short)lValue;
			break;
		case eTypeSKInt32:
			*(int*)pcValue = (int)lValue;
			break;
		case eTypeSKUchar:
		case eTypeSKUint8:
			*(unsigned char*)pcValue = (unsigned char)lValue;
			break;
		case eTypeSKUint16:
			*(unsigned short*)pcValue = (unsigned short)lValue;
			break;
		case eTypeSKUint32:
			*(unsigned int*)pcValue = (unsigned int)lValue;
			break;
		case eTypeSKInt64:
			*(long long*)pcValue = (long long)lValue;
			break;
		case eTypeSKUint64:
			*(unsigned long long*)pcValue = (unsigned long long)lValue;
			break;
		default:
			TLV_LOG_ERR("%s::ERROR unknown type(%d)", MODULE, ptMetaField->hType );
			iRet = -1;
			break;
	}
	return 0;
}

int SKPBPickle :: UnpackBaseTypePtr(
	const SKMetaField_t * ptMetaField,
	SKPBDecoder *poBuffer,
	char * pcValue,
	int iSize
)
{
	if( NULL == pcValue )
	{
		TLV_LOG_ERR( "%s :: ERROR struct null",MODULE);
		return -1;
	}

	int iRet = 0;
	switch( ptMetaField -> hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
			iRet = poBuffer->GetArray( (char *)pcValue, iSize/sizeof(char) );
			break;
		case eTypeSKInt16:
		        iRet = poBuffer->GetArray( (short *)pcValue, iSize/sizeof(short) );
			break;
		case eTypeSKInt32:
			iRet = poBuffer->GetArray( (int *)pcValue, iSize/sizeof(int) );
			break;
		case eTypeSKUchar:
		case eTypeSKUint8:
			iRet = poBuffer->GetArray( (unsigned char*)pcValue, iSize/sizeof(unsigned char) );
			break;
		case eTypeSKUint16:
		        iRet = poBuffer->GetArray( (unsigned short*)pcValue, iSize/sizeof(unsigned short) );
			break;
		case eTypeSKUint32:
			iRet = poBuffer->GetArray( (unsigned int*)pcValue, iSize/sizeof(unsigned int) );
			break;
		case eTypeSKFloat32:
			iRet = poBuffer->GetArray( (float*)pcValue, iSize/sizeof(float) );
			break;
		case eTypeSKInt64:
			iRet = poBuffer->GetArray( (long long*)pcValue, iSize/sizeof(long long) );
			break;
		case eTypeSKUint64:
			iRet = poBuffer->GetArray( (unsigned long long*)pcValue , iSize/sizeof(unsigned long long));
			break;
		case eTypeSKDouble64:
			iRet = poBuffer->GetArray( (double*)pcValue, iSize/sizeof(double) );
			break;
		case eTypeSKSInt32:
			iRet = poBuffer->GetSInt32Array( ( int *)pcValue, iSize/sizeof(int) );
			break;
		case eTypeSKSInt64:
			iRet = poBuffer->GetSInt64Array( (long long*)pcValue, iSize/sizeof(long long) );
			break;
		default:
			TLV_LOG_ERR("%s::ERROR unknown type(%d)", MODULE, ptMetaField->hType );
			iRet = -1;
			break;
	}
	return iRet ;
}


};
