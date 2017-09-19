#include <stdlib.h>

#include "skmetainfo.h"
#include "sktlvpickle.h"
#include "skmetautils.h"
#include "skbuffer.h"

#include "tlvport.h"
#include "iVByte.h"

#include "iSKTLVPack.h"
#include "iSKTLVBuffer.h"


#include "iHash.h"

namespace Comm
{

//for hash
typedef struct tagKey
{
	int iLen;
	int iNodeNum;

	tagKey(){}
	tagKey(const int &aiLen,const int &aiNodeNum):iLen(aiLen),iNodeNum(aiNodeNum){}

}Key_t;

typedef const SKMetaStruct_t* SKMetaStruct_const_PTR;
typedef struct tagSKTLVPickleImpl {

	const SKMetaInfo_t * ptMetaInfo;
	int iNetworkByteOrder;

	CHashTable< int, Key_t > KeyHash;
	CHashTable< int, SKMetaStruct_const_PTR> MetaStructHash;

	static unsigned int HFunc(const int &key)
	{
		return (unsigned int )key;
	}

	tagSKTLVPickleImpl():KeyHash(tagSKTLVPickleImpl :: HFunc,1),MetaStructHash(tagSKTLVPickleImpl :: HFunc,0){}

} SKTLVPickleImpl_t;

SKTLVPickle :: SKTLVPickle( const SKMetaInfo_t * ptMetaInfo )
{
	m_ptImpl = new SKTLVPickleImpl_t;
	m_ptImpl->ptMetaInfo = ptMetaInfo;
	m_ptImpl->iNetworkByteOrder = 0;
}

SKTLVPickle :: SKTLVPickle( const SKMetaInfo_t * ptMetaInfo, int iNetworkByteOrder )
{
	m_ptImpl = new SKTLVPickleImpl_t;
	m_ptImpl->ptMetaInfo = ptMetaInfo;
	m_ptImpl->iNetworkByteOrder = iNetworkByteOrder;
}

SKTLVPickle :: ~SKTLVPickle()
{
	delete m_ptImpl;
	m_ptImpl = NULL;
}

// public ================

void SKTLVPickle :: SetNetworkByteOrder( int iNetworkByteOrder )
{
	m_ptImpl->iNetworkByteOrder = iNetworkByteOrder;
}


int SKTLVPickle :: Struct2Buffer(
	const int iTypeId,
	const void *pvStruct,
	const int iStructSize,
	SKBuffer *poBuffer
)
{
	int iRet = COMM_ERR_GENERAL ;

	int iLen = 0,iNodenum = 0,iLev = 1;

	iRet = m_ptImpl->KeyHash.Init();
	if( iRet != 0 )
	{
		TLV_LOG_ERR ( "ERROR: KeyHash table init err ret(%d)",
			iRet);
			return iRet;
	}

	iRet = m_ptImpl->MetaStructHash.Init();
	if( iRet != 0 )
	{
		TLV_LOG_ERR ( "ERROR: KeyHash table init err ret(%d)",
			iRet);
			return iRet;
	}
		
	iRet = CountLen(iTypeId,pvStruct,iStructSize,iLev,iNodenum,iLen);
	
	iLen = SKTLVBuffer::SizeBuf(iLen);//buflen+headerlen

	SKTLVBuffer * ptTlvBuffer = new SKTLVBuffer(iLen);

	SKTLVPack *poTlvBuffer = new SKTLVPack(ptTlvBuffer);
	
	iLev = 1;	

	if( 0 == iRet )
	{
		iRet = PackBuffer( iTypeId, pvStruct, iStructSize, poTlvBuffer, iLev );
		if ( 0 == iRet )
		{
			char *pcBuffer = NULL;
			int iUsedSize=0;
			int iAllocSize=0;
			iRet = ptTlvBuffer->Detach(&pcBuffer, &iUsedSize, &iAllocSize);
			if ( 0 == iRet )
			{
				iRet = poBuffer->Write(pcBuffer, iUsedSize);
				if ( iRet == iUsedSize )
				{
					/* final ok */
					iRet = 0 ;
				}
			}

			if ( pcBuffer ) 
				delete[] pcBuffer, pcBuffer=NULL;
		}
	}

	if( iRet )
		TLV_LOG_ERR ( "ERROR: SvrKit tlv struct2buffer ret %d len %i ", iRet, poBuffer->GetLen() );

	if( poTlvBuffer )
		delete poTlvBuffer, poTlvBuffer=NULL;
	if( ptTlvBuffer )
		delete ptTlvBuffer, ptTlvBuffer=NULL;
	return iRet ;
}

int SKTLVPickle :: Buffer2Struct(
	const int iTypeId,
	SKBuffer *poBuffer,
	void *pvStruct,
	const int iStructSize
)
{
	int iRet = COMM_ERR_GENERAL ;

	SKTLVBuffer * ptTlvBuffer = new SKTLVBuffer(0);

	iRet = ptTlvBuffer->Attach((char *)poBuffer->GetBuffer(), poBuffer->GetLen());
	if ( 0 != iRet )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit tlv buffer attach failed %i len %i",
			iRet, poBuffer->GetLen() );
		return COMM_ERR_PARA;
	}

	SKTLVPack *poTlvBuffer = new SKTLVPack(ptTlvBuffer,0);
	
	iRet = poTlvBuffer->Init();

	if ( 0 != iRet )
	{
		iRet = COMM_ERR_PARA;
		TLV_LOG_ERR ( "ERROR: SvrKit tlv buffer2struct ret %d len %i ", iRet, poBuffer->GetLen() );
	}
	else
	{
		iRet = UnpackBuffer( iTypeId, poTlvBuffer, pvStruct, iStructSize);
		if ( 0 != iRet )
		{
			/* failed */
		}
	}


	/* detach SKBuffer first */
	ptTlvBuffer->Detach(NULL, NULL, NULL);

    delete poTlvBuffer, poTlvBuffer=NULL;

    delete ptTlvBuffer, ptTlvBuffer=NULL;
    return iRet ;
}


