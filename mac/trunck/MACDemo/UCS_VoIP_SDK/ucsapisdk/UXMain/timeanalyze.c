
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mach/mach_time.h>
#include <mach/mach.h>

#include "timeanalyze.h"

#pragma mark - local variants

struct TimeAnalyzeItem
{
	unsigned int nID;
	
	unsigned int nHitCount;
	unsigned long long nHitTime;
	
	unsigned int nHotHitCount;
	unsigned long long nHotHitTime;
	unsigned int nHotLostCount;
	unsigned long long nHotLostTime;
	
	unsigned int nZeroValueCount;
	unsigned int nUpValueSum;
	unsigned int nDownValueSum;
	
	unsigned int nLastBeginValue;
	unsigned int nLastEndValue;
	
	unsigned long long nLastBeginTime;
	unsigned long long nLastEndTime;
	
	unsigned int nLastDiff;
	
	char* pszDescription;
};

#define MAX_ITEM_COUNT			0x100
#define ITEM_MASK				0xFF

#define HOTHIT_TIME_INTERVAL	200
	
static struct TimeAnalyzeItem* m_pMainItems[MAX_ITEM_COUNT];
static struct TimeAnalyzeItem* m_pSubItems[MAX_ITEM_COUNT];
static struct TimeAnalyzeItem* m_pExtItems[MAX_ITEM_COUNT];

#pragma mark - Current Time in microsecond


#pragma mark - TimeAnalyze init and dealloc

int TimeAnalyzeItemInit()
{
	int nRet = 0;
	for(unsigned int i=0; i<MAX_ITEM_COUNT; ++i)
	{
		m_pMainItems[i] = NULL;
		m_pSubItems[i] = NULL;
		m_pExtItems[i] = NULL;
	}
	
	nRet = 1;
	return nRet;
}

int TimeAnalyzeItemDealloc()
{
	int nRet = 0;
	for(unsigned int i=0; i<MAX_ITEM_COUNT; ++i)
	{
		if( m_pMainItems[i] )
		{
			if( m_pMainItems[i]->pszDescription )
			{
				free( m_pMainItems[i]->pszDescription );
			}
			free( m_pMainItems[i] );
			m_pMainItems[i] = NULL;
		}
		if( m_pSubItems[i] )
		{
			if( m_pSubItems[i]->pszDescription )
			{
				free( m_pSubItems[i]->pszDescription );
			}
			free( m_pSubItems[i] );
			m_pSubItems[i] = NULL;
		}
		if( m_pExtItems[i] )
		{
			if( m_pExtItems[i]->pszDescription )
			{
				free( m_pExtItems[i]->pszDescription );
			}
			free( m_pExtItems[i] );
			m_pExtItems[i] = NULL;
		}
	}
	nRet = 1;
	return nRet;
}

#pragma mark - TimeAnalyze Begin and End

int TimeAnalyzeBeginMain( unsigned int nID, unsigned int nValue, const char* pszDescription )
{
	int nRet = 0;
	unsigned int nMaskID = nID & ITEM_MASK;
	if( !m_pMainItems[nMaskID] )
	{
		char* pBuffer = (char *)malloc( sizeof(struct TimeAnalyzeItem) );
		memset( pBuffer, 0, sizeof(struct TimeAnalyzeItem));
		m_pMainItems[nMaskID] = (struct TimeAnalyzeItem*)pBuffer;
		
		if( pszDescription && !m_pMainItems[nMaskID]->pszDescription )
		{
			size_t nLen = strlen(pszDescription);
			m_pMainItems[nMaskID]->pszDescription = (char *)malloc(nLen+1);
			memcpy( m_pMainItems[nMaskID]->pszDescription, pszDescription, nLen+1 );
		}
	}
	
	if( m_pMainItems[nMaskID]->nLastBeginTime > 0 && m_pMainItems[nMaskID]->nLastEndTime == 0 )
	{
		++m_pMainItems[nMaskID]->nHotLostCount;
		
		m_pMainItems[nMaskID]->nHotLostTime += ( (GetTimeInMicroSecond()) - m_pMainItems[nMaskID]->nLastBeginTime );
	}
	
	m_pMainItems[nMaskID]->nID = nID;
	m_pMainItems[nMaskID]->nLastBeginTime = (GetTimeInMicroSecond());
	m_pMainItems[nMaskID]->nLastEndTime = 0;
	m_pMainItems[nMaskID]->nLastBeginValue = nValue;
	nRet = 1;
	return nRet;
}

