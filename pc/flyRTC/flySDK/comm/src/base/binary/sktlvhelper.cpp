#include <stdlib.h>

#include "sktlvhelper.h"
#include "skbuffer.h"

#include "iTLVPickle.h"
#include "iSKTLVMergePack.h"

#include <map>
#include <string.h>

#include <stdlib.h>



namespace Comm
{
	
	


SKTLVHelper :: SKTLVHelper()
{
	//使用变长
	CTLVPack :: SetDefaultMode(CTLVPack::TLV_VARIABLE_TL_SIZE);
}

SKTLVHelper :: ~SKTLVHelper()
{
}


int SKTLVHelper :: Merge(
		const SKBuffer * poSrcBuffer1,
		const SKBuffer * poSrcBuffer2,
		SKBuffer * poDestBuffer
		)
{
	SKTLVMergePack oMergePack;
	int ret = oMergePack.Init( poSrcBuffer1, poSrcBuffer2 );
	if( ret )
	{
		LogErr("%s buff init err",__FUNCTION__);
		return ret;
	}

	CTLVPack poTlvBuffer;

	ret = MergeBuffer( &oMergePack, &poTlvBuffer );
	if( 0 != ret )
	{
		LogErr("%s mergebuffer 2 struct fail",__FUNCTION__ );
	}
	else
	{
		char *pcBuffer = NULL;
		int iUsedSize=0;
		int iAllocSize=0;
		int iRet = poTlvBuffer.Detach(&pcBuffer, &iUsedSize, &iAllocSize);
		if ( 0 == iRet )
		{
			iRet = poDestBuffer->Write(pcBuffer, iUsedSize);
			if ( iRet == iUsedSize )
			{
				/* final ok */
				iRet = 0 ;
			}
		}

		if ( pcBuffer ) delete[] pcBuffer, pcBuffer=NULL;
	}
	return ret;

}

int SKTLVHelper :: MergeBuffer(
		SKTLVMergePack * poMergePack,
		CTLVPack * poTlvBuffer
)
{
	int iRet = COMM_OK ;

	CTLVPack * poPack1 = NULL, * poPack2 = NULL;
	
	iRet = poMergePack->GetTLVPack( poPack1, poPack2 );
	if( iRet )
	{
		return iRet;
	}

	std::map<int,int> vecFieldID1, vecFieldID2;
	poPack1->GetIDMap( &vecFieldID1 );
	poPack2->GetIDMap( &vecFieldID2 );

	std::map<int,int>::iterator pos1 = vecFieldID1.begin(), pos2 = vecFieldID2.begin();

	while( pos1 != vecFieldID1.end() || pos2 != vecFieldID2.end() )
	{
		if( pos1!=vecFieldID1.end() && pos2!=vecFieldID2.end() && pos1->first == pos2->first )
		{
			CTLVPack *poListPack1, *poListPack2 = NULL;
			int iRet1 = poPack1 ->GetNestedTLVBuf ( pos1->first, &poListPack1 );
			int iRet2 = poPack2 ->GetNestedTLVBuf ( pos1->first, &poListPack2 );
			if( iRet1 == 0 && iRet2 == 0 )
			{
				CTLVPack poListBuffer; 

				for ( int itemIndex = 0, ok = 1; !iRet && ok ; ++itemIndex )
				{
					int iGetRet1 = 0, iGetRet2 = 0;
					CTLVPack *poPackItem1 = NULL, *poPackItem2 = NULL;
					iGetRet1 = poListPack1 ->GetNestedTLVBuf ( itemIndex, &poPackItem1 );
					iGetRet2 = poListPack2 ->GetNestedTLVBuf ( itemIndex, &poPackItem2 );
					if( !iGetRet1 && !iGetRet2 )
					{
						SKTLVMergePack oMergeItem;
						//析构了poPackitem1, poPackitem2
						iRet = oMergeItem.SetTLVPack( poPackItem1, poPackItem2 );
						if( iRet == 0 )
						{
							CTLVPack poBuffer; 
							iRet = MergeBuffer( &oMergeItem, &poBuffer );
							if( iRet == 0 )
							{
								iRet = poListBuffer.AddNestedTLV ( itemIndex, &poBuffer ) ;
							}
						}
					}
					else if( !iGetRet1 )
					{
						//		iRet = poListBuffer->AddNestedTLV ( itemIndex, poPackItem1 ) ;
					}
					else if( !iGetRet2 )
					{
						iRet = poListBuffer.AddNestedTLV ( itemIndex, poPackItem2 ) ;
					}
					else
					{
						ok = 0;
					}
					if( iRet )
					{
						ok = 0;
					}

					if( poPackItem1 )
					{
						delete poPackItem1;
						poPackItem1 = NULL;
					}
					if( poPackItem2 )
					{
						delete poPackItem2;
						poPackItem2 = NULL;
					}
				}
				if( iRet == 0 )
					iRet = poTlvBuffer->AddNestedTLV ( pos1->first, &poListBuffer ) ;
			}
			else
			{
				char * pcValue = NULL;
				int iSize = 0;
				iRet = poMergePack->GetBuff( pos1->first, pcValue, iSize );
				if( iRet == 0 )
				{
					iRet = poTlvBuffer->AddBuf( pos1->first, pcValue, iSize );
				}	
				if( pcValue )
				{
					free( pcValue );
					pcValue = NULL;
				}

			}
			if( poListPack1 )
			{
				delete poListPack1;
				poListPack1 = NULL;
			}
			if( poListPack2 )
				{
					delete poListPack2;
					poListPack2 = NULL;
				}

			++pos1;++pos2;
		}
		else
		{
			if( pos2==vecFieldID2.end() || ( pos1!=vecFieldID1.end() && pos1->first < pos2->first ) )
			{
				CTLVPack *poListPack1 = NULL;
				iRet = poPack1 ->GetNestedTLVBuf ( pos1->first, &poListPack1 );
				if( iRet == 0 )
				{
					iRet = poTlvBuffer->AddNestedTLV(pos1->first, poListPack1);
				}
				else
				{
					char * pcValue = NULL;
					int iSize = 0;
					iRet = poMergePack->GetBuff( pos1->first, pcValue, iSize );
					if( iRet == 0 )
					{
						iRet = poTlvBuffer->AddBuf( pos1->first, pcValue, iSize );
					}
					if( pcValue )
					{
						free( pcValue );
						pcValue = NULL;
					}
				}
				if( poListPack1 )
				{
					delete poListPack1;
					poListPack1 = NULL;
				}
				++pos1;
			}
			else
			{
				CTLVPack *poListPack2 = NULL;
				iRet = poPack2 ->GetNestedTLVBuf ( pos2->first, &poListPack2 );
				if( iRet == 0 )
				{
					iRet = poTlvBuffer->AddNestedTLV(pos2->first, poListPack2 );
				}
				else
				{
					char * pcValue = NULL;
					int iSize = 0;
					iRet = poMergePack->GetBuff( pos2->first, pcValue, iSize );
					if( iRet == 0 )
					{
						iRet = poTlvBuffer->AddBuf( pos2->first, pcValue, iSize );
					}
					if( pcValue )
					{
						free( pcValue );
						pcValue = NULL;
					}
				}
				if( poListPack2 )
				{
					delete poListPack2;
					poListPack2 = NULL;
				}
				++pos2;
			}
		}
		if( iRet )
		{
			break;
		}
	}

	return iRet ;
}

static char hostname[128] = {0};

void SKTLVHelper :: GetHostName()
{
#if 0
	if( gethostname(hostname, sizeof( hostname ) ) )
		hostname[0] = 0;
#endif

}

bool SKTLVHelper :: CheckIsMM()
{
#if 0
	if( hostname[0] == 0 )
		GetHostName();
	if( hostname[0] )
		return strncasecmp( hostname, "mm", 2 ) == 0 ;
	return false;
#endif

	return true;

}

}