// private ================
int SKTLVPickle :: CountLen(
	const int iTypeId,
	const void *pvStruct,
	const int iStructSize,
	int &iLev,
	int &iNodeNum,
	int &iLen
)
{
	int iRet = COMM_OK;
	Key_t val;
	iRet = m_ptImpl ->KeyHash.Find(iLev,val);
	if( iRet == 0 )
	{
		iLen = val.iLen;
		iNodeNum = val.iNodeNum;
		return iRet;
	}

	int iCurLev = iLev++;

	iNodeNum = 1;

	iLen = 0;

	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;

	iRet = m_ptImpl->MetaStructHash.Find( iTypeId, ptMetaStruct );

	if( iRet != 0 )
	{
		ptMetaStruct = SKMetaUtils::FindStruct ( m_ptImpl->ptMetaInfo, iTypeId );

		if ( NULL != ptMetaStruct )
		{
			iRet = m_ptImpl->MetaStructHash.Insert( iTypeId, ptMetaStruct );
			if( iRet < 0 )
			{
				TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer find struct hash insert error");
				return iRet;
			}
		}
	}

	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer FindStruct null id %i", iTypeId );
		return -1;
	}

	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[i]) ;

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType < ptMetaField->hType )
		{
			/* user defined type or built-in type, recursive */
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, i, pvStruct, iStructSize );
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
				int iLen = 0,iChildNodeNum = 0;
				iRet = CountLen( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex),
					ptMetaField->iItemSize,iLev,iChildNodeNum, iLen);

				if( COMM_OK != iRet )
				{
					return iRet;
				}

				iItemlen += SKTLVBuffer :: SizeBufWithTL( itemIndex, iLen );//tl+header+buf
				iNodeNum += iChildNodeNum;	
			}
			
			iLen += SKTLVBuffer::SizeBufWithTL( ptMetaField->hId, iItemlen);
		}
		else
		{
			int iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, i, pvStruct, iStructSize ); 

			iLen += SKTLVBuffer::SizeBuf( ptMetaField->hId, iSize);
		}
	}

	iRet = m_ptImpl->KeyHash.Insert(iCurLev,Key_t(iLen,iNodeNum));
	if( iRet < 0 )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer KeyHash insert error" );
	}

	return iRet ;
}

