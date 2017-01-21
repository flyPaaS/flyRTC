
#pragma once

#include <string>
#include <vector>
#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

template < class ValType_t, typename ValTypeImpl_t  >
class SKVector
{
	public:
		SKVector();
		~SKVector();

		void Set( ValTypeImpl_t *ItemValue, const int &iSize );
		void Set( ValTypeImpl_t **ItemValue, int *iSize );

		void ReSet();
		void Clear();
		void Append( const ValType_t &Item );

		//for array
		void SetVal( const int &iIndex, const ValType_t & Item );

		const ValType_t &operator [] ( const int iIndex )const;
		ValType_t &operator [] ( const int iIndex );
		SKVector<ValType_t,ValTypeImpl_t> &operator = ( const SKVector<ValType_t,ValTypeImpl_t> & vValue );

		int Size()const;
		const std::vector<ValType_t> * GetList() const;

		typedef typename std::vector< ValType_t > :: iterator iterator;
		typedef typename std::vector< ValType_t > :: const_iterator const_iterator;

		iterator & begin();
		iterator & end();

		const_iterator & begin()const;
		const_iterator & end()const;

	private:
		int Recalloc();

	private:
		int m_isArray;

		int m_iArrayDataLen;// for array
		ValTypeImpl_t * m_ptArrayPtr;

		ValTypeImpl_t **m_pBuff;
		int *m_piDataLen;
		int m_iBuffLen;
		std::vector< ValType_t > ClassBuff;
};

template < class ValType_t  >
class SKVector4BaseType
{
	public:
		SKVector4BaseType();
		~SKVector4BaseType();

		void ReSet();
		void Clear();

		void Append( const ValType_t & Item );

		void Set( ValType_t *ItemValue, const int &iSize );

		void Set( ValType_t **ItemValue, int *iSize );

		void SetVal( const int &iIndex, const ValType_t &Item );

		const ValType_t & operator [] ( const int iIndex )const;

		ValType_t & operator [] ( const int iIndex );

		SKVector4BaseType<ValType_t> &operator = ( const SKVector4BaseType<ValType_t> & vValue );

		const std::vector<ValType_t> * GetList() const;

		int Size()const;

		typedef typename std::vector< ValType_t > :: iterator iterator;
		typedef typename std::vector< ValType_t > :: const_iterator const_iterator;

		iterator & begin();
		iterator & end();

		const_iterator & begin()const;
		const_iterator & end()const;

	private:	
		int Recalloc();

	private:
		int m_isArray;

		int *m_piDataLen;
		ValType_t * m_ptArrayPtr;
		int m_iArrayDataLen;// for array
		int m_iBuffLen;
		ValType_t **m_pBuff;
		std::vector< ValType_t > ClassBuff;
};

typedef SKVector4BaseType<char> SKVector_char;
typedef SKVector4BaseType<unsigned char> SKVector_uchar;
typedef SKVector4BaseType<short> SKVector_int8;
typedef SKVector4BaseType<unsigned short> SKVector_uint8;
typedef SKVector4BaseType<int> SKVector_int32;
typedef SKVector4BaseType<unsigned int> SKVector_uint32;
typedef SKVector4BaseType<long long> SKVector_int64;
typedef SKVector4BaseType<unsigned long long> SKVector_uint64;

class SKString
{
	public:
		SKString();
		SKString( char ** sValue, int * iSize = NULL );
		SKString( char * sValue, int * iSize = NULL );
		~SKString();
		void ReSet();

		void Append( const char & cValue );
		void Append( const char * pcValue, const int &iSize = -1 );

		void Set( char ** pcValue, int *iSize = NULL );
		int Size()const;
		const char * GetString()const;
		char * GetString();

		SKString &operator = ( const char *sValue );
		SKString &operator = ( const SKString &sValue );
		const char &operator[] ( const int &iIndex )const;
		char &operator[] ( const int &iIndex );

		bool operator == ( const SKString &sString );
		bool operator < ( const SKString &sString );
		bool operator > ( const SKString &sString );
		bool operator <= ( const SKString &sString );
		bool operator >= ( const SKString &sString );

		static int cmp ( const SKString sString1, const SKString &sString2 );

	private:
		int CopyFrom( const char * pcValue, const int &iSize );
		int *m_piBuffLen;
		char **m_pcBuff;
		char *m_pcTmpBuff;
		int m_iTmpLen;
};

template< class ValType_t, typename ValTypeImpl_t >
SKVector<ValType_t, ValTypeImpl_t> :: SKVector()
{
	m_pBuff = 0;
	m_iBuffLen = 0;
	m_piDataLen = 0;
	m_isArray = 0;
}

