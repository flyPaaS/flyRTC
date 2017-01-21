#include <stdlib.h>

#include "skalloc.h"

#include "tlvport.h"

#include "skmetainfo.h"
#include "skmetautils.h"

#include <string.h>

namespace Comm
{
	
SKAllocator :: SKAllocator( const SKMetaInfo_t * ptMetaInfo )
{
	m_ptMetaInfo = ptMetaInfo;
}

// free first, then deep copy
int SKAllocator :: DeepCopy( 
	const int iTypeId, 
	void * pDestStruct, 
	const void * pSourceStruct, 
	const int iStructSize 
)
{
	int iRet = FreeField( iTypeId, pDestStruct, iStructSize );

	if ( -1 == iRet )
	{
		return -1;
	}

	return DeepCopy2( iTypeId, pDestStruct, pSourceStruct, iStructSize, true );
}

int SKAllocator :: DeepCopy2( 
	const int iTypeId, 
	void * pDestStruct, 
	const void * pSourceStruct, 
	const int iStructSize, 
	bool bFlag 
)
{
	int iRet = 0;
	const SKMetaStruct_t * ptMetaStruct = NULL;
	const SKMetaField_t * ptMetaField = NULL;
	char * pcSourceBase = ( char * )pSourceStruct;
	char * pcDestBase = ( char * )pDestStruct;

	if ( NULL == pcSourceBase )
	{
		return 0;
	}

	ptMetaStruct = SKMetaUtils :: FindStruct( m_ptMetaInfo, iTypeId );
	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit DeepCopy FindStruct null id %i", iTypeId );
		return COMM_ERR_PARA;
	}

	if ( bFlag )
	{
		memcpy( pcDestBase, pcSourceBase, iStructSize );
	}
	
	for ( int i = 0; i < ptMetaStruct->hFieldCount; i++ )
	{
		ptMetaField = &( ptMetaStruct->ptFieldList[i] );

		int iItemCnt = SKMetaUtils :: GetItemCnt( ptMetaStruct, i, pSourceStruct, iStructSize );

		//desc
		char * pcDestPos = NULL;
		//source
		char * pcSourcePos = NULL;
		
		if ( eTypeSKBuiltinType < ptMetaField->hType )
		{
			if ( ptMetaField->cIsPtr )
			{
				bFlag = true;
				pcSourcePos = (char *)*(void **)(pcSourceBase + ptMetaField->hOffset);
				if ( NULL != pcSourcePos )
				{
					pcDestPos = (char *)calloc( ptMetaField->iItemSize, iItemCnt );
				}
				*((void **)(pcDestBase + ptMetaField->hOffset)) = (void *)pcDestPos;
			}
			else
			{
				bFlag = false;
				pcSourcePos = pcSourceBase + ptMetaField->hOffset;
				pcDestPos = pcDestBase + ptMetaField->hOffset;
			}

			for ( int itemIndex = 0; itemIndex < iItemCnt; itemIndex++ )
			{
				iRet = DeepCopy2( ptMetaField->hType, 
						(void *)(pcDestPos + ptMetaField->iItemSize * itemIndex), 
						(void *)(pcSourcePos + ptMetaField->iItemSize * itemIndex),
						ptMetaField->iItemSize, bFlag );

				if ( -1 == iRet )
				{
					TLV_LOG_ERR ( "Error: SvrKit DeepCopy failed" );
					return -1;
				}
			}
		}
		else
		{
			if ( ptMetaField->cIsPtr )
			{
				pcSourcePos = (char *)*(void **)(pcSourceBase + ptMetaField->hOffset);

				if ( NULL != pcSourcePos )
				{
					pcDestPos = (char *)calloc( ptMetaField->iItemSize, iItemCnt );
					memcpy( pcDestPos, pcSourcePos, ptMetaField->iItemSize * iItemCnt );
					*((void **)(pcDestBase + ptMetaField->hOffset)) = (void *)pcDestPos;
				}
			}
		}
	}
	return 0;
}