int SKTLVPickle :: PackBuffer(
	const int iTypeId,
	const void *pvStruct,
	const int iStructSize,
	SKTLVPack *poTlvBuffer,
	int &iLev
)
{
	int iCurLev = iLev++;	
	int iRet = COMM_OK ;
	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;
	
	iRet = m_ptImpl->MetaStructHash.Find( iTypeId, ptMetaStruct );

	if( iRet != 0 )
	{
		ptMetaStruct = SKMetaUtils::FindStruct ( m_ptImpl->ptMetaInfo, iTypeId );

		if ( NULL != ptMetaStruct )
		{
			iRet = m_ptImpl->MetaStructHash.Insert( iTypeId, ptMetaStruct );
			if( iRet < 0 )
			{
				TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer KeyHash insert error TypeId(%d)", iTypeId );
				return iRet;
			}	
		}
	}

	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer FindStruct null id %i", iTypeId );
		return COMM_ERR_PARA;
	}

	int iChildNodeNum = 0;

	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[i]) ;

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType >= ptMetaField->hType )
		{
			/* base type */
			iRet = PackBaseType ( ptMetaStruct, i, pvStruct,
					ptMetaField->iItemSize, poTlvBuffer, m_ptImpl->iNetworkByteOrder ) ;
			if ( COMM_OK != iRet )
			{
				TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer basetype failed %i", i );
				return iRet;
			}
		}
	}

	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[i]) ;

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType < ptMetaField->hType )
		{
			/* user defined type or built-in type, recursive */
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, i, pvStruct, iStructSize );
			if( iItemCnt < 0 )
			{
				TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer basetype failed %i", i );
				return -1;
			}
			char * pcPos = NULL;
			
			if( ptMetaField->cIsPtr )
			{
				pcPos = (char*)*(void**)(pcBase + ptMetaField->hOffset);
			} else {
				pcPos = pcBase + ptMetaField->hOffset;
			}
			
			int iLen=0;
			int *tmpLen = new int[iItemCnt];
			for ( int itemIndex = 0; itemIndex < iItemCnt; ++itemIndex )
			{
				int iItemLen=0;
				int tmpNodeChildNum = 0;
				int tmpLev = iCurLev+iChildNodeNum+1;

				CountLen( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex),
					ptMetaField->iItemSize, tmpLev,tmpNodeChildNum, iItemLen);

				tmpLen[itemIndex] = iItemLen;

				iChildNodeNum += tmpNodeChildNum;	

				iLen += SKTLVBuffer :: SizeBufWithTL( itemIndex, iItemLen);
			}

			poTlvBuffer->AddTL( ptMetaField->hId, SKTLVBuffer :: SizeBuf(iLen) );//just add len + header

			SKTLVPack poListBuffer(poTlvBuffer->GetTLVBuf());
			
			for ( int itemIndex = 0; itemIndex < iItemCnt; ++itemIndex )
			{
				int iItemLen = tmpLen[itemIndex];

				poListBuffer.AddTL( itemIndex, SKTLVBuffer :: SizeBuf( iItemLen ) );//just add len + header

				SKTLVPack poBuffer(poListBuffer.GetTLVBuf());;

				iRet = PackBuffer( ptMetaField->hType, (void *)(pcPos+ptMetaField->iItemSize* itemIndex),
					ptMetaField->iItemSize, &poBuffer,iLev );

				if ( 0 == iRet )
				{
					iRet = poListBuffer.AddNestedTLV ( &poBuffer ) ;
					if ( 0 != iRet )
					{
						TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer AddNestedTLV failed %i id %i",
								itemIndex, ptMetaField->hId );
					}
				}
				else
				{
					TLV_LOG_ERR ( "ERROR: SvrKit PackBuffer recursive failed %i", itemIndex );
					iRet = COMM_ERR_DATA;
				}

				if ( 0 != iRet )
				{
					break;
				}
			}

			iRet = poTlvBuffer->AddNestedTLV ( &poListBuffer ) ;

			if( tmpLen != NULL )
				delete[] tmpLen;
		}
	}

	return iRet ;
}