template< class ValType_t, typename ValTypeImpl_t >
SKVector<ValType_t, ValTypeImpl_t> :: ~SKVector()
{
}	

template< class ValType_t, typename ValTypeImpl_t >
void SKVector<ValType_t, ValTypeImpl_t> :: ReSet()
{
	m_pBuff = 0;
	m_iBuffLen = 0;
	m_piDataLen = 0;
	m_isArray = 0;
	ClassBuff.clear();	
}

template< class ValType_t, typename ValTypeImpl_t >
void SKVector<ValType_t, ValTypeImpl_t> :: Clear()
{
	if( m_piDataLen != NULL )
		*m_piDataLen = 0;

	if( !m_isArray )
	{
		m_iBuffLen = 0;
		ClassBuff.clear();	
	}

}	

template< class ValType_t, typename ValTypeImpl_t >
void SKVector<ValType_t, ValTypeImpl_t> :: Append( const ValType_t &Item )
{
	if( m_piDataLen != NULL && *m_piDataLen == m_iBuffLen )
	{
		int ret = Recalloc( );
		if( ret < 0 )
			return ;
	}

	ValType_t newItem = Item;

	ClassBuff.push_back( newItem );

	if( m_pBuff != NULL && *m_pBuff != NULL )
	{
		ValTypeImpl_t * ptr = newItem.DetachImpl();

		printf("%p ptr\n",ptr);
		memcpy( &( ( *m_pBuff )[ ( *m_piDataLen )++  ] ), ptr, sizeof( ValTypeImpl_t ) );

		free( ptr );
		ptr = NULL;

	}
}

template< class ValType_t, typename ValTypeImpl_t >
void SKVector<ValType_t, ValTypeImpl_t> :: Set( ValTypeImpl_t *ItemValue, const int &iSize )
{
	//Set For Array

	if( m_pBuff != NULL && *m_pBuff != NULL && *m_pBuff != ItemValue && !m_isArray )
	{
		free(*m_pBuff);
	}

	m_isArray = 1;
	m_ptArrayPtr = ItemValue;
	m_pBuff = &m_ptArrayPtr;

	m_piDataLen = &m_iArrayDataLen;
	m_iArrayDataLen = 0;
	m_iBuffLen = iSize;

	ClassBuff.clear();
	for( int i = 0; i < iSize; ++i )
	{
		ValType_t temp = (*m_pBuff)[i];
		ClassBuff.push_back( temp ); 
	}

}

template< class ValType_t, typename ValTypeImpl_t >
void SKVector<ValType_t, ValTypeImpl_t> :: Set( ValTypeImpl_t **ItemValue, int *iSize )
{
	if( m_pBuff != NULL && *m_pBuff != NULL && m_pBuff != ItemValue && !m_isArray )
	{
		free(*m_pBuff);
	}

	m_isArray = 0;
	m_pBuff = ItemValue;
	m_piDataLen = iSize;
	m_iBuffLen = *iSize;

	ClassBuff.clear();
	if( m_piDataLen != NULL )
	{
		for( int i = 0; i < *m_piDataLen; ++i )
		{
			ValType_t temp = (*m_pBuff)[i];
			ClassBuff.push_back( temp ); 
		}
	}

}

template< class ValType_t, typename ValTypeImpl_t >
void SKVector<ValType_t, ValTypeImpl_t> :: SetVal( const int &iIndex, const ValType_t & Item )
{
	if( iIndex >= m_iBuffLen )
	{
		assert( m_iBuffLen < iIndex );
		return ;
	}

	if( m_pBuff != NULL && *m_pBuff != NULL )
	{
		ValType_t newItem = Item;

		ClassBuff[iIndex] = newItem;

		ValTypeImpl_t * ptr = newItem.DetachImpl();

		memcpy( &( ( *m_pBuff )[ iIndex ] ), ptr, sizeof( ValTypeImpl_t ) );

		free( ptr );
		ptr = NULL;

	}

}

template< class ValType_t, typename ValTypeImpl_t >
const ValType_t &SKVector<ValType_t, ValTypeImpl_t> :: operator [] ( const int iIndex )const
{
	return ClassBuff[iIndex];
}


template< class ValType_t, typename ValTypeImpl_t >
ValType_t & SKVector<ValType_t, ValTypeImpl_t> :: operator [] ( const int iIndex )
{
	return ClassBuff[iIndex];
}