int TimeAnalyzeEndMain( unsigned int nID, unsigned int nValue )
{
	int nRet = 0;
	unsigned int nMaskID = nID & ITEM_MASK;
	unsigned long long nNow = GetTimeInMicroSecond();
	if( m_pMainItems[nMaskID] )
	{
		if( 0 != m_pMainItems[nMaskID]->nLastBeginTime && 0 == m_pMainItems[nMaskID]->nLastEndTime )
		{
			unsigned long long nDiff = nNow - m_pMainItems[nMaskID]->nLastBeginTime;
			++m_pMainItems[nMaskID]->nHitCount;
			m_pMainItems[nMaskID]->nHitTime += nDiff;
			if( nDiff >= HOTHIT_TIME_INTERVAL )
			{
				++m_pMainItems[nMaskID]->nHotHitCount;
				m_pMainItems[nMaskID]->nHotHitTime += nDiff;
			}
			m_pMainItems[nMaskID]->nLastBeginTime = 0;
			m_pMainItems[nMaskID]->nLastEndTime = 0;
			m_pMainItems[nMaskID]->nLastDiff = (unsigned int)nDiff;
			m_pMainItems[nMaskID]->nLastEndValue = nValue;
			
			if( nValue == m_pMainItems[nMaskID]->nLastBeginValue )
			{
				m_pMainItems[nMaskID]->nZeroValueCount++;
			}
			else if( nValue > m_pMainItems[nMaskID]->nLastBeginValue )
			{
				m_pMainItems[nMaskID]->nUpValueSum += ( nValue - m_pMainItems[nMaskID]->nLastBeginValue );
			}
			else
			{
				m_pMainItems[nMaskID]->nUpValueSum += ( m_pMainItems[nMaskID]->nLastBeginValue - nValue );
			}
		}
		else
		{
			++m_pMainItems[nMaskID]->nHotLostTime;
		}
		nRet = 1;
	}
	return nRet;
}