int SKTLVPickle :: UnpackBuffer(
	const int iTypeId,
	SKTLVPack *poTlvBuffer,
	void *pvStruct,
	const int iStructSize
)
{
	int iRet = COMM_OK ;
	const SKMetaStruct_t *ptMetaStruct = NULL ;
	const SKMetaField_t *ptMetaField = NULL ;
	char *pcBase = (char *)pvStruct;

	ptMetaStruct = SKMetaUtils::FindStruct ( m_ptImpl->ptMetaInfo, iTypeId );
	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit UnpackBuffer FindStruct null id %i", iTypeId );
		return COMM_ERR_PARA;
	}

	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[i]) ;

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType >= ptMetaField->hType )
		{
			/* base type */
			iRet = UnpackBaseType ( ptMetaStruct, i, poTlvBuffer,
					pvStruct, ptMetaField->iItemSize, m_ptImpl->iNetworkByteOrder);

			if ( COMM_OK != iRet )
			{
				if ( 0 == ptMetaField->cIsRequired )
				{
					iRet = 0;
					//TLV_LOG_DEBUG ( "DEBUG: SvrKit UnpackBaseType missing optional field id %d", ptMetaField->hId );
					continue ;
				}

				TLV_LOG_ERR ( "ERROR: SvrKit UnpackBaseType failed %i %i", iRet, i );
				return iRet;
			}
		}
	}

	for ( int i=0; i<ptMetaStruct->hFieldCount; ++i )
	{
		ptMetaField = &(ptMetaStruct->ptFieldList[i]) ;

		if ( /*eTypeSKUserDefine*/eTypeSKBuiltinType < ptMetaField->hType )
		{  
			/* user defined type, recursive */
			int iItemCnt = SKMetaUtils::GetItemCnt ( ptMetaStruct, i, pvStruct, iStructSize );

			char *pcValue = pcBase+ptMetaField->hOffset;
			if ( ptMetaField->cIsPtr )
			{
				pcValue = (char *)calloc(ptMetaField->iItemSize, iItemCnt);
				*(void**)(pcBase + ptMetaField->hOffset) = pcValue;
			}

			SKTLVPack *poListBuffer = NULL;
			iRet = poTlvBuffer->GetNestedTLVBuf ( ptMetaField->hId, &poListBuffer );

			if ( 0 != iRet )
			{
				if( 0 == ptMetaField->cIsRequired )
				{
					iRet = 0;
					//TLV_LOG_ERR ( "DEBUG: SvrKit GetNestedTLVBuf missing optional field id %d", ptMetaField->hId );
					continue;
				} else {
					TLV_LOG_ERR ( "ERROR: SvrKit UnpackBuffer GetNestedTLVBuf failed %i id %i ",
									iRet, ptMetaField->hId );
					if( poListBuffer )
					{
						delete poListBuffer;
						poListBuffer=NULL;
					}
					break;
				}
			}

			for ( int i=0; i<iItemCnt; ++i )
			{
				SKTLVPack *poBuffer = NULL;
				iRet = poListBuffer->GetNestedTLVBuf ( i, &poBuffer );

				if ( 0 != iRet )
				{
					/* failed */
					TLV_LOG_ERR ( "ERROR: SvrKit UnpackBuffer GetNestedTLVBuf failed %i id %i ", iRet, ptMetaField->hId );
				}
				else
				{
					iRet = UnpackBuffer( ptMetaField->hType, poBuffer,
							(void *)(pcValue + ptMetaField->iItemSize*i), ptMetaStruct->iSize);
					if ( COMM_OK != iRet )
					{
						TLV_LOG_ERR ( "ERROR: SvrKit UnpackBuffer failed %i %i", iRet, i );
					}
				}
				if( poBuffer )
				{
					delete poBuffer;
					poBuffer=NULL;
				}
				if( iRet != COMM_OK )
					break;
			}
			if( poListBuffer )
			{
				delete poListBuffer;
				poListBuffer=NULL;
			}
		}
	}

	return iRet ;
}

