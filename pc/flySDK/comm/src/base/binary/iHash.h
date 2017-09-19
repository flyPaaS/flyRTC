



#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define HashTableEnd -1
namespace Comm
{

template< typename Key_t, typename Val_t>
class CHashTable
{

private:

	enum
	{
		NorMal, //一维哈希表
		NonNorMal // 二维哈希表 动态增加行数
	};

const static int s_iBufPerSize = 1024;

public:
	typedef unsigned int (*HashFunc)( const Key_t &key ); 

	CHashTable(const HashFunc &HFunc, const int &Mode = NorMal)
	{
		m_ptHashTable = NULL;
		m_piHashTalbePtr = NULL;
		m_iUsedSize = NULL;
		m_iHashMode = Mode;
		m_iRowSize = 0;
		m_iColSize = 0;

		m_pHFunc = HFunc;
	}

	~CHashTable()
	{
		Clear();
	/*
		for( int i=0; i < m_iRowSize; ++i )
		{
			if( m_ptHashTable  != NULL )
			{
				if( m_ptHashTable[i] != NULL )
				{
					free (m_ptHashTable[i]);
					m_ptHashTable[i] = NULL;
				}
			}

			if( m_piHashTalbePtr != NULL )
			{
				if( m_piHashTalbePtr[i] != NULL )
				{
					free (m_piHashTalbePtr[i]);
					m_piHashTalbePtr[i] = NULL;
				}
			}
		}
		
		if( m_iUsedSize != NULL )
		{
			free( m_iUsedSize );
			m_iUsedSize = NULL;
		}

		if( m_ptHashTable != NULL )
		{
			free( m_ptHashTable );
			m_ptHashTable = NULL;
		}
		
		if( m_piHashTalbePtr != NULL )
		{
			free( m_piHashTalbePtr );
			m_piHashTalbePtr = NULL;
		}
*/
		m_iHashMode = 0;
	}
	
	int Clear()
	{
		for( int i=0; i < m_iRowSize; ++i )
		{
			if( m_ptHashTable  != NULL )
			{
				if( m_ptHashTable[i] != NULL )
				{
					free (m_ptHashTable[i]);
					m_ptHashTable[i] = NULL;
				}
			}

			if( m_piHashTalbePtr != NULL )
			{
				if( m_piHashTalbePtr[i] != NULL )
				{
					free (m_piHashTalbePtr[i]);
					m_piHashTalbePtr[i] = NULL;
				}
			}
		}
		
		if( m_iUsedSize != NULL )
		{
			free( m_iUsedSize );
			m_iUsedSize = NULL;
		}

		if( m_ptHashTable != NULL )
		{
			free( m_ptHashTable );
			m_ptHashTable = NULL;
		}
		
		if( m_piHashTalbePtr != NULL )
		{
			free( m_piHashTalbePtr );
			m_piHashTalbePtr = NULL;
		}
		return 0;
	}

	int Init(const int &TableSize=100)
	{
		if( m_iHashMode == NorMal )
		{
			m_iRowSize = 1;
			m_iColSize = TableSize;
			m_iTableSize = TableSize;

			m_ptHashTable = (HashTable_t**)calloc(sizeof(HashTable_t*)*m_iRowSize,1);
			m_piHashTalbePtr = (int**)calloc(sizeof(int*)*m_iRowSize,1);
			m_iUsedSize = (int *)calloc(sizeof(int)*m_iRowSize,1);

			for(int i=0;i<m_iRowSize;++i)
			{
				m_ptHashTable[i] = (HashTable_t*)malloc(m_iColSize*sizeof(HashTable_t));
				m_piHashTalbePtr[i] = (int *)malloc(sizeof(int)*m_iColSize);
				memset( m_piHashTalbePtr[i], -1, sizeof(int)*m_iColSize );
			}

		}
		else
		{
			m_iColSize = s_iBufPerSize;
			m_iRowSize = 1;
			m_iTableSize = m_iColSize * m_iRowSize;
				
			m_ptHashTable = (HashTable_t**)calloc(sizeof(HashTable_t*)*m_iRowSize,1);
			m_piHashTalbePtr = (int**)calloc(sizeof(int*)*m_iRowSize,1);
			m_iUsedSize = (int *)calloc(sizeof(int)*m_iRowSize,1);
		}
		return 0;
	}
	