int TimeAnalyzeBeginSub( unsigned int nID, unsigned int nValue, const char* pszDescription )
{
	int nRet = 0;
	
	unsigned int nMaskID = nID & ITEM_MASK;
	
	if( m_pSubItems[nMaskID] )
	{
		if( nID == m_pSubItems[nMaskID]->nID )
		{
			if( m_pSubItems[nMaskID]->nLastBeginTime > 0 && m_pSubItems[nMaskID]->nLastEndTime == 0 )
			{
				++m_pSubItems[nMaskID]->nHotLostCount;
				m_pSubItems[nMaskID]->nHotLostTime += (GetTimeInMicroSecond()) - m_pSubItems[nMaskID]->nLastBeginTime;
			}
			
			m_pSubItems[nMaskID]->nID = nID;
			m_pSubItems[nMaskID]->nLastBeginTime = (GetTimeInMicroSecond());
			m_pSubItems[nMaskID]->nLastEndTime = 0;
			m_pSubItems[nMaskID]->nLastBeginValue = nValue;
			nRet = 1;
		}
		else
		{
			for( unsigned int i=0; i<MAX_ITEM_COUNT; ++i )
			{
				if( m_pSubItems[i] )
				{
					if( nID == m_pSubItems[i]->nID )
					{
						if( m_pSubItems[i]->nLastBeginTime > 0 && m_pSubItems[i]->nLastEndTime == 0 )
						{
							++m_pSubItems[i]->nHotLostCount;
							m_pSubItems[i]->nHotLostTime += ( (GetTimeInMicroSecond()) - m_pSubItems[i]->nLastBeginTime );
						}
						m_pSubItems[i]->nID = nID;
						m_pSubItems[i]->nLastBeginTime = (GetTimeInMicroSecond());
						m_pSubItems[i]->nLastEndTime = 0;
						m_pSubItems[i]->nLastBeginValue = nValue;
						nRet = 1;
						break;
					}
				}
			}
			
			if( !nRet )
			{
				for( unsigned int i=0; i<MAX_ITEM_COUNT; ++i )
				{
					if( !m_pSubItems[i] )
					{
						char* pBuffer = (char *)malloc( sizeof(struct TimeAnalyzeItem) );
						memset( pBuffer, 0, sizeof(struct TimeAnalyzeItem));
						m_pSubItems[i] = (struct TimeAnalyzeItem*)pBuffer;
						m_pSubItems[i]->nID = nID;
						m_pSubItems[i]->nLastBeginTime = (GetTimeInMicroSecond());
						m_pSubItems[i]->nLastEndTime = 0;
						m_pSubItems[i]->nLastBeginValue = nValue;
						
						if( pszDescription && !m_pSubItems[i]->pszDescription )
						{
							size_t nLen = strlen(pszDescription);
							m_pSubItems[i]->pszDescription = (char *)malloc(nLen+1);
							memcpy( m_pSubItems[i]->pszDescription, pszDescription, nLen+1 );
						}
						nRet = 1;
						break;
					}
				}
			}
		}
	}
	else
	{
		char* pBuffer = (char *)malloc( sizeof(struct TimeAnalyzeItem) );
		memset( pBuffer, 0, sizeof(struct TimeAnalyzeItem));
		m_pSubItems[nMaskID] = (struct TimeAnalyzeItem*)pBuffer;
		m_pSubItems[nMaskID]->nID = nID;
		m_pSubItems[nMaskID]->nLastBeginTime = (GetTimeInMicroSecond());
		m_pSubItems[nMaskID]->nLastEndTime = 0;
		m_pSubItems[nMaskID]->nLastBeginValue = nValue;
		
		if( pszDescription && !m_pSubItems[nMaskID]->pszDescription )
		{
			size_t nLen = strlen(pszDescription);
			m_pSubItems[nMaskID]->pszDescription = (char *)malloc(nLen+1);
			memcpy( m_pSubItems[nMaskID]->pszDescription, pszDescription, nLen+1 );
		}
		nRet = 1;
	}
	
	return nRet;
}

int TimeAnalyzeEndSub( unsigned int nID, unsigned int nValue )
{
	int nRet = 0;
	
	unsigned long long nNow = GetTimeInMicroSecond();
	unsigned int nMaskID = nID & ITEM_MASK;
	
	if( m_pSubItems[nMaskID] )
	{
		if( nID == m_pSubItems[nMaskID]->nID )
		{
			unsigned long long nDiff = nNow - m_pSubItems[nMaskID]->nLastBeginTime;
			
			++m_pSubItems[nMaskID]->nHitCount;
			m_pSubItems[nMaskID]->nHitTime += nDiff;
			if( nDiff >= HOTHIT_TIME_INTERVAL )
			{
				++m_pSubItems[nMaskID]->nHotHitCount;
				m_pSubItems[nMaskID]->nHotHitTime += nDiff;
			}
			m_pSubItems[nMaskID]->nLastBeginTime = 0;
			m_pSubItems[nMaskID]->nLastEndTime = 0;
			m_pSubItems[nMaskID]->nLastDiff = (unsigned int)nDiff;
			m_pSubItems[nMaskID]->nLastEndValue = nValue;
			
			if( nValue == m_pSubItems[nMaskID]->nLastBeginValue )
			{
				m_pSubItems[nMaskID]->nZeroValueCount++;
			}
			else if( nValue > m_pSubItems[nMaskID]->nLastBeginValue )
			{
				m_pSubItems[nMaskID]->nUpValueSum += ( nValue - m_pSubItems[nMaskID]->nLastBeginValue );
			}
			else
			{
				m_pSubItems[nMaskID]->nUpValueSum += ( m_pSubItems[nMaskID]->nLastBeginValue - nValue );
			}
			
			nRet = 1;
		}
	}
	
	if( !nRet )
	{
		for( unsigned int i=0; i<MAX_ITEM_COUNT; ++i )
		{
			if( m_pSubItems[i] )
			{
				if( nID == m_pSubItems[i]->nID )
				{
					unsigned long long nDiff = nNow - m_pSubItems[i]->nLastBeginTime;
					
					++m_pSubItems[i]->nHitCount;
					m_pSubItems[i]->nHitTime += nDiff;
					if( nDiff >= HOTHIT_TIME_INTERVAL )
					{
						++m_pSubItems[i]->nHotHitCount;
						m_pSubItems[i]->nHotHitTime += nDiff;
					}
					m_pSubItems[i]->nLastBeginTime = 0;
					m_pSubItems[i]->nLastEndTime = 0;
					m_pSubItems[i]->nLastDiff = (unsigned int)nDiff;
					m_pSubItems[i]->nLastEndValue = nValue;
					
					if( nValue == m_pSubItems[i]->nLastBeginValue )
					{
						m_pSubItems[i]->nZeroValueCount++;
					}
					else if( nValue > m_pSubItems[i]->nLastBeginValue )
					{
						m_pSubItems[i]->nUpValueSum += ( nValue - m_pSubItems[i]->nLastBeginValue );
					}
					else
					{
						m_pSubItems[i]->nUpValueSum += ( m_pSubItems[i]->nLastBeginValue - nValue );
					}
					nRet = 1;
				}
			}
		}
	}
	
	return nRet;
}