int SKTLVPickle :: PackBaseType( 
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	const void *pvStruct,
	const int iStructSize,
	SKTLVPack *poTlvBuffer,
	int iNetworkByteOrder
)
{
	int iRet = COMM_OK ;
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]);
	char *pcBase = (char *)pvStruct;
	int iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct, iStructSize ); 
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

	if( 0 == iNetworkByteOrder )
	{
		iRet = poTlvBuffer->AddBuf(ptMetaField->hId, pcValue, iSize );
	} else {
		if( ptMetaField->cIsPtr || ptMetaField->hArraySize > 0 )
		{
			iRet = PackBaseTypePtr( ptMetaField, pcValue, iSize, poTlvBuffer );
		} else {
			iRet = PackBaseTypeVal( ptMetaField, pcValue, poTlvBuffer );
		}
	}

	return iRet;
}

int SKTLVPickle :: PackBaseTypePtr (
	const SKMetaField_t * ptMetaField,
	const char * pcPtr,
	int iSize,
	SKTLVPack *poTlvBuffer
)
{
	int iRet = COMM_OK;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
		case eTypeSKUchar:
		case eTypeSKUint8:
			iRet = poTlvBuffer->AddBuf( ptMetaField->hId, pcPtr, iSize );
			break;
		case eTypeSKInt16:
		case eTypeSKUint16:
		{
			char * tmp = (char*)calloc( sizeof( char ), iSize );
			for( int i = 0; i < iSize; i += sizeof( uint16_t ) )
			{
				unsigned short val = 0;
				memcpy( &val, pcPtr + i, sizeof( val ) );
				val = htons( val );
				memcpy( tmp + i, &val, sizeof( val ) );
			}
			iRet = poTlvBuffer->AddBuf( ptMetaField->hId, tmp, iSize );
			free( tmp );
			break;
		}
		case eTypeSKInt32:
		case eTypeSKSInt32:
		case eTypeSKUint32:
		case eTypeSKFloat32:
		{
			char * tmp = (char*)calloc( sizeof( char ), iSize );
			for( int i = 0; i < iSize; i += sizeof( uint32_t ) )
			{
				uint32_t val = 0;
				memcpy( &val, pcPtr + i, sizeof( val ) );
				val = htonl( val );
				memcpy( tmp + i, &val, sizeof( val ) );
			}
			iRet = poTlvBuffer->AddBuf( ptMetaField->hId, tmp, iSize );
			free( tmp );
			break;
		}
		case eTypeSKInt64:
		case eTypeSKSInt64:
		case eTypeSKUint64:
		case eTypeSKDouble64:
		{
			char * tmp = (char*)calloc( sizeof( char ), iSize );
			for( int i = 0; i < iSize; i += sizeof( uint64_t ) )
			{
				uint64_t val = 0;
				memcpy( &val, pcPtr + i, sizeof( val ) );
				val = ( (unsigned long long) ( htonl( (unsigned long)val ) ) << 32 )
					| ( unsigned long long) htonl( (unsigned long)( val >> 32 ) );
				memcpy( tmp + i, &val, sizeof( val ) );
			}
			iRet = poTlvBuffer->AddBuf( ptMetaField->hId, tmp, iSize );
			free( tmp );
			break;
		}
		default:
			iRet = -1;
			break;
	}

	return iRet ;
}