int SKAllocator :: Diff( 
	const int iTypeId, 
	void * ptStruct1, 
	void * ptStruct2, 
	const int iStructSize
)
{
	int iRet = 0;
	const SKMetaStruct_t * ptMetaStruct = NULL;
	const SKMetaField_t * ptMetaField = NULL;
	char * pcBase1 = ( char * )ptStruct1;
	char * pcBase2 = ( char * )ptStruct2;

	if ( NULL == pcBase1 || NULL == pcBase2 )
	{
		if ( NULL != pcBase1 )
		{
			return -1;
		}
		if ( NULL != pcBase2 )
		{
			return -1;
		}
		return 0;
	}

	ptMetaStruct = SKMetaUtils :: FindStruct( m_ptMetaInfo, iTypeId );
	if ( NULL == ptMetaStruct )
	{
		TLV_LOG_ERR ( "ERROR: SvrKit Diff FindStruct null id %i", iTypeId );
		return COMM_ERR_PARA;
	}

	for ( int i = 0; i < ptMetaStruct->hFieldCount; i++ )
	{
		ptMetaField = &( ptMetaStruct->ptFieldList[i] );
		
		
		int  iItemCnt =  SKMetaUtils :: GetItemCnt( ptMetaStruct, i, ptStruct1, iStructSize );
		if ( 0 == strcasecmp(ptMetaField->pcReferTo, "strlen") )
		{
			-- iItemCnt;
		}

		char * pcPos1 = NULL;
		char * pcPos2 = NULL;
		
		if ( ptMetaField->cIsPtr )
		{
			pcPos1 = (char *)*(void **)(pcBase1 + ptMetaField->hOffset);
			pcPos2 = (char *)*(void **)(pcBase2 + ptMetaField->hOffset);
		}
		else
		{
			pcPos1 = pcBase1 + ptMetaField->hOffset;
			pcPos2 = pcBase2 + ptMetaField->hOffset;
		}
		
		if ( eTypeSKBuiltinType < ptMetaField->hType )
		{
			for ( int itemIndex = 0; itemIndex < iItemCnt; itemIndex++ )
			{
				iRet = Diff( ptMetaField->hType,
						(void *)(pcPos1 + ptMetaField->iItemSize * itemIndex),
						(void *)(pcPos2 + ptMetaField->iItemSize * itemIndex),
						ptMetaField->iItemSize );
				if ( -1 == iRet )
				{
					return -1;
				}
			}
		}
		else
		{
			int iToDiffSize = ptMetaField->iItemSize * iItemCnt;
			for ( int it = 0; it < iToDiffSize; it++ )
			{
				if ( *( pcPos1 + it ) != *( pcPos2 + it ) )
				{
					return -1;
				}
			}
		}
	}
	return 0;
}

int SKAllocator :: FreeField(
	const int iType,
	const void *pvStruct,
	const int iSize
)
{
	int ret = 0;

	if( NULL == pvStruct ) return -1;

	char * base = (char*)pvStruct;

	const SKMetaStruct_t * metaStruct = SKMetaUtils::FindStruct( m_ptMetaInfo, iType );

	if( NULL == metaStruct ) return -1;

	if( (int)(metaStruct->iSize) != iSize ) return -1;

	for( int i = 0; i < metaStruct->hFieldCount && 0 == ret; i++ ) {
		const SKMetaField_t * field = metaStruct->ptFieldList + i;

		if( field->hType > eTypeSKBuiltinType/*eTypeSKUserDefine*/ ) {
			if( field->cIsPtr ) {
				int referCount = SKMetaUtils::GetReferCount( pvStruct, metaStruct, field );
				const SKMetaStruct_t * referStruct = SKMetaUtils::FindStruct( m_ptMetaInfo, field->hType );
				char * referBase = (char*)*(void**)(base + field->hOffset);

				for( int j = 0; j < referCount && 0 == ret; j++ ) {
					ret = FreeField( field->hType, referBase + ( j * referStruct->iSize ), field->iItemSize );
				}

				if( NULL != referBase ) ::free( referBase );
			} else {
                /***************bug fixed************************/
                int iItemCnt = field->hArraySize;
                if (-1 == iItemCnt)
                {
                    ret = FreeField( field->hType, base + field->hOffset, field->iItemSize );
                }
                else
                {
                    // char * referBase = (char*)*(void**)(base + field->hOffset);
                    // const SKMetaStruct_t * referStruct = SKMetaUtils::FindStruct( m_ptMetaInfo, field->hType );
                    for(int j = 0; j < iItemCnt && 0 == ret; j++)
                    {
                        ret = FreeField( field->hType, base+field->hOffset+(j*field->iItemSize), field->iItemSize );
                    }
                }
                /***************bug fixed*************************/
				//ret = FreeField( field->hType, base + field->hOffset, field->iItemSize );
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

}