int TimeAnalyzeBeginExt( unsigned int nID, unsigned int nValue, const char* pszDescription )
{
	int nRet = 0;
	
	unsigned int nMaskID = nID & ITEM_MASK;
	
	if( m_pExtItems[nMaskID] )
	{
		if( nID == m_pExtItems[nMaskID]->nID )
		{
			if( m_pExtItems[nMaskID]->nLastBeginTime > 0 && m_pExtItems[nMaskID]->nLastEndTime == 0 )
			{
				++m_pExtItems[nMaskID]->nHotLostCount;
				m_pExtItems[nMaskID]->nHotLostTime += ((GetTimeInMicroSecond()) - m_pExtItems[nMaskID]->nLastBeginTime);
			}
			
			m_pExtItems[nMaskID]->nID = nID;
			m_pExtItems[nMaskID]->nLastBeginTime = (GetTimeInMicroSecond());
			m_pExtItems[nMaskID]->nLastEndTime = 0;
			m_pExtItems[nMaskID]->nLastBeginValue = nValue;
			nRet = 1;
		}
		else
		{
			for( unsigned int i=0; i<MAX_ITEM_COUNT; ++i )
			{
				if( m_pExtItems[i] )
				{
					if( nID == m_pExtItems[i]->nID )
					{
						if( m_pExtItems[i]->nLastBeginTime > 0 && m_pExtItems[i]->nLastEndTime == 0 )
						{
							++m_pExtItems[i]->nHotLostCount;
							m_pExtItems[i]->nHotLostTime += (GetTimeInMicroSecond()) - m_pExtItems[i]->nLastBeginTime;
						}
						m_pExtItems[i]->nID = nID;
						m_pExtItems[i]->nLastBeginTime = (GetTimeInMicroSecond());
						m_pExtItems[i]->nLastEndTime = 0;
						m_pExtItems[i]->nLastBeginValue = nValue;
						nRet = 1;
						break;
					}
				}
			}
			
			if( !nRet )
			{
				for( unsigned int i=0; i<MAX_ITEM_COUNT; ++i )
				{
					if( !m_pExtItems[i] )
					{
						char* pBuffer = (char *)malloc( sizeof(struct TimeAnalyzeItem) );
						memset( pBuffer, 0, sizeof(struct TimeAnalyzeItem));
						m_pExtItems[i] = (struct TimeAnalyzeItem*)pBuffer;
						m_pExtItems[i]->nID = nID;
						m_pExtItems[i]->nLastBeginTime = (GetTimeInMicroSecond());
						m_pExtItems[i]->nLastEndTime = 0;
						m_pExtItems[i]->nLastBeginValue = nValue;
						
						if( pszDescription && !m_pExtItems[i]->pszDescription )
						{
							size_t nLen = strlen(pszDescription);
							m_pExtItems[i]->pszDescription = (char *)malloc(nLen+1);
							memcpy( m_pExtItems[i]->pszDescription, pszDescription, nLen+1 );
						}
						
						nRet = 1;
						break;
					}
				}
			}
		}
	}
	else
	{
		char* pBuffer = (char *)malloc( sizeof(struct TimeAnalyzeItem) );
		memset( pBuffer, 0, sizeof(struct TimeAnalyzeItem));
		m_pExtItems[nMaskID] = (struct TimeAnalyzeItem*)pBuffer;
		m_pExtItems[nMaskID]->nID = nID;
		m_pExtItems[nMaskID]->nLastBeginTime = (GetTimeInMicroSecond());
		m_pExtItems[nMaskID]->nLastEndTime = 0;
		m_pExtItems[nMaskID]->nLastBeginValue = nValue;
		
		if( pszDescription && !m_pExtItems[nMaskID]->pszDescription )
		{
			size_t nLen = strlen(pszDescription);
			m_pExtItems[nMaskID]->pszDescription = (char *)malloc(nLen+1);
			memcpy( m_pExtItems[nMaskID]->pszDescription, pszDescription, nLen+1 );
		}
		
		nRet = 1;
	}
	
	return nRet;
}

