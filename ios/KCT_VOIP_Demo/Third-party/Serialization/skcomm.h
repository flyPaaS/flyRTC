#pragma once

#include "skbuiltintype.h"
#include "assert.h"

namespace Comm
{




template <class ObjectType>
class SKVecRef
{
	unsigned char  ** m_ppArray;
  	size_t *m_piArraySize;
	size_t m_iAllocSize;
public:
	SKVecRef():m_ppArray(NULL),m_piArraySize(NULL),m_iAllocSize(0)
	{
	}
	void push_back(const ObjectType &r)
	{
		 assert(m_ppArray!=NULL && m_piArraySize!=NULL);
		 ++(*m_piArraySize);
		 if(*m_piArraySize>m_iAllocSize || *m_piArraySize< m_iAllocSize/2)
		 {
			UpdateAllocSize();		
		 }
		 (*m_ppArray) = (unsigned char  *)realloc( *m_ppArray,m_iAllocSize * ObjectType::size() );
		 assert(*m_ppArray != NULL);
		 
		 memset((*m_ppArray)+ (*m_piArraySize - 1 )*ObjectType::size(), 0, ObjectType::size() ) ;
		 *(ObjectType *)((*m_ppArray)+ (*m_piArraySize - 1 )* ObjectType::size()) = r;
	}
	ObjectType &operator[](size_t i)
	{
		 assert(m_ppArray!=NULL && m_piArraySize!=NULL && i<*m_piArraySize);
		 return *(ObjectType*)(*m_ppArray + i*ObjectType::size());
	}
	size_t size()
	{
		 assert(m_piArraySize!=NULL);
		 return *m_piArraySize;
	}
	size_t resize(size_t i)
	{
		 assert(m_ppArray!=NULL && m_piArraySize!=NULL);
		 if(i==*m_piArraySize)
		 {
			 return i;
		 }
		 size_t iOldArraySize=*m_piArraySize;
		 for(size_t j=i; j< *m_piArraySize; j++)
		 {
			 ((ObjectType*)(*m_ppArray+j*ObjectType::size()))->Free();
		 }
		 *m_piArraySize = i;
		 *m_ppArray = (unsigned char *)realloc(*m_ppArray,*m_piArraySize * ObjectType::size());
		 if(*m_piArraySize!=0 && *m_ppArray ==NULL)
		 {
			 assert(false);
		 }
		 m_iAllocSize=*m_piArraySize;
		 if(i>iOldArraySize)
		 {
			 memset((*m_ppArray)+ iOldArraySize*ObjectType::size(), 0, (i-iOldArraySize)*ObjectType::size()) ;
		 }
		 return i;
	 }
	 bool erase(size_t index)
	 {
		 if(m_ppArray==NULL || m_piArraySize==NULL || index >=*m_piArraySize)
		 {
			 return false;
		 }
		 ((ObjectType*)(*m_ppArray+index*ObjectType::size()))->Free();
		 if(*m_piArraySize==1)
		 {
			 free(*m_ppArray);
			 *m_ppArray=NULL;
			 *m_piArraySize=0;
			 return true;
		 }
		 unsigned char *tmpPtArray;
		 tmpPtArray = (unsigned char *)malloc((*m_piArraySize-1)* ObjectType::size() );
		 assert(tmpPtArray!=NULL);
		 m_iAllocSize=*m_piArraySize-1;
		 size_t iCopyLen=index* ObjectType::size();
		 memcpy(tmpPtArray, *m_ppArray, iCopyLen);
		 iCopyLen=((*m_piArraySize)-index-1)* ObjectType::size();
		 memcpy(tmpPtArray+index*ObjectType::size(), (*m_ppArray)+(index+1)*ObjectType::size(), iCopyLen);
		 free(*m_ppArray);
		 *m_ppArray=tmpPtArray;
		 (*m_piArraySize)--;
		 return true;
	 }
	 void Set(unsigned char **appArray,size_t *apiArraySize)
	 {
		 assert(appArray!=NULL && apiArraySize!=NULL);
		 m_ppArray = appArray;
		 m_piArraySize = apiArraySize;
		 m_iAllocSize=*m_piArraySize; 
	 }

private:
	 SKVecRef(const SKVecRef<ObjectType> & other)
	 {
	 }
	 SKVecRef & operator =(const SKVecRef<ObjectType> & other)
	 {
	 }
	 void UpdateAllocSize()
	 {
		unsigned char index=0;
		size_t iArraySize=*m_piArraySize;
		while(iArraySize !=0)
		{
			index++;	
			iArraySize=iArraySize>>1;
		}
		m_iAllocSize=(size_t)(1LL<<index);
	 }
};

template<class SimpleType>
class SKSimpleTypeVectorRef
{
	SimpleType ** m_ppArray;
  	size_t *m_piArraySize;
	size_t m_iAllocSize;
public:
	SKSimpleTypeVectorRef():m_ppArray(NULL),m_piArraySize(NULL),m_iAllocSize(0)
	{
	}
	void push_back(const SimpleType &r)
	{
		 assert(m_ppArray!=NULL && m_piArraySize!=NULL);
		 ++(*m_piArraySize);	
		 if(*m_piArraySize>m_iAllocSize || *m_piArraySize< m_iAllocSize/2)
		 {
			UpdateAllocSize();		
		 }
		 (*m_ppArray) = (SimpleType *)realloc( *m_ppArray,m_iAllocSize *sizeof(SimpleType) );
		 assert(*m_ppArray != NULL);
		 
		 *(SimpleType *)((*m_ppArray)+ *m_piArraySize - 1 ) = r;
	}
	SimpleType &operator[](size_t i)
	{
		 assert(m_ppArray!=NULL && m_piArraySize!=NULL && i<*m_piArraySize);
		 return *(SimpleType*)(*m_ppArray + i);
	}
	size_t size()
	{
		 assert(m_piArraySize!=NULL);
		 return *m_piArraySize;
	}
	size_t resize(size_t i)
	{
		 assert(m_ppArray!=NULL && m_piArraySize!=NULL);
		 if(i==*m_piArraySize)
		 {
			 return i;
		 }
		 size_t iOldArraySize=*m_piArraySize;
		 *m_piArraySize = i;
		 *m_ppArray = (SimpleType *)realloc(*m_ppArray,*m_piArraySize * sizeof(SimpleType));
		 if(*m_piArraySize!=0 && *m_ppArray ==NULL)
		 {
			 assert(false);
		 }
		 m_iAllocSize=*m_piArraySize;
		 if(i>iOldArraySize)
		 {
			 memset((*m_ppArray)+ iOldArraySize, 0, (i-iOldArraySize)*sizeof(SimpleType)) ;
		 }
		 return i;
	 }
	 bool erase(size_t index)
	 {
		 if(m_ppArray==NULL || m_piArraySize==NULL || index >=*m_piArraySize)
		 {
			 return false;
		 }
		 if(*m_piArraySize==1)
		 {
			 free(*m_ppArray);
			 *m_ppArray=NULL;
			 *m_piArraySize=0;
			 return true;
		 }
		 SimpleType* tmpPtArray;
		 tmpPtArray = (SimpleType*)malloc((*m_piArraySize-1)*sizeof(SimpleType) );
		 assert(tmpPtArray!=NULL);
		 m_iAllocSize=*m_piArraySize-1;
		 size_t iCopyLen=index*sizeof(SimpleType); 
		 memcpy(tmpPtArray, *m_ppArray, iCopyLen);
		 iCopyLen=((*m_piArraySize)-index-1)* sizeof(SimpleType);
		 memcpy(tmpPtArray+index, (*m_ppArray)+index+1, iCopyLen);
		 free(*m_ppArray);
		 *m_ppArray=tmpPtArray;
		 (*m_piArraySize)--;
		 return true;
	 }
	void Set(unsigned char **appArray,size_t *apiArraySize)
	{
		 assert(appArray!=NULL && apiArraySize!=NULL);
		 m_ppArray =(SimpleType**) appArray;
		 m_piArraySize = apiArraySize;
		 m_iAllocSize=*m_piArraySize; 
	}

private:
	SKSimpleTypeVectorRef(const SKSimpleTypeVectorRef<SimpleType> & other)
	{
	}
	SKSimpleTypeVectorRef & operator =(const SKSimpleTypeVectorRef<SimpleType> & other)
	{
	}
	void UpdateAllocSize()
	{
		unsigned char index=0;
		size_t iArraySize=*m_piArraySize;
		while(iArraySize !=0)
		{
			index++;	
			iArraySize=iArraySize>>1;
		}
		m_iAllocSize=(size_t)(1LL<<index);
	}
};

template <class ObjectType>
class SKVec
{
	size_t m_iAllocSize;
	unsigned char *m_pArray;
	size_t m_iArraySize;
public:
	SKVec():m_iAllocSize(0),m_pArray(NULL),m_iArraySize(0)
	{
	}
	~SKVec()
	{
		Free();
	}
	SKVec(const SKVec<ObjectType> & other)
	{
		m_pArray=NULL;
		m_iAllocSize=0;
		m_iArraySize=0;
		operator=(other);
	}
	SKVec & operator =(const SKVec<ObjectType> & other)
	{
		if(this == &other)
		{
			return *this;
		}
		Free();
		m_iArraySize=other.m_iArraySize;
		m_iAllocSize=other.m_iAllocSize;
		m_pArray=(unsigned char *)calloc(m_iAllocSize*ObjectType::size(), 1);
		assert(m_pArray!=NULL);
		for(size_t i=0; i<m_iArraySize; i++)
		{
			*(ObjectType*)(m_pArray+i*ObjectType::size())=*(ObjectType*)(other.m_pArray+i*ObjectType::size());//  deep copy
		}
		return *this;
	}
	void push_back(const ObjectType &r)
	{
		 ++m_iArraySize;	
		 if(m_iArraySize>m_iAllocSize || m_iArraySize< m_iAllocSize/2)
		 {
			UpdateAllocSize();		
		 }
		 m_pArray = (unsigned char  *)realloc( m_pArray,m_iAllocSize * ObjectType::size() );
		 assert(m_pArray != NULL);
		 
		 memset(m_pArray+ (m_iArraySize - 1 )*ObjectType::size(), 0, ObjectType::size() ) ;
		 *(ObjectType *)(m_pArray+ (m_iArraySize - 1 )* ObjectType::size()) = r;
	}
	ObjectType &operator[](size_t i)
	{
		 assert(m_pArray!=NULL && i<m_iArraySize);
		 return *(ObjectType*)(m_pArray + i*ObjectType::size());
	}
	size_t size()
	{
		 return m_iArraySize;
	}
	size_t resize(size_t i)
	{
		 if(i==m_iArraySize)
		 {
			 return i;
		 }
		 size_t iOldArraySize=m_iArraySize;
		 for(size_t j=i; j< m_iArraySize; j++)
		 {
			 ((ObjectType*)(m_pArray+j*ObjectType::size()))->Free();
		 }
		 m_iArraySize = i;
		 m_pArray = (unsigned char *)realloc(m_pArray,m_iArraySize * ObjectType::size());
		 if(m_iArraySize!=0 && m_pArray ==NULL)
		 {
			 assert(false);
		 }
		 m_iAllocSize=m_iArraySize;
		 if(i>iOldArraySize)
		 {
			 memset(m_pArray+ iOldArraySize*ObjectType::size(), 0, (i-iOldArraySize)*ObjectType::size()) ;
		 }
		 return i;
	 }
	 bool erase(size_t index)
	 {
		 if(m_pArray==NULL || index>=m_iArraySize)
		 {
			 return false;
		 }
		 ((ObjectType*)(m_pArray+index*ObjectType::size()))->Free();
		 if(m_iArraySize==1)
		 {
			 Free();
			 return true;
		 }
		 unsigned char *tmpPtArray;
		 tmpPtArray = (unsigned char *)malloc((m_iArraySize-1)* ObjectType::size() );
		 assert(tmpPtArray!=NULL);
		 m_iAllocSize=m_iArraySize-1;
		 size_t iCopyLen=index* ObjectType::size();
		 memcpy(tmpPtArray, m_pArray, iCopyLen);
		 iCopyLen=(m_iArraySize-index-1)* ObjectType::size();
		 memcpy(tmpPtArray+index*ObjectType::size(), m_pArray+(index+1)*ObjectType::size(), iCopyLen);
		 free(m_pArray);
		 m_pArray=tmpPtArray;
		 m_iArraySize--;
		 return true;
	 }
	 void CopyToArray(unsigned char **appArray,size_t *apiArraySize) const
	 {
		 assert(appArray!=NULL && apiArraySize !=NULL);
		 for(size_t j=0; j< *apiArraySize; j++)
		 {
			 ((ObjectType*)(*appArray+j*ObjectType::size()))->Free();
		 }
		 if(*appArray!=NULL)free(*appArray), *appArray=NULL;

		 if(m_pArray==NULL || m_iArraySize==0)
		 {
			 *apiArraySize=0;
			 return;
		 }
		 *appArray=(unsigned char *)calloc(m_iArraySize*ObjectType::size(), 1);
		 assert(*appArray!=NULL);
		 for(size_t i=0; i<m_iArraySize; i++)
		 {
			 *(ObjectType*)(*appArray+i*ObjectType::size())=*(ObjectType*)(m_pArray+i*ObjectType::size());//  deep copy
		 }
		 *apiArraySize=m_iArraySize;
	 }

private:
	 void Free()
	 {
		 if(m_pArray!=NULL)
		 {
			 for(size_t j=0; j< m_iArraySize; j++)
			 {
				 ((ObjectType*)(m_pArray+j*ObjectType::size()))->Free();
			 }
			 free(m_pArray);
		 }
		 m_pArray=NULL;
		 m_iAllocSize=0;
		 m_iArraySize=0;
	}
	void UpdateAllocSize()
	{
		unsigned char index=0;
		size_t iArraySize=m_iArraySize;
		while(iArraySize !=0)
		{
			index++;	
			iArraySize=iArraySize>>1;
		}
		m_iAllocSize=(size_t)(1LL<<index);
	}
};

template<class SimpleType>
class SKSimpleTypeVector
{
	size_t m_iAllocSize;
	SimpleType *m_pArray;
	size_t m_iArraySize;
public:
	SKSimpleTypeVector():m_iAllocSize(0),m_pArray(NULL),m_iArraySize(0)
	{
	}
	~SKSimpleTypeVector()
	{
		Free();
	}
	SKSimpleTypeVector(const SKSimpleTypeVector<SimpleType> & other)
	{
		m_pArray=NULL;
		m_iAllocSize=0;
		m_iArraySize=0;
		operator=(other);
	}
	SKSimpleTypeVector & operator =(const SKSimpleTypeVector<SimpleType> & other)
	{
		if(this == &other)
		{
			return *this;
		}
		Free();
		m_iArraySize=other.m_iArraySize;
		m_iAllocSize=other.m_iAllocSize;
		m_pArray=(SimpleType *)calloc(m_iAllocSize*sizeof(SimpleType), 1);
		assert(m_pArray!=NULL);
		memcpy(m_pArray, other.m_pArray, m_iArraySize*sizeof(SimpleType));
		return *this;
	}
	void push_back(const SimpleType &r)
	{
		 ++m_iArraySize;	
		 if(m_iArraySize>m_iAllocSize || m_iArraySize< m_iAllocSize/2)
		 {
			UpdateAllocSize();		
		 }
		 m_pArray = (SimpleType *)realloc( m_pArray,m_iAllocSize *sizeof(SimpleType) );
		 assert(m_pArray != NULL);
		 
		 *(SimpleType *)(m_pArray+ m_iArraySize - 1 ) = r;
	}
	SimpleType &operator[](size_t i)
	{
		 assert(m_pArray!=NULL && i<m_iArraySize);
		 return *(SimpleType*)(m_pArray + i);
	}
	size_t size()
	{
		 return m_iArraySize;
	}
	size_t resize(size_t i)
	{
		 if(i==m_iArraySize)
		 {
			 return i;
		 }
		 size_t iOldArraySize=m_iArraySize;
		 m_iArraySize = i;
		 m_pArray = (SimpleType *)realloc(m_pArray,m_iArraySize * sizeof(SimpleType));
		 if(m_iArraySize!=0 && m_pArray ==NULL)
		 {
			 assert(false);
		 }
		 m_iAllocSize=m_iArraySize;
		 if(i>iOldArraySize)
		 {
			 memset(m_pArray+ iOldArraySize, 0, (i-iOldArraySize)*sizeof(SimpleType)) ;
		 }
		 return i;
	 }
	 bool erase(size_t index)
	 {
		 if(m_pArray==NULL || index >=m_iArraySize)
		 {
			 return false;
		 }
		 if(m_iArraySize==1)
		 {
			 Free();
			 return true;
		 }
		 SimpleType* tmpPtArray;
		 tmpPtArray = (SimpleType*)malloc((m_iArraySize-1)*sizeof(SimpleType) );
		 assert(tmpPtArray!=NULL);
		 m_iAllocSize=m_iArraySize-1;
		 size_t iCopyLen=index*sizeof(SimpleType); 
		 memcpy(tmpPtArray, m_pArray, iCopyLen);
		 iCopyLen=(m_iArraySize-index-1)* sizeof(SimpleType);
		 memcpy(tmpPtArray+index, m_pArray+index+1, iCopyLen);
		 free(m_pArray);
		 m_pArray=tmpPtArray;
		 m_iArraySize--;
		 return true;
	 }
	 void CopyToArray(unsigned char **appArray,size_t *apiArraySize) const
	 {
		 assert(appArray!=NULL && apiArraySize !=NULL);
		 if(*appArray!=NULL)free(*appArray), *appArray=NULL;

		 if(m_pArray==NULL || m_iArraySize==0)
		 {
			 *apiArraySize=0;
			 return;
		 }
		 *appArray=(unsigned char *)malloc(m_iArraySize*sizeof(SimpleType) );
		 assert(*appArray!=NULL);
		 memcpy(*appArray, m_pArray, m_iArraySize*sizeof(SimpleType));
		 *apiArraySize=m_iArraySize;
	 }
private:

	void Free()
	{
		if(m_pArray!=NULL)
		{
			free(m_pArray);
		}
		m_pArray=NULL;
		m_iAllocSize=0;
		m_iArraySize=0;
	}
	void UpdateAllocSize()
	{
		unsigned char index=0;
		size_t iArraySize=m_iArraySize;
		while(iArraySize !=0)
		{
			index++;	
			iArraySize=iArraySize>>1;
		}
		m_iAllocSize=(size_t)(1LL<<index);
	}
};





template <class T>
class SKObject
{
	T *m_pObj;
public:
	SKObject()
	{
		 m_pObj = T::Alloc();
	}
 
	SKObject(const SKObject<T> & other)
	{
		 m_pObj = T::Alloc();
		 *m_pObj = other;
	}
	
	SKObject<T> & operator =(const SKObject<T> & other)
	{
		 if(this == &other)
		 {
			 return *this;
		 }
		 *m_pObj = other;
		 return *this;
	}
	SKObject<T> & operator =(const T & other)
	{
		 if(m_pObj == &other)
		 {
			 return *this;
		 }
		 *m_pObj = other;
		 return *this;
	}
	~SKObject()
	{
		 delete m_pObj;
	}
	T *operator->()
	{
		 return m_pObj;
	}
	const T *operator->() const
	{
		 return m_pObj;
	}
	operator T&()
	{
		 return *m_pObj;
	}
	operator const T&() const
	{
		 return *m_pObj;
	}
};


template <> class SKVecRef<long long>:public SKSimpleTypeVectorRef<long long> {};
template <> class SKVecRef<unsigned long long>:public SKSimpleTypeVectorRef<unsigned long long> {};
template <> class SKVecRef<int>:public SKSimpleTypeVectorRef<int> {};
template <> class SKVecRef<unsigned int>:public SKSimpleTypeVectorRef<unsigned int> {};
template <> class SKVecRef<short>:public SKSimpleTypeVectorRef<short> {};
template <> class SKVecRef<unsigned short>:public SKSimpleTypeVectorRef<unsigned short> {};
template <> class SKVecRef<char>:public SKSimpleTypeVectorRef<char> {};
template <> class SKVecRef<unsigned char>:public SKSimpleTypeVectorRef<unsigned char> {};
template <> class SKVecRef<float>:public SKSimpleTypeVectorRef<float> {};
template <> class SKVecRef<double>:public SKSimpleTypeVectorRef<double> {};

template <> class SKVecRef<SKBuiltinInt32_t>:public SKSimpleTypeVectorRef<SKBuiltinInt32_t> {};
template <> class SKVecRef<SKBuiltinUint32_t>:public SKSimpleTypeVectorRef<SKBuiltinUint32_t> {};
template <> class SKVecRef<SKBuiltinChar_t>:public SKSimpleTypeVectorRef<SKBuiltinChar_t> {};
template <> class SKVecRef<SKBuiltinUchar_t>:public SKSimpleTypeVectorRef<SKBuiltinUchar_t> {};
template <> class SKVecRef<SKBuiltinInt8_t>:public SKSimpleTypeVectorRef<SKBuiltinInt8_t> {};
template <> class SKVecRef<SKBuiltinUint8_t>:public SKSimpleTypeVectorRef<SKBuiltinUint8_t> {};
template <> class SKVecRef<SKBuiltinInt16_t>:public SKSimpleTypeVectorRef<SKBuiltinInt16_t> {};
template <> class SKVecRef<SKBuiltinUint16_t>:public SKSimpleTypeVectorRef<SKBuiltinUint16_t> {};
template <> class SKVecRef<SKBuiltinInt64_t>:public SKSimpleTypeVectorRef<SKBuiltinInt64_t> {};
template <> class SKVecRef<SKBuiltinUint64_t>:public SKSimpleTypeVectorRef<SKBuiltinUint64_t> {};
template <> class SKVecRef<SKBuiltinFloat32_t>:public SKSimpleTypeVectorRef<SKBuiltinFloat32_t> {};
template <> class SKVecRef<SKBuiltinDouble64_t>:public SKSimpleTypeVectorRef<SKBuiltinDouble64_t> {};


template <> class SKVec<long long>:public SKSimpleTypeVector<long long> {};
template <> class SKVec<unsigned long long>:public SKSimpleTypeVector<unsigned long long> {};
template <> class SKVec<int>:public SKSimpleTypeVector<int> {};
template <> class SKVec<unsigned int>:public SKSimpleTypeVector<unsigned int> {};
template <> class SKVec<short>:public SKSimpleTypeVector<short> {};
template <> class SKVec<unsigned short>:public SKSimpleTypeVector<unsigned short> {};
template <> class SKVec<char>:public SKSimpleTypeVector<char> {};
template <> class SKVec<unsigned char>:public SKSimpleTypeVector<unsigned char> {};
template <> class SKVec<float>:public SKSimpleTypeVector<float> {};
template <> class SKVec<double>:public SKSimpleTypeVector<double> {};

template <> class SKVec<SKBuiltinInt32_t>:public SKSimpleTypeVector<SKBuiltinInt32_t> {};
template <> class SKVec<SKBuiltinUint32_t>:public SKSimpleTypeVector<SKBuiltinUint32_t> {};
template <> class SKVec<SKBuiltinChar_t>:public SKSimpleTypeVector<SKBuiltinChar_t> {};
template <> class SKVec<SKBuiltinUchar_t>:public SKSimpleTypeVector<SKBuiltinUchar_t> {};
template <> class SKVec<SKBuiltinInt8_t>:public SKSimpleTypeVector<SKBuiltinInt8_t> {};
template <> class SKVec<SKBuiltinUint8_t>:public SKSimpleTypeVector<SKBuiltinUint8_t> {};
template <> class SKVec<SKBuiltinInt16_t>:public SKSimpleTypeVector<SKBuiltinInt16_t> {};
template <> class SKVec<SKBuiltinUint16_t>:public SKSimpleTypeVector<SKBuiltinUint16_t> {};
template <> class SKVec<SKBuiltinInt64_t>:public SKSimpleTypeVector<SKBuiltinInt64_t> {};
template <> class SKVec<SKBuiltinUint64_t>:public SKSimpleTypeVector<SKBuiltinUint64_t> {};
template <> class SKVec<SKBuiltinFloat32_t>:public SKSimpleTypeVector<SKBuiltinFloat32_t> {};
template <> class SKVec<SKBuiltinDouble64_t>:public SKSimpleTypeVector<SKBuiltinDouble64_t> {};
}