template< class ValType_t, typename ValTypeImpl_t >
SKVector<ValType_t,ValTypeImpl_t> & SKVector<ValType_t, ValTypeImpl_t> :: operator = ( const SKVector<ValType_t,ValTypeImpl_t> & vValue )
{
	if( m_pBuff != NULL && *m_pBuff != NULL && !m_isArray )
	{
		free ( *m_pBuff );
	}

	if( m_piDataLen != NULL )
		*m_piDataLen = 0;

	m_iBuffLen = 0;

	ClassBuff.clear();
	for(int i=0;i<vValue.Size();++i)
	{
		Append( vValue[i] );
	}

	return *this;
}

template< class ValType_t, typename ValTypeImpl_t >
const std::vector<ValType_t> * SKVector<ValType_t, ValTypeImpl_t> :: GetList() const
{
	return &ClassBuff;
}

template< class ValType_t, typename ValTypeImpl_t >
int SKVector<ValType_t, ValTypeImpl_t> :: Size()const
{
	return ClassBuff.size();
}

template< class ValType_t, typename ValTypeImpl_t >
int SKVector<ValType_t, ValTypeImpl_t> :: Recalloc()
{
	if( m_pBuff == NULL )
	{
		assert( m_pBuff != NULL );
		return -1;
	}

	if( m_isArray )
	{
		//is a array not recalloc
		assert( m_iArrayDataLen < m_iBuffLen );
		return -1;
	}

	if( m_iBuffLen == 0 )
		m_iBuffLen= 1;
	else
		m_iBuffLen <<= 1;

	ValTypeImpl_t * tmp = ( ValTypeImpl_t * )calloc( sizeof( ValTypeImpl_t ),  m_iBuffLen );
	memcpy( (char*)tmp, (char*)(*m_pBuff), (*m_piDataLen) * sizeof( ValTypeImpl_t ) );

	if( *m_pBuff != NULL )
		free( *m_pBuff );
	*m_pBuff = tmp;
	return 0;
}

template< class ValType_t, typename ValTypeImpl_t >
typename SKVector < ValType_t, ValTypeImpl_t > :: iterator & SKVector < ValType_t, ValTypeImpl_t > :: begin() 
{
	return ClassBuff.begin();
}

template< class ValType_t, typename ValTypeImpl_t >
typename SKVector < ValType_t, ValTypeImpl_t > :: iterator & SKVector < ValType_t, ValTypeImpl_t > :: end()
{
	return ClassBuff.end();
}

template< class ValType_t, typename ValTypeImpl_t >
typename SKVector < ValType_t, ValTypeImpl_t > :: const_iterator & SKVector < ValType_t, ValTypeImpl_t > :: begin()const
{
	return ClassBuff.begin();
}

template< class ValType_t, typename ValTypeImpl_t >
typename SKVector < ValType_t, ValTypeImpl_t > :: const_iterator & SKVector < ValType_t, ValTypeImpl_t > :: end()const
{
	return ClassBuff.end();
}

//************************************************************************************	

template < class ValType_t  >
SKVector4BaseType< ValType_t > :: SKVector4BaseType()
{
	m_pBuff = 0;
	m_iBuffLen = 0;
	m_piDataLen = 0;
	m_isArray = 0;
}

template < class ValType_t  >
SKVector4BaseType< ValType_t > :: ~SKVector4BaseType()
{
}	

template < class ValType_t  >
void SKVector4BaseType< ValType_t > :: ReSet()
{
	m_pBuff = 0;
	m_iBuffLen = 0;
	m_piDataLen = 0;
	m_isArray = 0;
	ClassBuff.clear();	
}	

template < class ValType_t  >
void SKVector4BaseType< ValType_t > :: Clear()
{
	if( m_piDataLen != NULL )
		*m_piDataLen = 0;

	if( !m_isArray )
	{
		m_iBuffLen = 0;
		ClassBuff.clear();	
	}
}	

template < class ValType_t  >
void SKVector4BaseType<ValType_t> :: Append( const ValType_t & Item )
{
	if( m_piDataLen != NULL && *m_piDataLen == m_iBuffLen )
	{
		int ret = Recalloc( );
		if( ret < 0 )
			return ;
	}

	ClassBuff.push_back( Item );

	if( m_pBuff != NULL && *m_pBuff != NULL )
	{
		memcpy( &( ( *m_pBuff )[ ( *m_piDataLen )++  ] ), &Item, sizeof( ValType_t ) );
	}
}

template < class ValType_t  >
void SKVector4BaseType<ValType_t> :: Set( ValType_t *ItemValue, const int &iSize )
{
	//Set For Array

	if( m_pBuff != NULL && *m_pBuff != NULL && *m_pBuff != ItemValue && !m_isArray )
	{
		free(*m_pBuff);
	}

	m_isArray = 1;
	m_ptArrayPtr = ItemValue;
	m_pBuff = &m_ptArrayPtr;

	m_piDataLen = &m_iArrayDataLen;
	m_iArrayDataLen = 0;
	m_iBuffLen = iSize;

	ClassBuff.clear();
	for( int i = 0; i < iSize; ++i )
	{
		ClassBuff.push_back( (*m_pBuff)[i] ); 
	}
}