int TimeAnalyzeEndExt( unsigned int nID, unsigned int nValue )
{
	int nRet = 0;
	
	unsigned long long nNow = (GetTimeInMicroSecond());
	unsigned int nMaskID = nID & ITEM_MASK;
	
	if( m_pExtItems[nMaskID] )
	{
		if( nID == m_pExtItems[nMaskID]->nID )
		{
			unsigned long long nDiff = nNow - m_pExtItems[nMaskID]->nLastBeginTime;
			
			++m_pExtItems[nMaskID]->nHitCount;
			m_pExtItems[nMaskID]->nHitTime += nDiff;
			if( nDiff >= HOTHIT_TIME_INTERVAL )
			{
				++m_pExtItems[nMaskID]->nHotHitCount;
				m_pExtItems[nMaskID]->nHotHitTime += nDiff;
			}
			m_pExtItems[nMaskID]->nLastBeginTime = 0;
			m_pExtItems[nMaskID]->nLastEndTime = 0;
			m_pExtItems[nMaskID]->nLastDiff = (unsigned int)nDiff;
			m_pExtItems[nMaskID]->nLastEndValue = nValue;
			
			if( nValue == m_pExtItems[nMaskID]->nLastBeginValue )
			{
				m_pExtItems[nMaskID]->nZeroValueCount++;
			}
			else if( nValue > m_pExtItems[nMaskID]->nLastBeginValue )
			{
				m_pExtItems[nMaskID]->nUpValueSum += ( nValue - m_pExtItems[nMaskID]->nLastBeginValue );
			}
			else
			{
				m_pExtItems[nMaskID]->nUpValueSum += ( m_pExtItems[nMaskID]->nLastBeginValue - nValue );
			}
			nRet = 1;
		}
	}
	
	if( !nRet )
	{
		for( unsigned int i=0; i<MAX_ITEM_COUNT; ++i )
		{
			if( m_pExtItems[i] )
			{
				if( nID == m_pExtItems[i]->nID )
				{
					unsigned long long nDiff = nNow - m_pExtItems[i]->nLastBeginTime;
					
					++m_pExtItems[i]->nHitCount;
					m_pExtItems[i]->nHitTime += nDiff;
					if( nDiff >= HOTHIT_TIME_INTERVAL )
					{
						++m_pExtItems[i]->nHotHitCount;
						m_pExtItems[i]->nHotHitTime += nDiff;
					}
					m_pExtItems[i]->nLastBeginTime = 0;
					m_pExtItems[i]->nLastEndTime = 0;
					m_pExtItems[i]->nLastDiff = (unsigned int)nDiff;
					m_pExtItems[i]->nLastEndValue = nValue;
					
					if( nValue == m_pExtItems[i]->nLastBeginValue )
					{
						m_pExtItems[i]->nZeroValueCount++;
					}
					else if( nValue > m_pExtItems[i]->nLastBeginValue )
					{
						m_pExtItems[i]->nUpValueSum += ( nValue - m_pExtItems[i]->nLastBeginValue );
					}
					else
					{
						m_pExtItems[i]->nUpValueSum += ( m_pExtItems[i]->nLastBeginValue - nValue );
					}
					nRet = 1;
				}
			}
		}
	}
	
	return nRet;
}