int SKTLVPickle :: PackBaseTypeVal (
	const SKMetaField_t * ptMetaField,
	const char * pcValue,
	SKTLVPack *poTlvBuffer
)
{
	int iRet = COMM_OK;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
		case eTypeSKUchar:
		case eTypeSKUint8:
			//printf("PkBase hid=%d type=%d add byte ptr=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = poTlvBuffer->AddByte( ptMetaField->hId, *pcValue );
			break;
		case eTypeSKInt16:
		case eTypeSKUint16:
		{
			//printf("PkBase hid=%d type=%d add word ptr=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			unsigned short tmp = 0;
			memcpy( &tmp, pcValue, sizeof( tmp ) );
			iRet = poTlvBuffer->AddWord( ptMetaField->hId, tmp );
			break;
		}
		case eTypeSKInt32:
		case eTypeSKSInt32:
		case eTypeSKUint32:
		case eTypeSKFloat32:
		{
			//printf("PkBase hid=%d type=%d add int ptr=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			unsigned int tmp = 0;
			memcpy( &tmp, pcValue, sizeof( tmp ) );
			iRet = poTlvBuffer->AddUInt( ptMetaField->hId, tmp );
			break;
		}
		case eTypeSKInt64:
		case eTypeSKSInt64:
		case eTypeSKUint64:
		case eTypeSKDouble64:
		{
			//printf("PkBase hid=%d type=%d add LLong ptr=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			uint64_t tmp = 0;
			memcpy( &tmp, pcValue, sizeof( tmp ) );
			iRet = poTlvBuffer->AddULongLong( ptMetaField->hId, tmp );
			break;
		}
		default:
			//printf("PkBase hid=%d type=%d add ERRTYPE ptr=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = -1;
			break;
	}

	return iRet ;
}

int SKTLVPickle :: UnpackBaseType (
	const SKMetaStruct_t *ptMetaStruct,
	const int iFieldId,
	SKTLVPack *poTlvBuffer,
	const void *pvStruct,
	const int iStructSize,
	int iNetworkByteOrder
)
{
	int iRet = COMM_OK ;
	const SKMetaField_t *ptMetaField = &(ptMetaStruct->ptFieldList[iFieldId]) ;
	char *pcBase = (char *)pvStruct;
	char *pcValue = NULL;
	int iSize = 0;

	//SKMetaUtils::DumpField(ptMetaField);

	if ( ptMetaField->cIsPtr )
	{
		if ( 0 == strcasecmp(ptMetaField->pcReferTo, "strlen") )
		{
			iRet = poTlvBuffer->GetBuf(ptMetaField->hId, NULL, &iSize);
			if ( 0 >= iSize )
			{
				//TLV_LOG_ERR ( "ERROR: UnpackBaseType GetBuf() %i failed -->", iRet );
				//SKMetaUtils::DumpField(ptMetaField);
				pcValue = (char *)calloc( 1, iSize );
				*(void**)(pcBase + ptMetaField->hOffset) = pcValue;
			}
		}
		else
		{
			iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct, iStructSize ); 
		}

		if ( 0 >= iSize )
		{
			return -1;
		}

		pcValue = (char *)calloc( 1, iSize );
		*(void**)(pcBase + ptMetaField->hOffset) = pcValue;
	}
	else
	{
		iSize = SKMetaUtils::GetFieldSize ( ptMetaStruct, iFieldId, pvStruct, iStructSize ); 
		pcValue = pcBase+ptMetaField->hOffset;
		//printf("fid=%d iSize=%d hid=%d offset=%d pcValue=%p\n", iFieldId, iSize, ptMetaField->hId, ptMetaField->hOffset, pcValue);
	}

	if( 0 == iNetworkByteOrder )
	{
		iRet = poTlvBuffer->GetBuf(ptMetaField->hId, pcValue, &iSize);
	} else {
		if( ptMetaField->cIsPtr || ptMetaField->hArraySize > 0 )
		{
			iRet = UnpackBaseTypePtr( ptMetaField, poTlvBuffer, pcValue, iSize );
		} else {
			iRet = UnpackBaseTypeVal( ptMetaField, poTlvBuffer, pcValue );
		}
	}

	return iRet ;
}