	void Extend(const int &NewSize)
	{
		int **piHashTalbePtr = (int**)malloc(sizeof(int*)*NewSize);
		memcpy(piHashTalbePtr,m_piHashTalbePtr,m_iRowSize*(sizeof(int*)));

		free(m_piHashTalbePtr);
		m_piHashTalbePtr = piHashTalbePtr;

		memset(m_piHashTalbePtr+m_iRowSize,0,(NewSize-m_iRowSize)*sizeof(int*));

		HashTable_t ** ptHashTable = (HashTable_t**)malloc(sizeof(HashTable_t*)*NewSize);
		memcpy(ptHashTable,m_ptHashTable,m_iRowSize*sizeof(HashTable_t*));

		free(m_ptHashTable);
		m_ptHashTable = ptHashTable;	
		memset(m_ptHashTable+m_iRowSize,0,(NewSize-m_iRowSize)*sizeof(HashTable_t*));

		int *piUsedSize = (int *)malloc(sizeof(int)*NewSize);
		memcpy(piUsedSize,m_iUsedSize,m_iRowSize*sizeof(int));

		free(m_iUsedSize);
		m_iUsedSize=piUsedSize;
		memset(piUsedSize+m_iRowSize,0,(NewSize-m_iRowSize)*sizeof(int));
	
		m_iRowSize = NewSize;
	}

	int Insert(const Key_t &key,const Val_t &val)
	{
		int hashval;
		if( m_iHashMode == NorMal )
		{
			if( m_iTableSize == 0 )
				return -1;
			hashval = m_pHFunc(key)%m_iTableSize;
		}
		else
			hashval = m_pHFunc(key);
		return Addkey(key, hashval,val);
	}

	int Find(const Key_t &key,Val_t &val)
	{
		int hashval;
		if( m_iHashMode == NorMal )
		{
			if( m_iTableSize == 0 )
				return -1;
			hashval = m_pHFunc(key)%m_iTableSize;
		}
		else
			hashval = m_pHFunc(key);
		return Findkey(key, hashval,val);
	}
	
	int GetTableSize()
	{
		return m_iTableSize;
	}

protected:

	typedef struct tagHashTable
	{
		Key_t key;
		Val_t val;
		int next;
		tagHashTable(){}
		tagHashTable(const Key_t &key,const Val_t &val,int next):key(key),val(val),next(next){}
	}HashTable_t;


protected:

	int Addkey(const Key_t &key, const int &hashkey,const Val_t &val)
	{
		if( m_iColSize == 0 )
		{
			return -1;
		}

		int lev = hashkey/m_iColSize;
		if( lev < 0 )
		{
			return -1;
		}

		if( lev >= m_iRowSize )
		{
			int NewSize = m_iRowSize;

			while(lev >= NewSize)NewSize<<=1;
			Extend(NewSize);
		}

		if( m_iColSize == 0 )
		{
			return -1;
		}

		int tHashkey = hashkey%m_iColSize;

		if( m_piHashTalbePtr[lev] == NULL )
		{
			m_ptHashTable[lev] = (HashTable_t*)malloc(m_iColSize*sizeof(HashTable_t));
			m_piHashTalbePtr[lev] = (int *)malloc(sizeof(int)*m_iColSize);
			memset(m_piHashTalbePtr[lev],-1,m_iColSize*sizeof(int));
			m_iUsedSize[lev]=0;
		}

		if( m_iUsedSize[lev] < 0 || m_iUsedSize[lev] >= m_iColSize )
		{
			return -1;
		}

		m_ptHashTable[lev][m_iUsedSize[lev]]=HashTable_t(key,val,m_piHashTalbePtr[lev][tHashkey]);
		m_piHashTalbePtr[lev][tHashkey]=m_iUsedSize[lev]++;

		return 0;
	}

	int Findkey(const Key_t &key,const int &hashkey, Val_t &val)
	{
		int lev = hashkey/m_iColSize;
		if( m_iColSize == 0 )
		{
			return -1;
		}

		int tHashkey = hashkey%m_iColSize;

		if( lev < 0 || lev >= m_iRowSize || m_piHashTalbePtr[lev] == NULL )return -1;

		for(int i=m_piHashTalbePtr[lev][tHashkey];i!=HashTableEnd;i=m_ptHashTable[lev][i].next)
		{
			if( m_ptHashTable[lev][i].key == key )
			{
				val = m_ptHashTable[lev][i].val;
				return 0;
			}
		}

		return -1;
	}
	
private:
	HashTable_t **m_ptHashTable;
	int m_iTableSize;
	int **m_piHashTalbePtr;
	int *m_iUsedSize;
	int m_iColSize;
	int m_iRowSize;
	int m_iHashMode;
	HashFunc m_pHFunc;
};



}