#pragma mark - TimeAnalyze is timeout or lost

int TimeAnalyzeIsMainTimeOut( unsigned int nID, unsigned int nTimeOutCount /*= 2000*/ )
{
	int nRet = 0;
	
	if( m_pMainItems[nID] )
	{
		if( 0 == m_pMainItems[nID]->nLastBeginTime && 0 == m_pMainItems[nID]->nLastEndTime )
		{
			if( m_pMainItems[nID]->nLastDiff >= nTimeOutCount )
			{
				nRet = 1;
			}
		}
	}
	
	return nRet;
}

int TimeAnalyzeIsMainLost( unsigned int nID )
{
	int nRet = 0;
	
	if( m_pMainItems[nID] )
	{
		if( m_pMainItems[nID]->nLastBeginTime > 0 && 0 == m_pMainItems[nID]->nLastEndTime )
		{
			nRet = 1;
		}
	}
	
	return nRet;
}

int TimeAnalyzeIsSubTimeOut( unsigned int nID, unsigned int nTimeOutCount /*= 2000*/ )
{
	int nRet = 0;
	
	if( m_pSubItems[nID] )
	{
		if( 0 == m_pSubItems[nID]->nLastBeginTime && 0 == m_pSubItems[nID]->nLastEndTime )
		{
			if( m_pSubItems[nID]->nLastDiff >= nTimeOutCount )
			{
				nRet = 1;
			}
		}
	}
	
	return nRet;
}

int TimeAnalyzeIsSubLost( unsigned int nID )
{
	int nRet = 0;
	
	if( m_pSubItems[nID] )
	{
		if( m_pSubItems[nID]->nLastBeginTime > 0 && 0 == m_pSubItems[nID]->nLastEndTime )
		{
			nRet = 1;
		}
	}
	
	return nRet;
}

int TimeAnalyzeIsExtTimeOut( unsigned int nID, unsigned int nTimeOutCount /*= 2000*/ )
{
	int nRet = 0;
	
	if( m_pExtItems[nID] )
	{
		if( 0 == m_pExtItems[nID]->nLastBeginTime && 0 == m_pExtItems[nID]->nLastEndTime )
		{
			if( m_pExtItems[nID]->nLastDiff >= nTimeOutCount )
			{
				nRet = 1;
			}
		}
	}
	
	return nRet;
}

int TimeAnalyzeIsExtLost( unsigned int nID )
{
	int nRet = 0;
	
	if( m_pExtItems[nID] )
	{
		if( m_pExtItems[nID]->nLastBeginTime > 0 && 0 == m_pExtItems[nID]->nLastEndTime )
		{
			nRet = 1;
		}
	}
	
	return nRet;
}

#pragma mark - TimeAnalyze Save

//#import <Foundation/Foundation.h>
//
//int TimeAnalyzeSaveAllData()
//{
////#ifdef NEED_TIME_ANALYZE
//    {
//        NSString *strDoc = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
//        NSString* strMainFileName = [NSString stringWithFormat:@"%@/main.txt", strDoc ];
//        NSString* strSubFileName = [NSString stringWithFormat:@"%@/sub.txt", strDoc ];
//        NSString* strExtFileName = [NSString stringWithFormat:@"%@/ext.txt", strDoc ];
//
//        const char* pszMainFileName = [strMainFileName UTF8String];
//        const char* pszSubFileName = [strSubFileName UTF8String];
//        const char* pszExtFileName = [strExtFileName UTF8String];
//
//        TimeAnalyzeSaveData( pszMainFileName, pszSubFileName, pszExtFileName );
//    }
////#endif
//}