int SKTLVPickle :: UnpackBaseTypeVal (
	const SKMetaField_t * ptMetaField,
	SKTLVPack *poTlvBuffer,
	char * pcValue
)
{
	int iRet = COMM_OK;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
		case eTypeSKUchar:
		case eTypeSKUint8:
			//printf("hid=%d type=%d GetByte pcValue=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = poTlvBuffer->GetByte( ptMetaField->hId, (unsigned char *)pcValue );
			break;
		case eTypeSKInt16:
		case eTypeSKUint16:
		{
			//printf("hid=%d type=%d GetWord pcValue=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = poTlvBuffer->GetWord( ptMetaField->hId, (unsigned short *)pcValue );
			break;
		}
		case eTypeSKInt32:
		case eTypeSKSInt32:
		case eTypeSKUint32:
		case eTypeSKFloat32:
		{
			//printf("hid=%d type=%d GetInt pcValue=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = poTlvBuffer->GetUInt( ptMetaField->hId, (unsigned int *)pcValue );
			break;
		}
		case eTypeSKInt64:
		case eTypeSKSInt64:
		case eTypeSKUint64:
		case eTypeSKDouble64:
		{
			//printf("hid=%d type=%d GetLongLong pcValue=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = poTlvBuffer->GetULongLong( ptMetaField->hId, (unsigned long long *)pcValue );
			break;
		}
		default:
			//printf("hid=%d type=%d ERRTYPE pcValue=%p\n", ptMetaField->hId, ptMetaField->hType, pcValue);
			iRet = -1;
			break;
	}

	return iRet ;
}

int SKTLVPickle :: UnpackBaseTypePtr(
	const SKMetaField_t * ptMetaField,
	SKTLVPack *poTlvBuffer,
	char * pcPtr,
	int iSize
)
{
	int iRet = poTlvBuffer->GetBuf( ptMetaField->hId, pcPtr, &iSize );

	if( COMM_OK != iRet ) return iRet;

	switch( ptMetaField->hType )
	{
		case eTypeSKChar:
		case eTypeSKInt8:
		case eTypeSKUchar:
		case eTypeSKUint8:
			// noop
			break;
		case eTypeSKInt16:
		case eTypeSKUint16:
		{
			for( int i = 0; i < iSize; i += sizeof( uint16_t ) )
			{
				unsigned short val = 0;
				memcpy( &val, pcPtr + i, sizeof( val ) );
				val = htons( val );
				memcpy( pcPtr + i, &val, sizeof( val ) );
			}
			break;
		}
		case eTypeSKInt32:
		case eTypeSKSInt32:
		case eTypeSKUint32:
		case eTypeSKFloat32:
		{
			for( int i = 0; i < iSize; i += sizeof( uint32_t ) )
			{
				uint32_t val = 0;
				memcpy( &val, pcPtr + i, sizeof( val ) );
				val = htonl( val );
				memcpy( pcPtr + i, &val, sizeof( val ) );
			}
			break;
		}
		case eTypeSKInt64:
		case eTypeSKSInt64:
		case eTypeSKUint64:
		case eTypeSKDouble64:
		{
			for( int i = 0; i < iSize; i += sizeof( uint64_t ) )
			{
				uint64_t val = 0;
				memcpy( &val, pcPtr + i, sizeof( val ) );
				val = ( (unsigned long long)(ntohl((unsigned long)val )) << 32 )
					| (unsigned long long)htonl((unsigned long)(val >> 32));
				memcpy( pcPtr + i, &val, sizeof( val ) );
			}
			break;
		}
		default:
			iRet = -1;
			break;
	}

	return iRet ;
}



}