template < class ValType_t  >
void SKVector4BaseType<ValType_t> :: Set( ValType_t **ItemValue, int *iSize )
{
	if( m_pBuff != NULL && *m_pBuff != NULL && m_pBuff != ItemValue && !m_isArray )
	{
		free(*m_pBuff);
		m_pBuff = NULL;
	}

	m_isArray = 0;
	m_pBuff = ItemValue;
	m_piDataLen = iSize;
	m_iBuffLen = *iSize;

	ClassBuff.clear();
	if( m_piDataLen != NULL )
	{
		for( int i = 0; i < *m_piDataLen; ++i )
		{
			ClassBuff.push_back( (*m_pBuff)[i] );
		}
	}
}

template < class ValType_t  >
void SKVector4BaseType<ValType_t> :: SetVal( const int &iIndex, const ValType_t &Item )
{
	if( !m_isArray )
		return;

	if( m_pBuff == NULL )
		return ;

	if( m_iBuffLen <= iIndex )
	{
		assert( m_iBuffLen > iIndex );
	}

	if( m_pBuff != NULL && *m_pBuff != NULL )
	{
		memcpy( &( ( *m_pBuff )[ iIndex  ] ), &Item, sizeof( ValType_t ) );
		ClassBuff[ iIndex ] = Item;
	}

}

template < class ValType_t  >
const ValType_t & SKVector4BaseType<ValType_t> :: operator [] ( const int iIndex )const
{
	if( m_pBuff != NULL && *m_pBuff != NULL )
	{
		return (*m_pBuff)[iIndex];
	}

	return ClassBuff[iIndex];
}


template < class ValType_t  >
ValType_t & SKVector4BaseType<ValType_t> :: operator [] ( const int iIndex )
{
	if( m_pBuff != NULL && *m_pBuff != NULL )
	{
		return (*m_pBuff)[iIndex];
	}
	return ClassBuff[iIndex];
}

template < class ValType_t  >
SKVector4BaseType<ValType_t> &SKVector4BaseType<ValType_t> :: operator = ( const SKVector4BaseType<ValType_t> & vValue )
{
	if( m_pBuff != NULL && *m_pBuff != NULL && !m_isArray )
	{
		free ( *m_pBuff );
	}

	if( m_piDataLen != NULL )
		*m_piDataLen = 0;

	m_iBuffLen = 0;

	ClassBuff.clear();
	for(int i=0;i<vValue.Size();++i)
	{
		Append( vValue[i] );
	}

	return *this;
}

template < class ValType_t  >
const std::vector<ValType_t> * SKVector4BaseType<ValType_t> :: GetList() const
{
	return &ClassBuff;
}

template < class ValType_t  >
int SKVector4BaseType<ValType_t> :: Size()const
{
	return ClassBuff.size();
}

template < class ValType_t  >
int SKVector4BaseType<ValType_t> :: Recalloc()
{
	if( m_pBuff == NULL )
	{
		assert( m_pBuff != NULL );
		return -1;
	}

	if( m_isArray )
	{
		//is a array not recalloc
		assert( m_iArrayDataLen < m_iBuffLen );
		return -1;
	}

	if( m_iBuffLen == 0 )
		m_iBuffLen= 1;
	else
		m_iBuffLen <<= 1;

	ValType_t * tmp = ( ValType_t * )calloc( sizeof( ValType_t ),  m_iBuffLen );
	memcpy( (char*)tmp, (char*)(*m_pBuff), (*m_piDataLen) * sizeof( ValType_t ) );

	if( *m_pBuff != NULL )
		free( *m_pBuff );
	*m_pBuff = tmp;
	return 0;
}

template < class ValType_t  >
typename SKVector4BaseType < ValType_t > :: iterator & SKVector4BaseType< ValType_t > :: begin() 
{
	return ClassBuff.begin();
}

template < class ValType_t  >
typename SKVector4BaseType < ValType_t > :: iterator & SKVector4BaseType< ValType_t > :: end()
{
	return ClassBuff.end();
}

template < class ValType_t  >
typename SKVector4BaseType < ValType_t > :: const_iterator & SKVector4BaseType< ValType_t > :: begin()const
{
	return ClassBuff.begin();
}

template < class ValType_t  >
typename SKVector4BaseType < ValType_t > :: const_iterator & SKVector4BaseType< ValType_t > :: end()const
{
	return ClassBuff.end();
}