int TimeAnalyzeSaveData( const char* pszMainFileName, const char* pszSubFileName, const char* pszExtFileName )
{
	int nRet = 0;
	
#ifdef NEED_TIME_ANALYZE
    
	if( pszMainFileName )
	{
		FILE* pf = fopen( pszMainFileName, "w" );
		if( pf )
		{
			fprintf( pf, "\n[id]        hit      time     avg    hot     time     avg  lost");
			for( unsigned int i = 0; i<MAX_ITEM_COUNT; ++i )
			{
				if( m_pMainItems[i] )
				{
					//m_pMainItems[i]
                    //if( m_pMainItems[i]->nHitTime > 0 )
                    {
                        fprintf( pf, "\n[%3d:%3d]%8d %8llu %6llu %5d %8llu %6llu %4d %s",
                                i,
                                m_pMainItems[i]->nID,
                                m_pMainItems[i]->nHitCount,
                                m_pMainItems[i]->nHitTime,
                                0 != m_pMainItems[i]->nHitCount ? m_pMainItems[i]->nHitTime / m_pMainItems[i]->nHitCount : 0,
                                m_pMainItems[i]->nHotHitCount,
                                m_pMainItems[i]->nHotHitTime,
                                0 != m_pMainItems[i]->nHotHitCount ? m_pMainItems[i]->nHotHitTime / m_pMainItems[i]->nHotHitCount : 0,
                                m_pMainItems[i]->nHotLostCount,
                                m_pMainItems[i]->pszDescription );
                    }
				}
			}
			fclose( pf );
		}
		else
		{
			printf( "open file error! %s", pszMainFileName );
		}
	}
	
	if( pszSubFileName )
	{
		FILE* pf2 = fopen( pszSubFileName, "w" );
		if( pf2 )
		{
			fprintf( pf2, "\n[id]        hit      time     avg    hot     time     avg  lost");
			for( unsigned int i = 0; i<MAX_ITEM_COUNT; ++i )
			{
				if( m_pSubItems[i] )
				{
					//m_pSubItems[i]
                    fprintf( pf2, "\n[%3d:%3d]%8d %8llu %6llu %5d %8llu %6llu %4d %s",
                            i,
                            m_pSubItems[i]->nID,
                            m_pSubItems[i]->nHitCount,
                            m_pSubItems[i]->nHitTime,
                            0 != m_pSubItems[i]->nHitCount ? m_pSubItems[i]->nHitTime / m_pSubItems[i]->nHitCount : 0,
                            m_pSubItems[i]->nHotHitCount,
                            m_pSubItems[i]->nHotHitTime,
                            0 != m_pSubItems[i]->nHotHitCount ? m_pSubItems[i]->nHotHitTime / m_pSubItems[i]->nHotHitCount : 0,
                            m_pSubItems[i]->nHotLostCount,
                            m_pSubItems[i]->pszDescription );
				}
			}
			fclose( pf2 );
		}
		else
		{
			printf( "open sub file error! %s", pszSubFileName );
		}
	}
	
	if( pszExtFileName )
	{
		FILE* pf3 = fopen( pszExtFileName, "w" );
		if( pf3 )
		{
			for( unsigned int i = 0; i<MAX_ITEM_COUNT; ++i )
			{
				if( m_pExtItems[i] )
				{
					//m_pExtItems[i]
					fprintf( pf3, "\n%4d%8d%8d%8llu%8d%8llu%8d%8llu%8d%8d%8d%8d%s",
							i,
							m_pExtItems[i]->nID,
							m_pExtItems[i]->nHitCount,
							m_pExtItems[i]->nHitTime,
							m_pExtItems[i]->nHotHitCount,
							m_pExtItems[i]->nHotHitTime,
							m_pExtItems[i]->nHotLostCount,
							m_pExtItems[i]->nHotLostTime,
							m_pExtItems[i]->nLastDiff,
							m_pExtItems[i]->nZeroValueCount,
							m_pExtItems[i]->nUpValueSum,
							m_pExtItems[i]->nDownValueSum,
							m_pExtItems[i]->pszDescription );
				}
			}
			fclose( pf3 );
		}
		else
		{
			printf( "open ext file error! %s", pszExtFileName );
		}
		
	}
	nRet = 1;
#endif
    
	return nRet;
}

int WarnLog( const char* pszFormat, ... )
{
	int nRet = 0;
#ifdef NEED_TIME_ANALYZE
	//	if( pszFormat )
	{
		va_list argList;
		va_start(argList, pszFormat);
		vfprintf( stdout, pszFormat, argList );
		va_end(argList);
		nRet = 1;
	}
#endif
	return nRet;
}
